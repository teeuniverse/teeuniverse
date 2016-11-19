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

#ifndef __GAME_CONSOLE__
#define __GAME_CONSOLE__

#include <client/gui/panellayout.h>
#include <client/gui/listlayout.h>
#include <client/gui/text-edit.h>
#include <shared/components/cli.h>

class CConsole : public gui::CVPanelLayout, public CCLI_Output
{
private:
	char m_aCommandLine[256];

	gui::CAbstractListLayout* m_pConsoleList;
	gui::CAbstractTextEdit* m_pPrompt;
	gui::CAbstractListLayout* m_pHelpList;
	class CGuiGame* m_pMenu;

public:
	CConsole(CGuiGame* pMenu);
	
	class CGuiGame* Menu() { return m_pMenu; }
	
	virtual void Update(bool ParentEnabled);
	virtual void Print(const char* pText, int Type);
};

#endif

