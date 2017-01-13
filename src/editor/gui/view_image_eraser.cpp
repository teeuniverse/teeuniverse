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

#include <editor/gui/view_image_eraser.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/maprenderer.h>

/* CURSORTOOL SPRITE CREATOR ******************************************/

CCursorTool_Eraser::CCursorTool_Eraser(CViewImage* pViewImage) :
	CViewImage::CCursorTool(pViewImage, _LSTRING("Eraser"), pViewImage->AssetsEditor()->m_Path_Sprite_IconErase)
{
	
}

void CCursorTool_Eraser::OnImagePicked(int MinX, int MinY, int MaxX, int MaxY)
{
	CAsset_Image* pImage = AssetsManager()->GetAsset_Hard<CAsset_Image>(ViewImage()->AssetsEditor()->GetEditedAssetPath());
	if(!pImage)
		return;
	
	int MinPixX = clamp((MinX * pImage->GetDataWidth())/pImage->GetGridWidth(), 0, pImage->GetDataWidth());
	int MinPixY = clamp((MinY * pImage->GetDataWidth())/pImage->GetGridWidth(), 0, pImage->GetDataWidth());
	int MaxPixX = clamp((MaxX * pImage->GetDataWidth())/pImage->GetGridWidth(), 0, pImage->GetDataWidth());
	int MaxPixY = clamp((MaxY * pImage->GetDataWidth())/pImage->GetGridWidth(), 0, pImage->GetDataWidth());
	
	AssetsManager()->SaveAssetInHistory(ViewImage()->AssetsEditor()->GetEditedAssetPath());
	
	array2d< uint8, allocator_default<uint8> >& ImageData = pImage->GetDataArray();
	
	for(int y=MinPixY; y<MaxPixY; y++)
	{
		for(int x=MinPixX; x<MaxPixX; x++)
		{
			for(int d=0; d<ImageData.get_depth(); d++)
			{
				ImageData.set_clamp(x, y, d, 0);
			}
		}
	}
	
	AssetsManager()->RequestUpdate(ViewImage()->AssetsEditor()->GetEditedAssetPath());
}
