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

#include "debug.h"
#include "string.h"

namespace debug
{

#if defined(CONF_FAMILY_UNIX)
static int g_TerminalColorSupported = -1;

bool TerminalColorSupported()
{	
	if(g_TerminalColorSupported < 0)
	{
		g_TerminalColorSupported = 0;
		const char* pTerm = getenv("TERM");
		
		if(pTerm)
		{
			if(str_comp(pTerm, "xterm") == 0)
				g_TerminalColorSupported = 1;
		}
	}
	
	return g_TerminalColorSupported;
}
#endif

}
