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

#include "unittest.h"

#include <shared/system/string.h>

TEST_BEGIN

TEST(char_lower('A') == 'a')
TEST(char_lower('b') == 'b')
TEST(char_lower('c') != 'C')
TEST(char_lower('8') == '8')
TEST(char_lower('?') == '?')
TEST(char_lower(0) == 0)

TEST(str_length("") == 0)
TEST(str_length("g") == 1)
TEST(str_length("tee") == 3)
TEST(str_length("tee universes") == 13)

TEST(str_check_pathname("a/normal/path"))
TEST(str_check_pathname("an/another/./normal/path"))
TEST(!str_check_pathname("an/evil/../path"))
TEST(!str_check_pathname(".."))

{
	string<_fixed_string_core<16> > String;
	
	String.copy("test");
	TEST_WITH_OUTPUT(String == "test", String.buffer())
	
	String.append(" hello");
	TEST_WITH_OUTPUT(String == "test hello", String.buffer())
}

{
	string<_dynamic_string_core<16> > String;
	
	String.copy("test");
	TEST_WITH_OUTPUT(String == "test", String.buffer())
	
	String.append(" hello universe");
	TEST_WITH_OUTPUT(String == "test hello universe", String.buffer())
	TEST(String.maxsize() == 32)
	
	String.clear();
	TEST_WITH_OUTPUT(String == "", String.buffer())
	
	String.copy("teeworlds");
	String.insert_at(3, "universe is a fork of tee");
	TEST_WITH_OUTPUT(String == "teeuniverse is a fork of teeworlds", String.buffer())
	
	String.clear();
	String.copy("123456789");
	String.insert_at(4, "a");
	TEST_WITH_OUTPUT(String == "1234a56789", String.buffer())
}

TEST_END
