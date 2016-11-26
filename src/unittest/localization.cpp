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

#include <shared/kernel.h>
#include <shared/components/localization.h>

TEST_BEGIN

CSharedKernel* pKernel = new CSharedKernel();
TEST(pKernel->Init(argc, argv))

TEST(str_comp(pKernel->Localization()->Localize(NULL, "__something not translated"), "__something not translated") == 0);
TEST(str_comp(pKernel->Localization()->Localize("xx_XX", "__something not translated"), "__something not translated") == 0);

{
	CLocalizableString LString("A big integer: {int:BigNumber}");
	LString.AddInteger("BigNumber", 41652864);
	
	dynamic_string Buffer;
	Buffer.clear();
	
	pKernel->Localization()->Format(Buffer, "xx_XX", LString);
	TEST_WITH_OUTPUT(str_comp(Buffer.buffer(), "A big integer: 41,652,864") == 0, Buffer.buffer())
}

pKernel->Shutdown();

delete pKernel;

TEST_END
