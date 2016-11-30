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

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

#include "input.h"
#include <shared/system/memory.h>
#include <shared/components/cli.h>
#include <client/keys.h>
#include <client/components/graphics.h>

#include <SDL.h>

// this header is protected so you don't include it from anywere
#define KEYS_INCLUDE
#include <client/keynames.h>
#undef KEYS_INCLUDE

void CInput::AddEvent(char *pText, int Key, int Flags)
{
	if(m_NumEvents != INPUT_BUFFER_SIZE)
	{
		m_aInputEvents[m_NumEvents].m_Key = Key;
		m_aInputEvents[m_NumEvents].m_Flags = Flags;
		if(!pText)
			m_aInputEvents[m_NumEvents].m_aText[0] = 0;
		else
			str_copy(m_aInputEvents[m_NumEvents].m_aText, pText, sizeof(m_aInputEvents[m_NumEvents].m_aText));
		m_aInputEvents[m_NumEvents].m_InputCount = m_InputCounter;
		m_NumEvents++;
	}
}

CInput::CInput(CClientKernel* pKernel) :
	CClientKernel::CComponent(pKernel)
{
	SetName("Input");
	
	mem_zero(m_aInputCount, sizeof(m_aInputCount));
	mem_zero(m_aInputState, sizeof(m_aInputState));

	m_InputCounter = 1;
	m_RelativeMode = false;

	m_LastRelease = 0;
	m_ReleaseDelta = -1;

	m_NumEvents = 0;
	m_TextEdited = false;
}

bool CInput::InitConfig(int argc, const char** argv)
{
	m_Cfg_Mousesens = 100;
	
	CLI()->RegisterConfigInteger("inp_mousesens", "Mouse sensitivity", &m_Cfg_Mousesens, 5, 100000);
	
	return true;
}

void CInput::SaveConfig(CCLI_Output* pOutput)
{
	fixed_string128 Buffer;
	
	str_format(Buffer.buffer(), Buffer.maxsize(), "inp_mousesens %d", m_Cfg_Mousesens); pOutput->Print(Buffer.buffer());
}

bool CInput::Init()
{
	m_Cursors[CURSOR_DEFAULT] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	m_Cursors[CURSOR_TRANSLATEX] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	m_Cursors[CURSOR_TRANSLATEY] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	m_Cursors[CURSOR_TEXT] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	m_Cursors[CURSOR_LOADING] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
	
	MouseModeAbsolute();
	
	m_Composing = false;
	m_TextEdited = false;
	
	return true;
}

void CInput::Shutdown()
{
	for(int i=0; i<NUM_CURSORS; i++)
	{
		SDL_FreeCursor(m_Cursors[i]);
	}
}

void CInput::SetCursorType(int CursorType)
{
	SDL_SetCursor(m_Cursors[CursorType]);
}

bool CInput::GetMousePosition(float *x, float *y)
{
	int nx = 0, ny = 0;
	
	if(m_RelativeMode)
	{
		float Sens = m_Cfg_Mousesens/100.0f;
		SDL_GetRelativeMouseState(&nx,&ny);
		*x = nx*Sens;
		*y = ny*Sens;
		return false;
	}
	else
	{
		SDL_GetMouseState(&nx, &ny);
		*x = nx;
		*y = ny;
		return true;
	}
}

void CInput::MouseModeAbsolute()
{
	m_RelativeMode = false;
	SDL_ShowCursor(SDL_ENABLE);
	SDL_SetRelativeMouseMode(SDL_FALSE);
}

void CInput::MouseModeRelative()
{
	m_RelativeMode = true;
	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_GetRelativeMouseState(NULL, NULL);
}

int CInput::MouseDoubleClick()
{
	if(m_ReleaseDelta >= 0 && m_ReleaseDelta < (time_freq() >> 2))
	{
		m_LastRelease = 0;
		m_ReleaseDelta = -1;
		return 1;
	}
	return 0;
}

void CInput::Clear()
{
	mem_zero(m_aInputState, sizeof(m_aInputState));
	mem_zero(m_aInputCount, sizeof(m_aInputCount));
	m_NumEvents = 0;
}

bool CInput::KeyState(int Key) const
{
	return m_aInputState[Key>=KEY_MOUSE_1 ? Key : SDL_GetScancodeFromKey(KeyToKeycode(Key))];
}

