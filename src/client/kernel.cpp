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

#include <client/kernel.h>

//TAG_NEW_CLIENT_COMPONENT
#include <client/components/graphics.h>
#include <client/components/textrenderer.h>
#include <client/components/assetsrenderer.h>
#include <client/components/input.h>
#include <client/components/input.h>
#include <client/components/gui.h>
#include <client/components/bindsmanager.h>

CClientKernel::CClientKernel()
{
	SetType(KERNEL_CLIENT);
	
	//TAG_NEW_CLIENT_COMPONENT
	m_pComponents.emplace_back(m_pGraphics = new CGraphics(this));
	m_pComponents.emplace_back(m_pTextRenderer = new CTextRenderer(this));
	m_pComponents.emplace_back(m_pAssetsRenderer = new CAssetsRenderer(this));
	m_pComponents.emplace_back(m_pInput = new CInput(this));
	m_pComponents.emplace_back(m_pBindsManager = new CBindsManager(this));
}
