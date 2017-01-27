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
	CSharedKernel* pKernel = new CSharedKernel();
	if(!pKernel->Init(argc, (const char**) argv))
	{
		dbg_msg("Kernel", "unable to initialize shared kernel");
		exit(EXIT_FAILURE);
	}
	
	int PackageId = pKernel->AssetsManager()->NewPackage("sport");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	
	CAssetPath ImageZonesPath = CreateNewImage(pKernel, PackageId, "entities", "images/univ_sport/zones.png", CStorage::TYPE_ALL, 16, 16, true, 0);
	
	//Sport
	{
		CAssetPath AssetPath;
		CSubPath SubPath;
		
		CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
		pAsset->SetName("sport");
		pAsset->SetImagePath(ImageZonesPath);
		
		int GroupId_Goal = pAsset->AddGroup();
		int GroupId_Rule = pAsset->AddGroup();
		
		pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Goal), "Goal");
		pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Rule), "Rules");
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexUsed(SubPath, false);
		
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Goal");
			pAsset->SetIndexColor(SubPath, 1.0f);
			pAsset->SetIndexUsed(SubPath, true);
			pAsset->SetIndexBorderIndex(SubPath, 1);
			pAsset->SetIndexBorderColor(SubPath, vec4(193.0f/255.0f, 193.0f/255.0f, 193.0f/255.0f, 1.0f));
			pAsset->SetIndexGroup(SubPath, GroupId_Goal);
		}
		
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Red Team Goal");
			pAsset->SetIndexColor(SubPath, 1.0f);
			pAsset->SetIndexUsed(SubPath, true);
			pAsset->SetIndexBorderIndex(SubPath, 1);
			pAsset->SetIndexBorderColor(SubPath, vec4(255.0f/255.0f, 106.0f/255.0f, 106.0f/255.0f, 1.0f));
			pAsset->SetIndexGroup(SubPath, GroupId_Goal);
		}
		
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Blue Team Goal");
			pAsset->SetIndexColor(SubPath, 1.0f);
			pAsset->SetIndexUsed(SubPath, true);
			pAsset->SetIndexBorderIndex(SubPath, 1);
			pAsset->SetIndexBorderColor(SubPath, vec4(56.0f/255.0f, 153.0f/255.0f, 255.0f/255.0f, 1.0f));
			pAsset->SetIndexGroup(SubPath, GroupId_Goal);
		}
		
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Out of play");
			pAsset->SetIndexColor(SubPath, 1.0f);
			pAsset->SetIndexUsed(SubPath, true);
			pAsset->SetIndexBorderIndex(SubPath, 2);
			pAsset->SetIndexBorderColor(SubPath, vec4(193.0f/255.0f, 193.0f/255.0f, 193.0f/255.0f, 1.0f));
			pAsset->SetIndexGroup(SubPath, GroupId_Rule);
		}
	}
	
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	pKernel->Shutdown();
	delete pKernel;

	exit(EXIT_SUCCESS);
}
