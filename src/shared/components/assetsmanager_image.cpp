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

#include <shared/components/storage.h>
#include <external/pnglite/pnglite.h>

#include "assetsmanager.h"

bool CAssetsManager::Save_Image(const char* pFilename, int StorageType, const CAssetPath& ImagePath)
{
	CAsset_Image* pImage = GetAsset_Hard<CAsset_Image>(ImagePath);
	if(!pImage)
		return false;
	
	// open file for reading
	png_init(0, 0);

	dynamic_string Buffer;
	Storage()->GetCompletePath(StorageType, pFilename, Buffer);

	png_t Png;
	int Error;
	
	Error = png_open_file_write(&Png, Buffer.buffer());
	if(Error != PNG_NO_ERROR)
	{
		dbg_msg("CAssetsManager::Save_Image", "failed to open image for saving. filename='%s', error=%s", Buffer.buffer(), png_error_string(Error));
		if(Error != PNG_FILE_ERROR)
			png_close_file(&Png);
		return false;
	}
	
	array2d< uint8, allocator_default<uint8> >& ImageData = pImage->GetDataArray();
	
	int ColorType;
	switch(ImageData.get_depth())
	{
		case 1:
			ColorType = PNG_GREYSCALE;
			break;
		case 3:
			ColorType = PNG_TRUECOLOR;
			break;
		case 4:
			ColorType = PNG_TRUECOLOR_ALPHA;
			break;
		default:
			dbg_msg("CAssetsManager::Save_Image", "image depth incompatible with png (depth=%d)", ImageData.get_depth());
			png_close_file(&Png);
			return false;
	}
	
	Error = png_set_data(&Png, ImageData.get_width(), ImageData.get_height(), 8, ColorType, (unsigned char *)ImageData.base_ptr());
	if(Error != PNG_NO_ERROR)
	{
		dbg_msg("CAssetsManager::Save_Image", "failed to save image. filename='%s', error=%s", pFilename, png_error_string(Error));
		if(Error != PNG_FILE_ERROR)
			png_close_file(&Png);
		return false;
	}
	
	png_close_file(&Png);
	
	return true;
}
