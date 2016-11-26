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

int main(int argc, const char **argv)
{
	CSharedKernel* pKernel = new CSharedKernel();
	if(!pKernel->Init(argc, argv))
	{
		dbg_msg("Kernel", "unable to initialize shared kernel");
		exit(EXIT_FAILURE);
	}
	
	int PackageId = -1;
	
	/* UNIV TEEWORLDS */
	PackageId = pKernel->AssetsManager()->NewPackage("universes/teeworlds");
	
	CAssetPath ImageEntitiesPath = CreateNewImage(pKernel, PackageId, "entities", "datasrc/images/univ_teeworlds/entities.png", 4, 4);
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(ImageEntitiesPath, CSubPath::Null(), CAsset_Image::TEXELSIZE, 768);
	
	//Physics
	{
		CAssetPath AssetPath;
		CSubPath SubPath;
		
		CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
		pAsset->SetName("physics");
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexDescription(SubPath, "Air");
		pAsset->SetIndexColor(SubPath, vec4(1.0f, 1.0f, 1.0f, 0.0f));
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexDescription(SubPath, "Hookable Ground");
		pAsset->SetIndexColor(SubPath, vec4(1.0f, 1.0f, 1.0f, 1.0f));
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexDescription(SubPath, "Unhookable Ground");
		pAsset->SetIndexColor(SubPath, vec4(228.0f/255.0f, 255.0f/255.0f, 0.0f/255.0f, 1.0f));
	}
	
	//Damage zone
	{
		CAssetPath AssetPath;
		CSubPath SubPath;
		
		CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
		pAsset->SetName("damage");
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexDescription(SubPath, "No Damage");
		pAsset->SetIndexColor(SubPath, vec4(1.0f, 1.0f, 1.0f, 0.0f));
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexDescription(SubPath, "Death");
		pAsset->SetIndexColor(SubPath, vec4(1.0f, 0.5f, 0.5f, 1.0f));
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
		pAsset->SetName("laserRifle");
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
		pAsset->SetName("heart");
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
	PackageId = pKernel->AssetsManager()->NewPackage("environments/clouds");
	CreateNewImage(pKernel, PackageId, "cloud1", "datasrc/images/env_clouds/bg_cloud1.png", 1, 1);
	CreateNewImage(pKernel, PackageId, "cloud2", "datasrc/images/env_clouds/bg_cloud2.png", 1, 1);
	CreateNewImage(pKernel, PackageId, "cloud3", "datasrc/images/env_clouds/bg_cloud3.png", 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV DESERT */
	PackageId = pKernel->AssetsManager()->NewPackage("environments/desert");
	CreateNewImage(pKernel, PackageId, "desertMain", "datasrc/images/env_desert/desert_main.png", 16, 16, true, 1);
	CreateNewImage(pKernel, PackageId, "desertDoodads", "datasrc/images/env_desert/desert_doodads.png", 16, 16, true, 1);
	CreateNewImage(pKernel, PackageId, "desertMountains1", "datasrc/images/env_desert/desert_mountains.png", 1, 1);
	CreateNewImage(pKernel, PackageId, "desertMountains2", "datasrc/images/env_desert/desert_mountains2.png", 1, 1);
	CreateNewImage(pKernel, PackageId, "desertSun", "datasrc/images/env_desert/desert_sun.png", 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV GENERIC */
	PackageId = pKernel->AssetsManager()->NewPackage("environments/generic");
	CreateNewImage(pKernel, PackageId, "genericSpikes", "datasrc/images/env_generic/generic_deathtiles.png", 16, 16, true, 1);
	CreateNewImage(pKernel, PackageId, "genericUnhookable", "datasrc/images/env_generic/generic_unhookable.png", 16, 16, true, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV GRASS */
	PackageId = pKernel->AssetsManager()->NewPackage("environments/grass");
	CreateNewImage(pKernel, PackageId, "grassMain", "datasrc/images/env_grass/grass_main.png", 16, 16, true, 1);
	CreateNewImage(pKernel, PackageId, "grassDoodads", "datasrc/images/env_grass/grass_doodads.png", 16, 16, true, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV JUNGLE */
	PackageId = pKernel->AssetsManager()->NewPackage("environments/jungle");
	CreateNewImage(pKernel, PackageId, "jungleMain", "datasrc/images/env_jungle/jungle_main.png", 16, 16, true, 1);
	CreateNewImage(pKernel, PackageId, "jungleDoodads", "datasrc/images/env_jungle/jungle_doodads.png", 16, 16, true, 1);
	CreateNewImage(pKernel, PackageId, "jungleSpikes", "datasrc/images/env_jungle/jungle_deathtiles.png", 16, 16, true, 1);
	CreateNewImage(pKernel, PackageId, "jungleUnhookable", "datasrc/images/env_jungle/jungle_unhookables.png", 16, 16, true, 1);
	CreateNewImage(pKernel, PackageId, "jungleBackground", "datasrc/images/env_jungle/jungle_background.png", 1, 1);
	CreateNewImage(pKernel, PackageId, "jungleMidground", "datasrc/images/env_jungle/jungle_midground.png", 16, 16, true, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV MOON */
	PackageId = pKernel->AssetsManager()->NewPackage("environments/moon");
	CreateNewImage(pKernel, PackageId, "moon", "datasrc/images/env_moon/moon.png", 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV MOUNTAINS */
	PackageId = pKernel->AssetsManager()->NewPackage("environments/mountains");
	CreateNewImage(pKernel, PackageId, "mountains", "datasrc/images/env_mountains/mountains.png", 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV SNOW */
	PackageId = pKernel->AssetsManager()->NewPackage("environments/snow");
	CreateNewImage(pKernel, PackageId, "snow", "datasrc/images/env_snow/snow.png", 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV STARS */
	PackageId = pKernel->AssetsManager()->NewPackage("environments/stars");
	CreateNewImage(pKernel, PackageId, "stars", "datasrc/images/env_stars/stars.png", 2, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV SUN */
	PackageId = pKernel->AssetsManager()->NewPackage("environments/sun");
	CreateNewImage(pKernel, PackageId, "sun", "datasrc/images/env_sun/sun.png", 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* ENV WINTER */
	PackageId = pKernel->AssetsManager()->NewPackage("environments/winter");
	CreateNewImage(pKernel, PackageId, "winterMain", "datasrc/images/env_winter/winter_main.png", 16, 16, true, 1);
	CreateNewImage(pKernel, PackageId, "winterDoodads", "datasrc/images/env_winter/winter_doodads.png", 16, 16, true, 1);
	CreateNewImage(pKernel, PackageId, "winterMountains1", "datasrc/images/env_winter/winter_mountains.png", 1, 1);
	CreateNewImage(pKernel, PackageId, "winterMountains2", "datasrc/images/env_winter/winter_mountains2.png", 1, 1);
	CreateNewImage(pKernel, PackageId, "winterMountains3", "datasrc/images/env_winter/winter_mountains3.png", 1, 1);
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	/* SHUTDOWN */
	pKernel->Shutdown();
	delete pKernel;

	exit(EXIT_SUCCESS);
}