bool CInput::PreUpdate()
{
	// keep the counter between 1..0xFFFF, 0 means not pressed
	m_InputCounter = (m_InputCounter%0xFFFF)+1;

	{
		int i;
		const Uint8 *pState = SDL_GetKeyboardState(&i);
		if(i >= KEY_LAST)
			i = KEY_LAST-1;
		mem_copy(m_aInputState, pState, i);
	}

	// these states must always be updated manually because they are not in the GetKeyState from SDL
	int i = SDL_GetMouseState(NULL, NULL);
	if(i&SDL_BUTTON(1)) m_aInputState[KEY_MOUSE_1] = 1; // 1 is left
	if(i&SDL_BUTTON(3)) m_aInputState[KEY_MOUSE_2] = 1; // 3 is right
	if(i&SDL_BUTTON(2)) m_aInputState[KEY_MOUSE_3] = 1; // 2 is middle
	if(i&SDL_BUTTON(4)) m_aInputState[KEY_MOUSE_4] = 1;
	if(i&SDL_BUTTON(5)) m_aInputState[KEY_MOUSE_5] = 1;
	if(i&SDL_BUTTON(6)) m_aInputState[KEY_MOUSE_6] = 1;
	if(i&SDL_BUTTON(7)) m_aInputState[KEY_MOUSE_7] = 1;
	if(i&SDL_BUTTON(8)) m_aInputState[KEY_MOUSE_8] = 1;
	if(i&SDL_BUTTON(9)) m_aInputState[KEY_MOUSE_9] = 1;

	{
		SDL_Event Event;

		while(SDL_PollEvent(&Event))
		{
			int Key = -1;
			int Scancode = 0;
			int Action = CInput::FLAG_PRESS;
			switch (Event.type)
			{
				case SDL_TEXTEDITING:
					if(strlen(Event.edit.text))
					{
						m_Composing = true;
						m_EditedText.copy((const char*) Event.edit.text);
					}
					else
					{
						m_Composing = false;
						m_EditedText.clear();
					}
					break;
				case SDL_TEXTINPUT:
					AddEvent(Event.text.text, 0, CInput::FLAG_TEXT);
					m_Composing = false;
					break;
				case SDL_KEYDOWN:
					Key = KeycodeToKey(Event.key.keysym.sym);
					Scancode = Event.key.keysym.scancode;
					break;
				case SDL_KEYUP:
					Action = CInput::FLAG_RELEASE;
					Key = KeycodeToKey(Event.key.keysym.sym);
					Scancode = Event.key.keysym.scancode;
					break;

				// handle mouse buttons
				case SDL_MOUSEBUTTONUP:
					Action = CInput::FLAG_RELEASE;

					if(Event.button.button == 1) // ignore_convention
					{
						m_ReleaseDelta = time_get() - m_LastRelease;
						m_LastRelease = time_get();
					}

					// fall through
				case SDL_MOUSEBUTTONDOWN:
					if(Event.button.button == SDL_BUTTON_LEFT) Key = KEY_MOUSE_1; // ignore_convention
					if(Event.button.button == SDL_BUTTON_RIGHT) Key = KEY_MOUSE_2; // ignore_convention
					if(Event.button.button == SDL_BUTTON_MIDDLE) Key = KEY_MOUSE_3; // ignore_convention
					if(Event.button.button == 4) Key = KEY_MOUSE_4; // ignore_convention
					if(Event.button.button == 5) Key = KEY_MOUSE_5; // ignore_convention
					if(Event.button.button == 6) Key = KEY_MOUSE_6; // ignore_convention
					if(Event.button.button == 7) Key = KEY_MOUSE_7; // ignore_convention
					if(Event.button.button == 8) Key = KEY_MOUSE_8; // ignore_convention
					if(Event.button.button == 9) Key = KEY_MOUSE_9; // ignore_convention
					Scancode = Key;
					break;

				case SDL_MOUSEWHEEL:
					if(Event.wheel.y > 0) Key = KEY_MOUSE_WHEEL_UP; // ignore_convention
					if(Event.wheel.y < 0) Key = KEY_MOUSE_WHEEL_DOWN; // ignore_convention
					Action |= CInput::FLAG_RELEASE;
					break;

				case SDL_WINDOWEVENT:
					switch(Event.window.event)
					{
						case SDL_WINDOWEVENT_SIZE_CHANGED:
							Graphics()->SetScreenSize(Event.window.data1, Event.window.data2);
							break;
#if defined(CONF_PLATFORM_MACOSX)	// Todo SDL: remove this when fixed (mouse state is faulty on start)
						case SDL_WINDOWEVENT_MAXIMIZED:
							MouseModeAbsolute();
							MouseModeRelative();
							break;
#endif
					}
					break;
				
				// other messages
				case SDL_QUIT:
					return false;
			}

			//
			if(Key != -1)
			{
				if(Action&CInput::FLAG_PRESS)
				{
					m_aInputState[Scancode] = 1;
					m_aInputCount[Key] = m_InputCounter;
				}
				AddEvent(0, Key, Action);
			}

		}
	}
	
	return true;
}

int CInput::KeyID(const char* pKeyName) const
{
	for(int i = 0; i < KEY_LAST; i++)
	{
		if(str_comp(pKeyName, KeyName(i)) == 0)
			return i;
	}
	return KEY_UNKNOWN;
}

/* FOREIGN CODE END: TeeWorlds ****************************************/

const char* CInput::GetEditedText() const
{
	if(m_Composing)
		return m_EditedText.buffer();
	else
		return 0;
}

void CInput::StartTextEditing(int x, int y, int w, int h)
{
	if(m_TextEdited)
	{
		SDL_Rect& InputRect = m_TextEditingQueue.increment();
		InputRect.x = x;
		InputRect.y = y;
		InputRect.w = w;
		InputRect.h = h;
	}
	else
	{
		SDL_StartTextInput();
		SDL_Rect InputRect;
		InputRect.x = x;
		InputRect.y = y;
		InputRect.w = w;
		InputRect.h = h;
		SDL_SetTextInputRect(&InputRect);
		
		m_TextEdited = true;
	}
}

void CInput::StopTextEditing()
{
	SDL_StopTextInput();
	m_Composing = false;
	m_EditedText.clear();
	
	if(m_TextEditingQueue.size())
	{
		SDL_StartTextInput();
		SDL_Rect InputRect;
		InputRect.x = m_TextEditingQueue[0].x;
		InputRect.y = m_TextEditingQueue[0].y;
		InputRect.w = m_TextEditingQueue[0].w;
		InputRect.h = m_TextEditingQueue[0].h;
		SDL_SetTextInputRect(&InputRect);
		
		m_TextEditingQueue.remove_index(0);
	}
	else
		m_TextEdited = false;
}

/* FOREIGN CODE END: TeeWorlds ****************************************/
