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

#include "listlayout.h"
	
namespace gui
{

/* ABSTRACT LIST LAYOUT ***********************************************/

CAbstractListLayout::CAbstractListLayout(CGui* pConfig) :
	CWidget(pConfig),
	m_BoxStylePath(CAssetPath::Null()),
	m_ClipEnabled(false)
{
	
}

void CAbstractListLayout::Destroy()
{
	for(int i=0; i<m_Childs.size(); i++)
	{
		m_Childs[i].m_pWidget->Destroy();
	}
	
	CWidget::Destroy();
}

void CAbstractListLayout::Clear()
{
	for(int i=0; i<m_Childs.size(); i++)
	{
		m_Childs[i].m_pWidget->Destroy();
	}
	m_Childs.clear();
}

void CAbstractListLayout::Add(CWidget* pWidget, bool Fill, int MinSize)
{
	CChild& Child = m_Childs.increment();
	Child.m_pWidget = pWidget;
	Child.m_Fill = Fill;
	Child.m_MinSize = MinSize;
}

void CAbstractListLayout::Update(bool ParentEnabled)
{
	for(int i=0; i<m_Childs.size(); i++)
	{
		m_Childs[i].m_pWidget->Update(ParentEnabled && IsEnabled());
	}
}
	
void CAbstractListLayout::Render()
{	
	//Background
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
	{
		CRect Rect = m_DrawRect;
		Rect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
	
		AssetsRenderer()->DrawGuiRect(&Rect, pBoxStyle->GetRectPath());
			
		Rect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
	}
	
	//Childs
	if(m_ClipEnabled)
		Graphics()->ClipPush(m_ClipRect.x, m_ClipRect.y, m_ClipRect.w, m_ClipRect.h);
	
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
			m_Childs[i].m_pWidget->Render();
	}
		
	if(m_ClipEnabled)
		Graphics()->ClipPop();
}

void CAbstractListLayout::OnMouseMove()
{
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
			m_Childs[i].m_pWidget->OnMouseMove();
	}
}

void CAbstractListLayout::OnButtonClick(int Button)
{
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
			m_Childs[i].m_pWidget->OnButtonClick(Button);
	}
}

void CAbstractListLayout::OnButtonRelease(int Button)
{
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
			m_Childs[i].m_pWidget->OnButtonRelease(Button);
	}
}

void CAbstractListLayout::OnInputEvent(const CInput::CEvent& Event)
{
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
			m_Childs[i].m_pWidget->OnInputEvent(Event);
	}
}

/* H LIST LAYOUT ******************************************************/
	
	//Separator	
CHListLayout::CSeparator::CSeparator(CGui* pConfig) :
	CWidget(pConfig)
{
	
}

void CHListLayout::CSeparator::UpdateBoundingSize()
{
	m_BoundingSizeRect.BSMinimum(5, 5);
}

void CHListLayout::CSeparator::Render()
{
	const CAsset_GuiLineStyle* pLineStyle = AssetsManager()->GetAsset<CAsset_GuiLineStyle>(Context()->GetSeparatorStyle());
	if(pLineStyle)
	{
		ivec2 Pos0(m_DrawRect.x + m_DrawRect.w/2, m_DrawRect.y);
		ivec2 Pos1(m_DrawRect.x + m_DrawRect.w/2, m_DrawRect.y + m_DrawRect.h);
		AssetsRenderer()->DrawGuiLine(Pos0, Pos1, Context()->GetSeparatorStyle());
	}
}

	//List
CHListLayout::CHListLayout(CGui *pConfig) :
	CAbstractListLayout(pConfig)
{
	
}

