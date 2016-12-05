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

#include <client/components/graphics.h>
#include <client/components/assetsrenderer.h>
#include <shared/components/assetsmanager.h>

#include "maprenderer.h"

CMapRenderer::CMapRenderer(CClientKernel* pKernel) :
	CClientKernel::CGuest(pKernel)
{
	
}
	
void CMapRenderer::SetCanvas(const gui::CRect& Canvas, vec2 ScreenPos)
{
	m_ScreenPos = ScreenPos;
	m_Canvas = Canvas;
}

void CMapRenderer::SetCamera(vec2 CameraPos, float CameraZoom)
{
	m_CameraPos = CameraPos;
	m_CameraZoom = CameraZoom;
	
	m_GroupCameraPos = (m_CameraPos * m_GroupHardParallax) + m_GroupPos;
}

void CMapRenderer::SetGroup(CAssetPath GroupPath)
{
	const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(GroupPath);
	if(pGroup)
	{
		m_GroupPos = pGroup->GetPosition();
		m_GroupHardParallax = pGroup->GetHardParallax();
	
		m_GroupCameraPos = (m_CameraPos * m_GroupHardParallax) + m_GroupPos;
	}
	else
		UnsetGroup();
}

void CMapRenderer::UnsetGroup()
{
	m_GroupPos = vec2(0.0f, 0.0f);
	m_GroupHardParallax = vec2(1.0f, 1.0f);
	
	m_GroupCameraPos = (m_CameraPos * m_GroupHardParallax) + m_GroupPos;
}

vec2 CMapRenderer::MapPosToScreenPos(vec2 MapPos) const
{
	return m_ScreenPos + (MapPos - m_GroupCameraPos)*m_CameraZoom;
}

vec2 CMapRenderer::ScreenPosToMapPos(vec2 ScreenPos) const
{
	return m_GroupCameraPos + (ScreenPos - m_ScreenPos)/m_CameraZoom;
}

vec2 CMapRenderer::MapPosToTilePos(vec2 MapPos) const
{
	return MapPos/32.0f;
}

vec2 CMapRenderer::TilePosToMapPos(vec2 TilePos) const
{
	return TilePos*32.0f;
}

vec2 CMapRenderer::ScreenPosToTilePos(vec2 ScreenPos) const
{
	return MapPosToTilePos(ScreenPosToMapPos(ScreenPos));
}

vec2 CMapRenderer::TilePosToScreenPos(vec2 TilePos) const
{
	return MapPosToScreenPos(TilePosToMapPos(TilePos));
}

