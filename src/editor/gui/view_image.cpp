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

#include <editor/gui/view_image.h>
#include <editor/components/gui.h>
#include <shared/components/assetsmanager.h>
#include <client/components/assetsrenderer.h>
#include <client/components/graphics.h>

/* VIEW IMAGE *********************************************************/

CViewImage::CViewImage(CGuiEditor* pAssetsEditor) :
	CViewManager::CView(pAssetsEditor)
{

}

void CViewImage::RenderView()
{
	const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(AssetsEditor()->GetEditedAssetPath());
	if(!pImage)
		return;
	
	float ImgRatio = static_cast<float>(pImage->GetDataWidth())/static_cast<float>(pImage->GetDataHeight());
	float WindowRatio = static_cast<float>(m_ViewRect.w)/static_cast<float>(m_ViewRect.h);
	float SizeX;
	float SizeY;
	
	if(ImgRatio > WindowRatio)
	{
		SizeX = m_ViewRect.w;
		SizeY = m_ViewRect.w/ImgRatio;
	}
	else
	{
		SizeX = m_ViewRect.h*ImgRatio;
		SizeY = m_ViewRect.h;
	}
	
	float x0 = m_ViewRect.x + m_ViewRect.w/2 - SizeX/2;
	float y0 = m_ViewRect.y + m_ViewRect.h/2 - SizeY/2;
	float x1 = x0 + SizeX;
	float y1 = y0 + SizeY;
	float xStep = SizeX / static_cast<float>(max(1, pImage->GetGridWidth()));
	float yStep = SizeY / static_cast<float>(max(1, pImage->GetGridHeight()));
	
	//Draw sprites
	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	float alpha = 0.75f;
	for(int s=0; s<AssetsManager()->GetNumPackages(); s++)
	{
		for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_Sprite>(s); i++)
		{
			CAssetPath SpritePath = CAssetPath(CAsset_Sprite::TypeId, s, i);
			const CAsset_Sprite* pSprite = AssetsManager()->GetAsset<CAsset_Sprite>(SpritePath);
			if(pSprite->GetImagePath() == AssetsEditor()->GetEditedAssetPath())
			{
				Graphics()->SetColor(alpha, alpha, alpha*0.5f, alpha);
					
				CGraphics::CQuadItem QuadItem(x0 + xStep * pSprite->GetX(), y0 + yStep * pSprite->GetY(), xStep * pSprite->GetWidth(), yStep * pSprite->GetHeight());
				Graphics()->QuadsDrawTL(&QuadItem, 1);
			}
		}
	}
	
	Graphics()->QuadsEnd();
	
	//Draw image
	AssetsRenderer()->TextureSet(AssetsEditor()->GetEditedAssetPath());
	
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
}
