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
#include <generated/assets/pathmaterial.h>

void TesselateBezierCurve(std::vector<CBezierVertex>& BezierVertices, std::vector<CLineVertex>& OutputVertices, float MinWidth)
{
	if(BezierVertices.size() <= 0)
		return;
	
	{
		OutputVertices.push_back(BezierVertices[0]);
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
				
				OutputVertices.emplace_back();
				CLineVertex& Vertex = OutputVertices.back();
				Vertex.m_Position = Point;
				Vertex.m_Color = Color0 * (1.0f - Alpha) + Color1 * Alpha;
				Vertex.m_Weight = Weight0 * (1.0f - Alpha) + Weight1 * Alpha;
			}
		}
		{
			OutputVertices.push_back(BezierVertices[i]);
		}
	}
}

void PolygonQuadrangulation(const std::vector<CLineVertex>& Vertices, std::vector<CQuad>& OutputQuads)
{
	if(Vertices.size() < 3)
		return;
	
	vec2 Barycenter = vec2(0.0f, 0.0f);
	for(unsigned int i=0; i<Vertices.size(); i++)
		Barycenter += Vertices[i].m_Position;
	Barycenter = Barycenter / Vertices.size();
	
	int i=0;
	do
	{
		int V0 = i;
		int V1 = i+1;
		int V2 = min(i+2, static_cast<int>(Vertices.size())-1);
		
		OutputQuads.emplace_back();
		CQuad& Quad = OutputQuads.back();
		
		Quad.m_Position[0] = Vertices[V0].m_Position;
		Quad.m_Position[1] = Vertices[V1].m_Position;
		Quad.m_Position[2] = Barycenter;
		Quad.m_Position[3] = Vertices[V2].m_Position;
		
		i += 2;
	}
	while(i < static_cast<int>(Vertices.size())-1);
}

void GenerateMaterialQuads_GetSpriteInfo(const CAssetsManager* pAssetsManager, const CAsset_PathMaterial::CSprite* pMaterialSprite, CSpriteInfo& SpriteInfo)
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
			float texSpacingX = pImage->GetGridSpacing()/static_cast<float>(ImageWidth);
			float texSpacingY = pImage->GetGridSpacing()/static_cast<float>(ImageHeight);
			SpriteInfo.m_UMin = pSprite->GetX()*texStepX + texSpacingX;
			SpriteInfo.m_UMax = (pSprite->GetX()+pSprite->GetWidth())*texStepX - texSpacingX;
			SpriteInfo.m_VMin = pSprite->GetY()*texStepY + texSpacingY;
			SpriteInfo.m_VMax = (pSprite->GetY()+pSprite->GetHeight())*texStepY - texSpacingY;
			SpriteInfo.m_Width = TexelSize * (pSprite->GetWidth() * static_cast<float>(ImageWidth) / GridWidth - 2*pImage->GetGridSpacing());
			SpriteInfo.m_Height = TexelSize * (pSprite->GetHeight() * static_cast<float>(ImageHeight) / GridHeight - 2*pImage->GetGridSpacing());
		}
	}
	
	SpriteInfo.m_Width *= pMaterialSprite->GetSize().x;
	SpriteInfo.m_Height *= pMaterialSprite->GetSize().y;
	
	float Tmp;
	if((pMaterialSprite->GetFlags() & CAsset_PathMaterial::SPRITEFLAG_VFLIP) || SpriteInfo.m_Width < 0)
	{
		Tmp = SpriteInfo.m_UMin;
		SpriteInfo.m_UMin = SpriteInfo.m_UMax;
		SpriteInfo.m_UMax = Tmp;
	}
	if((pMaterialSprite->GetFlags() & CAsset_PathMaterial::SPRITEFLAG_HFLIP) || SpriteInfo.m_Height < 0)
	{
		Tmp = SpriteInfo.m_VMin;
		SpriteInfo.m_VMin = SpriteInfo.m_VMax;
		SpriteInfo.m_VMax = Tmp;
	}
	if(pMaterialSprite->GetFlags() & CAsset_PathMaterial::SPRITEFLAG_ROTATION)
	{
		Tmp = SpriteInfo.m_Width;
		SpriteInfo.m_Width = SpriteInfo.m_Height;
		SpriteInfo.m_Height = Tmp;
	}
	
	SpriteInfo.m_Width = std::abs(SpriteInfo.m_Width);
	SpriteInfo.m_Height = std::abs(SpriteInfo.m_Height);
}

class CLineIterator
{
public:
	class CSegment
	{
	public:
		int m_StartVert;
		int m_EndVert;
		float m_MeanAngle;
		float m_StartAngle;
		float m_EndAngle;
		bool m_Closed;
	};
	
protected:
	const std::vector<CLineVertex>& m_Vertices;
	std::vector<CSegment> m_Segments;
	bool m_Closed;
	int m_CurrSegment;
	int m_CurrVert;
	float m_VertexAlpha;
	
public:
	CLineIterator(const std::vector<CLineVertex>& Vertices, bool Closed) :
		m_Vertices(Vertices),
		m_Closed(Closed),
		m_CurrSegment(-1),
		m_CurrVert(0),
		m_VertexAlpha(0.0f)
	{
		if(Vertices.size() > 1)
		{
			//Get the first broken vertex
			int FirstVert = 0;
			if(m_Closed)
			{
				do
				{
					if(IsBrokenVertex(FirstVert))
						break;
					GetPrevVertex(FirstVert);
				}
				while(FirstVert > 0);
			}
			
			//Create the list of segments
			int CurrVert = FirstVert;
			bool ContinueSegementGeneration = true;
			while(ContinueSegementGeneration)
			{
				ContinueSegementGeneration = false;
				
				m_Segments.emplace_back();
				CSegment& Segment = m_Segments.back();
				Segment.m_StartVert = CurrVert;
				Segment.m_EndVert = CurrVert;
				Segment.m_StartAngle = 0.0f;
				Segment.m_EndAngle = 0.0f;
				Segment.m_Closed = false;
				
				vec2 Angle = vec2(0.0f, 0.0f);
				
				while(GetNextVertex(Segment.m_EndVert))
				{
					//Sum up all directions with a weight (the length) 
					Angle += m_Vertices[Segment.m_EndVert].m_Position - m_Vertices[Segment.m_StartVert].m_Position;
					
					if(Segment.m_EndVert == FirstVert)
					{
						if(Segment.m_StartVert == Segment.m_EndVert && IsBrokenVertex(Segment.m_StartVert))
							Segment.m_Closed = true;
						break;
					}
					
					if(IsBrokenVertex(Segment.m_EndVert))
					{
						ContinueSegementGeneration = true;
						break;
					}
				}
				
				Segment.m_MeanAngle = angle(normalize(Angle));
				
				{
					int PrevId = Segment.m_EndVert;
					int NextId = Segment.m_EndVert;
					
					if(GetPrevVertex(PrevId) && GetNextVertex(NextId))
					{
						vec2 LineDir0 = normalize(m_Vertices[Segment.m_EndVert].m_Position - m_Vertices[PrevId].m_Position);
						vec2 LineDir1 = normalize(m_Vertices[NextId].m_Position - m_Vertices[Segment.m_EndVert].m_Position);
						Segment.m_EndAngle = angle(LineDir0, LineDir1);
					}
				}
				{
					int PrevId = Segment.m_StartVert;
					int NextId = Segment.m_StartVert;
					
					if(GetPrevVertex(PrevId) && GetNextVertex(NextId))
					{
						vec2 LineDir0 = normalize(m_Vertices[Segment.m_StartVert].m_Position - m_Vertices[PrevId].m_Position);
						vec2 LineDir1 = normalize(m_Vertices[NextId].m_Position - m_Vertices[Segment.m_StartVert].m_Position);
						Segment.m_StartAngle = angle(LineDir0, LineDir1);
					}
				}
				
				CurrVert = Segment.m_EndVert;
			}
		}
	}
	
