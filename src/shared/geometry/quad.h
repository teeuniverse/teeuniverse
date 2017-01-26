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

#ifndef __SHARED_GEOMETRY_QUAD__
#define __SHARED_GEOMETRY_QUAD__

#include <shared/math/vector.h>
#include <shared/assets/assetpath.h>

class CQuad
{
public:
	vec2 m_Position[4];
};

class CTexturedQuad
{
public:
	vec4 m_Color[4];
	vec2 m_Position[4];
	vec2 m_Texture[4];
	CAssetPath m_ImagePath;
	int m_TextureIndex;
};

void RotateQuadTexture(CTexturedQuad& Quad);
void TesselateQuad(const CTexturedQuad& Quad, array<CTexturedQuad>& Output, int TesselationX, int TesselationY);

#endif
