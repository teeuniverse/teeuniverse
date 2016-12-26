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

#include "gui.h"

#include <client/components/input.h>
#include <client/components/assetsrenderer.h>
#include <client/keys.h>
#include <client/gui/widget.h>
#include <client/gui/popup.h>
#include <shared/components/cli.h>

CGui::CGui(CClientKernel* pKernel) :
	CClientKernel::CComponent(pKernel),
	m_LocalizationUpdated(false),
	m_pMainWidget(NULL),
	m_pFocusedWidget(NULL)
{
	SetName("Gui");
	Localization()->AddListener(this);
}

CGui::~CGui()
{
	Localization()->RemoveListener(this);
	
	for(int i=0; i<m_pPopups.size(); i++)
		delete m_pPopups[i];
	if(m_pMainWidget)
		delete m_pMainWidget;
}
	
bool CGui::InitConfig(int argc, const char** argv)
{
	m_Cfg_Scale = GUI_DEFAULT_SCALE;
	
	CLI()->RegisterConfigInteger("gui_scale", "GUI Scaling factor (default = 128)", &m_Cfg_Scale, 0, 1024);
		
	return true;
}

void CGui::SaveConfig(class CCLI_Output* pOutput)
{
	fixed_string128 Buffer;
	
	str_format(Buffer.buffer(), Buffer.maxsize(), "gui_scale %d", m_Cfg_Scale); pOutput->Print(Buffer.buffer());
}
	
bool CGui::Init()
{
	m_Quit = false;
	
	LoadAssets();
	m_pMainWidget = CreateMainWidget();
	
	return true;
}

void CGui::DestroyWidget(gui::CWidget* pWidget)
{
	if(pWidget == m_pFocusedWidget)
	{
		m_DestructionList.add_by_copy(pWidget);
	}
	else
		delete pWidget;
}

