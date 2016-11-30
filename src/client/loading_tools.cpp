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

#include <client/loading_tools.h>
#include <shared/components/storage.h>

void CreateNewImage_LoadPng(CSharedKernel* pKernel, png_t& Png, unsigned char*& pBuffer, const char* pFilename, int StorageType)
{
	pBuffer = 0;
	
	// open file for reading
	png_init(0, 0);

	dynamic_string Buffer;
	IOHANDLE File = pKernel->Storage()->OpenFile(pFilename, IOFLAG_READ, StorageType, Buffer);
	if(File)
		io_close(File);
	else
	{
		dbg_msg("CreateNewImage", "failed to open file. filename='%s'", pFilename);
		return;
	}

	int Error = png_open_file(&Png, Buffer.buffer());
	if(Error != PNG_NO_ERROR)
	{
		dbg_msg("CreateNewImage", "failed to open file. filename='%s', error=%s", pFilename, png_error_string(Error));
		if(Error != PNG_FILE_ERROR)
			png_close_file(&Png);
		return;
	}

	if(Png.depth != 8)
	{
		dbg_msg("CreateNewImage", "invalid depth. filename='%s', depth='%d'", Png.depth);
		png_close_file(&Png);
		return;
	}
	else if(Png.color_type != PNG_TRUECOLOR && Png.color_type != PNG_TRUECOLOR_ALPHA)
	{
		dbg_msg("CreateNewImage", "invalid color type. filename='%s', color_type='%d'", Png.color_type);
		png_close_file(&Png);
		return;
	}
	else if(Png.width > (2<<12) || Png.height > (2<<12))
	{
		dbg_msg("CreateNewImage", "invalid size. filename='%s', width='%d', height='%d'", Png.width, Png.height);
		png_close_file(&Png);
		return;
	}

	pBuffer = new unsigned char[Png.width * Png.height * Png.bpp];
	Error = png_get_data(&Png, pBuffer);
	if(Error != PNG_NO_ERROR)
	{
		dbg_msg("CreateNewImage", "failed to get data from '%s', error=%s", Buffer.buffer(), png_error_string(Error));
		png_close_file(&Png);
		delete[] pBuffer;
		pBuffer = 0;
		return;
	}
	png_close_file(&Png);
}

void CreateNewImage_CopyData(png_t& Png, unsigned char*& pBuffer, CAsset_Image* pImage)
{
	array2d<uint8>& Data = pImage->GetDataArray();
	
	int BBP = 1;
	switch(Png.color_type)
	{
		case PNG_TRUECOLOR:
			BBP = 3;
			break;
		case PNG_TRUECOLOR_ALPHA:
			BBP = 4;
			break;
	}
	
	Data.resize(Png.width, Png.height, BBP);
	
	for(int j=0; j<Png.height; j++)
	{
		for(int i=0; i<Png.width; i++)
		{
			int Value = 0;
			if(Png.color_type == PNG_TRUECOLOR)
			{
				Data.set_clamp(i, j, 0, pBuffer[(j*Png.width+i)*Png.bpp+0]);
				Data.set_clamp(i, j, 1, pBuffer[(j*Png.width+i)*Png.bpp+1]);
				Data.set_clamp(i, j, 2, pBuffer[(j*Png.width+i)*Png.bpp+2]);
			}
			else if(Png.color_type == PNG_TRUECOLOR_ALPHA)
			{
				Data.set_clamp(i, j, 0, pBuffer[(j*Png.width+i)*Png.bpp+0]);
				Data.set_clamp(i, j, 1, pBuffer[(j*Png.width+i)*Png.bpp+1]);
				Data.set_clamp(i, j, 2, pBuffer[(j*Png.width+i)*Png.bpp+2]);
				Data.set_clamp(i, j, 3, pBuffer[(j*Png.width+i)*Png.bpp+3]);
			}
		}
	}
				
	delete[] pBuffer;
}

CAssetPath CreateNewImage(CSharedKernel* pKernel, int PackageId, const char* pName, const char* pFilename, int StorageType, int GridWidth, int GridHeight, bool TilingEnabled, int GridSpacing)
{
	png_t Png;
	unsigned char* pBuffer;
	
	CreateNewImage_LoadPng(pKernel, Png, pBuffer, pFilename, StorageType);
	if(!pBuffer)
		return CAssetPath::Null();
	
	CAssetPath ImagePath;
	CAsset_Image* pImage = pKernel->AssetsManager()->NewAsset_Hard<CAsset_Image>(&ImagePath, PackageId);
	pImage->SetName(pName);
	pImage->SetGridWidth(GridWidth < 1 ? Png.width/64 : GridWidth);
	pImage->SetGridHeight(GridHeight < 1 ? Png.height/64 : GridHeight);
	pImage->SetGridSpacing(GridSpacing);
	pImage->SetTilingEnabled(GridWidth == 16 && GridHeight == 16 && TilingEnabled);
	
	CreateNewImage_CopyData(Png, pBuffer, pImage);
	
	pKernel->AssetsManager()->RequestUpdate(ImagePath);
	
	return ImagePath;
}
