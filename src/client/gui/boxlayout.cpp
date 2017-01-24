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

#include "boxlayout.h"
	
namespace gui
{

/* BOX LAYOUT *********************************************************/

CBoxLayout::CBoxLayout(CGui *pContext) :
	CWidget(pContext),
	m_BoxStylePath(CAssetPath::Null()),
	m_ShowScrollBar(false),
	m_pScrollBar(0)
{
	m_pScrollBar = new CVScrollBar(pContext);
}

void CBoxLayout::Destroy()
{
	for(int i=0; i<m_Childs.size(); i++)
	{
		m_Childs[i].m_pWidget->Destroy();
	}
	
	if(m_pScrollBar)
		m_pScrollBar->Destroy();
	
	CWidget::Destroy();
}

void CBoxLayout::Clear()
{
	for(int i=0; i<m_Childs.size(); i++)
	{
		m_Childs[i].m_pWidget->Destroy();
	}
	m_Childs.clear();
}

void CBoxLayout::Add(CWidget* pWidget, bool Filling)
{
	CChild& Child = m_Childs.increment();
	Child.m_pWidget = pWidget;
	Child.m_Filling = Filling;
}

void CBoxLayout::Update(bool ParentEnabled)
{
	for(int i=0; i<m_Childs.size(); i++)
		m_Childs[i].m_pWidget->Update(ParentEnabled && IsEnabled());
	
	if(m_pScrollBar && m_ShowScrollBar)
		m_pScrollBar->Update(ParentEnabled && IsEnabled());
}

void CBoxLayout::UpdateBoundingSize()
{
	m_BoundingSizeRect.BSNoConstraint();
	
	for(int i=0; i<m_Childs.size(); i++)
	{
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

void CBoxLayout::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	gui::CWidget::UpdatePosition(BoundingRect, VisibilityRect);
	
	m_ClipRect = m_DrawRect;
	int Spacing = 0;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
	{
		Spacing = Context()->ApplyGuiScale(pBoxStyle->GetSpacing());
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
	}
	
	//First pass: check the size
	int LineWidth = 0;
	int LineHeight = 0;
	m_ContentHeight = 0;
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
		
		if(m_Childs[i].m_Filling)
		{
			m_ContentHeight += LineHeight + m_Childs[i].m_pWidget->GetBS().minh;
			LineWidth = 0;
			LineHeight = 0;
		}
		else
		{
			if(LineWidth + m_Childs[i].m_pWidget->GetBS().minw > m_ClipRect.w)
			{
				m_ContentHeight += LineHeight;
				LineWidth = 0;
				LineHeight = 0;
			}
			
			LineWidth += m_Childs[i].m_pWidget->GetBS().minw;
			LineHeight = max(LineHeight, m_Childs[i].m_pWidget->GetBS().minh);
		}
	}
	m_ContentHeight += LineHeight;
	
	//Check if scrollbar is needed
	m_ShowScrollBar = false;
	CRect ScrollBarRect = m_ClipRect;
	if(m_pScrollBar && m_ContentHeight > m_ClipRect.h)
	{
		m_ShowScrollBar = true;
		
		m_ClipRect.w -= m_pScrollBar->GetBS().minw;
		m_ClipRect.w -= Spacing;
		ScrollBarRect.w = m_pScrollBar->GetBS().minw;
		if(Localization()->GetWritingDirection() == CLocalization::DIRECTION_RTL)
		{
			ScrollBarRect.x = m_ClipRect.x;
			m_ClipRect.x += ScrollBarRect.w + Spacing;
		}
		else
			ScrollBarRect.x = m_ClipRect.x + m_ClipRect.w + Spacing;
	}
	
	//Second pass: affect the effective size of each child
	int PosY = m_ClipRect.y - m_pScrollBar->GetContentPos();
	LineWidth = 0;
	LineHeight = 0;
	m_ContentHeight = 0;
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
		
		if(m_Childs[i].m_Filling)
		{
			PosY += LineHeight;
			m_ContentHeight += LineHeight;
		
			CRect ChildRect(
				m_ClipRect.x,
				PosY,
				m_ClipRect.w,
				m_Childs[i].m_pWidget->GetBS().minh
			);
			m_Childs[i].m_pWidget->UpdatePosition(ChildRect, m_ClipRect);
			
			PosY += m_Childs[i].m_pWidget->GetBS().minh;
			m_ContentHeight += m_Childs[i].m_pWidget->GetBS().minh;
			LineWidth = 0;
			LineHeight = 0;
		}
		else
		{
			if(LineWidth + m_Childs[i].m_pWidget->GetBS().minw > m_ClipRect.w)
			{
				PosY += LineHeight;
				m_ContentHeight += LineHeight;
				LineWidth = 0;
				LineHeight = 0;
			}
			
			if(Localization()->GetWritingDirection() == CLocalization::DIRECTION_RTL)
			{
				CRect ChildRect(
					m_ClipRect.x + m_ClipRect.w - LineWidth - m_Childs[i].m_pWidget->GetBS().minw,
					PosY,
					m_Childs[i].m_pWidget->GetBS().minw,
					m_Childs[i].m_pWidget->GetBS().minh
				);
				
				m_Childs[i].m_pWidget->UpdatePosition(ChildRect, m_ClipRect);
			}
			else
			{
				CRect ChildRect(
					m_ClipRect.x + LineWidth,
					PosY,
					m_Childs[i].m_pWidget->GetBS().minw,
					m_Childs[i].m_pWidget->GetBS().minh
				);
				
				m_Childs[i].m_pWidget->UpdatePosition(ChildRect, m_ClipRect);
			}
		
			LineWidth += m_Childs[i].m_pWidget->GetBS().minw;
			LineHeight = max(LineHeight, m_Childs[i].m_pWidget->GetBS().minh);
		}
	}
	m_ContentHeight += LineHeight;
	