	inline bool GetPrevVertex(int& CurrId) const
	{
		if(m_Closed)
		{
			if(CurrId == 0)
				CurrId = m_Vertices.size()-2;
			else
				CurrId--;
		}
		else
		{
			if(CurrId == 0)
				return false;
			else
				CurrId--;
		}
		
		return true;
	}
	
	inline bool GetNextVertex(int& CurrId) const
	{
		if(m_Closed)
		{
			if(CurrId >= static_cast<int>(m_Vertices.size())-2)
				CurrId = 0;
			else
				CurrId++;
		}
		else
		{
			if(CurrId >= static_cast<int>(m_Vertices.size())-1)
				return false;
			else
				CurrId++;
		}
		
		return true;
	}
	
	inline bool IsBrokenVertex(int CurrId) const
	{
		int PrevId = CurrId;
		int NextId = CurrId;
		
		if(!GetPrevVertex(PrevId) || !GetNextVertex(NextId))
			return false;
		
		vec2 LineDir0 = normalize(m_Vertices[CurrId].m_Position - m_Vertices[PrevId].m_Position);
		vec2 LineDir1 = normalize(m_Vertices[NextId].m_Position - m_Vertices[CurrId].m_Position);
		float Angle = angle(LineDir0, LineDir1);
		return fabs(Angle) > (Pi/5.0f);
	}
	
	inline bool NextSegment()
	{
		m_CurrSegment++;
		if(m_CurrSegment >= static_cast<int>(m_Segments.size()))
			return false;
		
		m_CurrVert = m_Segments[m_CurrSegment].m_StartVert;
		m_VertexAlpha = 0.0f;
		
		return true;
	}
	
	inline const CSegment* GetSegment() const
	{
		return &m_Segments[m_CurrSegment];
	}
	
	inline const CSegment* GetNextSegment() const
	{
		if(!m_Segments.size())
			return NULL;
		
		if(m_Closed)
			return &m_Segments[(m_CurrSegment+1)%m_Segments.size()];
		else if(m_CurrSegment+1 < static_cast<int>(m_Segments.size()))
			return &m_Segments[m_CurrSegment+1];
		else
			return NULL;
	}
	
	inline float GetSegmentLength() const
	{
		assert(m_CurrSegment >= 0);
		
		float Length = 0.0f;
		
		int StartVert = m_Segments[m_CurrSegment].m_StartVert;
		int EndVert = m_Segments[m_CurrSegment].m_EndVert;
		
		int PrevVert = StartVert;
		int NextVert = PrevVert;
		while(GetNextVertex(NextVert))
		{
			vec2 Pos0 = m_Vertices[PrevVert].m_Position;
			vec2 Pos1 = m_Vertices[NextVert].m_Position;
			Length += distance(Pos0, Pos1);
			
			if(NextVert == EndVert)
				break;
			
			PrevVert = NextVert;
		}
		
		return Length;
	}
	
	inline bool IsStartVertex(int Vert) const
	{
		return Vert == m_Segments[m_CurrSegment].m_StartVert;
	}
	
	inline bool IsEndVertex(int Vert) const
	{
		return Vert == m_Segments[m_CurrSegment].m_EndVert;
	}
	
	inline bool IsClosedSegment() const
	{
		return m_Segments[m_CurrSegment].m_Closed;
	}
	
	enum
	{
		LINECURSORSTATE_START=0,
		LINECURSORSTATE_END,
		LINECURSORSTATE_VERTEX,
		LINECURSORSTATE_ERROR,
	};
	
	int MoveLineCursor_Forward(float WantedDistance, float& WalkedDistance, int& PrevVert, int& NextVert, float& PrevAlpha, float& NextAlpha, bool StopOnVertex)
	{
		assert(m_CurrSegment >= 0);
		
		WalkedDistance = 0.0f;
		PrevVert = m_CurrVert;
		NextVert = m_CurrVert;
		if(!GetNextVertex(NextVert))
			return LINECURSORSTATE_ERROR;
		
		//1024 is just to make sure that we can go out of this loop in any case.
		for(int i=0; i<1024; i++)
		{
			PrevAlpha = m_VertexAlpha;
			
			vec2 Pos0 = m_Vertices[PrevVert].m_Position;
			vec2 Pos1 = m_Vertices[NextVert].m_Position;
			float Length = distance(Pos0, Pos1);
			float LengthLeft = Length*(1.0f - m_VertexAlpha);
			
			//There is enough distance to finish the walk
			if(LengthLeft > WantedDistance)
			{
				WalkedDistance += WantedDistance;
				m_VertexAlpha = 1.0f - (LengthLeft - WantedDistance)/Length;
				NextAlpha = m_VertexAlpha;
				return LINECURSORSTATE_END;
			}
			//This is the last part of the segment, we extropolate to finish the walk
			else if(NextVert == m_Segments[m_CurrSegment].m_EndVert)
			{
				WalkedDistance += WantedDistance;
				m_VertexAlpha = (Length*m_VertexAlpha + WantedDistance)/Length;
				NextAlpha = m_VertexAlpha;
				return LINECURSORSTATE_END;
			}
			//If needed, we stop the walk because we reach a vertex
			else if(StopOnVertex)
			{
				WalkedDistance += LengthLeft;
				WantedDistance -= LengthLeft;
				NextAlpha = 1.0f;
				
				m_CurrVert = NextVert;
				m_VertexAlpha = 0.0f;
				
				return LINECURSORSTATE_VERTEX;
			}
			//Go to the next vertex and continue
			else
			{
				WalkedDistance += LengthLeft;
				WantedDistance -= LengthLeft;
				NextAlpha = 1.0f;
				
				m_CurrVert = NextVert;
				m_VertexAlpha = 0.0f;
				
				PrevVert = NextVert;
				if(!GetNextVertex(NextVert))
					return LINECURSORSTATE_ERROR;
			}
		}
		
		return LINECURSORSTATE_ERROR;
	}
	
