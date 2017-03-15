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

#include <editor/gui/view_map_object.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/maprenderer.h>
#include <shared/geometry/geometry.h>
#include <generated/assets/maplayerobjects.h>

/* CURSORTOOL OBJECT VERTEX PICKER ************************************/

CCursorTool_MapObjectVertexPicker::CCursorTool_MapObjectVertexPicker(CViewMap* pViewMap, const CLocalizableString& LString, CAssetPath IconPath) :
	CViewMap::CCursorTool(pViewMap, LString, IconPath)
{
	
}
	
void CCursorTool_MapObjectVertexPicker::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		default:
			Disable();
	}
	
	CCursorTool::Update(ParentEnabled);
}

/* CURSORTOOL OBJECT WEIGHT VERTEX ************************************/

CCursorTool_MapObjectWeightVertex::CCursorTool_MapObjectWeightVertex(CViewMap* pViewMap) :
	CCursorTool_MapObjectVertexPicker(pViewMap, _LSTRING("Weight Editor"), pViewMap->AssetsEditor()->m_Path_Sprite_IconWeightVertex),
	m_Drag(false),
	m_Token(CAssetsHistory::NEW_TOKEN)
{
	
}
	
void CCursorTool_MapObjectWeightVertex::RenderView()
{
	RenderPivots<CAsset_MapLayerObjects>();
	RenderVertices<CAsset_MapLayerObjects>();
}

void CCursorTool_MapObjectWeightVertex::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(m_CurrentVertex.IsNotNull())
	{
		AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex);
		m_Token = AssetsManager()->GenerateToken();
		m_Drag = true;
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapObjectWeightVertex::OnViewButtonRelease(int Button)
{
	if(Button == KEY_MOUSE_1)
	{
		m_Drag = false;
		m_Token = CAssetsHistory::NEW_TOKEN;
	}
}
	
void CCursorTool_MapObjectWeightVertex::OnViewMouseMove()
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(!m_Drag)
		return;
	
	const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	float Weight = AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_WEIGHT, 1.0f);
	
	vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	float Diff = MousePos.x - ClickPos.x;
	float NewWeight = max(Weight + Diff/30.0f, 0.01f);
	
	AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_WEIGHT, NewWeight, m_Token);
	
	ViewMap()->MapRenderer()->UnsetGroup();
}

void CCursorTool_MapObjectWeightVertex::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_LSTRING("Weight Editor: Change the thickness of vertices"));
	
	CViewMap::CCursorTool::OnMouseMove();
}
