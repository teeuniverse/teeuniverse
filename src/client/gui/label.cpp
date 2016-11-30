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

#include "label.h"
	
namespace gui
{

/* ABSTRACT LABEL *****************************************************/

CAbstractLabel::CAbstractLabel(CGui *pContext) :
	CWidget(pContext),
	m_IconPath(CAssetPath::Null()),
	m_pIconWidget(NULL),
	m_Localize(false),
	m_ClipText(true)
{
	SetLabelStyle(Context()->GetLabelStyle());
	m_Text.clear();
}

void CAbstractLabel::Destroy()
{
	if(m_pIconWidget)
		m_pIconWidget->Destroy();
}

void CAbstractLabel::OnTextUpdated()
{
	if(m_Localize)
		ApplyLocalization();
	
	m_TextCache.SetText(m_Text.buffer());
}

void CAbstractLabel::ApplyLocalization()
{
	m_Text.clear();
	Localization()->Format(m_Text, NULL, m_LString);
}

void CAbstractLabel::Update(bool ParentEnabled)
{
	if(ParentEnabled && m_Localize && Context()->LocalizationUpdated())
		OnTextUpdated();
	
	if(m_pIconWidget)
		m_pIconWidget->Update(ParentEnabled);
}

void CAbstractLabel::UpdateBoundingSize()
{
	m_FontSize = Context()->ApplyGuiScale(11);
	int Alignment = CAsset_GuiLabelStyle::TEXTALIGNMENT_LEFT;
	int Spacing = 0;
	int MinWidth = 1;
	int MinHeight = 1;
	m_BoundingSizeRect.BSNoConstraint();
	
	const CAsset_GuiLabelStyle* pLabelStyle = AssetsManager()->GetAsset<CAsset_GuiLabelStyle>(m_LabelStylePath);
	if(pLabelStyle)
	{
		MinWidth = Context()->ApplyGuiScale(pLabelStyle->GetMinWidth());
		MinHeight = Context()->ApplyGuiScale(pLabelStyle->GetMinHeight());
		m_BoundingSizeRect.BSAddSpacing(MinWidth, MinHeight);
		
		m_FontSize = Context()->ApplyGuiScale(pLabelStyle->GetFontSize());
		Spacing = Context()->ApplyGuiScale(pLabelStyle->GetSpacing());
		
		Alignment = pLabelStyle->GetTextAlignment();
	}
	
	CRect ContentRect;
	ContentRect.BSNoConstraint();
	
	const CAsset_Sprite* pSprite = AssetsManager()->GetAsset<CAsset_Sprite>(m_IconPath);
	if(m_pIconWidget)
	{
		m_pIconWidget->UpdateBoundingSize();
		
		CRect IconRect = m_pIconWidget->GetBS();
		
		if(Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER || Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED)
			ContentRect.BSVerticalAdd(IconRect);
		else
			ContentRect.BSHorizontalAdd(IconRect);
	}
	else
	{
		if(pSprite)
		{
			float IconScaling = Context()->GetGuiScale();
			int IconWidth = pSprite->GetPixelWidth();
			int IconHeight = pSprite->GetPixelHeight();
			
			if(Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED)
			{
				IconScaling = static_cast<float>(MinWidth) / IconWidth;
			}
			
			IconWidth *= IconScaling;
			IconHeight *= IconScaling;
			
			CRect IconRect;
			IconRect.BSMinimum(IconWidth, IconHeight);
			
			if(Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER || Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED)
				ContentRect.BSVerticalAdd(IconRect);
			else
				ContentRect.BSHorizontalAdd(IconRect);
		}
	}
	
	if(GetText()[0])
	{
		CRect TextRect;
		if(m_ClipText)
			TextRect.BSMinimum(m_FontSize, m_FontSize);
		else
			TextRect.BSMinimum(TextRenderer()->GetTextWidth(&m_TextCache), m_FontSize);
		
		if(Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER || Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED)
			ContentRect.BSVerticalAdd(TextRect);
		else
			ContentRect.BSHorizontalAdd(TextRect);
	}
	
	if(GetText()[0] && (m_pIconWidget || pSprite))
	{
		if(Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER || Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED)
			ContentRect.BSAddSpacing(0, Spacing);
		else
			ContentRect.BSAddSpacing(Spacing, 0);
	}
	
	m_BoundingSizeRect.BSInnerAdd(ContentRect);
	
	if(pLabelStyle)
	{
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pLabelStyle->GetPadding()));
		m_BoundingSizeRect.BSAddMargin(Context()->ApplyGuiScale(pLabelStyle->GetMargin()));
	}
	
	m_TextCache.SetFontSize(m_FontSize);
}

