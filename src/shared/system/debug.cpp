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

/*
 * Some parts of this file comes from other projects.
 * These parts are itendified in this file by the following block:
 * 
 * FOREIGN CODE BEGIN: ProjectName *************************************
 * 
 * FOREIGN CODE END: ProjectName ***************************************
 * 
 * If ProjectName is "TeeWorlds", then this part of the code follows the
 * TeeWorlds licence:
 *      (c) Magnus Auvinen. See LICENSE_TEEWORLDS in the root of the
 *      distribution for more information. If you are missing that file,
 *      acquire a complete release at teeworlds.com.
 */

#include "string.h"
#include "debug.h"

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

#if defined(CONF_FAMILY_UNIX)
	#include <cstdarg>
	#include <cstdio>
	
	#define VSNPRINTF vsnprintf
	
	//I don't know if theses includes are needed
	#if defined(CONF_PLATFORM_MACOSX)
		#include <Carbon/Carbon.h>
	#endif
	
#elif defined(CONF_FAMILY_WINDOWS)

	#define VSNPRINTF _vsnprintf
	
	//I don't know if theses includes are needed
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <fcntl.h>
	#include <direct.h>
	#include <errno.h>
	#include <wincrypt.h>
	
#else
	#error NOT IMPLEMENTED
#endif

void dbg_break()
{
	*((volatile unsigned*)0) = 0x0;
}

void dbg_assert_imp(const char *filename, int line, int test, const char *msg)
{
	if(!test)
	{
		dbg_msg("assert", "%s(%d): %s", filename, line, msg);
		dbg_break();
	}
}

void dbg_msg(const char *sys, const char *fmt, ...)
{
	char str[1024];
	char str2[1024*3];

	str_format(str, sizeof(str), "[%s]: %s\n", sys, fmt);

	va_list args;
	va_start(args, fmt);
	VSNPRINTF(str2, sizeof(str2), str, args);
	printf("%s", str2);
	va_end(args);
}

/* FOREIGN CODE END: TeeWorlds ****************************************/

