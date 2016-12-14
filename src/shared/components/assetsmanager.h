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

#ifndef __SHARED_ASSETSMANAGER__
#define __SHARED_ASSETSMANAGER__

#include <shared/kernel.h>
#include <shared/tl/hashtable.h>
#include <shared/assets/assetpath.h>
#include <shared/assets/assetspackage.h>
#include <shared/components/assetsmanager_history.h>
#include <tw07/shared/datafile.h>

class CAssetsManager : public CSharedKernel::CComponent
{
public:	
	struct CAssetUpdateDesc
	{
		CAssetPath m_Path;
		bool m_Updated;
	};
	
	enum
	{
		MAPFORMAT_TW=0,
		MAPFORMAT_INFCLASS,
		MAPFORMAT_OPENFNG,
	};
	
private:
	CAssetsHistory* m_pHistory;
	array<CAssetUpdateDesc> m_ImagesToUpdate;
	array<char*> m_pNamesToResolved;

public:
	int m_PackageId_UnivTeeWorlds;
	CAssetPath m_Path_ZoneType_TWPhysics;
	CAssetPath m_Path_ZoneType_TWDamage;
	CAssetPath m_Path_EntityType_TWSpawn;
	CAssetPath m_Path_EntityType_TWSpawnRed;
	CAssetPath m_Path_EntityType_TWSpawnBlue;
	CAssetPath m_Path_EntityType_TWShotgun;
	CAssetPath m_Path_EntityType_TWGrenade;
	CAssetPath m_Path_EntityType_TWLaserRifle;
	CAssetPath m_Path_EntityType_TWNinja;
	CAssetPath m_Path_EntityType_TWHeart;
	CAssetPath m_Path_EntityType_TWArmor;
	CAssetPath m_Path_EntityType_TWFlagBlue;
	CAssetPath m_Path_EntityType_TWFlagRed;
	
	int m_PackageId_UnivInfClass;
	CAssetPath m_Path_ZoneType_InfClassPhysics;
	CAssetPath m_Path_ZoneType_InfClassZones;
	CAssetPath m_Path_EntityType_InfClassHumanSpawn;
	CAssetPath m_Path_EntityType_InfClassInfectedSpawn;
	CAssetPath m_Path_EntityType_InfClassHeroFlag;
	
	int m_PackageId_UnivOpenFNG;
	CAssetPath m_Path_ZoneType_OpenFNGShrine;
	CAssetPath m_Path_EntityType_OpenFNGRedScore;
	CAssetPath m_Path_EntityType_OpenFNGBlueScore;
	
	int m_PackageId_EnvClouds;
	CAssetPath m_Path_Image_Cloud1;
	CAssetPath m_Path_Image_Cloud2;
	CAssetPath m_Path_Image_Cloud3;
	
	int m_PackageId_EnvDesert;
	CAssetPath m_Path_Image_DesertMain;
	CAssetPath m_Path_Image_DesertDoodads;
	CAssetPath m_Path_Image_DesertMountains1;
	CAssetPath m_Path_Image_DesertMountains2;
	CAssetPath m_Path_Image_DesertSun;
	
	int m_PackageId_EnvGeneric;
	CAssetPath m_Path_Image_GenericUnhookable;
	CAssetPath m_Path_Image_GenericSpikes;
	
	int m_PackageId_EnvGrass;
	CAssetPath m_Path_Image_GrassMain;
	CAssetPath m_Path_Image_GrassDoodads;
	
	int m_PackageId_EnvJungle;
	CAssetPath m_Path_Image_JungleMain;
	CAssetPath m_Path_Image_JungleDoodads;
	CAssetPath m_Path_Image_JungleSpikes;
	CAssetPath m_Path_Image_JungleUnhookable;
	CAssetPath m_Path_Image_JungleBackground;
	CAssetPath m_Path_Image_JungleMidground;
	
	int m_PackageId_EnvMoon;
	CAssetPath m_Path_Image_Moon;
	
	int m_PackageId_EnvMountains;
	CAssetPath m_Path_Image_Mountains;
	
	int m_PackageId_EnvSnow;
	CAssetPath m_Path_Image_Snow;
	
	int m_PackageId_EnvStars;
	CAssetPath m_Path_Image_Stars;
	
	int m_PackageId_EnvSun;
	CAssetPath m_Path_Image_Sun;
	
	int m_PackageId_EnvWinter;
	CAssetPath m_Path_Image_WinterMain;
	CAssetPath m_Path_Image_WinterDoodads;
	CAssetPath m_Path_Image_WinterMountains1;
	CAssetPath m_Path_Image_WinterMountains2;
	CAssetPath m_Path_Image_WinterMountains3;
	
private:	
	array<CAssetsPackage*> m_pPackages;
	
public:
	CAssetsManager(CSharedKernel* pKernel);
	virtual ~CAssetsManager();
	virtual bool PostUpdate();
	
