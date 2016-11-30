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

#ifndef __CLIENT_GUI_TOGGLE__
#define __CLIENT_GUI_TOGGLE__

#include "label.h"
	
namespace gui
{

class CAbstractToggle : public CAbstractLabel
{
protected:
	bool m_Clicked;
	CAssetPath m_ToggleStylePath;
	
protected:
	CAbstractToggle(class CGui *pConfig);
	
	void RefreshLabelStyle();
	
public:
	virtual void Update(bool ParentEnabled);
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);

	virtual bool GetValue() = 0;
	virtual void SetValue(bool Value) = 0;
	
	void SetToggleStyle(CAssetPath StylePath);
	inline CAssetPath GetToggleStyle() const { return m_ToggleStylePath; }
};

class CToggle : public CAbstractToggle
{
public:
	CToggle(class CGui *pConfig, const char* pText);
	CToggle(class CGui *pConfig, CAssetPath IconPath);
	CToggle(class CGui *pConfig, const CLocalizableString& LocalizableString);
	CToggle(class CGui *pConfig, const char* pText, CAssetPath IconPath);
	CToggle(class CGui *pConfig, const CLocalizableString& LocalizableString, CAssetPath IconPath);
};

}

#endif
