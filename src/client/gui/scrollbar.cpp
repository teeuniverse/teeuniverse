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

#include "scrollbar.h"
	
namespace gui
{

/* ABSTRACT SCROLLBAR *************************************************/

CAbstractScrollBar::CAbstractScrollBar(CGui *pContext) :
	CWidget(pContext),
	m_ViewSize(0),
	m_ContentSize(0),
	m_ContentPos(0.0f),
	m_Clicked(false),
	m_ClickShift(0)
{
	SetScrollbarStyle(Context()->GetScrollbarStyle());
}

void CAbstractScrollBar::UpdateBoundingSize()
{
	m_BoundingSizeRect.BSNoConstraint();
	
	const CAsset_GuiScrollbarStyle* pScrollbarStyle = AssetsManager()->GetAsset<CAsset_GuiScrollbarStyle>(m_ScrollbarStylePath);
	if(pScrollbarStyle)
	{
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pScrollbarStyle->GetPadding()));
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pScrollbarStyle->GetMargin()));
	}
}

void CAbstractScrollBar::SetContentInformations(int ContentSize, int ViewSize)
{
	m_ContentSize = ContentSize;
	m_ViewSize = ViewSize;
	
	if(m_ContentPos < 0 || m_ContentSize < m_ViewSize)
		m_ContentPos = 0;
	else if(m_ContentPos + m_ViewSize > m_ContentSize)
		m_ContentPos = m_ContentSize - m_ViewSize;
}

void CAbstractScrollBar::OnButtonRelease(int Button)
{
	if(Button != KEY_MOUSE_1)
		return;
	
	m_Clicked = false;
}

void CAbstractScrollBar::OnWheelDown()
{
	m_ContentPos += 100.0f;
	
	if(m_ContentPos < 0)
		m_ContentPos = 0;
	if(m_ContentPos + m_ViewSize > m_ContentSize)
		m_ContentPos = m_ContentSize - m_ViewSize;
}

void CAbstractScrollBar::OnWheelUp()
{
	m_ContentPos -= 100.0f;
	
	if(m_ContentPos < 0)
		m_ContentPos = 0;
	if(m_ContentPos + m_ViewSize > m_ContentSize)
		m_ContentPos = m_ContentSize - m_ViewSize;
}

int CAbstractScrollBar::GetContentPos() const
{
	return m_ContentPos;
}

/* H SCROLLBAR *************************************************/

CHScrollBar::CHScrollBar(CGui *pContext) :
	CAbstractScrollBar(pContext)
{
	
}

void CHScrollBar::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	CAbstractScrollBar::UpdatePosition(BoundingRect, VisibilityRect);
	
	int Padding = 0;
	m_ClipRect = m_DrawRect;
	const CAsset_GuiScrollbarStyle* pScrollbarStyle = AssetsManager()->GetAsset<CAsset_GuiScrollbarStyle>(m_ScrollbarStylePath);
	if(pScrollbarStyle)
	{
		Padding = Context()->ApplyGuiScale(pScrollbarStyle->GetPadding());
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pScrollbarStyle->GetMargin()));
		m_ClipRect.RemoveMargin(Padding);
	}
	
	m_ClipRect.y -= Padding;
	m_ClipRect.h += 2*Padding;
	
	int SliderPos = (m_ClipRect.w * m_ContentPos) / m_ContentSize;
	int SliderSize = (m_ClipRect.w * m_ViewSize) / m_ContentSize;
	
	m_SliderRect = CRect(
		m_ClipRect.x + SliderPos,
		m_ClipRect.y,
		SliderSize,
		m_ClipRect.h
	);
}
	
void CHScrollBar::Render()
{
	const CAsset_GuiScrollbarStyle* pScrollbarStyle = AssetsManager()->GetAsset<CAsset_GuiScrollbarStyle>(m_ScrollbarStylePath);
	if(pScrollbarStyle)
	{
		CRect Rect = m_DrawRect;
		Rect.RemoveMargin(Context()->ApplyGuiScale(pScrollbarStyle->GetMargin()));
	
		AssetsRenderer()->DrawGuiRect(&Rect, pScrollbarStyle->GetRectPath());
			
		Rect.RemoveMargin(Context()->ApplyGuiScale(pScrollbarStyle->GetPadding()));
	
		ivec2 MousePos = Context()->GetMousePos();
		
		ivec2 RailPos0(m_ClipRect.x, m_ClipRect.y + m_ClipRect.h/2);
		ivec2 RailPos1(m_ClipRect.x + m_ClipRect.w, m_ClipRect.y + m_ClipRect.h/2);
		AssetsRenderer()->DrawGuiLine(RailPos0, RailPos1, pScrollbarStyle->GetDefaultRailPath());
		
		ivec2 SliderPos0(m_SliderRect.x, m_SliderRect.y + m_SliderRect.h/2);
		ivec2 SliderPos1(m_SliderRect.x + m_SliderRect.w, m_SliderRect.y + m_SliderRect.h/2);
		if(m_SliderRect.IsInside(MousePos.x, MousePos.y))
			AssetsRenderer()->DrawGuiLine(SliderPos0, SliderPos1, pScrollbarStyle->GetMouseOverSliderPath());
		else
			AssetsRenderer()->DrawGuiLine(SliderPos0, SliderPos1, pScrollbarStyle->GetDefaultSliderPath());
	}
}

