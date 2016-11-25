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

#include <game/gui/settings.h>
#include <game/components/gui.h>
#include <client/gui/expand.h>
#include <client/gui/boxlayout.h>
#include <client/gui/toggle.h>
#include <client/gui/slider.h>
#include <client/gui/text-edit.h>
#include <client/gui/bind-edit.h>
#include <shared/components/localization.h>
#include <game/components/countryflags.h>
#include <game/components/bindsmanager.h>

/* CONFIG CHECKBOX ****************************************************/

class CConfigCheckBox : public gui::CToggle
{
protected:
	int* m_pValue;
	
	virtual bool GetValue() { return *m_pValue; }
	virtual void SetValue(bool Value) { *m_pValue = Value; }

public:
	CConfigCheckBox(CGui* pContext, int* pValue, const gui::CLocalizableString& LocalizableString) :
		gui::CToggle(pContext, LocalizableString),
		m_pValue(pValue)
	{ }
};

/* CONFIG SLIDER ******************************************************/

class CConfigSlider : public gui::CHSlider
{
protected:
	int* m_pValue;

public:
	CConfigSlider(CGui* pContext, int* pValue, int Min, int Max) :
		gui::CHSlider(pContext, Min, Max),
		m_pValue(pValue)
	{ }
	
	virtual int GetValue() const { return *m_pValue; }
	virtual void SetValue(int Value) { *m_pValue = Value; }
};

/* CONFIG DYNAMIC CAMERA **********************************************/

class CConfigDynamicCamera : public gui::CToggle
{
protected:
	CGuiGame* m_pMenu;
	
private:
	virtual bool GetValue()
	{
		return (m_pMenu->m_Cfg_MouseDeadzone > 0);
	}
	
	virtual void SetValue(bool Value)
	{
		if(Value)
		{
			m_pMenu->m_Cfg_MouseFollowfactor = 60;
			m_pMenu->m_Cfg_MouseMaxDistance = 1000;
			m_pMenu->m_Cfg_MouseDeadzone = 300;
		}
		else
		{
			m_pMenu->m_Cfg_MouseFollowfactor = 0;
			m_pMenu->m_Cfg_MouseMaxDistance = 400;
			m_pMenu->m_Cfg_MouseDeadzone = 0;
		}
	}

public:
	CConfigDynamicCamera(CGuiGame* pContext, const gui::CLocalizableString& LocalizableString) :
		gui::CToggle(pContext, LocalizableString),
		m_pMenu(pContext)
	{ }
};

/* CONFIG EXPAND ******************************************************/

class CConfigExpand : public gui::CExpand
{
protected:
	CGuiGame* m_pMenu;
	gui::CToggle* m_pToggle;
	
public:
	CConfigExpand(CGuiGame* pMenu, gui::CToggle* pToggle) :
		gui::CExpand(pMenu),
		m_pToggle(pToggle)
	{
		SetBoxStyle(m_pMenu->m_Path_Box_List);
		SetTitle(m_pToggle);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(m_pToggle->GetValue())
			ShowContent();
		else
			HideContent();
		
		gui::CExpand::Update(ParentEnabled);
	}
};

/* LANGUAGE BUTTON ****************************************************/

class CLanguageButton : public gui::CButton
{
private:
	fixed_string128 m_Filename;
	char m_aFilename[128];
	
	virtual void MouseClickAction()
	{
		Localization()->m_Cfg_MainLanguage.copy(m_Filename);
	}

public:
	CLanguageButton(CGui* pContext, const char* pName, const char* pFilename) :
		gui::CButton(pContext, pName)
	{
		str_copy(m_aFilename, pFilename, sizeof(m_aFilename));
	}
};

/* FLAG BUTTON ********************************************************/

class CFlagButton : public gui::CButton
{
private:
	const char* m_pSearch;
	int* m_pCode;
	int m_Index;
	CGuiGame* m_pMenu;
	
	virtual void MouseClickAction()
	{
		*m_pCode = m_pMenu->GameKernel()->CountryFlags()->GetFlagFromIndex(m_Index).m_Code;
	}

public:
	CFlagButton(CGuiGame* pContext, int* pCode, const char* pSearch, int Index) :
		gui::CButton(pContext, ""),
		m_pMenu(pContext),
		m_pSearch(pSearch),
		m_Index(Index),
		m_pCode(pCode)
	{
		CAssetPath SpritePath = m_pMenu->GameKernel()->CountryFlags()->GetFlagFromIndex(m_Index).m_SpritePath;
		const char* pCountryName = m_pMenu->GameKernel()->CountryFlags()->GetFlagFromIndex(m_Index).m_aEnglishName;
		
		SetButtonStyle(m_pMenu->m_Path_Button_Icon);
		SetIcon(SpritePath);
		SetText(pCountryName);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(str_length(m_pSearch))
		{
			const char* pCountryName = m_pMenu->GameKernel()->CountryFlags()->GetFlagFromIndex(m_Index).m_aEnglishName;
			
			if(str_find_nocase(pCountryName, m_pSearch))
				Enable();
			else
				Disable();
		}
		else
			Enable();
		
		gui::CButton::Update(ParentEnabled);
	}
};

