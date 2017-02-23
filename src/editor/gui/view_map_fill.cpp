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

#include <editor/gui/view_map_fill.h>
#include <editor/components/gui.h>
#include <client/maprenderer.h>
#include <client/components/assetsrenderer.h>

/* CURSOR TOOL ********************************************************/

CCursorTool_MapFill::CCursorTool_MapFill(CViewMap* pViewMap) :
	CCursorTool(pViewMap, _LSTRING("Filling"), pViewMap->AssetsEditor()->m_Path_Sprite_IconFill),
	m_DragEnabled(false)
{
	
}

void CCursorTool_MapFill::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(Button == KEY_MOUSE_1)
	{
		m_DragEnabled = true;
		
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		m_Pivot = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapFill::OnViewButtonRelease(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(m_DragEnabled)
	{
		m_DragEnabled = false;
		
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		
		if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
		{
			//Copy tiles
			const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer && pLayer->GetSourcePath().IsNull())
			{
				vec2 PivotTilePos = ViewMap()->MapRenderer()->MapPosToTilePos(m_Pivot);
				vec2 CursorTilePos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
				int TileX0 = std::floor(PivotTilePos.x) - pLayer->GetPositionX();
				int TileY0 = std::floor(PivotTilePos.y) - pLayer->GetPositionY();
				int TileX1 = std::floor(CursorTilePos.x) - pLayer->GetPositionX();
				int TileY1 = std::floor(CursorTilePos.y) - pLayer->GetPositionY();
			
				int X0 = min(TileX0, TileX1);
				int Y0 = min(TileY0, TileY1);
				int X1 = max(TileX0, TileX1) + 1;
				int Y1 = max(TileY0, TileY1) + 1;
				
				int Token = AssetsManager()->GenerateToken();
				
				for(int j=Y0; j<Y1; j++)
				{
					for(int i=X0; i<X1; i++)
					{
						CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(i, j);
						AssetsManager()->SetAssetValue<int>(
							AssetsEditor()->GetEditedAssetPath(),
							TilePath,
							CAsset_MapLayerTiles::TILE_INDEX,
							1,
							Token
						);
						AssetsManager()->SetAssetValue<int>(
							AssetsEditor()->GetEditedAssetPath(),
							TilePath,
							CAsset_MapLayerTiles::TILE_FLAGS,
							0x0,
							Token
						);
						AssetsManager()->SetAssetValue<int>(
							AssetsEditor()->GetEditedAssetPath(),
							TilePath,
							CAsset_MapLayerTiles::TILE_BRUSH,
							1,
							Token
						);
					}
				}
			}
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapFill::RenderView()
{
	CAssetPath ImagePath;
	vec4 Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
		if(!pLayer)
			return;
		
		ImagePath = pLayer->GetStylePath();
		Color = pLayer->GetColor();
	}
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	float Time = fmod(ViewMap()->MapRenderer()->GetLocalTime(), 1.0f);
	Color.a *= 0.3f + 0.7f*(1.0f+cos(2.0f*Pi*Time))/2.0f;
	
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	vec2 CursorTilePos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
	vec2 PivotTilePos = ViewMap()->MapRenderer()->MapPosToTilePos(m_Pivot);
	
	if(m_DragEnabled)
	{
		int TileX0 = std::floor(PivotTilePos.x);
		int TileY0 = std::floor(PivotTilePos.y);
		int TileX1 = std::floor(CursorTilePos.x);
		int TileY1 = std::floor(CursorTilePos.y);
	
		int X0 = min(TileX0, TileX1);
		int Y0 = min(TileY0, TileY1);
		int X1 = max(TileX0, TileX1) + 1;
		int Y1 = max(TileY0, TileY1) + 1;
		
		vec2 SelectionRectPos0 = ViewMap()->MapRenderer()->MapPosToScreenPos(vec2(X0*32.0f, Y0*32.0f));
		vec2 SelectionRectPos1 = ViewMap()->MapRenderer()->MapPosToScreenPos(vec2(X1*32.0f, Y1*32.0f));
		vec2 SelectionRectSize = SelectionRectPos1 - SelectionRectPos0;
		
		gui::CRect Rect;
		Rect.x = SelectionRectPos0.x;
		Rect.y = SelectionRectPos0.y;
		Rect.w = SelectionRectSize.x;
		Rect.h = SelectionRectSize.y;
	
		AssetsRenderer()->DrawGuiRect(&Rect, AssetsEditor()->m_Path_Rect_Selection);
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}

void CCursorTool_MapFill::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerTiles::TypeId:
		case CAsset_MapZoneTiles::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	CCursorTool::Update(ParentEnabled);
}

void CCursorTool_MapFill::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_LSTRING("Filling Tool: Fill area of the map."));
	
	CViewMap::CCursorTool::OnMouseMove();
}
