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
#include <shared/assets/assetssaveloadcontext.h>
#include <shared/archivefile.h>

#include "assetsmanager.h"

/* ASSETS MANAGER *****************************************************/
	
CAssetsManager::CAssetsManager(CSharedKernel* pKernel) :
	CSharedKernel::CComponent(pKernel),
	m_pHistory(NULL),
	m_PackageId_UnivTeeWorlds(-1),
	m_PackageId_UnivDDNet(-1),
	m_PackageId_UnivOpenFNG(-1),
	m_PackageId_UnivNinslash(-1),
	m_PackageId_UnivSport(-1),
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
	m_PackageId_EnvLab(-1),
	m_PackageId_EnvFactory(-1)
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
		
	if(m_pHistory)
		m_pHistory->Flush();
	
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
	m_pPackages[PackageId]->SetEdited(true);
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
	
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
	
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
	
	m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);

	#undef MACRO_ASSETTYPE
}

void CAssetsManager::TryChangeAssetName_Hard(CAssetPath AssetPath, const char* pName)
{
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
		SetAssetValue_Hard<const char*>(AssetPath, CSubPath::Null(), CAsset::NAME, Buffer.buffer());\
		break;

	switch(AssetPath.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);

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

void CAssetsManager::SetPackageEdited(int PackageId , bool Value)
{
	if(IsValidPackage(PackageId))
		m_pPackages[PackageId]->SetEdited(Value);
}

bool CAssetsManager::IsEditedPackage(int PackageId) const
{
	return m_pPackages[PackageId]->IsEdited();
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
	
	if(m_pHistory)
		m_pHistory->Flush();
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

bool CAssetsManager::GetPackageSaveFilename(int PackageId, dynamic_string& Filename)
{
	if(!IsValidPackage(PackageId))
		return false;
	
	Filename.append("assets/");
	Filename.append(m_pPackages[PackageId]->GetName());
	Filename.append(".tup");
	
	return true;
}

bool CAssetsManager::Save_AssetsFile(const char* pFilename, int StorageType, int PackageId)
{
	if(!IsValidPackage(PackageId))
		return false;
	
	dynamic_string FullPath;
	if(!GetPackageSaveFilename(PackageId, FullPath))
		return false;
	
	CArchiveFile ArchiveFile;
	CAssetsSaveLoadContext SaveLoadContext(this, &ArchiveFile, PackageId);
	
	m_pPackages[PackageId]->Save_AssetsFile(&SaveLoadContext);
	
	if(!ArchiveFile.Write(Storage(), FullPath.buffer()))
	{
		dbg_msg("AssetsManager", "can't write the file %s", FullPath.buffer());
		return false;
	}
	
	m_pPackages[PackageId]->SetEdited(false);
	
	return true;
}

bool CAssetsManager::Save_AssetsFile(int PackageId)
{
	if(!IsValidPackage(PackageId))
		return false;
	
	return Save_AssetsFile(m_pPackages[PackageId]->GetName(), CStorage::TYPE_SAVE, PackageId);
}

int CAssetsManager::Load_AssetsFile_Core(const char* pFileName, int StorageType, unsigned Crc, CErrorStack* pErrorStack)
{
	dynamic_string PackageName;
	dynamic_string FullPath;
	
	if(StorageType == CStorage::TYPE_ABSOLUTE)
	{
		FullPath.copy(pFileName);
		
		int Length = str_length(pFileName);
		int i=Length-1;
		int SegmentEnd = i;
		int SegmentStart = i;
		int SegmentLast = i;
		bool SlashFound = false;
		
		for(int i=Length-1; i>=0; i--)
		{
			if(pFileName[i] == '/' || pFileName[i] == '\\' || i==0)
			{
				if(!SlashFound)
				{
					SegmentStart = i + (i > 0);
					if(str_comp_num(pFileName+SegmentStart, "assets", SegmentEnd-SegmentStart+1) == 0)
					{
						PackageName.clear();
						PackageName.append_at(0, pFileName+SegmentLast);
						break;
					}
					else if(PackageName.empty())
					{
						PackageName.clear();
						PackageName.append_at(0, pFileName+SegmentLast);	
					}
				}
				SlashFound = true;
				SegmentLast = SegmentStart;
			}
			else if(SlashFound)
			{
				SegmentEnd = i;
				SlashFound = false;
			}
		}
		
		Length = PackageName.length();
		if(Length >= 4 && str_comp(PackageName.buffer()+Length-4, ".tup") == 0)
			PackageName.buffer()[Length-4] = 0;
	}
	else
	{
#if defined(CONF_FAMILY_WINDOWS)
		FullPath.append("assets\\");
#else
		FullPath.append("assets/");
#endif
		FullPath.append(pFileName);
		FullPath.append(".tup");
		
		PackageName.copy(pFileName);
	}
	
	int PackageId = -1;
	CAssetsPackage* pPackage = NULL;
	
	CArchiveFile ArchiveFile;
	if(!ArchiveFile.Open(Storage(), FullPath.buffer(), StorageType))
	{
		if(pErrorStack)
		{
			CLocalizableString LString(_("Can't open the package {str:Filename}"));
			LString.AddString("Filename", FullPath.buffer());
			pErrorStack->AddError(LString);
		}
		dbg_msg("AssetsManager", "can't open the file %s (storage type:%d)", FullPath.buffer(), StorageType);
		return PackageId;
	}
	
	if(Crc != 0 && ArchiveFile.Crc() != Crc)
	{
		dbg_msg("AssetsManager", "wrong crc for the file %s (%d != %d)", FullPath.buffer(), Crc, ArchiveFile.Crc());
		return PackageId;
	}
	
	for(int i=0; i<m_pPackages.size(); i++)
	{
		if(m_pPackages[i] && m_pPackages[i]->Identify(PackageName.buffer(), Crc))
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
		pPackage->SetIdentity(PackageName.buffer(), ArchiveFile.Crc());
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
			SaveLoadContext.AddDependency(Load_AssetsFile_Core(pPackageName, CStorage::TYPE_ALL, PackageCrc, pErrorStack));
		}
	}
	
	if(!pPackage->IsLoaded())
		pPackage->Load_AssetsFile(&SaveLoadContext);
	
	for(int i=0; i<GetNumAssets<CAsset_Image>(PackageId); i++)
		RequestUpdate(CAssetPath(CAsset_Image::TypeId, PackageId, i));
	
	pPackage->SetEdited(false);
	
	if(m_pHistory)
		m_pHistory->Flush();
	
	return PackageId;
}

int CAssetsManager::Load_AssetsFile(const char* pFileName, int StorageType, unsigned Crc, CErrorStack* pErrorStack)
{	
	//Load all dependencies
	int PackageId = Load_AssetsFile_Core(pFileName, StorageType, Crc, pErrorStack);
	
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
			m_Path_Image_Stars = FindAsset<CAsset_Image>(m_PackageId_EnvStars, "stars");
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

void CAssetsManager::Load_EnvLab()
{
	if(m_PackageId_EnvLab < 0)
	{
		m_PackageId_EnvLab = Load_AssetsFile("env_lab", CStorage::TYPE_ALL);
		if(m_PackageId_EnvLab >= 0)
		{
			m_Path_Image_LabMisc = FindAsset<CAsset_Image>(m_PackageId_EnvLab, "labMisc");
			m_Path_Image_LabMain = FindAsset<CAsset_Image>(m_PackageId_EnvLab, "labMain");
			m_Path_Image_LabBackground = FindAsset<CAsset_Image>(m_PackageId_EnvLab, "labBackground");
		}
	}
}

void CAssetsManager::Load_EnvFactory()
{
	if(m_PackageId_EnvFactory < 0)
	{
		m_PackageId_EnvFactory = Load_AssetsFile("env_factory", CStorage::TYPE_ALL);
		if(m_PackageId_EnvFactory >= 0)
		{
			m_Path_Image_FactoryMain = FindAsset<CAsset_Image>(m_PackageId_EnvFactory, "factoryMain");
			m_Path_Image_FactoryBackground = FindAsset<CAsset_Image>(m_PackageId_EnvFactory, "factoryBackground");
		}
	}
}

void CAssetsManager::Load_UnivTeeWorlds()
{
	if(m_PackageId_UnivTeeWorlds < 0)
	{
		m_PackageId_UnivTeeWorlds = Load_AssetsFile("teeworlds", CStorage::TYPE_ALL);
		if(m_PackageId_UnivTeeWorlds >= 0)
		{
			m_Path_ZoneType_TeeWorlds = FindAsset<CAsset_ZoneType>(m_PackageId_UnivTeeWorlds, "teeworlds");
			
			m_Path_EntityType_TWSpawn = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "spawn");
			m_Path_EntityType_TWSpawnRed = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "redSpawn");
			m_Path_EntityType_TWSpawnBlue = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "blueSpawn");
			m_Path_EntityType_TWGrenade = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "grenade");
			m_Path_EntityType_TWLaserRifle = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "rifle");
			m_Path_EntityType_TWShotgun = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "shotgun");
			m_Path_EntityType_TWNinja = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "ninja");
			m_Path_EntityType_TWHeart = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "health");
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
		m_PackageId_UnivDDNet = Load_AssetsFile("ddnet", CStorage::TYPE_ALL);
		if(m_PackageId_UnivDDNet >= 0)
		{
			m_Path_ZoneType_DDGame = FindAsset<CAsset_ZoneType>(m_PackageId_UnivDDNet, "ddGame");
			m_Path_ZoneType_DDFront = FindAsset<CAsset_ZoneType>(m_PackageId_UnivDDNet, "ddFront");
			m_Path_ZoneType_DDTele = FindAsset<CAsset_ZoneType>(m_PackageId_UnivDDNet, "ddTele");
			m_Path_ZoneType_DDSwitch = FindAsset<CAsset_ZoneType>(m_PackageId_UnivDDNet, "ddSwitch");
		}
	}
}