/* FLAG EDIT **********************************************************/

class CFlagLabel : public gui::CLabel
{
private:
	int* m_pCode;
	CGuiGame* m_pMenu;

public:
	CFlagLabel(CGuiGame* pContext, int* pCode) :
		gui::CLabel(pContext, ""),
		m_pMenu(pContext),
		m_pCode(pCode)
	{
		
	}
	
	virtual void Update()
	{
		int Index = m_pMenu->GameKernel()->CountryFlags()->FindIndex(*m_pCode);
		CAssetPath SpritePath = m_pMenu->GameKernel()->CountryFlags()->GetFlagFromIndex(Index).m_SpritePath;
		const char* pCountryName = m_pMenu->GameKernel()->CountryFlags()->GetFlagFromIndex(Index).m_aEnglishName;
		
		SetIcon(SpritePath);
		SetText(pCountryName);
	}
};

/* CONTROL EDIT *******************************************************/

class CControlEdit : public gui::CHListLayout
{
protected:
	CGuiGame* m_pMenu;
	
protected:
	class CBindEdit : public gui::CAbstractBindEdit
	{
	protected:
		std::string m_Command;
		int m_BindNum;
		CGuiGame* m_pMenu;
		
		virtual int GetValue() const
		{
			return m_pMenu->GameKernel()->BindsManager()->CommandToKey(m_Command.c_str(), m_BindNum);
		}
		
		virtual void SetValue(int Key)
		{
			m_pMenu->GameKernel()->BindsManager()->Bind(Key, m_Command.c_str(), m_BindNum);
		}
	
	public:
		CBindEdit(CGuiGame* pContext, const char* pCommand, int BindNum) :
			CAbstractBindEdit(pContext),
			m_pMenu(pContext),
			m_Command(pCommand),
			m_BindNum(BindNum)
		{
			
		}
	};
	
public:
	CControlEdit(CGuiGame* pMenu, const gui::CLocalizableString& LocalizableString, const char* pCommand) :
		gui::CHListLayout(pMenu),
		m_pMenu(pMenu)
	{
		SetBoxStyle(m_pMenu->m_Path_Box_Item);
		Add(new gui::CLabel(Context(), LocalizableString), true);
		Add(new CBindEdit(m_pMenu, pCommand, 0), true);
		Add(new CBindEdit(m_pMenu, pCommand, 1), true);
	}
};

/* SETTINGS ***********************************************************/

CSettings::CSettings(CGuiGame* pMenu) :
	gui::CTabs(pMenu),
	m_pMenu(pMenu)
{
	SetTabsStyle(Menu()->m_Path_Tabs_Pages);
	
	m_aSearchFlag[0] = 0;
	
	m_pGeneralTab = CreateGeneralTab();
	m_pPlayerTab = CreatePlayerTab();
	m_pCharacterTab = CreateCharacterTab();
	m_pControlTab = CreateControlTab();
	m_pGraphicsTab = CreateGraphicsTab();
	m_pSoundTab = CreateSoundTab();
	
	AddTab(m_pGeneralTab, _("General"), CAssetPath::Null());
	AddTab(m_pPlayerTab, _("Player"), CAssetPath::Null());
	AddTab(m_pCharacterTab, _("Tee"), CAssetPath::Null());
	AddTab(m_pControlTab, _("Controls"), CAssetPath::Null());
	AddTab(m_pGraphicsTab, _("Graphics"), CAssetPath::Null());
	AddTab(m_pSoundTab, _("Sound"), CAssetPath::Null());
}

