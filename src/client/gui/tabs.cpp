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

#include "tabs.h"
	
namespace gui
{

/* TABS ***************************************************************/

	//Button
CTabs::CTabButton::CTabButton(CGui* pContext, CTabs *pTabs, int Id, const char* pName, CAssetPath IconPath) :
	CButton(pContext, IconPath),
	m_pTabs(pTabs),
	m_Id(Id)
{
	SetText(pName);
}

CTabs::CTabButton::CTabButton(CGui* pContext, CTabs *pTabs, int Id, const CLocalizableString& Name, CAssetPath IconPath) :
	CButton(pContext, IconPath),
	m_pTabs(pTabs),
	m_Id(Id)
{
	SetText(Name);
}

void CTabs::CTabButton::MouseClickAction()
{
	m_pTabs->OpenTab(m_Id);
}

	//Tabs
CTabs::CTabs(CGui *pContext) :
	CWidget(pContext),
	m_SelectedTab(-1),
	m_pButtonList(0),
	m_TabsStylePath(Context()->GetTabsStyle())
{
	m_pButtonList = new CHListLayout(pContext);
}

void CTabs::Destroy()
{
	if(m_pButtonList)
		m_pButtonList->Destroy();
	
	for(int i=0; i<m_Tabs.size(); i++)
		m_Tabs[i].m_pWidget->Destroy();
	
	CWidget::Destroy();
}

void CTabs::OpenTab(int TabId)
{
	if(TabId >= 0 && TabId < m_Tabs.size())
		m_SelectedTab = TabId;
	else
		m_SelectedTab = -1;
}

void CTabs::AddTab(CWidget* pWidget, const char* pName, const CLocalizableString* pLocalizableName, CAssetPath IconPath)
{
	bool Fill = true;
	bool ButtonListText = true;
	const CAsset_GuiTabsStyle* pTabsStyle = AssetsManager()->GetAsset<CAsset_GuiTabsStyle>(m_TabsStylePath);
	if(pTabsStyle)
	{
		Fill = pTabsStyle->GetButtonListFill();
		ButtonListText = pTabsStyle->GetButtonListText();
	}
	int TabId = m_Tabs.size();
	CTab& Tab = m_Tabs.increment();
	
	Tab.m_pWidget = pWidget;
	Tab.m_pTabButton = 0;
	
	if(pName)
		Tab.m_pTabButton = new CTabButton(Context(), this, TabId, pName, IconPath);
	else
		Tab.m_pTabButton = new CTabButton(Context(), this, TabId, *pLocalizableName, IconPath);
	m_pButtonList->Add(Tab.m_pTabButton, Fill);
	if(pTabsStyle)
		Tab.m_pTabButton->SetButtonStyle(pTabsStyle->GetInactiveButtonPath());
	
	if(m_SelectedTab < 0)
		m_SelectedTab = TabId;
}

void CTabs::AddTab(CWidget* pWidget, const char* pName, CAssetPath IconPath)
{
	AddTab(pWidget, pName, NULL, IconPath);
}

void CTabs::AddTab(CWidget* pWidget, const CLocalizableString& LocalizableString, CAssetPath IconPath)
{
	AddTab(pWidget, NULL, &LocalizableString, IconPath);
}

void CTabs::Clear()
{
	m_SelectedTab = -1;
	for(int i=0; i<m_Tabs.size(); i++)
	{
		m_Tabs[i].m_pWidget->Destroy();
	}
	m_Tabs.clear();
	m_pButtonList->Clear();
}

void CTabs::UpdateBoundingSize()
{	
	m_BoundingSizeRect.BSNoConstraint();
	
	const CAsset_GuiTabsStyle* pTabsStyle = AssetsManager()->GetAsset<CAsset_GuiTabsStyle>(m_TabsStylePath);
	const CAsset_GuiBoxStyle* pLayoutStyle = 0;
	const CAsset_GuiBoxStyle* pContentStyle = 0;
	if(pTabsStyle)
	{
		pLayoutStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(pTabsStyle->GetLayoutPath());
		pContentStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(pTabsStyle->GetContentPath());
	}
	
	m_pButtonList->UpdateBoundingSize();
	m_BoundingSizeRect.BSVerticalAdd(m_pButtonList->GetBS());
	
	if(pLayoutStyle)
		m_BoundingSizeRect.BSAddSpacing(0, Context()->ApplyGuiScale(pLayoutStyle->GetSpacing()));
	
	if(m_SelectedTab >= 0)
	{
		m_Tabs[m_SelectedTab].m_pWidget->UpdateBoundingSize();
		
		CRect BSSelectedTab = m_Tabs[m_SelectedTab].m_pWidget->GetBS();
		
		if(pContentStyle)
		{
			BSSelectedTab.BSAddMargin(Context()->ApplyGuiScale(pContentStyle->GetPadding()));
			BSSelectedTab.BSAddMargin(Context()->ApplyGuiScale(pContentStyle->GetMargin()));
		}
		
		m_BoundingSizeRect.BSVerticalAdd(BSSelectedTab);
	}
	
	if(pLayoutStyle)
	{
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pLayoutStyle->GetPadding()));
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pLayoutStyle->GetMargin()));
	}
}

