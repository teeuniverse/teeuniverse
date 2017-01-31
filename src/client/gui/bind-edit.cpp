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

#include <client/components/assetsrenderer.h>
#include <client/components/graphics.h>

#include "bind-edit.h"
	
namespace gui
{

/* ABSTRACT BIND EDIT *************************************************/

CAbstractBindEdit::CAbstractBindEdit(CGui* pContext) :
	CAbstractLabel(pContext),
	m_Clicked(false),
	m_MouseOver(false),
	m_CatchInput(false)
{
	m_ButtonStylePath = Context()->GetButtonStyle();
}

void CAbstractBindEdit::RefreshLabelStyle()
{
	const CAsset_GuiButtonStyle* pButtonStyle = AssetsManager()->GetAsset<CAsset_GuiButtonStyle>(m_ButtonStylePath);
	if(pButtonStyle)
	{
		if(m_MouseOver)
			SetLabelStyle(pButtonStyle->GetMouseOverStylePath());
		else
			SetLabelStyle(pButtonStyle->GetIdleStylePath());
	}
	else
		SetLabelStyle(CAssetPath::Null());
}

void CAbstractBindEdit::SetButtonStyle(CAssetPath StylePath)
{
	m_ButtonStylePath = StylePath;
	RefreshLabelStyle();
}
	
void CAbstractBindEdit::Update(bool ParentEnabled)
{
	Context()->TryToGetFocus(this);
	
	if(m_CatchInput)
	{
		SetText("Press any key or button");
	}
	else
	{
		CBindsManager::CKey Key = GetValue();
		if(Key.m_KeyID == KEY_LAST)
		{
			SetText("");
		}
		else
		{
			dynamic_string Buffer;
			int Iter = 0;
			if(Key.m_Modifier == CBindsManager::MODIFIER_CTRL)
				Iter = Buffer.append_at(Iter, "Ctrl + ");
			Iter = Buffer.append_at(Iter, Input()->KeyName(Key.m_KeyID));
			SetText(Buffer.buffer());
		}
	}
	
	RefreshLabelStyle();
	
	CAbstractLabel::Update(ParentEnabled);
}

void CAbstractBindEdit::OnButtonClick(int Button)
{
	if(Button == KEY_MOUSE_1)
	{
		if(m_DrawRect.IsInside(Context()->GetMousePos()))
			m_Clicked = true;
		else
			Context()->StopFocus(this);
	}
	
	CAbstractLabel::OnButtonClick(Button);
}

void CAbstractBindEdit::OnButtonRelease(int Button)
{
	if(m_Clicked && Button == KEY_MOUSE_1)
	{
		if(m_DrawRect.IsInside(Context()->GetMousePos()))
		{
			Context()->StartFocus(this);
			m_Clicked = false;
			m_CatchInput = true;
		}
	}
	
	CAbstractLabel::OnButtonRelease(Button);
}

void CAbstractBindEdit::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		m_MouseOver = true;
	else
		m_MouseOver = false;
	
	CAbstractLabel::OnMouseMove();
}

int CAbstractBindEdit::GetInputToBlock()
{
	if(m_CatchInput)
		return CGui::BLOCKEDINPUT_KEY;
	else
		return CGui::BLOCKEDINPUT_NONE;
}

void CAbstractBindEdit::OnInputEvent(const CInput::CEvent& Event)
{
	if(m_CatchInput)
	{
		if(Event.m_Key == KEY_ESCAPE && Event.m_Flags & CInput::FLAG_RELEASE)
		{
			m_CatchInput = false;
		}
		else if(Event.m_Key > 0)
		{
			if(m_AcceptModifier)
			{
				if(Event.m_Key != KEY_LCTRL)
				{
					CBindsManager::CKey Key;
					Key.m_KeyID = Event.m_Key;
					
					if(Input()->KeyIsPressed(KEY_LCTRL))
						Key.m_Modifier = CBindsManager::MODIFIER_CTRL;
					else
						Key.m_Modifier = CBindsManager::MODIFIER_NONE;
					
					SetValue(Key);
					m_CatchInput = false;
				}
			}
			else
			{
				CBindsManager::CKey Key;
				Key.m_KeyID = Event.m_Key;
				Key.m_Modifier = CBindsManager::MODIFIER_NONE;
				SetValue(Key);
				m_CatchInput = false;
			}
		}
	}
	else if(Context()->HasFocus(this) && (Event.m_Key == KEY_RETURN || Event.m_Key == KEY_KP_ENTER) && (Event.m_Flags & CInput::FLAG_RELEASE))
	{
		m_CatchInput = true;
	}
}

void CAbstractBindEdit::OnFocusStart()
{
	
}

void CAbstractBindEdit::OnFocusStop()
{
	m_CatchInput = false;
}

/* BIND EDIT **********************************************************/

CBindsManager::CKey CBindEdit::GetValue()
{
	CBindsManager::CKey Key;
	Key.m_KeyID = KEY_LAST;
	BindsManager()->CommandToKey(m_Command.buffer(), Key, m_BindNum);
	return Key;
}

void CBindEdit::SetValue(CBindsManager::CKey Key)
{
	BindsManager()->Bind(Key.m_KeyID, Key.m_Modifier, m_Command.buffer(), m_BindNum);
}

CBindEdit::CBindEdit(CGui* pContext, const char* pCommand, int BindNum, bool Modifier) :
	CAbstractBindEdit(pContext)
{
	m_Command = pCommand;
	m_BindNum = BindNum;
	m_AcceptModifier = Modifier;
}

}