gui::CWidget* CSettings::CreateGeneralTab()
{
	gui::CHListLayout* pTab = new gui::CHListLayout(Context());
	pTab->SetBoxStyle(Menu()->m_Path_Box_Column);
	
	gui::CVScrollLayout* pLeftList = new gui::CVScrollLayout(Context());
	pLeftList->SetBoxStyle(Menu()->m_Path_Box_SectionList);
	pTab->Add(pLeftList, true);
	
	gui::CVScrollLayout* pRightList = new gui::CVScrollLayout(Context());
	pRightList->SetBoxStyle(Menu()->m_Path_Box_SectionList);
	pTab->Add(pRightList, true);
	
	//Game
	{
		gui::CVListLayout* pSection = new gui::CVListLayout(Context());
		pLeftList->Add(pSection, false);
		
		pSection->SetBoxStyle(Menu()->m_Path_Box_Section);
		pSection->Add(new gui::CLabelHeader(Context(), _("Game")), false);
		
		pSection->Add(new CConfigDynamicCamera(Menu(), _("Dynamic Camera")), false);
		pSection->Add(new CConfigCheckBox(Menu(), &Menu()->m_Cfg_AutoswitchWeapons, _("Switch weapon on pickup")), false);
		pSection->Add(new CConfigCheckBox(Menu(), &Menu()->m_Cfg_Showhud, _("Show ingame HUD")), false);
		pSection->Add(new CConfigCheckBox(Menu(), &Menu()->m_Cfg_ShowChatFriends, _("Show only chat messages from friends")), false);
		pSection->Add(new CConfigCheckBox(Menu(), &Menu()->m_Cfg_Showsocial, _("Show social")), false);
		{
			CConfigCheckBox* pTitle = new CConfigCheckBox(Context(), &Menu()->m_Cfg_Nameplates, _("Show name plates"));
			CConfigExpand* pExpand = new CConfigExpand(Menu(), pTitle);
			pExpand->Add(new CConfigCheckBox(Context(), &Menu()->m_Cfg_NameplatesAlways, _("Always show name plates")));
			
			gui::CHListLayout* pList = new gui::CHListLayout(Context());
			pList->SetBoxStyle(Menu()->m_Path_Box_Item);
			pList->Add(new gui::CLabel(Context(), _("Size"), Menu()->m_Path_Sprite_Void), false, 200);
			pList->Add(new CConfigSlider(Context(), &Menu()->m_Cfg_NameplatesSize, 0, 100), true);
			pExpand->Add(pList);
			
			pExpand->Add(new CConfigCheckBox(Context(), &Menu()->m_Cfg_NameplatesTeamcolors, _("Use team colors for name plates")));
			pSection->Add(pExpand, false);
		}
	}
	
	//Client
	{
		gui::CVListLayout* pSection = new gui::CVListLayout(Context());
		pLeftList->Add(pSection, false);
		
		pSection->SetBoxStyle(Menu()->m_Path_Box_Section);
		pSection->Add(new gui::CLabelHeader(Context(), _("Client")), false);
		
		{
			CConfigCheckBox* pTitle = new CConfigCheckBox(Menu(), &Menu()->m_Cfg_AutoDemoRecord, _("Automatically record demos"));
			CConfigExpand* pExpand = new CConfigExpand(Menu(), pTitle);
			pSection->Add(pExpand, false);
			
			gui::CHListLayout* pList = new gui::CHListLayout(Context());
			pList->SetBoxStyle(Menu()->m_Path_Box_Item);
			pList->Add(new gui::CLabel(Context(), _("Max"), Menu()->m_Path_Sprite_Void), false, 200);
			pList->Add(new CConfigSlider(Menu(), &Menu()->m_Cfg_AutoDemoMax, 0, 100), true);
			pExpand->Add(pList);
		}
		
		{
			CConfigCheckBox* pTitle = new CConfigCheckBox(Menu(), &Menu()->m_Cfg_AutoScreenshot, _("Automatically take game over screenshot"));
			CConfigExpand* pExpand = new CConfigExpand(Menu(), pTitle);
			pSection->Add(pExpand, false);
			
			gui::CHListLayout* pList = new gui::CHListLayout(Context());
			pList->SetBoxStyle(Menu()->m_Path_Box_Item);
			pList->Add(new gui::CLabel(Context(), _("Max"), Menu()->m_Path_Sprite_Void), false, 200);
			pList->Add(new CConfigSlider(Menu(), &Menu()->m_Cfg_AutoScreenshotMax, 0, 100), true);
			pExpand->Add(pList);
		}
	}
	
	//Language chooser
	{
		gui::CVListLayout* pSection = new gui::CVListLayout(Context());
		pRightList->Add(pSection, true);
		
		pSection->SetBoxStyle(Menu()->m_Path_Box_Section);
		pSection->Add(new gui::CLabelHeader(Context(), _("Language")), false);
		
		gui::CVScrollLayout* pLanguageList = new gui::CVScrollLayout(Context());
		pLanguageList->SetBoxStyle(Menu()->m_Path_Box_List);
		pSection->Add(pLanguageList, true);
		
		pLanguageList->Add(new CLanguageButton(Context(), "English", ""), false);
		for(int i=0; i<Localization()->m_pLanguages.size(); i++)
		{
			pLanguageList->Add(new CLanguageButton(Context(), Localization()->m_pLanguages[i]->GetName(), Localization()->m_pLanguages[i]->GetFilename()), false);
		}
	}
	
	return pTab;
}

