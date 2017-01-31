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

#ifndef __GAME_KERNEL__
#define __GAME_KERNEL__

#include <client/kernel.h>

//TAG_NEW_GAME_COMPONENT
class CCountryFlags;
class CGuiGame;

class CGameKernel: public CClientKernel
{
public:
	class CGuest : public CClientKernel::CGuest
	{
	private:
		CGameKernel* m_pKernel;
		
	public:
		CGuest(CGameKernel* pKernel) : CClientKernel::CGuest(pKernel), m_pKernel(pKernel) { }
		virtual ~CGuest() {}
		
		inline CGameKernel* Kernel() { return m_pKernel; }
		inline const CGameKernel* Kernel() const { return m_pKernel; }
		
		//TAG_NEW_GAME_COMPONENT
		inline CCountryFlags* CountryFlags() { return m_pKernel->CountryFlags(); }
		inline CGuiGame* Gui() { return m_pKernel->Gui(); }
	};
	
	class CComponent : public CGuest, public CSharedKernel::IComponent
	{
	public:
		CComponent(CGameKernel* pKernel) : CGuest(pKernel) { }
		virtual ~CComponent() {}
	};

private:
	//TAG_NEW_GAME_COMPONENT
	CCountryFlags* m_pCountryFlags;
	CGuiGame* m_pGui;
	
public:	
	CGameKernel();
	virtual ~CGameKernel() {}
	
	//TAG_NEW_GAME_COMPONENT
	inline CCountryFlags* CountryFlags() { return m_pCountryFlags; }
	inline CGuiGame* Gui() { return m_pGui; }
	
	//TAG_NEW_GAME_COMPONENT
	inline const CCountryFlags* CountryFlags() const { return m_pCountryFlags; }
	inline const CGuiGame* Gui() const { return m_pGui; }
};

#endif
