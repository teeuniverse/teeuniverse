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

#include "filler.h"
	
namespace gui
{

/* ABSTRACT LABEL *****************************************************/

CFiller::CFiller(CGui *pContext) :
	CWidget(pContext)
{
	
}

void CFiller::UpdateBoundingSize()
{
	m_BoundingSizeRect.BSNoConstraint();
}

void CFiller::Render()
{
	
}

}
