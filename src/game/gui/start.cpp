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

#include <game/components/gui.h>
#include <game/gui/start.h>
#include <client/gui/label.h>
#include <client/gui/button.h>
#include <client/gui/boxlayout.h>
#include <client/gui/filler.h>

/* QUIT BUTTON ********************************************************/

class CQuitButton : public gui::CButton
{
protected:
	CGuiGame* m_pMenu;

	virtual void MouseClickAction()
	{
		m_pMenu->SetPage(CGuiGame::PAGE_QUIT);
	}
	
public:
	CQuitButton(CGuiGame* pMenu, const gui::CLocalizableString& LocalizationString, CAssetPath IconPath) :
		gui::CButton(pMenu, LocalizationString, IconPath),
		m_pMenu(pMenu)
	{
		SetButtonStyle(pMenu->m_Path_Button_Quit);
	}
};

/* PAGE BUTTON ********************************************************/

class CPageButton : public gui::CButton
{
protected:
	CGuiGame* m_pMenu;
	int m_Page;

	virtual void MouseClickAction()
	{
		m_pMenu->SetPage(m_Page);
	}
	
public:
	CPageButton(CGuiGame* pMenu, int Page, const gui::CLocalizableString& LocalizationString, CAssetPath IconPath) :
		gui::CButton(pMenu, LocalizationString, IconPath),
		m_pMenu(pMenu),
		m_Page(Page)
	{
		SetButtonStyle(pMenu->m_Path_Button_Page);
	}
};

/* UNIVERSE BUTTON ****************************************************/

class CUniverseButton : public gui::CButton
{
protected:
	CGuiGame* m_pMenu;

	virtual void MouseClickAction()
	{
		
	}
	
public:
	CUniverseButton(CGuiGame* pMenu, const char* pText) :
		gui::CButton(pMenu, pText),
		m_pMenu(pMenu)
	{
		SetButtonStyle(pMenu->m_Path_Button_Universe);
	}
};

/* START **************************************************************/

CStart::CStart(CGuiGame* pMenu) :
	gui::CHListLayout(pMenu),
	m_pMenu(pMenu)
{
	gui::CVListLayout* pVList = new gui::CVListLayout(Context());
	pVList->SetBoxStyle(Menu()->m_Path_Box_StartMenu);
	Add(new gui::CFiller(Context()), true);
	Add(pVList, false);
	Add(new gui::CFiller(Context()), true);
	
	gui::CLabel* pLogo = new gui::CLabel(Context(), "", Menu()->m_Path_Sprite_Logo);
	pLogo->SetLabelStyle(Menu()->m_Path_Label_Logo);
	pVList->Add(pLogo, false);
	
	{
		gui::CVListLayout* pSection = new gui::CVListLayout(Context());
		pSection->SetBoxStyle(Menu()->m_Path_Box_Section);
		pVList->Add(pSection, true);
		
		pSection->Add(new gui::CLabelHeader(Context(), _("Choose a universe to play")), false);
		
		gui::CBoxLayout* pUniverses = new gui::CBoxLayout(Context());
		pUniverses->Add(new CUniverseButton(Menu(), "TeeWorlds 0.6"));
		pUniverses->Add(new CUniverseButton(Menu(), "TeeWorlds 0.7"));
		pUniverses->Add(new CUniverseButton(Menu(), "DDNet"));
		pUniverses->Add(new CUniverseButton(Menu(), "Infection"));
		pUniverses->Add(new CUniverseButton(Menu(), "Dark Worlds"));
		pUniverses->Add(new CUniverseButton(Menu(), "All servers"));
		pUniverses->Add(new CUniverseButton(Menu(), "Favorites"));
		pSection->Add(pUniverses, true);
	}
	
	gui::CHListLayout* pButtons = new gui::CHListLayout(Context());
	pButtons->SetBoxStyle(Menu()->m_Path_Box_SectionList);
	pButtons->Add(new CPageButton(Menu(), CGuiGame::PAGE_DEMOS, _("Demos"), Menu()->m_Path_Sprite_Demos), false);
	pButtons->Add(new CPageButton(Menu(), CGuiGame::PAGE_SETTINGS, _("Settings"), Menu()->m_Path_Sprite_Settings), false);
	pButtons->Add(new CPageButton(Menu(), CGuiGame::PAGE_ASSETSEDITOR, _("Assets Editor"), Menu()->m_Path_Sprite_Editor), false);
	pButtons->Add(new CQuitButton(Menu(), _("Quit"), Menu()->m_Path_Sprite_Quit), true);
	pVList->Add(pButtons, false);
}

