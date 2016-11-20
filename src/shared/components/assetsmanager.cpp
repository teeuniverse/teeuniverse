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
#include <shared/assets/assetssaveloadcontext.h>
#include <shared/archivefile.h>
#include <tw07/shared/datafile.h>
#include <tw07/shared/mapitems.h>

#include "assetsmanager.h"

/* ASSETS MANAGER *****************************************************/
	
CAssetsManager::CAssetsManager(CSharedKernel* pKernel) :
	CSharedKernel::CComponent(pKernel),
	m_pHistory(0),
	m_PackageId_EnvClouds(-1),
	m_PackageId_EnvDesert(-1),
	m_PackageId_EnvGeneric(-1),
	m_PackageId_EnvGrass(-1),
	m_PackageId_EnvJungle(-1),
	m_PackageId_EnvMoon(-1),
	m_PackageId_EnvMountains(-1),
	m_PackageId_EnvSnow(-1),
	m_PackageId_EnvStars(-1),
	m_PackageId_EnvSun(-1),
	m_PackageId_EnvWinter(-1),
	m_PackageId_UnivTeeWorlds(-1),
	m_PackageId_UnivDDNet(-1),
	m_PackageId_UnivInfClass(-1),
	m_PackageId_UnivOpenFNG(-1)
{
	SetName("AssetsManager");
}

CAssetsManager::~CAssetsManager()
{
	for(int i=0; i<m_pPackages.size(); i++)
	{
		if(m_pPackages[i])
			delete m_pPackages[i];
	}
	m_pPackages.clear();
	
	if(m_pHistory)
		delete m_pHistory;
}

void CAssetsManager::RequestUpdate(const CAssetPath& AssetPath)
{
	if(AssetPath.GetType() == CAsset_Image::TypeId)
	{
		for(int i=0; i<m_ImagesToUpdate.size(); i++)
		{
			if(m_ImagesToUpdate[i].m_Path == AssetPath)
				return;
		}
		
		CAssetUpdateDesc& Desc = m_ImagesToUpdate.increment();
		Desc.m_Path = AssetPath;
		Desc.m_Updated = false;
	}
}

bool CAssetsManager::PostUpdate()
{
	for(int i=m_ImagesToUpdate.size()-1; i>=0; i--)
	{
		if(m_ImagesToUpdate[i].m_Updated)
			m_ImagesToUpdate.remove_index(i);
	}
	
	return true;
}

int CAssetsManager::NewPackage(const char* pName)
{
	//Search for an empty place
	int PackageId = -1;
	CAssetsPackage* pPackage = NULL;
	for(int i=0; i<m_pPackages.size(); i++)
	{
		if(!m_pPackages[i])
		{
			PackageId = i;
			pPackage = new CAssetsPackage();
			m_pPackages[i] = pPackage;
		}
	}
	
	if(PackageId < 0)
	{
		PackageId = m_pPackages.size();
		pPackage = new CAssetsPackage();
		m_pPackages.increment() = pPackage;
	}
	
	if(pName)
		SetPackageName_Hard(PackageId, pName);
		
	return PackageId;
}

const char* CAssetsManager::GetPackageName(int PackageId) const
{
	if(!IsValidPackage(PackageId))
		return "";
	
	return m_pPackages[PackageId]->GetName();
}

void CAssetsManager::SetPackageName(int PackageId, const char* pName)
{
	if(!IsValidPackage(PackageId) || IsReadOnlyPackage(PackageId))
		return;
	
	SetPackageName_Hard(PackageId, pName);
}

