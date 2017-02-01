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

#ifndef __SHARED_SYSTEM_DEBUG__
#define __SHARED_SYSTEM_DEBUG__

#include <iostream>
#include <cassert>

#if defined(CONF_FAMILY_UNIX)
	#define STREAMCOLOR_RED (TerminalColorSupported() ? "\033[41m" : "")
	#define STREAMCOLOR_YELLOW (TerminalColorSupported() ? "\033[33m" : "")
	#define STREAMCOLOR_DEFAULT (TerminalColorSupported() ? "\033[0m" : "")
#else
	#define STREAMCOLOR_RED ""
	#define STREAMCOLOR_YELLOW ""
	#define STREAMCOLOR_DEFAULT ""
#endif

namespace debug
{
	
#if defined(CONF_FAMILY_UNIX)
bool TerminalColorSupported();
#endif

inline std::ostream& InfoStream() { return (std::cout << " * "); }
inline std::ostream& WarningStream() { return (std::cerr << STREAMCOLOR_YELLOW << " * Warning: " << STREAMCOLOR_DEFAULT); }
inline std::ostream& ErrorStream() { return (std::cerr << STREAMCOLOR_RED << " * Error: " << STREAMCOLOR_DEFAULT); }

inline std::ostream& InfoStream(const char* pSource) { return (std::cout << " * [" << pSource << "]: "); }
inline std::ostream& WarningStream(const char* pSource) { return (std::cerr << STREAMCOLOR_YELLOW << " * Warning from " << pSource << ": " << STREAMCOLOR_DEFAULT); }
inline std::ostream& ErrorStream(const char* pSource) { return (std::cerr << STREAMCOLOR_RED << " * Error from " << pSource << ": " << STREAMCOLOR_DEFAULT); }

}

#endif
