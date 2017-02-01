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

#include "panellayout.h"
	
namespace gui
{

/* ABSTRACT PANEL LAYOUT **********************************************/

CAbstractPanelLayout::CAbstractPanelLayout(CGui *pContext) :
	CWidget(pContext),
	m_BoxStylePath(CAssetPath::Null()),
	m_MouseOver(false)
{
	
}

void CAbstractPanelLayout::Destroy()
{
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		m_Childs[i].m_pWidget->Destroy();
	}
	
	CWidget::Destroy();
}

void CAbstractPanelLayout::Clear()
{
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		m_Childs[i].m_pWidget->Destroy();
	}
	m_Childs.clear();
}

void CAbstractPanelLayout::Add(CWidget* pWidget, int Size)
{
	m_Childs.emplace_back();
	CChild& Child = m_Childs.back();
	Child.m_pWidget = pWidget;
	Child.m_Size = Size;
	Child.m_Fill = (Size < 0);
	Child.m_SliderClicked = false;
}

void CAbstractPanelLayout::Update(bool ParentEnabled)
{
	for(unsigned int i=0; i<m_Childs.size(); i++)
		m_Childs[i].m_pWidget->Update(ParentEnabled && IsEnabled());
}
	
void CAbstractPanelLayout::Render()
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
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
			
		m_Childs[i].m_pWidget->Render();
	}
}

void CAbstractPanelLayout::OnButtonClick(int Button)
{
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
			
		if(m_Childs[i].m_SliderRect.IsInside(Context()->GetMousePos()))
			m_Childs[i].m_SliderClicked = true;
		
		m_Childs[i].m_pWidget->OnButtonClick(Button);
	}
}

void CAbstractPanelLayout::OnButtonRelease(int Button)
{
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
			
		m_Childs[i].m_SliderClicked = false;
		m_Childs[i].m_pWidget->OnButtonRelease(Button);
	}
}

void CAbstractPanelLayout::OnInputEvent(const CInput::CEvent& Event)
{
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
			
		m_Childs[i].m_pWidget->OnInputEvent(Event);
	}
}


/* H PANEL LAYOUT *****************************************************/

CHPanelLayout::CHPanelLayout(CGui *pContext) :
	CAbstractPanelLayout(pContext)
{
	
}

void CHPanelLayout::Update(bool ParentEnabled)
{
	if(m_MouseOver)
		Context()->SetCursor(this, CInput::CURSOR_TRANSLATEX);
	
	CAbstractPanelLayout::Update(ParentEnabled);
}

void CHPanelLayout::UpdateBoundingSize()
{
	int Spacing = 0;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
		Spacing = Context()->ApplyGuiScale(pBoxStyle->GetSpacing());
	
	m_BoundingSizeRect.BSNoConstraint();
	
	bool FirstChild = true;
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
		
		m_Childs[i].m_pWidget->UpdateBoundingSize();
		
		if(FirstChild)
			FirstChild = false;
		else
			m_BoundingSizeRect.BSAddSpacing(Spacing, 0);
			
		m_BoundingSizeRect.BSHorizontalAdd(m_Childs[i].m_pWidget->GetBS());
	}
	
	if(pBoxStyle)
	{
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
	}
}

