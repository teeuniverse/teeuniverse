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

#ifndef __SHARED_SYSTEM_DEBUG__
#define __SHARED_SYSTEM_DEBUG__

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

void dbg_break();

/*

	Function: dbg_assert
		Breaks into the debugger based on a test.

	Parameters:
		test - Result of the test.
		msg - Message that should be printed if the test fails.

	Remarks:
		Does nothing in release version of the library.

	See Also:
		<dbg_break>
*/
void dbg_assert(int test, const char *msg);

/*

	Function: dbg_assert
		Breaks into the debugger based on a test.

	Parameters:
		test - Result of the test.
		msg - Message that should be printed if the test fails.

	Remarks:
		Does nothing in release version of the library.

	See Also:
		<dbg_break>
*/
void dbg_assert(int test, const char *msg);
#define dbg_assert(test,msg) dbg_assert_imp(__FILE__, __LINE__, test, msg)
void dbg_assert_imp(const char *filename, int line, int test, const char *msg);

#ifdef __clang_analyzer__
#include <assert.h>
#undef dbg_assert
#define dbg_assert(test,msg) assert(test)
#endif

/*
	Function: dbg_msg

	Prints a debug message.

	Parameters:
		sys - A string that describes what system the message belongs to
		fmt - A printf styled format string.

	Remarks:
		Does nothing in release version of the library.

	See Also:
		<dbg_assert>
*/
void dbg_msg(const char *sys, const char *fmt, ...);

/* FOREIGN CODE END: TeeWorlds ****************************************/

#endif
