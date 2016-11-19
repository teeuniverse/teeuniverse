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

#include <generated/assets/sprite.h>
#include <generated/assets/image.h>
#include <shared/components/assetsmanager.h>

int CAsset_Sprite::GetPixelWidth() const
{
	const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(m_ImagePath);
	if(pImage)
		return (pImage->GetDataWidth() * m_Width)/pImage->GetGridWidth();
	else
		return m_Width;
}

int CAsset_Sprite::GetPixelHeight() const
{
	const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(m_ImagePath);
	if(pImage)
		return (pImage->GetDataHeight() * m_Height)/pImage->GetGridHeight();
	else
		return m_Height;
}
