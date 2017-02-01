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

#include <shared/components/storage.h>
#include <external/ddnet/datafile.h>
#include <external/ddnet/mapitems.h>
#include <external/ninslash/mapitems.h>
#include <shared/geometry/linetesselation.h>

#include "assetsmanager.h"

/* ASSETS MANAGER *****************************************************/

class CLoadMap_ZoneList
{
public:
	CAssetsManager* m_pAssetsManager;
	CAsset_Map* m_pMap;
	CAssetPath m_MapPath;
	
	CAssetPath m_TeeWorldsPath;
	CAssetPath m_DDGamePath;
	CAssetPath m_DDFrontPath;
	CAssetPath m_DDTelePath;
	CAssetPath m_DDSwitchPath;
	CAssetPath m_OpenFNGPath;
	CAssetPath m_NinslashPath;
	CAssetPath m_SportPath;
	CAssetPath m_UnknownPath;
	
	CAsset_MapZoneTiles* m_pTeeWorldsZone;
	CAsset_MapZoneTiles* m_pDDGameZone;
	CAsset_MapZoneTiles* m_pDDFrontZone;
	CAsset_MapZoneTiles* m_pDDTeleZone;
	CAsset_MapZoneTiles* m_pDDSwitchZone;
	CAsset_MapZoneTiles* m_pOpenFNGZone;
	CAsset_MapZoneTiles* m_pNinslashZone;
	CAsset_MapZoneTiles* m_pSportZone;
	CAsset_MapZoneTiles* m_pUnknownZone;
	
	CLoadMap_ZoneList(CAssetsManager* pAssetsManager, CAsset_Map* pMap, CAssetPath MapPath) :
		m_pAssetsManager(pAssetsManager),
		m_pMap(pMap),
		m_MapPath(MapPath),
		m_pTeeWorldsZone(NULL),
		m_pDDGameZone(NULL),
		m_pDDFrontZone(NULL),
		m_pDDTeleZone(NULL),
		m_pDDSwitchZone(NULL),
		m_pOpenFNGZone(NULL),
		m_pNinslashZone(NULL),
		m_pSportZone(NULL),
		m_pUnknownZone(NULL)
	{
		
	}
	
	void CreateZone(CAsset_MapZoneTiles** ppZone, CAssetPath& ZonePath, const char* pName, CAssetPath ZoneType, int Width, int Height)
	{
		if(*ppZone != NULL)
			return;
		
		const CAsset_ZoneType* pZoneType = m_pAssetsManager->GetAsset<CAsset_ZoneType>(ZoneType);
		if(!pZoneType)
			return;
		
		*ppZone = m_pAssetsManager->NewAsset_Hard<CAsset_MapZoneTiles>(&ZonePath, m_MapPath.GetPackageId());
		
		m_pAssetsManager->TryChangeAssetName_Hard(ZonePath, pName);
		(*ppZone)->SetParentPath(m_MapPath);
		(*ppZone)->SetZoneTypePath(ZoneType);

		array2d<CAsset_MapZoneTiles::CTile>& Data = (*ppZone)->GetTileArray();
		Data.resize(Width, Height);
		
		if(pZoneType->GetDataIntArraySize() > 0)
		{
			array2d<int>& DataInt = (*ppZone)->GetDataIntArray();
			DataInt.resize(Width, Height, pZoneType->GetDataIntArraySize());
		}
		
		CSubPath ZoneLayer = CAsset_Map::SubPath_ZoneLayer(m_pMap->AddZoneLayer());
		m_pMap->SetZoneLayer(ZoneLayer, ZonePath);
		
		if(m_pTeeWorldsZone && &m_pTeeWorldsZone != ppZone)
			m_pTeeWorldsZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_TeeWorldsPath);
		if(m_pDDGameZone && &m_pDDGameZone != ppZone)
			m_pDDGameZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_DDGamePath);
		if(m_pDDFrontZone && &m_pDDFrontZone != ppZone)
			m_pDDFrontZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_DDFrontPath);
		if(m_pDDTeleZone && &m_pDDTeleZone != ppZone)
			m_pDDTeleZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_DDTelePath);
		if(m_pDDSwitchZone && &m_pDDSwitchZone != ppZone)
			m_pDDSwitchZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_DDSwitchPath);
		if(m_pOpenFNGZone && &m_pOpenFNGZone != ppZone)
			m_pOpenFNGZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_OpenFNGPath);
		if(m_pNinslashZone && &m_pNinslashZone != ppZone)
			m_pNinslashZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_NinslashPath);
		if(m_pSportZone && &m_pSportZone != ppZone)
			m_pSportZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_SportPath);
		if(m_pUnknownZone && &m_pUnknownZone != ppZone)
			m_pUnknownZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_UnknownPath);
	}
};

