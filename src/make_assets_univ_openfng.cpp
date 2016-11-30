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

int main(int argc, const char **argv)
{
	CSharedKernel* pKernel = new CSharedKernel();
	if(!pKernel->Init(argc, argv))
	{
		dbg_msg("Kernel", "unable to initialize shared kernel");
		exit(EXIT_FAILURE);
	}
	
	int PackageId = pKernel->AssetsManager()->NewPackage("univ_openfng");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	
	CAssetPath ImageEntitiesPath = CreateNewImage(pKernel, PackageId, "entities", "datasrc/images/univ_openfng/entities.png", CStorage::TYPE_ALL, 8, 4);
	pKernel->AssetsManager()->SetAssetValue_Hard<>(ImageEntitiesPath, CSubPath::Null(), CAsset_Image::TEXELSIZE, 768);
	
	//Shrine
	{
		CAssetPath AssetPath;
		CSubPath SubPath;
		
		CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
		pAsset->SetName("shrine");
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexUsed(SubPath, false);
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexDescription(SubPath, "Shrine");
		pAsset->SetIndexColor(SubPath, vec4(1.0f, 0.0f, 1.0f, 1.0f));
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexDescription(SubPath, "Red Team Shrine");
		pAsset->SetIndexColor(SubPath, vec4(1.0f, 0.0f, 0.0f, 1.0f));
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexDescription(SubPath, "Blue Team Shrine");
		pAsset->SetIndexColor(SubPath, vec4(0.0f, 0.0f, 1.0f, 1.0f));
	}
	
	//EntityType, Red Team Score
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoScoreRed", ImageEntitiesPath, 0, 1, 3, 3);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("redTeamScore");
		pAsset->SetCollisionRadius(64.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	
	//EntityType, Blue Team Score
	{
		CAssetPath GizmoPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoScoreBlue", ImageEntitiesPath, 3, 1, 3, 3);
		
		CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
		pAsset->SetName("blueTeamScore");
		pAsset->SetCollisionRadius(64.0f);
		pAsset->SetGizmoPath(GizmoPath);
	}
	
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	pKernel->Shutdown();
	delete pKernel;

	exit(EXIT_SUCCESS);
}
