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

int main(int argc, char* argv[])
{
	{
		std::unique_ptr<CSharedKernel> pKernel(new CSharedKernel());
		
		if(!pKernel->Init(argc, (const char**) argv))
		{
			debug::ErrorStream("Kernel") << "unable to initialize shared kernel" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		int PackageId = -1;
		
		PackageId = pKernel->AssetsManager()->NewPackage("infclass");
		pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
		pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
		pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
		
		CAssetPath ImageZonesDamagePath = CreateNewImage(pKernel.get(), PackageId, "damage", "images/univ_infclass/damage.png", CStorage::TYPE_ALL, 16, 16, true, 0);
		CAssetPath ImageZonesTeleportationPath = CreateNewImage(pKernel.get(), PackageId, "teleportation", "images/univ_infclass/teleportation.png", CStorage::TYPE_ALL, 16, 16, true, 0);
		CAssetPath ImageZonesBonusPath = CreateNewImage(pKernel.get(), PackageId, "bonus", "images/univ_infclass/bonus.png", CStorage::TYPE_ALL, 16, 16, true, 0);
		CAssetPath ImageEntitiesPath = CreateNewImage(pKernel.get(), PackageId, "entities", "images/univ_infclass/entities.png", CStorage::TYPE_ALL, 4, 4);
		pKernel->AssetsManager()->SetAssetValue_Hard<>(ImageEntitiesPath, CSubPath::Null(), CAsset_Image::TEXELSIZE, 768);
			
		//Zone, damage
		{
			CAssetPath AssetPath;
			CSubPath SubPath;
			
			CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
			pAsset->SetName("icDamage");
			pAsset->SetImagePath(ImageZonesDamagePath);
			
			int GroupId_Death = pAsset->AddGroup();
			int GroupId_State = pAsset->AddGroup();
			
			pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Death), "Death");
			pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_State), "State change");
			
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "No damage");
			
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Death zone");
			pAsset->SetIndexDescription(SubPath, "Any player touching this zone dies instantly");
			pAsset->SetIndexBorderIndex(SubPath, 1);
			pAsset->SetIndexBorderColor(SubPath, vec4(1.0f, 0.0f, 0.0f, 1.0f));
			pAsset->SetIndexGroup(SubPath, GroupId_Death);
			
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Death zone (non-undead only)");
			pAsset->SetIndexDescription(SubPath, "Any player touching this zone, that is not an undead, dies instantly");
			pAsset->SetIndexBorderIndex(SubPath, 1);
			pAsset->SetIndexBorderColor(SubPath, vec4(1.0f, 164.0f/255.0f, 0.0f, 1.0f));
			pAsset->SetIndexGroup(SubPath, GroupId_Death);
			
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Death zone (infected only)");
			pAsset->SetIndexDescription(SubPath, "Any infected touching this zone dies instantly");
			pAsset->SetIndexBorderIndex(SubPath, 1);
			pAsset->SetIndexBorderColor(SubPath, vec4(157.0f/255.0f, 0.0f, 1.0f, 1.0f));
			pAsset->SetIndexGroup(SubPath, GroupId_Death);
			
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Infected area");
			pAsset->SetIndexDescription(SubPath, "Any human touching this zone is immediately infected");
			pAsset->SetIndexBorderIndex(SubPath, 2);
			pAsset->SetIndexBorderColor(SubPath, vec4(11.0f/255.0f, 173.0f/255.0f, 0.0f, 1.0f));
			pAsset->SetIndexGroup(SubPath, GroupId_State);
		}	
		//Zone, teleportation
		{
			CAssetPath AssetPath;
			CSubPath SubPath;
			
			CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
			pAsset->SetName("icTele");
			pAsset->SetImagePath(ImageZonesTeleportationPath);
			
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "All accepted");
			pAsset->SetIndexDescription(SubPath, "Witches can spawn infected and scientist can be teleported in this zone");
			
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "No witch spawn");
			pAsset->SetIndexDescription(SubPath, "Witches can't spawn infected in this zone. This zone is nice to place over spikes and other dangerous parts of the map");
			pAsset->SetIndexBorderIndex(SubPath, 1);
			pAsset->SetIndexBorderColor(SubPath, vec4(1.0f, 0.0f, 0.0f, 1.0f));
			
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "No scientist teleport");
			pAsset->SetIndexDescription(SubPath, "Scientists can't be teleported in this zone");
			pAsset->SetIndexBorderIndex(SubPath, 2);
			pAsset->SetIndexBorderColor(SubPath, vec4(0.0f, 1.0f, 164.0f/255.0f, 1.0f));
		}
		//Zone, bonus
		{
			CAssetPath AssetPath;
			CSubPath SubPath;
			
			CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
			pAsset->SetName("icBonus");
			pAsset->SetImagePath(ImageZonesBonusPath);
			
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "No bonus");
			
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "+5/min");
			pAsset->SetIndexDescription(SubPath, "Humans get +5 points each time they accumulated 60 seconds by staying inside this zone");
			pAsset->SetIndexBorderIndex(SubPath, 1);
			pAsset->SetIndexBorderColor(SubPath, vec4(148.0f/255.0f, 0.0f, 1.0f, 1.0f));
		}
		//EntityType, Spawn
		{
			CAssetPath GizmoPath;
			CAssetPath AssetPath;
			
			CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoHumanSpawn", ImageEntitiesPath, 0, 3, 1, 1);
			
			CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
			pAsset->SetName("icHuman");
			pAsset->SetCollisionRadius(64.0f);
			pAsset->SetGizmoPath(GizmoPath);
		}
		//EntityType, Blue Spawn
		{
			CAssetPath GizmoPath;
			CAssetPath AssetPath;
			
			CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoInfectedSpawn", ImageEntitiesPath, 1, 3, 1, 1);
			
			CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
			pAsset->SetName("icInfected");
			pAsset->SetCollisionRadius(64.0f);
			pAsset->SetGizmoPath(GizmoPath);
		}
		//EntityType, Hero Flag
		{
			CAssetPath GizmoPath;
			CAssetPath AssetPath;
			
			CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoHeroFlag", ImageEntitiesPath, 3, 2, 1, 2);
			
			CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
			pAsset->SetName("icHeroFlag");
			pAsset->SetCollisionRadius(42.0f);
			pAsset->SetGizmoPath(GizmoPath);
		}
		
		pKernel->AssetsManager()->Save_AssetsFile(PackageId);
		
		pKernel->Shutdown();
	}

	exit(EXIT_SUCCESS);
}