int CAssetsManager::Load_Map(const char* pFileName, int StorageType, int Format, unsigned Crc)
{
	dynamic_string PackageName;
	dynamic_string FullPath;
	
	if(StorageType == CStorage::TYPE_ABSOLUTE)
	{
		FullPath = pFileName;
		
		int Length = str_length(pFileName);
		int i=Length-1;
				
		for(; i>=0; i--)
			if(pFileName[i] != '/' && pFileName[i] != '\\')
				break;
		for(; i>=0; i--)
			if(pFileName[i] == '/' || pFileName[i] == '\\')
				break;
		
		if(Length >= 4 && str_comp(pFileName+Length-4, ".map") == 0)
		{
			PackageName.clear();
			PackageName.append_at_num(0, pFileName+i+1, Length-i-5);
		}
		else
			PackageName = pFileName+i;
	}
	else
	{
		FullPath.append("maps/");
		FullPath.append(pFileName);
		FullPath.append(".map");
		
		PackageName = pFileName;
	}
	
	int PackageId = NewPackage(PackageName.buffer());
	
	ddnet::CDataFileReader ArchiveFile;
	if(!ArchiveFile.Open(Storage(), FullPath.buffer(), StorageType))
	{
		dbg_msg("AssetsManager", "can't open the file %s (storage type:%d)", FullPath.buffer(), StorageType);
		return -1;
	}
	
	if(Crc != 0 && ArchiveFile.Crc() != Crc)
	{
		dbg_msg("AssetsManager", "wrong crc for the file %s (%d != %d)", FullPath.buffer(), Crc, ArchiveFile.Crc());
		return -1;
	}
	
	ddnet::CMapItemVersion *pItem = (ddnet::CMapItemVersion *) ArchiveFile.FindItem(ddnet::MAPITEMTYPE_VERSION, 0);
	if(!pItem)
		return -1;

	if(pItem->m_Version != 1)
		return -1;

	if(Format == MAPFORMAT_NINSLASH)
		Load_UnivNinslash();
	else
		Load_UnivTeeWorlds();
	
	char aBuf[128];

	CAssetPath MapPath;
	CAsset_Map* pMap = NewAsset_Hard<CAsset_Map>(&MapPath, PackageId);
	AssetsManager()->TryChangeAssetName_Hard(MapPath, "map");
	
	//Create the TeeWorlds layer
	CLoadMap_ZoneList Zones(this, pMap, MapPath);
	
	CAssetPath* pImagePath;
	//Load images
	{
		int Start, Num;
		ArchiveFile.GetType(ddnet::MAPITEMTYPE_IMAGE, &Start, &Num);
		
		pImagePath = new CAssetPath[Num];

		for(int i = 0; i < Num; i++)
		{
			ddnet::CMapItemImage *pItem = (ddnet::CMapItemImage *) ArchiveFile.GetItem(Start+i, 0, 0);
			
			//Image data
			if(pItem->m_External)
			{
				char* pFilename = (char*) ArchiveFile.GetData(pItem->m_ImageName);
				
				if(str_comp(pFilename, "bg_cloud1") == 0)
				{
					Load_EnvClouds();
					pImagePath[i] = m_Path_Image_Cloud2;
				}
				else if(str_comp(pFilename, "bg_cloud2") == 0)
				{
					Load_EnvClouds();
					pImagePath[i] = m_Path_Image_Cloud3;
				}
				else if(str_comp(pFilename, "bg_cloud3") == 0)
				{
					Load_EnvClouds();
					pImagePath[i] = m_Path_Image_Cloud1;
				}
				else if(str_comp(pFilename, "desert_doodads") == 0)
				{
					Load_EnvDesert();
					pImagePath[i] = m_Path_Image_DesertDoodads;
				}
				else if(str_comp(pFilename, "desert_main") == 0)
				{
					Load_EnvDesert();
					pImagePath[i] = m_Path_Image_DesertMain;
				}
				else if(str_comp(pFilename, "desert_mountains") == 0)
				{
					Load_EnvDesert();
					pImagePath[i] = m_Path_Image_DesertMountains1;
				}
				else if(str_comp(pFilename, "desert_mountains2") == 0)
				{
					Load_EnvDesert();
					pImagePath[i] = m_Path_Image_DesertMountains2;
				}
				else if(str_comp(pFilename, "desert_sun") == 0)
				{
					Load_EnvDesert();
					pImagePath[i] = m_Path_Image_DesertSun;
				}
				else if(str_comp(pFilename, "generic_deathtiles") == 0)
				{
					Load_EnvGeneric();
					pImagePath[i] = m_Path_Image_GenericSpikes;
				}
				else if(str_comp(pFilename, "generic_unhookable") == 0)
				{
					Load_EnvGeneric();
					pImagePath[i] = m_Path_Image_GenericUnhookable;
				}
				else if(str_comp(pFilename, "grass_main") == 0)
				{
					Load_EnvGrass();
					pImagePath[i] = m_Path_Image_GrassMain;
				}
				else if(str_comp(pFilename, "grass_doodads") == 0)
				{
					Load_EnvGrass();
					pImagePath[i] = m_Path_Image_GrassDoodads;
				}
				else if(str_comp(pFilename, "jungle_background") == 0)
				{
					Load_EnvJungle();
					pImagePath[i] = m_Path_Image_JungleBackground;
				}
				else if(str_comp(pFilename, "jungle_deathtiles") == 0)
				{
					Load_EnvJungle();
					pImagePath[i] = m_Path_Image_JungleSpikes;
				}
				else if(str_comp(pFilename, "jungle_doodads") == 0)
				{
					Load_EnvJungle();
					pImagePath[i] = m_Path_Image_JungleDoodads;
				}
				else if(str_comp(pFilename, "jungle_main") == 0)
				{
					Load_EnvJungle();
					pImagePath[i] = m_Path_Image_JungleMain;
				}
				else if(str_comp(pFilename, "jungle_midground") == 0)
				{
					Load_EnvJungle();
					pImagePath[i] = m_Path_Image_JungleMidground;
				}
				else if(str_comp(pFilename, "jungle_unhookables") == 0)
				{
					Load_EnvJungle();
					pImagePath[i] = m_Path_Image_JungleUnhookable;
				}
				else if(str_comp(pFilename, "moon") == 0)
				{
					Load_EnvMoon();
					pImagePath[i] = m_Path_Image_Moon;
				}
				else if(str_comp(pFilename, "mountains") == 0)
				{
					Load_EnvMountains();
					pImagePath[i] = m_Path_Image_Mountains;
				}
				else if(str_comp(pFilename, "snow") == 0)
				{
					Load_EnvSnow();
					pImagePath[i] = m_Path_Image_Snow;
				}
				else if(str_comp(pFilename, "stars") == 0)
				{
					Load_EnvStars();
					pImagePath[i] = m_Path_Image_Stars;
				}
				else if(str_comp(pFilename, "sun") == 0)
				{
					Load_EnvSun();
					pImagePath[i] = m_Path_Image_Sun;
				}
				else if(str_comp(pFilename, "winter_doodads") == 0)
				{
					Load_EnvWinter();
					pImagePath[i] = m_Path_Image_WinterDoodads;
				}
				else if(str_comp(pFilename, "winter_main") == 0)
				{
					Load_EnvWinter();
					pImagePath[i] = m_Path_Image_WinterMain;
				}
				else if(str_comp(pFilename, "winter_mountains") == 0)
				{
					Load_EnvWinter();
					pImagePath[i] = m_Path_Image_WinterMountains1;
				}
				else if(str_comp(pFilename, "winter_mountains2") == 0)
				{
					Load_EnvWinter();
					pImagePath[i] = m_Path_Image_WinterMountains2;
				}
				else if(str_comp(pFilename, "winter_mountains3") == 0)
				{
					Load_EnvWinter();
					pImagePath[i] = m_Path_Image_WinterMountains3;
				}
				else if(Format == MAPFORMAT_NINSLASH && str_comp(pFilename, "lab") == 0)
				{
					Load_EnvLab();
					pImagePath[i] = m_Path_Image_LabMisc;
				}
				else if(Format == MAPFORMAT_NINSLASH && str_comp(pFilename, "lab_main") == 0)
				{
					Load_EnvLab();
					pImagePath[i] = m_Path_Image_LabMain;
				}
				else if(Format == MAPFORMAT_NINSLASH && str_comp(pFilename, "lab_background") == 0)
				{
					Load_EnvLab();
					pImagePath[i] = m_Path_Image_LabBackground;
				}
				else if(Format == MAPFORMAT_NINSLASH && str_comp(pFilename, "factory_main") == 0)
				{
					Load_EnvFactory();
					pImagePath[i] = m_Path_Image_FactoryMain;
				}
				else if(Format == MAPFORMAT_NINSLASH && str_comp(pFilename, "factory_background") == 0)
				{
					Load_EnvFactory();
					pImagePath[i] = m_Path_Image_FactoryBackground;
				}
				
				ArchiveFile.UnloadData(pItem->m_ImageName);
			}
			else
			{
				CAsset_Image* pImage = NewAsset_Hard<CAsset_Image>(pImagePath+i, PackageId);
				
				//Image name
				{
					char *pName = (char *)ArchiveFile.GetData(pItem->m_ImageName);
					AssetsManager()->TryChangeAssetName_Hard(pImagePath[i], pName);
					ArchiveFile.UnloadData(pItem->m_ImageName);
				}
				
				unsigned char* pData = (unsigned char*) ArchiveFile.GetData(pItem->m_ImageData);				
				array2d<uint8>& Data = pImage->GetDataArray();
				
				Data.resize(pItem->m_Width, pItem->m_Height, 4);
				
				for(int j=0; j<pItem->m_Height; j++)
				{
					for(int i=0; i<pItem->m_Width; i++)
					{
						Data.set_clamp(i, j, 0, pData[(j*pItem->m_Width+i)*4+0]);
						Data.set_clamp(i, j, 1, pData[(j*pItem->m_Width+i)*4+1]);
						Data.set_clamp(i, j, 2, pData[(j*pItem->m_Width+i)*4+2]);
						Data.set_clamp(i, j, 3, pData[(j*pItem->m_Width+i)*4+3]);
					}
				}
				
				ArchiveFile.UnloadData(pItem->m_ImageData);	
			}
		}
	}
	
	bool Background = true;
	
	//Load groups
	{
		int LayersStart, LayersNum;
		ArchiveFile.GetType(ddnet::MAPITEMTYPE_LAYER, &LayersStart, &LayersNum);

		int Start, Num;
		ArchiveFile.GetType(ddnet::MAPITEMTYPE_GROUP, &Start, &Num);
		for(int g = 0; g < Num; g++)
		{
			ddnet::CMapItemGroup *pGItem = (ddnet::CMapItemGroup *)ArchiveFile.GetItem(Start+g, 0, 0);

			if(pGItem->m_Version < 1 || pGItem->m_Version > ddnet::CMapItemGroup::CURRENT_VERSION)
				continue;

			// load group name
			aBuf[0] = 0;

			if(pGItem->m_Version >= 3)
				IntsToStr(pGItem->m_aName, sizeof(pGItem->m_aName)/sizeof(int), aBuf);

			if(!aBuf[0])
				str_copy(aBuf, "group", sizeof(aBuf));
			
			if(str_comp("#Entities", aBuf) == 0)
			{
				for(int l = 0; l < pGItem->m_NumLayers; l++)
				{
					ddnet::CMapItemLayer *pLayerItem = (ddnet::CMapItemLayer *) ArchiveFile.GetItem(LayersStart+pGItem->m_StartLayer+l, 0, 0);
					if(!pLayerItem)
						continue;
					
					if(pLayerItem->m_Type != ddnet::LAYERTYPE_QUADS)
						continue;
					
					ddnet::CMapItemLayerQuads *pQuadsItem = (ddnet::CMapItemLayerQuads *)pLayerItem;
					
					//Name
					aBuf[0] = 0;

					if(pQuadsItem->m_Version >= 2)
						IntsToStr(pQuadsItem->m_aName, sizeof(pQuadsItem->m_aName)/sizeof(int), aBuf);

					//Search the name in all assets
					CAssetPath EntityTypePath;
					for(int p=0; p<GetNumPackages(); p++)
					{
						if(!IsValidPackage(p))
							continue;
						
						EntityTypePath = FindAsset<CAsset_EntityType>(p, aBuf);
						if(EntityTypePath.IsNotNull())
							break;
					}
					
					if(EntityTypePath.IsNull())
						continue;
						
					CAssetPath EntitiesPath;
					CAsset_MapEntities* pEntities = NewAsset_Hard<CAsset_MapEntities>(&EntitiesPath, PackageId);
					AssetsManager()->TryChangeAssetName_Hard(EntitiesPath, aBuf);
					CSubPath MapSubPath = CAsset_Map::SubPath_EntityLayer(pMap->AddEntityLayer());
					pMap->SetEntityLayer(MapSubPath, EntitiesPath);
					pEntities->SetParentPath(MapPath);
					
					//Quads
					ddnet::CQuad *pQuads = (ddnet::CQuad *) ArchiveFile.GetDataSwapped(pQuadsItem->m_Data);
					for(int i=0; i<pQuadsItem->m_NumQuads; i++)
					{
						vec2 P0(fx2f(pQuads[i].m_aPoints[0].x), fx2f(pQuads[i].m_aPoints[0].y));
						vec2 P1(fx2f(pQuads[i].m_aPoints[1].x), fx2f(pQuads[i].m_aPoints[1].y));
						vec2 P2(fx2f(pQuads[i].m_aPoints[2].x), fx2f(pQuads[i].m_aPoints[2].y));
						vec2 P3(fx2f(pQuads[i].m_aPoints[3].x), fx2f(pQuads[i].m_aPoints[3].y));
						vec2 Pos = (P0+P1+P2+P3)/4.0f;
						CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
						pEntities->SetEntityTypePath(EntityPath, EntityTypePath);
						pEntities->SetEntityPosition(EntityPath, Pos);
					}
				}
			}
			else if(str_comp("#Zones", aBuf) == 0)
			{
				for(int l = 0; l < pGItem->m_NumLayers; l++)
				{
					ddnet::CMapItemLayer *pLayerItem = (ddnet::CMapItemLayer *) ArchiveFile.GetItem(LayersStart+pGItem->m_StartLayer+l, 0, 0);
					if(!pLayerItem)
						continue;
					
					if(pLayerItem->m_Type == ddnet::LAYERTYPE_TILES)
					{
						ddnet::CMapItemLayerTilemap *pTilemapItem = (ddnet::CMapItemLayerTilemap *)pLayerItem;
							
						//Name
						aBuf[0] = 0;

						if(pTilemapItem->m_Version >= 3)
							IntsToStr(pTilemapItem->m_aName, sizeof(pTilemapItem->m_aName)/sizeof(int), aBuf);

						//Search the name in all assets
						CAssetPath ZoneTypePath;
						for(int p=0; p<GetNumPackages(); p++)
						{
							if(!IsValidPackage(p))
								continue;
							
							ZoneTypePath = FindAsset<CAsset_ZoneType>(p, aBuf);
							if(ZoneTypePath.IsNotNull())
								break;
						}
						
						CAssetPath ZonePath;
						CAsset_MapZoneTiles* pZone = NewAsset_Hard<CAsset_MapZoneTiles>(&ZonePath, PackageId);
						AssetsManager()->TryChangeAssetName_Hard(ZonePath, aBuf);
						CSubPath MapSubPath = CAsset_Map::SubPath_ZoneLayer(pMap->AddZoneLayer());
						pMap->SetZoneLayer(MapSubPath, ZonePath);
						pZone->SetParentPath(MapPath);
						pZone->SetZoneTypePath(ZoneTypePath);
						
						//Tiles
						ddnet::CTile* pTiles = (ddnet::CTile*) ArchiveFile.GetData(pTilemapItem->m_Data);
						int Width = pTilemapItem->m_Width;
						int Height = pTilemapItem->m_Height;
						
						array2d<CAsset_MapZoneTiles::CTile>& Data = pZone->GetTileArray();
						Data.resize(Width, Height);
						
						for(int j=0; j<Height; j++)
						{
							for(int i=0; i<Width; i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								pZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
							}
						}						
					}
					else if(pLayerItem->m_Type == ddnet::LAYERTYPE_QUADS)
					{
						ddnet::CMapItemLayerQuads *pQuadsItem = (ddnet::CMapItemLayerQuads *)pLayerItem;

						//Name
						aBuf[0] = 0;

						if(pQuadsItem->m_Version >= 2)
							IntsToStr(pQuadsItem->m_aName, sizeof(pQuadsItem->m_aName)/sizeof(int), aBuf);
						
						//Search the name in all assets
						CAssetPath ZoneTypePath;
						for(int p=0; p<GetNumPackages(); p++)
						{
							if(!IsValidPackage(p))
								continue;
							
							ZoneTypePath = FindAsset<CAsset_ZoneType>(p, aBuf);
							if(ZoneTypePath.IsNotNull())
								break;
						}
						
						CAssetPath ZonePath;
						CAsset_MapZoneObjects* pZone = NewAsset_Hard<CAsset_MapZoneObjects>(&ZonePath, PackageId);
						AssetsManager()->TryChangeAssetName_Hard(ZonePath, aBuf);
						CSubPath MapSubPath = CAsset_Map::SubPath_ZoneLayer(pMap->AddZoneLayer());
						pMap->SetZoneLayer(MapSubPath, ZonePath);
						pZone->SetParentPath(MapPath);
						pZone->SetZoneTypePath(ZoneTypePath);
						
						//Quads
						ddnet::CQuad *pQuads = (ddnet::CQuad *) ArchiveFile.GetDataSwapped(pQuadsItem->m_Data);
						for(int i=0; i<pQuadsItem->m_NumQuads; i++)
						{
							CSubPath ObjectPath = CAsset_MapZoneObjects::SubPath_Object(pZone->AddObject());
							
							vec2 Pivot;
							Pivot.x = fx2f(pQuads[i].m_aPoints[4].x);
							Pivot.y = fx2f(pQuads[i].m_aPoints[4].y);
							
							pZone->SetObjectPosition(ObjectPath, Pivot);
							pZone->SetObjectPathType(ObjectPath, CAsset_MapZoneObjects::PATHTYPE_CLOSED);
							pZone->SetObjectFillType(ObjectPath, CAsset_MapZoneObjects::FILLTYPE_INSIDE);
							pZone->SetObjectZoneIndex(ObjectPath, pQuads[i].m_ColorEnvOffset);
							
							{
								CSubPath VertexPath = CAsset_MapZoneObjects::SubPath_ObjectVertex(ObjectPath.GetId(), pZone->AddObjectVertex(ObjectPath));
								pZone->SetObjectVertexPosition(VertexPath, vec2(fx2f(pQuads[i].m_aPoints[0].x), fx2f(pQuads[i].m_aPoints[0].y)) - Pivot);
								pZone->SetObjectVertexSmoothness(VertexPath, CBezierVertex::TYPE_CORNER);
							}
							{
								CSubPath VertexPath = CAsset_MapZoneObjects::SubPath_ObjectVertex(ObjectPath.GetId(), pZone->AddObjectVertex(ObjectPath));
								pZone->SetObjectVertexPosition(VertexPath, vec2(fx2f(pQuads[i].m_aPoints[1].x), fx2f(pQuads[i].m_aPoints[1].y)) - Pivot);
								pZone->SetObjectVertexSmoothness(VertexPath, CBezierVertex::TYPE_CORNER);
							}
							{
								CSubPath VertexPath = CAsset_MapZoneObjects::SubPath_ObjectVertex(ObjectPath.GetId(), pZone->AddObjectVertex(ObjectPath));
								pZone->SetObjectVertexPosition(VertexPath, vec2(fx2f(pQuads[i].m_aPoints[3].x), fx2f(pQuads[i].m_aPoints[3].y)) - Pivot);
								pZone->SetObjectVertexSmoothness(VertexPath, CBezierVertex::TYPE_CORNER);
							}
							{
								CSubPath VertexPath = CAsset_MapZoneObjects::SubPath_ObjectVertex(ObjectPath.GetId(), pZone->AddObjectVertex(ObjectPath));
								pZone->SetObjectVertexPosition(VertexPath, vec2(fx2f(pQuads[i].m_aPoints[2].x), fx2f(pQuads[i].m_aPoints[2].y)) - Pivot);
								pZone->SetObjectVertexSmoothness(VertexPath, CBezierVertex::TYPE_CORNER);
							}
						}
					}
				}					
			}
			else
			{
				CAssetPath MapGroupPath;
				CAsset_MapGroup* pMapGroup = NewAsset_Hard<CAsset_MapGroup>(&MapGroupPath, PackageId);

				if(Background)
				{
					CSubPath MapSubPath = CAsset_Map::SubPath_BgGroup(pMap->AddBgGroup());
					pMap->SetBgGroup(MapSubPath, MapGroupPath);
				}
				else
				{
					CSubPath MapSubPath = CAsset_Map::SubPath_FgGroup(pMap->AddFgGroup());
					pMap->SetFgGroup(MapSubPath, MapGroupPath);
				}
				pMapGroup->SetParentPath(MapPath);
				
				//Don't forget to add parameters when the game group is splitted
				pMapGroup->SetPosition(vec2(pGItem->m_OffsetX, pGItem->m_OffsetY));
				pMapGroup->SetHardParallax(vec2(pGItem->m_ParallaxX/100.0f, pGItem->m_ParallaxY/100.0f));

				if(pGItem->m_Version >= 2)
				{
					pMapGroup->SetClipping(pGItem->m_UseClipping);
					pMapGroup->SetClipPosition(vec2(pGItem->m_ClipX, pGItem->m_ClipY));
					pMapGroup->SetClipSize(vec2(pGItem->m_ClipW, pGItem->m_ClipH));
				}
				
				AssetsManager()->TryChangeAssetName_Hard(MapGroupPath, aBuf);

				for(int l = 0; l < pGItem->m_NumLayers; l++)
				{
					ddnet::CMapItemLayer *pLayerItem = (ddnet::CMapItemLayer *) ArchiveFile.GetItem(LayersStart+pGItem->m_StartLayer+l, 0, 0);
					if(!pLayerItem)
						continue;

					if(pLayerItem->m_Type == ddnet::LAYERTYPE_TILES)
					{
						ddnet::CMapItemLayerTilemap *pTilemapItem = (ddnet::CMapItemLayerTilemap *)pLayerItem;

						if(pTilemapItem->m_Flags == ddnet::TILESLAYERFLAG_GAME)
						{
							Background = false;
								
							//Tiles
							ddnet::CTile* pTiles = (ddnet::CTile*) ArchiveFile.GetData(pTilemapItem->m_Data);
							int Width = pTilemapItem->m_Width;
							int Height = pTilemapItem->m_Height;
							
							pMap->SetCameraPosition(vec2(Width*32.0f, Height*32.0f)/2.0f);
							
							CAssetPath EntitiesSpawnPath;
							NewAsset_Hard<CAsset_MapEntities>(&EntitiesSpawnPath, PackageId);
							AssetsManager()->TryChangeAssetName_Hard(EntitiesSpawnPath, "spawn");
							
							CAssetPath EntitiesWeaponPath;
							NewAsset_Hard<CAsset_MapEntities>(&EntitiesWeaponPath, PackageId);
							AssetsManager()->TryChangeAssetName_Hard(EntitiesWeaponPath, "weapons");
							
							CAssetPath EntitiesPickupPath;
							NewAsset_Hard<CAsset_MapEntities>(&EntitiesPickupPath, PackageId);
							AssetsManager()->TryChangeAssetName_Hard(EntitiesPickupPath, "pickups");
							
							CAsset_MapEntities* pEntitiesSpawn = GetAsset_Hard<CAsset_MapEntities>(EntitiesSpawnPath);
							CAsset_MapEntities* pEntitiesWeapon = GetAsset_Hard<CAsset_MapEntities>(EntitiesWeaponPath);
							CAsset_MapEntities* pEntitiesPickup = GetAsset_Hard<CAsset_MapEntities>(EntitiesPickupPath);
							
							pEntitiesSpawn->SetParentPath(MapPath);
							pEntitiesWeapon->SetParentPath(MapPath);
							pEntitiesPickup->SetParentPath(MapPath);
					
							CSubPath MapSubPath;
							MapSubPath = CAsset_Map::SubPath_EntityLayer(pMap->AddEntityLayer());
							pMap->SetEntityLayer(MapSubPath, EntitiesSpawnPath);	
							MapSubPath = CAsset_Map::SubPath_EntityLayer(pMap->AddEntityLayer());		
							pMap->SetEntityLayer(MapSubPath, EntitiesWeaponPath);	
							MapSubPath = CAsset_Map::SubPath_EntityLayer(pMap->AddEntityLayer());		
							pMap->SetEntityLayer(MapSubPath, EntitiesPickupPath);			
							
							for(int j=0; j<Height; j++)
							{
								for(int i=0; i<Width; i++)
								{
									CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
									
									bool WriteInUnknown = false;
									
									if(Format == MAPFORMAT_NINSLASH)
									{
										switch(pTiles[j*Width+i].m_Index)
										{
											case ninslash::ENTITY_SPAWN + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesSpawn->AddEntity());
												pEntitiesSpawn->SetEntityTypePath(EntityPath, m_Path_EntityType_NSSpawn);
												pEntitiesSpawn->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ninslash::ENTITY_SPAWN_RED + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesSpawn->AddEntity());
												pEntitiesSpawn->SetEntityTypePath(EntityPath, m_Path_EntityType_NSSpawnRed);
												pEntitiesSpawn->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ninslash::ENTITY_SPAWN_BLUE + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesSpawn->AddEntity());
												pEntitiesSpawn->SetEntityTypePath(EntityPath, m_Path_EntityType_NSSpawnBlue);
												pEntitiesSpawn->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ninslash::ENTITY_FLAGSTAND_RED + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesPickup->AddEntity());
												pEntitiesPickup->SetEntityTypePath(EntityPath, m_Path_EntityType_NSFlagRed);
												pEntitiesPickup->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ninslash::ENTITY_FLAGSTAND_BLUE + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesPickup->AddEntity());
												pEntitiesPickup->SetEntityTypePath(EntityPath, m_Path_EntityType_NSFlagBlue);
												pEntitiesPickup->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ninslash::ENTITY_HEALTH_1 + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesPickup->AddEntity());
												pEntitiesPickup->SetEntityTypePath(EntityPath, m_Path_EntityType_NSHeart);
												pEntitiesPickup->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ninslash::ENTITY_WEAPON_CHAINSAW + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesWeapon->AddEntity());
												pEntitiesWeapon->SetEntityTypePath(EntityPath, m_Path_EntityType_NSChainsaw);
												pEntitiesWeapon->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ninslash::ENTITY_WEAPON_SHOTGUN + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesWeapon->AddEntity());
												pEntitiesWeapon->SetEntityTypePath(EntityPath, m_Path_EntityType_NSShotgun);
												pEntitiesWeapon->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ninslash::ENTITY_WEAPON_GRENADE + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesWeapon->AddEntity());
												pEntitiesWeapon->SetEntityTypePath(EntityPath, m_Path_EntityType_NSGrenade);
												pEntitiesWeapon->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ninslash::ENTITY_WEAPON_LASER + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesWeapon->AddEntity());
												pEntitiesWeapon->SetEntityTypePath(EntityPath, m_Path_EntityType_NSLaser);
												pEntitiesWeapon->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ninslash::ENTITY_WEAPON_RIFLE + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesWeapon->AddEntity());
												pEntitiesWeapon->SetEntityTypePath(EntityPath, m_Path_EntityType_NSRifle);
												pEntitiesWeapon->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ninslash::ENTITY_WEAPON_ELECTRIC + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesWeapon->AddEntity());
												pEntitiesWeapon->SetEntityTypePath(EntityPath, m_Path_EntityType_NSElectric);
												pEntitiesWeapon->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ninslash::ENTITY_WEAPON_FLAMER + ninslash::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesWeapon->AddEntity());
												pEntitiesWeapon->SetEntityTypePath(EntityPath, m_Path_EntityType_NSFlamer);
												pEntitiesWeapon->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											default:
											{
												Zones.CreateZone(&Zones.m_pNinslashZone, Zones.m_NinslashPath, "ninslash", m_Path_ZoneType_Ninslash, Width, Height);
												if(Zones.m_pNinslashZone)
													Zones.m_pNinslashZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
												else
													WriteInUnknown = true;
											}
										}
									}
									else
									{
										switch(pTiles[j*Width+i].m_Index)
										{
											case ddnet::ENTITY_SPAWN + ddnet::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesSpawn->AddEntity());
												pEntitiesSpawn->SetEntityTypePath(EntityPath, m_Path_EntityType_TWSpawn);
												pEntitiesSpawn->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ddnet::ENTITY_SPAWN_RED + ddnet::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesSpawn->AddEntity());
												pEntitiesSpawn->SetEntityTypePath(EntityPath, m_Path_EntityType_TWSpawnRed);
												pEntitiesSpawn->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ddnet::ENTITY_SPAWN_BLUE + ddnet::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesSpawn->AddEntity());
												pEntitiesSpawn->SetEntityTypePath(EntityPath, m_Path_EntityType_TWSpawnBlue);
												pEntitiesSpawn->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ddnet::ENTITY_HEALTH_1 + ddnet::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesPickup->AddEntity());
												pEntitiesPickup->SetEntityTypePath(EntityPath, m_Path_EntityType_TWHeart);
												pEntitiesPickup->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ddnet::ENTITY_ARMOR_1 + ddnet::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesPickup->AddEntity());
												pEntitiesPickup->SetEntityTypePath(EntityPath, m_Path_EntityType_TWArmor);
												pEntitiesPickup->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ddnet::ENTITY_POWERUP_NINJA + ddnet::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesWeapon->AddEntity());
												pEntitiesWeapon->SetEntityTypePath(EntityPath, m_Path_EntityType_TWNinja);
												pEntitiesWeapon->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ddnet::ENTITY_WEAPON_GRENADE + ddnet::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesWeapon->AddEntity());
												pEntitiesWeapon->SetEntityTypePath(EntityPath, m_Path_EntityType_TWGrenade);
												pEntitiesWeapon->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ddnet::ENTITY_WEAPON_RIFLE + ddnet::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesWeapon->AddEntity());
												pEntitiesWeapon->SetEntityTypePath(EntityPath, m_Path_EntityType_TWLaserRifle);
												pEntitiesWeapon->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ddnet::ENTITY_WEAPON_SHOTGUN + ddnet::ENTITY_OFFSET:
											{
												CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesWeapon->AddEntity());
												pEntitiesWeapon->SetEntityTypePath(EntityPath, m_Path_EntityType_TWShotgun);
												pEntitiesWeapon->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												break;
											}
											case ddnet::ENTITY_FLAGSTAND_BLUE + ddnet::ENTITY_OFFSET:
											{
												if(Format == MAPFORMAT_DUMMYCAPTURE)
												{
													Load_UnivSport();
													Zones.CreateZone(&Zones.m_pSportZone, Zones.m_SportPath, "sport", m_Path_ZoneType_Sport, Width, Height);
													if(Zones.m_pSportZone)
														Zones.m_pSportZone->SetTileIndex(TilePath, 3);
													else
														WriteInUnknown = true;
												}
												else
												{
													CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesPickup->AddEntity());
													pEntitiesPickup->SetEntityTypePath(EntityPath, m_Path_EntityType_TWFlagBlue);
													pEntitiesPickup->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												}
												break;
											}
											case ddnet::ENTITY_FLAGSTAND_RED + ddnet::ENTITY_OFFSET:
											{
												if(Format == MAPFORMAT_DUMMYCAPTURE)
												{
													Load_UnivSport();
													Zones.CreateZone(&Zones.m_pSportZone, Zones.m_SportPath, "sport", m_Path_ZoneType_Sport, Width, Height);
													if(Zones.m_pSportZone)
														Zones.m_pSportZone->SetTileIndex(TilePath, 2);
													else
														WriteInUnknown = true;
												}
												else
												{
													CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntitiesPickup->AddEntity());
													pEntitiesPickup->SetEntityTypePath(EntityPath, m_Path_EntityType_TWFlagRed);
													pEntitiesPickup->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
												}
												break;
											}
											case ddnet::TILE_AIR:
											case ddnet::TILE_SOLID:
											case ddnet::TILE_DEATH:
											case ddnet::TILE_NOHOOK:
											{
												Zones.CreateZone(&Zones.m_pTeeWorldsZone, Zones.m_TeeWorldsPath, "teeworlds", m_Path_ZoneType_TeeWorlds, Width, Height);
												if(Zones.m_pTeeWorldsZone)
													Zones.m_pTeeWorldsZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
												else
													WriteInUnknown = true;
												break;
											}
											//OpenFNG and DDNet
											case 7:
											case 8:
											{
												if(Format == MAPFORMAT_OPENFNG)
												{
													Load_UnivOpenFNG();
													Zones.CreateZone(&Zones.m_pOpenFNGZone, Zones.m_OpenFNGPath, "openfng", m_Path_ZoneType_OpenFNG, Width, Height);
													if(Zones.m_pOpenFNGZone)
														Zones.m_pOpenFNGZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
													else
														WriteInUnknown = true;
												}
												else
													WriteInUnknown = true;
												break;
											}
											case ddnet::TILE_FREEZE:
											{
												if(Format == MAPFORMAT_DDNET)
												{
													Load_UnivDDNet();
													Zones.CreateZone(&Zones.m_pDDGameZone, Zones.m_DDGamePath, "ddnetGame", m_Path_ZoneType_DDGame, Width, Height);
													if(Zones.m_pDDGameZone)
														Zones.m_pDDGameZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
													else
														WriteInUnknown = true;
												}
												else if(Format == MAPFORMAT_OPENFNG)
												{
													Load_UnivOpenFNG();
													Zones.CreateZone(&Zones.m_pOpenFNGZone, Zones.m_OpenFNGPath, "openfng", m_Path_ZoneType_OpenFNG, Width, Height);
													if(Zones.m_pOpenFNGZone)
														Zones.m_pOpenFNGZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
													else
														WriteInUnknown = true;
												}
												else
													WriteInUnknown = true;
												break;
											}
											case 10:
											{
												if(Format == MAPFORMAT_OPENFNG)
												{
													Load_UnivOpenFNG();
													Zones.CreateZone(&Zones.m_pOpenFNGZone, Zones.m_OpenFNGPath, "openfng", m_Path_ZoneType_OpenFNG, Width, Height);
													if(Zones.m_pOpenFNGZone)
														Zones.m_pOpenFNGZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
													else
														WriteInUnknown = true;
												}
												else
													WriteInUnknown = true;
												break;
											}
											case ddnet::TILE_UNFREEZE:
											case ddnet::TILE_DFREEZE:
											{
												if(Format == MAPFORMAT_DDNET)
												{
													Load_UnivDDNet();
													Zones.CreateZone(&Zones.m_pDDGameZone, Zones.m_DDGamePath, "ddnetGame", m_Path_ZoneType_DDGame, Width, Height);
													if(Zones.m_pDDGameZone)
														Zones.m_pDDGameZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
													else
														WriteInUnknown = true;
												}
												else if(Format == MAPFORMAT_OPENFNG)
												{
													Load_UnivOpenFNG();
													Zones.CreateZone(&Zones.m_pOpenFNGZone, Zones.m_OpenFNGPath, "openfng", m_Path_ZoneType_OpenFNG, Width, Height);
													if(Zones.m_pOpenFNGZone)
														Zones.m_pOpenFNGZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
													else
														WriteInUnknown = true;
												}
												else
													WriteInUnknown = true;
												break;
											}
											case ddnet::TILE_DUNFREEZE:
											{
												if(Format == MAPFORMAT_DDNET)
												{
													Load_UnivDDNet();
													Zones.CreateZone(&Zones.m_pDDGameZone, Zones.m_DDGamePath, "ddnetGame", m_Path_ZoneType_DDGame, Width, Height);
													if(Zones.m_pDDGameZone)
														Zones.m_pDDGameZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
													else
														WriteInUnknown = true;
												}
												else
													WriteInUnknown = true;
												break;
											}
											default:
												WriteInUnknown = true;
										}
									}
									
									if(WriteInUnknown)
									{
										if(Format == MAPFORMAT_DDNET)
										{
											Load_UnivDDNet();
											Zones.CreateZone(&Zones.m_pDDGameZone, Zones.m_DDGamePath, "ddnetGame", m_Path_ZoneType_DDGame, Width, Height);
											if(Zones.m_pDDGameZone)
												Zones.m_pDDGameZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
										}
										else
										{
											Zones.CreateZone(&Zones.m_pUnknownZone, Zones.m_UnknownPath, "unknown", CAssetPath::Null(), Width, Height);
											if(Zones.m_pUnknownZone)
												Zones.m_pUnknownZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
										}
									}
									
									int Skip = pTiles[j*Width+i].m_Skip;
									for(int s=0; s<Skip; s++)
										i++;
								}
							}
							
							//Create a new group to separate background/foreground
							{
								CAsset_MapGroup* pNewGroup = NewAsset_Hard<CAsset_MapGroup>(&MapGroupPath, PackageId);
								
								CSubPath FgGroup = CAsset_Map::SubPath_FgGroup(pMap->AddFgGroup());
								pMap->SetFgGroup(FgGroup, MapGroupPath);
								
								AssetsManager()->TryChangeAssetName_Hard(MapGroupPath, pMapGroup->GetName());
								pNewGroup->SetPosition(pMapGroup->GetPosition());
								pNewGroup->SetHardParallax(pMapGroup->GetHardParallax());
								pNewGroup->SetParentPath(MapPath);
								pMapGroup = pNewGroup;
							}
						}
						else if(pTilemapItem->m_Flags == ddnet::TILESLAYERFLAG_FRONT)
						{
							//Tiles
							ddnet::CTile* pTiles = (ddnet::CTile*) ArchiveFile.GetData(pTilemapItem->m_Front);
							int Width = pTilemapItem->m_Width;
							int Height = pTilemapItem->m_Height;
							
							for(int j=0; j<Height; j++)
							{
								for(int i=0; i<Width; i++)
								{
									CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
									
									if(Format == MAPFORMAT_DDNET)
									{
										Load_UnivDDNet();
										Zones.CreateZone(&Zones.m_pDDFrontZone, Zones.m_DDFrontPath, "ddnetFront", m_Path_ZoneType_DDFront, Width, Height);
										if(Zones.m_pDDFrontZone)
											Zones.m_pDDFrontZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
									}
									else
									{
										Zones.CreateZone(&Zones.m_pUnknownZone, Zones.m_UnknownPath, "unknown", CAssetPath::Null(), Width, Height);
										if(Zones.m_pUnknownZone)
											Zones.m_pUnknownZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
									}
									
									int Skip = pTiles[j*Width+i].m_Skip;
									for(int s=0; s<Skip; s++)
										i++;
								}
							}
						}
						else if(pTilemapItem->m_Flags == ddnet::TILESLAYERFLAG_TELE)
						{
							//Tiles
							ddnet::CTeleTile* pTiles = (ddnet::CTeleTile*) ArchiveFile.GetData(pTilemapItem->m_Tele);
							int Width = pTilemapItem->m_Width;
							int Height = pTilemapItem->m_Height;
							
							for(int j=0; j<Height; j++)
							{
								for(int i=0; i<Width; i++)
								{
									CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
									
									if(Format == MAPFORMAT_DDNET)
									{
										Load_UnivDDNet();
										Zones.CreateZone(&Zones.m_pDDTeleZone, Zones.m_DDTelePath, "ddnetTeleport", m_Path_ZoneType_DDTele, Width, Height);
										if(Zones.m_pDDTeleZone)
										{
											Zones.m_pDDTeleZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Type);
											Zones.m_pDDTeleZone->SetDataInt(TilePath, pTiles[j*Width+i].m_Number);
										}
									}
								}
							}
						}
						else if(pTilemapItem->m_Flags == ddnet::TILESLAYERFLAG_SWITCH)
						{
							//Tiles
							ddnet::CSwitchTile* pTiles = (ddnet::CSwitchTile*) ArchiveFile.GetData(pTilemapItem->m_Switch);
							int Width = pTilemapItem->m_Width;
							int Height = pTilemapItem->m_Height;
							
							for(int j=0; j<Height; j++)
							{
								for(int i=0; i<Width; i++)
								{
									CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
									
									if(Format == MAPFORMAT_DDNET)
									{
										Load_UnivDDNet();
										Zones.CreateZone(&Zones.m_pDDSwitchZone, Zones.m_DDSwitchPath, "ddnetSwitch", m_Path_ZoneType_DDSwitch, Width, Height);
										if(Zones.m_pDDSwitchZone)
										{
											Zones.m_pDDSwitchZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Type);
											Zones.m_pDDSwitchZone->GetDataIntArray().set_clamp(i, j, 0, pTiles[j*Width+i].m_Delay);
											Zones.m_pDDSwitchZone->GetDataIntArray().set_clamp(i, j, 1, pTiles[j*Width+i].m_Number);
										}
									}
								}
							}
						}
						else if(pTilemapItem->m_Flags == 0x0)
						{
							CAssetPath MapLayerPath;
							CAsset_MapLayerTiles* pMapLayer = NewAsset_Hard<CAsset_MapLayerTiles>(&MapLayerPath, PackageId);
							
							CSubPath MapGroupSubPath = CAsset_MapGroup::SubPath_Layer(pMapGroup->AddLayer());
							pMapGroup->SetLayer(MapGroupSubPath, MapLayerPath);
							pMapLayer->SetParentPath(MapGroupPath);
							
							//Name
							aBuf[0] = 0;
							
							if(pTilemapItem->m_Version >= 3)
								IntsToStr(pTilemapItem->m_aName, sizeof(pTilemapItem->m_aName)/sizeof(int), aBuf);
							
							if(!aBuf[0])
								str_format(aBuf, sizeof(aBuf), "tilelayer%d", MapLayerPath.GetId());
							
							AssetsManager()->TryChangeAssetName_Hard(MapLayerPath, aBuf);
											
							//Tiles
							int Width = pTilemapItem->m_Width;
							int Height = pTilemapItem->m_Height;
							
							array2d<CAsset_MapLayerTiles::CTile>& Data = pMapLayer->GetTileArray();
							Data.resize(Width, Height);
							
							ddnet::CTile* pTiles = (ddnet::CTile*) ArchiveFile.GetData(pTilemapItem->m_Data);
							
							for(int j=0; j<Height; j++)
							{
								for(int i=0; i<Width; i++)
								{
									CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(i, j);
									
									int Skip = pTiles[j*Width+i].m_Skip;
									pMapLayer->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
									pMapLayer->SetTileFlags(TilePath, pTiles[j*Width+i].m_Flags);
									
									for(int s=0; s<Skip; s++)
									{
										pMapLayer->SetTileIndex(TilePath, 0);
										pMapLayer->SetTileFlags(TilePath, 0);
										i++;
										
										TilePath = CAsset_MapLayerTiles::SubPath_Tile(i, j);
									}
								}
							}
							
							//Image
							if(pTilemapItem->m_Image >= 0)
							{
								pMapLayer->SetImagePath(pImagePath[pTilemapItem->m_Image]);
								
								SetAssetValue_Hard<int>(pImagePath[pTilemapItem->m_Image], CSubPath::Null(), CAsset_Image::GRIDWIDTH, 16);
								SetAssetValue_Hard<int>(pImagePath[pTilemapItem->m_Image], CSubPath::Null(), CAsset_Image::GRIDHEIGHT, 16);
								SetAssetValue_Hard<int>(pImagePath[pTilemapItem->m_Image], CSubPath::Null(), CAsset_Image::GRIDSPACING, 2);
								SetAssetValue_Hard<bool>(pImagePath[pTilemapItem->m_Image], CSubPath::Null(), CAsset_Image::TILINGENABLED, true);
							}
							
							//Color
							vec4 Color;
							Color.r = static_cast<float>(pTilemapItem->m_Color.r)/255.0f;
							Color.g = static_cast<float>(pTilemapItem->m_Color.g)/255.0f;
							Color.b = static_cast<float>(pTilemapItem->m_Color.b)/255.0f;
							Color.a = static_cast<float>(pTilemapItem->m_Color.a)/255.0f;
							pMapLayer->SetColor(Color);
						}
						else //Layers that are unknown flags (DDNet, old InfClass, ...)
						{
							CAssetPath ZonePath;
							CAsset_MapZoneTiles* pZone = NewAsset_Hard<CAsset_MapZoneTiles>(&ZonePath, PackageId);
							AssetsManager()->TryChangeAssetName_Hard(ZonePath, "unknown");
							CSubPath MapSubPath = CAsset_Map::SubPath_ZoneLayer(pMap->AddZoneLayer());
							pMap->SetZoneLayer(MapSubPath, ZonePath);
							pZone->SetParentPath(MapPath);
							
							//Tiles
							ddnet::CTile* pTiles = (ddnet::CTile*) ArchiveFile.GetData(pTilemapItem->m_Data);
							int Width = pTilemapItem->m_Width;
							int Height = pTilemapItem->m_Height;
							
							array2d<CAsset_MapZoneTiles::CTile>& Data = pZone->GetTileArray();
							Data.resize(Width, Height);
							
							for(int j=0; j<Height; j++)
							{
								for(int i=0; i<Width; i++)
								{
									CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
									pZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
								}
							}
						}
					}
					else if(pLayerItem->m_Type == ddnet::LAYERTYPE_QUADS)
					{
						ddnet::CMapItemLayerQuads *pQuadsItem = (ddnet::CMapItemLayerQuads *)pLayerItem;

						CAssetPath MapLayerPath;
						CAsset_MapLayerQuads* pMapLayer = NewAsset_Hard<CAsset_MapLayerQuads>(&MapLayerPath, PackageId);
						
						CSubPath MapGroupSubPath = CAsset_MapGroup::SubPath_Layer(pMapGroup->AddLayer());
						pMapGroup->SetLayer(MapGroupSubPath, MapLayerPath);
						pMapLayer->SetParentPath(MapGroupPath);
						
						//Name
						aBuf[0] = 0;

						if(pQuadsItem->m_Version >= 2)
							IntsToStr(pQuadsItem->m_aName, sizeof(pQuadsItem->m_aName)/sizeof(int), aBuf);

						if(!aBuf[0])
							str_format(aBuf, sizeof(aBuf), "quadlayer%d", MapLayerPath.GetId());

						AssetsManager()->TryChangeAssetName_Hard(MapLayerPath, aBuf);
						
						//Quads
						ddnet::CQuad *pQuads = (ddnet::CQuad *) ArchiveFile.GetDataSwapped(pQuadsItem->m_Data);
						for(int i=0; i<pQuadsItem->m_NumQuads; i++)
						{
							CSubPath QuadPath = CAsset_MapLayerQuads::SubPath_Quad(pMapLayer->AddQuad());
							
							vec2 Pivot;
							Pivot.x = fx2f(pQuads[i].m_aPoints[4].x);
							Pivot.y = fx2f(pQuads[i].m_aPoints[4].y);
							pMapLayer->SetQuadPivot(QuadPath, Pivot);
							
							pMapLayer->SetQuadVertex0(QuadPath, vec2(fx2f(pQuads[i].m_aPoints[0].x), fx2f(pQuads[i].m_aPoints[0].y)) - Pivot);
							pMapLayer->SetQuadVertex1(QuadPath, vec2(fx2f(pQuads[i].m_aPoints[1].x), fx2f(pQuads[i].m_aPoints[1].y)) - Pivot);
							pMapLayer->SetQuadVertex2(QuadPath, vec2(fx2f(pQuads[i].m_aPoints[2].x), fx2f(pQuads[i].m_aPoints[2].y)) - Pivot);
							pMapLayer->SetQuadVertex3(QuadPath, vec2(fx2f(pQuads[i].m_aPoints[3].x), fx2f(pQuads[i].m_aPoints[3].y)) - Pivot);
							
							pMapLayer->SetQuadUV0(QuadPath, vec2(fx2f(pQuads[i].m_aTexcoords[0].x), fx2f(pQuads[i].m_aTexcoords[0].y)));
							pMapLayer->SetQuadUV1(QuadPath, vec2(fx2f(pQuads[i].m_aTexcoords[1].x), fx2f(pQuads[i].m_aTexcoords[1].y)));
							pMapLayer->SetQuadUV2(QuadPath, vec2(fx2f(pQuads[i].m_aTexcoords[2].x), fx2f(pQuads[i].m_aTexcoords[2].y)));
							pMapLayer->SetQuadUV3(QuadPath, vec2(fx2f(pQuads[i].m_aTexcoords[3].x), fx2f(pQuads[i].m_aTexcoords[3].y)));
							
							vec4 QuadColor = 1.0f;
							vec4 VertexColor[4];
							for(int v=0; v<4; v++)
								VertexColor[v] = IntsToColor(pQuads[i].m_aColors[v].r, pQuads[i].m_aColors[v].g, pQuads[i].m_aColors[v].b, pQuads[i].m_aColors[v].a);
							
							if(VertexColor[0] == VertexColor[1] && VertexColor[0] == VertexColor[2] && VertexColor[0] == VertexColor[3])
							{
								QuadColor = VertexColor[0];
								for(int v=0; v<4; v++)
									VertexColor[v] = 1.0f;
							}
							
							pMapLayer->SetQuadColor(QuadPath, QuadColor);
							pMapLayer->SetQuadColor0(QuadPath, VertexColor[0]);
							pMapLayer->SetQuadColor1(QuadPath, VertexColor[1]);
							pMapLayer->SetQuadColor2(QuadPath, VertexColor[2]);
							pMapLayer->SetQuadColor3(QuadPath, VertexColor[3]);
						}
						
						//Image
						if(pQuadsItem->m_Image >= 0)
							pMapLayer->SetImagePath(pImagePath[pQuadsItem->m_Image]);
					}
				}
			}
		}
	}

	delete[] pImagePath;
	
	for(int i=0; i<GetNumAssets<CAsset_Image>(PackageId); i++)
		RequestUpdate(CAssetPath(CAsset_Image::TypeId, PackageId, i));
	
	ArchiveFile.Close();
	
	if(IsValidPackage(PackageId))
		m_pPackages[PackageId]->SetEdited(true);
	
	if(m_pHistory)
		m_pHistory->Flush();
	
	return PackageId;
}