	int MoveLineCursor_Backward(float WantedDistance, float& WalkedDistance, int& PrevVert, int& NextVert, float& PrevAlpha, float& NextAlpha, bool StopOnVertex)
	{
		assert(m_CurrSegment >= 0);
		
		WalkedDistance = 0.0f;
		PrevVert = m_CurrVert;
		NextVert = m_CurrVert;
		if(!GetNextVertex(NextVert))
			return LINECURSORSTATE_ERROR;
		
		//1024 is just to make sure that we can go out of this loop in any case.
		for(int i=0; i<1024; i++)
		{
			NextAlpha = m_VertexAlpha;
			
			vec2 Pos0 = m_Vertices[PrevVert].m_Position;
			vec2 Pos1 = m_Vertices[NextVert].m_Position;
			float Length = distance(Pos0, Pos1);
			float LengthLeft = Length*m_VertexAlpha;
			
			//There is enough distance to finish the walk
			if(LengthLeft > WantedDistance)
			{
				WalkedDistance += WantedDistance;
				m_VertexAlpha = (LengthLeft - WantedDistance)/Length;
				PrevAlpha = m_VertexAlpha;
				return LINECURSORSTATE_END;
			}
			//This is the last part of the segment, we extropolate to finish the walk
			else if(PrevVert == m_Segments[m_CurrSegment].m_StartVert)
			{
				WalkedDistance += WantedDistance;
				m_VertexAlpha = (Length*(m_VertexAlpha) - WantedDistance)/Length;
				PrevAlpha = m_VertexAlpha;
				return LINECURSORSTATE_END;
			}
			//If needed, we stop the walk because we reach a vertex
			else if(StopOnVertex)
			{
				WalkedDistance += LengthLeft;
				WantedDistance -= LengthLeft;
				PrevAlpha = 0.0f;
				
				m_CurrVert = PrevVert;
				m_VertexAlpha = 1.0f;
				
				return LINECURSORSTATE_VERTEX;
			}
			//Go to the next vertex and continue
			else
			{
				WalkedDistance += LengthLeft;
				WantedDistance -= LengthLeft;
				PrevAlpha = 0.0f;
				
				m_CurrVert = PrevVert;
				m_VertexAlpha = 1.0f;
				
				NextVert = PrevVert;
				if(!GetPrevVertex(PrevVert))
					return LINECURSORSTATE_ERROR;
			}
		}
		
		return LINECURSORSTATE_ERROR;
	}
	
	int MoveLineCursor(float WantedDistance, float& WalkedDistance, int& PrevVert, int& NextVert, float& PrevAlpha, float& NextAlpha, bool StopOnVertex)
	{
		if(WantedDistance < 0.0f)
		{
			int Res = MoveLineCursor_Backward(-WantedDistance, WalkedDistance, PrevVert, NextVert, PrevAlpha, NextAlpha, StopOnVertex);
			WalkedDistance = -WalkedDistance;
			return Res;
		}
		else
			return MoveLineCursor_Forward(WantedDistance, WalkedDistance, PrevVert, NextVert, PrevAlpha, NextAlpha, StopOnVertex);
	}
};

struct CLineTile
{
	int m_ErrorLevel;
	int m_Id;
	float m_Width;
	bool m_FillingEnabled;
};

int FindSprite(const std::vector<CAsset_PathMaterial::CSprite>& Sprites, int Type, const std::vector<int>& AcceptedLabels0, const std::vector<int>& AcceptedLabels1)
{
	//Try to get a start tile
	for(int t=0; t<3; t++)
	{
		for(unsigned int i=0; i<Sprites.size(); i++)
		{
			if(Sprites[i].GetTileType() != Type)
				continue;
			
			if(t <= 0)
			{
				bool Found = false;
				for(unsigned int l=0; l<AcceptedLabels1.size(); l++)
				{
					if(Sprites[i].GetTileLabel1() == AcceptedLabels1[l])
					{
						Found = true;
						break;
					}
				}
				if(!Found)
					continue;
			}
			
			if(t == 1)
			{
				bool Found = false;
				for(unsigned int l=0; l<AcceptedLabels0.size(); l++)
				{
					if(Sprites[i].GetTileLabel0() == AcceptedLabels0[l])
					{
						Found = true;
						break;
					}
				}
				if(!Found)
					continue;
			}
			
			return i;
		}
	}
	
	return -1;
}

void GetAcceptableLabels(const CAsset_PathMaterial* pMaterial, float MeanAngle, std::vector<int>& AcceptedLabels)
{
	CAsset_PathMaterial::CIteratorLabel LabelIter;
	for(LabelIter = pMaterial->BeginLabel(); LabelIter != pMaterial->EndLabel(); ++LabelIter)
	{
		float AngleRange0 = pMaterial->GetLabelAngleStart(*LabelIter);
		float AngleRange1 = pMaterial->GetLabelAngleEnd(*LabelIter);
		if(AngleRange0 > AngleRange1)
		{
			float Tmp = AngleRange0;
			AngleRange0 = AngleRange1;
			AngleRange1 = Tmp;
		}
		
		float AngleTest = MeanAngle;
		while(AngleTest < AngleRange1)
		{
			if(AngleTest >= AngleRange0)
			{
				AcceptedLabels.push_back((*LabelIter).GetId());
				break;
			}
			AngleTest += 2.0f*Pi;
		}
	}
}

