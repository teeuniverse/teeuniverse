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
#include <editor/gui/view_map_quadtransform.h>
#include <editor/components/gui.h>
#include <client/maprenderer.h>

/* VIEW MAP ***********************************************************/

CViewMap::CViewMap(CGuiEditor* pAssetsEditor) :
	CViewManager::CView(pAssetsEditor),
	m_CameraPos(0.0f, 0.0f),
	m_CameraZoom(1.0f),
	m_CameraDraged(false),
	m_pMapRenderer(NULL),
	m_pCursorTool_TileStamp(NULL),
	m_pCursorTool_QuadTransform(NULL)
{
	m_pCursorTool_TileStamp = new CCursorTool_TileStamp(this);
	m_pToolbar->Add(m_pCursorTool_TileStamp);
	
	m_pCursorTool_QuadTransform = new CCursorTool_QuadTransform(this);
	m_pToolbar->Add(m_pCursorTool_QuadTransform);
	
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
			case CAsset_MapLayerTiles::TypeId:
			case CAsset_MapZoneTiles::TypeId:
				SetCursorTool(m_pCursorTool_TileStamp);
				break;
			case CAsset_MapLayerQuads::TypeId:
				SetCursorTool(m_pCursorTool_QuadTransform);
				break;
		}
	}
}

void CViewMap::RenderView()
{
	CAssetPath MapPath = GetMapPath();
	
	MapRenderer()->SetTime(0.0f);
	MapRenderer()->SetLocalTime(time_get()/(float)time_freq());
	MapRenderer()->SetCanvas(m_ViewRect, vec2(m_ViewRect.x + m_ViewRect.w/2, m_ViewRect.y + m_ViewRect.h/2));
	MapRenderer()->SetCamera(m_CameraPos, m_CameraZoom);
	
	MapRenderer()->RenderMap(MapPath);
	MapRenderer()->RenderMap_Zones(MapPath, AssetsEditor()->m_Path_Image_ZoneTexture);
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
