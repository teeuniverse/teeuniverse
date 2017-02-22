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

#include <shared/components/assetsmanager.h>

#ifndef __SHARED_AUTOLAYER__
#define __SHARED_AUTOLAYER__

void ApplyTilingMaterials_Tiles(CAssetsManager* pAssetsManager, array2d<CAsset_MapLayerTiles::CTile>& Tiles, CAssetPath StylePath, int X, int Y, int W, int H, int Seed);
void ApplyTilingMaterials_Layer(CAssetsManager* pAssetsManager, CAssetPath LayerPath, int X, int Y, int W, int H, int Token);
void ApplyTilingMaterials_FullLayer(CAssetsManager* pAssetsManager, CAssetPath LayerPath, int Token);

#endif
