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

#include "unittest.h"

#include <shared/system/fs.h>

TEST_BEGIN

TEST(fs_makedir("__unittestdir__"))
TEST(fs_makedir("__unittestdir__/test1"))
TEST(fs_makedir("__unittestdir__/test2"))

fs_listdir_iterator* pIter = fs_create_listdir_iterator("__unittestdir__");
int Counter = 0;
while(pIter->next())
{
	const char* pFilename = pIter->get_filename();
	TEST(pFilename != NULL)
	TEST_WITH_OUTPUT(
		str_comp(pFilename, "__unittestdir__/test1") == 0 ||
		str_comp(pFilename, "__unittestdir__/test2") == 0 ||
		str_comp(pFilename, "__unittestdir__/.") == 0 ||
		str_comp(pFilename, "__unittestdir__/..") == 0,
		pFilename)
	
	Counter++;
}
delete pIter;
TEST_WITH_OUTPUT(Counter == 4, Counter)

TEST(fs_rename("__unittestdir__/test1", "__unittestdir__/test3"))

TEST(fs_remove("__unittestdir__/test2"))
TEST(fs_remove("__unittestdir__/test3"))
TEST(fs_remove("__unittestdir__"))

TEST_END