void CTabs::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	CWidget::UpdatePosition(BoundingRect, VisibilityRect);
	
	const CAsset_GuiTabsStyle* pTabsStyle = AssetsManager()->GetAsset<CAsset_GuiTabsStyle>(m_TabsStylePath);
	const CAsset_GuiBoxStyle* pLayoutStyle = 0;
	const CAsset_GuiBoxStyle* pContentStyle = 0;
	if(pTabsStyle)
	{
		pLayoutStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(pTabsStyle->GetLayoutPath());
		pContentStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(pTabsStyle->GetContentPath());
	}
	
	int Spacing = 0;
	CRect TabsRect = m_DrawRect;
	if(pLayoutStyle)
	{
		Spacing = Context()->ApplyGuiScale(pLayoutStyle->GetSpacing());
		TabsRect.RemoveMargin(Context()->ApplyGuiScale(pLayoutStyle->GetPadding()));
		TabsRect.RemoveMargin(Context()->ApplyGuiScale(pLayoutStyle->GetMargin()));
	}
	
	CRect ButtonListRect(
		TabsRect.x,
		TabsRect.y,
		TabsRect.w,
		m_pButtonList->GetBS().minh
	);
	m_pButtonList->UpdatePosition(ButtonListRect, m_VisibilityRect);
	
	m_ClipRect = CRect(
		TabsRect.x,
		ButtonListRect.y + ButtonListRect.h + Spacing,
		TabsRect.w,
		TabsRect.h - ButtonListRect.h - Spacing
	);
	
	CRect ChildRect = m_ClipRect;
	if(pContentStyle)
	{
		ChildRect.RemoveMargin(Context()->ApplyGuiScale(pContentStyle->GetMargin()));			
		ChildRect.RemoveMargin(Context()->ApplyGuiScale(pContentStyle->GetPadding()));
	}
	
	if(m_SelectedTab >= 0)
	{
		m_Tabs[m_SelectedTab].m_pWidget->UpdatePosition(ChildRect, ChildRect);
	}
}

void CTabs::Update(bool ParentEnabled)
{
	const CAsset_GuiTabsStyle* pTabsStyle = AssetsManager()->GetAsset<CAsset_GuiTabsStyle>(m_TabsStylePath);
	if(pTabsStyle)
		m_pButtonList->SetBoxStyle(pTabsStyle->GetButtonListPath());
		
	for(int i=0; i<m_Tabs.size(); i++)
		m_Tabs[i].m_pWidget->Update(ParentEnabled && IsEnabled());
	
	int FirstEnabledTab = -1;
	for(int i=0; i<m_Tabs.size(); i++)
	{
		if(m_Tabs[i].m_pWidget->IsDisabled())
			m_Tabs[i].m_pTabButton->Disable();
		else
		{
			m_Tabs[i].m_pTabButton->Enable();
			if(FirstEnabledTab < 0)
				FirstEnabledTab = i;
		}
	}
	
	if(m_SelectedTab >= 0 && m_SelectedTab < m_Tabs.size() && m_Tabs[m_SelectedTab].m_pWidget->IsDisabled())
		m_SelectedTab = FirstEnabledTab;
	
	if(pTabsStyle)
	{
		for(int i=0; i<m_Tabs.size(); i++)
		{
			if(m_Tabs[i].m_pTabButton)
			{
				if(i == m_SelectedTab)
					m_Tabs[i].m_pTabButton->SetButtonStyle(pTabsStyle->GetActiveButtonPath());
				else
					m_Tabs[i].m_pTabButton->SetButtonStyle(pTabsStyle->GetInactiveButtonPath());
			}
		}
	}
	
	m_pButtonList->Update(ParentEnabled && IsEnabled());
}

void CTabs::Render()
{
	const CAsset_GuiTabsStyle* pTabsStyle = AssetsManager()->GetAsset<CAsset_GuiTabsStyle>(m_TabsStylePath);
	const CAsset_GuiBoxStyle* pLayoutStyle = 0;
	const CAsset_GuiBoxStyle* pContentStyle = 0;
	if(pTabsStyle)
	{
		pLayoutStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(pTabsStyle->GetLayoutPath());
		pContentStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(pTabsStyle->GetContentPath());
	}
	
	ivec2 MousePos = Context()->GetMousePos();
	
	if(pLayoutStyle)
	{
		CRect Rect = m_DrawRect;
		Rect.RemoveMargin(Context()->ApplyGuiScale(pLayoutStyle->GetMargin()));
	
		AssetsRenderer()->DrawGuiRect(&Rect, pLayoutStyle->GetRectPath());
			
		Rect.RemoveMargin(Context()->ApplyGuiScale(pLayoutStyle->GetPadding()));
	}
	
	if(pContentStyle)
	{
		CRect Rect = m_ClipRect;
		Rect.RemoveMargin(Context()->ApplyGuiScale(pContentStyle->GetMargin()));
	
		AssetsRenderer()->DrawGuiRect(&Rect, pContentStyle->GetRectPath());
			
		Rect.RemoveMargin(Context()->ApplyGuiScale(pContentStyle->GetPadding()));
	}
		
	m_pButtonList->Render();
	
	if(m_SelectedTab >= 0)
	{
		m_Tabs[m_SelectedTab].m_pWidget->Render();
	}
}

void CTabs::OnMouseMove()
{
	m_pButtonList->OnMouseMove();
		
	if(m_SelectedTab >= 0)
	{
		m_Tabs[m_SelectedTab].m_pWidget->OnMouseMove();
	}
}

void CTabs::OnButtonClick(int Button)
{
	m_pButtonList->OnButtonClick(Button);
		
	if(m_SelectedTab >= 0)
	{
		m_Tabs[m_SelectedTab].m_pWidget->OnButtonClick(Button);
	}
}

void CTabs::OnButtonRelease(int Button)
{
	m_pButtonList->OnButtonRelease(Button);
		
	if(m_SelectedTab >= 0)
	{
		m_Tabs[m_SelectedTab].m_pWidget->OnButtonRelease(Button);
	}
}

void CTabs::OnInputEvent(const CInput::CEvent& Event)
{
	m_pButtonList->OnInputEvent(Event);
		
	if(m_SelectedTab >= 0)
	{
		m_Tabs[m_SelectedTab].m_pWidget->OnInputEvent(Event);
	}
}

}
