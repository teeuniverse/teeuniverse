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

#ifndef __SHARED_TL_ARRAY__
#define __SHARED_TL_ARRAY__

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

#include <shared/system/memory.h>
#include "range.h"
#include "allocator.h"

/*
	Class: array
		Normal dynamic array class

	Remarks:
		- Grows 50% each time it needs to fit new items
		- Use resize() if you know how many elements
		- Use optimize() to reduce the needed space.
*/
template <class T, class ALLOCATOR = allocator_default<T> >
class array : private ALLOCATOR
{
protected:
	T* m_pData;
	int m_MaxSize;
	int m_Size;
	
protected:
	void init()
	{
		m_pData = NULL;
		m_MaxSize = 0;
		m_Size = 0;
	}

public:
	typedef plain_range<T> range;

	/*
		Function: array constructor
	*/
	array()
	{
		init();
	}


	/*
		Function: array destructor
	*/
	~array()
	{
		if(m_pData)
			ALLOCATOR::free_array(m_pData);
	}

	/*
		Function: array copy
	*/
	void copy(const array &other)
	{
		if(m_pData)
			ALLOCATOR::free_array(m_pData);
		
		m_pData = ALLOCATOR::alloc_array(other.m_MaxSize);
		m_MaxSize = other.m_MaxSize;
		m_Size = other.m_Size;
		for(int i = 0; i < m_Size; i++)
			ALLOCATOR::copy(m_pData[i], other.m_pData[i]);
	}

	/*
		Function: array copy
	*/
	void transfert(array &other)
	{
		if(m_pData)
			ALLOCATOR::free_array(m_pData);
		
		m_pData = other.m_pData;
		m_MaxSize = other.m_MaxSize;
		m_Size = other.m_Size;
		
		other.m_pData = NULL;
		other.m_MaxSize = 0;
		other.m_Size = 0;
	}


	/*
		Function: clear

		Remarks:
			- Invalidates ranges
	*/
	void clear()
	{
		if(m_pData)
			ALLOCATOR::free_array(m_pData);
		
		m_pData = NULL;
		m_MaxSize = 0;
		m_Size = 0;
	}

	/*
		Function: size
	*/
	int size() const
	{
		return m_Size;
	}

	/*
		Function: remove_index_fast

		Remarks:
			- Invalidates ranges
	*/
	void remove_index_fast(int index)
	{
		ALLOCATOR::transfert(m_pData[index], m_pData[m_Size-1]);
		resize(size()-1);
	}

	/*
		Function: remove_fast

		Remarks:
			- Invalidates ranges
	*/
	void remove_fast(const T& item)
	{
		for(int i = 0; i < size(); i++)
			if(m_pData[i] == item)
			{
				remove_index_fast(i);
				return;
			}
	}

	/*
		Function: remove_index

		Remarks:
			- Invalidates ranges
	*/
	void remove_index(int index)
	{
		for(int i = index+1; i < m_Size; i++)
			ALLOCATOR::transfert(m_pData[i-1], m_pData[i]);

		resize(size()-1);
	}

	/*
		Function: remove

		Remarks:
			- Invalidates ranges
	*/
	bool remove(const T& item)
	{
		for(int i = 0; i < size(); i++)
			if(m_pData[i] == item)
			{
				remove_index(i);
				return true;
			}
		return false;
	}

	/*
		Function: add
			Adds an item to the array.

		Arguments:
			item - Item to add.

		Remarks:
			- Invalidates ranges
			- See remarks about <array> how the array grows.
	*/
	void add_by_copy(const T& item)
	{
		T& newItem = increment();
		ALLOCATOR::copy(newItem, item);
	}
	
	T& increment()
	{
		incsize();
		resize(size()+1);
		return m_pData[m_Size-1];
	}

	/*
		Function: insert
			Inserts an item into the array at a specified location.

		Arguments:
			item - Item to insert.
			index - Index where to insert the item

		Remarks:
			- Invalidates ranges
			- See remarks about <array> how the array grows.
	*/
	T& insertat(int index)
	{
		incsize();
		resize(size()+1);

		for(int i = m_Size-1; i > index; i--)
			ALLOCATOR::transfert(m_pData[i], m_pData[i-1]);

		return m_pData[index];
	}