void GenerateMaterialQuads_Line(
	const CAssetsManager* pAssetsManager,
	std::vector<CTexturedQuad>& OutputQuads,
	const std::vector<CLineVertex>& Vertices,
	const matrix2x2& Transform,
	vec2 ObjPosition,
	const CAsset_PathMaterial* pMaterial,
	const CAsset_PathMaterial::CLayer* pLayer,
	bool Closed,
	int OrthoTesselation
)
{
	const std::vector<CAsset_PathMaterial::CSprite>& Sprites = pLayer->GetSpriteArray();
	if(Sprites.size() <= 0)
		return;
	
	CLineIterator LineIterator(Vertices, Closed);
	
	int FirstTileFromLastSegment = -1;
	float TiledLengthFromLastSegment = 0.0f;
	std::vector<int> PreviousLabel;
	while(LineIterator.NextSegment())
	{
		CSpriteInfo SpriteInfo;
		std::vector<CLineTile> Tiling;
		float SegmentLength = LineIterator.GetSegmentLength();
		float TiledLength = TiledLengthFromLastSegment;
		
		//Step 1: Tiling generation
		{
			std::vector<int> AcceptedLabels;
			GetAcceptableLabels(pMaterial, LineIterator.GetSegment()->m_MeanAngle, AcceptedLabels);
			
			//Find tiles with the appropriate label
			int FirstTile = -1;
			int LastTile = -1;
			if(!LineIterator.IsClosedSegment())
			{
				const CLineIterator::CSegment* pNextSegment = LineIterator.GetNextSegment();
				
				//Try to find the first tile only if the previous one is not a corner
				if(FirstTileFromLastSegment < 0)
				{
					//Try to get a start tile
					FirstTile = FindSprite(Sprites, CAsset_PathMaterial::SPRITETILE_CAP_START, PreviousLabel, AcceptedLabels);
				}
				
				//There is a segment after the current one. Try to get a corner tile
				if(pNextSegment)
				{
					std::vector<int> NextAcceptedLabels;
					GetAcceptableLabels(pMaterial, pNextSegment->m_MeanAngle, NextAcceptedLabels);
					
					int TileType = CAsset_PathMaterial::SPRITETILE_CORNER_CONVEX;
					if(LineIterator.GetSegment()->m_EndAngle < 0.0f)
						TileType = CAsset_PathMaterial::SPRITETILE_CORNER_CONCAVE;
					LastTile = FindSprite(Sprites, TileType, AcceptedLabels, NextAcceptedLabels);
					
					if(LastTile >= 0)
						FirstTileFromLastSegment = LastTile;
				}
				//No corner tiles was found, try the get an end tile
				if(LastTile < 0)
				{
					FirstTileFromLastSegment = -1;
					LastTile = FindSprite(Sprites, CAsset_PathMaterial::SPRITETILE_CAP_END, AcceptedLabels, AcceptedLabels);
				}
			}
			
			if(FirstTile >= 0)
			{
				GenerateMaterialQuads_GetSpriteInfo(pAssetsManager, &Sprites[FirstTile], SpriteInfo);
				TiledLength += SpriteInfo.m_Width;
				
				Tiling.emplace_back();
				CLineTile& LineTile = Tiling.back();
				LineTile.m_Id = FirstTile;
				LineTile.m_Width = SpriteInfo.m_Width;
				LineTile.m_FillingEnabled = false;
				
				PreviousLabel.clear();
				PreviousLabel.push_back(Sprites[FirstTile].GetTileLabel1());
			}
			
			CLineTile LastLineTile;
			if(LastTile >= 0)
			{
				LastLineTile.m_Id = LastTile;
				LastLineTile.m_FillingEnabled = false;
				
				GenerateMaterialQuads_GetSpriteInfo(pAssetsManager, &Sprites[LastTile], SpriteInfo);
				if(Sprites[LastTile].GetTileType() == CAsset_PathMaterial::SPRITETILE_CORNER_CONVEX)
				{
					float Angle = (Pi - LineIterator.GetSegment()->m_EndAngle)/2.0f;
					float Height = (SpriteInfo.m_Width/2.0f - Sprites[LastTile].GetPosition().x);
					LastLineTile.m_Width = Height/std::tan(Angle);
				}
				else if(Sprites[LastTile].GetTileType() == CAsset_PathMaterial::SPRITETILE_CORNER_CONCAVE)
				{
					float Angle = (Pi + LineIterator.GetSegment()->m_EndAngle)/2.0f;
					float Height = (SpriteInfo.m_Width/2.0f + Sprites[LastTile].GetPosition().x);
					LastLineTile.m_Width = Height/std::tan(Angle);
				}
				else
					LastLineTile.m_Width = SpriteInfo.m_Width;
				
				TiledLength += LastLineTile.m_Width;
			}
			
			//1024 is just to make sure that we can go out of this loop in any case.
			for(int i=0; i<1024; i++)
			{
				int Tile = FindSprite(Sprites, CAsset_PathMaterial::SPRITETILE_LINE, PreviousLabel, AcceptedLabels);
				if(Tile >= 0)
				{
					GenerateMaterialQuads_GetSpriteInfo(pAssetsManager, &Sprites[Tile], SpriteInfo);
					
					if(
						(i == 0 && (SegmentLength - TiledLength > 0.0f)) ||
						(TiledLength + SpriteInfo.m_Width/2.0f < SegmentLength)
					)
					{
						TiledLength += SpriteInfo.m_Width;
						
						Tiling.emplace_back();
						CLineTile& LineTile = Tiling.back();
						LineTile.m_Id = Tile;
						LineTile.m_Width = SpriteInfo.m_Width;
						LineTile.m_FillingEnabled = false;
						if(Sprites[LineTile.m_Id].GetFilling() == CAsset_PathMaterial::SPRITEFILLING_STRETCHING)
							LineTile.m_FillingEnabled = true;
						
						PreviousLabel.clear();
						PreviousLabel.push_back(Sprites[Tile].GetTileLabel1());
					}
					else
						break;
				}
				else
					break;
			}
			
			if(LastTile >= 0)
			{
				Tiling.push_back(LastLineTile);
				PreviousLabel.clear();
				PreviousLabel.push_back(Sprites[LastTile].GetTileLabel1());
			}
		}
		
		//Move the iterator if the previous segment already tile a part of the current segment
		{
			float Distance_NoUse;
			int PrevVert_NoUse;
			int NextVert_NoUse;
			float PrevAlpha_NoUse;
			float NextAlpha_NoUse;
			LineIterator.MoveLineCursor(TiledLengthFromLastSegment, Distance_NoUse, PrevVert_NoUse, NextVert_NoUse, PrevAlpha_NoUse, NextAlpha_NoUse, false);
		}
		
		TiledLengthFromLastSegment = 0.0f;
		
		//Step 2: Tiling rendering
		if(Tiling.size())
		{
			//GlobalSpacing represent the spacing from the left (or right) side. Not both in the same time.
			float GlobalSpacing = 0.0f;
			int NumResizableTiles = 0;
			
			for(unsigned int i=0; i<Tiling.size(); i++)
			{
				if(Tiling[i].m_FillingEnabled)
					NumResizableTiles++;
			}
			
			if(NumResizableTiles)
				GlobalSpacing = ((SegmentLength - TiledLength)/NumResizableTiles)/2.0f;
			
			for(unsigned int i=0; i<Tiling.size(); i++)
			{
				const CAsset_PathMaterial::CSprite* pSprite = &Sprites[Tiling[i].m_Id];
				GenerateMaterialQuads_GetSpriteInfo(pAssetsManager, pSprite, SpriteInfo);
				
				float SpriteWidth = Tiling[i].m_Width;
				if(Tiling[i].m_FillingEnabled)
					SpriteWidth += 2.0f*GlobalSpacing;
				
				float WantedDistance = SpriteWidth;
				
				float PrevU = 0.0f;
				float NextU = 0.0f;
				
				float Distance;
				int PrevVert;
				int NextVert;
				float PrevAlpha;
				float NextAlpha;
				
				bool BreakOnVertex = false;
				if(
					pSprite->GetAlignment() == CAsset_PathMaterial::SPRITEALIGN_STRETCHED &&
						((pSprite->GetTileType() != CAsset_PathMaterial::SPRITETILE_CORNER_CONCAVE &&
						pSprite->GetTileType() != CAsset_PathMaterial::SPRITETILE_CORNER_CONVEX) ||
						i == Tiling.size()-1)
				)
					BreakOnVertex = true;
				
				int Stop = CLineIterator::LINECURSORSTATE_START;
				do
				{
					Stop = LineIterator.MoveLineCursor(WantedDistance, Distance, PrevVert, NextVert, PrevAlpha, NextAlpha, BreakOnVertex);
					if(Stop == CLineIterator::LINECURSORSTATE_ERROR)
						break;
					
					PrevU = NextU;
					NextU += (Distance / SpriteWidth);
					
					//~ if(Distance > 0.0001f)
					{
						vec2 PositionVert0 = Vertices[PrevVert].m_Position;
						vec2 PositionVert1 = Vertices[NextVert].m_Position;
						vec2 OrthoVert0 = Vertices[PrevVert].m_Thickness;
						vec2 OrthoVert1 = Vertices[NextVert].m_Thickness;
						vec2 SegDir = normalize(PositionVert1 - PositionVert0);
						vec2 OrthoSeg = ortho(SegDir);
						
						vec2 Position0 = mix(PositionVert0, PositionVert1, PrevAlpha);
						vec2 Position1 = mix(PositionVert0, PositionVert1, NextAlpha);
						vec4 Color0 = mix(Vertices[PrevVert].m_Color, Vertices[NextVert].m_Color, PrevAlpha);
						vec4 Color1 = mix(Vertices[PrevVert].m_Color, Vertices[NextVert].m_Color, NextAlpha);
												
						//float Weight0 = mix(Vertices[PrevVert].m_Weight, Vertices[NextVert].m_Weight, PrevAlpha);
						//float Weight1 = mix(Vertices[PrevVert].m_Weight, Vertices[NextVert].m_Weight, NextAlpha);
						
						if(pSprite->GetAlignment() == CAsset_PathMaterial::SPRITEALIGN_STRETCHED)
						{
							float USize = SpriteInfo.m_UMax - SpriteInfo.m_UMin;
							float VSize = SpriteInfo.m_VMax - SpriteInfo.m_VMin;
							
							//Stretched corner
							if(
								(pSprite->GetTileType() == CAsset_PathMaterial::SPRITETILE_CORNER_CONCAVE || pSprite->GetTileType() == CAsset_PathMaterial::SPRITETILE_CORNER_CONVEX) &&
								i == Tiling.size()-1
							)
							{								
								if(LineIterator.GetNextSegment())
								{
									TiledLengthFromLastSegment = SpriteWidth;
									
									int PrevVert_2 = LineIterator.GetNextSegment()->m_StartVert;
									int NextVert_2 = PrevVert_2;
									LineIterator.GetNextVertex(NextVert_2);
									
									vec2 PositionVert0_2 = Vertices[PrevVert_2].m_Position;
									vec2 PositionVert1_2 = Vertices[NextVert_2].m_Position;
									vec2 SegDir_2 = normalize(PositionVert1_2 - PositionVert0_2);
									vec2 OrthoSeg_2 = ortho(SegDir_2);
									
									CTexturedQuad Quad;
									Quad.m_ImagePath = SpriteInfo.m_ImagePath;
									Quad.m_TextureIndex = -1;
									
									Quad.m_Color[0] = Color0 * pSprite->GetColor();
									Quad.m_Color[1] = Color1 * pSprite->GetColor();
									Quad.m_Color[2] = Color0 * pSprite->GetColor();
									Quad.m_Color[3] = Color1 * pSprite->GetColor();
									
									if(pSprite->GetTileType() == CAsset_PathMaterial::SPRITETILE_CORNER_CONCAVE)
									{
										Quad.m_Texture[0] = vec2(SpriteInfo.m_UMin, SpriteInfo.m_VMin);
										Quad.m_Texture[1] = vec2(SpriteInfo.m_UMin + USize, SpriteInfo.m_VMin);
										Quad.m_Texture[2] = vec2(SpriteInfo.m_UMin, SpriteInfo.m_VMin + VSize);
										Quad.m_Texture[3] = vec2(SpriteInfo.m_UMin + USize, SpriteInfo.m_VMin + VSize);
										
										Quad.m_Position[0] = ObjPosition + Transform*(Position1 + OrthoVert1 * (pSprite->GetPosition().y + SpriteInfo.m_Height/2.0f));
										Quad.m_Position[1] = ObjPosition + Transform*(Position1 + SegDir_2 * SpriteWidth + OrthoSeg_2 * (pSprite->GetPosition().x - SpriteInfo.m_Width/2.0f));
										Quad.m_Position[2] = ObjPosition + Transform*(Position1 - SegDir * SpriteWidth + OrthoSeg * (pSprite->GetPosition().y - SpriteInfo.m_Height/2.0f));
										Quad.m_Position[3] = ObjPosition + Transform*(Position1 + OrthoVert1 * (pSprite->GetPosition().y - SpriteInfo.m_Height/2.0f));
									}
									else
									{
										Quad.m_Texture[0] = vec2(SpriteInfo.m_UMin + USize, SpriteInfo.m_VMin);
										Quad.m_Texture[1] = vec2(SpriteInfo.m_UMin + USize, SpriteInfo.m_VMin + VSize);
										Quad.m_Texture[2] = vec2(SpriteInfo.m_UMin, SpriteInfo.m_VMin);
										Quad.m_Texture[3] = vec2(SpriteInfo.m_UMin, SpriteInfo.m_VMin + VSize);
										
										Quad.m_Position[0] = ObjPosition + Transform*(Position1 + OrthoVert1 * (pSprite->GetPosition().y + SpriteInfo.m_Height/2.0f));
										Quad.m_Position[1] = ObjPosition + Transform*(Position1 + SegDir_2 * SpriteWidth + OrthoSeg_2 * (pSprite->GetPosition().x + SpriteInfo.m_Width/2.0f));
										Quad.m_Position[2] = ObjPosition + Transform*(Position1 - SegDir * SpriteWidth + OrthoSeg * (pSprite->GetPosition().y + SpriteInfo.m_Height/2.0f));
										Quad.m_Position[3] = ObjPosition + Transform*(Position1 + OrthoVert1 * (pSprite->GetPosition().y - SpriteInfo.m_Height/2.0f));
									}
										
									if(pSprite->GetFlags() & CAsset_PathMaterial::SPRITEFLAG_ROTATION)
										RotateQuadTexture(Quad);
																
									TesselateQuad(Quad, OutputQuads, OrthoTesselation, OrthoTesselation);
								}
							}
							//Stretched line
							else
							{
								CTexturedQuad Quad;
								Quad.m_ImagePath = SpriteInfo.m_ImagePath;
								Quad.m_TextureIndex = -1;
								
								Quad.m_Color[0] = Color0 * pSprite->GetColor();
								Quad.m_Color[1] = Color1 * pSprite->GetColor();
								Quad.m_Color[2] = Color0 * pSprite->GetColor();
								Quad.m_Color[3] = Color1 * pSprite->GetColor();
								
								if(pSprite->GetFlags() & CAsset_PathMaterial::SPRITEFLAG_ROTATION)
								{
									Quad.m_Texture[0] = vec2(SpriteInfo.m_UMin, SpriteInfo.m_VMax - VSize * PrevU);
									Quad.m_Texture[1] = vec2(SpriteInfo.m_UMin, SpriteInfo.m_VMax - VSize * NextU);
									Quad.m_Texture[2] = vec2(SpriteInfo.m_UMin + USize, SpriteInfo.m_VMax - VSize * PrevU);
									Quad.m_Texture[3] = vec2(SpriteInfo.m_UMin + USize, SpriteInfo.m_VMax - VSize * NextU);
								}
								else
								{
									Quad.m_Texture[0] = vec2(SpriteInfo.m_UMin + USize * PrevU, SpriteInfo.m_VMin);
									Quad.m_Texture[1] = vec2(SpriteInfo.m_UMin + USize * NextU, SpriteInfo.m_VMin);
									Quad.m_Texture[2] = vec2(SpriteInfo.m_UMin + USize * PrevU, SpriteInfo.m_VMin + VSize);
									Quad.m_Texture[3] = vec2(SpriteInfo.m_UMin + USize * NextU, SpriteInfo.m_VMin + VSize);
								}
								
								vec2 Ortho0 = OrthoSeg;
								vec2 Ortho1 = OrthoSeg;
								if(Stop == CLineIterator::LINECURSORSTATE_VERTEX && !LineIterator.IsEndVertex(NextVert))
									Ortho1 = OrthoVert1;
								if(PrevAlpha == 0.0f && !LineIterator.IsStartVertex(PrevVert))
									Ortho0 = OrthoVert0;
								
								Quad.m_Position[0] = ObjPosition + Transform*(Position0 + Ortho0 * (pSprite->GetPosition().y + SpriteInfo.m_Height/2.0f));
								Quad.m_Position[1] = ObjPosition + Transform*(Position1 + Ortho1 * (pSprite->GetPosition().y + SpriteInfo.m_Height/2.0f));
								Quad.m_Position[2] = ObjPosition + Transform*(Position0 + Ortho0 * (pSprite->GetPosition().y - SpriteInfo.m_Height/2.0f));
								Quad.m_Position[3] = ObjPosition + Transform*(Position1 + Ortho1 * (pSprite->GetPosition().y - SpriteInfo.m_Height/2.0f));
								
								TesselateQuad(Quad, OutputQuads, 1, OrthoTesselation);
							}
						}
						//Sprites along the line
						else if(Stop == CLineIterator::LINECURSORSTATE_END)
						{
							OutputQuads.emplace_back();
							CTexturedQuad& Quad = OutputQuads.back();
							
							//Add position shift
							vec2 DirX = vec2(-1.0f, 0.0f);
							vec2 DirY = vec2(0.0f, -1.0f);
							if(pSprite->GetAlignment() == CAsset_PathMaterial::SPRITEALIGN_LINE)
							{
								DirY = normalize(OrthoSeg);
								DirX = -ortho(DirY);
							}
							vec2 Pos = Position1;
							Pos += DirX * pSprite->GetPosition().x;
							Pos += DirY * pSprite->GetPosition().y;
							
							Quad.m_ImagePath = SpriteInfo.m_ImagePath;
							Quad.m_TextureIndex = -1;
							
							Quad.m_Color[0] = Color1*pSprite->GetColor();
							Quad.m_Color[1] = Quad.m_Color[0];
							Quad.m_Color[2] = Quad.m_Color[0];
							Quad.m_Color[3] = Quad.m_Color[0];
											
							if(pSprite->GetFlags() & CAsset_PathMaterial::SPRITEFLAG_ROTATION)
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
						}
					}
					
					if(Stop != CLineIterator::LINECURSORSTATE_END)
						WantedDistance -= Distance;
				}
				while(Stop != CLineIterator::LINECURSORSTATE_END && Stop != CLineIterator::LINECURSORSTATE_ERROR);
			}
		}
	}
}

