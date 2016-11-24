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

#include <editor/gui/view_map.h>

#include <editor/gui/view_map_stamp.h>
#include <editor/gui/view_map_transform.h>
#include <editor/gui/view_map_crop.h>
#include <editor/components/gui.h>
#include <client/maprenderer.h>
#include <client/components/assetsrenderer.h>
#include <client/gui/slider.h>
#include <client/gui/toggle.h>
#include <client/gui/expand.h>

/* VIEW MAP ***********************************************************/

class CZoneOpacitySlider : public gui::CHSlider
{
protected:
	CViewMap* m_pViewMap;	
	
protected:
	virtual int GetValue() const
	{
		return m_pViewMap->GetZoneOpacity()*100.0f;
	}
	
	virtual void SetValue(int Value)
	{
		m_pViewMap->SetZoneOpacity(Value/100.0f);
	}
	
public:
	CZoneOpacitySlider(CViewMap* pViewMap) :
		gui::CHSlider(pViewMap->Context(), 0, 100),
		m_pViewMap(pViewMap)
	{
		
	}
};

CViewMap::CViewMap(CGuiEditor* pAssetsEditor) :
	CViewManager::CView(pAssetsEditor),
	m_CameraPos(0.0f, 0.0f),
	m_CameraZoom(1.0f),
	m_CameraDraged(false),
	m_pMapRenderer(NULL),
	m_pCursorTool_MapStamp(NULL),
	m_pCursorTool_MapTransform(NULL),
	m_pCursorTool_MapEdit(NULL),
	m_ZoneOpacity(0.5f)
{
	
	m_pCursorTool_MapStamp = new CCursorTool_MapStamp(this);
	m_pToolbar->Add(m_pCursorTool_MapStamp);
	
	m_pCursorTool_MapTransform = new CCursorTool_MapTransform(this);
	m_pToolbar->Add(m_pCursorTool_MapTransform);
	
	m_pCursorTool_MapEdit = new CCursorTool_MapEdit(this);
	m_pToolbar->Add(m_pCursorTool_MapEdit);
	
	m_pCursorTool_MapCrop = new CCursorTool_MapCrop(this);
	m_pToolbar->Add(m_pCursorTool_MapCrop);
	
	m_pToolbar->Add(new gui::CExpand(Context()), true);
	m_pToolbar->Add(new CZoneOpacitySlider(this), false, 200);
	
	m_pMapRenderer = new CMapRenderer(AssetsEditor()->EditorKernel());
}

CViewMap::~CViewMap()
{
	if(m_pMapRenderer)
		delete m_pMapRenderer;
}

CAssetPath CViewMap::GetMapGroupPath()
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapGroup::TypeId:
			return AssetsEditor()->GetEditedAssetPath();
		case CAsset_MapLayerTiles::TypeId:
		case CAsset_MapLayerQuads::TypeId:
			for(int s=0; s<AssetsManager()->GetNumPackages(); s++)
			{
				for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_Map>(s); i++)
				{
					CAssetPath MapPath = CAssetPath(CAsset_Map::TypeId, s, i);
					const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(MapPath);
					if(pMap)
					{
						CAsset_Map::CIteratorBgGroup IterBgGroup;
						for(IterBgGroup = pMap->BeginBgGroup(); IterBgGroup != pMap->EndBgGroup(); ++IterBgGroup)
						{
							const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(pMap->GetBgGroup(*IterBgGroup));
							if(pGroup)
							{
								CAsset_MapGroup::CIteratorLayer IterLayer;
								for(IterLayer = pGroup->BeginLayer(); IterLayer != pGroup->EndLayer(); ++IterLayer)
								{
									if(pGroup->GetLayer(*IterLayer) == AssetsEditor()->GetEditedAssetPath())
										return pMap->GetBgGroup(*IterBgGroup);
								}
							}
						}
						CAsset_Map::CIteratorFgGroup IterFgGroup;
						for(IterFgGroup = pMap->BeginFgGroup(); IterFgGroup != pMap->EndFgGroup(); ++IterFgGroup)
						{
							const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(pMap->GetFgGroup(*IterFgGroup));
							if(pGroup)
							{
								CAsset_MapGroup::CIteratorLayer IterLayer;
								for(IterLayer = pGroup->BeginLayer(); IterLayer != pGroup->EndLayer(); ++IterLayer)
								{
									if(pGroup->GetLayer(*IterLayer) == AssetsEditor()->GetEditedAssetPath())
										return pMap->GetFgGroup(*IterFgGroup);
								}
							}
						}
					}
				}
			}
			break;
	}
	
	return CAssetPath::Null();
}