void CAbstractLabel::UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect)
{
	CWidget::UpdatePosition(BoundingRect, VisibilityRect);
	
	CRect Rect = m_DrawRect;
	int Alignment = CAsset_GuiLabelStyle::TEXTALIGNMENT_LEFT;
	int Spacing = 0;
	
	const CAsset_GuiLabelStyle* pLabelStyle = AssetsManager()->GetAsset<CAsset_GuiLabelStyle>(m_LabelStylePath);
	if(pLabelStyle)
	{
		Alignment = pLabelStyle->GetTextAlignment();
		
		Rect.RemoveMargin(Context()->ApplyGuiScale(pLabelStyle->GetMargin()));
		Rect.RemoveMargin(Context()->ApplyGuiScale(pLabelStyle->GetPadding()));
	}
	
	m_IconScaling = 1.0f;
	m_IconRect.w = 0;
	m_IconRect.h = 0;
	int TextWidth = 0;
	
	const CAsset_Sprite* pSprite = NULL;
	if(m_pIconWidget)
	{
		m_IconRect.w = m_pIconWidget->GetBS().minw;
		m_IconRect.h = m_pIconWidget->GetBS().minh;
		
		if(Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED)
		{
			m_IconRect.w = Rect.w;
			m_IconRect.h = Rect.h;
		}
		else
		{
			m_IconRect.w = m_pIconWidget->GetBS().minw;
			m_IconRect.h = m_pIconWidget->GetBS().minh;
		}
	}
	else
	{
		pSprite = AssetsManager()->GetAsset<CAsset_Sprite>(m_IconPath);
		if(pSprite)
		{
			m_IconScaling = Context()->GetGuiScale();
			m_IconRect.w = pSprite->GetPixelWidth();
			m_IconRect.h = pSprite->GetPixelHeight();
			
			if(Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED)
			{
				int MinWidth = Context()->ApplyGuiScale(pLabelStyle->GetMinWidth());
				
				m_IconScaling = static_cast<float>(MinWidth) / m_IconRect.w;
			}
				
			m_IconRect.w *= m_IconScaling;
			m_IconRect.h *= m_IconScaling;
		}
	}
	
	m_TextCache.SetBoxSize(ivec2(-1, Rect.h));
	
	if(GetText()[0])
	{
		if((pSprite || m_pIconWidget) && pLabelStyle)
			Spacing = Context()->ApplyGuiScale(pLabelStyle->GetSpacing());
		
		TextWidth = TextRenderer()->GetTextWidth(&m_TextCache);
	}
	
	int TotalWidth = m_IconRect.w + Spacing + TextWidth;
	int PosX = 0;
	
	//Inverse direction in case of RTL localization
	if(Localization()->GetWritingDirection() == CLocalization::DIRECTION_RTL)
	{
		if(Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_LEFT)
			Alignment = CAsset_GuiLabelStyle::TEXTALIGNMENT_RIGHT;
		else if(Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_RIGHT)
			Alignment = CAsset_GuiLabelStyle::TEXTALIGNMENT_LEFT;
	}
	
	if(Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_LEFT)
		PosX = Rect.x;
	else if(Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_RIGHT)
		PosX = Rect.x + Rect.w - TotalWidth;
	else
		PosX = Rect.x + Rect.w/2 - TotalWidth/2;
	
	if(m_pIconWidget)
		m_pIconWidget->Render();
	
	if(Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER || Alignment == CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED)
	{
		int PosY = Rect.y;
		
		m_IconRect.x = Rect.x;
		m_IconRect.y = PosY;
		
		PosY += Spacing + m_IconRect.h/2;
		
		if(GetText()[0])
		{
			m_TextRect.x = Rect.x + Rect.w/2 - TextWidth/2;
			m_TextRect.y = PosY;
			m_TextRect.w = TextRenderer()->GetTextWidth(&m_TextCache);
			m_TextRect.h = Rect.h;
			
			m_TextCache.SetBoxSize(ivec2(-1, m_TextRect.h));
		}
	}
	else if(Localization()->GetWritingDirection() == CLocalization::DIRECTION_RTL)
	{
		if(GetText()[0])
		{
			m_TextRect.x = PosX;
			m_TextRect.y = Rect.y;
			m_TextRect.w = TextRenderer()->GetTextWidth(&m_TextCache);
			m_TextRect.h = Rect.h;
			
			m_TextCache.SetBoxSize(ivec2(-1, m_TextRect.h));
			
			PosX += TextWidth;
		}
		
		PosX += Spacing;
		
		m_IconRect.x = PosX;
		m_IconRect.y = Rect.y;
	}
	else
	{
		m_IconRect.x = PosX;
		m_IconRect.y = Rect.y;
		
		if(m_pIconWidget || pSprite)
			PosX += m_IconRect.w;
		
		PosX += Spacing;
		
		if(GetText()[0])
		{
			m_TextRect.x = PosX;
			m_TextRect.y = Rect.y;
			m_TextRect.w = TextRenderer()->GetTextWidth(&m_TextCache);
			m_TextRect.h = Rect.h;
			
			m_TextCache.SetBoxSize(ivec2(-1, m_TextRect.h));
		}
	}
	
	if(m_pIconWidget)
		m_pIconWidget->UpdatePosition(m_IconRect, m_VisibilityRect);
}

