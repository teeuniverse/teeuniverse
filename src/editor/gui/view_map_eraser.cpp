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

#include <editor/gui/view_map_eraser.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/maprenderer.h>
#include <shared/geometry/geometry.h>
#include <generated/assets/maplayerquads.h>
#include <client/gui/toggle.h>

/* CURSORTOOL MAP ERASER **********************************************/

CCursorTool_MapEraser::CCursorTool_MapEraser(CViewMap* pViewMap) :
	CCursorTool_MapPicker(pViewMap, _LSTRING("Eraser"), pViewMap->AssetsEditor()->m_Path_Sprite_IconErase)
{
	
}

void CCursorTool_MapEraser::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	CSubPath ItemFound = Pick(CursorPos);
		
	if(!ItemFound.IsNull())
	{
		CAssetState* pState = AssetsManager()->GetAssetState(AssetsEditor()->GetEditedAssetPath());
		pState->m_NumUpdates++;
		
		AssetsManager()->DeleteSubItem(AssetsEditor()->GetEditedAssetPath(), ItemFound);
		AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null());
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapEraser::RenderView()
{
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId || AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		RenderPivots();
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapEraser::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapZoneObjects::TypeId:
		case CAsset_MapLayerObjects::TypeId:
		case CAsset_MapLayerQuads::TypeId:
		case CAsset_MapEntities::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	CCursorTool::Update(ParentEnabled);
}

void CCursorTool_MapEraser::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_LSTRING("Eraser Tool: Delete objects in the map"));
	
	CCursorTool_MapPicker::OnMouseMove();
}
