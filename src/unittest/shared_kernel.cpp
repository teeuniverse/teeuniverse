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

#include <shared/kernel.h>
#include <shared/components/cli.h>

class COutputTest : public CCLI_Output
{
public:
	dynamic_string m_String;

	virtual void Print(const char* pText, int Type = CLI_LINETYPE_NORMAL)
	{
		m_String = pText;
	}
};

TEST_BEGIN

CSharedKernel* pKernel = new CSharedKernel();
TEST(pKernel->Init(argc, (const char**) argv))

COutputTest Output;

pKernel->CLI()->Execute("echo test", &Output);
TEST(Output.m_String == "test");

pKernel->Shutdown();

delete pKernel;

TEST_END