void CAbstractLabel::Render()
{
	CRect Rect = m_DrawRect;
	CRect ClipRect = m_DrawRect;
	vec4 FontColor = 1.0f;
	
	const CAsset_GuiLabelStyle* pLabelStyle = AssetsManager()->GetAsset<CAsset_GuiLabelStyle>(m_LabelStylePath);
	if(pLabelStyle)
	{
		FontColor = pLabelStyle->GetTextColor();
		
		Rect.RemoveMargin(Context()->ApplyGuiScale(pLabelStyle->GetMargin()));
		ClipRect.RemoveMargin(Context()->ApplyGuiScale(pLabelStyle->GetMargin()));
		
		AssetsRenderer()->DrawGuiRect(&Rect, pLabelStyle->GetRectPath());
			
		Rect.RemoveMargin(Context()->ApplyGuiScale(pLabelStyle->GetPadding()));
	}
	
	Graphics()->ClipPush(ClipRect.x, ClipRect.y, ClipRect.w, ClipRect.h);
	
	if(GetText()[0])
		TextRenderer()->DrawText(&m_TextCache, ivec2(m_TextRect.x, m_TextRect.y), FontColor);
	
	if(m_pIconWidget)
		m_pIconWidget->Render();
	else
	{
		AssetsRenderer()->DrawSprite(
			m_IconPath,
			vec2(m_IconRect.x + m_IconRect.w/2, m_IconRect.y + m_IconRect.h/2),
			m_IconScaling, 0.0f, 0x0, 1.0f
		);
	}
	
	Graphics()->ClipPop();
	
	if(Context()->HasFocus(this))
		Context()->DrawFocusRect(m_DrawRect);
}

void CAbstractLabel::SetText(const char* pText)
{
	m_Localize = false;
	
	if(pText)
		m_Text.copy(pText);
	else
		m_Text.clear();
	
	OnTextUpdated();
}

void CAbstractLabel::SetText(const CLocalizableString& Text)
{
	m_Localize = true;
	m_LString.copy(Text);
	
	OnTextUpdated();
}

void CAbstractLabel::SetLabelStyle(CAssetPath Path)
{
	m_LabelStylePath = Path;
	
	if(m_IconPath.IsNull())
	{
		const CAsset_GuiLabelStyle* pLabelStyle = AssetsManager()->GetAsset<CAsset_GuiLabelStyle>(m_LabelStylePath);
		if(pLabelStyle)
		{
			if(pLabelStyle->GetIconPath().IsNotNull())
				SetIcon(pLabelStyle->GetIconPath());
			else
				SetIcon(CAssetPath::Null());
		}
	}
}

/* LABEL **************************************************************/

CLabel::CLabel(CGui* pContext, const char* pText, CAssetPath IconPath) :
	CAbstractLabel(pContext)
{	
	SetIcon(IconPath);
	SetText(pText);
}

CLabel::CLabel(CGui* pContext, const CLocalizableString& LocalizableString, CAssetPath IconPath) :
	CAbstractLabel(pContext)
{
	SetIcon(IconPath);
	SetText(LocalizableString);
}

CLabel::CLabel(CGui* pContext, const CLocalizableString& LocalizableString, gui::CWidget* pIconWidget) :
	CAbstractLabel(pContext)
{
	SetIconWidget(pIconWidget);
	SetText(LocalizableString);
}

/* LABEL HEADER *******************************************************/

CLabelHeader::CLabelHeader(CGui* pContext, const char* pText, CAssetPath IconPath) :
	CAbstractLabel(pContext)
{
	SetLabelStyle(Context()->GetLabelHeaderStyle());
	SetIcon(IconPath);
	SetText(pText);
}

CLabelHeader::CLabelHeader(CGui* pContext, const CLocalizableString& LocalizableString, CAssetPath IconPath) :
	CAbstractLabel(pContext)
{
	SetLabelStyle(Context()->GetLabelHeaderStyle());
	SetIcon(IconPath);
	SetText(LocalizableString);
}

}
