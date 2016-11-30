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

#include <editor/gui/view_sprite.h>
#include <editor/components/gui.h>
#include <shared/components/assetsmanager.h>
#include <client/components/assetsrenderer.h>
#include <client/components/graphics.h>

/* VIEW SPRITE ********************************************************/

CViewSprite::CViewSprite(CGuiEditor* pAssetsEditor) :
	CViewManager::CView(pAssetsEditor)
{

}

void CViewSprite::RenderView()
{
	const CAsset_Sprite* pSprite = AssetsManager()->GetAsset<CAsset_Sprite>(AssetsEditor()->GetEditedAssetPath());
	if(!pSprite)
		return;
	
	vec2 Pos;
	Pos.x = m_ViewRect.x + m_ViewRect.w/2;
	Pos.y = m_ViewRect.y + m_ViewRect.h/2;
	
	AssetsRenderer()->DrawSprite(AssetsEditor()->GetEditedAssetPath(), Pos, 1.0f, 0.0f, 0x0, 1.0f);
}