void CHListLayout::UpdateBoundingSize()
{
	int Spacing = 0;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
		Spacing = Context()->ApplyGuiScale(pBoxStyle->GetSpacing());
	
	m_BoundingSizeRect.BSNoConstraint();
	
	int EnabledChilds = 0;
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
		{
			EnabledChilds++;
			m_Childs[i].m_pWidget->UpdateBoundingSize();
			CRect ChildRect = m_Childs[i].m_pWidget->GetBS();
			ChildRect.BSInnerAdd(CRect(m_Childs[i].m_MinSize, 0, m_Childs[i].m_MinSize, 0));
			m_BoundingSizeRect.BSHorizontalAdd(ChildRect);
		}
	}
	
	if(EnabledChilds > 1)
		m_BoundingSizeRect.BSAddSpacing(Spacing * (m_Childs.size()-1), 0);
	
	if(pBoxStyle)
	{
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
	}
}

void CHListLayout::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	CAbstractListLayout::UpdatePosition(BoundingRect, VisibilityRect);
	
	m_ClipRect = m_DrawRect;
	int Spacing = 0;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
	{
		Spacing = Context()->ApplyGuiScale(pBoxStyle->GetSpacing());
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
	}
	
	int EnabledChilds = 0;
	int NumFill = 0;
	int AvailableSpace = m_ClipRect.w;
	for(int i=0; i<m_Childs.size(); i++)
	{
		EnabledChilds++;
			
		if(m_Childs[i].m_pWidget->IsEnabled())
		{
			if(m_Childs[i].m_Fill)
				NumFill++;
			else
				AvailableSpace -= max(m_Childs[i].m_MinSize, m_Childs[i].m_pWidget->GetBS().minw);
		}
	}
	if(EnabledChilds > 1)
		AvailableSpace -= Spacing * (EnabledChilds-1);
	
	int FillSize = 0;
	if(NumFill > 0)
		FillSize = AvailableSpace/NumFill;
	
	if(Localization()->GetWritingDirection() == CLocalization::DIRECTION_RTL)
	{
		int PosX = m_ClipRect.x + m_ClipRect.w;
		for(int i=0; i<m_Childs.size(); i++)
		{
			if(m_Childs[i].m_pWidget->IsEnabled())
			{
				int ChildWidth = (m_Childs[i].m_Fill ? FillSize : max(m_Childs[i].m_MinSize, m_Childs[i].m_pWidget->GetBS().minw));
				CRect ChildRect(
					PosX - ChildWidth,
					m_ClipRect.y,
					ChildWidth,
					m_ClipRect.h
				);
				m_Childs[i].m_pWidget->UpdatePosition(ChildRect, m_ClipRect);
				PosX -= ChildRect.w + Spacing;
			}
		}
	}
	else
	{
		int PosX = m_ClipRect.x;
		for(int i=0; i<m_Childs.size(); i++)
		{
			if(m_Childs[i].m_pWidget->IsEnabled())
			{
				int ChildWidth = (m_Childs[i].m_Fill ? FillSize : max(m_Childs[i].m_MinSize, m_Childs[i].m_pWidget->GetBS().minw));
				CRect ChildRect(
					PosX,
					m_ClipRect.y,
					ChildWidth,
					m_ClipRect.h
				);
				m_Childs[i].m_pWidget->UpdatePosition(ChildRect, m_ClipRect);
				PosX += ChildRect.w + Spacing;
			}
		}
	}
}

void CHListLayout::AddSeparator()
{
	Add(new CHListLayout::CSeparator(Context()), false);
}

/* V LIST LAYOUT ******************************************************/

	//Separator
CVListLayout::CSeparator::CSeparator(CGui* pConfig) :
	CWidget(pConfig)
{
	
}

void CVListLayout::CSeparator::UpdateBoundingSize()
{
	m_BoundingSizeRect.BSMinimum(5, 5);
}

void CVListLayout::CSeparator::Render()
{
	const CAsset_GuiLineStyle* pLineStyle = AssetsManager()->GetAsset<CAsset_GuiLineStyle>(Context()->GetSeparatorStyle());
	if(pLineStyle)
	{
		ivec2 Pos0(m_DrawRect.x, m_DrawRect.y + m_DrawRect.h/2);
		ivec2 Pos1(m_DrawRect.x + m_DrawRect.w, m_DrawRect.y + m_DrawRect.h/2);
		AssetsRenderer()->DrawGuiLine(Pos0, Pos1, Context()->GetSeparatorStyle());
	}
}

	//List
