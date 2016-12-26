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

#include <shared/geometry/linetesselation.h>
#include <shared/components/assetsmanager.h>
#include <generated/assets/sprite.h>
#include <generated/assets/image.h>
#include <generated/assets/material.h>

void TesselateBezierCurve(array<CBezierVertex>& BezierVertices, array<CLineVertex>& OutputVertices, float MinWidth)
{
	if(BezierVertices.size() <= 0)
		return;
	
	{
		CLineVertex& Vertex = OutputVertices.increment();
		Vertex = BezierVertices[0];
	}
	
	int NumVertices = BezierVertices.size();
	for(int i=1; i<NumVertices; i++)
	{
		vec2 Position0 = BezierVertices[i-1].m_Position;			
		vec2 Position1 = BezierVertices[i].m_Position;
		float Length = distance(Position0, Position1);
		
		float Weight0 = BezierVertices[i-1].m_Weight;
		float Weight1 = BezierVertices[i].m_Weight;
		
		vec4 Color0 = BezierVertices[i-1].m_Color;
		vec4 Color1 = BezierVertices[i].m_Color;
		
		bool Smooth0 = (BezierVertices[i-1].m_Type != CBezierVertex::TYPE_CORNER);
		bool Smooth1 = (BezierVertices[i].m_Type != CBezierVertex::TYPE_CORNER);
		
		if(Smooth0 || Smooth1)
		{
			if(BezierVertices[i-1].m_Type == CBezierVertex::TYPE_AUTOSMOOTH)
				BezierVertices[i-1].m_aControlPoints[1] = Position0 - normalize(ortho(BezierVertices[i-1].m_Thickness)) * Length/3.0f;
			else if(BezierVertices[i-1].m_Type == CBezierVertex::TYPE_CORNER)
				BezierVertices[i-1].m_aControlPoints[1] = Position0;
			
			if(BezierVertices[i].m_Type == CBezierVertex::TYPE_AUTOSMOOTH)
				BezierVertices[i].m_aControlPoints[0] = Position1 + normalize(ortho(BezierVertices[i].m_Thickness)) * Length/3.0f;
			else if(BezierVertices[i].m_Type == CBezierVertex::TYPE_CORNER)
				BezierVertices[i].m_aControlPoints[0] = Position1;
			
			int NbSegments = min((int)(Length/MinWidth), 256); //Small constraint to avoid infinite number of segment in case of corrupted files
			for(int j=1; j<NbSegments; j++)
			{
				float Alpha = static_cast<float>(j)/NbSegments; 
				vec2 Tmp0 = Position0 + (BezierVertices[i-1].m_aControlPoints[1] - Position0)*Alpha;
				vec2 Tmp1 = BezierVertices[i-1].m_aControlPoints[1] + (BezierVertices[i].m_aControlPoints[0] - BezierVertices[i-1].m_aControlPoints[1])*Alpha;
				vec2 Tmp2 = BezierVertices[i].m_aControlPoints[0] + (Position1 - BezierVertices[i].m_aControlPoints[0])*Alpha;
				vec2 Tmp3 = Tmp0 + (Tmp1 - Tmp0)*Alpha;
				vec2 Tmp4 = Tmp1 + (Tmp2 - Tmp1)*Alpha;
				vec2 Point = Tmp3 + (Tmp4 - Tmp3)*Alpha;
				
				CLineVertex& Vertex = OutputVertices.increment();
				Vertex.m_Position = Point;
				Vertex.m_Color = Color0 * (1.0f - Alpha) + Color1 * Alpha;
				Vertex.m_Weight = Weight0 * (1.0f - Alpha) + Weight1 * Alpha;
			}
		}
		{
			CLineVertex& Vertex = OutputVertices.increment();
			Vertex = BezierVertices[i];
		}
	}
}

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

