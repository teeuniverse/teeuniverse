/*
 * Copyright (C) 2016 necropotame (necropotame@gmail.com)
 * 
 * This file is part of TeeUniverse.
 * 
 * TeeUniverse is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * TeeUniverse is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Some parts of this file comes from other projects.
 * These parts are itendified in this file by the following block:
 * 
 * FOREIGN CODE BEGIN: ProjectName *************************************
 * 
 * FOREIGN CODE END: ProjectName ***************************************
 * 
 * If ProjectName is "TeeWorlds", then this part of the code follows the
 * TeeWorlds licence:
 *      (c) Magnus Auvinen. See LICENSE_TEEWORLDS in the root of the
 *      distribution for more information. If you are missing that file,
 *      acquire a complete release at teeworlds.com.
 */

#ifndef __CLIENT_GRAPHICS_BACKEND_SDL__
#define __CLIENT_GRAPHICS_BACKEND_SDL__

#include "graphics.h"

#include <shared/tl/threading.h>

#include <SDL.h>
#include <SDL_opengl.h>
#if defined(CONF_PLATFORM_MACOSX)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

#if defined(CONF_PLATFORM_MACOSX)
	#include <objc/objc-runtime.h>

	class semaphore
	{
		SDL_sem *sem;
	public:
		semaphore() { sem = SDL_CreateSemaphore(0); }
		~semaphore() { SDL_DestroySemaphore(sem); }
		void wait() { SDL_SemWait(sem); }
		void signal() { SDL_SemPost(sem); }
	};

	class CAutoreleasePool
	{
	private:
		id m_Pool;

	public:
		CAutoreleasePool()
		{
			Class NSAutoreleasePoolClass = (Class) objc_getClass("NSAutoreleasePool");
			m_Pool = class_createInstance(NSAutoreleasePoolClass, 0);
			SEL selector = sel_registerName("init");
			((id(*)(id, SEL))objc_msgSend)(m_Pool, selector);
		}

		~CAutoreleasePool()
		{
			SEL selector = sel_registerName("drain");
			((id(*)(id, SEL))objc_msgSend)(m_Pool, selector);
		}
	};
#endif

enum EGraphicsBackendErrorCodes
{
	GRAPHICS_BACKEND_ERROR_CODE_UNKNOWN = -1,
	GRAPHICS_BACKEND_ERROR_CODE_NONE = 0,
	GRAPHICS_BACKEND_ERROR_CODE_OPENGL_CONTEXT_FAILED,
	GRAPHICS_BACKEND_ERROR_CODE_OPENGL_VERSION_FAILED,
	GRAPHICS_BACKEND_ERROR_CODE_SDL_INIT_FAILED,
	GRAPHICS_BACKEND_ERROR_CODE_SDL_SCREEN_REQUEST_FAILED,
	GRAPHICS_BACKEND_ERROR_CODE_SDL_SCREEN_INFO_REQUEST_FAILED,
	GRAPHICS_BACKEND_ERROR_CODE_SDL_SCREEN_RESOLUTION_REQUEST_FAILED,
	GRAPHICS_BACKEND_ERROR_CODE_SDL_WINDOW_CREATE_FAILED,
};

// interface for the graphics backend
// all these functions are called on the main thread
class IGraphicsBackend
{
public:
	enum
	{
		INITFLAG_FULLSCREEN = 1 << 0,
		INITFLAG_VSYNC = 1 << 1,
		INITFLAG_RESIZABLE = 1 << 2,
		INITFLAG_BORDERLESS = 1 << 3,
	};
	
	virtual ~IGraphicsBackend() {}

	virtual int Init(const char *pName, int *pScreen, int *pWidth, int *pHeight, int FsaaSamples, int Flags, int *pDesktopWidth, int *pDesktopHeight) = 0;
	virtual int Shutdown() = 0;

	virtual int MemoryUsage() const = 0;

	virtual int GetNumScreens() const = 0;
	
	virtual void Minimize() = 0;
	virtual void Maximize() = 0;
	virtual bool Fullscreen(bool State) = 0;
	virtual void SetWindowBordered(bool State) = 0;
	virtual bool SetWindowScreen(int Index) = 0;
	virtual int GetWindowScreen() = 0;
	virtual int WindowActive() = 0;
	virtual int WindowOpen() = 0;
	
	virtual void RunBuffer(CCommandBuffer *pBuffer) = 0;
	virtual bool IsIdle() const = 0;
	virtual void WaitForIdle() = 0;
};

class CGraphicsBackend_SDL : public IGraphicsBackend
{
public:
	class ICommandProcessor
	{
	public:
		virtual ~ICommandProcessor() {}
		virtual void RunBuffer(CCommandBuffer *pBuffer) = 0;
	};

protected:
	ICommandProcessor *m_pProcessor;
	CCommandBuffer * volatile m_pBuffer;
	volatile bool m_Shutdown;
	semaphore m_Activity;
	semaphore m_BufferDone;
	void *m_pThread;

	static void ThreadFunc(void *pUser);
	
	SDL_Window *m_pWindow;
	SDL_GLContext m_GLContext;
	volatile int m_TextureMemoryUsage;
	int m_NumScreens;
	int m_DesktopWidth;
	int m_DesktopHeight;
	
protected:
	void StartProcessor(ICommandProcessor *pProcessor);
	void StopProcessor();
	
public:
	CGraphicsBackend_SDL();
	virtual ~CGraphicsBackend_SDL() {}
	
