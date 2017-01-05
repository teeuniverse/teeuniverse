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
	if((pMaterialSprite->GetFlags() & CAsset_Material::SPRITEFLAG_VFLIP) || SpriteInfo.m_Width < 0)
	{
		Tmp = SpriteInfo.m_UMin;
		SpriteInfo.m_UMin = SpriteInfo.m_UMax;
		SpriteInfo.m_UMax = Tmp;
	}
	if((pMaterialSprite->GetFlags() & CAsset_Material::SPRITEFLAG_HFLIP) || SpriteInfo.m_Height < 0)
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
		bool m_Closed;
	};
	
protected:
	const array<CLineVertex>& m_Vertices;
	array<CSegment> m_Segments;
	bool m_Closed;
	int m_CurrSegment;
	int m_CurrVert;
	float m_VertexAlpha;
	
public:
	CLineIterator(const array<CLineVertex>& Vertices, bool Closed) :
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
				
				CSegment& Segment = m_Segments.increment();
				Segment.m_StartVert = CurrVert;
				Segment.m_EndVert = CurrVert;
				Segment.m_Closed = false;
				
				while(GetNextVertex(Segment.m_EndVert))
				{
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
			if(CurrId >= m_Vertices.size()-2)
				CurrId = 0;
			else
				CurrId++;
		}
		else
		{
			if(CurrId >= m_Vertices.size()-1)
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
		return (dot(LineDir0, LineDir1) < std::cos(Pi/5.0f));
	}
	
	inline bool NextSegment()
	{
		m_CurrSegment++;
		if(m_CurrSegment >= m_Segments.size())
			return false;
		
		m_CurrVert = m_Segments[m_CurrSegment].m_StartVert;
		m_VertexAlpha = 0.0f;
		
		return true;
	}
	
	inline const CSegment* GetNextSegment() const
	{
		if(m_CurrSegment+1 < m_Segments.size())
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
		LINECURSORSTATE_END=0,
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

void GenerateMaterialQuads_Line(
	const CAssetsManager* pAssetsManager,
	array<CTexturedQuad>& OutputQuads,
	const array<CLineVertex>& Vertices,
	const matrix2x2& Transform,
	vec2 ObjPosition,
	const CAsset_Material::CLayer* pLayer,
	bool Closed,
	int OrthoTesselation
)
{
	const array< CAsset_Material::CSprite, allocator_copy<CAsset_Material::CSprite> >& Sprites = pLayer->GetSpriteArray();
	if(Sprites.size() <= 0)
		return;
	
	CLineIterator LineIterator(Vertices, Closed);
	
	int PrevCornerTile = -1;
	while(LineIterator.NextSegment())
	{
		CSpriteInfo SpriteInfo;
		array<int> Tiling;
		float SegmentShift = 0.0f;
		float SegmentLength = LineIterator.GetSegmentLength();
		float TiledLength = 0.0f;
		
		//Step 1: Tiling generation
		{
			int FirstTile = -1;
			int LastTile = -1;
			if(!LineIterator.IsClosedSegment())
			{
				const CLineIterator::CSegment* pNextSegment = LineIterator.GetNextSegment();
				
				//Try to find the first tile only if the previous one is not a corner
				if(PrevCornerTile < 0)
				{
					//Try to get a start tile
					for(int i=0; i<Sprites.size(); i++)
					{
						if(Sprites[i].GetTileType() == CAsset_Material::SPRITETILE_CAP_START)
						{
							FirstTile = i;
							break;
						}
					}
				}
				else
					FirstTile = PrevCornerTile;
				
				//There is a segment after the current one. Try to get a corner tile
				if(pNextSegment)
				{
					for(int i=0; i<Sprites.size(); i++)
					{
						if(Sprites[i].GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONVEX)
						{
							LastTile = i;
							break;
						}
					}
					if(LastTile >= 0)
						PrevCornerTile = LastTile;
				}
				//No corner tiles was found, try the get a end tile
				if(LastTile < 0)
				{
					PrevCornerTile = -1;
					for(int i=0; i<Sprites.size(); i++)
					{
						if(Sprites[i].GetTileType() == CAsset_Material::SPRITETILE_CAP_END)
						{
							LastTile = i;
							break;
						}
					}
				}
			}
			
			if(FirstTile >= 0)
			{
				GenerateMaterialQuads_GetSpriteInfo(pAssetsManager, &Sprites[FirstTile], SpriteInfo);
				if(Sprites[FirstTile].GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONCAVE || Sprites[FirstTile].GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONVEX)
				{
					SegmentShift -= Sprites[FirstTile].GetPosition().y;
					SegmentLength += Sprites[FirstTile].GetPosition().y;
					TiledLength += SpriteInfo.m_Height/2.0f;
				}
				else
					TiledLength += SpriteInfo.m_Width;
				Tiling.add_by_copy(FirstTile);
			}
			if(LastTile >= 0)
			{
				GenerateMaterialQuads_GetSpriteInfo(pAssetsManager, &Sprites[LastTile], SpriteInfo);
				if(Sprites[LastTile].GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONCAVE || Sprites[LastTile].GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONVEX)
				{
					SegmentLength += Sprites[FirstTile].GetPosition().x;
					TiledLength += SpriteInfo.m_Width/2.0f;
				}
				else
					TiledLength += SpriteInfo.m_Width;
			}
			
			//1024 is just to make sure that we can go out of this loop in any case.
			for(int i=0; i<1024; i++)
			{
				int Tile = -1;
				for(int i=0; i<Sprites.size(); i++)
				{
					if(Sprites[i].GetTileType() == CAsset_Material::SPRITETILE_LINE)
					{
						Tile = i;
						break;
					}
				}
				
				if(Tile >= 0)
				{
					GenerateMaterialQuads_GetSpriteInfo(pAssetsManager, &Sprites[Tile], SpriteInfo);
					if(TiledLength + SpriteInfo.m_Width < SegmentLength)
					{
						TiledLength += SpriteInfo.m_Width;
						Tiling.add_by_copy(Tile);
					}
					else
						break;
				}
				else
					break;
			}
			
			if(LastTile >= 0)
				Tiling.add_by_copy(LastTile);
		}
		
		//Step 2: Tiling rendering
		if(Tiling.size())
		{
			float Distance_NoUse;
			int PrevVert_NoUse;
			int NextVert_NoUse;
			float PrevAlpha_NoUse;
			float NextAlpha_NoUse;
			
			LineIterator.MoveLineCursor(SegmentShift, Distance_NoUse, PrevVert_NoUse, NextVert_NoUse, PrevAlpha_NoUse, NextAlpha_NoUse, false);
						
			//GlobalSpacing represent the spacing from the left (or right) side. Not both in the same time.
			float GlobalSpacing = 0.0f;
			if(Tiling.size() > 2)
				GlobalSpacing = ((SegmentLength - TiledLength)/(Tiling.size()-2))/2.0f;
			
			for(int i=0; i<Tiling.size(); i++)
			{
				const CAsset_Material::CSprite* pSprite = &Sprites[Tiling[i]];
				GenerateMaterialQuads_GetSpriteInfo(pAssetsManager, pSprite, SpriteInfo);
				
				float SpriteWidth = SpriteInfo.m_Width;
				
				//Shift for corner tiles
				if(i == 0)
				{
					if(Sprites[Tiling[0]].GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONCAVE || Sprites[Tiling[0]].GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONVEX)
					{
						SpriteWidth /= 2.0f;
					}
				}
				else if(i == Tiling.size()-1)
				{
					if(Sprites[Tiling[Tiling.size()-1]].GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONCAVE || Sprites[Tiling[Tiling.size()-1]].GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONVEX)
					{
						SpriteWidth /= 2.0f;
					}
				}
				else
				{
					if(pSprite->GetFilling() == CAsset_Material::SPRITEFILLING_STRETCHING)
					{
						SpriteWidth += 2.0f*GlobalSpacing;
					}
				}
				
				float WantedDistance = SpriteWidth;
				
				float PrevU = 0.0f;
				float NextU = 0.0f;
				
				float Distance;
				int PrevVert;
				int NextVert;
				float PrevAlpha;
				float NextAlpha;
				
				bool BreakOnVertex = (pSprite->GetAlignment() == CAsset_Material::SPRITEALIGN_STRETCHED);
				while(WantedDistance > 0.0f)
				{
					int Stop = LineIterator.MoveLineCursor(WantedDistance, Distance, PrevVert, NextVert, PrevAlpha, NextAlpha, BreakOnVertex);
					if(Stop == CLineIterator::LINECURSORSTATE_ERROR)
						break;
					
					PrevU = NextU;
					NextU += (Distance / SpriteWidth);
					
					if(Distance > 0.0001f)
					{
						vec2 PositionVert0 = Vertices[PrevVert].m_Position;
						vec2 PositionVert1 = Vertices[NextVert].m_Position;
						vec2 OrthoVert0 = Vertices[PrevVert].m_Thickness;
						vec2 OrthoVert1 = Vertices[NextVert].m_Thickness;
						vec2 OrthoSeg = ortho(normalize(PositionVert1 - PositionVert0));
						
						vec2 Position0 = mix(PositionVert0, PositionVert1, PrevAlpha);
						vec2 Position1 = mix(PositionVert0, PositionVert1, NextAlpha);
						vec4 Color0 = mix(Vertices[PrevVert].m_Color, Vertices[NextVert].m_Color, PrevAlpha);
						vec4 Color1 = mix(Vertices[PrevVert].m_Color, Vertices[NextVert].m_Color, NextAlpha);
												
						//float Weight0 = mix(Vertices[PrevVert].m_Weight, Vertices[NextVert].m_Weight, PrevAlpha);
						//float Weight1 = mix(Vertices[PrevVert].m_Weight, Vertices[NextVert].m_Weight, NextAlpha);
						
						if(pSprite->GetAlignment() == CAsset_Material::SPRITEALIGN_STRETCHED)
						{
							int VerticalTesselation = OrthoTesselation;
							if(SpriteInfo.m_ImagePath.IsNull())
								VerticalTesselation = 1;
							for(int k=0; k<VerticalTesselation; k++)
							{
								float USize = SpriteInfo.m_UMax - SpriteInfo.m_UMin;
								float VSize = SpriteInfo.m_VMax - SpriteInfo.m_VMin;
								
								CTexturedQuad& Quad = OutputQuads.increment();
								
								float StepMin = -(2.0f * static_cast<float>(k)/VerticalTesselation - 1.0f);
								float StepMax = -(2.0f * static_cast<float>(k+1)/VerticalTesselation - 1.0f);
								float VMin = static_cast<float>(k)/VerticalTesselation;
								float VMax = static_cast<float>(k+1)/VerticalTesselation;
								
								Quad.m_ImagePath = SpriteInfo.m_ImagePath;
								
								Quad.m_Color[0] = Color0 * pSprite->GetColor();
								Quad.m_Color[1] = Color1 * pSprite->GetColor();
								Quad.m_Color[2] = Color0 * pSprite->GetColor();
								Quad.m_Color[3] = Color1 * pSprite->GetColor();
								
								if(
									(pSprite->GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONCAVE || pSprite->GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONVEX) &&
									i == 0
								)
								{
									if(pSprite->GetFlags() & CAsset_Material::SPRITEFLAG_ROTATION)
									{
										Quad.m_Texture[0] = vec2(SpriteInfo.m_UMax - USize * (1.0f-PrevU) * (1.0f-VMin), SpriteInfo.m_VMin + VSize * VMin);
										Quad.m_Texture[1] = vec2(SpriteInfo.m_UMax - USize * (1.0f-NextU) * (1.0f-VMin), SpriteInfo.m_VMin + VSize * VMin);
										Quad.m_Texture[2] = vec2(SpriteInfo.m_UMax - USize * (1.0f-PrevU) * (1.0f-VMax), SpriteInfo.m_VMin + VSize * VMax);
										Quad.m_Texture[3] = vec2(SpriteInfo.m_UMax - USize * (1.0f-NextU) * (1.0f-VMax), SpriteInfo.m_VMin + VSize * VMax);
									}
									else
									{
										Quad.m_Texture[0] = vec2(SpriteInfo.m_UMax - USize * VMin, SpriteInfo.m_VMax - VSize * (1.0f-PrevU) * (1.0f-VMin));
										Quad.m_Texture[1] = vec2(SpriteInfo.m_UMax - USize * VMin, SpriteInfo.m_VMax - VSize * (1.0f-NextU) * (1.0f-VMin));
										Quad.m_Texture[2] = vec2(SpriteInfo.m_UMax - USize * VMax, SpriteInfo.m_VMax - VSize * (1.0f-PrevU) * (1.0f-VMax));
										Quad.m_Texture[3] = vec2(SpriteInfo.m_UMax - USize * VMax, SpriteInfo.m_VMax - VSize * (1.0f-NextU) * (1.0f-VMax));
									}
									
									Quad.m_Position[0] = ObjPosition + Transform*(Position0 + OrthoVert0 * (pSprite->GetPosition().y + StepMin * SpriteInfo.m_Height/2.0f));
									Quad.m_Position[1] = ObjPosition + Transform*(Position1 + OrthoSeg * (pSprite->GetPosition().y + StepMin * SpriteInfo.m_Height/2.0f));
									Quad.m_Position[2] = ObjPosition + Transform*(Position0 + OrthoVert0 * (pSprite->GetPosition().y + StepMax * SpriteInfo.m_Height/2.0f));
									Quad.m_Position[3] = ObjPosition + Transform*(Position1 + OrthoSeg * (pSprite->GetPosition().y + StepMax * SpriteInfo.m_Height/2.0f));
								}
								else if(
									(pSprite->GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONCAVE || pSprite->GetTileType() == CAsset_Material::SPRITETILE_CORNER_CONVEX) &&
									i == Tiling.size()-1
								)
								{
									if(pSprite->GetFlags() & CAsset_Material::SPRITEFLAG_ROTATION)
									{
										Quad.m_Texture[0] = vec2(SpriteInfo.m_UMin + USize * VMin, SpriteInfo.m_VMax - VSize * PrevU * (1.0f-VMin));
										Quad.m_Texture[1] = vec2(SpriteInfo.m_UMin + USize * VMin, SpriteInfo.m_VMax - VSize * NextU * (1.0f-VMin));
										Quad.m_Texture[2] = vec2(SpriteInfo.m_UMin + USize * VMax, SpriteInfo.m_VMax - VSize * PrevU * (1.0f-VMax));
										Quad.m_Texture[3] = vec2(SpriteInfo.m_UMin + USize * VMax, SpriteInfo.m_VMax - VSize * NextU * (1.0f-VMax));
									}
									else
									{
										Quad.m_Texture[0] = vec2(SpriteInfo.m_UMin + USize * PrevU * (1.0f-VMin), SpriteInfo.m_VMin + VSize * VMin);
										Quad.m_Texture[1] = vec2(SpriteInfo.m_UMin + USize * NextU * (1.0f-VMin), SpriteInfo.m_VMin + VSize * VMin);
										Quad.m_Texture[2] = vec2(SpriteInfo.m_UMin + USize * PrevU * (1.0f-VMax), SpriteInfo.m_VMin + VSize * VMax);
										Quad.m_Texture[3] = vec2(SpriteInfo.m_UMin + USize * NextU * (1.0f-VMax), SpriteInfo.m_VMin + VSize * VMax);
									}
									
									Quad.m_Position[0] = ObjPosition + Transform*(Position0 + OrthoSeg * (pSprite->GetPosition().y + StepMin * SpriteInfo.m_Height/2.0f));
									Quad.m_Position[1] = ObjPosition + Transform*(Position1 + OrthoVert1 * (pSprite->GetPosition().y + StepMin * SpriteInfo.m_Height/2.0f));
									Quad.m_Position[2] = ObjPosition + Transform*(Position0 + OrthoSeg * (pSprite->GetPosition().y + StepMax * SpriteInfo.m_Height/2.0f));
									Quad.m_Position[3] = ObjPosition + Transform*(Position1 + OrthoVert1 * (pSprite->GetPosition().y + StepMax * SpriteInfo.m_Height/2.0f));
								}
								else
								{
									if(pSprite->GetFlags() & CAsset_Material::SPRITEFLAG_ROTATION)
									{
										Quad.m_Texture[0] = vec2(SpriteInfo.m_UMin + USize * VMin, SpriteInfo.m_VMax - VSize * PrevU);
										Quad.m_Texture[1] = vec2(SpriteInfo.m_UMin + USize * VMin, SpriteInfo.m_VMax - VSize * NextU);
										Quad.m_Texture[2] = vec2(SpriteInfo.m_UMin + USize * VMax, SpriteInfo.m_VMax - VSize * PrevU);
										Quad.m_Texture[3] = vec2(SpriteInfo.m_UMin + USize * VMax, SpriteInfo.m_VMax - VSize * NextU);
									}
									else
									{
										Quad.m_Texture[0] = vec2(SpriteInfo.m_UMin + USize * PrevU, SpriteInfo.m_VMin + VSize * VMin);
										Quad.m_Texture[1] = vec2(SpriteInfo.m_UMin + USize * NextU, SpriteInfo.m_VMin + VSize * VMin);
										Quad.m_Texture[2] = vec2(SpriteInfo.m_UMin + USize * PrevU, SpriteInfo.m_VMin + VSize * VMax);
										Quad.m_Texture[3] = vec2(SpriteInfo.m_UMin + USize * NextU, SpriteInfo.m_VMin + VSize * VMax);
									}
							
									vec2 Ortho0 = OrthoSeg;
									vec2 Ortho1 = OrthoSeg;
									if(NextAlpha == 1.0f)
										Ortho1 = OrthoVert1;
									if(PrevAlpha == 0.0f)
										Ortho0 = OrthoVert0;
									
									Quad.m_Position[0] = ObjPosition + Transform*(Position0 + Ortho0 * (pSprite->GetPosition().y + StepMin * SpriteInfo.m_Height/2.0f));
									Quad.m_Position[1] = ObjPosition + Transform*(Position1 + Ortho1 * (pSprite->GetPosition().y + StepMin * SpriteInfo.m_Height/2.0f));
									Quad.m_Position[2] = ObjPosition + Transform*(Position0 + Ortho0 * (pSprite->GetPosition().y + StepMax * SpriteInfo.m_Height/2.0f));
									Quad.m_Position[3] = ObjPosition + Transform*(Position1 + Ortho1 * (pSprite->GetPosition().y + StepMax * SpriteInfo.m_Height/2.0f));
								}
							}
						}
						else if(Stop == CLineIterator::LINECURSORSTATE_END)
						{
							CTexturedQuad& Quad = OutputQuads.increment();
							
							//Add position shift
							vec2 DirX = vec2(-1.0f, 0.0f);
							vec2 DirY = vec2(0.0f, -1.0f);
							if(pSprite->GetAlignment() == CAsset_Material::SPRITEALIGN_LINE)
							{
								DirY = normalize(OrthoSeg);
								DirX = -ortho(DirY);
							}
							vec2 Pos = Position1;
							Pos += DirX * pSprite->GetPosition().x;
							Pos += DirY * pSprite->GetPosition().y;
							
							Quad.m_ImagePath = SpriteInfo.m_ImagePath;
							
							Quad.m_Color[0] = Color1*pSprite->GetColor();
							Quad.m_Color[1] = Quad.m_Color[0];
							Quad.m_Color[2] = Quad.m_Color[0];
							Quad.m_Color[3] = Quad.m_Color[0];
											
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
						}
					}
					
					if(Stop == CLineIterator::LINECURSORSTATE_END)
						break;
					else
						WantedDistance -= Distance;
				}
			}
		}
	}
}

void GenerateMaterialQuads(
	const CAssetsManager* pAssetsManager,
	array<CTexturedQuad>& OutputQuads,
	const array<CLineVertex>& Vertices,
	const matrix2x2& Transform,
	vec2 ObjPosition,
	CAssetPath MaterialPath,
	bool Closed,
	bool ShowLine,
	int OrthoTesselation
)
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
				
				Quad.m_Position[0] = ObjPosition + Transform*(Vertices[V0].m_Position + normalize(Vertices[V0].m_Thickness) * pMaterial->GetTextureSpacing());
				Quad.m_Position[1] = ObjPosition + Transform*(Vertices[V1].m_Position + normalize(Vertices[V1].m_Thickness) * pMaterial->GetTextureSpacing());
				Quad.m_Position[2] = ObjPosition + Transform*(Vertices[V2].m_Position + normalize(Vertices[V2].m_Thickness) * pMaterial->GetTextureSpacing());
				Quad.m_Position[3] = ObjPosition + Transform*(Vertices[V3].m_Position + normalize(Vertices[V3].m_Thickness) * pMaterial->GetTextureSpacing());
				
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
	if(ShowLine)
	{
		CAsset_Material::CIteratorLayer LayerIter;
		for(LayerIter = pMaterial->BeginLayer(); LayerIter != pMaterial->EndLayer(); ++LayerIter)
		{
			const CAsset_Material::CLayer* pLayer = &pMaterial->GetLayer(*LayerIter);
				
			GenerateMaterialQuads_Line(pAssetsManager, OutputQuads, Vertices, Transform, ObjPosition, pLayer, Closed, OrthoTesselation);
		}
	}
}

