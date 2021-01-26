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

#ifndef SHARED_TL_HASHTABLE_H
#define SHARED_TL_HASHTABLE_H

#include <shared/system/string.h>

#include <vector>

template <typename T, unsigned int TABLESIZE>
class hashtable
{
protected:
	typedef unsigned int HASH;

	class entry
	{
	public:
		dynamic_string m_Key;
		T m_Data;
		
		entry& operator=(const entry& old)
		{
			m_Key = old.m_Key;
			m_Data = old.m_Data;
			return *this;
		}
	};
	
protected:
	std::vector<entry> m_Table[TABLESIZE];

protected:
	HASH hash(const char* pKey) const
	{
		HASH Hash = 5381;
		for(; *pKey; pKey++)
			Hash = ((Hash << 5) + Hash) + (*pKey); /* Hash * 33 + c */
		return Hash%TABLESIZE;
	}

public:
	/*
		Function: clear
		 	Clear all entry in the hashtable
	*/
	void clear()
	{
		for(unsigned int i=0; i<TABLESIZE; i++)
			m_Table[i].clear();
	}

	/*
		Function: add
			Adds an item to the array.
	*/
	T* set(const char* pKey)
	{
		HASH Hash = hash(pKey);
		for(unsigned int i=0; i<m_Table[Hash].size(); i++)
		{
			if(str_comp(m_Table[Hash][i].m_Key.buffer(), pKey) == 0)
				return &m_Table[Hash][i].m_Data;
		}
		
		m_Table[Hash].emplace_back();
		entry& NewEntry = m_Table[Hash].back();
		NewEntry.m_Key = pKey;
		return &NewEntry.m_Data;
	}

	/*
		Function: add
			Adds an item to the array.
	*/
	T* set(const char* pKey, const T& Data)
	{
		HASH Hash = hash(pKey);
		for(unsigned int i=0; i<m_Table[Hash].size(); i++)
		{
			if(str_comp(m_Table[Hash][i].m_Key.buffer(), pKey) == 0)
			{
				m_Table[Hash][i].m_Data = Data;
				return &m_Table[Hash][i].m_Data;
			}
		}
		
		m_Table[Hash].emplace_back();
		entry& NewEntry = m_Table[Hash].back();
		NewEntry.m_Key = pKey;
		NewEntry.m_Data = Data;
		return &NewEntry.m_Data;
	}

	/*
		Function: remove
		 	Remove an element from its key
	*/
	void unset(const char* pKey)
	{
		HASH Hash = hash(pKey);
		for(unsigned int i=0; i<m_Table[Hash].size(); i++)
		{
			if(str_comp(m_Table[Hash][i].m_Key.buffer(), pKey) == 0)
			{
				m_Table[Hash].erase(m_Table[Hash].begin() + i);
				break;
			}
		}
	}

	/*
		Function: get
		 	Return a point to the element associated with pKey.
		 	If the element doesn't exist, the function return 0
	*/
	const T* get(const char* pKey) const
	{
		HASH Hash = hash(pKey);
		for(unsigned int i=0; i<m_Table[Hash].size(); i++)
		{
			if(str_comp(m_Table[Hash][i].m_Key.buffer(), pKey) == 0)
				return &m_Table[Hash][i].m_Data;
		}
		
		return 0;
	}

	T* get(unsigned int Id, unsigned int SubId)
	{
		if(Id < TABLESIZE && SubId < m_Table[Id].size())
			return &m_Table[Id][SubId].m_Data;
		else
			return 0;
	}

	const char* get_key(unsigned int Id, unsigned int SubId) const
	{
		if(Id < TABLESIZE && SubId < m_Table[Id].size())
			return m_Table[Id][SubId].m_Key.buffer();
		else
			return 0;
	}
	
	T* get(const char* pKey)
	{
		HASH Hash = hash(pKey);
		for(unsigned int i=0; i<m_Table[Hash].size(); i++)
		{
			if(str_comp(m_Table[Hash][i].m_Key.buffer(), pKey) == 0)
				return &m_Table[Hash][i].m_Data;
		}
		
		return 0;
	}
	
	int get_subtable_size(int Table) const
	{
		return m_Table[Table].size();
	}

public:
	class iterator
	{
	public:
		hashtable* m_pHashTable;
		unsigned int m_Id;
		unsigned int m_SubId;

	public:
		iterator() : m_pHashTable(0), m_Id(0), m_SubId(0) {}
		iterator(hashtable* pHashTable) : m_pHashTable(pHashTable), m_SubId(0)
		{
			for(m_Id=0; m_Id<TABLESIZE; m_Id++)
			{
				if(m_pHashTable->get_subtable_size(m_Id))
					break;
			}
		}
		iterator(hashtable* pHashTable, unsigned int Id, unsigned int SubId) : m_pHashTable(pHashTable), m_Id(Id), m_SubId(SubId) {}

		iterator& operator++()
		{
			if(m_Id < TABLESIZE)
			{
				m_SubId++;
				unsigned int TableSubSize = m_pHashTable->get_subtable_size(m_Id);
				while(m_SubId >= TableSubSize && m_Id < TABLESIZE)
				{
					m_Id++;
					m_SubId = 0;
					TableSubSize = m_pHashTable->get_subtable_size(m_Id);
				}
			}
			else
			{
				m_Id = TABLESIZE;
				m_SubId = 0;
			}
			
			return *this;
		}
		T* data() { return m_pHashTable->get(m_Id, m_SubId); }
		const char* key() const { return m_pHashTable->get_key(m_Id, m_SubId); }
		bool operator==(const iterator& Iter2) const { return (Iter2.m_pHashTable == m_pHashTable) && (Iter2.m_Id == m_Id) && (Iter2.m_SubId == m_SubId); }
		bool operator!=(const iterator& Iter2) const { return (Iter2.m_pHashTable != m_pHashTable) || (Iter2.m_Id != m_Id) || (Iter2.m_SubId != m_SubId); }
	};
	iterator begin() { return iterator(this); }
	iterator end() { return iterator(this, TABLESIZE, 0); }
};

#endif // TL_FILE_HASHTABLE_HPP