	if(m_pScrollBar)
	{
		m_pScrollBar->SetContentInformations(m_ContentHeight, m_ClipRect.h);
		
		if(m_ShowScrollBar)
			m_pScrollBar->UpdatePosition(ScrollBarRect, m_VisibilityRect);
	}
}
	
void CBoxLayout::Render()
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
	Graphics()->ClipPush(m_ClipRect.x, m_ClipRect.y, m_ClipRect.w, m_ClipRect.h);
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
		
		m_Childs[i].m_pWidget->Render();
	}
	Graphics()->ClipPop();
	
	if(m_pScrollBar && m_ShowScrollBar)
		m_pScrollBar->Render();
}

void CBoxLayout::OnMouseMove()
{
	if(m_pScrollBar && m_ShowScrollBar)
		m_pScrollBar->OnMouseMove();
		
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
		
		m_Childs[i].m_pWidget->OnMouseMove();
	}
}

void CBoxLayout::OnButtonClick(int Button)
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
	
	if(m_ClipRect.IsInside(Context()->GetMousePos()))
	{
		for(int i=0; i<m_Childs.size(); i++)
		{
			if(m_Childs[i].m_pWidget->IsDisabled())
				continue;
			
			m_Childs[i].m_pWidget->OnButtonClick(Button);
		}
	}
}

void CBoxLayout::OnButtonRelease(int Button)
{
	if(m_pScrollBar && m_ShowScrollBar)
		m_pScrollBar->OnButtonRelease(Button);
		
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
		
		m_Childs[i].m_pWidget->OnButtonRelease(Button);
	}
}

void CBoxLayout::OnInputEvent(const CInput::CEvent& Event)
{
	if(m_pScrollBar && m_ShowScrollBar)
		m_pScrollBar->OnInputEvent(Event);
		
	for(int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
		
		m_Childs[i].m_pWidget->OnInputEvent(Event);
	}
}

}
