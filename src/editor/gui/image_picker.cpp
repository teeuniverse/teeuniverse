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

#include <editor/gui/image_picker.h>
#include <shared/components/assetsmanager.h>
#include <client/components/graphics.h>
#include <client/components/assetsrenderer.h>
#include <editor/components/gui.h>

CImagePicker::CImagePicker(CGuiEditor* pAssetsEditor, CAssetPath ImagePath) :
	gui::CWidget(pAssetsEditor),
	m_pAssetsEditor(pAssetsEditor),
	m_ImagePath(ImagePath),
	m_SelectionEnabled(false),
	m_DragSelection(false)
{
	
}

void CImagePicker::UpdateBoundingSize()
{
	m_BoundingSizeRect.BSNoConstraint();
}
	
void CImagePicker::OnButtonClick(int Button)
{
	if(!m_SelectionEnabled || Button != KEY_MOUSE_1)
		return;
	
	const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(m_ImagePath);
	if(!pImage)
		return;
	
	float ImgRatio = static_cast<float>(pImage->GetDataWidth())/static_cast<float>(pImage->GetDataHeight());
	float WindowRatio = static_cast<float>(m_DrawRect.w)/static_cast<float>(m_DrawRect.h);
	float SizeX;
	float SizeY;
	
	if(ImgRatio > WindowRatio)
	{
		SizeX = m_DrawRect.w;
		SizeY = m_DrawRect.w/ImgRatio;
	}
	else
	{
		SizeX = m_DrawRect.h*ImgRatio;
		SizeY = m_DrawRect.h;
	}
	
	float x0 = m_DrawRect.x + m_DrawRect.w/2 - SizeX/2;
	float y0 = m_DrawRect.y + m_DrawRect.h/2 - SizeY/2;
	float xStep = SizeX / static_cast<float>(max(1, pImage->GetGridWidth()));
	float yStep = SizeY / static_cast<float>(max(1, pImage->GetGridHeight()));

	ivec2 MousePos = Context()->GetMousePos();
	
	m_Pivot.x = clamp((int)((MousePos.x - x0) / xStep), 0, pImage->GetGridWidth()-1);
	m_Pivot.y = clamp((int)((MousePos.y - y0) / yStep), 0, pImage->GetGridHeight()-1);
	m_DragSelection = true;
}

void CImagePicker::OnButtonRelease(int Button)
{
	if(Button != KEY_MOUSE_1)
		return;
	
	m_DragSelection = false;
	
	if(m_SelectionEnabled)
	{
		const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(m_ImagePath);
		if(!pImage)
			return;
		
		float ImgRatio = static_cast<float>(pImage->GetDataWidth())/static_cast<float>(pImage->GetDataHeight());
		float WindowRatio = static_cast<float>(m_DrawRect.w)/static_cast<float>(m_DrawRect.h);
		float SizeX;
		float SizeY;
		
		if(ImgRatio > WindowRatio)
		{
			SizeX = m_DrawRect.w;
			SizeY = m_DrawRect.w/ImgRatio;
		}
		else
		{
			SizeX = m_DrawRect.h*ImgRatio;
			SizeY = m_DrawRect.h;
		}
		
		float x0 = m_DrawRect.x + m_DrawRect.w/2 - SizeX/2;
		float y0 = m_DrawRect.y + m_DrawRect.h/2 - SizeY/2;
		float xStep = SizeX / static_cast<float>(max(1, pImage->GetGridWidth()));
		float yStep = SizeY / static_cast<float>(max(1, pImage->GetGridHeight()));
		
		ivec2 MousePos = Context()->GetMousePos();
		ivec2 MouseImagePos;
		MouseImagePos.x = clamp((int)((MousePos.x - x0) / xStep), 0, pImage->GetGridWidth()-1);
		MouseImagePos.y = clamp((int)((MousePos.y - y0) / yStep), 0, pImage->GetGridHeight()-1);
		
		ivec2 BoxMin;
		ivec2 BoxMax;
		
		BoxMin.x = min(m_Pivot.x, MouseImagePos.x);
		BoxMin.y = min(m_Pivot.y, MouseImagePos.y);
		BoxMax.x = max(m_Pivot.x, MouseImagePos.x)+1;
		BoxMax.y = max(m_Pivot.y, MouseImagePos.y)+1;
		
		OnImagePicked(BoxMin.x, BoxMin.y, BoxMax.x, BoxMax.y);
	}
}

