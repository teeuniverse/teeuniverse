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

#include "unittest.h"

#include <shared/kernel.h>
#include <shared/archivefile.h>
#include <shared/components/assetsmanager.h>

TEST_BEGIN

dynamic_string Dir;

CSharedKernel* pKernel = new CSharedKernel();
TEST(pKernel->Init(argc, argv))

{
	int PackageId = pKernel->AssetsManager()->NewPackage("unittestpkg");
	TEST_WITH_OUTPUT(PackageId == 0, PackageId)
	
	CAssetPath SpritePath;
	CAsset_Sprite* pSprite = pKernel->AssetsManager()->NewAsset_Hard<CAsset_Sprite>(&SpritePath, PackageId);
	TEST(pSprite)
	
	pSprite->SetName("mysprite");
	TEST_WITH_OUTPUT(str_comp(pSprite->GetName(), "mysprite") == 0, pSprite->GetName());
	
	const CAsset_Sprite* pSprite2 = pKernel->AssetsManager()->GetAsset<CAsset_Sprite>(SpritePath);
	TEST(pSprite2)
	
	const char* pSpriteName = pKernel->AssetsManager()->GetAssetValue<const char*>(SpritePath, CSubPath::Null(), CAsset::NAME, NULL);
	TEST(pSpriteName)
	TEST_WITH_OUTPUT(str_comp(pSpriteName, "mysprite") == 0, pSpriteName);
}

pKernel->Shutdown();
delete pKernel;

TEST_END
