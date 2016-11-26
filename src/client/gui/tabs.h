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

#ifndef __CLIENT_GUI_TABS__
#define __CLIENT_GUI_TABS__

#include <shared/tl/array.h>

#include "listlayout.h"
#include "button.h"
	
namespace gui
{

class CTabs : public CWidget
{
private:
	class CTabButton : public CButton
	{
	protected:
		CTabs* m_pTabs;
		int m_Id;
		virtual void MouseClickAction();
		
	public:
		CTabButton(CGui* pContext, CTabs *m_pTabs, int Id, const char* pName, CAssetPath IconPath);
		CTabButton(CGui* pContext, CTabs *m_pTabs, int Id, const CLocalizableString& Name, CAssetPath IconPath);
	};
	
	struct CTab
	{
		CWidget* m_pWidget;
		CTabButton* m_pTabButton;
	};
	
protected:
	CAssetPath m_TabsStylePath;
	CRect m_ClipRect;
	array<CTab> m_Tabs;
	int m_SelectedTab;
	CHListLayout* m_pButtonList;
	
protected:
	void RegenerateButtons();
	void AddTab(CWidget* pWidget, const char* pName, const CLocalizableString* pLocalizableName, CAssetPath IconPath);
	
public:
	CTabs(class CGui *pConfig);
	
	virtual void Destroy();
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	virtual void Update(bool ParentEnabled);
	virtual void Render();
	
	virtual void OnMouseMove();
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	virtual void OnInputEvent(const CInput::CEvent& Event);
	
	void AddTab(CWidget* pWidget, const char* pName, CAssetPath IconPath);
	void AddTab(CWidget* pWidget, const CLocalizableString& LocalizableString, CAssetPath IconPath);
	void Clear();
	void OpenTab(int Id);
	
	void SetTabsStyle(CAssetPath TabsStylePath) { m_TabsStylePath = TabsStylePath; }
	CAssetPath GetTabsStyle() const { return m_TabsStylePath; }
};

}

#endif
