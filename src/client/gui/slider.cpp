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

#include <shared/components/assetsmanager.h>
#include <client/components/graphics.h>
#include <client/components/assetsrenderer.h>

#include "slider.h"
	
namespace gui
{

/* ABSTRACT SLIDER ****************************************************/

CAbstractSlider::CAbstractSlider(CGui* pContext, int Min, int Max) :
	CWidget(pContext),
	m_Clicked(false),
	m_Min(Min),
	m_Max(Max)
{
	SetSliderStyle(Context()->GetSliderStyle());
}

void CAbstractSlider::UpdateBoundingSize()
{
	m_BoundingSizeRect.BSNoConstraint();
	
	const CAsset_GuiSliderStyle* pSliderStyle = AssetsManager()->GetAsset<CAsset_GuiSliderStyle>(m_SliderStylePath);
	if(pSliderStyle)
	{
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pSliderStyle->GetPadding()));
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pSliderStyle->GetMargin()));
	}
}

void CAbstractSlider::OnButtonRelease(int Button)
{
	if(Button != KEY_MOUSE_1)
		return;
	
	m_Clicked = false;
}

/* H SLIDER ***********************************************************/

CHSlider::CHSlider(CGui* pContext, int Min, int Max) :
	CAbstractSlider(pContext, Min, Max)
{
	
}

void CHSlider::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	CAbstractSlider::UpdatePosition(BoundingRect, VisibilityRect);
	
	int Padding = 0;
	m_ClipRect = m_DrawRect;
	const CAsset_GuiSliderStyle* pSliderStyle = AssetsManager()->GetAsset<CAsset_GuiSliderStyle>(m_SliderStylePath);
	if(pSliderStyle)
	{
		Padding = Context()->ApplyGuiScale(pSliderStyle->GetPadding());
		m_ClipRect.RemoveMargin(Context()->ApplyGuiScale(pSliderStyle->GetMargin()));
		m_ClipRect.RemoveMargin(Padding);
		
		const CAsset_Sprite* pSprite = AssetsManager()->GetAsset<CAsset_Sprite>(pSliderStyle->GetCursorPath());
		if(pSprite)
		{
			int PosX = static_cast<float>(m_ClipRect.w * (GetValue() - m_Min))/static_cast<float>(m_Max - m_Min);
			int Width = Context()->GetGuiScale()*pSprite->GetPixelWidth();
			int Height = Context()->GetGuiScale()*pSprite->GetPixelHeight();
			m_CursorRect = CRect(
				m_ClipRect.x + PosX - Width/2,
				m_ClipRect.y + m_ClipRect.h/2 - Height/2,
				Width, 
				Height
			);
		}
	}
}
	
void CHSlider::Render()
{
	const CAsset_GuiSliderStyle* pSliderStyle = AssetsManager()->GetAsset<CAsset_GuiSliderStyle>(m_SliderStylePath);
	if(pSliderStyle)
	{
		CRect Rect = m_DrawRect;
		Rect.RemoveMargin(Context()->ApplyGuiScale(pSliderStyle->GetMargin()));
	
		AssetsRenderer()->DrawGuiRect(&Rect, pSliderStyle->GetRectPath());
			
		Rect.RemoveMargin(Context()->ApplyGuiScale(pSliderStyle->GetPadding()));
		
		ivec2 RailPos0(m_ClipRect.x, m_ClipRect.y + m_ClipRect.h/2);
		ivec2 RailPos1(m_ClipRect.x + m_ClipRect.w, m_ClipRect.y + m_ClipRect.h/2);
		AssetsRenderer()->DrawGuiLine(RailPos0, RailPos1, pSliderStyle->GetDefaultRailPath());
		
		const CAsset_Sprite* pSprite = AssetsManager()->GetAsset<CAsset_Sprite>(pSliderStyle->GetCursorPath());
		if(pSprite)
		{
			AssetsRenderer()->DrawSprite(
				pSliderStyle->GetCursorPath(),
				vec2(m_CursorRect.x + m_CursorRect.w/2, m_CursorRect.y + m_CursorRect.h/2),
				Context()->GetGuiScale(), 0.0f, 0x0, 1.0f
			);
		}
	}
}

void CHSlider::OnButtonClick(int Button)
{
	if(Button == KEY_MOUSE_1)
	{
		if(m_CursorRect.IsInside(Context()->GetMousePos()))
		{
			m_Clicked = true;
			m_ClickShift = Context()->GetMousePos().x - (m_CursorRect.x + m_CursorRect.w/2);
		}
	}
	else if(m_ClipRect.IsInside(Context()->GetMousePos()))
	{
		if(Button == KEY_MOUSE_WHEEL_UP)
			SetValue(clamp(GetValue()+1, m_Min, m_Max));
		else if(Button == KEY_MOUSE_WHEEL_DOWN)
			SetValue(clamp(GetValue()-1, m_Min, m_Max));
	}
}

void CHSlider::OnMouseMove()
{
	if(m_Clicked)
	{
		int CursorPos = (Context()->GetMousePos().x - m_ClickShift) - m_ClipRect.x;
		int Value = m_Min + static_cast<float>(CursorPos * (m_Max - m_Min)) / m_ClipRect.w;
		SetValue(clamp(Value, m_Min, m_Max));
	}
}

/* EXTERNAL H SLIDER **************************************************/

CExternalHSlider::CExternalHSlider(class CGui *pContext, int* pValue, int Min, int Max) :
	CHSlider(pContext, Min, Max),
	m_pValue(pValue)
{
	
}

int CExternalHSlider::GetValue() const
{
	return clamp(*m_pValue, m_Min, m_Max);
}

void CExternalHSlider::SetValue(int Value)
{
	*m_pValue = clamp(Value, m_Min, m_Max);
}

}