void GenerateMaterialQuads_GetSpriteInfo(const CAssetsManager* pAssetsManager, const CAsset_Material::CSprite* pMaterialSprite, CSpriteInfo& SpriteInfo)
{
	SpriteInfo.m_UMin = 0.0f;
	SpriteInfo.m_UMax = 1.0f;
	SpriteInfo.m_VMin = 0.0f;
	SpriteInfo.m_VMax = 1.0f;
	SpriteInfo.m_Width = 1;
	SpriteInfo.m_Height = 1;
	SpriteInfo.m_ImagePath = CAssetPath::Null();
	
	const CAsset_Sprite* pSprite = pAssetsManager->GetAsset<CAsset_Sprite>(pMaterialSprite->GetPath());
	if(pSprite)
	{
		const CAsset_Image* pImage = pAssetsManager->GetAsset<CAsset_Image>(pSprite->GetImagePath());
		if(pImage)
		{
			int ImageWidth = pImage->GetDataWidth();
			int ImageHeight = pImage->GetDataHeight();
			int GridWidth = pImage->GetGridWidth();
			int GridHeight = pImage->GetGridHeight();
			float TexelSize = static_cast<float>(pImage->GetTexelSize())/1024.0f;
			SpriteInfo.m_ImagePath = pSprite->GetImagePath();
		
			float texStepX = 1.0f/GridWidth;
			float texStepY = 1.0f/GridHeight;
			SpriteInfo.m_UMin = pSprite->GetX()*texStepX;
			SpriteInfo.m_UMax = (pSprite->GetX()+pSprite->GetWidth())*texStepX;
			SpriteInfo.m_VMin = pSprite->GetY()*texStepY;
			SpriteInfo.m_VMax = (pSprite->GetY()+pSprite->GetHeight())*texStepY;
			SpriteInfo.m_Width = TexelSize * pSprite->GetWidth() * static_cast<float>(ImageWidth) / GridWidth;
			SpriteInfo.m_Height = TexelSize * pSprite->GetHeight() * static_cast<float>(ImageHeight) / GridHeight;
		}
	}
	
	float Tmp;
	if(pMaterialSprite->GetFlags() & CAsset_Material::SPRITEFLAG_VFLIP)
	{
		Tmp = SpriteInfo.m_UMin;
		SpriteInfo.m_UMin = SpriteInfo.m_UMax;
		SpriteInfo.m_UMax = Tmp;
	}
	if(pMaterialSprite->GetFlags() & CAsset_Material::SPRITEFLAG_HFLIP)
	{
		Tmp = SpriteInfo.m_VMin;
		SpriteInfo.m_VMin = SpriteInfo.m_VMax;
		SpriteInfo.m_VMax = Tmp;
	}
	if(pMaterialSprite->GetFlags() & CAsset_Material::SPRITEFLAG_ROTATION)
	{
		Tmp = SpriteInfo.m_Width;
		SpriteInfo.m_Width = SpriteInfo.m_Height;
		SpriteInfo.m_Height = Tmp;
	}
	
	SpriteInfo.m_Width *= pMaterialSprite->GetSize().x;
	SpriteInfo.m_Height *= pMaterialSprite->GetSize().y;
}