void GenerateMaterialQuads(
	const CAssetsManager* pAssetsManager,
	std::vector<CTexturedQuad>& OutputQuads,
	const std::vector<CLineVertex>& Vertices,
	const matrix2x2& Transform,
	vec2 ObjPosition,
	CAssetPath MaterialPath,
	bool Closed,
	bool ShowLine,
	int OrthoTesselation
)
{
	const CAsset_PathMaterial* pMaterial = pAssetsManager->GetAsset<CAsset_PathMaterial>(MaterialPath);
	if(!pMaterial)
		return;
	
	//Render polygon
	if(Closed && pMaterial->GetTextureEnabled())
	{
		if(Vertices.size() >= 3)
		{
			vec2 Barycenter = vec2(0.0f, 0.0f);
			for(unsigned int i=0; i<Vertices.size(); i++)
				Barycenter += Vertices[i].m_Position;
			Barycenter = ObjPosition + Transform*(Barycenter / Vertices.size());
			
			float TextureSize = 1.0f;
			
			const CAsset_Image* pImage = pAssetsManager->GetAsset<CAsset_Image>(pMaterial->GetTexturePath());
			if(pImage)
				TextureSize = pImage->GetDataWidth() * pImage->GetTexelSize() / 1024.0f;
			
			unsigned int i=0;
			do
			{
				int V0 = i;
				int V1 = i+1;
				int V2 = min(i+2, static_cast<unsigned int>(Vertices.size()-1));
				
				OutputQuads.emplace_back();
				CTexturedQuad& Quad = OutputQuads.back();
				
				Quad.m_Position[0] = ObjPosition + Transform*(Vertices[V0].m_Position + normalize(Vertices[V0].m_Thickness) * pMaterial->GetTextureSpacing());
				Quad.m_Position[1] = ObjPosition + Transform*(Vertices[V1].m_Position + normalize(Vertices[V1].m_Thickness) * pMaterial->GetTextureSpacing());
				Quad.m_Position[2] = Barycenter;
				Quad.m_Position[3] = ObjPosition + Transform*(Vertices[V2].m_Position + normalize(Vertices[V2].m_Thickness) * pMaterial->GetTextureSpacing());
				
				Quad.m_Texture[0] = rotate(Quad.m_Position[0]/(pMaterial->GetTextureSize() * TextureSize), pMaterial->GetTextureAngle());
				Quad.m_Texture[1] = rotate(Quad.m_Position[1]/(pMaterial->GetTextureSize() * TextureSize), pMaterial->GetTextureAngle());
				Quad.m_Texture[2] = rotate(Quad.m_Position[2]/(pMaterial->GetTextureSize() * TextureSize), pMaterial->GetTextureAngle());
				Quad.m_Texture[3] = rotate(Quad.m_Position[3]/(pMaterial->GetTextureSize() * TextureSize), pMaterial->GetTextureAngle());
				
				Quad.m_Color[0] = pMaterial->GetTextureColor();
				Quad.m_Color[1] = pMaterial->GetTextureColor();
				Quad.m_Color[2] = pMaterial->GetTextureColor();
				Quad.m_Color[3] = pMaterial->GetTextureColor();
				
				Quad.m_ImagePath = pMaterial->GetTexturePath();
				Quad.m_TextureIndex = -1;
				
				i += 2;
			}
			while(i < Vertices.size()-1);
		}
	}
	
	//Render line layers
	if(ShowLine)
	{
		CAsset_PathMaterial::CIteratorLayer LayerIter;
		for(LayerIter = pMaterial->BeginLayer(); LayerIter != pMaterial->EndLayer(); ++LayerIter)
		{
			const CAsset_PathMaterial::CLayer* pLayer = &pMaterial->GetLayer(*LayerIter);
				
			GenerateMaterialQuads_Line(pAssetsManager, OutputQuads, Vertices, Transform, ObjPosition, pMaterial, pLayer, Closed, OrthoTesselation);
		}
	}
}