CAssetPath CViewMap::GetMapPath()
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_Map::TypeId:
			return AssetsEditor()->GetEditedAssetPath();
		case CAsset_MapZoneTiles::TypeId:
			for(int s=0; s<AssetsManager()->GetNumPackages(); s++)
			{
				for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_Map>(s); i++)
				{
					CAssetPath MapPath = CAssetPath(CAsset_Map::TypeId, s, i);
					const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(MapPath);
					if(pMap)
					{
						CAsset_Map::CIteratorZoneLayer IterZoneLayer;
						for(IterZoneLayer = pMap->BeginZoneLayer(); IterZoneLayer != pMap->EndZoneLayer(); ++IterZoneLayer)
						{
							if(pMap->GetZoneLayer(*IterZoneLayer) == AssetsEditor()->GetEditedAssetPath())
								return MapPath;
						}
					}
				}
			}
			break;
		case CAsset_MapEntities::TypeId:
			for(int s=0; s<AssetsManager()->GetNumPackages(); s++)
			{
				for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_Map>(s); i++)
				{
					CAssetPath MapPath = CAssetPath(CAsset_Map::TypeId, s, i);
					const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(MapPath);
					if(pMap)
					{
						CAsset_Map::CIteratorEntityLayer IterEntityLayer;
						for(IterEntityLayer = pMap->BeginEntityLayer(); IterEntityLayer != pMap->EndEntityLayer(); ++IterEntityLayer)
						{
							if(pMap->GetEntityLayer(*IterEntityLayer) == AssetsEditor()->GetEditedAssetPath())
								return MapPath;
						}
					}
				}
			}
			break;
		case CAsset_MapGroup::TypeId:
			for(int s=0; s<AssetsManager()->GetNumPackages(); s++)
			{
				for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_Map>(s); i++)
				{
					CAssetPath MapPath = CAssetPath(CAsset_Map::TypeId, s, i);
					const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(MapPath);
					if(pMap)
					{
						CAsset_Map::CIteratorBgGroup IterBgGroup;
						for(IterBgGroup = pMap->BeginBgGroup(); IterBgGroup != pMap->EndBgGroup(); ++IterBgGroup)
						{
							if(pMap->GetBgGroup(*IterBgGroup) == AssetsEditor()->GetEditedAssetPath())
								return MapPath;
						}
						CAsset_Map::CIteratorFgGroup IterFgGroup;
						for(IterFgGroup = pMap->BeginFgGroup(); IterFgGroup != pMap->EndFgGroup(); ++IterFgGroup)
						{
							if(pMap->GetFgGroup(*IterFgGroup) == AssetsEditor()->GetEditedAssetPath())
								return MapPath;
						}
					}
				}
			}
			break;
		case CAsset_MapLayerTiles::TypeId:
		case CAsset_MapLayerQuads::TypeId:
			for(int s=0; s<AssetsManager()->GetNumPackages(); s++)
			{
				for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_Map>(s); i++)
				{
					CAssetPath MapPath = CAssetPath(CAsset_Map::TypeId, s, i);
					const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(MapPath);
					if(pMap)
					{
						CAsset_Map::CIteratorBgGroup IterBgGroup;
						for(IterBgGroup = pMap->BeginBgGroup(); IterBgGroup != pMap->EndBgGroup(); ++IterBgGroup)
						{
							const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(pMap->GetBgGroup(*IterBgGroup));
							if(pGroup)
							{
								CAsset_MapGroup::CIteratorLayer IterLayer;
								for(IterLayer = pGroup->BeginLayer(); IterLayer != pGroup->EndLayer(); ++IterLayer)
								{
									if(pGroup->GetLayer(*IterLayer) == AssetsEditor()->GetEditedAssetPath())
										return MapPath;
								}
							}
						}
						CAsset_Map::CIteratorFgGroup IterFgGroup;
						for(IterFgGroup = pMap->BeginFgGroup(); IterFgGroup != pMap->EndFgGroup(); ++IterFgGroup)
						{
							const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(pMap->GetFgGroup(*IterFgGroup));
							if(pGroup)
							{
								CAsset_MapGroup::CIteratorLayer IterLayer;
								for(IterLayer = pGroup->BeginLayer(); IterLayer != pGroup->EndLayer(); ++IterLayer)
								{
									if(pGroup->GetLayer(*IterLayer) == AssetsEditor()->GetEditedAssetPath())
										return MapPath;
								}
							}
						}
					}
				}
			}
			break;
	}
	
	return CAssetPath::Null();
}

