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
#include <tw07/shared/datafile.h>
#include <tw07/shared/mapitems.h>
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
	CAssetPath m_DDFreezePath;
	CAssetPath m_OpenFNGPath;
	CAssetPath m_UnknownPath;
	
	CAsset_MapZoneTiles* m_pTeeWorldsZone;
	CAsset_MapZoneTiles* m_pDDGameZone;
	CAsset_MapZoneTiles* m_pDDFreezeZone;
	CAsset_MapZoneTiles* m_pOpenFNGZone;
	CAsset_MapZoneTiles* m_pUnknownZone;
	
	CLoadMap_ZoneList(CAssetsManager* pAssetsManager, CAsset_Map* pMap, CAssetPath MapPath) :
		m_pAssetsManager(pAssetsManager),
		m_pMap(pMap),
		m_MapPath(MapPath),
		m_pTeeWorldsZone(NULL),
		m_pDDGameZone(NULL),
		m_pDDFreezeZone(NULL),
		m_pOpenFNGZone(NULL),
		m_pUnknownZone(NULL)
	{
		
	}
	
	void CreateZone(CAsset_MapZoneTiles** ppZone, CAssetPath& ZonePath, const char* pName, CAssetPath ZoneType, int Width, int Height)
	{
		if(*ppZone != NULL)
			return;
		
		*ppZone = m_pAssetsManager->NewAsset_Hard<CAsset_MapZoneTiles>(&ZonePath, m_MapPath.GetPackageId());
		
		m_pAssetsManager->TryChangeAssetName_Hard(ZonePath, pName);
		(*ppZone)->SetParentPath(m_MapPath);
		(*ppZone)->SetZoneTypePath(ZoneType);

		array2d< CAsset_MapZoneTiles::CTile, allocator_copy<CAsset_MapZoneTiles::CTile> >& Data = (*ppZone)->GetTileArray();
		Data.resize(Width, Height);
		
		CSubPath ZoneLayer = CAsset_Map::SubPath_ZoneLayer(m_pMap->AddZoneLayer());
		m_pMap->SetZoneLayer(ZoneLayer, ZonePath);
		
		if(m_pTeeWorldsZone && &m_pTeeWorldsZone != ppZone)
			m_pTeeWorldsZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_TeeWorldsPath);
		if(m_pDDGameZone && &m_pDDGameZone != ppZone)
			m_pDDGameZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_DDGamePath);
		if(m_pDDFreezeZone && &m_pDDFreezeZone != ppZone)
			m_pDDFreezeZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_DDFreezePath);
		if(m_pOpenFNGZone && &m_pOpenFNGZone != ppZone)
			m_pOpenFNGZone = m_pAssetsManager->GetAsset_Hard<CAsset_MapZoneTiles>(m_OpenFNGPath);
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
		FullPath.copy(pFileName);
		
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
			PackageName.copy(pFileName+i);
	}
	else
	{
		FullPath.append("maps/");
		FullPath.append(pFileName);
		FullPath.append(".map");
		
		PackageName.copy(pFileName);
	}
	
	int PackageId = NewPackage(PackageName.buffer());
	
	tw07::CDataFileReader ArchiveFile;
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
	
	tw07::CMapItemVersion *pItem = (tw07::CMapItemVersion *) ArchiveFile.FindItem(tw07::MAPITEMTYPE_VERSION, 0);
	if(!pItem)
		return -1;

	if(pItem->m_Version != tw07::CMapItemVersion::CURRENT_VERSION)
		return -1;

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
		ArchiveFile.GetType(tw07::MAPITEMTYPE_IMAGE, &Start, &Num);
		
		pImagePath = new CAssetPath[Num];

		for(int i = 0; i < Num; i++)
		{
			tw07::CMapItemImage *pItem = (tw07::CMapItemImage *) ArchiveFile.GetItem(Start+i, 0, 0);
			
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
				
				if(pItem->m_Version <= 1 || pItem->m_Format == CImageInfo::FORMAT_RGBA)
				{
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
				}
				else if(pItem->m_Format == CImageInfo::FORMAT_RGB)
				{
					Data.resize(pItem->m_Width, pItem->m_Height, 3);
					
					for(int j=0; j<pItem->m_Height; j++)
					{
						for(int i=0; i<pItem->m_Width; i++)
						{
							Data.set_clamp(i, j, 0, pData[(j*pItem->m_Width+i)*3+0]);
							Data.set_clamp(i, j, 1, pData[(j*pItem->m_Width+i)*3+1]);
							Data.set_clamp(i, j, 2, pData[(j*pItem->m_Width+i)*3+2]);
						}
					}
				}
				else if(pItem->m_Format == CImageInfo::FORMAT_ALPHA)
				{
					Data.resize(pItem->m_Width, pItem->m_Height, 1);
					
					for(int j=0; j<pItem->m_Height; j++)
					{
						for(int i=0; i<pItem->m_Width; i++)
						{
							Data.set_clamp(i, j, 0, pData[j*pItem->m_Width+i]);
						}
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
		ArchiveFile.GetType(tw07::MAPITEMTYPE_LAYER, &LayersStart, &LayersNum);

		int Start, Num;
		ArchiveFile.GetType(tw07::MAPITEMTYPE_GROUP, &Start, &Num);
		for(int g = 0; g < Num; g++)
		{
			tw07::CMapItemGroup *pGItem = (tw07::CMapItemGroup *)ArchiveFile.GetItem(Start+g, 0, 0);

			if(pGItem->m_Version < 1 || pGItem->m_Version > tw07::CMapItemGroup::CURRENT_VERSION)
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
					tw07::CMapItemLayer *pLayerItem = (tw07::CMapItemLayer *) ArchiveFile.GetItem(LayersStart+pGItem->m_StartLayer+l, 0, 0);
					if(!pLayerItem)
						continue;
					
					if(pLayerItem->m_Type != tw07::LAYERTYPE_QUADS)
						continue;
					
					tw07::CMapItemLayerQuads *pQuadsItem = (tw07::CMapItemLayerQuads *)pLayerItem;
					
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
					tw07::CQuad *pQuads = (tw07::CQuad *) ArchiveFile.GetDataSwapped(pQuadsItem->m_Data);
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
					tw07::CMapItemLayer *pLayerItem = (tw07::CMapItemLayer *) ArchiveFile.GetItem(LayersStart+pGItem->m_StartLayer+l, 0, 0);
					if(!pLayerItem)
						continue;
					
					if(pLayerItem->m_Type == tw07::LAYERTYPE_TILES)
					{
						tw07::CMapItemLayerTilemap *pTilemapItem = (tw07::CMapItemLayerTilemap *)pLayerItem;
							
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
						tw07::CTile* pTiles = (tw07::CTile*) ArchiveFile.GetData(pTilemapItem->m_Data);
						int Width = pTilemapItem->m_Width;
						int Height = pTilemapItem->m_Height;
						
						array2d< CAsset_MapZoneTiles::CTile, allocator_copy<CAsset_MapZoneTiles::CTile> >& Data = pZone->GetTileArray();
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
					tw07::CMapItemLayer *pLayerItem = (tw07::CMapItemLayer *) ArchiveFile.GetItem(LayersStart+pGItem->m_StartLayer+l, 0, 0);
					if(!pLayerItem)
						continue;

					if(pLayerItem->m_Type == tw07::LAYERTYPE_TILES)
					{
						tw07::CMapItemLayerTilemap *pTilemapItem = (tw07::CMapItemLayerTilemap *)pLayerItem;

						if(pTilemapItem->m_Flags == tw07::TILESLAYERFLAG_GAME)
						{
							Background = false;
								
							//Tiles
							tw07::CTile* pTiles = (tw07::CTile*) ArchiveFile.GetData(pTilemapItem->m_Data);
							int Width = pTilemapItem->m_Width;
							int Height = pTilemapItem->m_Height;
							
							pMap->SetCameraPosition(vec2(Width*32.0f, Height*32.0f)/2.0f);
							
							CAssetPath EntitiesPath;
							CAsset_MapEntities* pEntities = NewAsset_Hard<CAsset_MapEntities>(&EntitiesPath, PackageId);
							AssetsManager()->TryChangeAssetName_Hard(EntitiesPath, "entities");
							pEntities->SetParentPath(MapPath);
					
							CSubPath MapSubPath = CAsset_Map::SubPath_EntityLayer(pMap->AddEntityLayer());
							pMap->SetEntityLayer(MapSubPath, EntitiesPath);			
							
							for(int j=0; j<Height; j++)
							{
								for(int i=0; i<Width; i++)
								{
									CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
									
									bool WriteInUnknown = false;
									
									switch(pTiles[j*Width+i].m_Index)
									{
										case tw07::ENTITY_SPAWN + tw07::ENTITY_OFFSET:
										{
											CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
											pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_TWSpawn);
											pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
											break;
										}
										case tw07::ENTITY_SPAWN_RED + tw07::ENTITY_OFFSET:
										{
											CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
											pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_TWSpawnRed);
											pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
											break;
										}
										case tw07::ENTITY_SPAWN_BLUE + tw07::ENTITY_OFFSET:
										{
											CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
											pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_TWSpawnBlue);
											pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
											break;
										}
										case tw07::ENTITY_HEALTH_1 + tw07::ENTITY_OFFSET:
										{
											CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
											pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_TWHeart);
											pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
											break;
										}
										case tw07::ENTITY_ARMOR_1 + tw07::ENTITY_OFFSET:
										{
											CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
											pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_TWArmor);
											pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
											break;
										}
										case tw07::ENTITY_POWERUP_NINJA + tw07::ENTITY_OFFSET:
										{
											CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
											pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_TWNinja);
											pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
											break;
										}
										case tw07::ENTITY_WEAPON_GRENADE + tw07::ENTITY_OFFSET:
										{
											CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
											pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_TWGrenade);
											pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
											break;
										}
										case tw07::ENTITY_WEAPON_LASER + tw07::ENTITY_OFFSET:
										{
											CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
											pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_TWLaserRifle);
											pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
											break;
										}
										case tw07::ENTITY_WEAPON_SHOTGUN + tw07::ENTITY_OFFSET:
										{
											CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
											pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_TWShotgun);
											pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
											break;
										}
										case tw07::ENTITY_FLAGSTAND_BLUE + tw07::ENTITY_OFFSET:
										{
											CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
											pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_TWFlagBlue);
											pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
											break;
										}
										case tw07::ENTITY_FLAGSTAND_RED + tw07::ENTITY_OFFSET:
										{
											CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
											pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_TWFlagRed);
											pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
											break;
										}
										case tw07::TILE_AIR:
										case tw07::TILE_SOLID:
										case tw07::TILE_DEATH:
										case tw07::TILE_NOHOOK:
										{
											Zones.CreateZone(&Zones.m_pTeeWorldsZone, Zones.m_TeeWorldsPath, "teeworlds", m_Path_ZoneType_TeeWorlds, Width, Height);
											if(Zones.m_pTeeWorldsZone)
												Zones.m_pTeeWorldsZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index);
											else
												WriteInUnknown = true;
											break;
										}
										//OpenFNG and DDNet
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
										case 9:
										{
											if(Format == MAPFORMAT_DDNET)
											{
												Load_UnivDDNet();
												Zones.CreateZone(&Zones.m_pDDFreezeZone, Zones.m_DDFreezePath, "freeze", m_Path_ZoneType_DDFreeze, Width, Height);
												if(Zones.m_pDDFreezeZone)
													Zones.m_pDDFreezeZone->SetTileIndex(TilePath, 1);
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
										case 11:
										case 12:
										{
											if(Format == MAPFORMAT_DDNET)
											{
												Load_UnivDDNet();
												Zones.CreateZone(&Zones.m_pDDFreezeZone, Zones.m_DDFreezePath, "freeze", m_Path_ZoneType_DDFreeze, Width, Height);
												if(Zones.m_pDDFreezeZone)
													Zones.m_pDDFreezeZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index-9);
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
										case 13:
										{
											if(Format == MAPFORMAT_DDNET)
											{
												Load_UnivDDNet();
												Zones.CreateZone(&Zones.m_pDDFreezeZone, Zones.m_DDFreezePath, "freeze", m_Path_ZoneType_DDFreeze, Width, Height);
												if(Zones.m_pDDFreezeZone)
													Zones.m_pDDFreezeZone->SetTileIndex(TilePath, pTiles[j*Width+i].m_Index-9);
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
									
									if(WriteInUnknown)
									{
										if(Format == MAPFORMAT_DDNET)
										{
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
							
							array2d< CAsset_MapLayerTiles::CTile, allocator_copy<CAsset_MapLayerTiles::CTile> >& Data = pMapLayer->GetTileArray();
							Data.resize(Width, Height);
							
							tw07::CTile* pTiles = (tw07::CTile*) ArchiveFile.GetData(pTilemapItem->m_Data);
							
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
								SetAssetValue_Hard<int>(pImagePath[pTilemapItem->m_Image], CSubPath::Null(), CAsset_Image::GRIDSPACING, 1);
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
							tw07::CTile* pTiles = (tw07::CTile*) ArchiveFile.GetData(pTilemapItem->m_Data);
							int Width = pTilemapItem->m_Width;
							int Height = pTilemapItem->m_Height;
							
							array2d< CAsset_MapZoneTiles::CTile, allocator_copy<CAsset_MapZoneTiles::CTile> >& Data = pZone->GetTileArray();
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
					else if(pLayerItem->m_Type == tw07::LAYERTYPE_QUADS)
					{
						tw07::CMapItemLayerQuads *pQuadsItem = (tw07::CMapItemLayerQuads *)pLayerItem;

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
						tw07::CQuad *pQuads = (tw07::CQuad *) ArchiveFile.GetDataSwapped(pQuadsItem->m_Data);
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
							
							pMapLayer->SetQuadColor0(QuadPath, IntsToColor(pQuads[i].m_aColors[0].r, pQuads[i].m_aColors[0].g, pQuads[i].m_aColors[0].b, pQuads[i].m_aColors[0].a));
							pMapLayer->SetQuadColor1(QuadPath, IntsToColor(pQuads[i].m_aColors[1].r, pQuads[i].m_aColors[1].g, pQuads[i].m_aColors[1].b, pQuads[i].m_aColors[1].a));
							pMapLayer->SetQuadColor2(QuadPath, IntsToColor(pQuads[i].m_aColors[2].r, pQuads[i].m_aColors[2].g, pQuads[i].m_aColors[2].b, pQuads[i].m_aColors[2].a));
							pMapLayer->SetQuadColor3(QuadPath, IntsToColor(pQuads[i].m_aColors[3].r, pQuads[i].m_aColors[3].g, pQuads[i].m_aColors[3].b, pQuads[i].m_aColors[3].a));
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

void CAssetsManager::Save_Map_Group(tw07::CDataFileWriter& ArchiveFile, const CAssetPath& GroupPath, array<CAssetPath>& Images, int& GroupId, int& LayerId)
{
	const CAsset_MapGroup* pGroup = GetAsset<CAsset_MapGroup>(GroupPath);
	if(!pGroup)
		return;
	
	int StartLayer = LayerId;
	
	CAsset_MapGroup::CIteratorLayer LayerIter;
	for(LayerIter = pGroup->BeginLayer(); LayerIter != pGroup->EndLayer(); ++LayerIter)
	{
		CAssetPath LayerPath = pGroup->GetLayer(*LayerIter);
		
		if(LayerPath.GetType() == CAsset_MapLayerTiles::TypeId)
		{
			const CAsset_MapLayerTiles* pLayer = GetAsset<CAsset_MapLayerTiles>(LayerPath);
			if(!pLayer)
				continue;
			
			int Width = pLayer->GetTileWidth();
			int Height = pLayer->GetTileHeight();
			tw07::CTile* pTiles = new tw07::CTile[Width*Height];
			
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
				tw07::CMapItemLayerTilemap LItem;
				
				LItem.m_Version = 3;
				LItem.m_Flags = 0;
				LItem.m_Layer.m_Type = tw07::LAYERTYPE_TILES;
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
				LItem.m_Data = ArchiveFile.AddData(Width*Height*sizeof(tw07::CTile), pTiles);
				StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pLayer->GetName());
			
				const CAsset_Image* pImage = GetAsset<CAsset_Image>(pLayer->GetImagePath());
				if(pImage)
				{
					for(int i=0; i<Images.size(); i++)
					{
						if(Images[i] == pLayer->GetImagePath())
						{
							LItem.m_Image = i;
							break;
						}
					}
					
					if(LItem.m_Image == -1)
					{
						LItem.m_Image = Images.size();
						Images.increment() = pLayer->GetImagePath();
					}
				}
				
				ArchiveFile.AddItem(tw07::MAPITEMTYPE_LAYER, LayerId++, sizeof(tw07::CMapItemLayerTilemap), &LItem);
			}
			
			delete[] pTiles;
		}
		else if(LayerPath.GetType() == CAsset_MapLayerQuads::TypeId)
		{
			const CAsset_MapLayerQuads* pLayer = GetAsset<CAsset_MapLayerQuads>(LayerPath);
			if(!pLayer)
				continue;
			
			tw07::CQuad* pQuads = new tw07::CQuad[pLayer->GetQuadArraySize()];
			
			for(int i=0; i<pLayer->GetQuadArraySize(); i++)
			{
				const CAsset_MapLayerQuads::CQuad& Quad = pLayer->GetQuad(CAsset_MapLayerQuads::SubPath_Quad(i));
				
				vec2 Pos;
				
				Pos = rotate(Quad.GetVertex0() * Quad.GetSize(), Quad.GetAngle()) + Quad.GetPivot();
				pQuads[i].m_aPoints[0].x = f2fx(Pos.x);
				pQuads[i].m_aPoints[0].y = f2fx(Pos.y);
				pQuads[i].m_aTexcoords[0].x = f2fx(Quad.GetUV0X());
				pQuads[i].m_aTexcoords[0].y = f2fx(Quad.GetUV0Y());
				pQuads[i].m_aColors[0].r = Quad.GetColor0().r*255.0f;
				pQuads[i].m_aColors[0].g = Quad.GetColor0().g*255.0f;
				pQuads[i].m_aColors[0].b = Quad.GetColor0().b*255.0f;
				pQuads[i].m_aColors[0].a = Quad.GetColor0().a*255.0f;
				
				Pos = rotate(Quad.GetVertex1() * Quad.GetSize(), Quad.GetAngle()) + Quad.GetPivot();
				pQuads[i].m_aPoints[1].x = f2fx(Pos.x);
				pQuads[i].m_aPoints[1].y = f2fx(Pos.y);
				pQuads[i].m_aTexcoords[1].x = f2fx(Quad.GetUV1X());
				pQuads[i].m_aTexcoords[1].y = f2fx(Quad.GetUV1Y());
				pQuads[i].m_aColors[1].r = Quad.GetColor1().r*255.0f;
				pQuads[i].m_aColors[1].g = Quad.GetColor1().g*255.0f;
				pQuads[i].m_aColors[1].b = Quad.GetColor1().b*255.0f;
				pQuads[i].m_aColors[1].a = Quad.GetColor1().a*255.0f;
				
				Pos = rotate(Quad.GetVertex2() * Quad.GetSize(), Quad.GetAngle()) + Quad.GetPivot();
				pQuads[i].m_aPoints[2].x = f2fx(Pos.x);
				pQuads[i].m_aPoints[2].y = f2fx(Pos.y);
				pQuads[i].m_aTexcoords[2].x = f2fx(Quad.GetUV2X());
				pQuads[i].m_aTexcoords[2].y = f2fx(Quad.GetUV2Y());
				pQuads[i].m_aColors[2].r = Quad.GetColor2().r*255.0f;
				pQuads[i].m_aColors[2].g = Quad.GetColor2().g*255.0f;
				pQuads[i].m_aColors[2].b = Quad.GetColor2().b*255.0f;
				pQuads[i].m_aColors[2].a = Quad.GetColor2().a*255.0f;
				
				Pos = rotate(Quad.GetVertex3() * Quad.GetSize(), Quad.GetAngle()) + Quad.GetPivot();
				pQuads[i].m_aPoints[3].x = f2fx(Pos.x);
				pQuads[i].m_aPoints[3].y = f2fx(Pos.y);
				pQuads[i].m_aTexcoords[3].x = f2fx(Quad.GetUV3X());
				pQuads[i].m_aTexcoords[3].y = f2fx(Quad.GetUV3Y());
				pQuads[i].m_aColors[3].r = Quad.GetColor3().r*255.0f;
				pQuads[i].m_aColors[3].g = Quad.GetColor3().g*255.0f;
				pQuads[i].m_aColors[3].b = Quad.GetColor3().b*255.0f;
				pQuads[i].m_aColors[3].a = Quad.GetColor3().a*255.0f;
				
				pQuads[i].m_aPoints[4].x = f2fx(Quad.GetPivotX());
				pQuads[i].m_aPoints[4].y = f2fx(Quad.GetPivotY());
				
				pQuads[i].m_PosEnv = -1;
				pQuads[i].m_PosEnvOffset = 0;
				pQuads[i].m_ColorEnv = -1;
				pQuads[i].m_ColorEnvOffset = 0;
			}
			
			tw07::CMapItemLayerQuads LItem;
			LItem.m_Version = 2;
			LItem.m_Layer.m_Type = tw07::LAYERTYPE_QUADS;
			LItem.m_Layer.m_Flags = 0;
			LItem.m_Image = -1;
			LItem.m_NumQuads = pLayer->GetQuadArraySize();
			LItem.m_Data = ArchiveFile.AddDataSwapped(pLayer->GetQuadArraySize()*sizeof(tw07::CQuad), pQuads);
			StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pLayer->GetName());
			
			const CAsset_Image* pImage = GetAsset<CAsset_Image>(pLayer->GetImagePath());
			if(pImage)
			{
				for(int i=0; i<Images.size(); i++)
				{
					if(Images[i] == pLayer->GetImagePath())
					{
						LItem.m_Image = i;
						break;
					}
				}
				
				if(LItem.m_Image == -1)
				{
					LItem.m_Image = Images.size();
					Images.increment() = pLayer->GetImagePath();
				}
			}

			ArchiveFile.AddItem(tw07::MAPITEMTYPE_LAYER, LayerId++, sizeof(tw07::CMapItemLayerQuads), &LItem);
			
			delete[] pQuads;
		}
		else if(LayerPath.GetType() == CAsset_MapLayerObjects::TypeId)
		{
			const CAsset_MapLayerObjects* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(LayerPath);
			if(!pLayer)
				return;
			
			array<CTexturedQuad> Quads;
			
			CAsset_MapLayerObjects::CIteratorObject Iter;
			for(Iter = pLayer->BeginObject(); Iter != pLayer->EndObject(); ++Iter)
				GenerateMaterialQuads_Object(AssetsManager(), 0.0f, Quads, pLayer->GetObject(*Iter));
			
			if(Quads.size())
			{
				array<tw07::CQuad> ExportedQuads;
				
				CAssetPath CurrentImagePath;
				for(int i=0; i<Quads.size(); i++)
				{
					if(i>0 && CurrentImagePath != Quads[i].m_ImagePath)
					{
						//Image switch: save the current quads in a layer
						tw07::CMapItemLayerQuads LItem;
						LItem.m_Version = 2;
						LItem.m_Layer.m_Type = tw07::LAYERTYPE_QUADS;
						LItem.m_Layer.m_Flags = 0;
						LItem.m_Image = -1;
						LItem.m_NumQuads = ExportedQuads.size();
						LItem.m_Data = ArchiveFile.AddDataSwapped(ExportedQuads.size()*sizeof(tw07::CQuad), &ExportedQuads[0]);
						StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pLayer->GetName());
						
						const CAsset_Image* pImage = GetAsset<CAsset_Image>(CurrentImagePath);
						if(pImage)
						{
							for(int i=0; i<Images.size(); i++)
							{
								if(Images[i] == CurrentImagePath)
								{
									LItem.m_Image = i;
									break;
								}
							}
							
							if(LItem.m_Image == -1)
							{
								LItem.m_Image = Images.size();
								Images.increment() = CurrentImagePath;
							}
						}

						ArchiveFile.AddItem(tw07::MAPITEMTYPE_LAYER, LayerId++, sizeof(tw07::CMapItemLayerQuads), &LItem);
						ExportedQuads.clear();
					}
					
					if(i==0 || CurrentImagePath != Quads[i].m_ImagePath)
						CurrentImagePath = Quads[i].m_ImagePath;
					
					tw07::CQuad& Quad = ExportedQuads.increment();
					
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
				tw07::CMapItemLayerQuads LItem;
				LItem.m_Version = 2;
				LItem.m_Layer.m_Type = tw07::LAYERTYPE_QUADS;
				LItem.m_Layer.m_Flags = 0;
				LItem.m_Image = -1;
				LItem.m_NumQuads = ExportedQuads.size();
				LItem.m_Data = ArchiveFile.AddDataSwapped(ExportedQuads.size()*sizeof(tw07::CQuad), &ExportedQuads[0]);
				StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pLayer->GetName());
				
				const CAsset_Image* pImage = GetAsset<CAsset_Image>(CurrentImagePath);
				if(pImage)
				{
					for(int i=0; i<Images.size(); i++)
					{
						if(Images[i] == CurrentImagePath)
						{
							LItem.m_Image = i;
							break;
						}
					}
					
					if(LItem.m_Image == -1)
					{
						LItem.m_Image = Images.size();
						Images.increment() = CurrentImagePath;
					}
				}

				ArchiveFile.AddItem(tw07::MAPITEMTYPE_LAYER, LayerId++, sizeof(tw07::CMapItemLayerQuads), &LItem);
			}
		}
	}		
	
	tw07::CMapItemGroup GItem;
	GItem.m_Version = tw07::CMapItemGroup::CURRENT_VERSION;
	GItem.m_OffsetX = pGroup->GetPositionX();
	GItem.m_OffsetY = pGroup->GetPositionY();
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
	ArchiveFile.AddItem(tw07::MAPITEMTYPE_GROUP, GroupId++, sizeof(tw07::CMapItemGroup), &GItem);
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
	
	tw07::CDataFileWriter ArchiveFile;
	
	if(!ArchiveFile.Open(Storage(), StorageType, pFileName))
	{
		dbg_msg("AssetsManager", "Can't export the map at this location: %s", pFileName);
		return false;
	}
	
	Load_UnivTeeWorlds();
	
	array<CAssetPath> Images;
	bool ZoneGroupNeeded = false;
	bool EntityGroupNeeded = false;
	array<CAsset_MapEntities::CEntity> PTUMTeeWorldsEntities;
	
	//Map version
	tw07::CMapItemVersion VerItem;
	VerItem.m_Version = 1;
	ArchiveFile.AddItem(tw07::MAPITEMTYPE_VERSION, 0, sizeof(tw07::CMapItemVersion), &VerItem);
	
	//Map info
	tw07::CMapItemInfo InfoItem;
	InfoItem.m_Version = 1;
	InfoItem.m_MapVersion = ArchiveFile.AddData(str_length(GetPackageVersion(PackageId))+1, (char*) GetPackageVersion(PackageId));
	InfoItem.m_Author = ArchiveFile.AddData(str_length(GetPackageAuthor(PackageId))+1, (char*) GetPackageAuthor(PackageId));
	InfoItem.m_Credits = ArchiveFile.AddData(str_length(GetPackageCredits(PackageId))+1, (char*) GetPackageCredits(PackageId));
	InfoItem.m_License = ArchiveFile.AddData(str_length(GetPackageLicense(PackageId))+1, (char*) GetPackageLicense(PackageId));
	ArchiveFile.AddItem(tw07::MAPITEMTYPE_INFO, 0, sizeof(tw07::CMapItemInfo), &InfoItem);
	
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
	
	//Step2: Save the game layer
	{
		int StartLayer = LayerId;
		
		//Get game layer size
		int Width = 0;
		int Height = 0;
		
		{
			CAsset_Map::CIteratorZoneLayer ZoneIter;
			for(ZoneIter = pMap->BeginZoneLayer(); ZoneIter != pMap->EndZoneLayer(); ++ZoneIter)
			{
				const CAsset_MapZoneTiles* pZone = GetAsset<CAsset_MapZoneTiles>(pMap->GetZoneLayer(*ZoneIter));
				if(!pZone || pZone->GetZoneTypePath() != m_Path_ZoneType_TeeWorlds)
					continue;
				
				Width = max(pZone->GetTileWidth(), Width);
				Height = max(pZone->GetTileHeight(), Height);
			}
		}
		
		tw07::CTile* pTiles = new tw07::CTile[Width*Height];
		
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
	
		//Game layer
		CAsset_Map::CIteratorZoneLayer ZoneIter;
		for(ZoneIter = pMap->BeginZoneLayer(); ZoneIter != pMap->EndZoneLayer(); ++ZoneIter)
		{
			const CAsset_MapZoneTiles* pZone = GetAsset<CAsset_MapZoneTiles>(pMap->GetZoneLayer(*ZoneIter));
			if(!pZone)
				continue;
			
			if(pZone->GetZoneTypePath() != m_Path_ZoneType_TeeWorlds)
			{
				ZoneGroupNeeded = true;
				continue;
			}
				
			for(int j=0; j<pZone->GetTileHeight(); j++)
			{
				for(int i=0; i<pZone->GetTileWidth(); i++)
				{
					CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
					
					switch(pZone->GetTileIndex(TilePath))
					{
						case 1:
							pTiles[j*Width+i].m_Index = tw07::TILE_SOLID;
							break;
						case 2:
							if(pTiles[j*Width+i].m_Index != tw07::TILE_SOLID && pTiles[j*Width+i].m_Index != tw07::TILE_NOHOOK)
								pTiles[j*Width+i].m_Index = tw07::TILE_DEATH;
							break;
						case 3:
							pTiles[j*Width+i].m_Index = tw07::TILE_NOHOOK;
							break;
					}
				}
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
				
				if(X < 0 || X >= Width || Y < 0 || Y >= Height)
				{
					EntityGroupNeeded = true;
					PTUMTeeWorldsEntities.add_by_copy(pEntities->GetEntity(SubPath));
					continue;
				}
				if(pTiles[Y*Width+X].m_Index == tw07::TILE_SOLID || pTiles[Y*Width+X].m_Index == tw07::TILE_NOHOOK)
				{
					EntityGroupNeeded = true;
					PTUMTeeWorldsEntities.add_by_copy(pEntities->GetEntity(SubPath));
					continue;
				}
				if(distance(pEntities->GetEntityPosition(SubPath), vec2(X*32.0f+16.0f, Y*32.0f+16.0f)) > 0.01f)
				{
					EntityGroupNeeded = true;
					PTUMTeeWorldsEntities.add_by_copy(pEntities->GetEntity(SubPath));
					continue;
				}
				
				if(EntityTypePath == m_Path_EntityType_TWSpawn)
					pTiles[Y*Width+X].m_Index = tw07::ENTITY_OFFSET + tw07::ENTITY_SPAWN;
				else if(EntityTypePath == m_Path_EntityType_TWSpawnRed)
					pTiles[Y*Width+X].m_Index = tw07::ENTITY_OFFSET + tw07::ENTITY_SPAWN_RED;
				else if(EntityTypePath == m_Path_EntityType_TWSpawnBlue)
					pTiles[Y*Width+X].m_Index = tw07::ENTITY_OFFSET + tw07::ENTITY_SPAWN_BLUE;
				else if(EntityTypePath == m_Path_EntityType_TWShotgun)
					pTiles[Y*Width+X].m_Index = tw07::ENTITY_OFFSET + tw07::ENTITY_WEAPON_SHOTGUN;
				else if(EntityTypePath == m_Path_EntityType_TWGrenade)
					pTiles[Y*Width+X].m_Index = tw07::ENTITY_OFFSET + tw07::ENTITY_WEAPON_GRENADE;
				else if(EntityTypePath == m_Path_EntityType_TWLaserRifle)
					pTiles[Y*Width+X].m_Index = tw07::ENTITY_OFFSET + tw07::ENTITY_WEAPON_LASER;
				else if(EntityTypePath == m_Path_EntityType_TWNinja)
					pTiles[Y*Width+X].m_Index = tw07::ENTITY_OFFSET + tw07::ENTITY_POWERUP_NINJA;
				else if(EntityTypePath == m_Path_EntityType_TWHeart)
					pTiles[Y*Width+X].m_Index = tw07::ENTITY_OFFSET + tw07::ENTITY_HEALTH_1;
				else if(EntityTypePath == m_Path_EntityType_TWArmor)
					pTiles[Y*Width+X].m_Index = tw07::ENTITY_OFFSET + tw07::ENTITY_ARMOR_1;
				else if(EntityTypePath == m_Path_EntityType_TWFlagBlue)
					pTiles[Y*Width+X].m_Index = tw07::ENTITY_OFFSET + tw07::ENTITY_FLAGSTAND_BLUE;
				else if(EntityTypePath == m_Path_EntityType_TWFlagRed)
					pTiles[Y*Width+X].m_Index = tw07::ENTITY_OFFSET + tw07::ENTITY_FLAGSTAND_RED;
				else
					EntityGroupNeeded = true;
			}
		}
	
		tw07::CMapItemLayerTilemap LItem;
		
		LItem.m_Version = 3;
		LItem.m_Flags = tw07::TILESLAYERFLAG_GAME;
		LItem.m_Layer.m_Type = tw07::LAYERTYPE_TILES;
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
		LItem.m_Data = ArchiveFile.AddData(Width*Height*sizeof(tw07::CTile), pTiles);
		StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), "Game");
		ArchiveFile.AddItem(tw07::MAPITEMTYPE_LAYER, LayerId++, sizeof(tw07::CMapItemLayerTilemap), &LItem);
		
		tw07::CMapItemGroup GItem;
		GItem.m_Version = tw07::CMapItemGroup::CURRENT_VERSION;
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
		ArchiveFile.AddItem(tw07::MAPITEMTYPE_GROUP, GroupId++, sizeof(tw07::CMapItemGroup), &GItem);
		
		delete[] pTiles;
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
	if(ZoneGroupNeeded)
	{
		int StartLayer = LayerId;
		
		CAsset_Map::CIteratorZoneLayer ZoneIter;
		for(ZoneIter = pMap->BeginZoneLayer(); ZoneIter != pMap->EndZoneLayer(); ++ZoneIter)
		{
			const CAsset_MapZoneTiles* pZone = GetAsset<CAsset_MapZoneTiles>(pMap->GetZoneLayer(*ZoneIter));
			if(!pZone || pZone->GetZoneTypePath() == m_Path_ZoneType_TeeWorlds)
				continue;
			
			int Width = pZone->GetTileWidth();
			int Height = pZone->GetTileHeight();
	
			tw07::CTile* pTiles = new tw07::CTile[Width*Height];
			
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
	
			tw07::CMapItemLayerTilemap LItem;
			
			LItem.m_Version = 3;
			LItem.m_Flags = 0;
			LItem.m_Layer.m_Type = tw07::LAYERTYPE_TILES;
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
			LItem.m_Data = ArchiveFile.AddData(Width*Height*sizeof(tw07::CTile), pTiles);
			
			{
				const char* pZoneName = GetAssetValue<const char*>(pZone->GetZoneTypePath(), CSubPath::Null(), CAsset_ZoneType::NAME, NULL);
				if(pZoneName)
					StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pZoneName);
				else
					StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), "unknown");
			}
			
			ArchiveFile.AddItem(tw07::MAPITEMTYPE_LAYER, LayerId++, sizeof(tw07::CMapItemLayerTilemap), &LItem);
			
			delete[] pTiles;
		}
			
		tw07::CMapItemGroup GItem;
		GItem.m_Version = tw07::CMapItemGroup::CURRENT_VERSION;
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
		ArchiveFile.AddItem(tw07::MAPITEMTYPE_GROUP, GroupId++, sizeof(tw07::CMapItemGroup), &GItem);
	}
	
	//Step5: Save other entities in PTUM format
	if(EntityGroupNeeded)
	{
		int StartLayer = LayerId;
		
		array<CAssetPath> EntityTypeList;
		array< array<tw07::CQuad>, allocator_copy< array<tw07::CQuad> > > EntityQuads;
		
		for(int i=0; i<PTUMTeeWorldsEntities.size(); i++)
		{
			CAsset_MapEntities::CEntity& Entity = PTUMTeeWorldsEntities[i];
			CAssetPath EntityTypePath = Entity.GetTypePath();
			
			//Search in previous layers
			int eId;
			for(eId=0; eId<EntityTypeList.size(); eId++)
			{
				if(EntityTypeList[eId] == EntityTypePath)
					break;
			}
			if(eId == EntityTypeList.size())
			{
				EntityTypeList.add_by_copy(EntityTypePath);
				EntityQuads.increment();
			}
			
			tw07::CQuad& Quad = EntityQuads[eId].increment();
			
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
				int eId;
				for(eId=0; eId<EntityTypeList.size(); eId++)
				{
					if(EntityTypeList[eId] == EntityTypePath)
						break;
				}
				if(eId == EntityTypeList.size())
				{
					EntityTypeList.add_by_copy(EntityTypePath);
					EntityQuads.increment();
				}
				
				tw07::CQuad& Quad = EntityQuads[eId].increment();
				
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
		
		for(int	eId=0; eId<EntityTypeList.size(); eId++)
		{
			tw07::CMapItemLayerQuads LItem;
			LItem.m_Version = 2;
			LItem.m_Layer.m_Type = tw07::LAYERTYPE_QUADS;
			LItem.m_Layer.m_Flags = 0;
			LItem.m_Image = -1;
			LItem.m_NumQuads = EntityQuads[eId].size();
			LItem.m_Data = ArchiveFile.AddDataSwapped(EntityQuads[eId].size()*sizeof(tw07::CQuad), &EntityQuads[eId][0]);
			{
				const char* pEntityName = GetAssetValue<const char*>(EntityTypeList[eId], CSubPath::Null(), CAsset_EntityType::NAME, NULL);
				if(pEntityName)
					StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), pEntityName);
				else
					StrToInts(LItem.m_aName, sizeof(LItem.m_aName)/sizeof(int), "unknown");
			}
			ArchiveFile.AddItem(tw07::MAPITEMTYPE_LAYER, LayerId++, sizeof(tw07::CMapItemLayerQuads), &LItem);
		}
		
		tw07::CMapItemGroup GItem;
		GItem.m_Version = tw07::CMapItemGroup::CURRENT_VERSION;
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
		ArchiveFile.AddItem(tw07::MAPITEMTYPE_GROUP, GroupId++, sizeof(tw07::CMapItemGroup), &GItem);
	}
	
	//Step6: Save images
	{
		for(int i=0; i<Images.size(); i++)
		{
			tw07::CMapItemImage IItem;
			IItem.m_Version = tw07::CMapItemImage::CURRENT_VERSION;
			IItem.m_Width = 0;
			IItem.m_Height = 0;
			IItem.m_External = 1;
			IItem.m_ImageName = -1;
			IItem.m_ImageData = -1;
			IItem.m_Format = CImageInfo::FORMAT_RGB;
			
			const CAsset_Image* pImage = GetAsset<CAsset_Image>(Images[i]);
			if(pImage)
			{
				IItem.m_Width = pImage->GetDataWidth();
				IItem.m_Height = pImage->GetDataHeight();
				switch(pImage->GetDataArray().get_depth())
				{
					case 1:
						IItem.m_Format = CImageInfo::FORMAT_ALPHA;
						break;
					case 3:
						IItem.m_Format = CImageInfo::FORMAT_RGB;
						break;
					case 4:
						IItem.m_Format = CImageInfo::FORMAT_RGBA;
						break;
				}
				
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
				
				if(IItem.m_ImageName == -1)
				{
					IItem.m_External = 0;
					IItem.m_ImageName = ArchiveFile.AddData(str_length(pImage->GetName())+1, (char*) pImage->GetName());
					IItem.m_ImageData = ArchiveFile.AddData(pImage->GetDataArray().get_linear_size(), (char*) pImage->GetDataPtr());
				}
			}
			
			ArchiveFile.AddItem(tw07::MAPITEMTYPE_IMAGE, i, sizeof(IItem), &IItem);
		}
	}
	
	ArchiveFile.Finish();
	
	return true;
}