void GenerateMaterialQuads_RepeatedSprites(const CAssetsManager* pAssetsManager, array<CTexturedQuad>& OutputQuads, const array<CLineVertex>& Vertices, const matrix2x2& Transform, vec2 ObjPosition, const CAsset_Material::CLayer* pLayer)
{
	const array< CAsset_Material::CSprite, allocator_copy<CAsset_Material::CSprite> >& Sprites = pLayer->GetSpriteArray();
	if(Sprites.size() <= 0)
		return;
	
	int SpriteId = 0;
	const CAsset_Material::CSprite* pSprite = &Sprites[SpriteId];
	
	CSpriteInfo SpriteInfo;
	GenerateMaterialQuads_GetSpriteInfo(pAssetsManager, pSprite, SpriteInfo);
	
	float LengthIter = 0.0;
	float LengthCutPos = 0.0;
	for(int i=1; i<Vertices.size(); i++)
	{
		vec2 Position0 = Vertices[i-1].m_Position;			
		vec2 Position1 = Vertices[i].m_Position;
		
		vec2 Dir0 = normalize(Vertices[i-1].m_Thickness);
		vec2 Dir1 = normalize(Vertices[i].m_Thickness);
		vec4 Color0 = Vertices[i-1].m_Color * pSprite->GetColor();
		vec4 Color1 = Vertices[i].m_Color * pSprite->GetColor();
		float Weight0 = Vertices[i-1].m_Weight;
		float Weight1 = Vertices[i].m_Weight;
		
		float Length = distance(Position0, Position1)/((Weight0 + Weight1)/2.0f);
		float NextLengthIter = LengthIter + Length;
		
		bool EndOfSegment = false;
		while(!EndOfSegment)
		{
			float SegmentLength = SpriteInfo.m_Width + pLayer->GetSpacing();
			
			if(LengthCutPos + SegmentLength > NextLengthIter)
			{
				EndOfSegment = true;
				LengthIter = NextLengthIter;
			}
			else
			{
				LengthCutPos += SegmentLength;
				LengthIter = LengthCutPos;
			}
			
			if(!EndOfSegment)
			{
				CTexturedQuad& Quad = OutputQuads.increment();
				
				float Alpha = (NextLengthIter - LengthIter)/Length;
				
				vec2 Pos = Position0 * Alpha + Position1 * (1.0f - Alpha);
				vec4 SpriteColor = Color0 * Alpha + Color1 * (1.0f - Alpha);
				
				//Add position shift
				vec2 DirX = vec2(-1.0f, 0.0f);
				vec2 DirY = vec2(0.0f, -1.0f);
				if(pSprite->GetAlignment() == CAsset_Material::SPRITEALIGN_LINE)
				{
					DirY = normalize(Dir0 * Alpha + Dir1 * (1.0f - Alpha)) * (Weight0 * Alpha + Weight1 * (1.0f - Alpha));
					DirX = -ortho(DirY);
				}
				Pos += DirY * pSprite->GetPosition().x;
				Pos += DirY * pSprite->GetPosition().y;
				
				Quad.m_ImagePath = SpriteInfo.m_ImagePath;
				
				Quad.m_Color[0] = SpriteColor;
				Quad.m_Color[1] = SpriteColor;
				Quad.m_Color[2] = SpriteColor;
				Quad.m_Color[3] = SpriteColor;
								
				if(pSprite->GetFlags() & CAsset_Material::SPRITEFLAG_ROTATION)
				{
					Quad.m_Texture[0] = vec2(SpriteInfo.m_UMin, SpriteInfo.m_VMin);
					Quad.m_Texture[1] = vec2(SpriteInfo.m_UMin, SpriteInfo.m_VMax);
					Quad.m_Texture[2] = vec2(SpriteInfo.m_UMax, SpriteInfo.m_VMin);
					Quad.m_Texture[3] = vec2(SpriteInfo.m_UMax, SpriteInfo.m_VMax);
				}
				else
				{
					Quad.m_Texture[0] = vec2(SpriteInfo.m_UMin, SpriteInfo.m_VMin);
					Quad.m_Texture[1] = vec2(SpriteInfo.m_UMax, SpriteInfo.m_VMin);
					Quad.m_Texture[2] = vec2(SpriteInfo.m_UMin, SpriteInfo.m_VMax);
					Quad.m_Texture[3] = vec2(SpriteInfo.m_UMax, SpriteInfo.m_VMax);
				}
				
				Quad.m_Position[0] = ObjPosition + Transform*(Pos + DirX * SpriteInfo.m_Width/2.0f + DirY * SpriteInfo.m_Height/2.0f);
				Quad.m_Position[1] = ObjPosition + Transform*(Pos - DirX * SpriteInfo.m_Width/2.0f + DirY * SpriteInfo.m_Height/2.0f);
				Quad.m_Position[2] = ObjPosition + Transform*(Pos + DirX * SpriteInfo.m_Width/2.0f - DirY * SpriteInfo.m_Height/2.0f);
				Quad.m_Position[3] = ObjPosition + Transform*(Pos - DirX * SpriteInfo.m_Width/2.0f - DirY * SpriteInfo.m_Height/2.0f);
				
				//Switch to the next sprite
				if(Sprites.size() > 1)
				{
					SpriteId = (SpriteId+1)%Sprites.size();
					pSprite = &Sprites[SpriteId];
					GenerateMaterialQuads_GetSpriteInfo(pAssetsManager, pSprite, SpriteInfo);
				}
			}
		}
		
		LengthIter = NextLengthIter;
	}
}