bool CGui::PreUpdate()
{
	m_DrawRect.x = 0;
	m_DrawRect.y = 0;
	m_DrawRect.w = Graphics()->ScreenWidth();
	m_DrawRect.h = Graphics()->ScreenHeight();
	
	//Remove unused widgets
	for(int i=0; i<m_pPopups.size(); i++)
	{
		if(m_pPopups[i]->IsClosed())
		{
			delete m_pPopups[i];
			m_pPopups.remove_index(i);
		}
	}
	for(int i=0; i<m_DestructionList.size(); i++)
	{
		delete m_DestructionList[i];
		
		if(m_pFocusedWidget == m_DestructionList[i])
			m_pFocusedWidget = 0;
	}
	m_DestructionList.clear();
	
	//Mouse motion
	float MousePosX;
	float MousePosY;
	
	if(Input()->GetMousePosition(&MousePosX, &MousePosY))
	{
		m_MouseDelta.x = MousePosX - m_MousePos.x;
		m_MouseDelta.y = MousePosY - m_MousePos.y;
		m_MousePos.x = MousePosX;
		m_MousePos.y = MousePosY;
	}
	else
	{
		m_MouseDelta.x = MousePosX;
		m_MouseDelta.y = MousePosY;
		m_MousePos.x += m_MouseDelta.x;
		m_MousePos.y += m_MouseDelta.y;
	}
	m_MousePos.x = clamp(m_MousePos.x, 0, Graphics()->ScreenWidth()-1);
	m_MousePos.y = clamp(m_MousePos.y, 0, Graphics()->ScreenHeight()-1);
	
	m_ShowLoadingCursor = false;
	Input()->SetCursorType(CInput::CURSOR_DEFAULT);
	
	int BlockedInput = 0x0;
	if(m_pFocusedWidget)
		BlockedInput |= m_pFocusedWidget->GetInputToBlock();
	
	//Process input event
	bool ForceMouseMove = false;
	bool NextFocusWanted = false;
	bool PrevFocusWanted = false;
	for(int i=0; i<Input()->NumEvents(); i++)
	{
		CInput::CEvent Event = Input()->GetEvent(i);
		if(Event.m_Key != KEY_UNKNOWN)
		{
			switch(Event.m_Key)
			{
				case KEY_MOUSE_1:
				case KEY_MOUSE_2:
				case KEY_MOUSE_3:
				case KEY_MOUSE_WHEEL_UP:
				case KEY_MOUSE_WHEEL_DOWN:
					if(Event.m_Flags & CInput::FLAG_PRESS)
					{
						ForceMouseMove = true;
						
						if(m_pFocusedWidget && (m_pFocusedWidget->GetInputToBlock() & BLOCKEDINPUT_KEY))
							m_pFocusedWidget->OnButtonClick(Event.m_Key);
						else
						{
							bool Blocked = false;
							for(int l=gui::CPopup::LEVEL_HIGHEST; l>=gui::CPopup::LEVEL_LOWEST && !Blocked; l--)
							{
								for(int i=0; i<m_pPopups.size() && !Blocked; i++)
								{
									if(m_pPopups[i]->GetLevel() == l)
									{
										m_pPopups[i]->OnButtonClick(Event.m_Key);
										Blocked = Blocked || (m_pPopups[i]->GetInputToBlock() & BLOCKEDINPUT_KEY);
									}
								}
							}
							if(!Blocked)
							{
								if(m_pMainWidget)
									m_pMainWidget->OnButtonClick(Event.m_Key);
							}
						}
					}
					if(Event.m_Flags & CInput::FLAG_RELEASE)
					{
						ForceMouseMove = true;
						
						if(m_pFocusedWidget && (m_pFocusedWidget->GetInputToBlock() & BLOCKEDINPUT_KEY))
							m_pFocusedWidget->OnButtonRelease(Event.m_Key);
						else
						{
							bool Blocked = false;
							for(int l=gui::CPopup::LEVEL_HIGHEST; l>=gui::CPopup::LEVEL_LOWEST && !Blocked; l--)
							{
								for(int i=0; i<m_pPopups.size() && !Blocked; i++)
								{
									if(m_pPopups[i]->GetLevel() == l)
									{
										m_pPopups[i]->OnButtonRelease(Event.m_Key);
										Blocked = Blocked || (m_pPopups[i]->GetInputToBlock() & BLOCKEDINPUT_KEY);
									}
								}
							}
							if(!Blocked)
							{
								if(m_pMainWidget)
									m_pMainWidget->OnButtonRelease(Event.m_Key);
							}
						}
					}
					break;
					
				case KEY_TAB:
					if(Event.m_Flags & CInput::FLAG_PRESS)
					{
						if(Input()->KeyIsPressed(KEY_LSHIFT))
							PrevFocusWanted = true;
						else
							NextFocusWanted = true;
					}
				default:
				{
					if(m_pFocusedWidget && (m_pFocusedWidget->GetInputToBlock() & BLOCKEDINPUT_KEY))
						m_pFocusedWidget->OnInputEvent(Event);
					else
					{
						bool Blocked = false;
						for(int l=gui::CPopup::LEVEL_HIGHEST; l>=gui::CPopup::LEVEL_LOWEST && !Blocked; l--)
						{
							for(int i=0; i<m_pPopups.size() && !Blocked; i++)
							{
								if(m_pPopups[i]->GetLevel() == l)
								{
									m_pPopups[i]->OnInputEvent(Event);
									Blocked = Blocked || (m_pPopups[i]->GetInputToBlock() & BLOCKEDINPUT_KEY);
								}
							}
						}
						if(!Blocked)
						{
							if(m_pMainWidget)
								m_pMainWidget->OnInputEvent(Event);
						}
					}
				}
			}
		}
		else if(Event.m_Flags & CInput::FLAG_TEXT)
		{
			if(m_pFocusedWidget && (m_pFocusedWidget->GetInputToBlock() & BLOCKEDINPUT_TEXT))
				m_pFocusedWidget->OnInputEvent(Event);
			else
			{
				bool Blocked = false;
				for(int l=gui::CPopup::LEVEL_HIGHEST; l>=gui::CPopup::LEVEL_LOWEST && !Blocked; l--)
				{
					for(int i=0; i<m_pPopups.size() && !Blocked; i++)
					{
						if(m_pPopups[i]->GetLevel() == l)
						{
							m_pPopups[i]->OnInputEvent(Event);
							Blocked = Blocked || (m_pPopups[i]->GetInputToBlock() & BLOCKEDINPUT_TEXT);
						}
					}
				}
				if(!Blocked)
				{
					if(m_pMainWidget)
						m_pMainWidget->OnInputEvent(Event);
				}
			}
		}
	}
	
	if(ForceMouseMove || m_MouseDelta.x != 0 || m_MouseDelta.y != 0 )
	{
		if(m_pFocusedWidget && (m_pFocusedWidget->GetInputToBlock() & BLOCKEDINPUT_KEY))
			m_pFocusedWidget->OnMouseMove();
		else
		{
			bool Blocked = false;
			for(int l=gui::CPopup::LEVEL_HIGHEST; l>=gui::CPopup::LEVEL_LOWEST && !Blocked; l--)
			{
				for(int i=0; i<m_pPopups.size() && !Blocked; i++)
				{
					if(m_pPopups[i]->GetLevel() == l)
					{
						m_pPopups[i]->OnMouseMove();
						Blocked = Blocked || (m_pPopups[i]->GetInputToBlock() & BLOCKEDINPUT_MOUSEMOVE);
					}
				}
			}
			if(!Blocked)
			{
				if(m_pMainWidget)
					m_pMainWidget->OnMouseMove();
			}
		}
	}

	m_FocusIteratorState = (NextFocusWanted ? TRYTOGETFOCUS_SEARCHNEXT : (PrevFocusWanted ? TRYTOGETFOCUS_SEARCHPREV : TRYTOGETFOCUS_NO));
	m_pFocusCandidate = NULL;
	
	m_BlockedInput = 0x0;
	if(m_pFocusedWidget)
		m_BlockedInput |= m_pFocusedWidget->GetInputToBlock();
	
	for(int l=gui::CPopup::LEVEL_HIGHEST; l>=gui::CPopup::LEVEL_LOWEST; l--)
	{
		for(int i=0; i<m_pPopups.size(); i++)
		{
			if(m_pPopups[i]->GetLevel() == l)
			{
				m_pPopups[i]->Update(true);
				m_pPopups[i]->UpdateBoundingSize();
				m_pPopups[i]->UpdatePosition(m_DrawRect, m_DrawRect);
				m_BlockedInput |= m_pPopups[i]->GetInputToBlock();
			}
		}
	}
	if(m_pMainWidget)
	{
		m_pMainWidget->Update(true);
		m_pMainWidget->UpdateBoundingSize();
		m_pMainWidget->UpdatePosition(m_DrawRect, m_DrawRect);
	}
	
	m_FocusIteratorState = TRYTOGETFOCUS_NO;
	m_pFocusCandidate = NULL;
	
	Input()->Clear();
	
	m_LocalizationUpdated = false;
	
	if(Graphics()->ReadyToRender())
		Render();
	
	return !m_Quit;
}