void GenerateMaterialCurve_Object(class CAssetsManager* pAssetsManager, float Time, std::vector<CLineVertex>& OutputLines, const CAsset_MapLayerObjects::CObject& Object)
{
	const std::vector<CAsset_MapLayerObjects::CVertex>& ObjectVertices = Object.GetVertexArray();
	bool Closed = (Object.GetPathType() == CAsset_MapLayerObjects::PATHTYPE_CLOSED);
	
	std::vector<CBezierVertex> BezierVertices;
	
	for(unsigned int i=0; i<ObjectVertices.size(); i++)
	{
		BezierVertices.emplace_back();
		CBezierVertex& Vertex = BezierVertices.back();
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
		BezierVertices.push_back(BezierVertices[0]);
	}
	
	ComputeLineNormals<CBezierVertex>(BezierVertices, Closed);
	TesselateBezierCurve(BezierVertices, OutputLines, 32.0f);
	ComputeLineNormals<CLineVertex>(OutputLines, Closed);
}

void GenerateMaterialQuads_Object(class CAssetsManager* pAssetsManager, float Time, std::vector<CTexturedQuad>& OutputQuads, const CAsset_MapLayerObjects::CObject& Object)
{
	bool Closed = (Object.GetPathType() == CAsset_MapLayerObjects::PATHTYPE_CLOSED);
	bool ShowLine = (Object.GetLineType() == CAsset_MapLayerObjects::LINETYPE_SHOW);
	vec2 ObjPosition;
	matrix2x2 Transform;
	Object.GetTransform(pAssetsManager, Time, &Transform, &ObjPosition);
	
	std::vector<CLineVertex> LineVertices;
	GenerateMaterialCurve_Object(pAssetsManager, Time, LineVertices, Object);
	
	GenerateMaterialQuads(pAssetsManager, OutputQuads, LineVertices, Transform, ObjPosition, Object.GetStylePath(), Closed, ShowLine, Object.GetOrthoTesselation());
}