	void RequestUpdate(const CAssetPath& AssetPath);
	const array<CAssetUpdateDesc>& GetImagesToUpdate() const { return m_ImagesToUpdate; }
	void MarkUpdated(int i) { m_ImagesToUpdate[i].m_Updated = true; }
	
	int NewPackage(const char* pName);
	const char* GetPackageName(int PackageId) const;
	void SetPackageName(int PackageId, const char* pName);
	void SetPackageName_Hard(int PackageId, const char* pName);
	int GetNumPackages() const;
	void ClosePackage(int PackageId);
	bool IsValidPackage(int PackageId) const;
	bool IsReadOnlyPackage(int PackageId) const;
	void SetPackageReadOnly(int PackageId , bool Value);
	bool IsEditedPackage(int PackageId) const;
	void SetPackageEdited(int PackageId , bool Value);
	
	inline const char* GetPackageAuthor(int PackageId) const { return (IsValidPackage(PackageId) ? m_pPackages[PackageId]->GetAuthor() : ""); }
	inline const char* GetPackageCredits(int PackageId) const { return (IsValidPackage(PackageId) ? m_pPackages[PackageId]->GetCredits() : ""); }
	inline const char* GetPackageLicense(int PackageId) const { return (IsValidPackage(PackageId) ? m_pPackages[PackageId]->GetLicense() : ""); }
	inline const char* GetPackageVersion(int PackageId) const { return (IsValidPackage(PackageId) ? m_pPackages[PackageId]->GetVersion() : ""); }
	
	inline void SetPackageAuthor(int PackageId, const char* pValue) { if(IsValidPackage(PackageId)) { m_pPackages[PackageId]->SetEdited(true); m_pPackages[PackageId]->SetAuthor(pValue); } }
	inline void SetPackageCredits(int PackageId, const char* pValue) { if(IsValidPackage(PackageId)) { m_pPackages[PackageId]->SetEdited(true); m_pPackages[PackageId]->SetCredits(pValue); } }
	inline void SetPackageLicense(int PackageId, const char* pValue) { if(IsValidPackage(PackageId)) { m_pPackages[PackageId]->SetEdited(true); m_pPackages[PackageId]->SetLicense(pValue); } }
	inline void SetPackageVersion(int PackageId, const char* pValue) { if(IsValidPackage(PackageId)) { m_pPackages[PackageId]->SetEdited(true); m_pPackages[PackageId]->SetVersion(pValue); } }
	
	int AddNameToResolve(const char* pName);
	
	CAssetState* GetAssetState(CAssetPath AssetPath);
	
	void InitAssetState(int Source, const CAssetState& State);
	
	bool Save_AssetsFile(int PackageId);
	bool Save_AssetsFile(const char *pFileName, int StorageType, int PackageId);
	int Load_AssetsFile_Core(const char *pFileName, int StorageType, unsigned Crc = 0);
	int Load_AssetsFile(const char *pFileName, int StorageType, unsigned Crc = 0);
	
	void Save_Map_Group(tw07::CDataFileWriter& ArchiveFile, const CAssetPath& GroupPath, array<CAssetPath>& Images, int& GroupId, int& LayerId);
	bool Save_Map(const char* pFileName, int StorageType, int PackageId, int Format);
	int Load_Map(const char* pFileName, int StorageType, int Format, unsigned Crc = 0);
	
	void Load_UnivTeeWorlds();
	void Load_UnivInfClass();
	void Load_UnivOpenFNG();
	
	void Load_EnvClouds();
	void Load_EnvDesert();
	void Load_EnvGeneric();
	void Load_EnvGrass();
	void Load_EnvJungle();
	void Load_EnvMoon();
	void Load_EnvMountains();
	void Load_EnvSnow();
	void Load_EnvStars();
	void Load_EnvSun();
	void Load_EnvWinter();
	
	CAssetPath DuplicateAsset(const CAssetPath& Path, int PackageId, int Token = -1);
	
	void TryChangeAssetName(CAssetPath AssetPath, const char* pName, int Token = -1);

	template<class ASSET>
	const ASSET* GetAsset(CAssetPath AssetPath) const
	{
		if(IsValidPackage(AssetPath.GetPackageId()))
			return m_pPackages[AssetPath.GetPackageId()]->GetAsset<ASSET>(AssetPath);
		else
			return NULL;
	}
	
	template<class ASSET>
	ASSET* GetAsset_Hard(CAssetPath AssetPath)
	{
		if(IsValidPackage(AssetPath.GetPackageId()))
			return m_pPackages[AssetPath.GetPackageId()]->GetAsset<ASSET>(AssetPath);
		else
			return NULL;
	}

	template<class ASSET>
	void SetAsset_Hard(CAssetPath AssetPath, const ASSET* pAsset)
	{
		if(IsValidPackage(AssetPath.GetPackageId()))
		{
			ASSET* pOldAsset = m_pPackages[AssetPath.GetPackageId()]->GetAsset<ASSET>(AssetPath);
			if(pOldAsset)
				pOldAsset->copy(*pAsset);
		}
	}
	
