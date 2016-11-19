/*
 * Copyright (C) 2016 necropotame (necropotame@gmail.com)
 * 
 * This file is part of TeeUniverses.
 * 
 * TeeUniverses is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * TeeUniverses is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with TeeUniverses.  If not, see <http://www.gnu.org/licenses/>.
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
	m_MouseOver(false)
{
	m_ButtonStylePath = Context()->GetButtonStyle();
}

void CAbstractButton::RefreshLabelStyle()
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

void CAbstractButton::SetButtonStyle(CAssetPath StylePath)
{
	m_ButtonStylePath = StylePath;
	RefreshLabelStyle();
}
	
void CAbstractButton::Update(bool ParentEnabled)
{
	RefreshLabelStyle();
	CAbstractLabel::Update(ParentEnabled);
}

void CAbstractButton::OnButtonClick(int Button)
{
	if(Button != KEY_MOUSE_1)
		return;
	
	if(m_DrawRect.IsInside(Context()->GetMousePos()))
	{
		m_Clicked = true;
	}
}

void CAbstractButton::OnButtonRelease(int Button)
{
	if(Button != KEY_MOUSE_1)
		return;
	
	if(!m_Clicked)
		return;
	
	if(m_DrawRect.IsInside(Context()->GetMousePos()))
	{
		m_Clicked = false;
		MouseClickAction();
	}
}

void CAbstractButton::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		m_MouseOver = true;
	else
		m_MouseOver = false;
}

/* BUTTON *************************************************************/

CButton::CButton(CGui* pContext, const char* pText, CAssetPath IconPath) :
	CAbstractButton(pContext)
{
	SetText(pText, false);
	SetIcon(IconPath);
}

CButton::CButton(CGui* pContext, const CLocalizableString& LocalizableString, CAssetPath IconPath) :
	CAbstractButton(pContext)
{
	SetText(LocalizableString.m_pText, true);
	SetIcon(IconPath);
}

CButton::CButton(CGui* pContext, CAssetPath IconPath) :
	CAbstractButton(pContext)
{
	SetText("");
	SetIcon(IconPath);
}

}