void GenerateZoneCurve_Object(class CAssetsManager* pAssetsManager, float Time, std::vector<CLineVertex>& OutputLines, const CAsset_MapZoneObjects::CObject& Object)
{
	const std::vector<CAsset_MapZoneObjects::CVertex>& ObjectVertices = Object.GetVertexArray();
	bool Closed = (Object.GetPathType() == CAsset_MapZoneObjects::PATHTYPE_CLOSED);
	
	vec2 ObjPosition;
	matrix2x2 Transform;
	Object.GetTransform(pAssetsManager, Time, &Transform, &ObjPosition);
	
	std::vector<CBezierVertex> BezierVertices;
	
	for(unsigned int i=0; i<ObjectVertices.size(); i++)
	{
		BezierVertices.emplace_back();
		CBezierVertex& Vertex = BezierVertices.back();
		Vertex.m_Position = ObjectVertices[i].GetPosition();
		Vertex.m_Weight = ObjectVertices[i].GetWeight();
		Vertex.m_Type = ObjectVertices[i].GetSmoothness();
		Vertex.m_aControlPoints[0] = Vertex.m_Position + ObjectVertices[i].GetControlPoint0();
		Vertex.m_aControlPoints[1] = Vertex.m_Position + ObjectVertices[i].GetControlPoint1();
		if(Vertex.m_Type >= CBezierVertex::NUM_TYPES || Vertex.m_Type < 0)
			Vertex.m_Type = CBezierVertex::TYPE_CORNER;
	}
	if(Closed && BezierVertices.size() > 0)
	{
		BezierVertices.push_back(BezierVertices[0]);
	}
	
	ComputeLineNormals<CBezierVertex>(BezierVertices, Closed);
	TesselateBezierCurve(BezierVertices, OutputLines, 32.0f);
	ComputeLineNormals<CLineVertex>(OutputLines, Closed);
}