	template<class ASSET>
	ASSET* NewAsset(CAssetPath* pAssetPath, int PackageId, int Token)
	{
		if(IsValidPackage(PackageId) && !IsReadOnlyPackage(PackageId))
		{
			ASSET* pNewAsset = m_pPackages[PackageId]->NewAsset<ASSET>(this, pAssetPath);
			if(pNewAsset)
			{
				pAssetPath->SetPackageId(PackageId);
				if(m_pHistory)
					m_pHistory->AddOperation_AddAsset(*pAssetPath, Token);
				m_pPackages[PackageId]->SetEdited(true);
			}
			return pNewAsset;
		}
		else
			return NULL;
	}
	
	template<class ASSET>
	ASSET* NewAsset_Hard(CAssetPath* pAssetPath, int PackageId)
	{
		if(IsValidPackage(PackageId))
		{
			ASSET* pNewAsset = m_pPackages[PackageId]->NewAsset<ASSET>(this, pAssetPath);
			if(pNewAsset)
			{
				pAssetPath->SetPackageId(PackageId);
				m_pPackages[PackageId]->SetEdited(true);
			}
			
			return pNewAsset;
		}
		else
			return NULL;
	}
	
	template<class ASSET>
	int GetNumAssets(int PackageId) const
	{
		if(IsValidPackage(PackageId))
			return m_pPackages[PackageId]->GetNumAssets<ASSET>();
		else
			return 0;
	}

	void DeleteAsset(const CAssetPath& Path, int Token = -1);
	void DeleteAssets(array<CAssetPath>& Pathes, int Token = -1);
	void DeleteAsset_Hard(const CAssetPath& Path);

	template<typename T>
	T GetAssetValue(CAssetPath AssetPath, CSubPath SubPath, int FieldType, T DefaultValue)
	{
		#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
		{\
			const CAsset_##Name* pAsset = GetAsset<CAsset_##Name>(AssetPath);\
			if(pAsset)\
				return pAsset->GetValue<T>(FieldType, SubPath, DefaultValue);\
			else\
				return DefaultValue;\
		}
		
		switch(AssetPath.GetType())
		{
			#include <generated/assets/assetsmacro.h>
		}
		
		#undef MACRO_ASSETTYPE
		
		return DefaultValue;
	}

	template<typename T>
	bool SetAssetValue_Hard(CAssetPath AssetPath, CSubPath SubPath, int FieldType, const T& Value)
	{
		if(!IsValidPackage(AssetPath.GetPackageId()))
			return false;
		
		#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
		{\
			CAsset_##Name* pAsset = GetAsset_Hard<CAsset_##Name>(AssetPath);\
			if(pAsset)\
				return pAsset->SetValue<T>(FieldType, SubPath, Value);\
			else\
				return false;\
		}
		
		switch(AssetPath.GetType())
		{
			#include <generated/assets/assetsmacro.h>
		}
		
		#undef MACRO_ASSETTYPE
		
		return false;
	}

	template<typename T>
	bool SetAssetValue(CAssetPath AssetPath, CSubPath SubPath, int FieldType, const T& Value, int Token = -1)
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
				if(pAsset->SetValue<T>(FieldType, SubPath, Value))\
				{\
					m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);\
					return true;\
				}\
			}\
			else\
				return false;\
		}
		
		switch(AssetPath.GetType())
		{
			#include <generated/assets/assetsmacro.h>
		}
		
		#undef MACRO_ASSETTYPE
		
		return false;
	}

	int AddSubItem(CAssetPath AssetPath, CSubPath SubPath, int Type, int Token = -1);
	int DeleteSubItem(CAssetPath AssetPath, CSubPath SubPath, int Token = -1);
	
	template<typename ASSET>
	CAssetPath FindAsset(int PackageId, const char* pAssetName) const
	{
		CAssetPath Result;
		if(IsValidPackage(PackageId))
		{
			if(m_pPackages[PackageId]->FindAsset<ASSET>(pAssetName, &Result))
			{
				Result.SetPackageId(PackageId);
				return Result;
			}
		}
		
		dbg_msg("AssetsManager", "Can't find the asset %s in the package #%d", pAssetName, PackageId);
		
		return CAssetPath::Null();
	}
	
	template<typename ASSET>
	bool FindAsset(const char* pPackageName, const char* pAssetName, CAssetPath* pPath) const
	{
		for(int i=0; i<m_pPackages.size(); i++)
		{
			if(m_pPackages[i] && str_comp(pPackageName, m_pPackages[i]->GetName()) == 0)
			{
				if(m_pPackages[i]->FindAsset<ASSET>(pAssetName, pPath))
				{
					pPath->SetPackageId(i);
					return true;
				}
			}
		}
		
		*pPath = CAssetPath::Null();
		return false;
	}
	
	void MapLayerTiles_HFlip(const CAssetPath& AssetPath, int Token);
	void MapLayerTiles_VFlip(const CAssetPath& AssetPath, int Token);
	
	void EnableAssetsHistory();
	void Undo();
	int GenerateToken();
	int GetHistorySize();
};

#endif