	/*
		Function: insert
			Inserts an item into the array at a specified location.

		Arguments:
			item - Item to insert.
			index - Index where to insert the item

		Remarks:
			- Invalidates ranges
			- See remarks about <array> how the array grows.
	*/
	T& insertat_by_copy(const T& item, int index)
	{
		T& newitem = insertat(index);
		ALLOCATOR::copy(newitem, item);
		return newitem;
	}

	/*
		Function: insert
			Inserts an item into the array at a specified location.

		Arguments:
			item - Item to insert.
			r - Range where to insert the item

		Remarks:
			- Invalidates ranges
			- See remarks about <array> how the array grows.
	*/
	T& insert_by_copy(const T& item, range r)
	{
		if(r.empty())
		{
			T& newItem = increment();
			ALLOCATOR::copy(newItem, item);
			return newItem;
		}
		
		return insertat_by_copy(item, (int)(&r.front()-m_pData));
	}

	/*
		Function: operator[]
	*/
	T& operator[] (int index)
	{
		dbg_assert(index >= 0 && index < size(), "Wrong index");
		return m_pData[index];
	}

	/*
		Function: const operator[]
	*/
	const T& operator[] (int index) const
	{
		dbg_assert(index >= 0 && index < size(), "Wrong index");
		return m_pData[index];
	}

	/*
		Function: base_ptr
	*/
	T *base_ptr()
	{
		return m_pData;
	}

	/*
		Function: base_ptr
	*/
	const T *base_ptr() const
	{
		return m_pData;
	}

	/*
		Function: set_size
			Resizes the array to the specified size.

		Arguments:
			new_size - The new size for the array.
	*/
	void resize(int new_size)
	{
		if(m_MaxSize < new_size)
			alloc(new_size);
		m_Size = new_size;
	}

	/*
		Function: hint_size
			Allocates the number of elements wanted but
			does not increase the list size.

		Arguments:
			hint - Size to allocate.

		Remarks:
			- If the hint is smaller then the number of elements, nothing will be done.
			- Invalidates ranges
	*/
	void hint_size(int hint)
	{
		if(m_Size < hint)
			alloc(hint);
	}
	
	void relative_move(int id, int shift)
	{
		if(id < 0 || id >= m_Size)
			return;
		
		if(id + shift >= m_Size)
			shift = m_Size-id-1;
		if(id + shift < 0)
			shift = -id;
		
		if(shift == 0)
			return;
		
		T Tmp;
		ALLOCATOR::transfert(Tmp, m_pData[id]);
		if(shift < 0)
		{
			for(int i=id; i>id+shift; i--)
				ALLOCATOR::transfert(m_pData[i], m_pData[i-1]);
		}
		else
		{
			for(int i=id; i<id+shift; i++)
				ALLOCATOR::transfert(m_pData[i], m_pData[i+1]);
		}
		ALLOCATOR::transfert(m_pData[id+shift], Tmp);
	}

	/*
		Function: all
			Returns a range that contains the whole array.
	*/
	range all() const { return range(m_pData, m_pData+m_Size); }
protected:

	void incsize()
	{
		if(m_Size == m_MaxSize)
		{
			if(m_MaxSize < 2)
				alloc(m_MaxSize+1);
			else
				alloc(m_MaxSize+m_MaxSize/2);
		}
	}

	void alloc(int new_len)
	{
		m_MaxSize = new_len;
		T *pNewData = ALLOCATOR::alloc_array(m_MaxSize);

		int end = m_Size < m_MaxSize ? m_Size : m_MaxSize;
		for(int i = 0; i < end; i++)
			ALLOCATOR::transfert(pNewData[i], m_pData[i]);

		ALLOCATOR::free_array(m_pData);

		m_Size = m_Size < m_MaxSize ? m_Size : m_MaxSize;
		m_pData = pNewData;
	}
};

/* FOREIGN CODE END: TeeWorlds ****************************************/

#endif
