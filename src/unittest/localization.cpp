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
TEST(str_comp(pKernel->Localization()->Localize("ar_FR", "__something not translated"), "__something not translated") == 0);

{
	const char* pTranslatedText = pKernel->Localization()->Localize("fr", "You must restart the game for all settings to take effect.");
	TEST_WITH_OUTPUT(
		str_comp(pTranslatedText, "Les changements prendront effet au prochain redémarrage.") == 0,
		pTranslatedText
	);
}

{
	dynamic_string Buffer;
	int Value;
	
	Buffer.clear();
	Value = 5;
	pKernel->Localization()->Format_LP(Buffer, "fr", Value, "{int:NumPlayers} players left ({percent:PlayerOnCapacity})", "NumPlayers", &Value, NULL);
	TEST_WITH_OUTPUT(str_comp(Buffer.buffer(), "5 participant·e·s restant·e·") == 0, Buffer.buffer())
	
	Buffer.clear();
	Value = 1;
	pKernel->Localization()->Format_LP(Buffer, "fr", Value, "{int:NumPlayers} players left", "NumPlayers", &Value, NULL);
	TEST_WITH_OUTPUT(str_comp(Buffer.buffer(), "un·e participant·e") == 0, Buffer.buffer())
}

{
	dynamic_string Buffer;
	int Value = 8254534;
	pKernel->Localization()->Format(Buffer, "ar", "{int:test}", "test", &Value, NULL);
	TEST_WITH_OUTPUT(str_comp(Buffer.buffer(), "٨٬٢٥٤٬٥٣٤") == 0, Buffer.buffer())
}

pKernel->Shutdown();

delete pKernel;

TEST_END
