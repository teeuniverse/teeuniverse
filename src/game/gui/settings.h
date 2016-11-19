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

#ifndef TU_CLIENT_MENU_SETTINGS_H
#define TU_CLIENT_MENU_SETTINGS_H

#include <client/gui/tabs.h>
#include <client/gui/listlayout.h>

class CSettings : public gui::CTabs
{
private:
	class CGuiGame* m_pMenu;
	gui::CWidget* m_pGeneralTab;
	gui::CWidget* m_pPlayerTab;
	gui::CWidget* m_pCharacterTab;
	gui::CWidget* m_pControlTab;
	gui::CWidget* m_pGraphicsTab;
	gui::CWidget* m_pSoundTab;
	
protected:
	gui::CWidget* CreateGeneralTab();
	gui::CWidget* CreatePlayerTab();
	gui::CWidget* CreateCharacterTab();
	gui::CWidget* CreateControlTab();
	gui::CWidget* CreateGraphicsTab();
	gui::CWidget* CreateSoundTab();
	char m_aSearchFlag[128];

public:
	CSettings(CGuiGame* pMenu);
	
	class CGuiGame* Menu() { return m_pMenu; }
};

#endif

