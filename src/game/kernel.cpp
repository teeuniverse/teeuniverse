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

#include <game/kernel.h>

//TAG_NEW_GAME_COMPONENT
#include <game/components/bindsmanager.h>
#include <game/components/countryflags.h>
#include <game/components/gui.h>

CGameKernel::CGameKernel()
{
	SetType(KERNEL_GAME);
	
	//TAG_NEW_GAME_COMPONENT
	m_pComponents.add_by_copy(m_pBindsManager = new CBindsManager(this));
	m_pComponents.add_by_copy(m_pCountryFlags = new CCountryFlags(this));
	m_pComponents.add_by_copy(m_pGui = new CGuiGame(this));
}

