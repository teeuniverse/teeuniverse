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

#include "expand.h"
	
namespace gui
{

CExpand::CExpand(CGui* pContext) :
	CWidget(pContext),
	m_pTitle(0),
	m_Expanded(true)
{
	
}

void CExpand::Destroy()
{
	for(int i=0; i<m_Childs.size(); i++)
		m_Childs[i]->Destroy();
	
	if(m_pTitle)
		m_pTitle->Destroy();
	
	CWidget::Destroy();
}

void CExpand::Clear()
{
	for(int i=0; i<m_Childs.size(); i++)
	{
		m_Childs[i]->Destroy();
	}
	m_Childs.clear();
}

void CExpand::UpdateBoundingSize()
{	
	m_BoundingSizeRect.BSNoConstraint();
	
	int Spacing = 0;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
		Spacing = Context()->ApplyGuiScale(pBoxStyle->GetSpacing());
	
	if(m_pTitle)
	{
		m_pTitle->UpdateBoundingSize();
		m_BoundingSizeRect.BSVerticalAdd(m_pTitle->GetBS());
	}
	
	if(m_Expanded)
	{
		for(int i=0; i<m_Childs.size(); i++)
		{
			m_Childs[i]->UpdateBoundingSize();
			
			if(m_pTitle || i>0)
				m_BoundingSizeRect.BSAddSpacing(0, Spacing);
			
			m_BoundingSizeRect.BSVerticalAdd(m_Childs[i]->GetBS());
		}
	}
	
	if(pBoxStyle)
	{
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
	}
}

void CExpand::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	CWidget::UpdatePosition(BoundingRect, VisibilityRect);
	
	int Spacing = 0;
	CRect ContentRect = m_DrawRect;
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
	{
		Spacing = pBoxStyle->GetSpacing();
		ContentRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetPadding()));
		ContentRect.RemoveMargin(Context()->ApplyGuiScale(pBoxStyle->GetMargin()));
	}
	
	int PosY = ContentRect.y;
	if(m_pTitle)
	{
		CRect TitleRect(
			ContentRect.x,
			PosY,
			ContentRect.w,
			m_pTitle->GetBS().minh
		);
		m_pTitle->UpdatePosition(TitleRect, m_VisibilityRect);
		PosY += TitleRect.h;
	}
	
	if(m_Expanded)
	{
		for(int i=0; i<m_Childs.size(); i++)
		{
			if(m_pTitle || i>0)
				PosY += Spacing;
			
			CRect ChildRect(
				ContentRect.x,
				PosY,
				ContentRect.w - GetShift(),
				m_Childs[i]->GetBS().minh
			);
			
			if(Localization()->GetWritingDirection() != CLocalization::DIRECTION_RTL)
				ChildRect.x += GetShift();
			
			m_Childs[i]->UpdatePosition(ChildRect, m_VisibilityRect);
			PosY += ChildRect.h;
		}
	}
}

void CExpand::Update(bool ParentEnabled)
{
	if(m_pTitle)
		m_pTitle->Update(ParentEnabled && IsEnabled());
	
	if(m_Expanded)
	{
		for(int i=0; i<m_Childs.size(); i++)
			m_Childs[i]->Update(ParentEnabled && IsEnabled());
	}
}
	
void CExpand::Render()
{
	//Background
	const CAsset_GuiBoxStyle* pBoxStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_BoxStylePath);
	if(pBoxStyle)
	{
		CRect Rect = m_DrawRect;
		Rect.RemoveMargin(pBoxStyle->GetMargin());
	
		AssetsRenderer()->DrawGuiRect(&Rect, pBoxStyle->GetRectPath());
			
		Rect.RemoveMargin(pBoxStyle->GetPadding());
	}
	
	if(m_pTitle)
		m_pTitle->Render();
	
	//Childs
	if(m_Expanded)
	{
		for(int i=0; i<m_Childs.size(); i++)
		{
			m_Childs[i]->Render();
		}
	}
}

void CExpand::SetTitle(CWidget* pWidget)
{
	if(m_pTitle)
		m_pTitle->Destroy();
	
	m_pTitle = pWidget;
}

void CExpand::Add(CWidget* pWidget)
{
	m_Childs.add_by_copy(pWidget);
}

void CExpand::OnMouseMove()
{
	if(m_pTitle)
		m_pTitle->OnMouseMove();
	
	if(m_Expanded)
	{
		for(int i=0; i<m_Childs.size(); i++)
			m_Childs[i]->OnMouseMove();
	}
}

void CExpand::OnButtonClick(int Button)
{
	if(m_pTitle)
		m_pTitle->OnButtonClick(Button);
	
	if(m_Expanded)
	{
		for(int i=0; i<m_Childs.size(); i++)
		{
			m_Childs[i]->OnButtonClick(Button);
		}
	}
}

void CExpand::OnButtonRelease(int Button)
{
	if(m_pTitle)
		m_pTitle->OnButtonRelease(Button);
	
	if(m_Expanded)
	{
		for(int i=0; i<m_Childs.size(); i++)
		{
			m_Childs[i]->OnButtonRelease(Button);
		}
	}
}

void CExpand::OnInputEvent(const CInput::CEvent& Event)
{
	if(m_pTitle)
		m_pTitle->OnInputEvent(Event);
	
	if(m_Expanded)
	{
		for(int i=0; i<m_Childs.size(); i++)
		{
			m_Childs[i]->OnInputEvent(Event);
		}
	}
}

}
