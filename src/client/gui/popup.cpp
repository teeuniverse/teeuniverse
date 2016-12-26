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

#include "popup.h"
	
namespace gui
{

CPopup::CPopup(CGui* pContext, const CRect& CreatorRect, int Width, int Height, int Alignment) :
	CWidget(pContext),
	m_Level(LEVEL_LOWEST),
	m_pChild(NULL),
	m_IsClosed(false),
	m_CreatorRect(CreatorRect),
	m_BoxStylePath(Context()->GetPopupStyle()),
	m_Width(Width),
	m_Height(Height),
	m_Alignment(Alignment)
{

}

void CPopup::Destroy()
{
	if(m_pChild)
		m_pChild->Destroy();
	
	CWidget::Destroy();
}

void CPopup::Clear()
{
	if(m_pChild)
	{
		m_pChild->Destroy();
		m_pChild = 0;
		m_ChildWidth = 0;
		m_ChildHeight = 0;
	}
}

void CPopup::UpdateBoundingSize()
{
	if(m_pChild)
		m_pChild->UpdateBoundingSize();
	
	if(m_Height < 0)
	{
		m_BoundingSizeRect.BSNoConstraint();
		m_BoundingSizeRect.BSAddSpacing(m_Width, m_pChild->GetBS().minh);
	}
	else
		m_BoundingSizeRect.BSFixed(m_Width, m_Height);
}

void CPopup::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	int Width = m_Width;
	if(Width < 0)
		Width = m_BoundingSizeRect.minw;
		
	int Height = m_Height;
	if(Height < 0)
		Height = m_BoundingSizeRect.minh;
		
	if(m_Alignment == ALIGNMENT_INNER)
	{
		m_DrawRect.x = m_CreatorRect.x + m_CreatorRect.w/2 - Width/2;
		m_DrawRect.y = m_CreatorRect.y + m_CreatorRect.h/2 - Height/2;
		
		m_DrawRect.w = Width;
		m_DrawRect.h = Height;
	}
	else if(m_Alignment == ALIGNMENT_SIDE)
	{
		int TryRight = m_CreatorRect.x;
		int TryLeft = m_CreatorRect.x - Width;
		int TryTop = m_CreatorRect.y + m_CreatorRect.h - Height;
		int TryBottom = m_CreatorRect.y;
		
		if((m_CreatorRect.x + m_CreatorRect.w/2) > (BoundingRect.x + BoundingRect.w/2))
		{
			if(TryRight + Width < BoundingRect.x + BoundingRect.w)
				m_DrawRect.x = TryRight;
			else
				m_DrawRect.x = TryLeft;
		}
		else
		{
			if(TryLeft >= BoundingRect.x)
				m_DrawRect.x = TryLeft;
			else
				m_DrawRect.x = TryRight;
		}
			
		if(TryBottom + Height < BoundingRect.y + BoundingRect.h)
			m_DrawRect.y = TryBottom;
		else
			m_DrawRect.y = TryTop;
	
		m_DrawRect.w = Width;
		m_DrawRect.h = Height;
	}
	else
	{
		m_DrawRect.x = m_CreatorRect.x;
		m_DrawRect.w = Width;
		m_DrawRect.h = Height;
		
		if(m_CreatorRect.y + m_CreatorRect.h + Height > BoundingRect.h)
			m_DrawRect.y = m_CreatorRect.y - Height;
		else
			m_DrawRect.y = m_CreatorRect.y + m_CreatorRect.h;
	}
	
	m_VisibilityRect = m_DrawRect.Intersection(VisibilityRect);
	
	if(m_pChild)
	{
		CRect ContentRect = m_DrawRect;
		const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
		if(pBoxStyle)
		{
			ContentRect.RemoveMargin(pBoxStyle->GetMargin());
			ContentRect.RemoveMargin(pBoxStyle->GetPadding());
		}
		m_pChild->UpdatePosition(ContentRect, m_VisibilityRect);
	}
}

void CPopup::Update(bool ParentEnabled)
{
	if(m_pChild)
		m_pChild->Update(ParentEnabled && IsEnabled());
}
	
void CPopup::Render()
{
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
	{
		CRect Rect = m_DrawRect;
		Rect.RemoveMargin(pBoxStyle->GetMargin());
	
		AssetsRenderer()->DrawGuiRect(&Rect, pBoxStyle->GetRectPath());
			
		Rect.RemoveMargin(pBoxStyle->GetPadding());
	}
	
	if(m_pChild)
		m_pChild->Render();
}

void CPopup::Add(CWidget* pWidget)
{
	if(m_pChild)
		m_pChild->Destroy();
	
	m_pChild = pWidget;
}

void CPopup::OnMouseMove()
{
	if(m_pChild)
	{
		m_pChild->OnMouseMove();
	}
}

void CPopup::OnButtonClick(int Button)
{
	if(m_DrawRect.IsInside(Context()->GetMousePos()))
	{
		if(m_pChild)
			m_pChild->OnButtonClick(Button);
	}
	else if(Button == KEY_MOUSE_1)
		Close();
}

void CPopup::OnButtonRelease(int Button)
{
	if(m_pChild)
	{
		m_pChild->OnButtonRelease(Button);
	}
}

void CPopup::OnInputEvent(const CInput::CEvent& Event)
{
	if(m_pChild)
	{
		m_pChild->OnInputEvent(Event);
	}
}

void CPopup::Close()
{
	m_IsClosed = true;
}

bool CPopup::IsClosed()
{
	return m_IsClosed;
}

}
