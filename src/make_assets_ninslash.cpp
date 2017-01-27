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

#include <shared/system/debug.h>
#include <shared/system/string.h>
#include <shared/kernel.h>
#include <shared/components/assetsmanager.h>
#include <shared/components/storage.h>
#include <client/loading_tools.h>

#include <cstdlib>

#define CREATE_SPRITE_PATH(path, packageid, name, image, x, y, w, h) {\
	CAsset_Sprite* pSprite = pKernel->AssetsManager()->NewAsset_Hard<CAsset_Sprite>(&path, packageid);\
	pSprite->SetName(name);\
	pSprite->SetImagePath(image);\
	pSprite->SetX(x);\
	pSprite->SetY(y);\
	pSprite->SetWidth(w);\
	pSprite->SetHeight(h);\
}

#define CREATE_ZONEINDEX_NOUSE() {\
	SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());\
	pAsset->SetIndexUsed(SubPath, false);\
}

int main(int argc, char* argv[])
{
	CSharedKernel* pKernel = new CSharedKernel();
	if(!pKernel->Init(argc, (const char**) argv))
	{
		dbg_msg("Kernel", "unable to initialize shared kernel");
		exit(EXIT_FAILURE);
	}
	
	int PackageId = pKernel->AssetsManager()->NewPackage("ninslash");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageCredits(PackageId, "Siile");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	
	CAssetPath ImageZonesPath = CreateNewImage(pKernel, PackageId, "zones", "images/univ_ninslash/zones.png", CStorage::TYPE_ALL, 16, 16, true, 0);
	CAssetPath ImageEntitiesPath = CreateNewImage(pKernel, PackageId, "entities", "images/univ_ninslash/entities.png", CStorage::TYPE_ALL, 8, 4);
	
	//Zones
	{
		CAssetPath AssetPath;
		CSubPath SubPath;
		
		CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
		pAsset->SetName("ninslash");
		pAsset->SetImagePath(ImageZonesPath);
		
		int GroupId_Physics = pAsset->AddGroup();
		int GroupId_Damage = pAsset->AddGroup();
		int GroupId_Entities = pAsset->AddGroup();
		
		pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Physics), "Physics");
		pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Damage), "Damage");
		pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Entities), "Entities");
		
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Air");
			pAsset->SetIndexGroup(SubPath, GroupId_Physics);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Ground");
			pAsset->SetIndexGroup(SubPath, GroupId_Physics);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Damage");
			pAsset->SetIndexGroup(SubPath, GroupId_Damage);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Death");
			pAsset->SetIndexGroup(SubPath, GroupId_Damage);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Slope right");
			pAsset->SetIndexGroup(SubPath, GroupId_Physics);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Slope left");
			pAsset->SetIndexGroup(SubPath, GroupId_Physics);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Reversed slope right");
			pAsset->SetIndexGroup(SubPath, GroupId_Physics);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Reversed slope left");
			pAsset->SetIndexGroup(SubPath, GroupId_Physics);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Fluid death");
			pAsset->SetIndexGroup(SubPath, GroupId_Damage);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Right to left accelerator");
			pAsset->SetIndexGroup(SubPath, GroupId_Physics);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Left to right accelerator");
			pAsset->SetIndexGroup(SubPath, GroupId_Physics);
		}
		
		for(int i=11; i<192; i++)
		{
			CREATE_ZONEINDEX_NOUSE()
		}
		
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Armor");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		CREATE_ZONEINDEX_NOUSE()
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Mine");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Kit");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Stand");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Flame trap right");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Flame trap left");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		
		CREATE_ZONEINDEX_NOUSE()
		
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Saw blade");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Mine 2");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Mine 3");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Barrel");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Lazer");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Monster");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Powerup");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Robot 1");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Robot 2");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Switch");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Door 1");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Door 2");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Door 3");
			pAsset->SetIndexGroup(SubPath, GroupId_Entities);
		}
	}
	
	//EntityType, Spawn
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoSpawn", ImageEntitiesPath, 0, 0, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsSpawn");
		pAsset->SetCollisionRadius(64.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Blue Spawn
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoBlueSpawn", ImageEntitiesPath, 1, 0, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsBlueSpawn");
		pAsset->SetCollisionRadius(64.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Red Spawn
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoRedSpawn", ImageEntitiesPath, 2, 0, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsRedSpawn");
		pAsset->SetCollisionRadius(64.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Weapon
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoChainsaw", ImageEntitiesPath, 0, 3, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsChainsaw");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Weapon
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoShotgun", ImageEntitiesPath, 1, 3, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsShotgun");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Weapon
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoGrenade", ImageEntitiesPath, 2, 3, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsGrenade");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Weapon
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoRifle", ImageEntitiesPath, 3, 3, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsRifle");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Weapon
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoElectric", ImageEntitiesPath, 4, 3, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsElectric");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Weapon
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoLaser", ImageEntitiesPath, 5, 3, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsLaser");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Weapon
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoFlamer", ImageEntitiesPath, 6, 3, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsFlamer");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Red Flag
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoRedFlag", ImageEntitiesPath, 7, 0, 1, 2);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsRedFlag");
		pAsset->SetCollisionRadius(42.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Blue Flag
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoBlueFlag", ImageEntitiesPath, 7, 2, 1, 2);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsBlueFlag");
		pAsset->SetCollisionRadius(42.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Heart
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoHeart", ImageEntitiesPath, 0, 1, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("nsHealth");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV LAB */
	PackageId = pKernel->AssetsManager()->NewPackage("env_lab");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageCredits(PackageId, "Ninslash");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	CreateNewImage(pKernel, PackageId, "labMisc", "images/env_lab/lab_misc.png", CStorage::TYPE_ALL, 16, 16, true, 2);
	CreateNewImage(pKernel, PackageId, "labMain", "images/env_lab/lab_main.png", CStorage::TYPE_ALL, 16, 16, true, 2);
	CreateNewImage(pKernel, PackageId, "labBackground", "images/env_lab/lab_background.png", CStorage::TYPE_ALL, 16, 16, true, 2);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV FACTORY */
	PackageId = pKernel->AssetsManager()->NewPackage("env_factory");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageCredits(PackageId, "Ninslash");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	CreateNewImage(pKernel, PackageId, "factoryMain", "images/env_factory/factory_main.png", CStorage::TYPE_ALL, 16, 16, true, 2);
	CreateNewImage(pKernel, PackageId, "factoryBackground", "images/env_factory/factory_background.png", CStorage::TYPE_ALL, 16, 16, true, 2);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	pKernel->Shutdown();
	delete pKernel;

	exit(EXIT_SUCCESS);
}
