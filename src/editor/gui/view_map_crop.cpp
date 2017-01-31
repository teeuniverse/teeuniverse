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

#include <editor/gui/view_map_crop.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/maprenderer.h>

/* CURSORTOOL CROP ****************************************************/

CCursorTool_MapCrop::CCursorTool_MapCrop(CViewMap* pViewMap) :
	CViewMap::CCursorTool(pViewMap, _LSTRING("Crop"), pViewMap->AssetsEditor()->m_Path_Sprite_IconCrop),
	m_Token(CAssetsHistory::NEW_TOKEN),
	m_DragType(DRAGTYPE_NONE)
{
	
}

void CCursorTool_MapCrop::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	vec2 CropMin;
	vec2 CropSize;
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapGroup::TypeId)
	{
		const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(AssetsEditor()->GetEditedAssetPath());
		if(!pGroup || !pGroup->GetClipping())
			return;
		
		ViewMap()->MapRenderer()->UnsetGroup();
		CropMin = ViewMap()->MapRenderer()->MapPosToScreenPos(pGroup->GetClipPosition());
		CropSize = ViewMap()->MapRenderer()->MapPosToScreenPos(pGroup->GetClipPosition() + pGroup->GetClipSize()) - CropMin;
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
		if(!pLayer)
			return;
		
		ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
		CropMin = ViewMap()->MapRenderer()->TilePosToScreenPos(vec2(pLayer->GetPositionX(), pLayer->GetPositionY()));
		CropSize = ViewMap()->MapRenderer()->TilePosToScreenPos(vec2(pLayer->GetPositionX(), pLayer->GetPositionY()) + vec2(pLayer->GetTileWidth(), pLayer->GetTileHeight())) - CropMin;
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
	{
		const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
		if(!pLayer)
			return;
			
		CropMin = ViewMap()->MapRenderer()->TilePosToScreenPos(0.0f);
		CropSize = ViewMap()->MapRenderer()->TilePosToScreenPos(vec2(pLayer->GetTileWidth(), pLayer->GetTileHeight())) - CropMin;
	}
	else
		return;
	
	float GizmoSize = 32.0f;
	
	vec2 CropMax = CropMin + CropSize;
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	
	if(distance(CursorPos, CropMin + CropSize/2.0f) < GizmoSize)
	{
		m_DragType = DRAGTYPE_TRANSLATION;
		m_CursorGizmoDiff = CursorPos - (CropMin + CropSize/2.0f);
		m_Token = AssetsManager()->GenerateToken();
	}
	else if(CursorPos.x > CropMin.x - GizmoSize && CursorPos.x <= CropMin.x && CursorPos.y > CropMin.y && CursorPos.y <= CropMax.y)
	{
		m_DragType = DRAGTYPE_LEFT;
		m_CursorGizmoDiff = vec2(CursorPos.x - CropMin.x, 0.0f);
		m_Token = AssetsManager()->GenerateToken();
	}
	else if(CursorPos.x >= CropMax.x && CursorPos.x < CropMax.x + GizmoSize && CursorPos.y > CropMin.y && CursorPos.y <= CropMax.y)
	{
		m_DragType = DRAGTYPE_RIGHT;
		m_CursorGizmoDiff = vec2(CursorPos.x - CropMax.x, 0.0f);
		m_Token = AssetsManager()->GenerateToken();
	}
	else if(CursorPos.y > CropMin.y - GizmoSize && CursorPos.y <= CropMin.y && CursorPos.x > CropMin.x && CursorPos.x <= CropMax.x)
	{
		m_DragType = DRAGTYPE_TOP;
		m_CursorGizmoDiff = vec2(0.0f, CursorPos.y - CropMin.y);
		m_Token = AssetsManager()->GenerateToken();
	}
	else if(CursorPos.y >= CropMax.y && CursorPos.y < CropMax.y + GizmoSize && CursorPos.x > CropMin.x && CursorPos.x <= CropMax.x)
	{
		m_DragType = DRAGTYPE_BOTTOM;
		m_CursorGizmoDiff = vec2(0.0f, CursorPos.y - CropMax.y);
		m_Token = AssetsManager()->GenerateToken();
	}
	else if(CursorPos.x > CropMin.x - GizmoSize && CursorPos.x <= CropMin.x && CursorPos.y > CropMin.y - GizmoSize && CursorPos.y <= CropMin.y)
	{
		m_DragType = DRAGTYPE_TL;
		m_CursorGizmoDiff = vec2(CursorPos.x - CropMin.x, CursorPos.y - CropMin.y);
		m_Token = AssetsManager()->GenerateToken();
	}
	else if(CursorPos.x >= CropMax.x && CursorPos.x < CropMax.x + GizmoSize && CursorPos.y > CropMin.y - GizmoSize && CursorPos.y <= CropMin.y)
	{
		m_DragType = DRAGTYPE_TR;
		m_CursorGizmoDiff = vec2(CursorPos.x - CropMax.x, CursorPos.y - CropMin.y);
		m_Token = AssetsManager()->GenerateToken();
	}
	else if(CursorPos.x > CropMin.x - GizmoSize && CursorPos.x <= CropMin.x && CursorPos.y >= CropMax.y && CursorPos.y < CropMax.y + GizmoSize)
	{
		m_DragType = DRAGTYPE_BL;
		m_CursorGizmoDiff = vec2(CursorPos.x - CropMin.x, CursorPos.y - CropMax.y);
		m_Token = AssetsManager()->GenerateToken();
	}
	else if(CursorPos.x >= CropMax.x && CursorPos.x < CropMax.x + GizmoSize && CursorPos.y >= CropMax.y && CursorPos.y < CropMax.y + GizmoSize)
	{
		m_DragType = DRAGTYPE_BR;
		m_CursorGizmoDiff = vec2(CursorPos.x - CropMax.x, CursorPos.y - CropMax.y);
		m_Token = AssetsManager()->GenerateToken();
	}
}
	
