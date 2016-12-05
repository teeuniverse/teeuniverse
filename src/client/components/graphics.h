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

#ifndef __CLIENT_GRAPHICS__
#define __CLIENT_GRAPHICS__

#include <shared/system/time.h>
#include <shared/tl/array.h>
#include <shared/math/vector.h>
#include <shared/graphics.h>
#include <client/kernel.h>
#include <external/pnglite/pnglite.h>

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

/*
	Structure: CVideoMode
*/
class CVideoMode
{
public:
	int m_Width, m_Height;
	int m_Red, m_Green, m_Blue;

	bool operator<(const CVideoMode &Other) { return Other.m_Width < m_Width; }
};

class CCommandBuffer
{
	class CBuffer
	{
		unsigned char *m_pData;
		unsigned m_Size;
		unsigned m_Used;
	public:
		CBuffer(unsigned BufferSize)
		{
			m_Size = BufferSize;
			m_pData = new unsigned char[m_Size];
			m_Used = 0;
		}

		~CBuffer()
		{
			delete [] m_pData;
			m_pData = 0x0;
			m_Used = 0;
			m_Size = 0;
		}

		void Reset()
		{
			m_Used = 0;
		}

		void *Alloc(unsigned Requested)
		{
			if(Requested + m_Used > m_Size)
				return 0;
			void *pPtr = &m_pData[m_Used];
			m_Used += Requested;
			return pPtr;
		}

		unsigned char *DataPtr() { return m_pData; }
		unsigned DataSize() const { return m_Size; }
		unsigned DataUsed() const { return m_Used; }
	};

public:
	CBuffer m_CmdBuffer;
	CBuffer m_DataBuffer;

	enum
	{
		MAX_TEXTURES=1024*4,
	};

	enum
	{
		// command groups
		CMDGROUP_CORE = 0, // commands that everyone has to implement
		CMDGROUP_PLATFORM_OPENGL = 10000, // commands specific to a platform
		CMDGROUP_PLATFORM_SDL = 20000,

		//
		CMD_NOP = CMDGROUP_CORE,

		//
		CMD_RUNBUFFER,

		// syncronization
		CMD_SIGNAL,

		// texture commands
		CMD_TEXTURE_CREATE,
		CMD_TEXTURE_DESTROY,
		CMD_TEXTURE_UPDATE,

		// rendering
		CMD_CLEAR,
		CMD_RENDER,

		// swap
		CMD_SWAP,

		// misc
		CMD_VSYNC,
		CMD_SCREENSHOT,
		CMD_VIDEOMODES,
		CMD_SETSIZE,

	};

	enum
	{
		TEXFORMAT_INVALID = 0,
		TEXFORMAT_RGB,
		TEXFORMAT_RGBA,
		TEXFORMAT_ALPHA,

		TEXFLAG_NOMIPMAPS = 1,
		TEXFLAG_COMPRESSED = 2,
		TEXFLAG_QUALITY = 4,
		TEXFLAG_TEXTURE3D = 8,
		TEXFLAG_TEXTURE2D = 16,
	};

	enum
	{
		//
		PRIMTYPE_INVALID = 0,
		PRIMTYPE_LINES,	
		PRIMTYPE_QUADS,
	};

	enum
	{
		BLEND_NONE = 0,
		BLEND_ALPHA,
		BLEND_ADDITIVE,
	};

	struct SPoint { float x, y, z; };
	struct STexCoord { float u, v, i; };
	struct SColor { float r, g, b, a; };

	struct SVertex
	{
		SPoint m_Pos;
		STexCoord m_Tex;
		SColor m_Color;
	};

	struct SCommand
	{
	public:
		SCommand(unsigned Cmd) : m_Cmd(Cmd), m_Size(0) {}
		unsigned m_Cmd;
		unsigned m_Size;
	};
	
	struct SClip
	{
		int x;
		int y;
		int w;
		int h;
	};

	struct SState
	{
		int m_BlendMode;
		int m_WrapModeU;
		int m_WrapModeV;
		int m_Texture;
		int m_Dimension;
		SPoint m_ScreenTL;
		SPoint m_ScreenBR;

		// clip
		bool m_ClipEnable;
		int m_ClipX;
		int m_ClipY;
		int m_ClipW;
		int m_ClipH;
	};
		
	struct SCommand_SetSize : public SCommand
	{
		SCommand_SetSize() : SCommand(CMD_SETSIZE) {}
		int m_Width;
		int m_Height;
	};
		