void CAssetsManager::SetPackageName_Hard(int PackageId, const char* pName)
{
	if(!IsValidPackage(PackageId) || !pName)
		return;
	
	char aNumBuffer[32];
	dynamic_string Buffer;
	Buffer.copy(pName);
	
	int NumDuplicates = 0;
	bool DuplicateFound = true;
	while(DuplicateFound)
	{
		DuplicateFound = false;
		for(int i=0; i<m_pPackages.size(); i++)
		{
			if(m_pPackages[i] && i != PackageId)
			{
				if(Buffer == m_pPackages[i]->GetName())
				{
					str_format(aNumBuffer, sizeof(aNumBuffer), "%d", NumDuplicates+2);
					NumDuplicates++;
					
					int BufferIter = 0;
					
					Buffer.clear();
					BufferIter = Buffer.append_at(BufferIter, pName);
					BufferIter = Buffer.append_at(BufferIter, "_");
					BufferIter = Buffer.append_at(BufferIter, aNumBuffer);
					DuplicateFound = true;
					break;
				}
			}
		}
	}
	
	m_pPackages[PackageId]->SetIdentity(Buffer.buffer());
}

void CAssetsManager::TryChangeAssetName(CAssetPath AssetPath, const char* pName, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return;
	
	char aNumBuffer[32];
	dynamic_string Buffer;
	Buffer.copy(pName);
	
	int NumDuplicates = 0;
	bool DuplicateFound = true;
		
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
		while(DuplicateFound)\
		{\
			DuplicateFound = false;\
			for(int i=0; i<GetNumAssets<CAsset_##Name>(AssetPath.GetPackageId()); i++)\
			{\
				const CAsset_##Name* pAsset = GetAsset<CAsset_##Name>(CAssetPath(CAsset_##Name::TypeId, AssetPath.GetPackageId(), i));\
				if(pAsset)\
				{\
					if(Buffer == pAsset->GetName())\
					{\
						str_format(aNumBuffer, sizeof(aNumBuffer), "%d", NumDuplicates+2);\
						NumDuplicates++;\
						int BufferIter = 0;\
						Buffer.clear();\
						BufferIter = Buffer.append_at(BufferIter, pName);\
						BufferIter = Buffer.append_at(BufferIter, "_");\
						BufferIter = Buffer.append_at(BufferIter, aNumBuffer);\
						DuplicateFound = true;\
						break;\
					}\
				}\
			}\
		}\
		SetAssetValue<const char*>(AssetPath, CSubPath::Null(), CAsset::NAME, Buffer.buffer(), Token);\
		break;

	switch(AssetPath.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}

	#undef MACRO_ASSETTYPE
}

int CAssetsManager::GetNumPackages() const
{
	return m_pPackages.size();
}

bool CAssetsManager::IsValidPackage(int PackageId) const
{
	return (PackageId >= 0 && PackageId < m_pPackages.size() && m_pPackages[PackageId]);
}

bool CAssetsManager::IsReadOnlyPackage(int PackageId) const
{
	return m_pPackages[PackageId]->IsReadOnly();
}

void CAssetsManager::SetPackageReadOnly(int PackageId , bool Value)
{
	if(IsValidPackage(PackageId))
		m_pPackages[PackageId]->SetReadOnly(Value);
}

void CAssetsManager::ClosePackage(int PackageId)
{
	if(!IsValidPackage(PackageId))
		return;
	
	delete m_pPackages[PackageId];
	m_pPackages[PackageId] = NULL;
}

int CAssetsManager::AddNameToResolve(const char* pName)
{
	for(int i=0; i<m_pNamesToResolved.size(); i++)
	{
		if(str_comp(pName, m_pNamesToResolved[i]) == 0)
			return i;
	}
	
	char*& pStoredName = m_pNamesToResolved.increment();
	int MemorySize = str_length(pName)+1;
	pStoredName = new char[MemorySize];
	str_copy(pStoredName, pName, MemorySize);
	
	return m_pNamesToResolved.size()-1;
}

bool CAssetsManager::Save_AssetsFile(const char* pFilename, int PackageId)
{
	if(!IsValidPackage(PackageId))
		return false;	
	
	dynamic_string Buffer;
	Buffer.append("assets/");
	Buffer.append(pFilename);
	Buffer.append(".assets");
	
	CArchiveFile ArchiveFile;
	CAssetsSaveLoadContext SaveLoadContext(this, &ArchiveFile, PackageId);
	
	m_pPackages[PackageId]->Save_AssetsFile(&SaveLoadContext);
	
	if(!ArchiveFile.Write(Storage(), Buffer.buffer()))
	{
		dbg_msg("AssetsManager", "can't write the file %s", Buffer.buffer());
		return false;
	}
	
	return true;
}

bool CAssetsManager::Save_AssetsFile(int PackageId)
{
	if(!IsValidPackage(PackageId))
		return false;
	
	return Save_AssetsFile(m_pPackages[PackageId]->GetName(), PackageId);
}

int CAssetsManager::Load_AssetsFile_Core(const char* pFileName, int StorageType, unsigned Crc)
{
	int PackageId = -1;
	CAssetsPackage* pPackage = NULL;
	
	dynamic_string Buffer;
	Buffer.append("assets/");
	Buffer.append(pFileName);
	Buffer.append(".assets");
	
	CArchiveFile ArchiveFile;
	if(!ArchiveFile.Open(Storage(), Buffer.buffer(), StorageType))
	{
		dbg_msg("AssetsManager", "can't open the file %s (storage type:%d)", pFileName, StorageType);
		return PackageId;
	}
	
	if(Crc != 0 && ArchiveFile.Crc() != Crc)
	{
		dbg_msg("AssetsManager", "wrong crc for the file %s (%d != %d)", pFileName, Crc, ArchiveFile.Crc());
		return PackageId;
	}
	
	for(int i=0; i<m_pPackages.size(); i++)
	{
		if(m_pPackages[i] && m_pPackages[i]->Identify(pFileName, Crc))
		{
			pPackage = m_pPackages[i];
			PackageId = i;
			
			if(pPackage->IsJustLoaded())
				return PackageId;
		}
	}
	
	if(!pPackage)
	{
		PackageId = NewPackage(NULL);
		pPackage = m_pPackages[PackageId];
		pPackage->SetIdentity(pFileName, ArchiveFile.Crc());
	}
	
	CAssetsSaveLoadContext SaveLoadContext(this, &ArchiveFile, PackageId);
	
	int NumDependencies = ArchiveFile.GetNumItems(1);
	for(int i = 0; i < NumDependencies; i++)
	{
		CAssetsPackage::CTuaType_Dependency* pItem = (CAssetsPackage::CTuaType_Dependency*) ArchiveFile.GetItem(1, i);
		const char* pPackageName = ArchiveFile.GetString(pItem->m_PackageName);
		uint32 PackageCrc = ArchiveFile.ReadUInt32(pItem->m_PackageCrc);
		if(pPackageName != NULL)
		{
			dbg_msg("DEBUG", "Dependency found: %s", pPackageName);
			SaveLoadContext.AddDependency(Load_AssetsFile_Core(pPackageName, PackageCrc));
		}
	}
	
	if(!pPackage->IsLoaded())
		pPackage->Load_AssetsFile(&SaveLoadContext);
	
	for(int i=0; i<GetNumAssets<CAsset_Image>(PackageId); i++)
		RequestUpdate(CAssetPath(CAsset_Image::TypeId, PackageId, i));
	
	return PackageId;
}

int CAssetsManager::Load_AssetsFile(const char* pFileName, int StorageType, unsigned Crc)
{
	//Load all dependencies
	int PackageId = Load_AssetsFile_Core(pFileName, StorageType, Crc);
	
	//Then resolve assets name and finalize
	for(int i=0; i<m_pPackages.size(); i++)
	{
		if(m_pPackages[i] && m_pPackages[i]->IsJustLoaded())
		{
			m_pPackages[i]->AssetPathOperation(CAssetPath::COperation::ResolveName(this, &m_pNamesToResolved));
			m_pPackages[i]->LoadingDone();
		}
	}
	
	for(int i=0; i<m_pNamesToResolved.size(); i++)
	{
		if(m_pNamesToResolved[i])
			delete[] m_pNamesToResolved[i];
	}
	m_pNamesToResolved.clear();
	
	return PackageId;
}

int CAssetsManager::Load_Map(const char* pFileName, int StorageType, int Format, unsigned Crc)
{	
	dynamic_string Buffer;
	Buffer.append("maps/");
	Buffer.append(pFileName);
	Buffer.append(".map");
	
	int PackageId = NewPackage(pFileName);
	
	tw07::CDataFileReader ArchiveFile;
	if(!ArchiveFile.Open(Storage(), Buffer.buffer(), StorageType))
	{
		dbg_msg("AssetsManager", "can't open the file %s (storage type:%d)", pFileName, StorageType);
		return -1;
	}
	
	if(Crc != 0 && ArchiveFile.Crc() != Crc)
	{
		dbg_msg("AssetsManager", "wrong crc for the file %s (%d != %d)", pFileName, Crc, ArchiveFile.Crc());
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
			
			//Don't forget to add parameters when the game group is splitted
			pMapGroup->SetPosition(vec2(pGItem->m_OffsetX, pGItem->m_OffsetY));
			pMapGroup->SetHardParallax(vec2(pGItem->m_ParallaxX/100.0f, pGItem->m_ParallaxY/100.0f));

			//~ if(pGItem->m_Version >= 2)
			//~ {
				//~ pGroup->m_UseClipping = pGItem->m_UseClipping;
				//~ pGroup->m_ClipX = pGItem->m_ClipX;
				//~ pGroup->m_ClipY = pGItem->m_ClipY;
				//~ pGroup->m_ClipW = pGItem->m_ClipW;
				//~ pGroup->m_ClipH = pGItem->m_ClipH;
			//~ }

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

void CAssetsManager::Load_EnvClouds()
{
	if(m_PackageId_EnvClouds < 0)
	{
		m_PackageId_EnvClouds = Load_AssetsFile("env_clouds", CStorage::TYPE_ALL);
		if(m_PackageId_EnvClouds >= 0)
		{
			m_Path_Image_Cloud1 = FindAsset<CAsset_Image>(m_PackageId_EnvClouds, "cloud1");
			m_Path_Image_Cloud2 = FindAsset<CAsset_Image>(m_PackageId_EnvClouds, "cloud2");
			m_Path_Image_Cloud3 = FindAsset<CAsset_Image>(m_PackageId_EnvClouds, "cloud3");
		}
	}
}

void CAssetsManager::Load_EnvDesert()
{
	if(m_PackageId_EnvDesert < 0)
	{
		m_PackageId_EnvDesert = Load_AssetsFile("env_desert", CStorage::TYPE_ALL);
		if(m_PackageId_EnvDesert >= 0)
		{
			m_Path_Image_DesertMain = FindAsset<CAsset_Image>(m_PackageId_EnvDesert, "desertMain");
			m_Path_Image_DesertDoodads = FindAsset<CAsset_Image>(m_PackageId_EnvDesert, "desertDoodads");
			m_Path_Image_DesertMountains1 = FindAsset<CAsset_Image>(m_PackageId_EnvDesert, "desertMountains1");
			m_Path_Image_DesertMountains2 = FindAsset<CAsset_Image>(m_PackageId_EnvDesert, "desertMountains2");
			m_Path_Image_DesertSun = FindAsset<CAsset_Image>(m_PackageId_EnvDesert, "desertSun");
		}
	}
}

void CAssetsManager::Load_EnvGeneric()
{
	if(m_PackageId_EnvGeneric < 0)
	{
		m_PackageId_EnvGeneric = Load_AssetsFile("env_generic", CStorage::TYPE_ALL);
		if(m_PackageId_EnvGeneric >= 0)
		{
			m_Path_Image_GenericSpikes = FindAsset<CAsset_Image>(m_PackageId_EnvGeneric, "genericSpikes");
			m_Path_Image_GenericUnhookable = FindAsset<CAsset_Image>(m_PackageId_EnvGeneric, "genericUnhookable");
		}
	}
}

void CAssetsManager::Load_EnvGrass()
{
	if(m_PackageId_EnvGrass < 0)
	{
		m_PackageId_EnvGrass = Load_AssetsFile("env_grass", CStorage::TYPE_ALL);
		if(m_PackageId_EnvGrass >= 0)
		{
			m_Path_Image_GrassMain = FindAsset<CAsset_Image>(m_PackageId_EnvGrass, "grassMain");
			m_Path_Image_GrassDoodads = FindAsset<CAsset_Image>(m_PackageId_EnvGrass, "grassDoodads");
		}
	}
}

void CAssetsManager::Load_EnvJungle()
{
	if(m_PackageId_EnvJungle < 0)
	{
		m_PackageId_EnvJungle = Load_AssetsFile("env_jungle", CStorage::TYPE_ALL);
		if(m_PackageId_EnvJungle >= 0)
		{
			m_Path_Image_JungleMain = FindAsset<CAsset_Image>(m_PackageId_EnvJungle, "jungleMain");
			m_Path_Image_JungleDoodads = FindAsset<CAsset_Image>(m_PackageId_EnvJungle, "jungleDoodads");
			m_Path_Image_JungleSpikes = FindAsset<CAsset_Image>(m_PackageId_EnvJungle, "jungleSpikes");
			m_Path_Image_JungleUnhookable = FindAsset<CAsset_Image>(m_PackageId_EnvJungle, "jungleUnhookable");
			m_Path_Image_JungleBackground = FindAsset<CAsset_Image>(m_PackageId_EnvJungle, "jungleBackground");
			m_Path_Image_JungleMidground = FindAsset<CAsset_Image>(m_PackageId_EnvJungle, "jungleMidground");
		}
	}
}

void CAssetsManager::Load_EnvMoon()
{
	if(m_PackageId_EnvMoon < 0)
	{
		m_PackageId_EnvMoon = Load_AssetsFile("env_moon", CStorage::TYPE_ALL);
		if(m_PackageId_EnvMoon >= 0)
		{
			m_Path_Image_Moon = FindAsset<CAsset_Image>(m_PackageId_EnvMoon, "moon");
		}
	}
}

void CAssetsManager::Load_EnvMountains()
{
	if(m_PackageId_EnvMountains < 0)
	{
		m_PackageId_EnvMountains = Load_AssetsFile("env_mountains", CStorage::TYPE_ALL);
		if(m_PackageId_EnvMountains >= 0)
		{
			m_Path_Image_Mountains = FindAsset<CAsset_Image>(m_PackageId_EnvMountains, "mountains");
		}
	}
}

void CAssetsManager::Load_EnvSnow()
{
	if(m_PackageId_EnvSnow < 0)
	{
		m_PackageId_EnvSnow = Load_AssetsFile("env_snow", CStorage::TYPE_ALL);
		if(m_PackageId_EnvSnow >= 0)
		{
			m_Path_Image_Snow = FindAsset<CAsset_Image>(m_PackageId_EnvSnow, "snow");
		}
	}
}

void CAssetsManager::Load_EnvStars()
{
	if(m_PackageId_EnvStars < 0)
	{
		m_PackageId_EnvStars = Load_AssetsFile("env_stars", CStorage::TYPE_ALL);
		if(m_PackageId_EnvStars >= 0)
		{
			m_Path_Image_Stars = FindAsset<CAsset_Image>(m_PackageId_EnvSnow, "stars");
		}
	}
}

void CAssetsManager::Load_EnvSun()
{
	if(m_PackageId_EnvSun < 0)
	{
		m_PackageId_EnvSun = Load_AssetsFile("env_sun", CStorage::TYPE_ALL);
		if(m_PackageId_EnvSun >= 0)
		{
			m_Path_Image_Sun = FindAsset<CAsset_Image>(m_PackageId_EnvSun, "sun");
		}
	}
}

void CAssetsManager::Load_EnvWinter()
{
	if(m_PackageId_EnvWinter < 0)
	{
		m_PackageId_EnvWinter = Load_AssetsFile("env_winter", CStorage::TYPE_ALL);
		if(m_PackageId_EnvWinter >= 0)
		{
			m_Path_Image_WinterMain = FindAsset<CAsset_Image>(m_PackageId_EnvWinter, "winterMain");
			m_Path_Image_WinterDoodads = FindAsset<CAsset_Image>(m_PackageId_EnvWinter, "winterDoodads");
			m_Path_Image_WinterMountains1 = FindAsset<CAsset_Image>(m_PackageId_EnvWinter, "winterMountains1");
			m_Path_Image_WinterMountains2 = FindAsset<CAsset_Image>(m_PackageId_EnvWinter, "winterMountains2");
			m_Path_Image_WinterMountains3 = FindAsset<CAsset_Image>(m_PackageId_EnvWinter, "winterMountains3");
		}
	}
}

void CAssetsManager::Load_UnivTeeWorlds()
{
	if(m_PackageId_UnivTeeWorlds < 0)
	{
		m_PackageId_UnivTeeWorlds = Load_AssetsFile("univ_teeworlds", CStorage::TYPE_ALL);
		if(m_PackageId_UnivTeeWorlds >= 0)
		{
			m_Path_ZoneType_TWPhysics = FindAsset<CAsset_ZoneType>(m_PackageId_UnivTeeWorlds, "physics");
			m_Path_ZoneType_TWDamage = FindAsset<CAsset_ZoneType>(m_PackageId_UnivTeeWorlds, "damage");
			
			m_Path_EntityType_TWSpawn = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "spawn");
			m_Path_EntityType_TWSpawnRed = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "redSpawn");
			m_Path_EntityType_TWSpawnBlue = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "blueSpawn");
			m_Path_EntityType_TWGrenade = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "grenade");
			m_Path_EntityType_TWLaserRifle = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "laserRifle");
			m_Path_EntityType_TWShotgun = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "shotgun");
			m_Path_EntityType_TWNinja = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "ninja");
			m_Path_EntityType_TWHeart = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "heart");
			m_Path_EntityType_TWArmor = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "armor");
			m_Path_EntityType_TWFlagBlue = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "blueFlag");
			m_Path_EntityType_TWFlagRed = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "redFlag");
		}
	}
}

