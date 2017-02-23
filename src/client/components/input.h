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

#ifndef __CLIENT_INPUT__
#define __CLIENT_INPUT__

#include <client/kernel.h>
#include <shared/system/time.h>

#include <SDL.h>

#include <list>

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

const int g_MaxKeys = 512;
extern const char g_aaKeyStrings[g_MaxKeys][20];

class CInput : public CClientKernel::CComponent
{
public:
	class CEvent
	{
	public:
		int m_Flags;
		int m_Key;
		char m_aText[32];
		int m_InputCount;
	};

protected:
	enum
	{
		INPUT_BUFFER_SIZE=32
	};

	// quick access to events
	int m_NumEvents;
	CInput::CEvent m_aInputEvents[INPUT_BUFFER_SIZE];

public:
	enum
	{
		FLAG_PRESS=1,
		FLAG_RELEASE=2,
		FLAG_REPEAT=4,
		FLAG_TEXT=8,
	};
	
	enum
	{
		CURSOR_DEFAULT=0,
		CURSOR_TRANSLATEX,
		CURSOR_TRANSLATEY,
		CURSOR_TEXT,
		CURSOR_LOADING,
		NUM_CURSORS,
	};
	
	struct CInputRect
	{
		int x;
		int y;
		int w;
		int h;
	};

public:
	int m_Cfg_Mousesens;

private:
	bool m_RelativeMode;
	bool m_TextEdited;
	bool m_Composing;
	dynamic_string m_EditedText;
	std::list<SDL_Rect> m_TextEditingQueue;
	SDL_Cursor* m_Cursors[NUM_CURSORS];
	int m_WantedCursor;
	int m_CurrentCursor;
	
	int64 m_LastRelease;
	int64 m_ReleaseDelta;

	void AddEvent(char *pText, int Key, int Flags);

public:
	void Clear();
	int NumEvents() const { return m_NumEvents; } //From old interface
	bool IsEventValid(CEvent *pEvent) const { return pEvent->m_InputCount == m_InputCounter; };
	CEvent GetEvent(int Index) const //From old interface
	{
		if(Index < 0 || Index >= m_NumEvents)
		{
			CInput::CEvent e;
			e.m_Flags = 0;
			e.m_Key = 0;
			e.m_aText[0] = 0;
			e.m_InputCount = 0;
			return e;
		}
		return m_aInputEvents[Index];
	}
	
	//quick access to input
	unsigned short m_aInputCount[g_MaxKeys];	// tw-KEY
	unsigned char m_aInputState[g_MaxKeys];	// SDL_SCANCODE
	int m_InputCounter;

	void ClearKeyStates();
	bool KeyState(int Key) const;

public:
	CInput(CClientKernel* pKernel);

	virtual bool InitConfig(int argc, const char** argv);
	virtual void SaveConfig(class CCLI_Output* pOutput);
	virtual bool Init();
	virtual void Shutdown();
	virtual bool PreUpdate();
	virtual bool PostUpdate();

	bool KeyIsPressed(int Key) const { return KeyState(Key); }
	bool KeyPress(int Key, bool CheckCounter) const { return CheckCounter ? (m_aInputCount[Key] == m_InputCounter) : m_aInputCount[Key]; }

	//Return true if the position is absolute
	bool GetMousePosition(float *x, float *y);
	void SetCursorType(int CursorType);
	
	void MouseModeAbsolute();
	void MouseModeRelative();
	int MouseDoubleClick();

	bool IsTextEdited() const;
	const char* GetEditedText() const;
	void StartTextEditing(int x, int y, int w, int h);
	void StopTextEditing();
	
	const char *KeyName(int Key) const { return (Key >= 0 && Key < g_MaxKeys) ? g_aaKeyStrings[Key] : g_aaKeyStrings[0]; } //From old interface
	int KeyID(const char* pKeyName) const;
	
	const char* GetClipboardText();
	void SetClipboardText(const char* pText);
};

/* FOREIGN CODE END: TeeWorlds ****************************************/

#endif
