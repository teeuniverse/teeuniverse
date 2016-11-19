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

#define CREATE_SPRITE(packageid, name, image, x, y, w, h) {\
	CAssetPath SpritePath;\
	CAsset_Sprite* pSprite = pKernel->AssetsManager()->NewAsset<CAsset_Sprite>(&SpritePath, packageid, CAssetsHistory::NO_TOKEN);\
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
		dbg_msg("Kernel", "unable to initialize server kernel");
		exit(EXIT_FAILURE);
	}
	
	int PackageId = pKernel->AssetsManager()->NewPackage("gui_cursor");
	
	CAssetPath ImagePath = CreateNewImage(pKernel, PackageId, "cursors", "datasrc/images/gui_cursor.png", 4, 5);
	
	CREATE_SPRITE(PackageId, "cursorPointer", ImagePath, 0, 2, 2, 3);
	CREATE_SPRITE(PackageId, "cursorTranslateX", ImagePath, 2, 1, 2, 2);
	CREATE_SPRITE(PackageId, "cursorTranslateY", ImagePath, 2, 3, 2, 2);
	CREATE_SPRITE(PackageId, "cursorText", ImagePath, 0, 0, 2, 2);
		
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	pKernel->Shutdown();
	delete pKernel;

	exit(EXIT_SUCCESS);
}
