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

#ifndef __UNITTEST__
#define __UNITTEST__

#include <cstdlib>
#include <iostream>

#define TEST_BEGIN int main(int argc, const char **argv) \
{ \
	int __TestCounter = 0; \
	int __ErrorCounter = 0;

#define TEST_END if(__ErrorCounter) \
	{ \
		std::cerr << "Failure (" << __ErrorCounter << "/" << __TestCounter << ")" << std::endl; \
		exit(EXIT_FAILURE); \
	} \
	else \
	{ \
		std::cout << "Success (" << __TestCounter << "/" << __TestCounter << ")" << std::endl; \
		exit(EXIT_SUCCESS); \
	} \
}

#define TEST(expr) __TestCounter++; \
	if(!(expr)) \
	{ \
		__ErrorCounter++; \
		std::cerr << "Failed test at line " << __LINE__ << ": [" << #expr << "]" << std::endl; \
	}

#define TEST_WITH_OUTPUT(expr, output) __TestCounter++; \
	if(!(expr)) \
	{ \
		__ErrorCounter++; \
		std::cerr << "Failed test at line " << __LINE__ << ": [" << #expr << "] (" << output << ")" << std::endl; \
	}
	

#endif
