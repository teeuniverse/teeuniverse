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

#include "button.h"
	
namespace gui
{

/* ABSTRACT BUTTON ****************************************************/

CAbstractButton::CAbstractButton(CGui *pContext) :
	CAbstractLabel(pContext),
	m_Clicked(false),
	m_MouseOver(false),
	m_Editable(true)
{
	m_ButtonStylePath = Context()->GetButtonStyle();
}

void CAbstractButton::RefreshLabelStyle()
{
	const CAsset_GuiButtonStyle* pButtonStyle = AssetsManager()->GetAsset<CAsset_GuiButtonStyle>(m_ButtonStylePath);
	if(pButtonStyle)
	{
		if(!m_Editable)
			SetLabelStyle(pButtonStyle->GetReadOnlyStylePath());
		else if(m_MouseOver)
			SetLabelStyle(pButtonStyle->GetMouseOverStylePath());
		else
			SetLabelStyle(pButtonStyle->GetIdleStylePath());
	}
	else
		SetLabelStyle(CAssetPath::Null());
}

void CAbstractButton::SetButtonStyle(CAssetPath StylePath)
{
	m_ButtonStylePath = StylePath;
	RefreshLabelStyle();
}
	
void CAbstractButton::Update(bool ParentEnabled)
{
	if(ParentEnabled && IsEnabled())
		Context()->TryToGetFocus(this);
	
	RefreshLabelStyle();
	
	CAbstractLabel::Update(ParentEnabled);
}

void CAbstractButton::OnButtonClick(int Button)
{
	if(m_Editable && Button == KEY_MOUSE_1)
	{
		if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
			m_Clicked = true;
		else
			Context()->StopFocus(this);
	}
	
	CAbstractLabel::OnButtonClick(Button);
}

void CAbstractButton::OnButtonRelease(int Button)
{
	if(m_Editable && Button == KEY_MOUSE_1 && m_Clicked)
	{
		if(m_DrawRect.IsInside(Context()->GetMousePos()))
		{
			m_Clicked = false;
			MouseClickAction();
		}
	}
	
	CAbstractLabel::OnButtonRelease(Button);
}

void CAbstractButton::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		m_MouseOver = true;
	else
		m_MouseOver = false;
	
	CAbstractLabel::OnMouseMove();
}

void CAbstractButton::OnInputEvent(const CInput::CEvent& Event)
{
	if(Context()->HasFocus(this) && (Event.m_Flags & CInput::FLAG_PRESS) && (Event.m_Key == KEY_RETURN || Event.m_Key == KEY_KP_ENTER))
		MouseClickAction();
}

/* BUTTON *************************************************************/

CButton::CButton(CGui* pContext, const char* pText, CAssetPath IconPath) :
	CAbstractButton(pContext)
{
	SetText(pText);
	SetIcon(IconPath);
}

CButton::CButton(CGui* pContext, const CLocalizableString& LocalizableString, CAssetPath IconPath) :
	CAbstractButton(pContext)
{
	SetText(LocalizableString);
	SetIcon(IconPath);
}

CButton::CButton(CGui* pContext, CAssetPath IconPath) :
	CAbstractButton(pContext)
{
	SetText("");
	SetIcon(IconPath);
}

}