void GenerateMaterialQuads_StretchedQuads(const CAssetsManager* pAssetsManager, array<CTexturedQuad>& OutputQuads, const array<CLineVertex>& Vertices, const matrix2x2& Transform, vec2 ObjPosition, const CAsset_Material::CLayer* pLayer)
{
	const array< CAsset_Material::CSprite, allocator_copy<CAsset_Material::CSprite> >& Sprites = pLayer->GetSpriteArray();
	if(Sprites.size() <= 0)
		return;
	
	int SpriteId = 0;
	const CAsset_Material::CSprite* pSprite = &Sprites[SpriteId];
	
	CSpriteInfo SpriteInfo;
	GenerateMaterialQuads_GetSpriteInfo(pAssetsManager, pSprite, SpriteInfo);
		
	float USize = SpriteInfo.m_UMax - SpriteInfo.m_UMin;
	float VSize = SpriteInfo.m_VMax - SpriteInfo.m_VMin;
	
	float LengthIter = 0.0;
	float LengthCutPos = 0.0;
	for(int i=1; i<Vertices.size(); i++)
	{
		vec2 Position0 = Vertices[i-1].m_Position;			
		vec2 Position1 = Vertices[i].m_Position;
		vec2 Ortho0 = Vertices[i-1].m_Thickness;
		vec2 Ortho1 = Vertices[i].m_Thickness;
		vec4 Color0 = Vertices[i-1].m_Color;
		vec4 Color1 = Vertices[i].m_Color;
		float Weight0 = Vertices[i-1].m_Weight;
		float Weight1 = Vertices[i].m_Weight;
		
		float Length = distance(Position0, Position1)/((Weight0 + Weight1)/2.0f);
		float NextLengthIter = LengthIter + Length;
		
		vec2 PositionAlphaPrev = Position0;
		vec2 OrthoAlphaPrev = Ortho0;
		vec4 ColorAlphaPrev = Color0;
		vec2 WeightAlphaPrev = Weight0;
		
		bool EndOfSegment = false;
		while(!EndOfSegment)
		{
			float SegmentLength = SpriteInfo.m_Width;
			
			float TextureBegin;
			float TextureEnd;
			if(LengthCutPos + SegmentLength > NextLengthIter)
			{
				EndOfSegment = true;
				TextureBegin = (LengthIter - LengthCutPos)/SegmentLength;
				TextureEnd = (NextLengthIter - LengthCutPos)/SegmentLength;
				LengthIter = NextLengthIter;
			}
			else
			{
				TextureBegin = (LengthIter - LengthCutPos)/SegmentLength;
				TextureEnd = 1.0f;
				LengthCutPos += SegmentLength;
				LengthIter = LengthCutPos;
			}
			
			float Alpha = (NextLengthIter - LengthIter)/Length;
			
			vec2 PositionAlpha = Position0 * Alpha + Position1 * (1.0f - Alpha);
			vec2 OrthoAlpha = Ortho0 * Alpha + Ortho1 * (1.0f - Alpha);
			vec4 ColorAlpha = Color0 * Alpha + Color1 * (1.0f - Alpha);
			vec2 WeightAlpha = Weight0 * Alpha + Weight1 * (1.0f - Alpha);
			
			int VerticalTesselation = 4;
			if(SpriteInfo.m_ImagePath.IsNull())
				VerticalTesselation = 1;
			for(int k=0; k<VerticalTesselation; k++)
			{
				CTexturedQuad& Quad = OutputQuads.increment();
				
				float StepMin = -(2.0f * static_cast<float>(k)/VerticalTesselation - 1.0f);
				float StepMax = -(2.0f * static_cast<float>(k+1)/VerticalTesselation - 1.0f);
				float VMin = static_cast<float>(k)/VerticalTesselation;
				float VMax = static_cast<float>(k+1)/VerticalTesselation;
				
				Quad.m_ImagePath = SpriteInfo.m_ImagePath;
				
				Quad.m_Color[0] = ColorAlphaPrev * pSprite->GetColor();
				Quad.m_Color[1] = ColorAlpha * pSprite->GetColor();
				Quad.m_Color[2] = ColorAlphaPrev * pSprite->GetColor();
				Quad.m_Color[3] = ColorAlpha * pSprite->GetColor();
				
				if(pSprite->GetFlags() & CAsset_Material::SPRITEFLAG_ROTATION)
				{
					Quad.m_Texture[0] = vec2(SpriteInfo.m_UMin + USize * VMin, SpriteInfo.m_VMax - VSize * TextureBegin);
					Quad.m_Texture[1] = vec2(SpriteInfo.m_UMin + USize * VMin, SpriteInfo.m_VMax - VSize * TextureEnd);
					Quad.m_Texture[2] = vec2(SpriteInfo.m_UMin + USize * VMax, SpriteInfo.m_VMax - VSize * TextureBegin);
					Quad.m_Texture[3] = vec2(SpriteInfo.m_UMin + USize * VMax, SpriteInfo.m_VMax - VSize * TextureEnd);
				}
				else
				{
					Quad.m_Texture[0] = vec2(SpriteInfo.m_UMin + USize * TextureBegin, SpriteInfo.m_VMin + VSize * VMin);
					Quad.m_Texture[1] = vec2(SpriteInfo.m_UMin + USize * TextureEnd, SpriteInfo.m_VMin + VSize * VMin);
					Quad.m_Texture[2] = vec2(SpriteInfo.m_UMin + USize * TextureBegin, SpriteInfo.m_VMin + VSize * VMax);
					Quad.m_Texture[3] = vec2(SpriteInfo.m_UMin + USize * TextureEnd, SpriteInfo.m_VMin + VSize * VMax);
				}
				
				Quad.m_Position[0] = ObjPosition + Transform*(PositionAlphaPrev + OrthoAlphaPrev * StepMin * SpriteInfo.m_Height/2.0f);
				Quad.m_Position[1] = ObjPosition + Transform*(PositionAlpha + OrthoAlpha * StepMin * SpriteInfo.m_Height/2.0f);
				Quad.m_Position[2] = ObjPosition + Transform*(PositionAlphaPrev + OrthoAlphaPrev * StepMax * SpriteInfo.m_Height/2.0f);
				Quad.m_Position[3] = ObjPosition + Transform*(PositionAlpha + OrthoAlpha * StepMax * SpriteInfo.m_Height/2.0f);
			}
			
			PositionAlphaPrev = PositionAlpha;
			OrthoAlphaPrev = OrthoAlpha;
			ColorAlphaPrev = ColorAlpha;
			WeightAlphaPrev = WeightAlpha;
			
			//Switch to the next sprite
			if(!EndOfSegment && Sprites.size() > 1)
			{
				SpriteId = (SpriteId+1)%Sprites.size();
				pSprite = &Sprites[SpriteId];
				GenerateMaterialQuads_GetSpriteInfo(pAssetsManager, pSprite, SpriteInfo);
			}
		}
	}
}