void CHPanelLayout::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	CWidget::UpdatePosition(BoundingRect, VisibilityRect);
	
	m_ClipRect = m_DrawRect;
	int Spacing = 10;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
	{
		//~ Spacing = Context()->ApplyGuiScale(pBoxStyle->GetSpacing());
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
	}
	
	int AvailableSpace = m_ClipRect.w;
	int NumFill = 0;
	int NumChilds = 0;
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
		
		NumChilds++;
		
		if(m_Childs[i].m_Fill)
			NumFill++;
		else
			AvailableSpace -= max(m_Childs[i].m_Size, m_Childs[i].m_pWidget->GetBS().minw);
	}
	if(NumChilds > 1)
		AvailableSpace -= (NumChilds-1)*Spacing;
	
	int FillSize = 0;
	if(NumFill > 0)
		FillSize = AvailableSpace/NumFill;
	
	if(Localization()->GetWritingDirection() == CLocalization::DIRECTION_RTL)
	{
		int PosX = m_ClipRect.x + m_ClipRect.w;
		for(unsigned int i=0; i<m_Childs.size(); i++)
		{
			if(m_Childs[i].m_pWidget->IsDisabled())
				continue;
				
			int ChildWidth;
			if(m_Childs[i].m_Fill)
				ChildWidth = FillSize;
			else
				ChildWidth = max(m_Childs[i].m_Size, m_Childs[i].m_pWidget->GetBS().minw);
			
			CRect ChildRect(
				PosX - ChildWidth,
				m_ClipRect.y,
				ChildWidth,
				m_ClipRect.h
			);
			m_Childs[i].m_pWidget->UpdatePosition(ChildRect, m_ClipRect);
			m_Childs[i].m_Size = ChildWidth;
			
			PosX -= ChildRect.w;
			
			m_Childs[i].m_SliderRect = CRect(PosX - Spacing, ChildRect.y, Spacing, ChildRect.h);
			
			PosX -= Spacing;
		}
	}
	else
	{
		int PosX = m_ClipRect.x;
		for(unsigned int i=0; i<m_Childs.size(); i++)
		{
			if(m_Childs[i].m_pWidget->IsDisabled())
				continue;
				
			int ChildWidth;
			if(m_Childs[i].m_Fill)
				ChildWidth = FillSize;
			else
				ChildWidth = max(m_Childs[i].m_Size, m_Childs[i].m_pWidget->GetBS().minw);
			
			CRect ChildRect(
				PosX,
				m_ClipRect.y,
				ChildWidth,
				m_ClipRect.h
			);
			m_Childs[i].m_pWidget->UpdatePosition(ChildRect, m_ClipRect);
			m_Childs[i].m_Size = ChildWidth;
			
			PosX += ChildRect.w;
			
			m_Childs[i].m_SliderRect = CRect(PosX, ChildRect.y, Spacing, ChildRect.h);
			
			PosX += Spacing;
		}
	}
}

void CHPanelLayout::OnMouseMove()
{
	m_MouseOver = false;
	
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
			
		if(m_Childs[i].m_SliderClicked && (i+1)<m_Childs.size())
		{
			int Diff = Context()->GetMouseRelPos().x;
			
			if(Localization()->GetWritingDirection() == CLocalization::DIRECTION_RTL)
				Diff = -Diff;
			
			if(m_Childs[i].m_Size >= 0)
			{
				if(m_Childs[i].m_Size + Diff < m_Childs[i].m_pWidget->GetBS().minw)
					Diff = m_Childs[i].m_pWidget->GetBS().minw - m_Childs[i].m_Size;
			}			
			if(m_Childs[i+1].m_Size >= 0)
			{
				if(m_Childs[i+1].m_Size - Diff < m_Childs[i+1].m_pWidget->GetBS().minw)
					Diff = m_Childs[i+1].m_Size - m_Childs[i+1].m_pWidget->GetBS().minw;
			}
			
			if(m_Childs[i].m_Size >= 0)
				m_Childs[i].m_Size += Diff;
			if((i+1)<m_Childs.size() &&  m_Childs[i+1].m_Size >= 0)
				m_Childs[i+1].m_Size -= Diff;
		}
		
		if(m_Childs[i].m_SliderRect.IsInside(Context()->GetMousePos()) || m_Childs[i].m_SliderClicked)
		{
			if(i<m_Childs.size()-1)
				m_MouseOver = true;
		}
		
		m_Childs[i].m_pWidget->OnMouseMove();
	}
}


/* V PANEL LAYOUT *****************************************************/

CVPanelLayout::CVPanelLayout(CGui *pContext) :
	CAbstractPanelLayout(pContext)
{
	
}

