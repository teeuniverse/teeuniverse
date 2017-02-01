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

#ifndef __SHARED_GEOMETRY_LINETESSELATION__
#define __SHARED_GEOMETRY_LINETESSELATION__

#include <shared/math/vector.h>
#include <shared/geometry/quad.h>
#include <generated/assets/maplayerobjects.h>
#include <generated/assets/mapzoneobjects.h>
#include <generated/assets/material.h>
#include <generated/assets/zonetype.h>

void TesselateBezierCurve(const std::vector<CBezierVertex>& BezierVertices, std::vector<CLineVertex>& OutputVertices, float MinWidth);

void PolygonQuadrangulation(const std::vector<CLineVertex>& Vertices, std::vector<CQuad>& OutputQuads);

class CSpriteInfo
{
public:
	CAssetPath m_ImagePath;
	float m_Width;
	float m_Height;
	float m_UMin;
	float m_UMax;
	float m_VMin;
	float m_VMax;
};

void GenerateMaterialQuads_GetSpriteInfo(const CAssetsManager* pAssetsManager, const CAsset_Material::CSprite* pMaterialSprite, CSpriteInfo& SpriteInfo);
void GenerateMaterialQuads(
	const class CAssetsManager* pAssetsManager,
	std::vector<CTexturedQuad>& OutputQuads,
	const std::vector<CLineVertex>& Vertices,
	const matrix2x2& Transform,
	vec2 ObjPosition,
	CAssetPath MaterialPath,
	bool Closed,
	bool ShowLine,
	int OrthoTesselation
);

void GenerateZoneQuads(
	const class CAssetsManager* pAssetsManager,
	std::vector<CTexturedQuad>& OutputQuads,
	const std::vector<CLineVertex>& Vertices,
	const matrix2x2& Transform,
	vec2 ObjPosition,
	CAssetPath ZoneTypePath,
	int Index,
	bool Closed
);

void GenerateMaterialCurve_Object(class CAssetsManager* pAssetsManager, float Time, std::vector<CLineVertex>& OutputLines, const CAsset_MapLayerObjects::CObject& Object);
void GenerateMaterialQuads_Object(class CAssetsManager* pAssetsManager, float Time, std::vector<CTexturedQuad>& OutputQuads, const CAsset_MapLayerObjects::CObject& Object);

void GenerateZoneCurve_Object(class CAssetsManager* pAssetsManager, float Time, std::vector<CLineVertex>& OutputLines, const CAsset_MapZoneObjects::CObject& Object);
void GenerateZoneQuads_Object(class CAssetsManager* pAssetsManager, float Time, std::vector<CTexturedQuad>& OutputQuads, const CAsset_MapZoneObjects::CObject& Object, CAssetPath ZoneTypePath);

//If the parameter "Closed" is true, the first and last vertices must be equals
template<typename VERTEX>
void ComputeLineNormals(std::vector<VERTEX>& Vertices, bool Closed)
{
	int NumVertices = Vertices.size();
	for(int i=0; i<NumVertices; i++)
	{
		vec2 Position = Vertices[i].m_Position;
		vec2 DirLeft = 0.0f;
		vec2 DirRight = 0.0f;
		
		if(i>0)
			DirLeft = normalize(Position - Vertices[i-1].m_Position);
		else if(Closed && NumVertices >= 2)
			DirLeft = normalize(Position - Vertices[NumVertices-2].m_Position);
		
		if(i+1<NumVertices)
			DirRight = normalize(Vertices[i+1].m_Position - Position);
		else if(Closed && NumVertices >= 2)
			DirRight = normalize(Vertices[1].m_Position - Position);
		
		vec2 Dir = normalize(DirLeft + DirRight);	
		vec2 OrthoDir = ortho(Dir);
		
		float Length = 1.0f;
		if(length(DirLeft) > 0.0f)
			Length /= dot(OrthoDir, ortho(DirLeft));
		else if(length(DirRight))
			Length /= dot(OrthoDir, ortho(DirRight));
		
		Vertices[i].m_Thickness = OrthoDir * Length;
	}
}

#endif