void CCursorTool_MapCrop::OnViewButtonRelease(int Button)
{
	m_DragType = DRAGTYPE_NONE;
}
	
void CCursorTool_MapCrop::OnViewMouseMove()
{
	if(m_DragType != DRAGTYPE_NONE)
	{
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		vec2 GizmoPos = CursorPos -  m_CursorGizmoDiff;
		
		if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapGroup::TypeId)
		{
			const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(AssetsEditor()->GetEditedAssetPath());
			if(!pGroup || !pGroup->GetClipping())
				return;
			
			ViewMap()->MapRenderer()->UnsetGroup();
			vec2 GizmoMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(GizmoPos);
			
			if(m_DragType == DRAGTYPE_TRANSLATION)
			{
				vec2 ClipPos = GizmoMapPos - pGroup->GetClipSize()/2.0f;
				AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPPOSITION, ClipPos, m_Token);
			}
			else if(m_DragType == DRAGTYPE_LEFT)
			{
				float ClipPos = GizmoMapPos.x;
				float ClipSize = pGroup->GetClipPosition().x + pGroup->GetClipSize().x - ClipPos;
				AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPPOSITION_X, ClipPos, m_Token);
				AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPSIZE_X, ClipSize, m_Token);
			}
			else if(m_DragType == DRAGTYPE_RIGHT)
			{
				float ClipSize = GizmoMapPos.x - pGroup->GetClipPosition().x;
				AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPSIZE_X, ClipSize, m_Token);
			}
			else if(m_DragType == DRAGTYPE_TOP)
			{
				float ClipPos = GizmoMapPos.y;
				float ClipSize = pGroup->GetClipPosition().y + pGroup->GetClipSize().y - ClipPos;
				AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPPOSITION_Y, ClipPos, m_Token);
				AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPSIZE_Y, ClipSize, m_Token);
			}
			else if(m_DragType == DRAGTYPE_BOTTOM)
			{
				float ClipSize = GizmoMapPos.y - pGroup->GetClipPosition().y;
				AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPSIZE_Y, ClipSize, m_Token);
			}
			else if(m_DragType == DRAGTYPE_TL)
			{
				vec2 ClipPos = GizmoMapPos;
				vec2 ClipSize = pGroup->GetClipPosition() + pGroup->GetClipSize() - ClipPos;
				AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPPOSITION, ClipPos, m_Token);
				AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPSIZE, ClipSize, m_Token);
			}
			else if(m_DragType == DRAGTYPE_TR)
			{
				vec2 ClipPos;
				vec2 ClipSize;
				ClipPos.x = pGroup->GetClipPosition().x;
				ClipPos.y = GizmoMapPos.y;
				ClipSize.x = GizmoMapPos.x - pGroup->GetClipPosition().x;
				ClipSize.y = pGroup->GetClipPosition().y + pGroup->GetClipSize().y - ClipPos.y;
				
				AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPPOSITION, ClipPos, m_Token);
				AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPSIZE, ClipSize, m_Token);
			}
			else if(m_DragType == DRAGTYPE_BL)
			{
				vec2 ClipPos;
				vec2 ClipSize;
				ClipPos.x = GizmoMapPos.x;
				ClipPos.y = pGroup->GetClipPosition().y;
				ClipSize.x = pGroup->GetClipPosition().x + pGroup->GetClipSize().x - ClipPos.x;
				ClipSize.y = GizmoMapPos.y - pGroup->GetClipPosition().y;
				
				AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPPOSITION, ClipPos, m_Token);
				AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPSIZE, ClipSize, m_Token);
			}
			else if(m_DragType == DRAGTYPE_BR)
			{
				vec2 ClipSize = GizmoMapPos - pGroup->GetClipPosition();
				AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::CLIPSIZE, ClipSize, m_Token);
			}
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
		{
			const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
			if(!pLayer)
				return;
			
			ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
			vec2 GizmoMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(GizmoPos);
			
			if(m_DragType == DRAGTYPE_TOP)
			{
				int ClipPosY = std::ceil(GizmoMapPos.y/32.0f);
				int ShiftY = pLayer->GetPositionY() - ClipPosY;
				
				if(ShiftY != 0 && AssetsManager()->SaveAssetInHistory(AssetsEditor()->GetEditedAssetPath(), m_Token))
				{
					CAsset_MapLayerTiles* pEditableLayer = AssetsManager()->GetAsset_Hard<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
					array2d<CAsset_MapLayerTiles::CTile>& Tiles = pEditableLayer->GetTileArray();
					Tiles.apply_shift(0, ShiftY);
					
					int ClipSizeY = pLayer->GetTileHeight() + ShiftY;
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::POSITIONY, ClipPosY, m_Token);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::TILE_HEIGHT, ClipSizeY, m_Token);
				}
			}
			else if(m_DragType == DRAGTYPE_TL)
			{
				int ClipPosX = std::ceil(GizmoMapPos.x/32.0f);
				int ClipPosY = std::ceil(GizmoMapPos.y/32.0f);
				int ShiftX = pLayer->GetPositionX() - ClipPosX;
				int ShiftY = pLayer->GetPositionY() - ClipPosY;
				
				if((ShiftX != 0 || ShiftY != 0) && AssetsManager()->SaveAssetInHistory(AssetsEditor()->GetEditedAssetPath(), m_Token))
				{
					CAsset_MapLayerTiles* pEditableLayer = AssetsManager()->GetAsset_Hard<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
					array2d<CAsset_MapLayerTiles::CTile>& Tiles = pEditableLayer->GetTileArray();
					Tiles.apply_shift(ShiftX, ShiftY);
					
					int ClipSizeX = pLayer->GetTileWidth() + ShiftX;
					int ClipSizeY = pLayer->GetTileHeight() + ShiftY;
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::POSITIONX, ClipPosX, m_Token);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::POSITIONY, ClipPosY, m_Token);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::TILE_WIDTH, ClipSizeX, m_Token);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::TILE_HEIGHT, ClipSizeY, m_Token);
				}			
			}
			else if(m_DragType == DRAGTYPE_TR)
			{
				int ClipPosY = std::ceil(GizmoMapPos.y/32.0f);
				int ShiftY = pLayer->GetPositionY() - ClipPosY;
				
				if(ShiftY != 0 && AssetsManager()->SaveAssetInHistory(AssetsEditor()->GetEditedAssetPath(), m_Token))
				{
					CAsset_MapLayerTiles* pEditableLayer = AssetsManager()->GetAsset_Hard<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
					array2d<CAsset_MapLayerTiles::CTile>& Tiles = pEditableLayer->GetTileArray();
					Tiles.apply_shift(0, ShiftY);
					
					int ClipSizeY = pLayer->GetTileHeight() + ShiftY;
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::POSITIONY, ClipPosY, m_Token);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::TILE_HEIGHT, ClipSizeY, m_Token);
				}
				
				int ClipSizeX = std::floor(GizmoMapPos.x/32.0f) - pLayer->GetPositionX();
				AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::TILE_WIDTH, ClipSizeX, m_Token);	
			}
			else if(m_DragType == DRAGTYPE_LEFT)
			{
				int ClipPosX = std::ceil(GizmoMapPos.x/32.0f);
				int ShiftX = pLayer->GetPositionX() - ClipPosX;
				
				if(ShiftX != 0 && AssetsManager()->SaveAssetInHistory(AssetsEditor()->GetEditedAssetPath(), m_Token))
				{
					CAsset_MapLayerTiles* pEditableLayer = AssetsManager()->GetAsset_Hard<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
					array2d<CAsset_MapLayerTiles::CTile>& Tiles = pEditableLayer->GetTileArray();
					Tiles.apply_shift(ShiftX, 0);
					
					int ClipSizeX = pLayer->GetTileWidth() + ShiftX;
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::POSITIONX, ClipPosX, m_Token);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::TILE_WIDTH, ClipSizeX, m_Token);
				}			
			}
			else if(m_DragType == DRAGTYPE_RIGHT)
			{
				int ClipSizeX = std::floor(GizmoMapPos.x/32.0f) - pLayer->GetPositionX();
				AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::TILE_WIDTH, ClipSizeX, m_Token);
			}
			else if(m_DragType == DRAGTYPE_BOTTOM)
			{
				int ClipSizeY = std::floor(GizmoMapPos.y/32.0f) - pLayer->GetPositionY();
				AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::TILE_HEIGHT, ClipSizeY, m_Token);
			}
			else if(m_DragType == DRAGTYPE_BL)
			{
				int ClipPosX = std::ceil(GizmoMapPos.x/32.0f);
				int ShiftX = pLayer->GetPositionX() - ClipPosX;
				
				if(ShiftX != 0 && AssetsManager()->SaveAssetInHistory(AssetsEditor()->GetEditedAssetPath(), m_Token))
				{
					CAsset_MapLayerTiles* pEditableLayer = AssetsManager()->GetAsset_Hard<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
					array2d<CAsset_MapLayerTiles::CTile>& Tiles = pEditableLayer->GetTileArray();
					Tiles.apply_shift(ShiftX, 0);
					
					int ClipSizeX = pLayer->GetTileWidth() + ShiftX;
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::POSITIONX, ClipPosX, m_Token);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::TILE_WIDTH, ClipSizeX, m_Token);
				}
				
				int ClipSizeY = std::floor(GizmoMapPos.y/32.0f) - pLayer->GetPositionY();
				AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::TILE_HEIGHT, ClipSizeY, m_Token);
			}
			else if(m_DragType == DRAGTYPE_BR)
			{
				int ClipSizeX = std::floor(GizmoMapPos.x/32.0f) - pLayer->GetPositionX();
				int ClipSizeY = std::floor(GizmoMapPos.y/32.0f) - pLayer->GetPositionY();
				AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::TILE_WIDTH, ClipSizeX, m_Token);
				AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerTiles::TILE_HEIGHT, ClipSizeY, m_Token);
			}
			
			ViewMap()->MapRenderer()->UnsetGroup();
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
		{
			const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
			if(!pLayer)
				return;
			
			ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
			vec2 GizmoMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(GizmoPos);
			
			if(m_DragType == DRAGTYPE_RIGHT)
			{
				int ClipSizeX = std::floor(GizmoMapPos.x/32.0f);
				AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapZoneTiles::TILE_WIDTH, ClipSizeX, m_Token);
			}
			else if(m_DragType == DRAGTYPE_BOTTOM)
			{
				int ClipSizeY = std::floor(GizmoMapPos.y/32.0f);
				AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapZoneTiles::TILE_HEIGHT, ClipSizeY, m_Token);
			}
			else if(m_DragType == DRAGTYPE_BR)
			{
				int ClipSizeX = std::floor(GizmoMapPos.x/32.0f);
				int ClipSizeY = std::floor(GizmoMapPos.y/32.0f);
				AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapZoneTiles::TILE_WIDTH, ClipSizeX, m_Token);
				AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapZoneTiles::TILE_HEIGHT, ClipSizeY, m_Token);
			}
			
			ViewMap()->MapRenderer()->UnsetGroup();
		}
		else
			return;
	}
}
	