void CAssetsManager::Load_UnivDDNet()
{
	if(m_PackageId_UnivDDNet < 0)
	{
		m_PackageId_UnivTeeWorlds = Load_AssetsFile("univ_ddnet", CStorage::TYPE_ALL);
	}
}

void CAssetsManager::Load_UnivInfClass()
{
	if(m_PackageId_UnivInfClass < 0)
	{
		m_PackageId_UnivInfClass = Load_AssetsFile("univ_infclass", CStorage::TYPE_ALL);
		if(m_PackageId_UnivInfClass >= 0)
		{
			m_Path_ZoneType_InfClassPhysics = FindAsset<CAsset_ZoneType>(m_PackageId_UnivInfClass, "infcPhysics");
			m_Path_ZoneType_InfClassZones = FindAsset<CAsset_ZoneType>(m_PackageId_UnivInfClass, "infcZones");
			
			m_Path_EntityType_InfClassHumanSpawn = FindAsset<CAsset_EntityType>(m_PackageId_UnivInfClass, "humanSpawn");
			m_Path_EntityType_InfClassInfectedSpawn = FindAsset<CAsset_EntityType>(m_PackageId_UnivInfClass, "infectedSpawn");
			m_Path_EntityType_InfClassHeroFlag = FindAsset<CAsset_EntityType>(m_PackageId_UnivInfClass, "heroFlag");
		}
	}
}