gui::CWidget* CSettings::CreatePlayerTab()
{
	gui::CHListLayout* pTab = new gui::CHListLayout(Context());
	pTab->SetBoxStyle(Menu()->m_Path_Box_Column);
	
	gui::CVScrollLayout* pLeftList = new gui::CVScrollLayout(Context());
	pLeftList->SetBoxStyle(Menu()->m_Path_Box_SectionList);
	pTab->Add(pLeftList, true);
	
	gui::CVScrollLayout* pRightList = new gui::CVScrollLayout(Context());
	pRightList->SetBoxStyle(Menu()->m_Path_Box_SectionList);
	pTab->Add(pRightList, true);
	
	//Player informations
	{
		gui::CVListLayout* pSection = new gui::CVListLayout(Context());
		pLeftList->Add(pSection, false);
		
		pSection->SetBoxStyle(Menu()->m_Path_Box_Section);
		pSection->Add(new gui::CLabelHeader(Context(), _("Personal")), false);
		
		{
			gui::CHListLayout* pList = new gui::CHListLayout(Context());
			pList->SetBoxStyle(Menu()->m_Path_Box_Item);
			pList->Add(new gui::CLabel(Context(), _("Name")), false, 200);
			pList->Add(new gui::CExternalTextEdit_Char(Menu(), Menu()->m_Cfg_PlayerName.buffer(), Menu()->m_Cfg_PlayerName.maxsize()), true);
			pSection->Add(pList);
		}
		{
			gui::CHListLayout* pList = new gui::CHListLayout(Context());
			pList->SetBoxStyle(Menu()->m_Path_Box_Item);
			pList->Add(new gui::CLabel(Context(), _("Clan")), false, 200);
			pList->Add(new gui::CExternalTextEdit_Char(Menu(), Menu()->m_Cfg_PlayerClan.buffer(), Menu()->m_Cfg_PlayerClan.maxsize()), true);
			pSection->Add(pList);
		}
		{
			gui::CHListLayout* pList = new gui::CHListLayout(Context());
			pList->SetBoxStyle(Menu()->m_Path_Box_Item);
			pList->Add(new gui::CLabel(Context(), _("Country")), false, 200);
			pList->Add(new CFlagLabel(Menu(), &Menu()->m_Cfg_PlayerCountry), true);
			pSection->Add(pList);
		}
	}
	
	//Country chooser
	{
		gui::CVListLayout* pSection = new gui::CVListLayout(Context());
		pRightList->Add(pSection, true);
		
		pSection->SetBoxStyle(Menu()->m_Path_Box_Section);
		pSection->Add(new gui::CLabelHeader(Context(), _("Country")), false);
		
		{
			gui::CHListLayout* pList = new gui::CHListLayout(Context());
			gui::CExternalTextEdit_Char* pSearchBox = new gui::CExternalTextEdit_Char(Context(), m_aSearchFlag, sizeof(m_aSearchFlag));
			pSearchBox->SaveOnChange();
			
			pList->SetBoxStyle(Menu()->m_Path_Box_Item);
			pList->Add(new gui::CLabel(Context(), _("Search"), Menu()->m_Path_Sprite_SearchIcon), false, 200);
			pList->Add(pSearchBox, true);
			pSection->Add(pList);
		}
		
		gui::CBoxLayout* pFlagList = new gui::CBoxLayout(Context());
		pFlagList->SetBoxStyle(Menu()->m_Path_Box_List);
		pSection->Add(pFlagList, true);
		
		CCountryFlags::CIterator Iter;
		for(Iter = Menu()->GameKernel()->CountryFlags()->BeginFlag(); Iter != Menu()->GameKernel()->CountryFlags()->EndFlag(); ++Iter)
		{
			pFlagList->Add(new CFlagButton(Menu(), &Menu()->m_Cfg_PlayerCountry, m_aSearchFlag, *Iter));
		}
	}
	
	return pTab;
}

gui::CWidget* CSettings::CreateCharacterTab()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->SetBoxStyle(Menu()->m_Path_Box_SectionList);
	
	return pTab;
}

