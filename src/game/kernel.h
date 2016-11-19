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

#ifndef __GAME_KERNEL__
#define __GAME_KERNEL__

#include <client/kernel.h>

//TAG_NEW_GAME_COMPONENT
class CBindsManager;
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
		inline CGameKernel* Kernel() { return m_pKernel; }
		inline const CGameKernel* Kernel() const { return m_pKernel; }
		
		//TAG_NEW_GAME_COMPONENT
		inline CBindsManager* BindsManager() { return m_pKernel->BindsManager(); }
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
	CBindsManager* m_pBindsManager;
	CCountryFlags* m_pCountryFlags;
	CGuiGame* m_pGui;
	
public:	
	CGameKernel();
	
	//TAG_NEW_GAME_COMPONENT
	inline CBindsManager* BindsManager() { return m_pBindsManager; }
	inline CCountryFlags* CountryFlags() { return m_pCountryFlags; }
	inline CGuiGame* Gui() { return m_pGui; }
	
	//TAG_NEW_GAME_COMPONENT
	inline const CBindsManager* BindsManager() const { return m_pBindsManager; }
	inline const CCountryFlags* CountryFlags() const { return m_pCountryFlags; }
	inline const CGuiGame* Gui() const { return m_pGui; }
};

#endif
