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

#include <shared/components/storage.h>
#include <tw07/shared/datafile.h>
#include <tw07/shared/mapitems.h>

#include "assetsmanager.h"

/* ASSETS MANAGER *****************************************************/

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

	switch(Format)
	{
		case MAPFORMAT_TW:
			Load_UnivTeeWorlds();
			break;
		case MAPFORMAT_DDNET:
			Load_UnivTeeWorlds();
			Load_UnivDDNet();
			break;
		case MAPFORMAT_INFCLASS:
			Load_UnivTeeWorlds();
			Load_UnivInfClass();
			break;
		case MAPFORMAT_OPENFNG:
			Load_UnivTeeWorlds();
			Load_UnivOpenFNG();
			break;
	}
	
	char aBuf[128];

	CAssetPath MapPath;
	CAsset_Map* pMap = NewAsset_Hard<CAsset_Map>(&MapPath, PackageId);
	pMap->SetName("map");
	
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
					pImage->SetName(pName);
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

			// load group name
			aBuf[0] = 0;

			if(pGItem->m_Version >= 3)
				IntsToStr(pGItem->m_aName, sizeof(pGItem->m_aName)/sizeof(int), aBuf);

			if(!aBuf[0])
				str_format(aBuf, sizeof(aBuf), "group%d", MapGroupPath.GetId());

			pMapGroup->SetName(aBuf);

			for(int l = 0; l < pGItem->m_NumLayers; l++)
			{
				tw07::CMapItemLayer *pLayerItem = (tw07::CMapItemLayer *) ArchiveFile.GetItem(LayersStart+pGItem->m_StartLayer+l, 0, 0);
				if(!pLayerItem)
					continue;

				if(pLayerItem->m_Type == tw07::LAYERTYPE_TILES)
				{
					tw07::CMapItemLayerTilemap *pTilemapItem = (tw07::CMapItemLayerTilemap *)pLayerItem;

					if(pTilemapItem->m_Flags&tw07::TILESLAYERFLAG_GAME)
					{
						Background = false;
							
						//Tiles
						tw07::CTile* pTiles = (tw07::CTile*) ArchiveFile.GetData(pTilemapItem->m_Data);
						int Width = pTilemapItem->m_Width;
						int Height = pTilemapItem->m_Height;
						
						CAssetPath PhysicsZonePath;
						CAsset_MapZoneTiles* pPhysicsZone = NewAsset_Hard<CAsset_MapZoneTiles>(&PhysicsZonePath, PackageId);
						pPhysicsZone->SetName("Physics");
						pPhysicsZone->SetParentPath(MapPath);
						
						if(Format == MAPFORMAT_INFCLASS)
							pPhysicsZone->SetZoneTypePath(m_Path_ZoneType_InfClassPhysics);
						else
							pPhysicsZone->SetZoneTypePath(m_Path_ZoneType_TWPhysics);
						
						{
							array2d< CAsset_MapZoneTiles::CTile, allocator_copy<CAsset_MapZoneTiles::CTile> >& Data = pPhysicsZone->GetTileArray();
							Data.resize(Width, Height);
							
							CSubPath ZoneLayer = CAsset_Map::SubPath_ZoneLayer(pMap->AddZoneLayer());
							pMap->SetZoneLayer(ZoneLayer, PhysicsZonePath);
						}
						
						CAsset_MapEntities* pEntities = NULL;
						if(Format != MAPFORMAT_INFCLASS)
						{
							CAssetPath EntitiesPath;
							pEntities = NewAsset_Hard<CAsset_MapEntities>(&EntitiesPath, PackageId);
							pEntities->SetName("Entities");
							pEntities->SetParentPath(MapPath);
					
							CSubPath MapSubPath = CAsset_Map::SubPath_EntityLayer(pMap->AddEntityLayer());
							pMap->SetEntityLayer(MapSubPath, EntitiesPath);	
						}			
						
						for(int j=0; j<Height; j++)
						{
							for(int i=0; i<Width; i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								
								switch(pTiles[j*Width+i].m_Index)
								{
									case tw07::TILE_SOLID:
										pPhysicsZone->SetTileIndex(TilePath, 1);
										break;
									case tw07::TILE_NOHOOK:
										pPhysicsZone->SetTileIndex(TilePath, 2);
										break;
									default:
										pPhysicsZone->SetTileIndex(TilePath, 0);
										break;
								}
								
								if(pEntities)
								{
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
									}
								}
								
								int Skip = pTiles[j*Width+i].m_Skip;
								for(int s=0; s<Skip; s++)
								{
									pPhysicsZone->SetTileIndex(TilePath, 0);
									i++;
									
									TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								}
							}
						}
						
						if(Format != MAPFORMAT_INFCLASS)
						{
							CAssetPath DamageZonePath;
							CAsset_MapZoneTiles* pDamageZone = NewAsset_Hard<CAsset_MapZoneTiles>(&DamageZonePath, PackageId);
							pDamageZone->SetName("Damage");
							pDamageZone->SetZoneTypePath(m_Path_ZoneType_TWDamage);
							pDamageZone->SetParentPath(MapPath);
							
							{
								array2d< CAsset_MapZoneTiles::CTile, allocator_copy<CAsset_MapZoneTiles::CTile> >& Data = pDamageZone->GetTileArray();
								Data.resize(Width, Height);
								
								CSubPath ZoneLayer = CAsset_Map::SubPath_ZoneLayer(pMap->AddZoneLayer());
								pMap->SetZoneLayer(ZoneLayer, DamageZonePath);
							}			
							
							for(int j=0; j<Height; j++)
							{
								for(int i=0; i<Width; i++)
								{
									CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
									
									switch(pTiles[j*Width+i].m_Index)
									{
										case tw07::TILE_DEATH:
											pDamageZone->SetTileIndex(TilePath, 1);
											break;
										default:
											pDamageZone->SetTileIndex(TilePath, 0);
											break;
									}
									
									int Skip = pTiles[j*Width+i].m_Skip;
									for(int s=0; s<Skip; s++)
									{
										pDamageZone->SetTileIndex(TilePath, 0);
										i++;
										
										TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
									}
								}
							}
						}
						
						//Create a new group to separate background/foreground
						{
							CAsset_MapGroup* pNewGroup = NewAsset_Hard<CAsset_MapGroup>(&MapGroupPath, PackageId);
							
							CSubPath FgGroup = CAsset_Map::SubPath_FgGroup(pMap->AddFgGroup());
							pMap->SetFgGroup(FgGroup, MapGroupPath);
							
							pNewGroup->SetName(pMapGroup->GetName());
							pNewGroup->SetPosition(pMapGroup->GetPosition());
							pNewGroup->SetHardParallax(pMapGroup->GetHardParallax());
							pMapGroup = pNewGroup;
						}
					}
					else if(Format == MAPFORMAT_INFCLASS && pTilemapItem->m_Flags&(tw07::TILESLAYERFLAG_GAME << 1))
					{
						//Tiles
						tw07::CTile* pTiles = (tw07::CTile*) ArchiveFile.GetData(pTilemapItem->m_Data);
						int Width = pTilemapItem->m_Width;
						int Height = pTilemapItem->m_Height;
						
						CAssetPath EntitiesPath;
						CAsset_MapEntities* pEntities = NewAsset_Hard<CAsset_MapEntities>(&EntitiesPath, PackageId);
						pEntities->SetName("Spawn");
						pEntities->SetParentPath(MapPath);
					
						CSubPath MapSubPath = CAsset_Map::SubPath_EntityLayer(pMap->AddEntityLayer());
						pMap->SetEntityLayer(MapSubPath, EntitiesPath);				
						
						for(int j=0; j<Height; j++)
						{
							for(int i=0; i<Width; i++)
							{
								switch(pTiles[j*Width+i].m_Index)
								{
									case 1:
									{
										CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
										pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_InfClassInfectedSpawn);
										pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
										break;
									}
									case 2:
									{
										CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
										pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_InfClassHumanSpawn);
										pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
										break;
									}
								}
								
								int Skip = pTiles[j*Width+i].m_Skip;
								for(int s=0; s<Skip; s++)
									i++;
							}
						}
						
						//Tiles
						pTiles = (tw07::CTile*) ArchiveFile.GetData(pTilemapItem->m_Data);
						Width = pTilemapItem->m_Width;
						Height = pTilemapItem->m_Height;
						
						pEntities = NewAsset_Hard<CAsset_MapEntities>(&EntitiesPath, PackageId);
						pEntities->SetName("Flags");
						pEntities->SetParentPath(MapPath);	
					
						MapSubPath = CAsset_Map::SubPath_EntityLayer(pMap->AddEntityLayer());
						pMap->SetEntityLayer(MapSubPath, EntitiesPath);				
						
						for(int j=0; j<Height; j++)
						{
							for(int i=0; i<Width; i++)
							{
								switch(pTiles[j*Width+i].m_Index)
								{
									case 4:
									{
										CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(pEntities->AddEntity());
										pEntities->SetEntityTypePath(EntityPath, m_Path_EntityType_InfClassHeroFlag);
										pEntities->SetEntityPosition(EntityPath, vec2(i*32.0f + 16.0f, j*32.0f + 16.0f));
										break;
									}
								}
								
								int Skip = pTiles[j*Width+i].m_Skip;
								for(int s=0; s<Skip; s++)
									i++;
							}
						}
					}
					else if(Format == MAPFORMAT_INFCLASS && pTilemapItem->m_Flags&(tw07::TILESLAYERFLAG_GAME << 2))
					{
						//Tiles
						tw07::CTile* pTiles = (tw07::CTile*) ArchiveFile.GetData(pTilemapItem->m_Data);
						int Width = pTilemapItem->m_Width;
						int Height = pTilemapItem->m_Height;
						
						CAssetPath PhysicsZonePath;
						CAsset_MapZoneTiles* pInfClassZone = NewAsset_Hard<CAsset_MapZoneTiles>(&PhysicsZonePath, PackageId);
						pInfClassZone->SetName("Zones");
						pInfClassZone->SetZoneTypePath(m_Path_ZoneType_InfClassZones);
						pInfClassZone->SetParentPath(MapPath);
						
						{
							array2d< CAsset_MapZoneTiles::CTile, allocator_copy<CAsset_MapZoneTiles::CTile> >& Data = pInfClassZone->GetTileArray();
							Data.resize(Width, Height);
							
							CSubPath ZoneLayer = CAsset_Map::SubPath_ZoneLayer(pMap->AddZoneLayer());
							pMap->SetZoneLayer(ZoneLayer, PhysicsZonePath);
						}
							
						for(int j=0; j<Height; j++)
						{
							for(int i=0; i<Width; i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								
								switch(pTiles[j*Width+i].m_Index)
								{
									case 2:
										pInfClassZone->SetTileIndex(TilePath, 2);
										break;
									case 3:
										pInfClassZone->SetTileIndex(TilePath, 3);
										break;
									default:
										pInfClassZone->SetTileIndex(TilePath, 0);
										break;
								}
								
								int Skip = pTiles[j*Width+i].m_Skip;
								for(int s=0; s<Skip; s++)
								{
									pInfClassZone->SetTileIndex(TilePath, 0);
									i++;
									
									TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								}
							}
						}
						
						CAssetPath DamageZonePath;
						CAsset_MapZoneTiles* pDamageZone = NewAsset_Hard<CAsset_MapZoneTiles>(&DamageZonePath, PackageId);
						pDamageZone->SetName("Damage");
						pDamageZone->SetZoneTypePath(m_Path_ZoneType_TWDamage);
						pDamageZone->SetParentPath(MapPath);
						
						{
							array2d< CAsset_MapZoneTiles::CTile, allocator_copy<CAsset_MapZoneTiles::CTile> >& Data = pDamageZone->GetTileArray();
							Data.resize(Width, Height);
							
							CSubPath ZoneLayer = CAsset_Map::SubPath_ZoneLayer(pMap->AddZoneLayer());
							pMap->SetZoneLayer(ZoneLayer, DamageZonePath);
						}			
						
						for(int j=0; j<Height; j++)
						{
							for(int i=0; i<Width; i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								
								switch(pTiles[j*Width+i].m_Index)
								{
									case 1:
										pDamageZone->SetTileIndex(TilePath, 1);
										break;
									default:
										pDamageZone->SetTileIndex(TilePath, 0);
										break;
								}
								
								int Skip = pTiles[j*Width+i].m_Skip;
								for(int s=0; s<Skip; s++)
								{
									pDamageZone->SetTileIndex(TilePath, 0);
									i++;
									
									TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
								}
							}
						}
					}
					else
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
						
						pMapLayer->SetName(aBuf);
										
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

					pMapLayer->SetName(aBuf);
					
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

	delete[] pImagePath;
	
	for(int i=0; i<GetNumAssets<CAsset_Image>(PackageId); i++)
		RequestUpdate(CAssetPath(CAsset_Image::TypeId, PackageId, i));
	
	ArchiveFile.Close();
	
	return PackageId;
}