template<class TILE>
void RenderTiles_Zone_Impl(CMapRenderer* pMapRenderer, CAssetPath ZoneTypePath, const array2d<TILE, allocator_copy<TILE> >& Tiles, vec2 Pos, vec4 LayerColor, CAssetPath ZoneTexture, bool Repeat)
{	
	const CAsset_ZoneType* pZoneType = pMapRenderer->AssetsManager()->GetAsset<CAsset_ZoneType>(ZoneTypePath);
	
	pMapRenderer->AssetsRenderer()->TextureSet(ZoneTexture);
	pMapRenderer->Graphics()->QuadsBegin();

	float TileSize = 32.0f*pMapRenderer->m_CameraZoom;

	//Draw tile layer
	vec2 MinMapPos = pMapRenderer->ScreenPosToMapPos(vec2(pMapRenderer->GetCanvas().x, pMapRenderer->GetCanvas().y));
	vec2 MaxMapPos = pMapRenderer->ScreenPosToMapPos(vec2(pMapRenderer->GetCanvas().x + pMapRenderer->GetCanvas().w, pMapRenderer->GetCanvas().y + pMapRenderer->GetCanvas().h));
	vec2 MinTilePos = pMapRenderer->MapPosToTilePos(MinMapPos - Pos);
	vec2 MaxTilePos = pMapRenderer->MapPosToTilePos(MaxMapPos - Pos);
	int MinX = (int)MinTilePos.x-1;
	int MaxX = (int)MaxTilePos.x+1;
	int MinY = (int)MinTilePos.y-1;
	int MaxY = (int)MaxTilePos.y+1;
	
	bool FadingEnabled = false;
	float GeneralFading = 1.0f;
	if(!Repeat)
	{
		MinX = clamp(MinX, 0, Tiles.get_width()-1);
		MaxX = clamp(MaxX, 0, Tiles.get_width()-1);
		MinY = clamp(MinY, 0, Tiles.get_height()-1);
		MaxY = clamp(MaxY, 0, Tiles.get_height()-1);
	}
	else if((MaxX - MinX) > 128)
	{
		FadingEnabled = true;
		GeneralFading = 0.0f;
		MinX = clamp(MinX, -16, Tiles.get_width()-1+16);
		MaxX = clamp(MaxX, -16, Tiles.get_width()-1+16);
		MinY = clamp(MinY, -16, Tiles.get_height()-1+16);
		MaxY = clamp(MaxY, -16, Tiles.get_height()-1+16);
	}
	else if((MaxX - MinX) > 64)
	{
		FadingEnabled = true;
		GeneralFading = 1.0f - clamp((MaxX - MinX - 64)/64.0f, 0.0f, 1.0f);
	}
	
	for(int j=MinY; j<=MaxY; j++)
	{
		for(int i=MinX; i<=MaxX; i++)
		{
			const TILE& Tile = Tiles.get_clamp(i, j);
			
			if(Tile.GetIndex() < 1)
				continue;
			
			float Fading = 1.0f;
			if(FadingEnabled)
			{
				int DistToBorder = 0;
				if(i<0)
					DistToBorder = max(-i, DistToBorder);
				if(i>=Tiles.get_width())
					DistToBorder = max(i-Tiles.get_width()+1, DistToBorder);
				if(j<0)
					DistToBorder = max(-j, DistToBorder);
				if(j>=Tiles.get_height())
					DistToBorder = max(j-Tiles.get_height()+1, DistToBorder);
				if(DistToBorder > 0)
					Fading *= 1.0f - clamp(DistToBorder / 16.0f, 0.0f, 1.0f);
				Fading = GeneralFading + (1.0f - GeneralFading)*Fading;
			}
			
			bool IndexFound = false;
			vec4 Color = 1.0f;
			if(pZoneType)
			{
				CSubPath IndexPath = CAsset_ZoneType::SubPath_Index(Tile.GetIndex());
				if(pZoneType->IsValidIndex(IndexPath))
				{
					Color = pZoneType->GetIndexColor(IndexPath);
					IndexFound = true;
				}
			}
			
			if(!IndexFound)
				continue;
			
			Color.a *= 0.5f * Fading;
			pMapRenderer->Graphics()->SetColor(LayerColor * Color, true);
			
			vec2 TilePos = pMapRenderer->MapPosToScreenPos(Pos + vec2(i*32.0f, j*32.0f));
			
			float Shift = fmod(pMapRenderer->m_LocalTime/4.0f, 1.0f);
			int Size = 8;
			
			//~ float x0 = ((i%Size))/static_cast<float>(Size) + Shift;
			//~ float y0 = ((j%Size))/static_cast<float>(Size) + Shift;
			//~ float x2 = x0 + 1.0f/static_cast<float>(Size);
			//~ float y2 = y0 + 1.0f/static_cast<float>(Size);
			//~ float x1 = x2;
			//~ float y1 = y0;
			//~ float x3 = x0;
			//~ float y3 = y2;
			float x0 = (TilePos.x/TileSize)/16.0f + Shift;
			float y0 = (TilePos.y/TileSize)/16.0f + Shift;
			float x2 = (TilePos.x/TileSize+1.0f)/16.0f + Shift;
			float y2 = (TilePos.y/TileSize+1.0f)/16.0f + Shift;
			float x1 = x2;
			float y1 = y0;
			float x3 = x0;
			float y3 = y2;
			
			pMapRenderer->Graphics()->QuadsSetSubsetFree(x0, y0, x1, y1, x2, y2, x3, y3);
			CGraphics::CQuadItem QuadItem(TilePos.x, TilePos.y, TileSize, TileSize);
			pMapRenderer->Graphics()->QuadsDrawTL(&QuadItem, 1);
		}
	}
	pMapRenderer->Graphics()->QuadsEnd();
	
	pMapRenderer->Graphics()->TextureClear();
	pMapRenderer->Graphics()->QuadsBegin();
	
	//Draw border
	for(int j=MinY; j<MaxY; j++)
	{
		for(int i=MinX; i<=MaxX; i++)
		{
			const TILE& Tile = Tiles.get_clamp(i, j);
			
			if(Tile.GetIndex() < 1)
				continue;
			
			float Fading = 1.0f;
			if(FadingEnabled)
			{
				int DistToBorder = 0;
				if(i<0)
					DistToBorder = max(-i, DistToBorder);
				if(i>=Tiles.get_width())
					DistToBorder = max(i-Tiles.get_width()+1, DistToBorder);
				if(j<0)
					DistToBorder = max(-j, DistToBorder);
				if(j>=Tiles.get_height())
					DistToBorder = max(j-Tiles.get_height()+1, DistToBorder);
				if(DistToBorder > 0)
					Fading *= 1.0f - clamp(DistToBorder / 16.0f, 0.0f, 1.0f);
				Fading = GeneralFading + (1.0f - GeneralFading)*Fading;
			}
			
			bool IndexFound = false;
			vec4 Color = 1.0f;
			if(pZoneType)
			{
				CSubPath IndexPath = CAsset_ZoneType::SubPath_Index(Tile.GetIndex());
				if(pZoneType->IsValidIndex(IndexPath))
				{
					Color = pZoneType->GetIndexColor(IndexPath);
					IndexFound = true;
				}
			}
			
			Color.a *= Fading;
			
			if(!IndexFound)
				continue;
			
			pMapRenderer->Graphics()->SetColor(LayerColor * Color, true);
			
			vec2 TilePos = pMapRenderer->MapPosToScreenPos(Pos + vec2(i*32.0f, j*32.0f));
			
			bool AirL = (Tiles.get_clamp(i-1, j).GetIndex() != Tile.GetIndex());
			bool AirR = (Tiles.get_clamp(i+1, j).GetIndex() != Tile.GetIndex());
			bool AirT = (Tiles.get_clamp(i, j-1).GetIndex() != Tile.GetIndex());
			bool AirB = (Tiles.get_clamp(i, j+1).GetIndex() != Tile.GetIndex());
			
			bool AirTL = (Tiles.get_clamp(i-1, j-1).GetIndex() != Tile.GetIndex());
			bool AirTR = (Tiles.get_clamp(i+1, j-1).GetIndex() != Tile.GetIndex());
			bool AirBL = (Tiles.get_clamp(i-1, j+1).GetIndex() != Tile.GetIndex());
			bool AirBR = (Tiles.get_clamp(i+1, j+1).GetIndex() != Tile.GetIndex());
			
			if(AirT)
			{
				CGraphics::CFreeformItem Freeform(
					TilePos.x, TilePos.y,
					TilePos.x + TileSize, TilePos.y,
					TilePos.x + (AirL ? TileSize/4.0f : 0.0f), TilePos.y+TileSize/4.0f,
					TilePos.x + TileSize - (AirR ? TileSize/4.0f : 0.0f), TilePos.y+TileSize/4.0f
				);
				pMapRenderer->Graphics()->QuadsDrawFreeform(&Freeform, 1);
			}
			if(AirB)
			{
				CGraphics::CFreeformItem Freeform(
					TilePos.x, TilePos.y+TileSize,
					TilePos.x+TileSize, TilePos.y+TileSize,
					TilePos.x+(AirL ? TileSize/4.0f : 0.0f), TilePos.y+TileSize-TileSize/4.0f,
					TilePos.x+TileSize-(AirR ? TileSize/4.0f : 0.0f), TilePos.y+TileSize-TileSize/4.0f
				);
				pMapRenderer->Graphics()->QuadsDrawFreeform(&Freeform, 1);
			}
			if(AirL)
			{
				CGraphics::CFreeformItem Freeform(
					TilePos.x, TilePos.y,
					TilePos.x, TilePos.y + TileSize,
					TilePos.x+TileSize/4.0f, TilePos.y+(AirT ? TileSize/4.0f : 0.0f),
					TilePos.x+TileSize/4.0f, TilePos.y+TileSize-(AirB ? TileSize/4.0f : 0.0f)
				);
				pMapRenderer->Graphics()->QuadsDrawFreeform(&Freeform, 1);
			}
			if(AirR)
			{
				CGraphics::CFreeformItem Freeform(
					TilePos.x+TileSize, TilePos.y,
					TilePos.x+TileSize, TilePos.y+TileSize,
					TilePos.x+TileSize-TileSize/4.0f, TilePos.y+(AirT ? TileSize/4.0f : 0.0f),
					TilePos.x+TileSize-TileSize/4.0f, TilePos.y+TileSize-(AirB ? TileSize/4.0f : 0.0f)
				);
				pMapRenderer->Graphics()->QuadsDrawFreeform(&Freeform, 1);
			}
			
			if(!AirT && !AirL && AirTL)
			{
				CGraphics::CFreeformItem Freeform(
					TilePos.x, TilePos.y,
					TilePos.x+TileSize/4.0f, TilePos.y,
					TilePos.x, TilePos.y+TileSize/4.0f,
					TilePos.x+TileSize/4.0f, TilePos.y+TileSize/4.0f
				);
				pMapRenderer->Graphics()->QuadsDrawFreeform(&Freeform, 1);
			}
			if(!AirT && !AirR && AirTR)
			{
				CGraphics::CFreeformItem Freeform(
					TilePos.x+TileSize, TilePos.y,
					TilePos.x+TileSize-TileSize/4.0f, TilePos.y,
					TilePos.x+TileSize, TilePos.y+TileSize/4.0f,
					TilePos.x+TileSize-TileSize/4.0f, TilePos.y+TileSize/4.0f
				);
				pMapRenderer->Graphics()->QuadsDrawFreeform(&Freeform, 1);
			}
			if(!AirB && !AirL && AirBL)
			{
				CGraphics::CFreeformItem Freeform(
					TilePos.x, TilePos.y+TileSize,
					TilePos.x+TileSize/4.0f, TilePos.y+TileSize,
					TilePos.x, TilePos.y+TileSize-TileSize/4.0f,
					TilePos.x+TileSize/4.0f, TilePos.y+TileSize-TileSize/4.0f
				);
				pMapRenderer->Graphics()->QuadsDrawFreeform(&Freeform, 1);
			}
			if(!AirB && !AirR && AirBR)
			{
				CGraphics::CFreeformItem Freeform(
					TilePos.x+TileSize, TilePos.y+TileSize,
					TilePos.x+TileSize-TileSize/4.0f, TilePos.y+TileSize,
					TilePos.x+TileSize, TilePos.y+TileSize-TileSize/4.0f,
					TilePos.x+TileSize-TileSize/4.0f, TilePos.y+TileSize-TileSize/4.0f
				);
				pMapRenderer->Graphics()->QuadsDrawFreeform(&Freeform, 1);
			}
		}
	}
	
	pMapRenderer->Graphics()->QuadsEnd();
}