void GenerateMaterialQuads(const CAssetsManager* pAssetsManager, array<CTexturedQuad>& OutputQuads, const array<CLineVertex>& Vertices, const matrix2x2& Transform, vec2 ObjPosition, CAssetPath MaterialPath, bool Closed)
{
	const CAsset_Material* pMaterial = pAssetsManager->GetAsset<CAsset_Material>(MaterialPath);
	if(!pMaterial)
		return;
	
	//Render polygon
	if(Closed && pMaterial->GetTextureEnabled())
	{
		if(Vertices.size() >= 3)
		{
			float TextureSize = 1.0f;
			
			const CAsset_Image* pImage = pAssetsManager->GetAsset<CAsset_Image>(pMaterial->GetTexturePath());
			if(pImage)
				TextureSize = pImage->GetDataWidth() * pImage->GetTexelSize() / 1024.0f;
			
			int V0 = 0%Vertices.size();
			int V1 = 1%Vertices.size();
			int V2 = 3%Vertices.size();
			int V3 = 2%Vertices.size();
			
			int VerticesLeft = Vertices.size() - 2;
			do
			{
				V1 = V0;
				V3 = V2;
				V0 = (V0 + Vertices.size() - 1)%Vertices.size();
				V2 = (V2 + 1)%Vertices.size();
				VerticesLeft = VerticesLeft-2;
				
				CTexturedQuad& Quad = OutputQuads.increment();
				
				Quad.m_Position[0] = ObjPosition + Transform*(Vertices[V0].m_Position);
				Quad.m_Position[1] = ObjPosition + Transform*(Vertices[V1].m_Position);
				Quad.m_Position[2] = ObjPosition + Transform*(Vertices[V2].m_Position);
				Quad.m_Position[3] = ObjPosition + Transform*(Vertices[V3].m_Position);
				
				Quad.m_Texture[0] = rotate(Quad.m_Position[0]/(pMaterial->GetTextureSize() * TextureSize), pMaterial->GetTextureAngle());
				Quad.m_Texture[1] = rotate(Quad.m_Position[1]/(pMaterial->GetTextureSize() * TextureSize), pMaterial->GetTextureAngle());
				Quad.m_Texture[2] = rotate(Quad.m_Position[2]/(pMaterial->GetTextureSize() * TextureSize), pMaterial->GetTextureAngle());
				Quad.m_Texture[3] = rotate(Quad.m_Position[3]/(pMaterial->GetTextureSize() * TextureSize), pMaterial->GetTextureAngle());
				
				Quad.m_Color[0] = pMaterial->GetTextureColor();
				Quad.m_Color[1] = pMaterial->GetTextureColor();
				Quad.m_Color[2] = pMaterial->GetTextureColor();
				Quad.m_Color[3] = pMaterial->GetTextureColor();
				
				Quad.m_ImagePath = pMaterial->GetTexturePath();
			}
			while(VerticesLeft > 0);
		}
	}
	
	//Render line layers
	CAsset_Material::CIteratorLayer LayerIter;
	for(LayerIter = pMaterial->BeginLayer(); LayerIter != pMaterial->EndLayer(); ++LayerIter)
	{
		const CAsset_Material::CLayer* pLayer = &pMaterial->GetLayer(*LayerIter);
			
		if(pLayer->GetRepeatType() == CAsset_Material::REPEATTYPE_STATIC)
			GenerateMaterialQuads_RepeatedSprites(pAssetsManager, OutputQuads, Vertices, Transform, ObjPosition, pLayer);
		else if(pLayer->GetRepeatType() == CAsset_Material::REPEATTYPE_STRETCH)
			GenerateMaterialQuads_StretchedQuads(pAssetsManager, OutputQuads, Vertices, Transform, ObjPosition, pLayer);
	}
}