void GenerateMaterialCurve_Object(class CAssetsManager* pAssetsManager, float Time, array<CLineVertex>& OutputLines, const CAsset_MapLayerObjects::CObject& Object)
{
	const array< CAsset_MapLayerObjects::CVertex, allocator_copy<CAsset_MapLayerObjects::CVertex> >& ObjectVertices = Object.GetVertexArray();
	bool Closed = Object.GetClosedPath();
	vec2 ObjPosition;
	matrix2x2 Transform;
	Object.GetTransform(pAssetsManager, Time, &Transform, &ObjPosition);
	
	array<CBezierVertex> BezierVertices;
	
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
	TesselateBezierCurve(BezierVertices, OutputLines, 32.0f);
	ComputeLineNormals<CLineVertex>(OutputLines, Closed);
}

void GenerateMaterialQuads_Object(class CAssetsManager* pAssetsManager, float Time, array<CTexturedQuad>& OutputQuads, const CAsset_MapLayerObjects::CObject& Object)
{
	bool Closed = Object.GetClosedPath();
	vec2 ObjPosition;
	matrix2x2 Transform;
	Object.GetTransform(pAssetsManager, Time, &Transform, &ObjPosition);
	
	array<CLineVertex> LineVertices;
	GenerateMaterialCurve_Object(pAssetsManager, Time, LineVertices, Object);
	
	GenerateMaterialQuads(pAssetsManager, OutputQuads, LineVertices, Transform, ObjPosition, Object.GetStylePath(), Closed, Object.GetShowLine(), Object.GetOrthoTesselation());
}
