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

#include "toggle.h"
	
namespace gui
{

/* ABSTRACT TOGGLE ****************************************************/

CAbstractToggle::CAbstractToggle(CGui* pConfig) :
	CAbstractLabel(pConfig),
	m_Clicked(false)
{
	m_ToggleStylePath = Context()->GetToggleStyle();
}

void CAbstractToggle::RefreshLabelStyle()
{
	const CAsset_GuiToggleStyle* pToggleStyle = AssetsManager()->GetAsset<CAsset_GuiToggleStyle>(m_ToggleStylePath);
	if(pToggleStyle)
	{
		if(GetValue())
		{
			if(pToggleStyle->GetSwitchIcon())
				SetIcon(pToggleStyle->GetIconTruePath());
			
			if(m_DrawRect.IsInside(Context()->GetMousePos()))
				SetLabelStyle(pToggleStyle->GetMouseOverTrueStylePath());
			else
				SetLabelStyle(pToggleStyle->GetIdleTrueStylePath());
		}
		else
		{
			if(pToggleStyle->GetSwitchIcon())
				SetIcon(pToggleStyle->GetIconFalsePath());
			
			if(m_DrawRect.IsInside(Context()->GetMousePos()))
				SetLabelStyle(pToggleStyle->GetMouseOverFalseStylePath());
			else
				SetLabelStyle(pToggleStyle->GetIdleFalseStylePath());
		}
	}
	else
		SetLabelStyle(CAssetPath::Null());
}

void CAbstractToggle::SetToggleStyle(CAssetPath StylePath)
{
	m_ToggleStylePath = StylePath;
	RefreshLabelStyle();
}
	
void CAbstractToggle::Update(bool ParentEnabled)
{
	RefreshLabelStyle();
	
	CAbstractLabel::Update(ParentEnabled);
}

void CAbstractToggle::OnButtonClick(int Button)
{
	if(Button == KEY_MOUSE_1)
	{
		if(m_DrawRect.IsInside(Context()->GetMousePos()))
			m_Clicked = true;
	}
		
	CAbstractLabel::OnButtonClick(Button);
}

void CAbstractToggle::OnButtonRelease(int Button)
{
	if(m_Clicked && Button == KEY_MOUSE_1)
	{
		if(m_DrawRect.IsInside(Context()->GetMousePos()))
		{
			m_Clicked = false;
			SetValue(!GetValue());
		}
	}
	
	CAbstractLabel::OnButtonRelease(Button);
}

/* TOGGLE *************************************************************/

CToggle::CToggle(CGui* pContext, CAssetPath IconPath) :
	CAbstractToggle(pContext)
{
	SetText("");
	SetIcon(IconPath);
}

CToggle::CToggle(CGui* pContext, const char* pText) :
	CAbstractToggle(pContext)
{
	SetText(pText);
}

CToggle::CToggle(CGui* pContext, const CLocalizableString& LocalizableString) :
	CAbstractToggle(pContext)
{
	SetText(LocalizableString);
}

CToggle::CToggle(CGui* pContext, const char* pText, CAssetPath IconPath) :
	CAbstractToggle(pContext)
{
	SetText(pText);
	SetIcon(IconPath);
}

CToggle::CToggle(CGui* pContext, const CLocalizableString& LocalizableString, CAssetPath IconPath) :
	CAbstractToggle(pContext)
{
	SetText(LocalizableString);
	SetIcon(IconPath);
}

}
