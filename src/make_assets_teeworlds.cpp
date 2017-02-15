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

#define CREATE_TILINGMATERIAL_CONDITION_NOTNULL(relx, rely) {\
	CSubPath CondPath = CAsset_TilingMaterial::SubPath_RuleCondition(RulePath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, RulePath, CAsset_TilingMaterial::TYPE_RULE_CONDITION));\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_TYPE, CAsset_TilingMaterial::CONDITIONTYPE_NOTNULL);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSX, relx);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSY, rely);\
}

#define CREATE_TILINGMATERIAL_CONDITION_NULL(relx, rely) {\
	CSubPath CondPath = CAsset_TilingMaterial::SubPath_RuleCondition(RulePath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, RulePath, CAsset_TilingMaterial::TYPE_RULE_CONDITION));\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_TYPE, CAsset_TilingMaterial::CONDITIONTYPE_NULL);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSX, relx);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSY, rely);\
}

#define CREATE_TILINGMATERIAL_CONDITION_INDEX(relx, rely, index) {\
	CSubPath CondPath = CAsset_TilingMaterial::SubPath_RuleCondition(RulePath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, RulePath, CAsset_TilingMaterial::TYPE_RULE_CONDITION));\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_TYPE, CAsset_TilingMaterial::CONDITIONTYPE_INDEX);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_VALUE, index);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSX, relx);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSY, rely);\
}

#define CREATE_TILINGMATERIAL_CONDITION_NOBORDER(relx, rely) {\
	CSubPath CondPath = CAsset_TilingMaterial::SubPath_RuleCondition(RulePath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, RulePath, CAsset_TilingMaterial::TYPE_RULE_CONDITION));\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_TYPE, CAsset_TilingMaterial::CONDITIONTYPE_NOBORDER);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSX, relx);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSY, rely);\
}

