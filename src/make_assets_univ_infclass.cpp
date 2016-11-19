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

int main(int argc, const char **argv)
{
	CSharedKernel* pKernel = new CSharedKernel();
	if(!pKernel->Init(argc, argv))
	{
		dbg_msg("Kernel", "unable to initialize server kernel");
		exit(EXIT_FAILURE);
	}
	
	int PackageId = pKernel->AssetsManager()->NewPackage("univ_infclass");
	
	//InfClass Physics
	{
		CAssetPath AssetPath;
		CSubPath SubPath;
		
		CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset<CAsset_ZoneType>(&AssetPath, PackageId, CAssetsHistory::NO_TOKEN);
		pAsset->SetName("infcPhysics");
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexName(SubPath, "Air");
		pAsset->SetIndexNumber(SubPath, 0);
		pAsset->SetIndexColor(SubPath, vec4(1.0f, 1.0f, 1.0f, 0.0f));
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexName(SubPath, "Hookable Ground");
		pAsset->SetIndexNumber(SubPath, 1);
		pAsset->SetIndexColor(SubPath, vec4(1.0f, 1.0f, 1.0f, 1.0f));
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexName(SubPath, "Unhookable Ground");
		pAsset->SetIndexNumber(SubPath, 3);
		pAsset->SetIndexColor(SubPath, vec4(228.0f/255.0f, 255.0f/255.0f, 0.0f/255.0f, 1.0f));
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexName(SubPath, "Water");
		pAsset->SetIndexNumber(SubPath, 2);
		pAsset->SetIndexColor(SubPath, vec4(0.3f, 0.3f, 1.0f, 1.0f));
	}
	
	//InfClass zones
	{
		CAssetPath AssetPath;
		CSubPath SubPath;
		
		CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset<CAsset_ZoneType>(&AssetPath, PackageId, CAssetsHistory::NO_TOKEN);
		pAsset->SetName("infcZones");
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexName(SubPath, "Infected area");
		pAsset->SetIndexNumber(SubPath, 2);
		pAsset->SetIndexColor(SubPath, vec4(0.0f, 0.7f, 0.0f, 1.0f));
		
		SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
		pAsset->SetIndexName(SubPath, "No spawn");
		pAsset->SetIndexNumber(SubPath, 3);
		pAsset->SetIndexColor(SubPath, vec4(1.0f, 0.3f, 1.0f, 1.0f));
	}
	
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	pKernel->Shutdown();
	delete pKernel;

	exit(EXIT_SUCCESS);
}