void CAssetsManager::Load_UnivOpenFNG()
{
	if(m_PackageId_UnivOpenFNG < 0)
	{
		m_PackageId_UnivOpenFNG = Load_AssetsFile("univ_openfng", CStorage::TYPE_ALL);
	}
}

int CAssetsManager::AddSubItem(CAssetPath AssetPath, CSubPath SubPath, int Type, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return false;
		
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
	
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
	{\
		CAsset_##Name* pAsset = GetAsset_Hard<CAsset_##Name>(AssetPath);\
		if(pAsset)\
			return pAsset->AddSubItem(Type, SubPath);\
		else\
			return -1;\
	}
	
	switch(AssetPath.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	#undef MACRO_ASSETTYPE
	
	return -1;
}
	
CAssetPath CAssetsManager::DuplicateAsset(const CAssetPath& Path, int PackageId, int Token)
{
	if(!IsValidPackage(PackageId) || IsReadOnlyPackage(PackageId))
		return CAssetPath::Null();
	
	CAssetPath NewAssetPath;
	char aBuf[128];
	
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
	{\
		if(!GetAsset<CAsset_##Name>(Path))\
			return CAssetPath::Null();\
		CAsset_##Name* pNewAsset = NewAsset<CAsset_##Name>(&NewAssetPath, PackageId, Token);\
		if(!pNewAsset)\
			return CAssetPath::Null();\
		const CAsset_##Name* pOldAsset = GetAsset<CAsset_##Name>(Path);\
		pNewAsset->copy(*pOldAsset);\
		\
		int DuplicateNum = ((PackageId == Path.GetPackageId()) ? 1 : 0);\
		bool NameFound;\
		do\
		{\
			NameFound = false;\
			DuplicateNum++;\
			if(DuplicateNum < 2)\
				str_copy(aBuf, pOldAsset->GetName(), sizeof(aBuf));\
			else\
				str_format(aBuf, sizeof(aBuf), "%s (%d)", pOldAsset->GetName(), DuplicateNum);\
			for(int i=0; i<GetNumAssets<CAsset_##Name>(PackageId); i++)\
			{\
				const CAsset_##Name* pTestedAsset = GetAsset<CAsset_##Name>(CAssetPath(Path.GetType(), PackageId, i));\
				if(str_comp(pTestedAsset->GetName(), aBuf) == 0)\
				{\
					NameFound = true;\
					break;\
				}\
			}\
		}\
		while(NameFound);\
		pNewAsset->SetName(aBuf);\
		break;\
	}
	
	switch(Path.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	#undef MACRO_ASSETTYPE
	
	return NewAssetPath;
}

void CAssetsManager::DeleteAsset(const CAssetPath& Path, int Token)
{
	if(IsValidPackage(Path.GetPackageId()))
	{
		m_pPackages[Path.GetPackageId()]->DeleteAsset(Path);

		CAssetPath::COperation Operation = CAssetPath::COperation::DeleteAsset(Path);
		
		for(int i=0; i<m_pPackages.size(); i++)
		{
			if(m_pPackages[i])
				m_pPackages[i]->AssetPathOperation(Operation);
		}
	}
}

CAssetState* CAssetsManager::GetAssetState(CAssetPath Path)
{
	if(IsValidPackage(Path.GetPackageId()))
	{
		#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
			return m_pPackages[Path.GetPackageId()]->GetAssetState<CAsset_##Name>(Path);

		switch(Path.GetType())
		{
			#include <generated/assets/assetsmacro.h>
		}
		
		#undef MACRO_ASSETTYPE
	}
	return 0;
}

void CAssetsManager::InitAssetState(int PackageId, const CAssetState& State)
{
	if(PackageId >= 0 && PackageId < m_pPackages.size() && m_pPackages[PackageId])
		m_pPackages[PackageId]->InitAssetState(State);
}

void CAssetsManager::EnableAssetsHistory()
{
	if(m_pHistory)
		delete m_pHistory;
	
	m_pHistory = new CAssetsHistory(this);
}

void CAssetsManager::Undo()
{
	if(m_pHistory)
		m_pHistory->Undo();
}

int CAssetsManager::GenerateToken()
{
	if(m_pHistory)
		return m_pHistory->GenerateToken();
	else
		return CAssetsHistory::NO_TOKEN;
}
