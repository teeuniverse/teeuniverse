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

#ifndef __CLIENT_MAPRENDERER__
#define __CLIENT_MAPRENDERER__

#include <shared/components/assetsmanager.h>
#include <client/kernel.h>
#include <client/gui/rect.h>

class CMapRenderer : public CClientKernel::CGuest
{
public:
	vec2 m_GroupPos;
	vec2 m_GroupHardParallax;
	
	gui::CRect m_Canvas;
	vec2 m_ScreenPos;
	vec2 m_CameraPos;
	vec2 m_GroupCameraPos;
	float m_CameraZoom;
	double m_Time;
	bool m_Clipping;
	
public:	
	CMapRenderer(CClientKernel* pKernel);
	
	inline void SetTime(double Time) { m_Time = Time; }
	
	inline double GetTime() const { return m_Time; }
	
	void SetCanvas(const gui::CRect& Canvas, vec2 ScreenPos);
	void SetCamera(vec2 CameraPos, float CameraZoom);
	void SetGroup(CAssetPath GroupPath);
	void UnsetGroup();
	
	inline const gui::CRect& GetCanvas() const { return m_Canvas; }
	
	vec2 MapPosToScreenPos(vec2 MapPos) const;
	vec2 MapPosToTilePos(vec2 MapPos) const;
	vec2 ScreenPosToMapPos(vec2 MapPos) const;
	vec2 ScreenPosToTilePos(vec2 MapPos) const;
	vec2 TilePosToMapPos(vec2 MapPos) const;
	vec2 TilePosToScreenPos(vec2 MapPos) const;
	
	void RenderGrid(float Step, vec4 Color);
	void RenderGrid_LayerTiles(CAssetPath LayerPath);
	void RenderTiles_Style(const array2d<CAsset_MapLayerTiles::CTile>& Tiles, vec2 Pos, CAssetPath StylePath, vec4 Color, bool Repeat);
	void RenderTiles_Image(const array2d<CAsset_MapLayerTiles::CTile>& Tiles, vec2 Pos, CAssetPath ImagePath, vec4 Color, bool Repeat);
	void RenderTiles_Zone(CAssetPath ZoneTypePath, const array2d<CAsset_MapLayerTiles::CTile>& Tiles, vec2 Pos, vec4 Color, bool Repeat);
	void RenderTiles_Zone(CAssetPath ZoneTypePath, const array2d<CAsset_MapZoneTiles::CTile>& Tiles, vec2 Pos, vec4 Color, bool Repeat);
	void RenderQuads(const CAsset_MapLayerQuads::CQuad* pQuads, int NbQuads, vec2 Pos, CAssetPath ImagePath, vec4 Color);
	void RenderQuads_Mesh(const CAsset_MapLayerQuads::CQuad* pQuads, int NbQuads);
	void RenderObject(const CAsset_MapLayerObjects::CObject& Object, vec2 Pos, vec4 Color, bool DrawMesh);
	void RenderObjectCurve(const CAsset_MapLayerObjects::CObject& Object, vec2 Pos);
	void RenderObjects(CAssetPath LayerPath, vec2 Pos, bool DrawMesh = false);
	void RenderObjects_Zone(CAssetPath ZoneTypePath, const std::vector<CAsset_MapZoneObjects::CObject>& Objects, vec2 Pos, vec4 Color);
	void RenderZoneIntData(CAssetPath ZonePath, CAssetPath ZoneTypePath);
	
	void RenderGroup(CAssetPath GroupPath, vec4 Color, int LoD, bool DrawMesh = false);
	void RenderMap(CAssetPath MapPath, vec4 Color, int LoD, bool DrawMesh = false);
	
	void RenderMap_Zones(CAssetPath MapPath, vec4 Color);
};

#endif
