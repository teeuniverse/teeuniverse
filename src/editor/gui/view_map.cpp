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

#include <editor/gui/view_map.h>

#include <editor/gui/view_map_stamp.h>
#include <editor/gui/view_map_transform.h>
#include <editor/gui/view_map_crop.h>
#include <editor/gui/view_map_object.h>
#include <editor/components/gui.h>
#include <client/maprenderer.h>
#include <client/components/assetsrenderer.h>
#include <client/gui/slider.h>
#include <client/gui/toggle.h>
#include <client/gui/filler.h>

/* VIEW MAP ***********************************************************/

class CSimpleToggle : public gui::CToggle
{
protected:
	CGuiEditor* m_pAssetsEditor;
	bool* m_pBoolean;
	CLocalizableString m_LHint;
	
protected:
	virtual bool GetValue()
	{
		return *m_pBoolean;
	}

	virtual void SetValue(bool Value)
	{
		*m_pBoolean = Value;
	}
	
public:
	CSimpleToggle(CGuiEditor* pAssetsEditor, bool* pBoolean, CAssetPath IconPath, const CLocalizableString& LHint) :
		gui::CToggle(pAssetsEditor, IconPath),
		m_pAssetsEditor(pAssetsEditor),
		m_pBoolean(pBoolean)
	{
		SetToggleStyle(pAssetsEditor->m_Path_Toggle_Toolbar);
		m_LHint.copy(LHint);
	}

	virtual void OnMouseMove()
	{
		if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
			m_pAssetsEditor->SetHint(m_LHint);
		
		gui::CToggle::OnMouseMove();
	}
};

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

	virtual void OnMouseMove()
	{
		if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
			m_pViewMap->AssetsEditor()->SetHint(_LSTRING("Zones visibility: Change the visibility of zones from \"invisible\" to \"emphasis\""));
		
		gui::CHSlider::OnMouseMove();
	}
};

