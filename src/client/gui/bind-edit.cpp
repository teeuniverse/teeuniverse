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
		SetText(Input()->KeyName(GetValue()));
	}
	
	RefreshLabelStyle();
	CAbstractLabel::Update(ParentEnabled);
}

void CAbstractBindEdit::OnButtonClick(int Button)
{
	if(Button != KEY_MOUSE_1)
		return;
	
	if(m_DrawRect.IsInside(Context()->GetMousePos()))
		m_Clicked = true;
	else
		Context()->StopFocus(this);
}

void CAbstractBindEdit::OnButtonRelease(int Button)
{
	if(Button != KEY_MOUSE_1)
		return;
	
	if(!m_Clicked)
		return;
	
	if(m_DrawRect.IsInside(Context()->GetMousePos()))
	{
		Context()->StartFocus(this);
		m_Clicked = false;
		m_CatchInput = true;
	}
}

void CAbstractBindEdit::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		m_MouseOver = true;
	else
		m_MouseOver = false;
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
			SetValue(Event.m_Key);
			m_CatchInput = false;
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

CBindEdit::CBindEdit(CGui* pContext) :
	CAbstractBindEdit(pContext)
{
	
}

}