void CVPanelLayout::Update(bool ParentEnabled)
{
	if(m_MouseOver)
		Context()->SetCursor(this, CInput::CURSOR_TRANSLATEY);
	
	CAbstractPanelLayout::Update(ParentEnabled);
}

void CVPanelLayout::UpdateBoundingSize()
{
	int Spacing = 0;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
		Spacing = Context()->ApplyGuiScale(pBoxStyle->GetSpacing());
	
	m_BoundingSizeRect.BSNoConstraint();
	
	bool FirstChild = true;
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
		
		m_Childs[i].m_pWidget->UpdateBoundingSize();
		
		if(FirstChild)
			FirstChild = false;
		else
			m_BoundingSizeRect.BSAddSpacing(0, Spacing);
			
		m_BoundingSizeRect.BSVerticalAdd(m_Childs[i].m_pWidget->GetBS());
	}
	
	if(pBoxStyle)
	{
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
	}
}

void CVPanelLayout::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	CWidget::UpdatePosition(BoundingRect, VisibilityRect);
	
	m_ClipRect = m_DrawRect;
	int Spacing = 10;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
	{
		//~ Spacing = Context()->ApplyGuiScale(pBoxStyle->GetSpacing());
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
	}
	
	int AvailableSpace = m_ClipRect.h;
	int NumFill = 0;
	int NumChilds = 0;
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
		
		NumChilds++;
		
		if(m_Childs[i].m_Fill)
			NumFill++;
		else
			AvailableSpace -= max(m_Childs[i].m_Size, m_Childs[i].m_pWidget->GetBS().minw);
	}
	if(NumChilds > 1)
		AvailableSpace -= (NumChilds-1)*Spacing;
	
	int FillSize = 0;
	if(NumFill > 0)
		FillSize = AvailableSpace/NumFill;
	
	int PosY = m_ClipRect.y;
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_pWidget->IsDisabled())
			continue;
		
		int ChildHeight;
		if(m_Childs[i].m_Fill)
			ChildHeight = FillSize;
		else
			ChildHeight = max(m_Childs[i].m_Size, m_Childs[i].m_pWidget->GetBS().minh);
		
		CRect ChildRect(
			m_ClipRect.x,
			PosY,
			m_ClipRect.w,
			ChildHeight
		);
		m_Childs[i].m_pWidget->UpdatePosition(ChildRect, m_ClipRect);
		m_Childs[i].m_Size = ChildHeight;
		
		PosY += ChildRect.h;
		
		m_Childs[i].m_SliderRect = CRect(ChildRect.x, PosY, ChildRect.w, Spacing);
		
		PosY += Spacing;
	}
}

void CVPanelLayout::OnMouseMove()
{
	m_MouseOver = false;
	
	for(unsigned int i=0; i<m_Childs.size(); i++)
	{
		if(m_Childs[i].m_SliderClicked && (i+1)<m_Childs.size())
		{
			int Diff = Context()->GetMouseRelPos().y;
			
			if(m_Childs[i].m_Size >= 0)
			{
				if(m_Childs[i].m_Size + Diff < m_Childs[i].m_pWidget->GetBS().minh)
					Diff = m_Childs[i].m_pWidget->GetBS().minh - m_Childs[i].m_Size;
			}			
			if(m_Childs[i+1].m_Size >= 0)
			{
				if(m_Childs[i+1].m_Size - Diff < m_Childs[i+1].m_pWidget->GetBS().minh)
					Diff = m_Childs[i+1].m_Size - m_Childs[i+1].m_pWidget->GetBS().minh;
			}
			
			if(m_Childs[i].m_Size >= 0)
				m_Childs[i].m_Size += Diff;
			if((i+1)<m_Childs.size() &&  m_Childs[i+1].m_Size >= 0)
				m_Childs[i+1].m_Size -= Diff;
		}
		
		if(m_Childs[i].m_SliderRect.IsInside(Context()->GetMousePos()) || m_Childs[i].m_SliderClicked)
			m_MouseOver = true;
		
		m_Childs[i].m_pWidget->OnMouseMove();
	}
}

}
