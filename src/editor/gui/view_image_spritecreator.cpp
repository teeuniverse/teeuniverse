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

#include <editor/gui/view_image_spritecreator.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/maprenderer.h>

/* CURSORTOOL SPRITE CREATOR ******************************************/

CCursorTool_ImageSpriteCreator::CCursorTool_ImageSpriteCreator(CViewImage* pViewImage) :
	CViewImage::CCursorTool(pViewImage, _LSTRING("Sprite creator"), pViewImage->AssetsEditor()->m_Path_Sprite_IconSpriteCreator)
{
	
}

void CCursorTool_ImageSpriteCreator::OnImagePicked(int MinX, int MinY, int MaxX, int MaxY)
{
	if(ViewImage()->AssetsEditor()->GetEditedAssetPath().GetType() != CAsset_Image::TypeId)
		return;
	
	int Token = AssetsManager()->GenerateToken();
	CAssetPath SpritePath;
	CAsset_Sprite* pSprite = AssetsManager()->NewAsset<CAsset_Sprite>(&SpritePath, ViewImage()->AssetsEditor()->GetEditedPackageId(), Token);
	if(pSprite)
	{
		AssetsManager()->TryChangeAssetName(SpritePath, "sprite", Token);
		pSprite->SetImagePath(ViewImage()->AssetsEditor()->GetEditedAssetPath());
		pSprite->SetX(MinX);
		pSprite->SetY(MinY);
		pSprite->SetWidth(MaxX - MinX);
		pSprite->SetHeight(MaxY - MinY);
		
		ViewImage()->AssetsEditor()->RefreshAssetsTree();
	}
}
