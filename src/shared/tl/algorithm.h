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

#include <utility>

#ifndef __SHARED_TL_ALGORITHM__
#define __SHARED_TL_ALGORITHM__

template<typename T>
void relative_move(std::vector<T>& v, int id, int shift)
{
	if(id < 0 || id >= static_cast<int>(v.size()))
		return;
	
	if(id + shift >= static_cast<int>(v.size()))
		shift = static_cast<int>(v.size())-id-1;
	if(id + shift < 0)
		shift = -id;
	
	if(shift == 0)
		return;
	
	T Tmp = std::move(v[id]);
	if(shift < 0)
	{
		for(int i=id; i>id+shift; i--)
			v[i] = std::move(v[i-1]);
	}
	else
	{
		for(int i=id; i<id+shift; i++)
			v[i] = std::move(v[i+1]);
	}
	v[id+shift] = std::move(Tmp);
}
#endif
