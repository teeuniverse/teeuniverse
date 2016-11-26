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

#ifndef __CLIENT_GUI_BUTTON__
#define __CLIENT_GUI_BUTTON__

#include "label.h"
	
namespace gui
{

class CAbstractButton : public CAbstractLabel
{
protected:
	bool m_Clicked;
	bool m_MouseOver;
	CAssetPath m_ButtonStylePath;
	bool m_Editable;
	
protected:
	CAbstractButton(class CGui *pConfig);
	
	virtual void MouseClickAction() = 0;
	
public:
	virtual void Update(bool ParentEnabled);
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	virtual void OnMouseMove();
	virtual void OnInputEvent(const CInput::CEvent& Event);
	
	void RefreshLabelStyle();
	inline CAssetPath GetButtonStyle() const { return m_ButtonStylePath; }
	void SetButtonStyle(CAssetPath Path);
	
	inline bool IsEditable() const { return m_Editable; }
	inline void Editable(bool Value) { m_Editable = Value; }
};

class CButton : public CAbstractButton
{
public:
	CButton(class CGui *pConfig, const char* pText, CAssetPath IconPath = CAssetPath::Null());
	CButton(class CGui *pConfig, const CLocalizableString& LocalizableString, CAssetPath IconPath = CAssetPath::Null());
	CButton(class CGui *pConfig, CAssetPath IconPath);
};

}

#endif
