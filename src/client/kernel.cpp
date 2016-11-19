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

#include <client/kernel.h>

//TAG_NEW_CLIENT_COMPONENT
#include <client/components/graphics.h>
#include <client/components/textrenderer.h>
#include <client/components/assetsrenderer.h>
#include <client/components/input.h>
#include <client/components/input.h>
#include <client/components/gui.h>

CClientKernel::CClientKernel()
{
	SetType(KERNEL_CLIENT);
	
	//TAG_NEW_CLIENT_COMPONENT
	m_pComponents.add_by_copy(m_pGraphics = new CGraphics(this));
	m_pComponents.add_by_copy(m_pTextRenderer = new CTextRenderer(this));
	m_pComponents.add_by_copy(m_pAssetsRenderer = new CAssetsRenderer(this));
	m_pComponents.add_by_copy(m_pInput = new CInput(this));
}