	struct SCommand_Clear : public SCommand
	{
		SCommand_Clear() : SCommand(CMD_CLEAR) {}
		SColor m_Color;
	};
		
	struct SCommand_Signal : public SCommand
	{
		SCommand_Signal() : SCommand(CMD_SIGNAL) {}
		class semaphore* m_pSemaphore;
	};

	struct SCommand_RunBuffer : public SCommand
	{
		SCommand_RunBuffer() : SCommand(CMD_RUNBUFFER) {}
		CCommandBuffer *m_pOtherBuffer;
	};

	struct SCommand_Render : public SCommand
	{
		SCommand_Render() : SCommand(CMD_RENDER) {}
		SState m_State;
		unsigned m_PrimType;
		unsigned m_PrimCount;
		SVertex *m_pVertices; // you should use the command buffer data to allocate vertices for this command
	};

	struct SCommand_Screenshot : public SCommand
	{
		SCommand_Screenshot() : SCommand(CMD_SCREENSHOT) {}
		int m_X, m_Y, m_W, m_H; // specify rectangle size, -1 if fullscreen (width/height)
		CImageInfo *m_pImage; // processor will fill this out, the one who adds this command must free the data as well
	};

	struct SCommand_VideoModes : public SCommand
	{
		SCommand_VideoModes() : SCommand(CMD_VIDEOMODES) {}

		CVideoMode *m_pModes; // processor will fill this in
		int m_MaxModes; // maximum of modes the processor can write to the m_pModes
		int *m_pNumModes; // processor will write to this pointer
		int m_Screen;
	};

	struct SCommand_Swap : public SCommand
	{
		SCommand_Swap() : SCommand(CMD_SWAP) {}

		int m_Finish;
	};

	struct SCommand_VSync : public SCommand
	{
		SCommand_VSync() : SCommand(CMD_VSYNC) {}

		int m_VSync;
		bool *m_pRetOk;
	};

	struct SCommand_Texture_Create : public SCommand
	{
		SCommand_Texture_Create() : SCommand(CMD_TEXTURE_CREATE) {}

		// texture information
		int m_Slot;

		int m_Width;
		int m_Height;
		int m_GridWidth;
		int m_GridHeight;
		int m_PixelSize;
		int m_Format;
		int m_StoreFormat;
		int m_Flags;
		unsigned char *m_pData; // will be freed by the command processor
	};

	struct SCommand_Texture_Update : public SCommand
	{
		SCommand_Texture_Update() : SCommand(CMD_TEXTURE_UPDATE) {}

		// texture information
		int m_Slot;

		int m_X;
		int m_Y;
		int m_Width;
		int m_Height;
		int m_Format;
		unsigned char *m_pData; // will be freed by the command processor
	};


	struct SCommand_Texture_Destroy : public SCommand
	{
		SCommand_Texture_Destroy() : SCommand(CMD_TEXTURE_DESTROY) {}

		// texture information
		int m_Slot;
	};
	
	//
	CCommandBuffer(unsigned CmdBufferSize, unsigned DataBufferSize)
	: m_CmdBuffer(CmdBufferSize), m_DataBuffer(DataBufferSize)
	{
	}

	void *AllocData(unsigned WantedSize)
	{
		return m_DataBuffer.Alloc(WantedSize);
	}

	template<class T>
	bool AddCommand(const T &Command)
	{
		// make sure that we don't do something stupid like ->AddCommand(&Cmd);
		(void)static_cast<const SCommand *>(&Command);

		// allocate and copy the command into the buffer
		SCommand *pCmd = (SCommand *)m_CmdBuffer.Alloc(sizeof(Command));
		if(!pCmd)
			return false;
		mem_copy(pCmd, &Command, sizeof(Command));
		pCmd->m_Size = sizeof(Command);
		return true;
	}

	SCommand *GetCommand(unsigned *pIndex)
	{
		if(*pIndex >= m_CmdBuffer.DataUsed())
			return NULL;

		SCommand *pCommand = (SCommand *)&m_CmdBuffer.DataPtr()[*pIndex];
		*pIndex += pCommand->m_Size;
		return pCommand;
	}

	void Reset()
	{
		m_CmdBuffer.Reset();
		m_DataBuffer.Reset();
	}
};

class CGraphics : public CClientKernel::CComponent
{
public:
	enum
	{
		NUM_CMDBUFFERS = 2,

		MAX_VERTICES = 32*1024,
		MAX_TEXTURES = 1024*4,
		
		DRAWING_QUADS=1,
		DRAWING_LINES=2
	};
	
