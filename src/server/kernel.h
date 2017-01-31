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

#ifndef __SERVER_KERNEL__
#define __SERVER_KERNEL__

#include <shared/kernel.h>

class CKernel: public CSharedKernel
{
public:
	class CGuest : public CSharedKernel::CGuest
	{
	private:
		CKernel* m_pKernel;
		
	public:
		CGuest(CKernel* pKernel) : CSharedKernel::CGuest(pKernel), m_pKernel(pKernel) { }
		virtual ~CGuest() {}
	
		inline CKernel* Kernel() { return m_pKernel; }
		inline const CKernel* Kernel() const { return m_pKernel; }
	};
	
	class CComponent : public CGuest, CSharedKernel::IComponent
	{
	public:
		CComponent(CKernel* pKernel) : CGuest(pKernel) { }
		virtual ~CComponent() {}
	};

public:	
	CKernel();
	virtual ~CKernel() {}
};

#endif
