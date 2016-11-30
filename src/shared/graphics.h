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

#ifndef __SHARED_GRAPHICS__
#define __SHARED_GRAPHICS__

/*
 * This class should not be in the shared folder.
 * However, tw07 images must be loaded in the assets editor
 */
 
class CImageInfo
{
public:
	enum
	{
		FORMAT_AUTO=-1,
		FORMAT_RGB=0,
		FORMAT_RGBA=1,
		FORMAT_ALPHA=2,
	};

	/* Variable: width
		Contains the width of the image */
	int m_Width;

	/* Variable: height
		Contains the height of the image */
	int m_Height;

	/* Variable: format
		Contains the format of the image. See <Image Formats> for more information. */
	int m_Format;

	/* Variable: data
		Pointer to the image data. */
	unsigned char* m_pData;
};

/*
 * This class should not be in the shared folder.
 * However, image assets are shared and must know texture handle for
 * the client implementation.
 */

class CTextureHandle
{
private:
	int m_Id;
	
public:
	CTextureHandle()
	: m_Id(-1)
	{}

	inline bool IsValid() const { return Id() >= 0; }
	inline int Id() const { return m_Id; }
	inline void SetId(int Id) { m_Id = Id; }
	inline void Invalidate() { m_Id = -1; }
};

#endif