	/* Constants: Texture Loading Flags
		TEXLOAD_NORESAMPLE - Prevents the texture from any resampling
		TEXLOAD_NOMIPMAPS - Prevents the texture from generating mipmaps
		TEXLOAD_ARRAY_256 - Texture will be loaded as 3D texture with 16*16 subtiles
		TEXLOAD_MULTI_DIMENSION - Texture will be loaded as 2D and 3D texture
	*/
	enum
	{
		TEXLOAD_NORESAMPLE = 1,
		TEXLOAD_NOMIPMAPS = 2,
		TEXLOAD_ARRAY_256 = 4,
		TEXLOAD_MULTI_DIMENSION = 8,
	};
	
	enum
	{
		WRAP_REPEAT = 0,
		WRAP_CLAMP,
	};
	
protected:
	int m_ScreenWidth;
	int m_ScreenHeight;
	int m_DesktopScreenWidth;
	int m_DesktopScreenHeight;
	
	CCommandBuffer::SState m_State;
	class CGraphicsBackend_SDL* m_pBackend;

	CCommandBuffer *m_apCommandBuffers[NUM_CMDBUFFERS];
	CCommandBuffer *m_pCommandBuffer;
	unsigned m_CurrentCommandBuffer;

	//
	class CStorage *m_pStorage;
	class IConsole *m_pConsole;

	CCommandBuffer::SVertex m_aVertices[MAX_VERTICES];
	int m_NumVertices;

	CCommandBuffer::SColor m_aColor[4];
	CCommandBuffer::STexCoord m_aTexture[4];

	bool m_RenderEnable;

	float m_Rotation;
	int m_Drawing;
	bool m_DoScreenshot;
	char m_aScreenshotName[128];

//TAG_DEBUG
public:
	array<CCommandBuffer::SClip> m_ClipStack;

protected:
	CTextureHandle m_InvalidTexture;

	int m_aTextureIndices[MAX_TEXTURES];
	int m_FirstFreeTexture;
	int m_TextureMemoryUsage;

protected:
	int m_RenderFrames;
	int64 m_LastRenderTime;
	int64 m_RenderFrameTime;
	int64 m_RenderFrameTimeLow;
	int64 m_RenderFrameTimeHigh;
	bool m_IsReadyToRender;
	int m_DesktopWidth;
	int m_DesktopHeight;

public:
	int m_Cfg_Alphabits;
	int m_Cfg_Clear;
	int m_Cfg_HighDetail;
	int m_Cfg_RefreshRate;
	int m_Cfg_AsyncRender;
	int m_Cfg_Finish;
	int m_Cfg_Screen;
	int m_Cfg_FsaaSamples;
	int m_Cfg_DisplayAllModes;
	int m_Cfg_ScreenWidth;
	int m_Cfg_ScreenHeight;
	int m_Cfg_Borderless;
	int m_Cfg_Fullscreen;
	int m_Cfg_Vsync;
	int m_Cfg_Resizable;
	int m_Cfg_TextureCompression;
	int m_Cfg_TextureQuality;
	int m_Cfg_DbgTexture;

protected:
	inline CTextureHandle CreateTextureHandle(int Index)
	{
		CTextureHandle Tex;
		Tex.SetId(Index);
		return Tex;
	}
	
	void FlushVertices();
	void AddVertices(int Count);
	void Rotate4(const CCommandBuffer::SPoint &rCenter, CCommandBuffer::SVertex *pPoints);

	void KickCommandBuffer();

	int IssueInit();
	int InitWindow();

public:
	CGraphics(CClientKernel* pKernel);
	
	void SetScreenSize(int Width, int Height);
	
	int ScreenWidth() const { return m_ScreenWidth; }
	int ScreenHeight() const { return m_ScreenHeight; }
	float ScreenAspect() const { return (float)ScreenWidth()/(float)ScreenHeight(); }
	
	void ClipEnable(int x, int y, int w, int h);
	void ClipDisable();

	void ClipPush(int x, int y, int w, int h);
	void ClipPop();

	void BlendNone();
	void BlendNormal();
	void BlendAdditive();

	void WrapNormal();
	void WrapClamp();
	void WrapMode(int WrapU, int WrapV);

	int MemoryUsage() const;

	void MapScreen(float TopLeftX, float TopLeftY, float BottomRightX, float BottomRightY);
	void GetScreen(float *pTopLeftX, float *pTopLeftY, float *pBottomRightX, float *pBottomRightY);

	struct CLineItem
	{
		float m_X0, m_Y0, m_X1, m_Y1;
		CLineItem() {}
		CLineItem(float x0, float y0, float x1, float y1) : m_X0(x0), m_Y0(y0), m_X1(x1), m_Y1(y1) {}
	};
	