void CViewMap::Update(bool ParentEnabled)
{
	CView::Update(ParentEnabled);
	
	if(!GetCursorTool() || GetCursorTool()->IsDisabled())
	{
		switch(AssetsEditor()->GetEditedAssetPath().GetType())
		{
			case CAsset_MapEntities::TypeId:
			case CAsset_MapLayerTiles::TypeId:
			case CAsset_MapZoneTiles::TypeId:
			case CAsset_MapLayerQuads::TypeId:
				SetCursorTool(m_pCursorTool_MapStamp);
				break;
			case CAsset_MapGroup::TypeId:
				SetCursorTool(m_pCursorTool_MapTransform);
				break;
		}
	}
}

void CViewMap::RenderView()
{
	CAssetPath MapPath = GetMapPath();
	
	MapRenderer()->SetTime(0.0f);
	MapRenderer()->SetLocalTime((double)time_get()/(double)time_freq());
	MapRenderer()->SetCanvas(m_ViewRect, vec2(m_ViewRect.x + m_ViewRect.w/2, m_ViewRect.y + m_ViewRect.h/2));
	MapRenderer()->SetCamera(m_CameraPos, m_CameraZoom);
	
	vec4 Color = 1.0f;
	if(m_ZoneOpacity > 0.5f)
	{
		Color.r = 1.0f - 2.0f*(m_ZoneOpacity-0.5f);
		Color.g = 1.0f - 2.0f*(m_ZoneOpacity-0.5f);
		Color.b = 1.0f - 2.0f*(m_ZoneOpacity-0.5f);
	}
	
	MapRenderer()->RenderMap(MapPath, Color);
	
	Color = 1.0f;
	if(m_ZoneOpacity < 0.5f)
		Color.a = 2.0f*m_ZoneOpacity;
	
	MapRenderer()->RenderMap_Zones(MapPath, AssetsEditor()->m_Path_Image_ZoneTexture, Color);
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapEntities::TypeId)
	{
		const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(GetMapPath());
		if(pMap)
		{
			CAsset_Map::CIteratorEntityLayer IterEntityLayer;
			for(IterEntityLayer = pMap->BeginEntityLayer(); IterEntityLayer != pMap->EndEntityLayer(); ++IterEntityLayer)
			{
				CAssetPath EntitiesPath = pMap->GetEntityLayer(*IterEntityLayer);
				
				const CAsset_MapEntities* pEntities = AssetsManager()->GetAsset<CAsset_MapEntities>(EntitiesPath);
				if(!pEntities)
					continue;
				
				const CAssetState* pState = AssetsManager()->GetAssetState(EntitiesPath);
				if(pState && !pState->m_Visible)
					continue;
				
				vec4 Color = 1.0f;
				if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapEntities::TypeId && EntitiesPath != AssetsEditor()->GetEditedAssetPath())
					Color = vec4(1.0f, 1.0f, 1.0f, 0.5f);
				
				CAsset_MapEntities::CIteratorEntity IterEntity;
				for(IterEntity = pEntities->BeginEntity(); IterEntity != pEntities->EndEntity(); ++IterEntity)
				{
					vec2 Pos = MapRenderer()->MapPosToScreenPos(pEntities->GetEntityPosition(*IterEntity));
					CAssetPath TypePath = pEntities->GetEntityTypePath(*IterEntity);
					
					const CAsset_EntityType* pEntityType = AssetsManager()->GetAsset<CAsset_EntityType>(TypePath);
					if(pEntityType)
						AssetsRenderer()->DrawSprite(pEntityType->GetGizmoPath(), Pos, 1.0f, 0.0f, 0x0, 1.0f);
					else
						AssetsRenderer()->DrawSprite(AssetsEditor()->m_Path_Sprite_GizmoPivot, Pos, 1.0f, 0.0f, 0x0, 1.0f);
				}
			}
		}
	}
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
		if(pLayer)
		{
			MapRenderer()->SetGroup(AssetsEditor()->GetEditedAssetPath());
			
			vec2 MinCorner = MapRenderer()->TilePosToScreenPos(vec2(0.0f, 0.0f));
			vec2 MaxCorner = MapRenderer()->TilePosToScreenPos(vec2(pLayer->GetTileWidth(), pLayer->GetTileHeight()));
			
			gui::CRect Rect;
			Rect.x = MinCorner.x;
			Rect.y = MinCorner.y;
			Rect.w = MaxCorner.x - MinCorner.x;
			Rect.h = MaxCorner.y - MinCorner.y;
			AssetsRenderer()->DrawGuiRect(&Rect, AssetsEditor()->m_Path_Rect_Border);
			
			MapRenderer()->UnsetGroup();
		}
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
	{
		const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
		if(pLayer)
		{
			MapRenderer()->SetGroup(AssetsEditor()->GetEditedAssetPath());
			
			vec2 MinCorner = MapRenderer()->TilePosToScreenPos(vec2(0.0f, 0.0f));
			vec2 MaxCorner = MapRenderer()->TilePosToScreenPos(vec2(pLayer->GetTileWidth(), pLayer->GetTileHeight()));
			
			gui::CRect Rect;
			Rect.x = MinCorner.x;
			Rect.y = MinCorner.y;
			Rect.w = MaxCorner.x - MinCorner.x;
			Rect.h = MaxCorner.y - MinCorner.y;
			AssetsRenderer()->DrawGuiRect(&Rect, AssetsEditor()->m_Path_Rect_Border);
			
			MapRenderer()->UnsetGroup();
		}
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapGroup::TypeId)
	{
		const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(AssetsEditor()->GetEditedAssetPath());
		if(pGroup && pGroup->GetClipping())
		{
			vec2 MinCorner = MapRenderer()->MapPosToScreenPos(pGroup->GetClipPosition());
			vec2 MaxCorner = MapRenderer()->MapPosToScreenPos(pGroup->GetClipPosition() + pGroup->GetClipSize());
			
			gui::CRect Rect;
			Rect.x = MinCorner.x;
			Rect.y = MinCorner.y;
			Rect.w = MaxCorner.x - MinCorner.x;
			Rect.h = MaxCorner.y - MinCorner.y;
			AssetsRenderer()->DrawGuiRect(&Rect, AssetsEditor()->m_Path_Rect_Border);
		}
	}
}

void CViewMap::OnButtonClick(int Button)
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
	{
		if(Button == KEY_MOUSE_WHEEL_UP)
		{
			m_CameraZoom *= 1.1f;
		}
		if(Button == KEY_MOUSE_WHEEL_DOWN)
		{
			m_CameraZoom /= 1.1f;
		}
		else if(Button == KEY_MOUSE_3)
		{
			m_CameraDraged = true;
			return;
		}
	}
	
	CView::OnButtonClick(Button);
}

void CViewMap::OnButtonRelease(int Button)
{
	if(Button == KEY_MOUSE_3)
	{
		m_CameraDraged = false;
	}
	
	CView::OnButtonRelease(Button);
}

void CViewMap::OnMouseMove()
{
	if(m_CameraDraged)
	{
		ivec2 MouseRelPos = Context()->GetMouseRelPos();
		m_CameraPos -= vec2(MouseRelPos.x, MouseRelPos.y)/m_CameraZoom;
		return;
	}
	
	CView::OnMouseMove();
}