void CAssetsManager::Save_Map_Group(ddnet::CDataFileWriter& ArchiveFile, const CAssetPath& GroupPath, std::vector<CAssetPath>& Images, int& GroupId, int& LayerId)
{
	const CAsset_MapGroup* pGroup = GetAsset<CAsset_MapGroup>(GroupPath);
	if(!pGroup)
		return;
	
	CAsset_MapGroup::CIteratorLayer LayerIter;
	for(LayerIter = pGroup->BeginLayer(); LayerIter != pGroup->EndLayer(); ++LayerIter)
	{
		int StartLayer = LayerId;
		vec2 GroupOffset = 0.0f;
		
		CAssetPath LayerPath = pGroup->GetLayer(*LayerIter);
		
		if(LayerPath.GetType() == CAsset_MapLayerTiles::TypeId)
		{
			const CAsset_MapLayerTiles* pLayer = GetAsset<CAsset_MapLayerTiles>(LayerPath);
			if(!pLayer)
				continue;
			
			GroupOffset.x = -pLayer->GetPositionX()*32.0f;
			GroupOffset.y = -pLayer->GetPositionY()*32.0f;
			
			int Width = pLayer->GetTileWidth();
			int Height = pLayer->GetTileHeight();
			ddnet::CTile* pTiles = new ddnet::CTile[Width*Height];
			
			for(int j=0; j<Height; j++)
			{
				for(int i=0; i<Width; i++)
				{
					CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(i, j);
					pTiles[j*Width+i].m_Index = pLayer->GetTileIndex(TilePath);
					pTiles[j*Width+i].m_Flags = pLayer->GetTileFlags(TilePath);
					pTiles[j*Width+i].m_Skip = 0;
					pTiles[j*Width+i].m_Reserved = 0;
				}
			}
			
			{
				ddnet::CMapItemLayerTilemap LItem;
				
				LItem.m_Version = 3;
				LItem.m_Flags = 0;
				LItem.m_Layer.m_Type = ddnet::LAYERTYPE_TILES;
				LItem.m_Layer.m_Flags = 0;
				LItem.m_Width = Width;
				LItem.m_Height = Height;
				LItem.m_Color.r = pLayer->GetColor().r*255.0f;
				LItem.m_Color.g = pLayer->GetColor().g*255.0f;
				LItem.m_Color.b = pLayer->GetColor().b*255.0f;
				LItem.m_Color.a = pLayer->GetColor().a*255.0f;
				LItem.m_ColorEnv = -1;
				LItem.m_ColorEnvOffset = 0;
				LItem.m_Image = -1;
				LItem.m_Data = ArchiveFile.AddData(Width*Height*sizeof(ddnet::CTile), pTiles);
				StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pLayer->GetName());
			
				const CAsset_Image* pImage = GetAsset<CAsset_Image>(pLayer->GetImagePath());
				if(pImage)
				{
					for(unsigned int i=0; i<Images.size(); i++)
					{
						if(Images[i] == pLayer->GetImagePath())
						{
							LItem.m_Image = static_cast<int>(i);
							break;
						}
					}
					
					if(LItem.m_Image == -1)
					{
						LItem.m_Image = Images.size();
						Images.push_back(pLayer->GetImagePath());
					}
				}
				
				ArchiveFile.AddItem(ddnet::MAPITEMTYPE_LAYER, LayerId++, sizeof(ddnet::CMapItemLayerTilemap), &LItem);
			}
			
			delete[] pTiles;
		}
		else if(LayerPath.GetType() == CAsset_MapLayerQuads::TypeId)
		{
			const CAsset_MapLayerQuads* pLayer = GetAsset<CAsset_MapLayerQuads>(LayerPath);
			if(!pLayer)
				continue;
			
			ddnet::CQuad* pQuads = new ddnet::CQuad[pLayer->GetQuadArraySize()];
			
			for(int i=0; i<pLayer->GetQuadArraySize(); i++)
			{
				const CAsset_MapLayerQuads::CQuad& Quad = pLayer->GetQuad(CAsset_MapLayerQuads::SubPath_Quad(i));
				
				vec2 Pos;
				
				Pos = rotate(Quad.GetVertex0() * Quad.GetSize(), Quad.GetAngle()) + Quad.GetPivot();
				pQuads[i].m_aPoints[0].x = f2fx(Pos.x);
				pQuads[i].m_aPoints[0].y = f2fx(Pos.y);
				pQuads[i].m_aTexcoords[0].x = f2fx(Quad.GetUV0X());
				pQuads[i].m_aTexcoords[0].y = f2fx(Quad.GetUV0Y());
				pQuads[i].m_aColors[0].r = Quad.GetColor().r*Quad.GetColor0().r*255.0f;
				pQuads[i].m_aColors[0].g = Quad.GetColor().g*Quad.GetColor0().g*255.0f;
				pQuads[i].m_aColors[0].b = Quad.GetColor().b*Quad.GetColor0().b*255.0f;
				pQuads[i].m_aColors[0].a = Quad.GetColor().a*Quad.GetColor0().a*255.0f;
				
				Pos = rotate(Quad.GetVertex1() * Quad.GetSize(), Quad.GetAngle()) + Quad.GetPivot();
				pQuads[i].m_aPoints[1].x = f2fx(Pos.x);
				pQuads[i].m_aPoints[1].y = f2fx(Pos.y);
				pQuads[i].m_aTexcoords[1].x = f2fx(Quad.GetUV1X());
				pQuads[i].m_aTexcoords[1].y = f2fx(Quad.GetUV1Y());
				pQuads[i].m_aColors[1].r = Quad.GetColor().r*Quad.GetColor1().r*255.0f;
				pQuads[i].m_aColors[1].g = Quad.GetColor().g*Quad.GetColor1().g*255.0f;
				pQuads[i].m_aColors[1].b = Quad.GetColor().b*Quad.GetColor1().b*255.0f;
				pQuads[i].m_aColors[1].a = Quad.GetColor().a*Quad.GetColor1().a*255.0f;
				
				Pos = rotate(Quad.GetVertex2() * Quad.GetSize(), Quad.GetAngle()) + Quad.GetPivot();
				pQuads[i].m_aPoints[2].x = f2fx(Pos.x);
				pQuads[i].m_aPoints[2].y = f2fx(Pos.y);
				pQuads[i].m_aTexcoords[2].x = f2fx(Quad.GetUV2X());
				pQuads[i].m_aTexcoords[2].y = f2fx(Quad.GetUV2Y());
				pQuads[i].m_aColors[2].r = Quad.GetColor().r*Quad.GetColor2().r*255.0f;
				pQuads[i].m_aColors[2].g = Quad.GetColor().g*Quad.GetColor2().g*255.0f;
				pQuads[i].m_aColors[2].b = Quad.GetColor().b*Quad.GetColor2().b*255.0f;
				pQuads[i].m_aColors[2].a = Quad.GetColor().a*Quad.GetColor2().a*255.0f;
				
				Pos = rotate(Quad.GetVertex3() * Quad.GetSize(), Quad.GetAngle()) + Quad.GetPivot();
				pQuads[i].m_aPoints[3].x = f2fx(Pos.x);
				pQuads[i].m_aPoints[3].y = f2fx(Pos.y);
				pQuads[i].m_aTexcoords[3].x = f2fx(Quad.GetUV3X());
				pQuads[i].m_aTexcoords[3].y = f2fx(Quad.GetUV3Y());
				pQuads[i].m_aColors[3].r = Quad.GetColor().r*Quad.GetColor3().r*255.0f;
				pQuads[i].m_aColors[3].g = Quad.GetColor().g*Quad.GetColor3().g*255.0f;
				pQuads[i].m_aColors[3].b = Quad.GetColor().b*Quad.GetColor3().b*255.0f;
				pQuads[i].m_aColors[3].a = Quad.GetColor().a*Quad.GetColor3().a*255.0f;
				
				pQuads[i].m_aPoints[4].x = f2fx(Quad.GetPivotX());
				pQuads[i].m_aPoints[4].y = f2fx(Quad.GetPivotY());
				
				pQuads[i].m_PosEnv = -1;
				pQuads[i].m_PosEnvOffset = 0;
				pQuads[i].m_ColorEnv = -1;
				pQuads[i].m_ColorEnvOffset = 0;
			}
			
			ddnet::CMapItemLayerQuads LItem;
			LItem.m_Version = 2;
			LItem.m_Layer.m_Type = ddnet::LAYERTYPE_QUADS;
			LItem.m_Layer.m_Flags = 0;
			LItem.m_Image = -1;
			LItem.m_NumQuads = pLayer->GetQuadArraySize();
			LItem.m_Data = ArchiveFile.AddDataSwapped(pLayer->GetQuadArraySize()*sizeof(ddnet::CQuad), pQuads);
			StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pLayer->GetName());
			
			const CAsset_Image* pImage = GetAsset<CAsset_Image>(pLayer->GetImagePath());
			if(pImage)
			{
				for(unsigned int i=0; i<Images.size(); i++)
				{
					if(Images[i] == pLayer->GetImagePath())
					{
						LItem.m_Image = static_cast<int>(i);
						break;
					}
				}
				
				if(LItem.m_Image == -1)
				{
					LItem.m_Image = Images.size();
					Images.push_back(pLayer->GetImagePath());
				}
			}

			ArchiveFile.AddItem(ddnet::MAPITEMTYPE_LAYER, LayerId++, sizeof(ddnet::CMapItemLayerQuads), &LItem);
			
			delete[] pQuads;
		}
		else if(LayerPath.GetType() == CAsset_MapLayerObjects::TypeId)
		{
			const CAsset_MapLayerObjects* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(LayerPath);
			if(!pLayer)
				return;
			
			std::vector<CTexturedQuad> Quads;
			
			CAsset_MapLayerObjects::CIteratorObject Iter;
			for(Iter = pLayer->BeginObject(); Iter != pLayer->EndObject(); ++Iter)
				GenerateMaterialQuads_Object(AssetsManager(), 0.0f, Quads, pLayer->GetObject(*Iter));
			
			if(Quads.size())
			{
				std::vector<ddnet::CQuad> ExportedQuads;
				
				CAssetPath CurrentImagePath;
				for(unsigned int i=0; i<Quads.size(); i++)
				{
					if(i>0 && CurrentImagePath != Quads[i].m_ImagePath)
					{
						//Image switch: save the current quads in a layer
						ddnet::CMapItemLayerQuads LItem;
						LItem.m_Version = 2;
						LItem.m_Layer.m_Type = ddnet::LAYERTYPE_QUADS;
						LItem.m_Layer.m_Flags = 0;
						LItem.m_Image = -1;
						LItem.m_NumQuads = ExportedQuads.size();
						LItem.m_Data = ArchiveFile.AddDataSwapped(ExportedQuads.size()*sizeof(ddnet::CQuad), &ExportedQuads[0]);
						StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pLayer->GetName());
						
						const CAsset_Image* pImage = GetAsset<CAsset_Image>(CurrentImagePath);
						if(pImage)
						{
							for(unsigned int i=0; i<Images.size(); i++)
							{
								if(Images[i] == CurrentImagePath)
								{
									LItem.m_Image = static_cast<int>(i);
									break;
								}
							}
							
							if(LItem.m_Image == -1)
							{
								LItem.m_Image = Images.size();
								Images.push_back(CurrentImagePath);
							}
						}

						ArchiveFile.AddItem(ddnet::MAPITEMTYPE_LAYER, LayerId++, sizeof(ddnet::CMapItemLayerQuads), &LItem);
						ExportedQuads.clear();
					}
					
					if(i==0 || CurrentImagePath != Quads[i].m_ImagePath)
						CurrentImagePath = Quads[i].m_ImagePath;
					
					ExportedQuads.emplace_back();
					ddnet::CQuad& Quad = ExportedQuads.back();
					
					vec2 Barycenter = 0.0f;
					for(int j=0; j<4; j++)
					{
						Barycenter += vec2(Quads[i].m_Position[j]);
						Quad.m_aPoints[j].x = f2fx(Quads[i].m_Position[j].x);
						Quad.m_aPoints[j].y = f2fx(Quads[i].m_Position[j].y);
						Quad.m_aTexcoords[j].x = f2fx(Quads[i].m_Texture[j].x);
						Quad.m_aTexcoords[j].y = f2fx(Quads[i].m_Texture[j].y);
						Quad.m_aColors[j].r = Quads[i].m_Color[j].r*255.0f;
						Quad.m_aColors[j].g = Quads[i].m_Color[j].g*255.0f;
						Quad.m_aColors[j].b = Quads[i].m_Color[j].b*255.0f;
						Quad.m_aColors[j].a = Quads[i].m_Color[j].a*255.0f;
					}
					
					Quad.m_aPoints[4].x = f2fx(Barycenter.x/4.0f);
					Quad.m_aPoints[4].y = f2fx(Barycenter.y/4.0f);
					
					Quad.m_PosEnv = -1;
					Quad.m_PosEnvOffset = 0;
					Quad.m_ColorEnv = -1;
					Quad.m_ColorEnvOffset = 0;
				}
				
				//Save the remaning quads
				ddnet::CMapItemLayerQuads LItem;
				LItem.m_Version = 2;
				LItem.m_Layer.m_Type = ddnet::LAYERTYPE_QUADS;
				LItem.m_Layer.m_Flags = 0;
				LItem.m_Image = -1;
				LItem.m_NumQuads = ExportedQuads.size();
				LItem.m_Data = ArchiveFile.AddDataSwapped(ExportedQuads.size()*sizeof(ddnet::CQuad), &ExportedQuads[0]);
				StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pLayer->GetName());
				
				const CAsset_Image* pImage = GetAsset<CAsset_Image>(CurrentImagePath);
				if(pImage)
				{
					for(unsigned int i=0; i<Images.size(); i++)
					{
						if(Images[i] == CurrentImagePath)
						{
							LItem.m_Image = static_cast<int>(i);
							break;
						}
					}
					
					if(LItem.m_Image == -1)
					{
						LItem.m_Image = Images.size();
						Images.push_back(CurrentImagePath);
					}
				}

				ArchiveFile.AddItem(ddnet::MAPITEMTYPE_LAYER, LayerId++, sizeof(ddnet::CMapItemLayerQuads), &LItem);
			}
		}
	
		ddnet::CMapItemGroup GItem;
		GItem.m_Version = ddnet::CMapItemGroup::CURRENT_VERSION;
		GItem.m_OffsetX = pGroup->GetPositionX() + GroupOffset.x;
		GItem.m_OffsetY = pGroup->GetPositionY() + GroupOffset.y;
		GItem.m_ParallaxX = pGroup->GetHardParallaxX()*100.0f;
		GItem.m_ParallaxY = pGroup->GetHardParallaxY()*100.0f;
		GItem.m_StartLayer = StartLayer;
		GItem.m_NumLayers = LayerId-StartLayer;
		GItem.m_UseClipping = pGroup->GetClipping();
		GItem.m_ClipX = pGroup->GetClipPositionX();
		GItem.m_ClipY = pGroup->GetClipPositionY();
		GItem.m_ClipW = pGroup->GetClipSizeX();
		GItem.m_ClipH = pGroup->GetClipSizeY();
		StrToInts(GItem.m_aName, sizeof(GItem.m_aName)/sizeof(int), pGroup->GetName());
		ArchiveFile.AddItem(ddnet::MAPITEMTYPE_GROUP, GroupId++, sizeof(ddnet::CMapItemGroup), &GItem);
	}
}