void GenerateMaterialQuads_Object(class CAssetsManager* pAssetsManager, float Time, array<CTexturedQuad>& OutputQuads, const CAsset_MapLayerObjects::CObject& Object)
{
	const array< CAsset_MapLayerObjects::CVertex, allocator_copy<CAsset_MapLayerObjects::CVertex> >& ObjectVertices = Object.GetVertexArray();
	bool Closed = Object.GetClosedPath();
	vec2 ObjPosition;
	matrix2x2 Transform;
	Object.GetTransform(pAssetsManager, Time, &Transform, &ObjPosition);
	
	array<CBezierVertex> BezierVertices;
	array<CLineVertex> LineVertices;
	
	for(int i=0; i<ObjectVertices.size(); i++)
	{
		CBezierVertex& Vertex = BezierVertices.increment();
		Vertex.m_Position = ObjectVertices[i].GetPosition();
		Vertex.m_Weight = ObjectVertices[i].GetWeight();
		Vertex.m_Color = ObjectVertices[i].GetColor();
		Vertex.m_Type = ObjectVertices[i].GetSmoothness();
		Vertex.m_aControlPoints[0] = Vertex.m_Position + ObjectVertices[i].GetControlPoint0();
		Vertex.m_aControlPoints[1] = Vertex.m_Position + ObjectVertices[i].GetControlPoint1();
		if(Vertex.m_Type >= CBezierVertex::NUM_TYPES || Vertex.m_Type < 0)
			Vertex.m_Type = CBezierVertex::TYPE_CORNER;
	}
	if(Closed && BezierVertices.size() > 0)
	{
		CBezierVertex& Vertex = BezierVertices.increment();
		Vertex = BezierVertices[0];
	}
	
	ComputeLineNormals<CBezierVertex>(BezierVertices, Closed);
	TesselateBezierCurve(BezierVertices, LineVertices, 32.0f);
	ComputeLineNormals<CLineVertex>(LineVertices, Closed);
	
	GenerateMaterialQuads(pAssetsManager, OutputQuads, LineVertices, Transform, ObjPosition, Object.GetStylePath(), Closed);
}
