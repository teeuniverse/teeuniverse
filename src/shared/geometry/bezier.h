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

#ifndef __SHARED_GEOMETRY_BEZIER__
#define __SHARED_GEOMETRY_BEZIER__

#include <shared/math/vector.h>

class CLineVertex
{
public:
	vec4 m_Color;
	vec2 m_Position;
	vec2 m_Thickness;
	float m_Weight;
};

class CBezierVertex : public CLineVertex
{
public:
	enum
	{
		TYPE_CORNER=0,
		TYPE_AUTOSMOOTH,
		NUM_TYPES,
	};
	
public:
	vec2 m_aControlPoints[2];
	int m_Type;
};

#endif