void CMapRenderer::RenderTiles_Zone(CAssetPath ZoneTypePath, const array2d<CAsset_MapLayerTiles::CTile, allocator_copy<CAsset_MapLayerTiles::CTile> >& Tiles, vec2 Pos, vec4 Color, CAssetPath ZoneTexture, bool Repeat)
{
	RenderTiles_Zone_Impl(this, ZoneTypePath, Tiles, Pos, Color, ZoneTexture, Repeat);
}

void CMapRenderer::RenderTiles_Zone(CAssetPath ZoneTypePath, const array2d<CAsset_MapZoneTiles::CTile, allocator_copy<CAsset_MapZoneTiles::CTile> >& Tiles, vec2 Pos, vec4 Color, CAssetPath ZoneTexture, bool Repeat)
{
	RenderTiles_Zone_Impl(this, ZoneTypePath, Tiles, Pos, Color, ZoneTexture, Repeat);
}

void CMapRenderer::RenderGrid(float Step, vec4 Color)
{
	//Draw tile layer
	vec2 MinMapPos = ScreenPosToMapPos(vec2(GetCanvas().x, GetCanvas().y));
	vec2 MaxMapPos = ScreenPosToMapPos(vec2(GetCanvas().x + GetCanvas().w, GetCanvas().y + GetCanvas().h));
	int MinX = (int)(MinMapPos.x/Step)-1;
	int MaxX = (int)(MaxMapPos.x/Step)+1;
	int MinY = (int)(MinMapPos.y/Step)-1;
	int MaxY = (int)(MaxMapPos.y/Step)+1;
	MinMapPos.x = MinX*Step;
	MinMapPos.y = MinY*Step;
	MaxMapPos.x = MaxX*Step;
	MaxMapPos.y = MaxY*Step;
	
	Graphics()->TextureClear();
	Graphics()->LinesBegin();
	Graphics()->SetColor(Color, true);
	
	//Draw tile layer
	float Iter = MinMapPos.x;
	while(Iter < MaxMapPos.x)
	{
		vec2 Pos0 = MapPosToScreenPos(vec2(Iter, MinMapPos.y));
		vec2 Pos1 = MapPosToScreenPos(vec2(Iter, MaxMapPos.y));
		CGraphics::CLineItem Line(static_cast<int>(Pos0.x)+0.5f, static_cast<int>(Pos0.y)+0.5f, static_cast<int>(Pos1.x)+0.5f, static_cast<int>(Pos1.y)+0.5f);
		Graphics()->LinesDraw(&Line, 1);
		Iter += Step;
	}
	Iter = MinMapPos.y;
	while(Iter < MaxMapPos.y)
	{
		vec2 Pos0 = MapPosToScreenPos(vec2(MinMapPos.x, Iter));
		vec2 Pos1 = MapPosToScreenPos(vec2(MaxMapPos.x, Iter));
		CGraphics::CLineItem Line(static_cast<int>(Pos0.x)+0.5f, static_cast<int>(Pos0.y)+0.5f, static_cast<int>(Pos1.x)+0.5f, static_cast<int>(Pos1.y)+0.5f);
		Graphics()->LinesDraw(&Line, 1);
		Iter += Step;
	}
	
	Graphics()->LinesEnd();
}

