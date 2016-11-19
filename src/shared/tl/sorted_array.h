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

/*
 * Some parts of this file comes from other projects.
 * These parts are itendified in this file by the following block:
 * 
 * FOREIGN CODE BEGIN: ProjectName *************************************
 * 
 * FOREIGN CODE END: ProjectName ***************************************
 * 
 * If ProjectName is "TeeWorlds", then this part of the code follows the
 * TeeWorlds licence:
 *      (c) Magnus Auvinen. See LICENSE_TEEWORLDS in the root of the
 *      distribution for more information. If you are missing that file,
 *      acquire a complete release at teeworlds.com.
 */

#ifndef __TL_SORTED_ARRAY__
#define __TL_SORTED_ARRAY__

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

#include <shared/system/debug.h>

#include "algorithm.h"
#include "array.h"

template <class T, class ALLOCATOR = allocator_default<T> >
class sorted_array : public array<T, ALLOCATOR>
{
	typedef array<T, ALLOCATOR> parent;

	// insert and size is not allowed
	int insert(const T& item, typename parent::range r);
	int set_size(int new_size);

public:
	typedef plain_range_sorted<T> range;

	T& add_by_copy(const T& item)
	{
		return parent::insert_by_copy(item, partition_binary(all(), item));
	}

	void add_unsorted(const T& item)
	{
		parent::add_by_copy(item);
	}

	void sort_range()
	{
		sort(all());
	}


	/*
		Function: all
			Returns a sorted range that contains the whole array.
	*/
	range all() const { return range(parent::m_pData, parent::m_pData+parent::m_Size); }
};

/* FOREIGN CODE END: TeeWorlds ****************************************/

#endif