void CImagePicker::Render()
{
	const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(m_ImagePath);
	if(!pImage)
		return;
	
	float ImgRatio = static_cast<float>(pImage->GetDataWidth())/static_cast<float>(pImage->GetDataHeight());
	float WindowRatio = static_cast<float>(m_DrawRect.w)/static_cast<float>(m_DrawRect.h);
	float SizeX;
	float SizeY;
	
	if(ImgRatio > WindowRatio)
	{
		SizeX = m_DrawRect.w;
		SizeY = m_DrawRect.w/ImgRatio;
	}
	else
	{
		SizeX = m_DrawRect.h*ImgRatio;
		SizeY = m_DrawRect.h;
	}
	
	float x0 = m_DrawRect.x + m_DrawRect.w/2 - SizeX/2;
	float y0 = m_DrawRect.y + m_DrawRect.h/2 - SizeY/2;
	float x1 = x0 + SizeX;
	float y1 = y0 + SizeY;
	float xStep = SizeX / static_cast<float>(max(1, pImage->GetGridWidth()));
	float yStep = SizeY / static_cast<float>(max(1, pImage->GetGridHeight()));
	
	//Draw image
	AssetsRenderer()->TextureSet(m_ImagePath);
	
	Graphics()->QuadsBegin();
	CGraphics::CQuadItem QuadItem(x0, y0, SizeX, SizeY);
	Graphics()->QuadsDrawTL(&QuadItem, 1);
	Graphics()->QuadsEnd();
	
	//Draw grid
	Graphics()->TextureClear();
	Graphics()->LinesBegin();
	Graphics()->SetColor(0.0f, 0.0f, 0.0f, 0.5f);
	
	for(int i=0; i<=pImage->GetGridWidth(); i++)
	{
		float x = x0 + i * xStep;
		CGraphics::CLineItem Line(x, y0, x, y1);
		Graphics()->LinesDraw(&Line, 1);
	}
	for(int i=0; i<=pImage->GetGridHeight(); i++)
	{
		float y = y0 + i * yStep;
		CGraphics::CLineItem Line(x0, y, x1, y);
		Graphics()->LinesDraw(&Line, 1);
	}
	
	Graphics()->LinesEnd();
	
	if(m_DragSelection)
	{
		ivec2 MousePos = Context()->GetMousePos();
		ivec2 MouseImagePos;
		MouseImagePos.x = clamp((int)((MousePos.x - x0) / xStep), 0, pImage->GetGridWidth()-1);
		MouseImagePos.y = clamp((int)((MousePos.y - y0) / yStep), 0, pImage->GetGridHeight()-1);
		
		ivec2 BoxMin;
		ivec2 BoxMax;
		
		BoxMin.x = min(m_Pivot.x, MouseImagePos.x);
		BoxMin.y = min(m_Pivot.y, MouseImagePos.y);
		BoxMax.x = max(m_Pivot.x, MouseImagePos.x)+1;
		BoxMax.y = max(m_Pivot.y, MouseImagePos.y)+1;
		
		gui::CRect SelectionRect;
		SelectionRect.x = x0 + xStep * BoxMin.x;
		SelectionRect.y = y0 + yStep * BoxMin.y;
		SelectionRect.w = x0 + xStep * BoxMax.x - SelectionRect.x;
		SelectionRect.h = y0 + yStep * BoxMax.y - SelectionRect.y;
		AssetsRenderer()->DrawGuiRect(&SelectionRect, m_pAssetsEditor->m_Path_Rect_Selection);
	}
}
