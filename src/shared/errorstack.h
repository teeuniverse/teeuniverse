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

#ifndef __SHARED_ERRORSTACK__
#define __SHARED_ERRORSTACK__

#include <shared/tl/array.h>
#include <shared/components/localization.h>


class CErrorStack
{	
public:
	class CErrorStackEntry
	{
	public:
		CLocalizableString m_Message;

		CErrorStackEntry()
		{
			
		}
		
		CErrorStackEntry(const CLocalizableString& LString)
		{
			m_Message.copy(LString);
		}
		
		void copy(const CErrorStackEntry& Stack)
		{
			m_Message.copy(Stack.m_Message);
		}
		
		void transfert(CErrorStackEntry& Stack)
		{
			m_Message.transfert(Stack.m_Message);
		}
	};
	
private:
	array< CErrorStackEntry, allocator_copy<CErrorStackEntry> > m_Errors;

public:
	CErrorStack()
	{
		
	}
	
	void AddError(const CLocalizableString& LString)
	{
		m_Errors.add_by_copy(LString);
	}
	
	int Size() const
	{
		return m_Errors.size();
	}
	
	const CLocalizableString& GetMessage(int i) const
	{
		return m_Errors[i].m_Message;
	}
};

#endif