void CHScrollBar::OnButtonClick(int Button)
{
	if(Button != KEY_MOUSE_1)
		return;
	
	if(m_SliderRect.IsInside(Context()->GetMousePos()))
	{
		m_Clicked = true;
		m_ClickShift = Context()->GetMousePos().x - m_SliderRect.x;
	}
}

void CHScrollBar::OnMouseMove()
{
	if(m_Clicked)
	{
		int SliderPos = Context()->GetMousePos().x - m_SliderRect.x + m_ClickShift;
		m_ContentPos = (SliderPos * m_ContentSize)/m_ClipRect.w;
		if(m_ContentPos < 0)
			m_ContentPos = 0;
		if(m_ContentPos + m_ViewSize > m_ContentSize)
			m_ContentPos = m_ContentSize - m_ViewSize;
	}
}

/* V SCROLLBAR *************************************************/

CVScrollBar::CVScrollBar(CGui *pContext) :
	CAbstractScrollBar(pContext)
{
	
}

void CVScrollBar::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	CAbstractScrollBar::UpdatePosition(BoundingRect, VisibilityRect);
	
	int Padding = 0;
	m_ClipRect = m_DrawRect;
	const CAsset_GuiScrollbarStyle* pScrollbarStyle = AssetsManager()->GetAsset<CAsset_GuiScrollbarStyle>(m_ScrollbarStylePath);
	if(pScrollbarStyle)
	{
		Padding = Context()->ApplyGuiScale(pScrollbarStyle->GetPadding());
		
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pScrollbarStyle->GetMargin()));
		m_ClipRect.RemoveMargin(Padding);
	}
	
	int SliderPos = (m_ClipRect.h * m_ContentPos) / m_ContentSize;
	int SliderSize = (m_ClipRect.h * m_ViewSize) / m_ContentSize;
	
	m_ClipRect.x -= Padding;
	m_ClipRect.w += 2*Padding;
	
	m_SliderRect = CRect(
		m_ClipRect.x,
		m_ClipRect.y + SliderPos,
		m_ClipRect.w,
		SliderSize
	);
}
	
void CVScrollBar::Render()
{
	const CAsset_GuiScrollbarStyle* pScrollbarStyle = AssetsManager()->GetAsset<CAsset_GuiScrollbarStyle>(m_ScrollbarStylePath);
	if(pScrollbarStyle)
	{
		CRect Rect = m_DrawRect;
		Rect.RemoveMargin(Context()->ApplyGuiScale(pScrollbarStyle->GetMargin()));
		AssetsRenderer()->DrawGuiRect(&Rect, pScrollbarStyle->GetRectPath());
		Rect.RemoveMargin(Context()->ApplyGuiScale(pScrollbarStyle->GetPadding()));
	
		ivec2 MousePos = Context()->GetMousePos();
		
		ivec2 RailPos0(m_ClipRect.x + m_ClipRect.w/2, m_ClipRect.y);
		ivec2 RailPos1(m_ClipRect.x + m_ClipRect.w/2, m_ClipRect.y + m_ClipRect.h);
		AssetsRenderer()->DrawGuiLine(RailPos0, RailPos1, pScrollbarStyle->GetDefaultRailPath());
		
		ivec2 SliderPos0(m_SliderRect.x + m_SliderRect.w/2, m_SliderRect.y);
		ivec2 SliderPos1(m_SliderRect.x + m_SliderRect.w/2, m_SliderRect.y + m_SliderRect.h);
		if(m_SliderRect.IsInside(MousePos.x, MousePos.y))
			AssetsRenderer()->DrawGuiLine(SliderPos0, SliderPos1, pScrollbarStyle->GetMouseOverSliderPath());
		else
			AssetsRenderer()->DrawGuiLine(SliderPos0, SliderPos1, pScrollbarStyle->GetDefaultSliderPath());
	}
}

void CVScrollBar::OnButtonClick(int Button)
{
	if(Button != KEY_MOUSE_1)
		return;
	
	if(m_SliderRect.IsInside(Context()->GetMousePos()))
	{
		m_Clicked = true;
	}
}

void CVScrollBar::OnMouseMove()
{
	if(m_Clicked)
	{
		int SliderPos = m_SliderRect.y - m_ClipRect.y + Context()->GetMouseRelPos().y;
		m_ContentPos = static_cast<float>(SliderPos * m_ContentSize)/m_ClipRect.h;
		if(m_ContentPos < 0)
			m_ContentPos = 0;
		if(m_ContentPos + m_ViewSize > m_ContentSize)
			m_ContentPos = m_ContentSize - m_ViewSize;
	}
}

}
