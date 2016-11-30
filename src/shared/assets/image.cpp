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

#include <generated/assets/image.h>
#include <shared/math/math.h>

vec4 CAsset_Image::GetColor(int x, int y) const
{
	switch(m_Data.get_depth())
	{
		case 1:
			return vec4(0.0f, 0.0f, 0.0f, m_Data.get_clamp(x, y, 0)/255.0f);
		case 3:
			return vec4(m_Data.get_clamp(x, y, 0)/255.0f, m_Data.get_clamp(x, y, 1)/255.0f, m_Data.get_clamp(x, y, 2)/255.0f, 1.0f);
		case 4:
			return vec4(m_Data.get_clamp(x, y, 0)/255.0f, m_Data.get_clamp(x, y, 1)/255.0f, m_Data.get_clamp(x, y, 2)/255.0f, m_Data.get_clamp(x, y, 3)/255.0f);
	}
	
	return vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

vec4 CAsset_Image::Sample(vec2 uv) const
{
    int x0 = clamp(static_cast<int>(floor(uv.x * m_Data.get_width())), 0, m_Data.get_width()-1);
    int x1 = clamp(x0 + 1, 0, m_Data.get_width() - 1);
    int y0 = clamp(static_cast<int>(floor(uv.y * m_Data.get_height())), 0, m_Data.get_height()-1);
    int y1 = clamp(y0 + 1, 0, m_Data.get_height() - 1);
    
    float wx = uv.x * m_Data.get_width() - floor(uv.x * m_Data.get_width());
    float wy = uv.y * m_Data.get_height() - floor(uv.y * m_Data.get_height());
    
    vec4 value_y0 = GetColor(x0, y0) * (1.0f - wx) + GetColor(x1, y0) * wx;
    vec4 value_y1 = GetColor(x0, y1) * (1.0f - wx) + GetColor(x1, y1) * wx;
    vec4 value = value_y0 * (1.0f - wy) + value_y1 * wy;
    
    return value;
}