int main(int argc, char* argv[])
{
	CSharedKernel* pKernel = new CSharedKernel();
	if(!pKernel->Init(argc, (const char**) argv))
	{
		debug::ErrorStream("Kernel") << "unable to initialize shared kernel" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	int PackageId = -1;
	
	/* UNIV TEEWORLDS */
	PackageId = pKernel->AssetsManager()->NewPackage("teeworlds");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageCredits(PackageId, "TeeWorlds");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	
	CAssetPath ImageZonesPath = CreateNewImage(pKernel, PackageId, "zones", "images/univ_teeworlds/zones.png", CStorage::TYPE_ALL, 16, 16, true, 0);
	CAssetPath ImageEntitiesPath = CreateNewImage(pKernel, PackageId, "entities", "images/univ_teeworlds/entities.png", CStorage::TYPE_ALL, 4, 4);
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(ImageEntitiesPath, CSubPath::Null(), CAsset_Image::TEXELSIZE, 768);
	
	int GroupId_Physics = -1;
	int GroupId_Death = -1;

	//Zones
	{
		CAssetPath AssetPath;
		CSubPath SubPath;
		
		CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
		pAsset->SetName("teeworlds");
		pAsset->SetImagePath(ImageZonesPath);
		
		GroupId_Physics = pAsset->AddGroup();
		GroupId_Death = pAsset->AddGroup();
		
		pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Physics), "Physics");
		pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Death), "Death");
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexTitle(SubPath, "Air");
		pAsset->SetIndexColor(SubPath, vec4(1.0f, 1.0f, 1.0f, 0.0f));
		pAsset->SetIndexGroup(SubPath, GroupId_Physics);
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexTitle(SubPath, "Hookable Ground");
		pAsset->SetIndexColor(SubPath, 1.0f);
		pAsset->SetIndexBorderIndex(SubPath, 1);
		pAsset->SetIndexGroup(SubPath, GroupId_Physics);
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexTitle(SubPath, "Death");
		pAsset->SetIndexColor(SubPath, 1.0f);
		pAsset->SetIndexBorderIndex(SubPath, 2);
		pAsset->SetIndexBorderColor(SubPath, vec4(1.0f, 0.0f, 0.0f, 1.0f));
		pAsset->SetIndexGroup(SubPath, GroupId_Death);
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexTitle(SubPath, "Unhookable Ground");
		pAsset->SetIndexColor(SubPath, 1.0f);
		pAsset->SetIndexBorderIndex(SubPath, 1);
		pAsset->SetIndexBorderColor(SubPath, vec4(149.0f/255.0f, 190.0f/255.0f, 222.0f/255.0f, 1.0f));
		pAsset->SetIndexGroup(SubPath, GroupId_Physics);
	}
	
	//EntityType, Spawn
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoSpawn", ImageEntitiesPath, 0, 3, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("spawn");
		pAsset->SetCollisionRadius(64.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Blue Spawn
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoBlueSpawn", ImageEntitiesPath, 1, 3, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("blueSpawn");
		pAsset->SetCollisionRadius(64.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Red Spawn
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoRedSpawn", ImageEntitiesPath, 2, 3, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("redSpawn");
		pAsset->SetCollisionRadius(64.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Shotgun
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoShotgun", ImageEntitiesPath, 0, 2, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("shotgun");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Grenade
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoGrenade", ImageEntitiesPath, 1, 2, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("grenade");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Rifle
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoRifle", ImageEntitiesPath, 2, 2, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("rifle");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Gun
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoGun", ImageEntitiesPath, 0, 1, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("gun");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Hammer
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoHammer", ImageEntitiesPath, 1, 1, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("hammer");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Ninja
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoNinja", ImageEntitiesPath, 2, 1, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("ninja");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Red Flag
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoRedFlag", ImageEntitiesPath, 3, 0, 1, 2);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("redFlag");
		pAsset->SetCollisionRadius(42.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Blue Flag
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoBlueFlag", ImageEntitiesPath, 3, 2, 1, 2);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("blueFlag");
		pAsset->SetCollisionRadius(42.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Heart
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoHeart", ImageEntitiesPath, 0, 0, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("health");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	//EntityType, Armor
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoArmor", ImageEntitiesPath, 1, 0, 1, 1);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("armor");
		pAsset->SetCollisionRadius(20.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV CLOUDS */
	PackageId = pKernel->AssetsManager()->NewPackage("env_clouds");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageCredits(PackageId, "TeeWorlds");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	CreateNewImage(pKernel, PackageId, "cloud1", "images/env_clouds/bg_cloud1.png", CStorage::TYPE_ALL, 1, 1);
	CreateNewImage(pKernel, PackageId, "cloud2", "images/env_clouds/bg_cloud2.png", CStorage::TYPE_ALL, 1, 1);
	CreateNewImage(pKernel, PackageId, "cloud3", "images/env_clouds/bg_cloud3.png", CStorage::TYPE_ALL, 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV DESERT */
	{
		PackageId = pKernel->AssetsManager()->NewPackage("env_desert");
		pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
		pKernel->AssetsManager()->SetPackageCredits(PackageId, "TeeWorlds");
		pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
		pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
		
		CAssetPath MainImagePath = CreateNewImage(pKernel, PackageId, "desertMain", "images/env_desert/desert_main.png", CStorage::TYPE_ALL, 16, 16, true, 2);
		CreateNewImage(pKernel, PackageId, "desertDoodads", "images/env_desert/desert_doodads.png", CStorage::TYPE_ALL, 16, 16, true, 2);
		CreateNewImage(pKernel, PackageId, "desertMountains1", "images/env_desert/desert_mountains.png", CStorage::TYPE_ALL, 1, 1);
		CreateNewImage(pKernel, PackageId, "desertMountains2", "images/env_desert/desert_mountains2.png", CStorage::TYPE_ALL, 1, 1);
		CreateNewImage(pKernel, PackageId, "desertSun", "images/env_desert/desert_sun.png", CStorage::TYPE_ALL, 1, 1);
		
		CAssetPath RedRockTopPath;
		CAssetPath YellowRockTopPath;
		CAssetPath BrownRockTopPath;
		CAssetPath UndergroundCablePath;
		CAssetPath UndergroundCable2Path;
		CAssetPath UndergroundCableCap;
		CAssetPath UndergroundCableCornerConvex;
		CAssetPath UndergroundCableCornerConcave;
		
		CREATE_SPRITE_PATH(RedRockTopPath, PackageId, "redRockTop", MainImagePath, 0, 1, 1, 1);
		CREATE_SPRITE_PATH(YellowRockTopPath, PackageId, "yellowRockTop", MainImagePath, 5, 0, 1, 1);
		CREATE_SPRITE_PATH(BrownRockTopPath, PackageId, "brownRockTop", MainImagePath, 0, 6, 1, 1);
		CREATE_SPRITE_PATH(UndergroundCablePath, PackageId, "undergroundCable", MainImagePath, 7, 7, 1, 1);
		CREATE_SPRITE_PATH(UndergroundCable2Path, PackageId, "undergroundCable2", MainImagePath, 8, 5, 1, 1);
		CREATE_SPRITE_PATH(UndergroundCableCap, PackageId, "undergroundCableCap", MainImagePath, 9, 5, 1, 1);
		CREATE_SPRITE_PATH(UndergroundCableCornerConvex, PackageId, "undergroundCableCornerConvex", MainImagePath, 8, 6, 1, 1);
		CREATE_SPRITE_PATH(UndergroundCableCornerConcave, PackageId, "undergroundCableCornerConcave", MainImagePath, 9, 7, 1, 1);
		
		//Material: RedRock
		{
			CAssetPath MaterialPath;
			CAsset_PathMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_PathMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("redRock");
			pAsset->SetTextureEnabled(true);
			pAsset->SetTextureColor(vec4(188.0f/255.0f, 95.0f/255.0f, 53.0f/255.0f, 1.0f));
			pAsset->SetTextureSpacing(-16.0f);
			
			CSubPath LayerPath = CAsset_PathMaterial::SubPath_Layer(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_PathMaterial::TYPE_LAYER));
			
			CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
			pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
			pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
			pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, RedRockTopPath);
			pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.5f);
			pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
		}
		//Material: YellowRock
		{
			CAssetPath MaterialPath;
			CAsset_PathMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_PathMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("yellowRock");
			pAsset->SetTextureEnabled(true);
			pAsset->SetTextureColor(vec4(244.0f/255.0f, 209.0f/255.0f, 126.0f/255.0f, 1.0f));
			pAsset->SetTextureSpacing(-16.0f);
			
			CSubPath LayerPath = CAsset_PathMaterial::SubPath_Layer(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_PathMaterial::TYPE_LAYER));
			
			CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
			pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
			pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
			pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, YellowRockTopPath);
			pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.5f);
			pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
		}
		//Material: BrownRock
		{
			CAssetPath MaterialPath;
			CAsset_PathMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_PathMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("brownRock");
			pAsset->SetTextureEnabled(true);
			pAsset->SetTextureColor(vec4(129.0f/255.0f, 90.0f/255.0f, 71.0f/255.0f, 1.0f));
			pAsset->SetTextureSpacing(-16.0f);
			
			CSubPath LayerPath = CAsset_PathMaterial::SubPath_Layer(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_PathMaterial::TYPE_LAYER));
			
			CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
			pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
			pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
			pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, BrownRockTopPath);
			pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.5f);
			pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
		}
		//Material: Underground cable
		{
			CAssetPath MaterialPath;
			CAsset_PathMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_PathMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("undergroundCable");
			
			CSubPath LayerPath = CAsset_PathMaterial::SubPath_Layer(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_PathMaterial::TYPE_LAYER));
			
			{
				CSubPath SpritePath;
				
				for(int i=0; i<2; i++)
				{
					SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
					pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
					pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
					pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, UndergroundCablePath);
					pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.5f);
					pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FLAGS, CAsset_PathMaterial::SPRITEFLAG_ROTATION);
					pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_LINE);
					pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 0);
					pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 0);
				}
				
				SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, UndergroundCable2Path);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.5f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_LINE);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 0);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 0);
				
				for(int i=0; i<2; i++)
				{
					SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
					pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
					pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
					pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, UndergroundCablePath);
					pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.5f);
					pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FLAGS, CAsset_PathMaterial::SPRITEFLAG_ROTATION);
					pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_LINE);
					pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 0);
					pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 0);
				}
			}
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, UndergroundCableCap);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.5f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FLAGS, CAsset_PathMaterial::SPRITEFLAG_VFLIP);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_CAP_START);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 0);
			}
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, UndergroundCableCap);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.5f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_CAP_END);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 0);
			}
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, UndergroundCableCornerConvex);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.5f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FLAGS, CAsset_PathMaterial::SPRITEFLAG_VFLIP);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_CORNER_CONVEX);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 0);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 0);
			}
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, UndergroundCableCornerConcave);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.5f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_CORNER_CONCAVE);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 0);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 0);
			}
		}
		
		pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	}
	
	/* ENV GENERIC */
	{
		PackageId = pKernel->AssetsManager()->NewPackage("env_generic");
		pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
		pKernel->AssetsManager()->SetPackageCredits(PackageId, "TeeWorlds");
		pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
		pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
		CreateNewImage(pKernel, PackageId, "genericSpikes", "images/env_generic/generic_deathtiles.png", CStorage::TYPE_ALL, 16, 16, true, 2);
		CAssetPath UnhookableImagePath = CreateNewImage(pKernel, PackageId, "genericUnhookable", "images/env_generic/generic_unhookable.png", CStorage::TYPE_ALL, 16, 16, true, 2);
		
		//TilingMaterial: SmallUnhookable
		{
			CAssetPath MaterialPath;
			CAsset_TilingMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_TilingMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("smallUnhookable");
			pAsset->SetImagePath(UnhookableImagePath);
			
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 23);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 24);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 25);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 8);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 9);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 39);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 40);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 41);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
		}
		//TilingMaterial: smallBlueUnhookable
		{
			CAssetPath MaterialPath;
			CAsset_TilingMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_TilingMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("smallBlueUnhookable");
			pAsset->SetImagePath(UnhookableImagePath);
			
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 16);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 17);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 18);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 1);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 2);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 32);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 33);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 34);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
		}
		//TilingMaterial: smallRedUnhookable
		{
			CAssetPath MaterialPath;
			CAsset_TilingMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_TilingMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("smallRedUnhookable");
			pAsset->SetImagePath(UnhookableImagePath);
			
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 144);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 145);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 146);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 129);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 130);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 160);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 161);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 162);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 0.125f);
				CREATE_TILINGMATERIAL_CONDITION_INDEX(0, 0, 3);
			}
		}
		
		pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	}
	
	/* ENV GRASS */
	{
		PackageId = pKernel->AssetsManager()->NewPackage("env_grass");
		pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
		pKernel->AssetsManager()->SetPackageCredits(PackageId, "TeeWorlds");
		pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
		pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
		CAssetPath MainImagePath = CreateNewImage(pKernel, PackageId, "grassMain", "images/env_grass/grass_main.png", CStorage::TYPE_ALL, 16, 16, true, 2);
		CreateNewImage(pKernel, PackageId, "grassDoodads", "images/env_grass/grass_doodads.png", CStorage::TYPE_ALL, 16, 16, true, 2);
		
		CAssetPath GrassPath;
		CAssetPath GrassCornerConvexPath;
		CAssetPath GrassCornerConcavePath;
		CAssetPath GrassToDirtPath;
		CAssetPath DirtToGrassPath;
		CAssetPath DirtPath;
		CAssetPath DirtCornerConvexPath;
		CAssetPath DirtCornerConcavePath;
		CAssetPath GrassDirtCornerPath;
		CAssetPath DarkDirtPath;
		CAssetPath DirtTransitionPath;
		
		CREATE_SPRITE_PATH(GrassPath, PackageId, "grass", MainImagePath, 0, 1, 1, 1);
		CREATE_SPRITE_PATH(GrassCornerConvexPath, PackageId, "grassCornerConvex", MainImagePath, 1, 2, 1, 1);
		CREATE_SPRITE_PATH(GrassCornerConcavePath, PackageId, "grassCornerConcave", MainImagePath, 0, 3, 1, 1);
		
		CREATE_SPRITE_PATH(DirtPath, PackageId, "dirt", MainImagePath, 4, 1, 1, 1);
		CREATE_SPRITE_PATH(DirtCornerConvexPath, PackageId, "dirtCornerConvex", MainImagePath, 5, 2, 1, 1);
		CREATE_SPRITE_PATH(DirtCornerConcavePath, PackageId, "dirtCornerConcave", MainImagePath, 6, 3, 1, 1);
		
		CREATE_SPRITE_PATH(DirtToGrassPath, PackageId, "grassToDirt", MainImagePath, 6, 1, 1, 1);
		CREATE_SPRITE_PATH(GrassToDirtPath, PackageId, "dirtToGrass", MainImagePath, 6, 2, 1, 1);
		CREATE_SPRITE_PATH(GrassDirtCornerPath, PackageId, "grassDirtCorner", MainImagePath, 5, 0, 1, 1);
		
		CREATE_SPRITE_PATH(DarkDirtPath, PackageId, "darkDirt", MainImagePath, 8, 1, 1, 1);
		CREATE_SPRITE_PATH(DirtTransitionPath, PackageId, "dirtTransition", MainImagePath, 8, 5, 1, 1);
		
		//TilingMaterial: GrassAndDirt
		{
			CAssetPath MaterialPath;
			CAsset_TilingMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_TilingMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("grassAndDirt");
			pAsset->SetImagePath(MainImagePath);
			
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 2);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/150.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOBORDER(0, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 3);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/150.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOBORDER(0, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 66);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/150.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOBORDER(0, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 67);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/150.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOBORDER(0, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 68);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/150.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOBORDER(0, 0);
			}
			//top
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 16);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
			}
			//right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 21);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
			}
			//bottom
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 52);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
			}
			//left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 20);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
			}
			//corner top-right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 5);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
			}
			//corner top-left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 4);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
			}
			//corner bottom-left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 36);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
			}
			//corner bottom-right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 37);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
			}
			//inside corner top-right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 54);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
			}
			//inside corner top-left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 53);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
			}
			//inside corner bottom-left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 49);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
			}
			//inside corner bottom-right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 48);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
			}
			//right transition
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 22);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
			}
			//left transition
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 38);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
			}
			//left grass
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 19);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -2);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 1);
			}
			//right grass
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 17);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -2);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 1);
			}
			//top corner right 2
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 33);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 33);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 2);
			}
			//top corner left 2
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 32);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 32);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 2);
			}
		}
		
		//TilingMaterial: Cave
		{
			CAssetPath MaterialPath;
			CAsset_TilingMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_TilingMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("cave");
			pAsset->SetImagePath(MainImagePath);
			
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 13);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 29);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/150.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOBORDER(0, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 42);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/150.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOBORDER(0, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 43);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/150.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOBORDER(0, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 44);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/150.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOBORDER(0, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 45);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/150.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOBORDER(0, 0);
			}
			//top
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 26);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
			}
			//right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 25);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
			}
			//bottom
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 10);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
			}
			//left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 24);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
			}
			//corner top-left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 8);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
			}
			//corner top-right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 9);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
			}
			//corner bottom-left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 40);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
			}
			//corner bottom-right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 41);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
			}
			//inside corner top-left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 11);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
			}
			//inside corner top-right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 12);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
			}
			//inside corner bottom-left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 27);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
			}
			//inside corner bottom-right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 28);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
			}
		}

		//Material: GrassAndDirt
		{
			CAssetPath MaterialPath;
			CAsset_PathMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_PathMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("grassAndDirt");
			pAsset->SetTextureEnabled(true);
			pAsset->SetTextureColor(vec4(161.0f/255.0f, 110.0f/255.0f, 54.0f/255.0f, 1.0f));
			pAsset->SetTextureSpacing(-32.0f);
			
			//Labels
			{
				CSubPath LabelPath = CAsset_PathMaterial::SubPath_Label(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_PathMaterial::TYPE_LABEL));
				pKernel->AssetsManager()->SetAssetValue_Hard<vec4>(MaterialPath, LabelPath, CAsset_PathMaterial::LABEL_COLOR, vec4(144.0f/255.0f, 1.0f, 0.0f, 0.5f));
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, LabelPath, CAsset_PathMaterial::LABEL_ANGLESTART, -Pi/4.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, LabelPath, CAsset_PathMaterial::LABEL_ANGLEEND, Pi/4.0f);
			}
			{
				CSubPath LabelPath = CAsset_PathMaterial::SubPath_Label(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_PathMaterial::TYPE_LABEL));
				pKernel->AssetsManager()->SetAssetValue_Hard<vec4>(MaterialPath, LabelPath, CAsset_PathMaterial::LABEL_COLOR, vec4(1.0f, 143.0f/255.0f, 0.0f, 0.5f));
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, LabelPath, CAsset_PathMaterial::LABEL_ANGLESTART, Pi/4.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, LabelPath, CAsset_PathMaterial::LABEL_ANGLEEND, 7.0f*Pi/4.0f);
			}
			
			//Layers
			CSubPath LayerPath = CAsset_PathMaterial::SubPath_Layer(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_PathMaterial::TYPE_LAYER));
			
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, GrassPath);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.516129032f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_LINE);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 0);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 0);
			}
			
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, DirtPath);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.516129032f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FLAGS, CAsset_PathMaterial::SPRITEFLAG_ROTATION);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_LINE);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 1);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 1);
			}
			
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, GrassToDirtPath);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.516129032f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FLAGS, CAsset_PathMaterial::SPRITEFLAG_ROTATION | CAsset_PathMaterial::SPRITEFLAG_VFLIP);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_LINE);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 0);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 1);
			}
			
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, DirtToGrassPath);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.516129032f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FLAGS, CAsset_PathMaterial::SPRITEFLAG_ROTATION | CAsset_PathMaterial::SPRITEFLAG_HFLIP);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_LINE);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 1);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 0);
			}
			
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, GrassCornerConvexPath);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.516129032f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_X, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_CORNER_CONVEX);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 0);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 0);
			}
			
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, GrassCornerConcavePath);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.516129032f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_X, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_CORNER_CONCAVE);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 0);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 0);
			}
			
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, DirtCornerConvexPath);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.516129032f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_X, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FLAGS, CAsset_PathMaterial::SPRITEFLAG_HFLIP);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_CORNER_CONVEX);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 1);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 1);
			}
			
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, DirtCornerConcavePath);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.516129032f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_X, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FLAGS, CAsset_PathMaterial::SPRITEFLAG_HFLIP);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_CORNER_CONCAVE);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 1);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 1);
			}
			
			{
				CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
				pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, GrassDirtCornerPath);
				pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.516129032f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_X, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILETYPE, CAsset_PathMaterial::SPRITETILE_CORNER_CONVEX);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL0, 0);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_TILELABEL1, 1);
			}
		}
		
		//Material: DarkDirt
		{
			CAssetPath MaterialPath;
			CAsset_PathMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_PathMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("darkDirt");
			pAsset->SetTextureEnabled(true);
			pAsset->SetTextureColor(vec4(98.0f/255.0f, 73.0f/255.0f, 45.0f/255.0f, 1.0f));
			pAsset->SetTextureSpacing(-16.0f);
			
			CSubPath LayerPath = CAsset_PathMaterial::SubPath_Layer(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_PathMaterial::TYPE_LAYER));
			
			CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
			pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
			pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
			pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, DarkDirtPath);
			pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.5f);
			pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
			pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FLAGS, CAsset_PathMaterial::SPRITEFLAG_ROTATION);
		}
		
		//Material: DirtTransition
		{
			CAssetPath MaterialPath;
			CAsset_PathMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_PathMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("dirtTransition");
			pAsset->SetTextureEnabled(true);
			pAsset->SetTextureColor(vec4(84.0f/255.0f, 62.0f/255.0f, 36.0f/255.0f, 1.0f));
			pAsset->SetTextureSpacing(-16.0f);
			
			CSubPath LayerPath = CAsset_PathMaterial::SubPath_Layer(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_PathMaterial::TYPE_LAYER));
			
			CSubPath SpritePath = CAsset_PathMaterial::SubPath_LayerSprite(LayerPath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, LayerPath, CAsset_PathMaterial::TYPE_LAYER_SPRITE));
			pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_ALIGNMENT, CAsset_PathMaterial::SPRITEALIGN_STRETCHED);
			pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FILLING, CAsset_PathMaterial::SPRITEFILLING_STRETCHING);
			pKernel->AssetsManager()->SetAssetValue_Hard<CAssetPath>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_PATH, DirtTransitionPath);
			pKernel->AssetsManager()->SetAssetValue_Hard<vec2>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_SIZE, 0.5f);
			pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_POSITION_Y, -16.0f);
			pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, SpritePath, CAsset_PathMaterial::LAYER_SPRITE_FLAGS, CAsset_PathMaterial::SPRITEFLAG_ROTATION);
		}
		
		pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	}
	
	/* ENV JUNGLE */
	{
		PackageId = pKernel->AssetsManager()->NewPackage("env_jungle");
		pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
		pKernel->AssetsManager()->SetPackageCredits(PackageId, "TeeWorlds");
		pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
		pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
		CAssetPath MainImagePath = CreateNewImage(pKernel, PackageId, "jungleMain", "images/env_jungle/jungle_main.png", CStorage::TYPE_ALL, 16, 16, true, 2);
		CreateNewImage(pKernel, PackageId, "jungleDoodads", "images/env_jungle/jungle_doodads.png", CStorage::TYPE_ALL, 16, 16, true, 2);
		CreateNewImage(pKernel, PackageId, "jungleSpikes", "images/env_jungle/jungle_deathtiles.png", CStorage::TYPE_ALL, 16, 16, true, 2);
		CreateNewImage(pKernel, PackageId, "jungleUnhookable", "images/env_jungle/jungle_unhookables.png", CStorage::TYPE_ALL, 16, 16, true, 2);
		CreateNewImage(pKernel, PackageId, "jungleBackground", "images/env_jungle/jungle_background.png", CStorage::TYPE_ALL, 1, 1);
		CreateNewImage(pKernel, PackageId, "jungleMidground", "images/env_jungle/jungle_midground.png", CStorage::TYPE_ALL, 16, 16, true, 2);
		
		//TilingMaterial: Cave
		{
			CAssetPath MaterialPath;
			CAsset_TilingMaterial* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_TilingMaterial>(&MaterialPath, PackageId);
			pAsset->SetName("jungle");
			pAsset->SetImagePath(MainImagePath);
			
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
			}
			//random bricks
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 66);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/200.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOBORDER(0, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 67);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/200.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOBORDER(0, 0);
			}
			//top
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 16);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 96);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/15.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 97);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/15.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 98);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/15.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
			}
			//right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 21);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
			}
			//bottom
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 52);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 99);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/10.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 100);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/10.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 101);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/10.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
			}
			//left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 21);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEFLAGS, CAsset_MapLayerTiles::TILEFLAG_VFLIP);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
			}
			//corner top-right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 5);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
			}
			//corner top-left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 5);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEFLAGS, CAsset_MapLayerTiles::TILEFLAG_VFLIP);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
			}
			//corner bottom-left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 37);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEFLAGS, CAsset_MapLayerTiles::TILEFLAG_VFLIP);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 39);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEFLAGS, CAsset_MapLayerTiles::TILEFLAG_VFLIP);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/2.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
			}
			//corner bottom-right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 37);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 39);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/2.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, 1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
			}
			//inside corner top-right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 54);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 53);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEFLAGS, CAsset_MapLayerTiles::TILEFLAG_VFLIP);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/2.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
			}
			//inside corner top-left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 54);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEFLAGS, CAsset_MapLayerTiles::TILEFLAG_VFLIP);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 53);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/2.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
			}
			//inside corner bottom-left
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 48);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEFLAGS, CAsset_MapLayerTiles::TILEFLAG_VFLIP);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 49);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/3.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 50);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEFLAGS, CAsset_MapLayerTiles::TILEFLAG_HFLIP);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/3.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
			}
			//inside corner bottom-right
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 48);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 49);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEFLAGS, CAsset_MapLayerTiles::TILEFLAG_VFLIP);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/3.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
			}
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 51);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEFLAGS, CAsset_MapLayerTiles::TILEFLAG_HFLIP);
				pKernel->AssetsManager()->SetAssetValue_Hard<float>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_PROBABILITY, 1.0f/3.0f);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, -1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, -1);
			}
			//right bottom
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 22);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
			}
			//left bottom
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 22);
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEFLAGS, CAsset_MapLayerTiles::TILEFLAG_VFLIP);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 1);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 1);
			}
			//top corner right 2
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 33);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(1, 1);
			}
			//top corner left 2
			{
				CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, CSubPath::Null(), CAsset_TilingMaterial::TYPE_RULE));
				pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, RulePath, CAsset_TilingMaterial::RULE_TILEINDEX, 32);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(0, 0);
				CREATE_TILINGMATERIAL_CONDITION_NULL(0, -1);
				CREATE_TILINGMATERIAL_CONDITION_NULL(-1, 0);
				CREATE_TILINGMATERIAL_CONDITION_NOTNULL(-1, 1);
			}
		}
		
		pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	}
	
	/* ENV MOON */
	PackageId = pKernel->AssetsManager()->NewPackage("env_moon");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageCredits(PackageId, "TeeWorlds");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	CreateNewImage(pKernel, PackageId, "moon", "images/env_moon/moon.png", CStorage::TYPE_ALL, 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV MOUNTAINS */
	PackageId = pKernel->AssetsManager()->NewPackage("env_mountains");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageCredits(PackageId, "TeeWorlds");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	CreateNewImage(pKernel, PackageId, "mountains", "images/env_mountains/mountains.png", CStorage::TYPE_ALL, 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV SNOW */
	PackageId = pKernel->AssetsManager()->NewPackage("env_snow");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageCredits(PackageId, "TeeWorlds");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	CreateNewImage(pKernel, PackageId, "snow", "images/env_snow/snow.png", CStorage::TYPE_ALL, 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV STARS */
	PackageId = pKernel->AssetsManager()->NewPackage("env_stars");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageCredits(PackageId, "TeeWorlds");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	CreateNewImage(pKernel, PackageId, "stars", "images/env_stars/stars.png", CStorage::TYPE_ALL, 2, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV SUN */
	PackageId = pKernel->AssetsManager()->NewPackage("env_sun");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageCredits(PackageId, "TeeWorlds");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	CreateNewImage(pKernel, PackageId, "sun", "images/env_sun/sun.png", CStorage::TYPE_ALL, 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV WINTER */
	PackageId = pKernel->AssetsManager()->NewPackage("env_winter");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageCredits(PackageId, "TeeWorlds");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	CreateNewImage(pKernel, PackageId, "winterMain", "images/env_winter/winter_main.png", CStorage::TYPE_ALL, 16, 16, true, 2);
	CreateNewImage(pKernel, PackageId, "winterDoodads", "images/env_winter/winter_doodads.png", CStorage::TYPE_ALL, 16, 16, true, 2);
	CreateNewImage(pKernel, PackageId, "winterMountains1", "images/env_winter/winter_mountains.png", CStorage::TYPE_ALL, 1, 1);
	CreateNewImage(pKernel, PackageId, "winterMountains2", "images/env_winter/winter_mountains2.png", CStorage::TYPE_ALL, 1, 1);
	CreateNewImage(pKernel, PackageId, "winterMountains3", "images/env_winter/winter_mountains3.png", CStorage::TYPE_ALL, 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* SHUTDOWN */
	pKernel->Shutdown();
	delete pKernel;

	exit(EXIT_SUCCESS);
}