void CMapRenderer::RenderTiles_Image(const array2d<CAsset_MapLayerTiles::CTile, allocator_copy<CAsset_MapLayerTiles::CTile> >& Tiles, vec2 Pos, CAssetPath ImagePath, vec4 Color, bool Repeat)
{
	AssetsRenderer()->TextureSet(ImagePath);
	
	Graphics()->QuadsBegin();

	//Draw tile layer
	vec2 MinMapPos = ScreenPosToMapPos(vec2(GetCanvas().x, GetCanvas().y));
	vec2 MaxMapPos = ScreenPosToMapPos(vec2(GetCanvas().x + GetCanvas().w, GetCanvas().y + GetCanvas().h));
	vec2 MinTilePos = MapPosToTilePos(MinMapPos - Pos);
	vec2 MaxTilePos = MapPosToTilePos(MaxMapPos - Pos);
	int MinX = (int)MinTilePos.x-1;
	int MaxX = (int)MaxTilePos.x+1;
	int MinY = (int)MinTilePos.y-1;
	int MaxY = (int)MaxTilePos.y+1;
	
	bool FadingEnabled = false;
	float GeneralFading = 1.0f;
	if(!Repeat)
	{
		MinX = clamp(MinX, 0, Tiles.get_width()-1);
		MaxX = clamp(MaxX, 0, Tiles.get_width()-1);
		MinY = clamp(MinY, 0, Tiles.get_height()-1);
		MaxY = clamp(MaxY, 0, Tiles.get_height()-1);
	}
	else if((MaxX - MinX) > 128)
	{
		FadingEnabled = true;
		GeneralFading = 0.0f;
		MinX = clamp(MinX, -16, Tiles.get_width()-1+16);
		MaxX = clamp(MaxX, -16, Tiles.get_width()-1+16);
		MinY = clamp(MinY, -16, Tiles.get_height()-1+16);
		MaxY = clamp(MaxY, -16, Tiles.get_height()-1+16);
	}
	else if((MaxX - MinX) > 64)
	{
		FadingEnabled = true;
		GeneralFading = 1.0f - clamp((MaxX - MinX - 64)/64.0f, 0.0f, 1.0f);
	}
	
	float USpacing = 0.0f;
	float VSpacing = 0.0f;
	
	const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(ImagePath);
	if(pImage)
	{
		USpacing = pImage->GetGridSpacing() * pImage->GetGridWidth() / static_cast<float>(pImage->GetDataWidth());
		VSpacing = pImage->GetGridSpacing() * pImage->GetGridHeight() / static_cast<float>(pImage->GetDataHeight());
	}
	
	for(int j=MinY; j<=MaxY; j++)
	{
		for(int i=MinX; i<=MaxX; i++)
		{
			const CAsset_MapLayerTiles::CTile& Tile = Tiles.get_clamp(i, j);
			
			int Index = Tile.GetIndex();
			if(Index)
			{
				vec2 TilePos = MapPosToScreenPos(Pos + vec2(i*32.0f, j*32.0f));
				
				float Fading = 1.0f;
				if(FadingEnabled)
				{
					int DistToBorder = 0;
					if(i<0)
						DistToBorder = max(-i, DistToBorder);
					if(i>=Tiles.get_width())
						DistToBorder = max(i-Tiles.get_width()+1, DistToBorder);
					if(j<0)
						DistToBorder = max(-j, DistToBorder);
					if(j>=Tiles.get_height())
						DistToBorder = max(j-Tiles.get_height()+1, DistToBorder);
					if(DistToBorder > 0)
						Fading *= 1.0f - clamp(DistToBorder / 16.0f, 0.0f, 1.0f);
					Fading = GeneralFading + (1.0f - GeneralFading)*Fading;
				}
				vec4 TileColor = Color;
				TileColor.a *= Fading;
				Graphics()->SetColor(TileColor, true);
				
				float x0 = USpacing;
				float y0 = VSpacing;
				float x1 = 1.0f-USpacing;
				float y1 = VSpacing;
				float x2 = 1.0f-USpacing;
				float y2 = 1.0f-VSpacing;
				float x3 = USpacing;
				float y3 = 1.0f-VSpacing;

				if(Tile.GetFlags()&CAsset_MapLayerTiles::TILEFLAG_VFLIP)
				{
					x0 = x2;
					x1 = x3;
					x2 = x3;
					x3 = x0;
				}

				if(Tile.GetFlags()&CAsset_MapLayerTiles::TILEFLAG_HFLIP)
				{
					y0 = y3;
					y2 = y1;
					y3 = y1;
					y1 = y0;
				}

				if(Tile.GetFlags()&CAsset_MapLayerTiles::TILEFLAG_ROTATE)
				{
					float Tmp = x0;
					x0 = x3;
					x3 = x2;
					x2 = x1;
					x1 = Tmp;
					Tmp = y0;
					y0 = y3;
					y3 = y2;
					y2 = y1;
					y1 = Tmp;
				}
				
				Graphics()->QuadsSetSubsetFree(x0, y0, x1, y1, x2, y2, x3, y3, Index);
				CGraphics::CQuadItem QuadItem(TilePos.x, TilePos.y, 32.0f*m_CameraZoom, 32.0f*m_CameraZoom);
				Graphics()->QuadsDrawTL(&QuadItem, 1);
			}
		}
	}
	
	Graphics()->QuadsEnd();
}

void CMapRenderer::RenderQuads(const CAsset_MapLayerQuads::CQuad* pQuads, int NbQuads, vec2 LayerPos, CAssetPath ImagePath, vec4 LayerColor)
{
	AssetsRenderer()->TextureSet(ImagePath);
	
	Graphics()->QuadsBegin();
	
	for(int i=0; i<NbQuads; i++)
	{
		const CAsset_MapLayerQuads::CQuad* pQuad = &pQuads[i];
				
		vec2 Position;
		matrix2x2 Transform;
		vec4 Color;
		int DrawState;
		pQuad->GetTransform(AssetsManager(), m_Time, &Transform, &Position);
		pQuad->GetDrawState(AssetsManager(), m_Time, &Color, &DrawState);
		
		if(DrawState == CAsset_SkeletonAnimation::LAYERSTATE_HIDDEN)
			continue;
		
		Position += LayerPos;
		Color *= LayerColor;
		
		//Color
		Graphics()->SetColor4(pQuad->GetColor0()*Color, pQuad->GetColor1()*Color, pQuad->GetColor2()*Color, pQuad->GetColor3()*Color, true);
		
		//UVs
		bool RepeatU = false;
		bool RepeatV = false;
		
		if(pQuad->GetUV0().x < 0.0f || pQuad->GetUV0().x > 1.0f)
			RepeatU = true;
		if(pQuad->GetUV0().y < 0.0f || pQuad->GetUV0().y > 1.0f)
			RepeatV = true;
		if(pQuad->GetUV1().x < 0.0f || pQuad->GetUV1().x > 1.0f)
			RepeatU = true;
		if(pQuad->GetUV1().y < 0.0f || pQuad->GetUV1().y > 1.0f)
			RepeatV = true;
		if(pQuad->GetUV2().x < 0.0f || pQuad->GetUV2().x > 1.0f)
			RepeatU = true;
		if(pQuad->GetUV2().y < 0.0f || pQuad->GetUV2().y > 1.0f)
			RepeatV = true;
		if(pQuad->GetUV3().x < 0.0f || pQuad->GetUV3().x > 1.0f)
			RepeatU = true;
		if(pQuad->GetUV3().y < 0.0f || pQuad->GetUV3().y > 1.0f)
			RepeatV = true;
		
		Graphics()->WrapMode(
			RepeatU ? CGraphics::WRAP_REPEAT : CGraphics::WRAP_CLAMP,
			RepeatV ? CGraphics::WRAP_REPEAT : CGraphics::WRAP_CLAMP);
		
		Graphics()->QuadsSetSubsetFree(
			pQuad->GetUV0().x, pQuad->GetUV0().y,
			pQuad->GetUV1().x, pQuad->GetUV1().y,
			pQuad->GetUV2().x, pQuad->GetUV2().y,
			pQuad->GetUV3().x, pQuad->GetUV3().y
		);
		
		//Vertices
		
		vec2 Vertices[4];
		{
			vec2 TestBefore = pQuad->GetVertex0();
			vec2 TestAfter = Transform*pQuad->GetVertex0() + Position;
			Vertices[0] = MapPosToScreenPos(Transform*pQuad->GetVertex0() + Position);
		}
		{
			vec2 TestBefore = pQuad->GetVertex1();
			vec2 TestAfter = Transform*pQuad->GetVertex1() + Position;
			Vertices[1] = MapPosToScreenPos(Transform*pQuad->GetVertex1() + Position);
		}
		{
			vec2 TestBefore = pQuad->GetVertex2();
			vec2 TestAfter = Transform*pQuad->GetVertex2() + Position;
			Vertices[2] = MapPosToScreenPos(Transform*pQuad->GetVertex2() + Position);
		}
		{
			vec2 TestBefore = pQuad->GetVertex3();
			vec2 TestAfter = Transform*pQuad->GetVertex3() + Position;
			Vertices[3] = MapPosToScreenPos(Transform*pQuad->GetVertex3() + Position);
		}

		CGraphics::CFreeformItem Freeform(
			Vertices[0].x, Vertices[0].y,
			Vertices[1].x, Vertices[1].y,
			Vertices[2].x, Vertices[2].y,
			Vertices[3].x, Vertices[3].y
		);
		Graphics()->QuadsDrawFreeform(&Freeform, 1);
		
	}
	
	Graphics()->QuadsEnd();
	Graphics()->WrapNormal();
}