void CCursorTool_MapCrop::RenderView()
{
	vec2 CropMin;
	vec2 CropSize;
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapGroup::TypeId)
	{
		const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(AssetsEditor()->GetEditedAssetPath());
		if(!pGroup || !pGroup->GetClipping())
			return;
		
		ViewMap()->MapRenderer()->UnsetGroup();
		CropMin = ViewMap()->MapRenderer()->MapPosToScreenPos(pGroup->GetClipPosition());
		CropSize = ViewMap()->MapRenderer()->MapPosToScreenPos(pGroup->GetClipPosition() + pGroup->GetClipSize()) - CropMin;
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
		if(!pLayer)
			return;
		
		ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
		CropMin = ViewMap()->MapRenderer()->TilePosToScreenPos(vec2(pLayer->GetPositionX(), pLayer->GetPositionY()));
		CropSize = ViewMap()->MapRenderer()->TilePosToScreenPos(vec2(pLayer->GetPositionX(), pLayer->GetPositionY()) + vec2(pLayer->GetTileWidth(), pLayer->GetTileHeight())) - CropMin;
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
	{
		const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
		if(!pLayer)
			return;
			
		CropMin = ViewMap()->MapRenderer()->MapPosToScreenPos(0.0f);
		CropSize = ViewMap()->MapRenderer()->TilePosToScreenPos(vec2(pLayer->GetTileWidth(), pLayer->GetTileHeight())) - CropMin;
	}
	else
		return;
	
	float GizmoSize = 16.0f;
	vec2 CropMax = CropMin + CropSize;
	
	
	//Scaling
	CAssetPath GizmoPath = AssetsEditor()->m_Path_Sprite_GizmoScale;
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() != CAsset_MapZoneTiles::TypeId)
	{
		//Pivot
		AssetsRenderer()->DrawSprite(
			AssetsEditor()->m_Path_Sprite_GizmoPivot,
			vec2(CropMin + CropSize/2.0f),
			1.0f, 0.0f, 0x0, 1.0f
		);
		
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(CropMin.x - GizmoSize, CropMin.y - GizmoSize), 1.0f, 0.0f, 0x0, 1.0f);
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(CropMin.x - GizmoSize, CropMax.y + GizmoSize), 1.0f, Pi/2.0f, 0x0, 1.0f);
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(CropMin.x - GizmoSize, CropMin.y + CropSize.y/2.0f), 1.0f, Pi/2.0f+Pi/4.0f, 0x0, 1.0f);
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(CropMax.x + GizmoSize, CropMin.y - GizmoSize), 1.0f, Pi/2.0f, 0x0, 1.0f);
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(CropMin.x + CropSize.x/2.0f, CropMin.y - GizmoSize), 1.0f, Pi/4.0f, 0x0, 1.0f);
	}
	
	AssetsRenderer()->DrawSprite(GizmoPath, vec2(CropMax.x + GizmoSize, CropMax.y + GizmoSize), 1.0f, Pi, 0x0, 1.0f);
	AssetsRenderer()->DrawSprite(GizmoPath, vec2(CropMin.x + CropSize.x/2.0f, CropMax.y + GizmoSize), 1.0f, Pi/4.0f+Pi, 0x0, 1.0f);
	AssetsRenderer()->DrawSprite(GizmoPath, vec2(CropMax.x + GizmoSize, CropMin.y + CropSize.y/2.0f), 1.0f, Pi/2.0f+Pi/4.0f+Pi, 0x0, 1.0f);
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapCrop::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapGroup::TypeId:
		case CAsset_MapLayerTiles::TypeId:
		case CAsset_MapZoneTiles::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	CViewMap::CCursorTool::Update(ParentEnabled);
}

void CCursorTool_MapCrop::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_LSTRING("Crop Tool: Edit boundaries of a layer"));
	
	CViewMap::CCursorTool::OnMouseMove();
}