CViewMap::CViewMap(CGuiEditor* pAssetsEditor) :
	CViewManager::CView(pAssetsEditor),
	m_CameraPos(0.0f, 0.0f),
	m_CameraZoom(1.0f),
	m_CameraDraged(false),
	m_pMapRenderer(NULL),
	m_pCursorTool_MapStamp(NULL),
	m_pCursorTool_MapEdit(NULL),
	m_pCursorTool_MapEraser(NULL),
	m_pCursorTool_MapCrop(NULL),
	m_pCursorTool_MapEditVertex(NULL),
	m_pCursorTool_MapAddVertex(NULL),
	m_pCursorTool_MapWeightVertex(NULL),
	m_ZoneOpacity(0.5f),
	m_ShowGrid(false),
	m_ShowMeshes(false)
{
	m_pCursorTool_MapStamp = new CCursorTool_MapStamp(this);
	m_pToolbar->Add(m_pCursorTool_MapStamp);
	
	m_pCursorTool_MapTransform = new CCursorTool_MapTransform(this);
	m_pToolbar->Add(m_pCursorTool_MapTransform);
	
	m_pCursorTool_MapEdit = new CCursorTool_MapEdit(this);
	m_pToolbar->Add(m_pCursorTool_MapEdit);
	
	m_pCursorTool_MapEraser = new CCursorTool_MapEraser(this);
	m_pToolbar->Add(m_pCursorTool_MapEraser);
	
	m_pCursorTool_MapCrop = new CCursorTool_MapCrop(this);
	m_pToolbar->Add(m_pCursorTool_MapCrop);
	
	m_pCursorTool_MapEditVertex = new CCursorTool_MapObjectEditVertex(this);
	m_pToolbar->Add(m_pCursorTool_MapEditVertex);
	
	m_pCursorTool_MapAddVertex = new CCursorTool_MapObjectAddVertex(this);
	m_pToolbar->Add(m_pCursorTool_MapAddVertex);
	
	m_pCursorTool_MapWeightVertex = new CCursorTool_MapObjectWeightVertex(this);
	m_pToolbar->Add(m_pCursorTool_MapWeightVertex);
	
	m_pCursorTool_MapStamp->UpdateToolbar();
	m_pCursorTool_MapEdit->UpdateToolbar();
	m_pCursorTool_MapEraser->UpdateToolbar();
	m_pCursorTool_MapCrop->UpdateToolbar();
	m_pCursorTool_MapEditVertex->UpdateToolbar();
	m_pCursorTool_MapAddVertex->UpdateToolbar();
	m_pCursorTool_MapWeightVertex->UpdateToolbar();
	
	m_pToolbar->Add(new gui::CFiller(Context()), true);
	m_pToolbar->Add(new CSimpleToggle(AssetsEditor(), &m_ShowMeshes, AssetsEditor()->m_Path_Sprite_IconBigMesh, _LSTRING("Show/hide meshes")), false);
	m_pToolbar->Add(new CSimpleToggle(AssetsEditor(), &m_ShowGrid, AssetsEditor()->m_Path_Sprite_IconGrid, _LSTRING("Show/hide grid")), false);
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
		{
			const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer)
				return pLayer->GetParentPath();
			break;
		}
		case CAsset_MapLayerQuads::TypeId:
		{
			const CAsset_MapLayerQuads* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer)
				return pLayer->GetParentPath();
			break;
		}
		case CAsset_MapLayerObjects::TypeId:
		{
			const CAsset_MapLayerObjects* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer)
				return pLayer->GetParentPath();
			break;
		}
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
		{
			const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer)
				return pLayer->GetParentPath();
			break;
		}
		case CAsset_MapEntities::TypeId:
		{
			const CAsset_MapEntities* pLayer = AssetsManager()->GetAsset<CAsset_MapEntities>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer)
				return pLayer->GetParentPath();
			break;
		}
		case CAsset_MapGroup::TypeId:
		{
			const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(AssetsEditor()->GetEditedAssetPath());
			if(pGroup)
				return pGroup->GetParentPath();
			break;
		}
		case CAsset_MapLayerTiles::TypeId:
		{
			const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer)
			{
				const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(pLayer->GetParentPath());
				if(pGroup)
					return pGroup->GetParentPath();
			}
			break;
		}
		case CAsset_MapLayerQuads::TypeId:
		{
			const CAsset_MapLayerQuads* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer)
			{
				const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(pLayer->GetParentPath());
				if(pGroup)
					return pGroup->GetParentPath();
			}
			break;
		}
		case CAsset_MapLayerObjects::TypeId:
		{
			const CAsset_MapLayerObjects* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer)
			{
				const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(pLayer->GetParentPath());
				if(pGroup)
					return pGroup->GetParentPath();
			}
			break;
		}
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
			case CAsset_MapLayerObjects::TypeId:
				SetCursorTool(m_pCursorTool_MapEditVertex);
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
	
	MapRenderer()->RenderMap(MapPath, Color, m_ShowMeshes);
	
	Color = 1.0f;
	if(m_ZoneOpacity < 0.5f)
		Color.a = 2.0f*m_ZoneOpacity;
	
	MapRenderer()->RenderMap_Zones(MapPath, AssetsEditor()->m_Path_Image_ZoneTexture, Color);
	
	//Draw grid
	if(m_ShowGrid)
	{
		float Log = log((1.0/m_CameraZoom)*32.0)/log(2.0);
		float Int = floor(Log);
		float Frac = Log - Int;
		float Alpha = 1.f-Frac;
		float Step = pow(2.0, Int);
		
		MapRenderer()->SetGroup(GetMapGroupPath());
		MapRenderer()->RenderGrid(Step, vec4(1.0f, 1.0f, 1.0f, 0.5f*(Alpha)));
		MapRenderer()->RenderGrid(Step*2.0f, vec4(1.0f, 1.0f, 1.0f, 0.5f*(1.0f-Alpha)));
		MapRenderer()->UnsetGroup();
	}
	
	//Draw entities
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
				
				if(!pEntities->GetVisibility())
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
						AssetsRenderer()->DrawSprite(pEntityType->GetGizmoPath(), Pos, 1.0f, 0.0f, 0x0, Color);
					else
						AssetsRenderer()->DrawSprite(AssetsEditor()->m_Path_Sprite_GizmoPivot, Pos, 1.0f, 0.0f, 0x0, Color);
				}
			}
		}
	}
	
	//Draw boxes
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
		if(pLayer)
		{
			MapRenderer()->SetGroup(GetMapGroupPath());
			
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
			MapRenderer()->SetGroup(GetMapGroupPath());
			
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
		else if(Button == KEY_MOUSE_3 || (Button == KEY_MOUSE_1 && Input()->KeyIsPressed(KEY_LCTRL)))
		{
			m_CameraDraged = true;
			return;
		}
	}
	
	CView::OnButtonClick(Button);
}

void CViewMap::OnButtonRelease(int Button)
{
	if(m_CameraDraged && (Button == KEY_MOUSE_1 || Button == KEY_MOUSE_3))
	{
		m_CameraDraged = false;
	}
	
	CView::OnButtonRelease(Button);
}

void CViewMap::OnMouseMove()
{
	ivec2 MousePos = Context()->GetMousePos();
	
	if(m_VisibilityRect.IsInside(MousePos))
	{
		vec2 MapPos = MapRenderer()->ScreenPosToMapPos(vec2(MousePos.x, MousePos.y));
		
		CLocalizableString LString(_("Map position: ({float:XCoord}   {float:YCoord})"));
		LString.AddFloat("XCoord", MapPos.x);
		LString.AddFloat("YCoord", MapPos.y);
		AssetsEditor()->SetCoordinates(LString);
	}
	
	if(m_CameraDraged)
	{
		ivec2 MouseRelPos = Context()->GetMouseRelPos();
		m_CameraPos -= vec2(MouseRelPos.x, MouseRelPos.y)/m_CameraZoom;
		return;
	}
	
	CView::OnMouseMove();
}