void CMapRenderer::RenderQuads_Mesh(const CAsset_MapLayerQuads::CQuad* pQuads, int NbQuads)
{
	Graphics()->TextureClear();
	Graphics()->LinesBegin();
	
	for(int i=0; i<NbQuads; i++)
	{
		const CAsset_MapLayerQuads::CQuad* pQuad = &pQuads[i];
		
		vec4 Color = 1.0f;
		
		vec2 Position;
		matrix2x2 Transform;
		pQuad->GetTransform(AssetsManager(), m_Time, &Transform, &Position);
		
		//Vertices
		{
			{
				vec2 Vertex0Pos = MapPosToScreenPos(Transform * pQuads[i].GetVertex0() + Position);
				vec2 Vertex1Pos = MapPosToScreenPos(Transform * pQuads[i].GetVertex1() + Position);
				
				Graphics()->SetColor2(pQuads[i].GetColor0(), pQuads[i].GetColor1(), true);
				
				CGraphics::CLineItem Line(Vertex0Pos.x, Vertex0Pos.y, Vertex1Pos.x, Vertex1Pos.y);
				Graphics()->LinesDraw(&Line, 1);
			}
			{
				vec2 Vertex0Pos = MapPosToScreenPos(Transform * pQuads[i].GetVertex1() + Position);
				vec2 Vertex1Pos = MapPosToScreenPos(Transform * pQuads[i].GetVertex3() + Position);
				
				Graphics()->SetColor2(pQuads[i].GetColor1(), pQuads[i].GetColor3(), true);
				
				CGraphics::CLineItem Line(Vertex0Pos.x, Vertex0Pos.y, Vertex1Pos.x, Vertex1Pos.y);
				Graphics()->LinesDraw(&Line, 1);
			}
			{
				vec2 Vertex0Pos = MapPosToScreenPos(Transform * pQuads[i].GetVertex3() + Position);
				vec2 Vertex1Pos = MapPosToScreenPos(Transform * pQuads[i].GetVertex2() + Position);
				
				Graphics()->SetColor2(pQuads[i].GetColor3(), pQuads[i].GetColor2(), true);
				
				CGraphics::CLineItem Line(Vertex0Pos.x, Vertex0Pos.y, Vertex1Pos.x, Vertex1Pos.y);
				Graphics()->LinesDraw(&Line, 1);
			}
			{
				vec2 Vertex0Pos = MapPosToScreenPos(Transform * pQuads[i].GetVertex2() + Position);
				vec2 Vertex1Pos = MapPosToScreenPos(Transform * pQuads[i].GetVertex0() + Position);
				
				Graphics()->SetColor2(pQuads[i].GetColor2(), pQuads[i].GetColor0(), true);
				
				CGraphics::CLineItem Line(Vertex0Pos.x, Vertex0Pos.y, Vertex1Pos.x, Vertex1Pos.y);
				Graphics()->LinesDraw(&Line, 1);
			}
		}
	}
	
	Graphics()->LinesEnd();
}

enum
{
	VERTEXFLAG_BEZIER = 1,
};

struct CVertex
{
	vec4 m_Color;
	vec2 m_Position;
	vec2 m_LeftOrtho;
	vec2 m_RightOrtho;
	int m_Flags;
	float m_LeftDist;
	float m_RightDist;
	float m_Weight;
};

class CLineTesselator
{
private:
	array<CVertex> m_aVertexBuffer[2];
	int m_CurrentBuffer;

public:
	CLineTesselator() :
		m_CurrentBuffer(0)
	{
		
	}
	
	inline array<CVertex>& GetCurrentVertices() { return m_aVertexBuffer[m_CurrentBuffer]; }
	inline array<CVertex>& GetNextVertices() { return m_aVertexBuffer[(m_CurrentBuffer+1)%2]; }
	
	inline array<CVertex>& SwapBuffers()
	{
		m_CurrentBuffer = (m_CurrentBuffer+1)%2;
		GetNextVertices().clear();
	}

	void AddVertex(vec2 Position, float Weight, vec4 Color, int Flags)
	{
		CVertex& Vertex = GetCurrentVertices().increment();
		Vertex.m_Position = Position;
		Vertex.m_Weight = Weight;
		Vertex.m_Color = Color;
		Vertex.m_Flags = Flags;
	}
	
	void ComputeOrthogonalVertors(bool Closed, int Buffer=0)
	{
		int NumVertices = m_aVertexBuffer[(m_CurrentBuffer+Buffer)%2].size();
		for(int i=0; i<NumVertices; i++)
		{
			vec2 Position = m_aVertexBuffer[(m_CurrentBuffer+Buffer)%2][i].m_Position;
			vec2 DirLeft = 0.0f;
			vec2 DirRight = 0.0f;
			
			if(i>0)
			{
				vec2 PositionPrev = m_aVertexBuffer[(m_CurrentBuffer+Buffer)%2][i-1].m_Position;
				DirLeft = normalize(Position - PositionPrev);
			}
			else if(Closed && NumVertices >= 2)
			{
				vec2 PositionPrev = m_aVertexBuffer[(m_CurrentBuffer+Buffer)%2][NumVertices-2].m_Position;
				DirLeft = normalize(Position - PositionPrev);
			}
			
			if(i+1<NumVertices)
			{
				vec2 PositionNext = m_aVertexBuffer[(m_CurrentBuffer+Buffer)%2][i+1].m_Position;
				DirRight = normalize(PositionNext - Position);
			}
			else if(Closed && NumVertices >= 2)
			{
				vec2 PositionNext = m_aVertexBuffer[(m_CurrentBuffer+Buffer)%2][1].m_Position;
				DirRight = normalize(PositionNext - Position);
			}
			
			vec2 Dir = normalize(DirLeft + DirRight);	
			vec2 OrthoDir = ortho(Dir);
			
			float Length = 1.0f;
			if(length(DirLeft) > 0.0f)
				Length = 1.0f / dot(OrthoDir, ortho(DirLeft));
			else if(length(DirRight))
				Length = 1.0f / dot(OrthoDir, ortho(DirRight));
			
			m_aVertexBuffer[(m_CurrentBuffer+Buffer)%2][i].m_LeftOrtho = OrthoDir * Length * m_aVertexBuffer[(m_CurrentBuffer+Buffer)%2][i].m_Weight;
			m_aVertexBuffer[(m_CurrentBuffer+Buffer)%2][i].m_RightOrtho = m_aVertexBuffer[(m_CurrentBuffer+Buffer)%2][i].m_LeftOrtho;
		}
	}
	