void CAssetsManager::Load_UnivOpenFNG()
{
	if(m_PackageId_UnivOpenFNG < 0)
	{
		m_PackageId_UnivOpenFNG = Load_AssetsFile("openfng", CStorage::TYPE_ALL);
		if(m_PackageId_UnivOpenFNG >= 0)
		{
			m_Path_ZoneType_OpenFNG = FindAsset<CAsset_ZoneType>(m_PackageId_UnivOpenFNG, "openfng");
		}
	}
}

void CAssetsManager::Load_UnivNinslash()
{
	if(m_PackageId_UnivNinslash < 0)
	{
		m_PackageId_UnivNinslash = Load_AssetsFile("ninslash", CStorage::TYPE_ALL);
		if(m_PackageId_UnivNinslash >= 0)
		{
			m_Path_ZoneType_Ninslash = FindAsset<CAsset_ZoneType>(m_PackageId_UnivNinslash, "ninslash");
			
			m_Path_EntityType_NSSpawn = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsSpawn");
			m_Path_EntityType_NSSpawnRed = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsRedSpawn");
			m_Path_EntityType_NSSpawnBlue = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsBlueSpawn");
			m_Path_EntityType_NSGrenade = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsGrenade");
			m_Path_EntityType_NSRifle = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsRifle");
			m_Path_EntityType_NSLaser = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsLaser");
			m_Path_EntityType_NSShotgun = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsShotgun");
			m_Path_EntityType_NSChainsaw = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsChainsaw");
			m_Path_EntityType_NSElectric = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsElectric");
			m_Path_EntityType_NSFlamer = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsFlamer");
			m_Path_EntityType_NSHeart = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsHealth");
			m_Path_EntityType_NSFlagBlue = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsBlueFlag");
			m_Path_EntityType_NSFlagRed = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsRedFlag");
		}
	}
}