CVListLayout::CVListLayout(CGui *pContext) :
	CAbstractListLayout(pContext)
{
	
}

void CVListLayout::UpdateBoundingSize()
{

	int Spacing = 0;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
		Spacing = Context()->ApplyGuiScale(pBoxStyle->GetSpacing());
	
	m_BoundingSizeRect.BSNoConstraint();
	
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
		{
			m_Childs[i].m_pWidget->UpdateBoundingSize();
			m_BoundingSizeRect.BSVerticalAdd(m_Childs[i].m_pWidget->GetBS());
		}
	}
	
	if(m_Childs.size() > 1)
		m_BoundingSizeRect.BSAddSpacing(0, Spacing * (m_Childs.size()-1));
	
	if(pBoxStyle)
	{
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
	}
}

void CVListLayout::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	CAbstractListLayout::UpdatePosition(BoundingRect, VisibilityRect);
	
	m_ClipRect = m_DrawRect;
	int Spacing = 0;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
	{
		Spacing = Context()->ApplyGuiScale(pBoxStyle->GetSpacing());
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
	}
	
	int NumFill = 0;
	int AvailableSpace = m_ClipRect.h;
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
		{
			if(m_Childs[i].m_Fill)
				NumFill++;
			else
				AvailableSpace -= m_Childs[i].m_pWidget->GetBS().minh;
			
			if(i != 0)
				AvailableSpace -= Spacing;
		}
	}
	int FillSize = 0;
	if(NumFill > 0)
		FillSize = AvailableSpace/NumFill;
	
	//Compute total size
	m_ChildrenLength = 0;
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
		{
			m_ChildrenLength += (m_Childs[i].m_Fill ? FillSize : m_Childs[i].m_pWidget->GetBS().minh);
			
			if(i != 0)
				m_ChildrenLength += Spacing;
		}
	}
	
	//Update children position
	int PosY = m_ClipRect.y;
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
		{
			CRect ChildRect(
				m_ClipRect.x,
				PosY,
				m_ClipRect.w,
				(m_Childs[i].m_Fill ? FillSize : m_Childs[i].m_pWidget->GetBS().minh)
			);
			m_Childs[i].m_pWidget->UpdatePosition(ChildRect, m_ClipRect);
			PosY += ChildRect.h + Spacing;
		}
	}
}

void CVListLayout::AddSeparator()
{
	Add(new CVListLayout::CSeparator(Context()), false);
}

/* V SCROLLLAYOT ******************************************************/

	//List
CVScrollLayout::CVScrollLayout(CGui *pContext) :
	CVListLayout(pContext),
	m_ShowScrollBar(false),
	m_pScrollBar(NULL)
{
	m_ClipEnabled = true;
	m_pScrollBar = new CVScrollBar(pContext);
}

void CVScrollLayout::Destroy()
{
	if(m_pScrollBar)
		m_pScrollBar->Destroy();
	
	CVListLayout::Destroy();
}

void CVScrollLayout::UpdateBoundingSize()
{
	m_BoundingSizeRect.BSNoConstraint();
	
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
			m_Childs[i].m_pWidget->UpdateBoundingSize();
	}
	
	int Spacing = 0;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
	{
		Spacing = Context()->ApplyGuiScale(pBoxStyle->GetSpacing());
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
	}
	
	if(m_ShowScrollBar)
	{
		m_pScrollBar->UpdateBoundingSize();
		
		m_BoundingSizeRect.BSAddSpacing(Spacing, 0);
		m_BoundingSizeRect.BSHorizontalAdd(m_pScrollBar->GetBS());
	}
}