	void ComputeBezierCurve()
	{
		if(GetCurrentVertices().size())
		{
			CVertex& Vertex = GetNextVertices().increment();
			Vertex = GetCurrentVertices()[0];
		}
		int NumVertices = GetCurrentVertices().size();
		for(int i=1; i<NumVertices; i++)
		{
			vec2 Position0 = GetCurrentVertices()[i-1].m_Position;			
			vec2 Position1 = GetCurrentVertices()[i].m_Position;
			float Length = distance(Position0, Position1);
			
			float Weight0 = GetCurrentVertices()[i-1].m_Weight;
			float Weight1 = GetCurrentVertices()[i].m_Weight;
			
			vec4 Color0 = GetCurrentVertices()[i-1].m_Color;
			vec4 Color1 = GetCurrentVertices()[i].m_Color;
			
			bool AutoSmooth0 = GetCurrentVertices()[i-1].m_Flags & VERTEXFLAG_BEZIER;
			bool AutoSmooth1 = GetCurrentVertices()[i].m_Flags & VERTEXFLAG_BEZIER;
			
			if(AutoSmooth0 || AutoSmooth1)
			{
				vec2 BezierPoint0;
				vec2 BezierPoint1;
				
				if(AutoSmooth0)
					BezierPoint0 = Position0 - normalize(ortho(GetCurrentVertices()[i-1].m_RightOrtho)) * Length/3.0f;
				else
					BezierPoint0 = Position0 + (Position1 - Position0)/3.0f;
				
				if(AutoSmooth1)
					BezierPoint1 = Position1 + normalize(ortho(GetCurrentVertices()[i].m_LeftOrtho)) * Length/3.0f;
				else
					BezierPoint1 = Position1 - (Position1 - Position0)/3.0f;
				
				int NbSegments = Length/32.0f;
				for(int j=1; j<NbSegments; j++)
				{
					float Alpha = static_cast<float>(j)/NbSegments; 
					vec2 Tmp0 = Position0 + (BezierPoint0 - Position0)*Alpha;
					vec2 Tmp1 = BezierPoint0 + (BezierPoint1 - BezierPoint0)*Alpha;
					vec2 Tmp2 = BezierPoint1 + (Position1 - BezierPoint1)*Alpha;
					vec2 Tmp3 = Tmp0 + (Tmp1 - Tmp0)*Alpha;
					vec2 Tmp4 = Tmp1 + (Tmp2 - Tmp1)*Alpha;
					vec2 Point = Tmp3 + (Tmp4 - Tmp3)*Alpha;
					
					CVertex& Vertex = GetNextVertices().increment();
					Vertex.m_Position = Point;
					Vertex.m_Color = Color0 * (1.0f - Alpha) + Color1 * Alpha;
					Vertex.m_Weight = Weight0 * (1.0f - Alpha) + Weight1 * Alpha;
					Vertex.m_Flags = 0x0;
				}
			}
			{
				CVertex& Vertex = GetNextVertices().increment();
				Vertex = GetCurrentVertices()[i];
			}
		}
		SwapBuffers();
	}
};