void CGui::Render()
{
	Graphics()->MapScreen(m_DrawRect.x, m_DrawRect.y, m_DrawRect.w, m_DrawRect.h);
	
	if(m_pMainWidget)
		m_pMainWidget->Render();
	
	for(int l=gui::CPopup::LEVEL_LOWEST; l<=gui::CPopup::LEVEL_HIGHEST; l++)
	{
		for(int i=0; i<m_pPopups.size(); i++)
		{
			if(m_pPopups[i]->GetLevel() == l)
			{
				m_pPopups[i]->Render();
			}
		}
	}
}
	
void CGui::SetCursor(gui::CWidget* pWidget, int CursorType)
{
	if(!m_ShowLoadingCursor && ((pWidget == m_pFocusedWidget) || !(m_BlockedInput & BLOCKEDINPUT_MOUSEMOVE)))
	{
		Input()->SetCursorType(CursorType);
	}
}

void CGui::ShowLoadingCursor()
{
	if(!m_ShowLoadingCursor)
	{
		Input()->SetCursorType(CInput::CURSOR_LOADING);
	}
}

void CGui::DisplayPopup(gui::CPopup* pPopup)
{
	m_pPopups.add_by_copy(pPopup);
}
	
void CGui::OnLocalizationModified()
{
	m_LocalizationUpdated = true;
}

void CGui::StartFocus(gui::CWidget* pWidget)
{
	if(pWidget == m_pFocusedWidget)
		return;
	
	if(m_pFocusedWidget)
		m_pFocusedWidget->OnFocusStop();
	
	m_pFocusedWidget = pWidget;
	m_pFocusedWidget->OnFocusStart();
}

void CGui::StopFocus(gui::CWidget* pWidget)
{
	if(pWidget != m_pFocusedWidget)
		return;
	
	if(m_pFocusedWidget)
	{
		m_pFocusedWidget->OnFocusStop();
		m_pFocusedWidget = 0;
	}
}

void CGui::TryToGetFocus(gui::CWidget* pWidget)
{
	switch(m_FocusIteratorState)
	{
		case TRYTOGETFOCUS_SEARCHPREV:
			if(HasFocus(pWidget))
			{
				if(m_pFocusCandidate)
					StartFocus(m_pFocusCandidate);
				m_FocusIteratorState = TRYTOGETFOCUS_NO;
			}
			else
				m_pFocusCandidate = pWidget;
			break;
		
		case TRYTOGETFOCUS_SEARCHNEXT:
			if(HasFocus(pWidget))
				m_FocusIteratorState = TRYTOGETFOCUS_TAKENEXT;
			break;
		
		case TRYTOGETFOCUS_TAKENEXT:
			StartFocus(pWidget);
			m_FocusIteratorState = TRYTOGETFOCUS_NO;
			break;
	}
}