bool CAssetsManager::Save_Map(const char* pFileName, int StorageType, int PackageId, int Format)
{
	if(!IsValidPackage(PackageId))
		return false;
	
	CAssetPath MapPath = CAssetPath(CAsset_Map::TypeId, PackageId, 0);
	const CAsset_Map* pMap = GetAsset<CAsset_Map>(MapPath);
	if(!pMap)
	{
		dbg_msg("AssetsManager", "No map to export", pFileName);
		return false;
	}
	
	ddnet::CDataFileWriter ArchiveFile;
	
	if(!ArchiveFile.Open(Storage(), StorageType, pFileName))
	{
		dbg_msg("AssetsManager", "Can't export the map at this location: %s", pFileName);
		return false;
	}
	
	if(Format == MAPFORMAT_NINSLASH)
		Load_UnivNinslash();
	else
		Load_UnivTeeWorlds();
	
	if(Format == MAPFORMAT_DDNET)
		Load_UnivDDNet();
	else if(Format == MAPFORMAT_OPENFNG)
		Load_UnivOpenFNG();
	else if(Format == MAPFORMAT_DUMMYCAPTURE)
		Load_UnivSport();
	
	std::vector<CAssetPath> Images;
	bool EntityGroupNeeded = false;
	std::vector<CAsset_MapEntities::CEntity> PTUMTeeWorldsEntities;
	std::vector<CAssetPath> ZoneLayers;
	
	//Map version
	ddnet::CMapItemVersion VerItem;
	VerItem.m_Version = 1;
	ArchiveFile.AddItem(ddnet::MAPITEMTYPE_VERSION, 0, sizeof(ddnet::CMapItemVersion), &VerItem);
	
	//Map info
	ddnet::CMapItemInfo InfoItem;
	InfoItem.m_Version = 1;
	InfoItem.m_MapVersion = ArchiveFile.AddData(str_length(GetPackageVersion(PackageId))+1, (char*) GetPackageVersion(PackageId));
	InfoItem.m_Author = ArchiveFile.AddData(str_length(GetPackageAuthor(PackageId))+1, (char*) GetPackageAuthor(PackageId));
	InfoItem.m_Credits = ArchiveFile.AddData(str_length(GetPackageCredits(PackageId))+1, (char*) GetPackageCredits(PackageId));
	InfoItem.m_License = ArchiveFile.AddData(str_length(GetPackageLicense(PackageId))+1, (char*) GetPackageLicense(PackageId));
	ArchiveFile.AddItem(ddnet::MAPITEMTYPE_INFO, 0, sizeof(ddnet::CMapItemInfo), &InfoItem);
	
	int GroupId = 0;
	int LayerId = 0;
	
	//Step1: Save background layers
	{
		CAsset_Map::CIteratorBgGroup Iter;
		for(Iter = pMap->BeginBgGroup(); Iter != pMap->EndBgGroup(); ++Iter)
		{
			Save_Map_Group(ArchiveFile, pMap->GetBgGroup(*Iter), Images, GroupId, LayerId);
		}
	}
	
	//Step2: Save the game/front/tele/switch layer
	{
		int StartLayer = LayerId;
		
		//Get game layer size
		int GameWidth = 0;
		int GameHeight = 0;
		int FrontWidth = 0;
		int FrontHeight = 0;
		int TeleWidth = 0;
		int TeleHeight = 0;
		int SwitchWidth = 0;
		int SwitchHeight = 0;
		
		{
			CAsset_Map::CIteratorZoneLayer ZoneIter;
			for(ZoneIter = pMap->BeginZoneLayer(); ZoneIter != pMap->EndZoneLayer(); ++ZoneIter)
			{
				const CAsset_MapZoneTiles* pZone = GetAsset<CAsset_MapZoneTiles>(pMap->GetZoneLayer(*ZoneIter));
				if(pZone)
				{
					if(Format == MAPFORMAT_NINSLASH)
					{
						if(pZone->GetZoneTypePath() == m_Path_ZoneType_Ninslash)
						{
							GameWidth = max(pZone->GetTileWidth(), GameWidth);
							GameHeight = max(pZone->GetTileHeight(), GameHeight);
						}
					}
					else
					{
						if(pZone->GetZoneTypePath() == m_Path_ZoneType_TeeWorlds)
						{
							GameWidth = max(pZone->GetTileWidth(), GameWidth);
							GameHeight = max(pZone->GetTileHeight(), GameHeight);
						}
						else if(Format == MAPFORMAT_DUMMYCAPTURE && m_PackageId_UnivSport >= 0 && pZone->GetZoneTypePath() == m_Path_ZoneType_Sport)
						{
							GameWidth = max(pZone->GetTileWidth(), GameWidth);
							GameHeight = max(pZone->GetTileHeight(), GameHeight);
						}
						else if(Format == MAPFORMAT_OPENFNG && m_PackageId_UnivOpenFNG >= 0 && pZone->GetZoneTypePath() == m_Path_ZoneType_OpenFNG)
						{
							GameWidth = max(pZone->GetTileWidth(), GameWidth);
							GameHeight = max(pZone->GetTileHeight(), GameHeight);
						}
						else if(Format == MAPFORMAT_DDNET && m_PackageId_UnivDDNet >= 0 && pZone->GetZoneTypePath() == m_Path_ZoneType_DDGame)
						{
							GameWidth = max(pZone->GetTileWidth(), GameWidth);
							GameHeight = max(pZone->GetTileHeight(), GameHeight);
						}
						else if(Format == MAPFORMAT_DDNET && m_PackageId_UnivDDNet >= 0 && pZone->GetZoneTypePath() == m_Path_ZoneType_DDFront)
						{
							FrontWidth = max(pZone->GetTileWidth(), FrontWidth);
							FrontHeight = max(pZone->GetTileHeight(), FrontHeight);
						}
						else if(Format == MAPFORMAT_DDNET && m_PackageId_UnivDDNet >= 0 && pZone->GetZoneTypePath() == m_Path_ZoneType_DDTele)
						{
							TeleWidth = max(pZone->GetTileWidth(), TeleWidth);
							TeleHeight = max(pZone->GetTileHeight(), TeleHeight);
						}
						else if(Format == MAPFORMAT_DDNET && m_PackageId_UnivDDNet >= 0 && pZone->GetZoneTypePath() == m_Path_ZoneType_DDSwitch)
						{
							SwitchWidth = max(pZone->GetTileWidth(), TeleWidth);
							SwitchHeight = max(pZone->GetTileHeight(), TeleHeight);
						}
					}
				}
				
			}
		}
		
		ddnet::CTile* pGameTiles = new ddnet::CTile[GameWidth*GameHeight];
		
		for(int j=0; j<GameHeight; j++)
		{
			for(int i=0; i<GameWidth; i++)
			{
				pGameTiles[j*GameWidth+i].m_Index = 0;
				pGameTiles[j*GameWidth+i].m_Flags = 0;
				pGameTiles[j*GameWidth+i].m_Skip = 0;
				pGameTiles[j*GameWidth+i].m_Reserved = 0;
			}
		}
			
		ddnet::CTile* pFrontTiles = NULL;
		if(FrontWidth > 0 && FrontHeight > 0)
		{
			pFrontTiles = new ddnet::CTile[FrontWidth*FrontHeight];	
			
			for(int j=0; j<FrontHeight; j++)
			{
				for(int i=0; i<FrontWidth; i++)
				{
					pFrontTiles[j*FrontWidth+i].m_Index = 0;
					pFrontTiles[j*FrontWidth+i].m_Flags = 0;
					pFrontTiles[j*FrontWidth+i].m_Skip = 0;
					pFrontTiles[j*FrontWidth+i].m_Reserved = 0;
				}
			}
		}
			
		ddnet::CTeleTile* pTeleTiles = NULL;
		if(TeleWidth > 0 && TeleHeight > 0)
		{
			pTeleTiles = new ddnet::CTeleTile[TeleWidth*TeleHeight];	
			
			for(int j=0; j<TeleHeight; j++)
			{
				for(int i=0; i<TeleWidth; i++)
				{
					pTeleTiles[j*TeleWidth+i].m_Number = 0;
					pTeleTiles[j*TeleWidth+i].m_Type = 0;
				}
			}
		}
			
		ddnet::CSwitchTile* pSwitchTiles = NULL;
		if(SwitchWidth > 0 && SwitchHeight > 0)
		{
			pSwitchTiles = new ddnet::CSwitchTile[SwitchWidth*SwitchHeight];	
			
			for(int j=0; j<SwitchHeight; j++)
			{
				for(int i=0; i<SwitchWidth; i++)
				{
					pSwitchTiles[j*SwitchWidth+i].m_Number = 0;
					pSwitchTiles[j*SwitchWidth+i].m_Type = 0;
				}
			}
		}
	
		//Game layer
		CAsset_Map::CIteratorZoneLayer ZoneIter;
		for(ZoneIter = pMap->BeginZoneLayer(); ZoneIter != pMap->EndZoneLayer(); ++ZoneIter)
		{
			if(pMap->GetZoneLayer(*ZoneIter).GetType() == CAsset_MapZoneTiles::TypeId)
			{
				const CAsset_MapZoneTiles* pZone = GetAsset<CAsset_MapZoneTiles>(pMap->GetZoneLayer(*ZoneIter));
				if(!pZone)
					continue;
				
				if(Format == MAPFORMAT_NINSLASH)
				{
					if(pZone->GetZoneTypePath() == m_Path_ZoneType_Ninslash)
					{
						for(int j=0; j<pZone->GetTileHeight(); j++)
						{
							for(int i=0; i<pZone->GetTileWidth(); i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								switch(pZone->GetTileIndex(TilePath))
								{
									case 0:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_SPAWN:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_SPAWN_RED:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_SPAWN_BLUE:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_FLAGSTAND_RED:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_FLAGSTAND_BLUE:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_HEALTH_1:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_CHAINSAW:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_SHOTGUN:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_GRENADE:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_LASER:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_RIFLE:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_ELECTRIC:
									case ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_FLAMER:
										break;
									default:
										pGameTiles[j*GameWidth+i].m_Index = pZone->GetTileIndex(TilePath);
								}
							}
						}
					}
					else
						ZoneLayers.push_back(pMap->GetZoneLayer(*ZoneIter));
				}
				else
				{
					if(pZone->GetZoneTypePath() == m_Path_ZoneType_TeeWorlds)
					{
						for(int j=0; j<pZone->GetTileHeight(); j++)
						{
							for(int i=0; i<pZone->GetTileWidth(); i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								
								switch(pZone->GetTileIndex(TilePath))
								{
									case 1:
										pGameTiles[j*GameWidth+i].m_Index = ddnet::TILE_SOLID;
										break;
									case 2:
										if(pGameTiles[j*GameWidth+i].m_Index != ddnet::TILE_SOLID && pGameTiles[j*GameWidth+i].m_Index != ddnet::TILE_NOHOOK)
											pGameTiles[j*GameWidth+i].m_Index = ddnet::TILE_DEATH;
										break;
									case 3:
										pGameTiles[j*GameWidth+i].m_Index = ddnet::TILE_NOHOOK;
										break;
								}
							}
						}
					}
					else if(Format == MAPFORMAT_DUMMYCAPTURE && m_PackageId_UnivSport >= 0 && pZone->GetZoneTypePath() == m_Path_ZoneType_Sport)
					{
						for(int j=0; j<pZone->GetTileHeight(); j++)
						{
							for(int i=0; i<pZone->GetTileWidth(); i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								if(pGameTiles[j*GameWidth+i].m_Index != ddnet::TILE_SOLID && pGameTiles[j*GameWidth+i].m_Index != ddnet::TILE_NOHOOK)
								{
									switch(pZone->GetTileIndex(TilePath))
									{
										case 2:
											pGameTiles[j*GameWidth+i].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_FLAGSTAND_RED;
											break;
										case 3:
											pGameTiles[j*GameWidth+i].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_FLAGSTAND_BLUE;
											break;
									}
								}
							}
						}
					}
					else if(Format == MAPFORMAT_OPENFNG && m_PackageId_UnivOpenFNG >= 0 && pZone->GetZoneTypePath() == m_Path_ZoneType_OpenFNG)
					{
						for(int j=0; j<pZone->GetTileHeight(); j++)
						{
							for(int i=0; i<pZone->GetTileWidth(); i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								if(pGameTiles[j*GameWidth+i].m_Index != ddnet::TILE_SOLID && pGameTiles[j*GameWidth+i].m_Index != ddnet::TILE_NOHOOK)
								{
									if(pZone->GetTileIndex(TilePath) > 0)
										pGameTiles[j*GameWidth+i].m_Index = pZone->GetTileIndex(TilePath);
								}
							}
						}
					}
					else if(Format == MAPFORMAT_DDNET && m_PackageId_UnivDDNet >= 0 && pZone->GetZoneTypePath() == m_Path_ZoneType_DDGame)
					{
						for(int j=0; j<pZone->GetTileHeight(); j++)
						{
							for(int i=0; i<pZone->GetTileWidth(); i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								if(pZone->GetTileIndex(TilePath) > 0)
									pGameTiles[j*GameWidth+i].m_Index = pZone->GetTileIndex(TilePath);
							}
						}
					}
					else if(Format == MAPFORMAT_DDNET && pFrontTiles && m_PackageId_UnivDDNet >= 0 && pZone->GetZoneTypePath() == m_Path_ZoneType_DDFront)
					{
						for(int j=0; j<pZone->GetTileHeight(); j++)
						{
							for(int i=0; i<pZone->GetTileWidth(); i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								if(pZone->GetTileIndex(TilePath) > 0)
									pFrontTiles[j*FrontWidth+i].m_Index = pZone->GetTileIndex(TilePath);
							}
						}
					}
					else if(Format == MAPFORMAT_DDNET && pTeleTiles && m_PackageId_UnivDDNet >= 0 && pZone->GetZoneTypePath() == m_Path_ZoneType_DDTele)
					{
						for(int j=0; j<pZone->GetTileHeight(); j++)
						{
							for(int i=0; i<pZone->GetTileWidth(); i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								if(pZone->GetTileIndex(TilePath) > 0)
									pTeleTiles[j*TeleWidth+i].m_Type = pZone->GetTileIndex(TilePath);
							}
						}
					}
					else if(Format == MAPFORMAT_DDNET && pSwitchTiles && m_PackageId_UnivDDNet >= 0 && pZone->GetZoneTypePath() == m_Path_ZoneType_DDSwitch)
					{
						for(int j=0; j<pZone->GetTileHeight(); j++)
						{
							for(int i=0; i<pZone->GetTileWidth(); i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								if(pZone->GetTileIndex(TilePath) > 0)
									pSwitchTiles[j*SwitchWidth+i].m_Type = pZone->GetTileIndex(TilePath);
							}
						}
					}
					else
						ZoneLayers.push_back(pMap->GetZoneLayer(*ZoneIter));
				}
			}
			else if(pMap->GetZoneLayer(*ZoneIter).GetType() == CAsset_MapZoneObjects::TypeId)
			{
				ZoneLayers.push_back(pMap->GetZoneLayer(*ZoneIter));
			}
		}
	
		//TeeWorlds entities
		CAsset_Map::CIteratorEntityLayer EntityLayerIter;
		for(EntityLayerIter = pMap->BeginEntityLayer(); EntityLayerIter != pMap->EndEntityLayer(); ++EntityLayerIter)
		{
			const CAsset_MapEntities* pEntities = GetAsset<CAsset_MapEntities>(pMap->GetEntityLayer(*EntityLayerIter));
			if(!pEntities)
				continue;
			
			for(int i=0; i<pEntities->GetEntityArraySize(); i++)
			{
				CSubPath SubPath = CAsset_MapEntities::SubPath_Entity(i);
				CAssetPath EntityTypePath = pEntities->GetEntityTypePath(SubPath);
				int X = (pEntities->GetEntityPositionX(SubPath)-16.0f)/32.0f;
				int Y = (pEntities->GetEntityPositionY(SubPath)-16.0f)/32.0f;
				
				if(X < 0 || X >= GameWidth || Y < 0 || Y >= GameHeight)
				{
					EntityGroupNeeded = true;
					PTUMTeeWorldsEntities.push_back(pEntities->GetEntity(SubPath));
					continue;
				}
				if(pGameTiles[Y*GameWidth+X].m_Index == ddnet::TILE_SOLID || pGameTiles[Y*GameWidth+X].m_Index == ddnet::TILE_NOHOOK)
				{
					EntityGroupNeeded = true;
					PTUMTeeWorldsEntities.push_back(pEntities->GetEntity(SubPath));
					continue;
				}
				if(distance(pEntities->GetEntityPosition(SubPath), vec2(X*32.0f+16.0f, Y*32.0f+16.0f)) > 0.01f)
				{
					EntityGroupNeeded = true;
					PTUMTeeWorldsEntities.push_back(pEntities->GetEntity(SubPath));
					continue;
				}
				
				if(Format == MAPFORMAT_NINSLASH)
				{
					if(EntityTypePath == m_Path_EntityType_NSSpawn)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_SPAWN;
					else if(EntityTypePath == m_Path_EntityType_NSSpawnRed)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_SPAWN_RED;
					else if(EntityTypePath == m_Path_EntityType_NSSpawnBlue)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_SPAWN_BLUE;
					else if(EntityTypePath == m_Path_EntityType_NSShotgun)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_SHOTGUN;
					else if(EntityTypePath == m_Path_EntityType_NSGrenade)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_GRENADE;
					else if(EntityTypePath == m_Path_EntityType_NSLaser)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_LASER;
					else if(EntityTypePath == m_Path_EntityType_NSRifle)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_RIFLE;
					else if(EntityTypePath == m_Path_EntityType_NSChainsaw)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_CHAINSAW;
					else if(EntityTypePath == m_Path_EntityType_NSElectric)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_ELECTRIC;
					else if(EntityTypePath == m_Path_EntityType_NSFlamer)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_WEAPON_FLAMER;
					else if(EntityTypePath == m_Path_EntityType_NSHeart)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_HEALTH_1;
					else if(EntityTypePath == m_Path_EntityType_NSFlagBlue)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_FLAGSTAND_BLUE;
					else if(EntityTypePath == m_Path_EntityType_NSFlagRed)
						pGameTiles[Y*GameWidth+X].m_Index = ninslash::ENTITY_OFFSET + ninslash::ENTITY_FLAGSTAND_RED;
					else
						EntityGroupNeeded = true;
				}
				else
				{
					if(EntityTypePath == m_Path_EntityType_TWSpawn)
						pGameTiles[Y*GameWidth+X].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_SPAWN;
					else if(EntityTypePath == m_Path_EntityType_TWSpawnRed)
						pGameTiles[Y*GameWidth+X].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_SPAWN_RED;
					else if(EntityTypePath == m_Path_EntityType_TWSpawnBlue)
						pGameTiles[Y*GameWidth+X].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_SPAWN_BLUE;
					else if(EntityTypePath == m_Path_EntityType_TWShotgun)
						pGameTiles[Y*GameWidth+X].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_WEAPON_SHOTGUN;
					else if(EntityTypePath == m_Path_EntityType_TWGrenade)
						pGameTiles[Y*GameWidth+X].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_WEAPON_GRENADE;
					else if(EntityTypePath == m_Path_EntityType_TWLaserRifle)
						pGameTiles[Y*GameWidth+X].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_WEAPON_RIFLE;
					else if(EntityTypePath == m_Path_EntityType_TWNinja)
						pGameTiles[Y*GameWidth+X].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_POWERUP_NINJA;
					else if(EntityTypePath == m_Path_EntityType_TWHeart)
						pGameTiles[Y*GameWidth+X].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_HEALTH_1;
					else if(EntityTypePath == m_Path_EntityType_TWArmor)
						pGameTiles[Y*GameWidth+X].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_ARMOR_1;
					else if(EntityTypePath == m_Path_EntityType_TWFlagBlue)
						pGameTiles[Y*GameWidth+X].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_FLAGSTAND_BLUE;
					else if(EntityTypePath == m_Path_EntityType_TWFlagRed)
						pGameTiles[Y*GameWidth+X].m_Index = ddnet::ENTITY_OFFSET + ddnet::ENTITY_FLAGSTAND_RED;
					else
						EntityGroupNeeded = true;
				}
			}
		}
		
		{
			ddnet::CMapItemLayerTilemap LItem;
			LItem.m_Version = 3;
			LItem.m_Flags = ddnet::TILESLAYERFLAG_GAME;
			LItem.m_Layer.m_Type = ddnet::LAYERTYPE_TILES;
			LItem.m_Layer.m_Flags = 0;
			LItem.m_Width = GameWidth;
			LItem.m_Height = GameHeight;
			LItem.m_Color.r = 255;
			LItem.m_Color.g = 255;
			LItem.m_Color.b = 255;
			LItem.m_Color.a = 255;
			LItem.m_ColorEnv = -1;
			LItem.m_ColorEnvOffset = 0;
			LItem.m_Image = -1;
			LItem.m_Data = ArchiveFile.AddData(GameWidth*GameHeight*sizeof(ddnet::CTile), pGameTiles);
			StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), "Game");
			ArchiveFile.AddItem(ddnet::MAPITEMTYPE_LAYER, LayerId++, sizeof(ddnet::CMapItemLayerTilemap), &LItem);
			
			delete[] pGameTiles;
		}
		if(pFrontTiles)
		{
			ddnet::CTile* pZeroTiles = new ddnet::CTile[FrontWidth*FrontHeight];
			mem_zero(pZeroTiles, sizeof(ddnet::CTile)*FrontWidth*FrontHeight);
			
			ddnet::CMapItemLayerTilemap LItem;
			LItem.m_Version = 3;
			LItem.m_Flags = ddnet::TILESLAYERFLAG_FRONT;
			LItem.m_Layer.m_Type = ddnet::LAYERTYPE_TILES;
			LItem.m_Layer.m_Flags = 0;
			LItem.m_Width = FrontWidth;
			LItem.m_Height = FrontHeight;
			LItem.m_Color.r = 255;
			LItem.m_Color.g = 255;
			LItem.m_Color.b = 255;
			LItem.m_Color.a = 255;
			LItem.m_ColorEnv = -1;
			LItem.m_ColorEnvOffset = 0;
			LItem.m_Image = -1;
			LItem.m_Data = ArchiveFile.AddData(FrontWidth*FrontHeight*sizeof(ddnet::CTile), pZeroTiles);
			LItem.m_Front = ArchiveFile.AddData(FrontWidth*FrontHeight*sizeof(ddnet::CTile), pFrontTiles);
			StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), "Front");
			ArchiveFile.AddItem(ddnet::MAPITEMTYPE_LAYER, LayerId++, sizeof(ddnet::CMapItemLayerTilemap), &LItem);
			
			delete[] pZeroTiles;
			delete[] pFrontTiles;
		}
		if(pTeleTiles)
		{
			ddnet::CTile* pZeroTiles = new ddnet::CTile[TeleWidth*TeleHeight];
			mem_zero(pZeroTiles, sizeof(ddnet::CTile)*TeleWidth*TeleHeight);
			
			ddnet::CMapItemLayerTilemap LItem;
			LItem.m_Version = 3;
			LItem.m_Flags = ddnet::TILESLAYERFLAG_TELE;
			LItem.m_Layer.m_Type = ddnet::LAYERTYPE_TILES;
			LItem.m_Layer.m_Flags = 0;
			LItem.m_Width = TeleWidth;
			LItem.m_Height = TeleHeight;
			LItem.m_Color.r = 255;
			LItem.m_Color.g = 255;
			LItem.m_Color.b = 255;
			LItem.m_Color.a = 255;
			LItem.m_ColorEnv = -1;
			LItem.m_ColorEnvOffset = 0;
			LItem.m_Image = -1;
			LItem.m_Data = ArchiveFile.AddData(TeleWidth*TeleHeight*sizeof(ddnet::CTile), pZeroTiles);
			LItem.m_Tele = ArchiveFile.AddData(TeleWidth*TeleHeight*sizeof(ddnet::CTeleTile), pTeleTiles);
			StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), "Front");
			ArchiveFile.AddItem(ddnet::MAPITEMTYPE_LAYER, LayerId++, sizeof(ddnet::CMapItemLayerTilemap), &LItem);
			
			delete[] pZeroTiles;
			delete[] pTeleTiles;
		}
		if(pSwitchTiles)
		{
			ddnet::CTile* pZeroTiles = new ddnet::CTile[SwitchWidth*SwitchHeight];
			mem_zero(pZeroTiles, sizeof(ddnet::CTile)*SwitchWidth*SwitchHeight);
			
			ddnet::CMapItemLayerTilemap LItem;
			LItem.m_Version = 3;
			LItem.m_Flags = ddnet::TILESLAYERFLAG_SWITCH;
			LItem.m_Layer.m_Type = ddnet::LAYERTYPE_TILES;
			LItem.m_Layer.m_Flags = 0;
			LItem.m_Width = SwitchWidth;
			LItem.m_Height = SwitchHeight;
			LItem.m_Color.r = 255;
			LItem.m_Color.g = 255;
			LItem.m_Color.b = 255;
			LItem.m_Color.a = 255;
			LItem.m_ColorEnv = -1;
			LItem.m_ColorEnvOffset = 0;
			LItem.m_Image = -1;
			LItem.m_Data = ArchiveFile.AddData(SwitchWidth*SwitchHeight*sizeof(ddnet::CTile), pZeroTiles);
			LItem.m_Switch = ArchiveFile.AddData(SwitchWidth*SwitchHeight*sizeof(ddnet::CSwitchTile), pSwitchTiles);
			StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), "Front");
			ArchiveFile.AddItem(ddnet::MAPITEMTYPE_LAYER, LayerId++, sizeof(ddnet::CMapItemLayerTilemap), &LItem);
			
			delete[] pZeroTiles;
			delete[] pSwitchTiles;
		}
		
		ddnet::CMapItemGroup GItem;
		GItem.m_Version = ddnet::CMapItemGroup::CURRENT_VERSION;
		GItem.m_OffsetX = 0;
		GItem.m_OffsetY = 0;
		GItem.m_ParallaxX = 100;
		GItem.m_ParallaxY = 100;
		GItem.m_StartLayer = StartLayer;
		GItem.m_NumLayers = LayerId-StartLayer;
		GItem.m_UseClipping = 0;
		GItem.m_ClipX = 0;
		GItem.m_ClipY = 0;
		GItem.m_ClipW = 0;
		GItem.m_ClipH = 0;
		StrToInts(GItem.m_aName, sizeof(GItem.m_aName)/sizeof(int), "Game");
		ArchiveFile.AddItem(ddnet::MAPITEMTYPE_GROUP, GroupId++, sizeof(ddnet::CMapItemGroup), &GItem);
		
	}
	
	//Step3: Save forground layers
	{
		CAsset_Map::CIteratorFgGroup Iter;
		for(Iter = pMap->BeginFgGroup(); Iter != pMap->EndFgGroup(); ++Iter)
		{
			Save_Map_Group(ArchiveFile, pMap->GetFgGroup(*Iter), Images, GroupId, LayerId);
		}
	}
	
	//Step4: Save other zones in PTUM format
	if(ZoneLayers.size())
	{
		int StartLayer = LayerId;
		
		CAsset_Map::CIteratorZoneLayer ZoneIter;
		for(unsigned int i=0; i<ZoneLayers.size(); i++)
		{
			if(ZoneLayers[i].GetType() == CAsset_MapZoneTiles::TypeId)
			{
				const CAsset_MapZoneTiles* pZone = GetAsset<CAsset_MapZoneTiles>(ZoneLayers[i]);
				if(!pZone)
					continue;
				
				int Width = pZone->GetTileWidth();
				int Height = pZone->GetTileHeight();
		
				ddnet::CTile* pTiles = new ddnet::CTile[Width*Height];
				
				for(int j=0; j<Height; j++)
				{
					for(int i=0; i<Width; i++)
					{
						pTiles[j*Width+i].m_Index = 0;
						pTiles[j*Width+i].m_Flags = 0;
						pTiles[j*Width+i].m_Skip = 0;
						pTiles[j*Width+i].m_Reserved = 0;
					}
				}
				
				for(int j=0; j<pZone->GetTileHeight(); j++)
				{
					for(int i=0; i<pZone->GetTileWidth(); i++)
					{
						CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
						pTiles[j*Width+i].m_Index = pZone->GetTileIndex(TilePath);
					}
				}
		
				ddnet::CMapItemLayerTilemap LItem;
				
				LItem.m_Version = 3;
				LItem.m_Flags = 0;
				LItem.m_Layer.m_Type = ddnet::LAYERTYPE_TILES;
				LItem.m_Layer.m_Flags = 0;
				LItem.m_Width = Width;
				LItem.m_Height = Height;
				LItem.m_Color.r = 255;
				LItem.m_Color.g = 255;
				LItem.m_Color.b = 255;
				LItem.m_Color.a = 255;
				LItem.m_ColorEnv = -1;
				LItem.m_ColorEnvOffset = 0;
				LItem.m_Image = -1;
				LItem.m_Data = ArchiveFile.AddData(Width*Height*sizeof(ddnet::CTile), pTiles);
				
				{
					const char* pZoneName = GetAssetValue<const char*>(pZone->GetZoneTypePath(), CSubPath::Null(), CAsset_ZoneType::NAME, NULL);
					if(pZoneName)
						StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pZoneName);
					else
						StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), "unknown");
				}
				
				ArchiveFile.AddItem(ddnet::MAPITEMTYPE_LAYER, LayerId++, sizeof(ddnet::CMapItemLayerTilemap), &LItem);
				
				delete[] pTiles;
			}
			else if(ZoneLayers[i].GetType() == CAsset_MapZoneObjects::TypeId)
			{
				const CAsset_MapZoneObjects* pZone = GetAsset<CAsset_MapZoneObjects>(pMap->GetZoneLayer(*ZoneIter));
				if(!pZone)
					continue;
				
				std::vector<ddnet::CQuad> Quads;
				
				CAsset_MapZoneObjects::CIteratorObject Iter;
				for(Iter = pZone->BeginObject(); Iter != pZone->EndObject(); ++Iter)
				{
					const CAsset_MapZoneObjects::CObject& Object = pZone->GetObject(*Iter);
					
					vec2 ObjPosition;
					matrix2x2 Transform;
					Object.GetTransform(this, 0.0f, &Transform, &ObjPosition);
					
					std::vector<CLineVertex> LineVertices;
					std::vector<CQuad> ObjectQuads;
					
					GenerateZoneCurve_Object(this, 0.0f, LineVertices, Object);
					PolygonQuadrangulation(LineVertices, ObjectQuads);
					
					for(unsigned int i=0; i<ObjectQuads.size(); i++)
					{
						Quads.emplace_back();
						ddnet::CQuad& Quad = Quads.back();
						
						Quad.m_aPoints[4].x = f2fx(ObjPosition.x);
						Quad.m_aPoints[4].y = f2fx(ObjPosition.y);
						
						for(int j=0; j<4; j++)
						{
							vec2 Pos = ObjPosition + Transform*ObjectQuads[i].m_Position[j];
							Quad.m_aPoints[j].x = f2fx(Pos.x);
							Quad.m_aPoints[j].y = f2fx(Pos.y);
							Quad.m_aTexcoords[j].x = 0.0f;
							Quad.m_aTexcoords[j].y = 0.0f;
							Quad.m_aColors[j].r = 255.0f;
							Quad.m_aColors[j].g = 255.0f;
							Quad.m_aColors[j].b = 255.0f;
							Quad.m_aColors[j].a = 255.0f;
						}
						
						Quad.m_PosEnv = -1;
						Quad.m_PosEnvOffset = 0;
						Quad.m_ColorEnv = -1;
						Quad.m_ColorEnvOffset = Object.GetZoneIndex();
					}					
				}
				
				ddnet::CMapItemLayerQuads LItem;
				LItem.m_Version = 2;
				LItem.m_Layer.m_Type = ddnet::LAYERTYPE_QUADS;
				LItem.m_Layer.m_Flags = 0;
				LItem.m_Image = -1;
				LItem.m_NumQuads = Quads.size();
				LItem.m_Data = ArchiveFile.AddDataSwapped(Quads.size()*sizeof(ddnet::CQuad), &Quads[0]);
				
				{
					const char* pZoneName = GetAssetValue<const char*>(pZone->GetZoneTypePath(), CSubPath::Null(), CAsset_ZoneType::NAME, NULL);
					if(pZoneName)
						StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pZoneName);
					else
						StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), "unknown");
				}
				
				ArchiveFile.AddItem(ddnet::MAPITEMTYPE_LAYER, LayerId++, sizeof(ddnet::CMapItemLayerQuads), &LItem);
			}
		}
			
		ddnet::CMapItemGroup GItem;
		GItem.m_Version = ddnet::CMapItemGroup::CURRENT_VERSION;
		GItem.m_OffsetX = 0;
		GItem.m_OffsetY = 0;
		GItem.m_ParallaxX = 100;
		GItem.m_ParallaxY = 100;
		GItem.m_StartLayer = StartLayer;
		GItem.m_NumLayers = LayerId-StartLayer;
		GItem.m_UseClipping = 1;
		GItem.m_ClipX = 0;
		GItem.m_ClipY = 0;
		GItem.m_ClipW = 0;
		GItem.m_ClipH = 0;
		StrToInts(GItem.m_aName, sizeof(GItem.m_aName)/sizeof(int), "#Zones");
		ArchiveFile.AddItem(ddnet::MAPITEMTYPE_GROUP, GroupId++, sizeof(ddnet::CMapItemGroup), &GItem);
	}
	
	//Step5: Save other entities in PTUM format
	if(EntityGroupNeeded)
	{
		int StartLayer = LayerId;
		
		std::vector<CAssetPath> EntityTypeList;
		std::vector< std::vector<ddnet::CQuad> > EntityQuads;
		
		for(unsigned int i=0; i<PTUMTeeWorldsEntities.size(); i++)
		{
			CAsset_MapEntities::CEntity& Entity = PTUMTeeWorldsEntities[i];
			CAssetPath EntityTypePath = Entity.GetTypePath();
			
			//Search in previous layers
			unsigned int eId;
			for(eId=0; eId<EntityTypeList.size(); eId++)
			{
				if(EntityTypeList[eId] == EntityTypePath)
					break;
			}
			if(eId == EntityTypeList.size())
			{
				EntityTypeList.push_back(EntityTypePath);
				EntityQuads.emplace_back();
			}
			
			EntityQuads[eId].emplace_back();
			ddnet::CQuad& Quad = EntityQuads[eId].back();
			
			Quad.m_aPoints[0].x = f2fx(Entity.GetPositionX()-16.0f);
			Quad.m_aPoints[0].y = f2fx(Entity.GetPositionY()-16.0f);
			Quad.m_aPoints[1].x = f2fx(Entity.GetPositionX()+16.0f);
			Quad.m_aPoints[1].y = f2fx(Entity.GetPositionY()-16.0f);
			Quad.m_aPoints[2].x = f2fx(Entity.GetPositionX()-16.0f);
			Quad.m_aPoints[2].y = f2fx(Entity.GetPositionY()+16.0f);
			Quad.m_aPoints[3].x = f2fx(Entity.GetPositionX()+16.0f);
			Quad.m_aPoints[3].y = f2fx(Entity.GetPositionY()+16.0f);
			Quad.m_aPoints[4].x = f2fx(Entity.GetPositionX());
			Quad.m_aPoints[4].y = f2fx(Entity.GetPositionY());
			
			for(int j=0; j<4; j++)
			{
				Quad.m_aTexcoords[j].x = 0.0f;
				Quad.m_aTexcoords[j].y = 0.0f;
				Quad.m_aColors[j].r = 255.0f;
				Quad.m_aColors[j].g = 255.0f;
				Quad.m_aColors[j].b = 255.0f;
				Quad.m_aColors[j].a = 255.0f;
			}
			
			Quad.m_PosEnv = -1;
			Quad.m_PosEnvOffset = 0;
			Quad.m_ColorEnv = -1;
			Quad.m_ColorEnvOffset = 0;
		}
		
		CAsset_Map::CIteratorEntityLayer EntityLayerIter;
		for(EntityLayerIter = pMap->BeginEntityLayer(); EntityLayerIter != pMap->EndEntityLayer(); ++EntityLayerIter)
		{
			const CAsset_MapEntities* pEntities = GetAsset<CAsset_MapEntities>(pMap->GetEntityLayer(*EntityLayerIter));
			if(!pEntities)
				continue;
			
			for(int i=0; i<pEntities->GetEntityArraySize(); i++)
			{
				CSubPath SubPath = CAsset_MapEntities::SubPath_Entity(i);
				CAssetPath EntityTypePath = pEntities->GetEntityTypePath(SubPath);
				
				if(EntityTypePath == m_Path_EntityType_TWSpawn)
					continue;
				else if(EntityTypePath == m_Path_EntityType_TWSpawnRed)
					continue;
				else if(EntityTypePath == m_Path_EntityType_TWSpawnBlue)
					continue;
				else if(EntityTypePath == m_Path_EntityType_TWShotgun)
					continue;
				else if(EntityTypePath == m_Path_EntityType_TWGrenade)
					continue;
				else if(EntityTypePath == m_Path_EntityType_TWLaserRifle)
					continue;
				else if(EntityTypePath == m_Path_EntityType_TWNinja)
					continue;
				else if(EntityTypePath == m_Path_EntityType_TWHeart)
					continue;
				else if(EntityTypePath == m_Path_EntityType_TWArmor)
					continue;
				else if(EntityTypePath == m_Path_EntityType_TWFlagBlue)
					continue;
				else if(EntityTypePath == m_Path_EntityType_TWFlagRed)
					continue;
				
				//Search in previous layers
				unsigned int eId;
				for(eId=0; eId<EntityTypeList.size(); eId++)
				{
					if(EntityTypeList[eId] == EntityTypePath)
						break;
				}
				if(eId == EntityTypeList.size())
				{
					EntityTypeList.push_back(EntityTypePath);
					EntityQuads.emplace_back();
				}
				
				EntityQuads[eId].emplace_back();
				ddnet::CQuad& Quad = EntityQuads[eId].back();
				
				Quad.m_aPoints[0].x = f2fx(pEntities->GetEntityPositionX(SubPath)-16.0f);
				Quad.m_aPoints[0].y = f2fx(pEntities->GetEntityPositionY(SubPath)-16.0f);
				Quad.m_aPoints[1].x = f2fx(pEntities->GetEntityPositionX(SubPath)+16.0f);
				Quad.m_aPoints[1].y = f2fx(pEntities->GetEntityPositionY(SubPath)-16.0f);
				Quad.m_aPoints[2].x = f2fx(pEntities->GetEntityPositionX(SubPath)-16.0f);
				Quad.m_aPoints[2].y = f2fx(pEntities->GetEntityPositionY(SubPath)+16.0f);
				Quad.m_aPoints[3].x = f2fx(pEntities->GetEntityPositionX(SubPath)+16.0f);
				Quad.m_aPoints[3].y = f2fx(pEntities->GetEntityPositionY(SubPath)+16.0f);
				Quad.m_aPoints[4].x = f2fx(pEntities->GetEntityPositionX(SubPath));
				Quad.m_aPoints[4].y = f2fx(pEntities->GetEntityPositionY(SubPath));
				
				for(int j=0; j<4; j++)
				{
					Quad.m_aTexcoords[j].x = 0.0f;
					Quad.m_aTexcoords[j].y = 0.0f;
					Quad.m_aColors[j].r = 255.0f;
					Quad.m_aColors[j].g = 255.0f;
					Quad.m_aColors[j].b = 255.0f;
					Quad.m_aColors[j].a = 255.0f;
				}
				
				Quad.m_PosEnv = -1;
				Quad.m_PosEnvOffset = 0;
				Quad.m_ColorEnv = -1;
				Quad.m_ColorEnvOffset = 0;
			}
		}
		
		for(unsigned int eId=0; eId<EntityTypeList.size(); eId++)
		{
			ddnet::CMapItemLayerQuads LItem;
			LItem.m_Version = 2;
			LItem.m_Layer.m_Type = ddnet::LAYERTYPE_QUADS;
			LItem.m_Layer.m_Flags = 0;
			LItem.m_Image = -1;
			LItem.m_NumQuads = EntityQuads[eId].size();
			LItem.m_Data = ArchiveFile.AddDataSwapped(EntityQuads[eId].size()*sizeof(ddnet::CQuad), &EntityQuads[eId][0]);
			{
				const char* pEntityName = GetAssetValue<const char*>(EntityTypeList[eId], CSubPath::Null(), CAsset_EntityType::NAME, NULL);
				if(pEntityName)
					StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pEntityName);
				else
					StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), "unknown");
			}
			ArchiveFile.AddItem(ddnet::MAPITEMTYPE_LAYER, LayerId++, sizeof(ddnet::CMapItemLayerQuads), &LItem);
		}
		
		ddnet::CMapItemGroup GItem;
		GItem.m_Version = ddnet::CMapItemGroup::CURRENT_VERSION;
		GItem.m_OffsetX = 0;
		GItem.m_OffsetY = 0;
		GItem.m_ParallaxX = 100;
		GItem.m_ParallaxY = 100;
		GItem.m_StartLayer = StartLayer;
		GItem.m_NumLayers = LayerId-StartLayer;
		GItem.m_UseClipping = 1;
		GItem.m_ClipX = 0;
		GItem.m_ClipY = 0;
		GItem.m_ClipW = 0;
		GItem.m_ClipH = 0;
		StrToInts(GItem.m_aName, sizeof(GItem.m_aName)/sizeof(int), "#Entities");
		ArchiveFile.AddItem(ddnet::MAPITEMTYPE_GROUP, GroupId++, sizeof(ddnet::CMapItemGroup), &GItem);
	}
	
	//Step6: Save images
	{
		for(unsigned int i=0; i<Images.size(); i++)
		{
			ddnet::CMapItemImage IItem;
			IItem.m_Version = 1;
			IItem.m_Width = 0;
			IItem.m_Height = 0;
			IItem.m_External = 1;
			IItem.m_ImageName = -1;
			IItem.m_ImageData = -1;
			
			const CAsset_Image* pImage = GetAsset<CAsset_Image>(Images[i]);
			if(pImage)
			{
				IItem.m_Width = pImage->GetDataWidth();
				IItem.m_Height = pImage->GetDataHeight();
				
				if(str_comp(GetPackageName(Images[i].GetPackageId()), "env_clouds") == 0)
				{
					if(str_comp(pImage->GetName(), "cloud1") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("bg_cloud1")+1, (char*) "bg_cloud1");
					if(str_comp(pImage->GetName(), "cloud2") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("bg_cloud2")+1, (char*) "bg_cloud2");
					if(str_comp(pImage->GetName(), "cloud3") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("bg_cloud3")+1, (char*) "bg_cloud3");
				}
				else if(str_comp(GetPackageName(Images[i].GetPackageId()), "env_desert") == 0)
				{
					if(str_comp(pImage->GetName(), "desertMain") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("desert_main")+1, (char*) "desert_main");
					if(str_comp(pImage->GetName(), "desertDoodads") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("desert_doodads")+1, (char*) "desert_doodads");
					if(str_comp(pImage->GetName(), "desertMountains1") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("desert_mountains")+1, (char*) "desert_mountains");
					if(str_comp(pImage->GetName(), "desertMountains2") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("desert_mountains2")+1, (char*) "desert_mountains2");
					if(str_comp(pImage->GetName(), "desertSun") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("desert_sun")+1, (char*) "desert_sun");
				}
				else if(str_comp(GetPackageName(Images[i].GetPackageId()), "env_generic") == 0)
				{
					if(str_comp(pImage->GetName(), "genericSpikes") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("generic_deathtiles")+1, (char*) "generic_deathtiles");
					if(str_comp(pImage->GetName(), "genericUnhookable") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("generic_unhookable")+1, (char*) "generic_unhookable");
				}
				else if(str_comp(GetPackageName(Images[i].GetPackageId()), "env_grass") == 0)
				{
					if(str_comp(pImage->GetName(), "grassMain") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("grass_main")+1, (char*) "grass_main");
					if(str_comp(pImage->GetName(), "grassDoodads") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("grass_doodads")+1, (char*) "grass_doodads");
				}
				else if(str_comp(GetPackageName(Images[i].GetPackageId()), "env_jungle") == 0)
				{
					if(str_comp(pImage->GetName(), "jungleMain") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("jungle_main")+1, (char*) "jungle_main");
					if(str_comp(pImage->GetName(), "jungleDoodads") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("jungle_doodads")+1, (char*) "jungle_doodads");
					if(str_comp(pImage->GetName(), "jungleSpikes") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("jungle_deathtiles")+1, (char*) "jungle_deathtiles");
					if(str_comp(pImage->GetName(), "jungleUnhookable") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("jungle_unhookables")+1, (char*) "jungle_unhookables");
					if(str_comp(pImage->GetName(), "jungleBackground") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("jungle_background")+1, (char*) "jungle_background");
					if(str_comp(pImage->GetName(), "jungleMidground") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("jungle_midground")+1, (char*) "jungle_midground");
				}
				else if(str_comp(GetPackageName(Images[i].GetPackageId()), "env_moon") == 0)
				{
					if(str_comp(pImage->GetName(), "moon") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("moon")+1, (char*) "moon");
				}
				else if(str_comp(GetPackageName(Images[i].GetPackageId()), "env_mountains") == 0)
				{
					if(str_comp(pImage->GetName(), "mountains") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("mountains")+1, (char*) "mountains");
				}
				else if(str_comp(GetPackageName(Images[i].GetPackageId()), "env_snow") == 0)
				{
					if(str_comp(pImage->GetName(), "snow") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("snow")+1, (char*) "snow");
				}
				else if(str_comp(GetPackageName(Images[i].GetPackageId()), "env_stars") == 0)
				{
					if(str_comp(pImage->GetName(), "stars") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("stars")+1, (char*) "stars");
				}
				else if(str_comp(GetPackageName(Images[i].GetPackageId()), "env_sun") == 0)
				{
					if(str_comp(pImage->GetName(), "sun") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("sun")+1, (char*) "sun");
				}
				else if(str_comp(GetPackageName(Images[i].GetPackageId()), "env_winter") == 0)
				{
					if(str_comp(pImage->GetName(), "winterMain") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("winter_main")+1, (char*) "winter_main");
					if(str_comp(pImage->GetName(), "winterDoodads") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("winter_doodads")+1, (char*) "winter_doodads");
					if(str_comp(pImage->GetName(), "winterMountains1") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("winter_mountains")+1, (char*) "winter_mountains");
					if(str_comp(pImage->GetName(), "winterMountains2") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("winter_mountains2")+1, (char*) "winter_mountains2");
					if(str_comp(pImage->GetName(), "winterMountains3") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("winter_mountains3")+1, (char*) "winter_mountains3");
				}
				else if(Format == MAPFORMAT_NINSLASH && str_comp(GetPackageName(Images[i].GetPackageId()), "env_lab") == 0)
				{
					if(str_comp(pImage->GetName(), "labMain") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("lab_main")+1, (char*) "lab_main");
					if(str_comp(pImage->GetName(), "labMisc") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("lab")+1, (char*) "lab");
					if(str_comp(pImage->GetName(), "labBackground") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("lab_background")+1, (char*) "lab_background");
				}
				else if(Format == MAPFORMAT_NINSLASH && str_comp(GetPackageName(Images[i].GetPackageId()), "env_factory") == 0)
				{
					if(str_comp(pImage->GetName(), "factoryMain") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("factory_main")+1, (char*) "factory_main");
					if(str_comp(pImage->GetName(), "factoryBackground") == 0)
						IItem.m_ImageName = ArchiveFile.AddData(str_length("factory_background")+1, (char*) "factory_background");
				}
				
				if(IItem.m_ImageName == -1)
				{
					IItem.m_External = 0;
					IItem.m_ImageName = ArchiveFile.AddData(str_length(pImage->GetName())+1, (char*) pImage->GetName());
					IItem.m_ImageData = ArchiveFile.AddData(pImage->GetDataArray().get_linear_size(), (char*) pImage->GetDataPtr());
				}
			}
			
			ArchiveFile.AddItem(ddnet::MAPITEMTYPE_IMAGE, i, sizeof(IItem), &IItem);
		}
	}
	
	ArchiveFile.Finish();
	
	return true;
}