void CMapRenderer::RenderLine(const array< CAsset_MapLayerObjects::CVertex, allocator_copy<CAsset_MapLayerObjects::CVertex> >& Vertices, vec2 Pos, const matrix2x2& Transform, CAssetPath MaterialPath, bool Closed, bool DrawMesh)
{
	const CAsset_Material* pMaterial = AssetsManager()->GetAsset<CAsset_Material>(MaterialPath);
	if(pMaterial)
	{
		CLineTesselator Tesselator;
		
		//Copy map vertices into the vertex buffer;
		for(int i=0; i<Vertices.size(); i++)
		{
			int Flags = 0x0;
			
			if(Vertices[i].GetSmoothness() & CAsset_MapLayerObjects::SMOOTHNESS_AUTOMATIC)
				Flags |= VERTEXFLAG_BEZIER;
			
			Tesselator.AddVertex(Vertices[i].GetPosition(), Vertices[i].GetWeight(), Vertices[i].GetColor(), Flags);
		}
		if(Closed && Vertices.size() > 0)
		{
			int Flags = 0x0;
			
			if(Vertices[0].GetSmoothness() & CAsset_MapLayerObjects::SMOOTHNESS_AUTOMATIC)
				Flags |= VERTEXFLAG_BEZIER;
			
			Tesselator.AddVertex(Vertices[0].GetPosition(), Vertices[0].GetWeight(), Vertices[0].GetColor(), Flags);
		}
		
		//Remove this step once Bezier points are stored!!
		//Compute orthogonal vectors
		Tesselator.ComputeOrthogonalVertors(Closed);
		
		//Apply bezier curves
		//This is a test: the bezier curve is forced instead of being stored somewhere
		Tesselator.ComputeBezierCurve();
		
		CAsset_Material::CIteratorLayer LayerIter;
		for(LayerIter = pMaterial->BeginLayer(); LayerIter != pMaterial->EndLayer(); ++LayerIter)
		{
			const array< CAsset_Material::CSprite, allocator_copy<CAsset_Material::CSprite> >& Sprites = pMaterial->GetLayerSpriteArray(*LayerIter);
			
			if(!Sprites.size())
				continue;
			
			const CAsset_Material::CSprite& Sprite = Sprites[0];
			
			CAssetsRenderer::CSpriteInfo SpriteInfo;
			AssetsRenderer()->GetSpriteInfo(pMaterial->GetLayerSpritePath(*LayerIter), SpriteInfo);
			
			//Cut segments to repeat the texture
				//First step: compute the total length
			float TotalLength = 0.0f;
			for(int i=1; i<Tesselator.GetCurrentVertices().size(); i++)
			{
				vec2 Position0 = Tesselator.GetCurrentVertices()[i-1].m_Position;			
				vec2 Position1 = Tesselator.GetCurrentVertices()[i].m_Position;
				
				float Weight0 = Tesselator.GetCurrentVertices()[i-1].m_Weight;
				float Weight1 = Tesselator.GetCurrentVertices()[i].m_Weight;
				
				TotalLength += distance(Position0, Position1)/(SpriteInfo.m_Width * (Weight0 + Weight1)/2.0f);
			}
			float GlobalWeight = TotalLength/max((int)round(TotalLength), 1);
			
				//Second step: cut the path
			float LengthIter = 0.0;
			if(Tesselator.GetCurrentVertices().size())
			{
				CVertex& Vertex = Tesselator.GetNextVertices().increment();
				Vertex = Tesselator.GetCurrentVertices()[0];
				Vertex.m_LeftDist = 0.0f;
				Vertex.m_RightDist = 0.0f;
				Vertex.m_Flags = 0x0;
			}
			for(int i=1; i<Tesselator.GetCurrentVertices().size(); i++)
			{
				vec2 Position0 = Tesselator.GetCurrentVertices()[i-1].m_Position;			
				vec2 Position1 = Tesselator.GetCurrentVertices()[i].m_Position;
				
				float Weight0 = Tesselator.GetCurrentVertices()[i-1].m_Weight * GlobalWeight;
				float Weight1 = Tesselator.GetCurrentVertices()[i].m_Weight * GlobalWeight;
				
				vec4 Color0 = Tesselator.GetCurrentVertices()[i-1].m_Color;
				vec4 Color1 = Tesselator.GetCurrentVertices()[i].m_Color;
				
				float Length = distance(Position0, Position1)/(SpriteInfo.m_Width * (Weight0 + Weight1)/2.0f);
				float NextLengthIter = LengthIter + Length;
				int NumSegments = ceil(NextLengthIter) - floor(LengthIter);
				
				for(int j=0; j<NumSegments; j++)
				{
					float Begin = fmod(LengthIter, 1.0);
					float End = min(NextLengthIter - floor(LengthIter), 1.0);
					
					LengthIter += End - Begin;
					float Alpha = (NextLengthIter - LengthIter)/Length;
					
					vec2 Pos = Position0 * Alpha + Position1 * (1.0f - Alpha);
					
					Tesselator.GetNextVertices()[Tesselator.GetNextVertices().size()-1].m_RightDist = Begin;
					
					CVertex& Vertex = Tesselator.GetNextVertices().increment();
					Vertex.m_Position = Pos;
					Vertex.m_Color = Color0 * Alpha + Color1 * (1.0f - Alpha);
					Vertex.m_Weight = Weight0 * Alpha + Weight1 * (1.0f - Alpha);
					Vertex.m_LeftDist = 0.0f;
					Vertex.m_LeftDist = End;
					Vertex.m_Flags = 0x0;
				}
				
				LengthIter = NextLengthIter;
			}
			
			Tesselator.ComputeOrthogonalVertors(Closed, 1);
			
			if(Closed && Tesselator.GetNextVertices().size())
				Tesselator.GetNextVertices().increment() = Tesselator.GetNextVertices()[0];
			
			//Draw quads
			int VerticalTesselation = 4;
			AssetsRenderer()->TextureSet(SpriteInfo.m_ImagePath);
			Graphics()->QuadsBegin();
			for(int i=1; i<Tesselator.GetNextVertices().size(); i++)
			{
				vec2 Position0 = Tesselator.GetNextVertices()[i-1].m_Position;			
				vec2 Position1 = Tesselator.GetNextVertices()[i].m_Position;
				vec2 Ortho0 = Tesselator.GetNextVertices()[i-1].m_RightOrtho * SpriteInfo.m_Height/2.0f;
				vec2 Ortho1 = Tesselator.GetNextVertices()[i].m_LeftOrtho * SpriteInfo.m_Height/2.0f;
				vec4 Color0 = Tesselator.GetNextVertices()[i-1].m_Color;
				vec4 Color1 = Tesselator.GetNextVertices()[i].m_Color;
					
				vec2 P00 = MapPosToScreenPos(Position0 + Ortho0);
				vec2 P01 = MapPosToScreenPos(Position1 + Ortho1);
				vec2 P10 = MapPosToScreenPos(Position0);
				vec2 P11 = MapPosToScreenPos(Position1);
				vec2 P20 = MapPosToScreenPos(Position0 - Ortho0);
				vec2 P21 = MapPosToScreenPos(Position1 - Ortho1);
				
				float USize = SpriteInfo.m_UMax - SpriteInfo.m_UMin;
				
				Graphics()->SetColor4(Color0, Color1, Color0, Color1, true);
				
				for(int j=0; j<VerticalTesselation; j++)
				{
					float StepMin = -(2.0f * static_cast<float>(j)/VerticalTesselation - 1.0f);
					float StepMax = -(2.0f * static_cast<float>(j+1)/VerticalTesselation - 1.0f);
					float VMin = static_cast<float>(j)/VerticalTesselation;
					float VMax = static_cast<float>(j+1)/VerticalTesselation;
					
					vec2 P00 = MapPosToScreenPos(Position0 + Ortho0 * StepMin);
					vec2 P01 = MapPosToScreenPos(Position1 + Ortho1 * StepMin);
					vec2 P10 = MapPosToScreenPos(Position0 + Ortho0 * StepMax);
					vec2 P11 = MapPosToScreenPos(Position1 + Ortho1 * StepMax);
					
					Graphics()->QuadsSetSubsetFree(
						SpriteInfo.m_UMin + USize * Tesselator.GetNextVertices()[i-1].m_RightDist, SpriteInfo.m_VMin + (SpriteInfo.m_VMax - SpriteInfo.m_VMin) * VMin,
						SpriteInfo.m_UMin + USize * Tesselator.GetNextVertices()[i].m_LeftDist, SpriteInfo.m_VMin + (SpriteInfo.m_VMax - SpriteInfo.m_VMin) * VMin,
						SpriteInfo.m_UMin + USize * Tesselator.GetNextVertices()[i-1].m_RightDist, SpriteInfo.m_VMin + (SpriteInfo.m_VMax - SpriteInfo.m_VMin) * VMax,
						SpriteInfo.m_UMin + USize * Tesselator.GetNextVertices()[i].m_LeftDist, SpriteInfo.m_VMin + (SpriteInfo.m_VMax - SpriteInfo.m_VMin) * VMax
					);
					CGraphics::CFreeformItem Freeform(P00, P01, P10, P11);
					Graphics()->QuadsDrawFreeform(&Freeform, 1);
				}
			}
			Graphics()->QuadsEnd();
			
			//Draw mesh
			if(DrawMesh)
			{
				Graphics()->TextureClear();
				Graphics()->LinesBegin();
				Graphics()->SetColor(vec4(1.0f, 1.0f, 1.0f, 0.25f), true);
				for(int i=1; i<Tesselator.GetNextVertices().size(); i++)
				{
					vec2 Position0 = Tesselator.GetNextVertices()[i-1].m_Position;			
					vec2 Position1 = Tesselator.GetNextVertices()[i].m_Position;
					vec2 Ortho0 = Tesselator.GetNextVertices()[i-1].m_RightOrtho * SpriteInfo.m_Height/2.0f;
					vec2 Ortho1 = Tesselator.GetNextVertices()[i].m_LeftOrtho * SpriteInfo.m_Height/2.0f;
					
					for(int j=0; j<VerticalTesselation; j++)
					{
						float StepMin = -(2.0f * static_cast<float>(j)/VerticalTesselation - 1.0f);
						float StepMax = -(2.0f * static_cast<float>(j+1)/VerticalTesselation - 1.0f);
						
						vec2 P00 = MapPosToScreenPos(Position0 + Ortho0 * StepMin);
						vec2 P01 = MapPosToScreenPos(Position1 + Ortho1 * StepMin);
						vec2 P10 = MapPosToScreenPos(Position0 + Ortho0 * StepMax);
						vec2 P11 = MapPosToScreenPos(Position1 + Ortho1 * StepMax);
						
						CGraphics::CLineItem aLines[] = {
							CGraphics::CLineItem(P00.x, P00.y, P01.x, P01.y),
							CGraphics::CLineItem(P10.x, P10.y, P11.x, P11.y),
							CGraphics::CLineItem(P00.x, P00.y, P10.x, P10.y),
							CGraphics::CLineItem(P01.x, P01.y, P11.x, P11.y),
							CGraphics::CLineItem(P00.x, P00.y, P11.x, P11.y),
						};
						Graphics()->LinesDraw(aLines, sizeof(aLines)/sizeof(CGraphics::CLineItem));
					}
				}
				Graphics()->LinesEnd();
			}
		}
	}
	else
	{
		Graphics()->TextureClear();
		Graphics()->LinesBegin();
		
		for(int i=1; i<Vertices.size(); i++)
		{
			vec2 Position0 = MapPosToScreenPos(Vertices[i-1].GetPosition());			
			vec2 Position1 = MapPosToScreenPos(Vertices[i].GetPosition());			
			
			CGraphics::CLineItem Line(Position0.x, Position0.y, Position1.x, Position1.y);
			Graphics()->LinesDraw(&Line, 1);
		}
		
		Graphics()->LinesEnd();
	}
}