void CAssetsManager::Load_UnivSport()
{
	if(m_PackageId_UnivSport < 0)
	{
		m_PackageId_UnivSport = Load_AssetsFile("sport", CStorage::TYPE_ALL);
		if(m_PackageId_UnivSport >= 0)
		{
			m_Path_ZoneType_Sport = FindAsset<CAsset_ZoneType>(m_PackageId_UnivSport, "sport");
		}
	}
}

int CAssetsManager::AddSubItem_Hard(CAssetPath AssetPath, CSubPath SubPath, int Type)
{
	if(!IsValidPackage(AssetPath.GetPackageId()))
		return -1;
	
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
	{\
		CAsset_##Name* pAsset = GetAsset_Hard<CAsset_##Name>(AssetPath);\
		if(pAsset)\
		{\
			m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);\
			return pAsset->AddSubItem(Type, SubPath);\
		}\
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

int CAssetsManager::AddSubItem(CAssetPath AssetPath, CSubPath SubPath, int Type, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return -1;
		
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
		
	return AddSubItem_Hard(AssetPath, SubPath, Type);
}

int CAssetsManager::AddSubItemAt(CAssetPath AssetPath, CSubPath SubPath, int Type, int Index, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return -1;
		
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
	
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
	{\
		CAsset_##Name* pAsset = GetAsset_Hard<CAsset_##Name>(AssetPath);\
		if(pAsset)\
		{\
			m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);\
			return pAsset->AddSubItemAt(Type, SubPath, Index);\
		}\
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

bool CAssetsManager::DeleteSubItem(CAssetPath AssetPath, CSubPath SubPath, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return false;
		
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
	
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
	{\
		CAsset_##Name* pAsset = GetAsset_Hard<CAsset_##Name>(AssetPath);\
		if(pAsset)\
		{\
			m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);\
			pAsset->DeleteSubItem(SubPath);\
		}\
	}
	
	switch(AssetPath.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	#undef MACRO_ASSETTYPE
	
	return true;
}

bool CAssetsManager::RelMoveSubItem(CAssetPath AssetPath, CSubPath SubPath, int RelMove, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return false;
		
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
	
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
	{\
		CAsset_##Name* pAsset = GetAsset_Hard<CAsset_##Name>(AssetPath);\
		if(pAsset)\
		{\
			m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);\
			pAsset->RelMoveSubItem(SubPath, RelMove);\
		}\
	}
	
	switch(AssetPath.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	#undef MACRO_ASSETTYPE
	
	return true;
}
	
CAssetPath CAssetsManager::DuplicateAsset(const CAssetPath& Path, int PackageId, int Token)
{
	if(!IsValidPackage(PackageId) || IsReadOnlyPackage(PackageId))
		return CAssetPath::Null();
	
	CAssetPath NewAssetPath;
	char aBuf[128];
	dynamic_string Buffer;
	
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
		const char* pName = pOldAsset->GetName();\
		const char* pCharIter = pName + str_length(pName) - 1;\
		while(pCharIter > pName && *pCharIter >= '0' && *pCharIter <= '9')\
		{\
			pCharIter--;\
		}\
		if(pCharIter >= pName)\
		{\
			Buffer.append_num(pName, pCharIter - pName + 1);\
			DuplicateNum = str_to_int(pCharIter + 1);\
		}\
		else\
			Buffer.copy(pName);\
		bool NameFound;\
		do\
		{\
			NameFound = false;\
			DuplicateNum++;\
			if(DuplicateNum < 2)\
				str_copy(aBuf, Buffer.buffer(), sizeof(aBuf));\
			else\
				str_format(aBuf, sizeof(aBuf), "%s%d", Buffer.buffer(), DuplicateNum);\
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
		m_pPackages[PackageId]->SetEdited(true);\
		break;\
	}
	
	switch(Path.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	#undef MACRO_ASSETTYPE
	
	return NewAssetPath;
}

void CAssetsManager::DeleteAsset_Hard(const CAssetPath& Path)
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

void CAssetsManager::DeleteAsset(const CAssetPath& Path, int Token)
{
	DeleteAsset_Hard(Path);
	if(m_pHistory)
		m_pHistory->Flush();
}

void CAssetsManager::DeleteAssets(array<CAssetPath>& Pathes, int Token)
{
	for(int a=0; a<Pathes.size(); a++)
	{
		if(IsValidPackage(Pathes[a].GetPackageId()))
		{
			m_pPackages[Pathes[a].GetPackageId()]->DeleteAsset(Pathes[a]);

			CAssetPath::COperation Operation = CAssetPath::COperation::DeleteAsset(Pathes[a]);
				
			for(int i=0; i<m_pPackages.size(); i++)
			{
				if(m_pPackages[i])
					m_pPackages[i]->AssetPathOperation(Operation);
			}
			
			for(int b=a+1; b<Pathes.size(); b++)
				Operation.Apply(Pathes[b]);
		}
	}
	if(m_pHistory)
		m_pHistory->Flush();
}
	
void CAssetsManager::MapLayerTiles_HFlip(const CAssetPath& AssetPath, int Token)
{
	const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetPath);
	if(!pLayer)
		return;
	
	array2d< CAsset_MapLayerTiles::CTile, allocator_copy<CAsset_MapLayerTiles::CTile> > Tiles;
	Tiles.copy(pLayer->GetTileArray());
	
	for(int j=0; j<Tiles.get_height(); j++)
	{
		for(int i=0; i<Tiles.get_width(); i++)
		{
			const CAsset_MapLayerTiles::CTile& Tile = Tiles.get_clamp(i, j);
			int Flags = Tile.GetFlags() ^ CAsset_MapLayerTiles::TILEFLAG_HFLIP;
			
			CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(i, Tiles.get_height()-j-1);
			AssetsManager()->SetAssetValue<int>(AssetPath, TilePath, CAsset_MapLayerTiles::TILE_INDEX, Tile.GetIndex(), Token);
			AssetsManager()->SetAssetValue<int>(AssetPath, TilePath, CAsset_MapLayerTiles::TILE_FLAGS, Flags, Token);
		}
	}
}

void CAssetsManager::MapLayerTiles_VFlip(const CAssetPath& AssetPath, int Token)
{
	const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetPath);
	if(!pLayer)
		return;
	
	array2d< CAsset_MapLayerTiles::CTile, allocator_copy<CAsset_MapLayerTiles::CTile> > Tiles;
	Tiles.copy(pLayer->GetTileArray());
	
	for(int j=0; j<Tiles.get_height(); j++)
	{
		for(int i=0; i<Tiles.get_width(); i++)
		{
			const CAsset_MapLayerTiles::CTile& Tile = Tiles.get_clamp(i, j);
			int Flags = Tile.GetFlags() ^ CAsset_MapLayerTiles::TILEFLAG_VFLIP;
			
			CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(Tiles.get_width()-i-1, j);
			AssetsManager()->SetAssetValue<int>(AssetPath, TilePath, CAsset_MapLayerTiles::TILE_INDEX, Tile.GetIndex(), Token);
			AssetsManager()->SetAssetValue<int>(AssetPath, TilePath, CAsset_MapLayerTiles::TILE_FLAGS, Flags, Token);
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

bool CAssetsManager::SaveAssetInHistory(CAssetPath AssetPath, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return false;
	
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
	
	m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);
	
	return true;
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

int CAssetsManager::GetHistorySize()
{
	if(m_pHistory)
		return m_pHistory->GetHistorySize();
	else
		return 0;
}