	void LinesBegin();
	void LinesEnd();
	void LinesDraw(const CLineItem *pArray, int Num);

	int UnloadTexture(CTextureHandle *Index);
	CTextureHandle LoadTextureRaw(int Width, int Height, int GridWidth, int GridHeight, int Format, const void *pData, int StoreFormat, int Flags);
	int LoadTextureRawSub(CTextureHandle TextureID, int x, int y, int Width, int Height, int Format, const void *pData);
	void TextureClear() { TextureSet(CTextureHandle()); }
	
	// simple uncompressed RGBA loaders
	//virtual CTextureHandle LoadTexture(const char *pFilename, int StorageType, int StoreFormat, int Flags);
	//int LoadPNG(CImageInfo *pImg, const char *pFilename, int StorageType);

	void ScreenshotDirect(const char *pFilename);

	void TextureSet(CTextureHandle TextureID);

	void Clear(float r, float g, float b);

	struct CQuadItem
	{
		float m_X, m_Y, m_Width, m_Height;
		CQuadItem() {}
		CQuadItem(float x, float y, float w, float h) : m_X(x), m_Y(y), m_Width(w), m_Height(h) {}
	};
	void QuadsBegin();
	void QuadsEnd();
	void QuadsSetRotation(float Angle);

	struct CColorVertex
	{
		int m_Index;
		float m_R, m_G, m_B, m_A;
		CColorVertex() {}
		CColorVertex(int i, float r, float g, float b, float a) : m_Index(i), m_R(r), m_G(g), m_B(b), m_A(a) {}
	};
	void SetColorVertex(const CColorVertex *pArray, int Num);
	void SetColor(float r, float g, float b, float a);
	void SetColor4(vec4 C0, vec4 C1, vec4 C2, vec4 C3);
	
	void SetColor(vec4 rgba, bool AlphaBlend);
	void SetColor2(vec4 Color0, vec4 Color1, bool AlphaBlend);
	void SetColor4(vec4 C0, vec4 C1, vec4 C2, vec4 C3, bool AlphaBlend);

	struct CFreeformItem
	{
		float m_X0, m_Y0, m_X1, m_Y1, m_X2, m_Y2, m_X3, m_Y3;
		CFreeformItem() {}
		CFreeformItem(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
			: m_X0(x0), m_Y0(y0), m_X1(x1), m_Y1(y1), m_X2(x2), m_Y2(y2), m_X3(x3), m_Y3(y3) {}
		CFreeformItem(vec2 v0, vec2 v1, vec2 v2, vec2 v3)
			: m_X0(v0.x), m_Y0(v0.y), m_X1(v1.x), m_Y1(v1.y), m_X2(v2.x), m_Y2(v2.y), m_X3(v3.x), m_Y3(v3.y) {}
	};
	void QuadsSetSubset(float TlU, float TlV, float BrU, float BrV, int TextureIndex = -1);
	void QuadsSetSubsetFree(
		float x0, float y0, float x1, float y1,
		float x2, float y2, float x3, float y3, int TextureIndex = -1);

	void QuadsDraw(CQuadItem *pArray, int Num);
	void QuadsDrawTL(const CQuadItem *pArray, int Num);
	void QuadsDrawFreeform(const CFreeformItem *pArray, int Num);
	void QuadsText(float x, float y, float Size, const char *pText);

	int GetNumScreens() const;
	void Minimize();
	void Maximize();
	bool Fullscreen(bool State);
	void SetWindowBordered(bool State);
	int SetWindowScreen(int Index);
	int GetWindowScreen();

	int WindowActive();
	int WindowOpen();

	virtual bool InitConfig(int argc, const char** argv);
	virtual void SaveConfig(class CCLI_Output* pOutput);
	virtual bool Init();
	virtual bool PreUpdate();
	virtual bool PostUpdate();
	virtual void Shutdown();

	void ReadBackbuffer(unsigned char **ppPixels, int x, int y, int w, int h);
	void TakeScreenshot(const char *pFilename);
	void Swap();
	bool SetVSync(bool State);

	int GetVideoModes(CVideoMode *pModes, int MaxModes, int Screen);

	// syncronization
	void InsertSignal(class semaphore* pSemaphore);
	bool IsIdle() const;
	void WaitForIdle();
	
	bool ReadyToRender() const { return m_IsReadyToRender; }
};

/* FOREIGN CODE END: TeeWorlds ****************************************/

#endif