void CVScrollLayout::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	CAbstractListLayout::UpdatePosition(BoundingRect, VisibilityRect);
	
	m_ClipRect = m_DrawRect;
	int Spacing = 0;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
	{
		Spacing = Context()->ApplyGuiScale(pBoxStyle->GetSpacing());
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
	}
	
	int NumFill = 0;
	int AvailableSpace = m_ClipRect.h;
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
		{
			if(m_Childs[i].m_Fill)
				NumFill++;
			else
				AvailableSpace -= m_Childs[i].m_pWidget->GetBS().minh;
		}
	}
	int FillSize = 0;
	if(NumFill > 0)
		FillSize = AvailableSpace/NumFill;
	
	//Compute total size
	m_ChildrenLength = 0;
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
		{
			m_ChildrenLength += (m_Childs[i].m_Fill ? FillSize : m_Childs[i].m_pWidget->GetBS().minh);
			
			if(i != 0)
				m_ChildrenLength += Spacing;
		}
	}
	
	//Check if scrollbar is needed
	m_ShowScrollBar = false;
	if(m_pScrollBar && m_ChildrenLength > m_ClipRect.h)
	{
		m_pScrollBar->SetContentInformations(m_ChildrenLength, m_ClipRect.h);
		
		m_ClipRect.w -= m_pScrollBar->GetBS().minw;
		m_ClipRect.w -= Spacing;
	
		CRect ScrollBarRect = m_ClipRect;
		ScrollBarRect.w = m_pScrollBar->GetBS().minw;
		if(Localization()->GetWritingDirection() == CLocalization::DIRECTION_RTL)
		{
			ScrollBarRect.x = m_ClipRect.x;
			m_ClipRect.x += ScrollBarRect.w + Spacing;
		}
		else
			ScrollBarRect.x = m_ClipRect.x + m_ClipRect.w + Spacing;
		
		m_pScrollBar->UpdatePosition(ScrollBarRect, m_VisibilityRect);
		
		m_ShowScrollBar = true;
	}
	
	//Update children position
	int PosY = m_ClipRect.y;
	if(m_ShowScrollBar)
	{
		PosY -= m_pScrollBar->GetContentPos();
	}
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsEnabled())
		{
			CRect ChildRect(
				m_ClipRect.x,
				PosY,
				m_ClipRect.w,
				(m_Childs[i].m_Fill ? FillSize : m_Childs[i].m_pWidget->GetBS().minh)
			);
			m_Childs[i].m_pWidget->UpdatePosition(ChildRect, m_ClipRect);
			PosY += ChildRect.h + Spacing;
		}
	}
}

void CVScrollLayout::Update(bool ParentEnabled)
{
	CVListLayout::Update(ParentEnabled);
	
	if(m_pScrollBar && m_ShowScrollBar)
		m_pScrollBar->Update(ParentEnabled && IsEnabled());
}
	
void CVScrollLayout::Render()
{
	CVListLayout::Render();
	
	if(m_pScrollBar && m_ShowScrollBar)
		m_pScrollBar->Render();
}

void CVScrollLayout::OnMouseMove()
{
	if(m_pScrollBar && m_ShowScrollBar)
		m_pScrollBar->OnMouseMove();
		
	CVListLayout::OnMouseMove();
}

void CVScrollLayout::OnButtonClick(int Button)
{
	if(m_ShowScrollBar && m_VisibilityRect.IsInside(Context()->GetMousePos()))
	{
		if(Button == KEY_MOUSE_WHEEL_UP)
		{
			m_pScrollBar->OnWheelUp();
			return;
		}
		else if(Button == KEY_MOUSE_WHEEL_DOWN)
		{
			m_pScrollBar->OnWheelDown();
			return;
		}
	}
	
	if(m_pScrollBar && m_ShowScrollBar)
		m_pScrollBar->OnButtonClick(Button);
	
	CVListLayout::OnButtonClick(Button);
}

void CVScrollLayout::OnButtonRelease(int Button)
{
	if(m_pScrollBar && m_ShowScrollBar)
		m_pScrollBar->OnButtonRelease(Button);
		
	CVListLayout::OnButtonRelease(Button);
}

void CVScrollLayout::OnInputEvent(const CInput::CEvent& Event)
{
	if(m_pScrollBar && m_ShowScrollBar)
		m_pScrollBar->OnInputEvent(Event);
		
	CVListLayout::OnInputEvent(Event);
}

}