gui::CWidget* CSettings::CreateControlTab()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->SetBoxStyle(Menu()->m_Path_Box_SectionList);
	
	//Movement
	{
		gui::CVListLayout* pSection = new gui::CVListLayout(Context());
		pTab->Add(pSection, false);
		
		pSection->SetBoxStyle(Menu()->m_Path_Box_Section);
		pSection->Add(new gui::CLabelHeader(Context(), _("Movement")), false);
		
		pSection->Add(new CControlEdit(Menu(), _("Move left"), "+left"), false);
		pSection->Add(new CControlEdit(Menu(), _("Move right"), "+right"), false);
		pSection->Add(new CControlEdit(Menu(), _("Jump"), "+jump"), false);
		pSection->Add(new CControlEdit(Menu(), _("Fire"), "+fire"), false);
		pSection->Add(new CControlEdit(Menu(), _("Hook"), "+hook"), false);
	}
	
	//Weapons
	{
		gui::CVListLayout* pSection = new gui::CVListLayout(Context());
		pTab->Add(pSection, false);
		
		pSection->SetBoxStyle(Menu()->m_Path_Box_Section);
		pSection->Add(new gui::CLabelHeader(Context(), _("Weapons")), false);
		
		pSection->Add(new CControlEdit(Menu(), _("Hammer"), "+weapon1"), false);
		pSection->Add(new CControlEdit(Menu(), _("Pistol"), "+weapon2"), false);
		pSection->Add(new CControlEdit(Menu(), _("Shotgun"), "+weapon3"), false);
		pSection->Add(new CControlEdit(Menu(), _("Grenade"), "+weapon4"), false);
		pSection->Add(new CControlEdit(Menu(), _("Laser"), "+weapon5"), false);
		pSection->Add(new CControlEdit(Menu(), _("Next weapon"), "+nextweapon"), false);
		pSection->Add(new CControlEdit(Menu(), _("Prev. weapon"), "+prevweapon"), false);
	}
	
	//Voting
	{
		gui::CVListLayout* pSection = new gui::CVListLayout(Context());
		pTab->Add(pSection, false);
		
		pSection->SetBoxStyle(Menu()->m_Path_Box_Section);
		pSection->Add(new gui::CLabelHeader(Context(), _("Voting")), false);
		
		pSection->Add(new CControlEdit(Menu(), _("Vote yes"), "vote yes"), false);
		pSection->Add(new CControlEdit(Menu(), _("Vote no"), "vote no"), false);
	}
	
	//Chat
	{
		gui::CVListLayout* pSection = new gui::CVListLayout(Context());
		pTab->Add(pSection, false);
		
		pSection->SetBoxStyle(Menu()->m_Path_Box_Section);
		pSection->Add(new gui::CLabelHeader(Context(), _("Chat")), false);
		
		pSection->Add(new CControlEdit(Menu(), _("Chat"), "chat all"), false);
		pSection->Add(new CControlEdit(Menu(), _("Team chat"), "chat team"), false);
		pSection->Add(new CControlEdit(Menu(), _("Show chat"), "+show_chat"), false);
	}
	
	//Misc
	{
		gui::CVListLayout* pSection = new gui::CVListLayout(Context());
		pTab->Add(pSection, false);
		
		pSection->SetBoxStyle(Menu()->m_Path_Box_Section);
		pSection->Add(new gui::CLabelHeader(Context(), _("Misc")), false);
		
		pSection->Add(new CControlEdit(Menu(), _("Emoticon"), "+emote"), false);
		pSection->Add(new CControlEdit(Menu(), _("Spectator mode"), "+spectate"), false);
		pSection->Add(new CControlEdit(Menu(), _("Spectate next"), "spectate_next"), false);
		pSection->Add(new CControlEdit(Menu(), _("Spectate previous"), "spectate_previous"), false);
		pSection->Add(new CControlEdit(Menu(), _("Console"), "toggle_local_console"), false);
		pSection->Add(new CControlEdit(Menu(), _("Remote console"), "toggle_remote_console"), false);
		pSection->Add(new CControlEdit(Menu(), _("Screenshot"), "screenshot"), false);
		pSection->Add(new CControlEdit(Menu(), _("Scoreboard"), "+scoreboard"), false);
		pSection->Add(new CControlEdit(Menu(), _("Respawn"), "kill"), false);
		pSection->Add(new CControlEdit(Menu(), _("Ready"), "ready_change"), false);
		pSection->Add(new CControlEdit(Menu(), _("Add demo marker"), "add_demomarker"), false);
	}
	
	return pTab;
}

gui::CWidget* CSettings::CreateGraphicsTab()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->SetBoxStyle(Menu()->m_Path_Box_SectionList);
	
	return pTab;
}

gui::CWidget* CSettings::CreateSoundTab()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->SetBoxStyle(Menu()->m_Path_Box_SectionList);
	
	return pTab;
}