void CMapRenderer::RenderObjects(CAssetPath LayerPath, vec2 Pos, bool DrawMesh)
{
	const CAsset_MapLayerObjects* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(LayerPath);
	if(!pLayer)
		return;
	
	CAsset_MapLayerObjects::CIteratorObject Iter;
	for(Iter = pLayer->BeginObject(); Iter != pLayer->EndObject(); ++Iter)
	{
		const CAsset_MapLayerObjects::CObject& Object = pLayer->GetObject(*Iter);
		RenderLine(Object.GetVertexArray(), Pos, matrix2x2::identity(), Object.GetStylePath(), Object.GetClosedPath(), DrawMesh);
	}
}

void CMapRenderer::RenderGroup(CAssetPath GroupPath, vec4 Color, bool DrawMesh)
{
	const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(GroupPath);
	if(!pGroup)
		return;
	
	if(!pGroup->GetVisibility())
		return;
	
	if(pGroup->GetClipping())
	{
		UnsetGroup();
		
		vec2 ClipMin = MapPosToScreenPos(pGroup->GetClipPosition());
		vec2 ClipSize = MapPosToScreenPos(pGroup->GetClipPosition() + pGroup->GetClipSize()) - ClipMin;
		Graphics()->ClipPush(ClipMin.x, ClipMin.y, ClipSize.x, ClipSize.y);
	}
	
	SetGroup(GroupPath);
	
	CAsset_MapGroup::CIteratorLayer IterLayer;
	for(IterLayer = pGroup->BeginLayer(); IterLayer != pGroup->EndLayer(); ++IterLayer)
	{
		CAssetPath LayerPath = pGroup->GetLayer(*IterLayer);
			
		if(LayerPath.GetType() == CAsset_MapLayerTiles::TypeId)
		{
			const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(LayerPath);
			if(!pLayer)
				continue;
			
			if(!pLayer->GetVisibility())
				continue;
				
			RenderTiles_Image(pLayer->GetTileArray(), vec2(0.0f, 0.0f), pLayer->GetImagePath(), pLayer->GetColor()*Color, true);
		}
		else if(LayerPath.GetType() == CAsset_MapLayerQuads::TypeId)
		{
			const CAsset_MapLayerQuads* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(LayerPath);
			if(!pLayer)
				continue;
			
			if(!pLayer->GetVisibility())
				continue;
			
			RenderQuads(pLayer->GetQuadPtr(), pLayer->GetQuadArraySize(), vec2(0.0f, 0.0f), pLayer->GetImagePath(), Color);
		}
		else if(LayerPath.GetType() == CAsset_MapLayerObjects::TypeId)
		{
			const CAsset_MapLayerObjects* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(LayerPath);
			if(!pLayer)
				continue;
			
			if(!pLayer->GetVisibility())
				continue;
			
			RenderObjects(LayerPath, vec2(0.0f, 0.0f), DrawMesh);
		}
	}
	
	if(pGroup->GetClipping())
		Graphics()->ClipPop();
}

void CMapRenderer::RenderMap(CAssetPath MapPath, vec4 Color, bool DrawMesh)
{
	const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(MapPath);
	if(!pMap)
		return;
	
	{
		CAsset_Map::CIteratorBgGroup Iter;
		for(Iter = pMap->BeginBgGroup(); Iter != pMap->EndBgGroup(); ++Iter)
		{
			RenderGroup(pMap->GetBgGroup(*Iter), Color, DrawMesh);
		}
	}
	{
		CAsset_Map::CIteratorFgGroup Iter;
		for(Iter = pMap->BeginFgGroup(); Iter != pMap->EndFgGroup(); ++Iter)
		{
			RenderGroup(pMap->GetFgGroup(*Iter), Color, DrawMesh);
		}
	}
		
	UnsetGroup();
}

void CMapRenderer::RenderMap_Zones(CAssetPath MapPath, CAssetPath ZoneTexture, vec4 Color)
{
	const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(MapPath);
	if(!pMap)
		return;
	
	CAsset_Map::CIteratorZoneLayer Iter;
	for(Iter = pMap->BeginZoneLayer(); Iter != pMap->EndZoneLayer(); ++Iter)
	{
		CAssetPath ZonePath = pMap->GetZoneLayer(*Iter);
		const CAsset_MapZoneTiles* pZone = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(ZonePath);
		if(!pZone)
			continue;
	
		if(!pZone->GetVisibility())
			continue;
		
		RenderTiles_Zone(pZone->GetZoneTypePath(), pZone->GetTileArray(), 0.0f, Color, ZoneTexture, true);
	}
}