	int Init(const char *pName, int *pScreen, int *pWidth, int *pHeight, int FsaaSamples, int Flags, int *pDesktopWidth, int *pDesktopHeight) override;
	int Shutdown() override;

	int MemoryUsage() const override;

	int GetNumScreens() const override { return m_NumScreens; }

	void Minimize() override;
	void Maximize() override;
	bool Fullscreen(bool State) override;
	void SetWindowBordered(bool State) override;
	bool SetWindowScreen(int Index) override;
	int GetWindowScreen() override;
	int WindowActive() override;
	int WindowOpen() override;
	
	void RunBuffer(CCommandBuffer *pBuffer) override;
	bool IsIdle() const override;
	void WaitForIdle() override;
};

// takes care of implementation independent operations
class CCommandProcessorFragment_General
{
	void Cmd_Nop();
	void Cmd_Signal(const CCommandBuffer::SCommand_Signal *pCommand);
public:
	bool RunCommand(const CCommandBuffer::SCommand * pBaseCommand);
};

// takes care of opengl related rendering
class CCommandProcessorFragment_OpenGL
{
	class CTexture
	{
	public:
		enum
		{
			STATE_EMPTY = 0,
			STATE_TEX2D = 1,
			STATE_TEX3D = 2,
		};
		GLuint m_Tex2D;
		GLuint m_Tex3D;
		int m_State;
		int m_Format;
		int m_MemSize;
	};
	CTexture m_aTextures[CCommandBuffer::MAX_TEXTURES];
	volatile int *m_pTextureMemoryUsage;

public:
	enum
	{
		CMD_INIT = CCommandBuffer::CMDGROUP_PLATFORM_OPENGL,
	};

	struct SCommand_Init : public CCommandBuffer::SCommand
	{
		SCommand_Init() : SCommand(CMD_INIT) {}
		volatile int *m_pTextureMemoryUsage;
	};

private:
	static int TexFormatToOpenGLFormat(int TexFormat);
	static unsigned char Sample(int w, int h, const unsigned char *pData, int u, int v, int Offset, int ScaleW, int ScaleH, int Bpp);
	static unsigned char* Rescale(int Width, int Height, int NewWidth, int NewHeight, int Format, const unsigned char* pData);

	void SetState(const CCommandBuffer::SState &State);

	void Cmd_Init(const SCommand_Init *pCommand);
	void Cmd_Texture_Update(const CCommandBuffer::SCommand_Texture_Update *pCommand);
	void Cmd_Texture_Destroy(const CCommandBuffer::SCommand_Texture_Destroy *pCommand);
	void Cmd_Texture_Create(const CCommandBuffer::SCommand_Texture_Create *pCommand);
	void Cmd_Clear(const CCommandBuffer::SCommand_Clear *pCommand);
	void Cmd_SetSize(const CCommandBuffer::SCommand_SetSize *pCommand);
	void Cmd_Render(const CCommandBuffer::SCommand_Render *pCommand);
	void Cmd_Screenshot(const CCommandBuffer::SCommand_Screenshot *pCommand);

public:
	CCommandProcessorFragment_OpenGL();

	bool RunCommand(const CCommandBuffer::SCommand * pBaseCommand);
};

// takes care of sdl related commands
class CCommandProcessorFragment_SDL
{
	// SDL stuff
	SDL_Window *m_pWindow;
	SDL_GLContext m_GLContext;
public:
	enum
	{
		CMD_INIT = CCommandBuffer::CMDGROUP_PLATFORM_SDL,
		CMD_SHUTDOWN,
	};

	struct SCommand_Init : public CCommandBuffer::SCommand
	{
		SCommand_Init() : SCommand(CMD_INIT) {}
		SDL_Window *m_pWindow;
		SDL_GLContext m_GLContext;
	};

	struct SCommand_Shutdown : public CCommandBuffer::SCommand
	{
		SCommand_Shutdown() : SCommand(CMD_SHUTDOWN) {}
	};

private:
	void Cmd_Init(const SCommand_Init *pCommand);
	void Cmd_Shutdown(const SCommand_Shutdown *pCommand);
	void Cmd_Swap(const CCommandBuffer::SCommand_Swap *pCommand);
	void Cmd_VSync(const CCommandBuffer::SCommand_VSync *pCommand);
	void Cmd_VideoModes(const CCommandBuffer::SCommand_VideoModes *pCommand);
public:
	CCommandProcessorFragment_SDL();

	bool RunCommand(const CCommandBuffer::SCommand *pBaseCommand);
};

// command processor impelementation, uses the fragments to combine into one processor
class CCommandProcessor_SDL_OpenGL : public CGraphicsBackend_SDL::ICommandProcessor
{
 	CCommandProcessorFragment_OpenGL m_OpenGL;
 	CCommandProcessorFragment_SDL m_SDL;
 	CCommandProcessorFragment_General m_General;
 public:
	virtual void RunBuffer(CCommandBuffer *pBuffer);
};

/* FOREIGN CODE END: TeeWorlds ****************************************/

#endif