void GenerateZoneQuads(
	const CAssetsManager* pAssetsManager,
	std::vector<CTexturedQuad>& OutputQuads,
	const std::vector<CLineVertex>& Vertices,
	const matrix2x2& Transform,
	vec2 ObjPosition,
	CAssetPath ZoneTypePath,
	int Index,
	bool Closed
)
{
	const CAsset_ZoneType* pZoneType = pAssetsManager->GetAsset<CAsset_ZoneType>(ZoneTypePath);
	if(!pZoneType)
		return;
		
	bool IndexFound = false;
	vec4 ZoneColor = 1.0f;
	vec4 BorderColor = 1.0f;
	if(pZoneType)
	{
		CSubPath IndexPath = CAsset_ZoneType::SubPath_Index(Index);
		if(pZoneType->IsValidIndex(IndexPath))
		{
			ZoneColor = pZoneType->GetIndexColor(IndexPath);
			BorderColor = pZoneType->GetIndexBorderColor(IndexPath);
			IndexFound = true;
		}
	}
	
	if(!IndexFound)
	{
		ZoneColor = vec4(0.0f, 0.0f, 0.0f, 0.5f);
		BorderColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	
	//Render polygon
	if(Closed)
	{
		if(Vertices.size() >= 3)
		{
			vec2 Barycenter = vec2(0.0f, 0.0f);
			for(unsigned int i=0; i<Vertices.size(); i++)
				Barycenter += Vertices[i].m_Position;
			Barycenter = ObjPosition + Transform*(Barycenter / Vertices.size());
			
			float TextureSize = 1.0f/32.0f;
			
			unsigned int i=0;
			do
			{
				int V0 = i;
				int V1 = i+1;
				int V2 = min(i+2, static_cast<unsigned int>(Vertices.size()-1));
				
				OutputQuads.emplace_back();
				CTexturedQuad& Quad = OutputQuads.back();
				
				Quad.m_Position[0] = ObjPosition + Transform*(Vertices[V0].m_Position);
				Quad.m_Position[1] = ObjPosition + Transform*(Vertices[V1].m_Position);
				Quad.m_Position[2] = Barycenter;
				Quad.m_Position[3] = ObjPosition + Transform*(Vertices[V2].m_Position);
				
				Quad.m_Texture[0] = Quad.m_Position[0] * TextureSize;
				Quad.m_Texture[1] = Quad.m_Position[1] * TextureSize;
				Quad.m_Texture[2] = Quad.m_Position[2] * TextureSize;
				Quad.m_Texture[3] = Quad.m_Position[3] * TextureSize;
				
				Quad.m_Color[0] = ZoneColor;
				Quad.m_Color[1] = ZoneColor;
				Quad.m_Color[2] = ZoneColor;
				Quad.m_Color[3] = ZoneColor;
				
				Quad.m_ImagePath = pZoneType->GetImagePath();
				Quad.m_TextureIndex = Index;
				
				i += 2;
			}
			while(i < Vertices.size()-1);
		}
		
		if(Vertices.size() >= 2)
		{
			for(unsigned int i=0; i<Vertices.size()-1; i++)
			{
				int V0 = i;
				int V1 = (i+1);
				
				OutputQuads.emplace_back();
				CTexturedQuad& Quad = OutputQuads.back();
				
				Quad.m_Position[0] = ObjPosition + Transform*(Vertices[V0].m_Position);
				Quad.m_Position[1] = ObjPosition + Transform*(Vertices[V1].m_Position);
				Quad.m_Position[2] = ObjPosition + Transform*(Vertices[V0].m_Position - Vertices[V0].m_Thickness*8.0f);
				Quad.m_Position[3] = ObjPosition + Transform*(Vertices[V1].m_Position - Vertices[V1].m_Thickness*8.0f);
				
				Quad.m_Texture[0] = 0.0f;
				Quad.m_Texture[1] = 0.0f;
				Quad.m_Texture[2] = 0.0f;
				Quad.m_Texture[3] = 0.0f;
				
				Quad.m_Color[0] = BorderColor;
				Quad.m_Color[1] = BorderColor;
				Quad.m_Color[2] = BorderColor;
				Quad.m_Color[3] = BorderColor;
				
				Quad.m_TextureIndex = -1;
			}
		}
	}
}

void GenerateZoneQuads_Object(class CAssetsManager* pAssetsManager, float Time, std::vector<CTexturedQuad>& OutputQuads, const CAsset_MapZoneObjects::CObject& Object, CAssetPath ZoneTypePath)
{
	bool Closed = (Object.GetPathType() == CAsset_MapZoneObjects::PATHTYPE_CLOSED);
	vec2 ObjPosition;
	matrix2x2 Transform;
	Object.GetTransform(pAssetsManager, Time, &Transform, &ObjPosition);
	
	std::vector<CLineVertex> LineVertices;
	GenerateZoneCurve_Object(pAssetsManager, Time, LineVertices, Object);
	
	GenerateZoneQuads(pAssetsManager, OutputQuads, LineVertices, Transform, ObjPosition, ZoneTypePath, Object.GetZoneIndex(), Closed);
}
