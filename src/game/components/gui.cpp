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
#include <game/gui/console.h>
#include <game/gui/settings.h>
#include <game/gui/start.h>
#include <client/gui/popup.h>
#include <client/gui/panellayout.h>
#include <client/components/input.h>
#include <shared/components/assetsmanager.h>
#include <shared/components/storage.h>
#include <client/keys.h>

/* CONSOLE POPUP ******************************************************/

class CConsolePopup : public gui::CPopup
{
public:
	CConsolePopup(CGui* pContext, const gui::CRect& CreatorRect, gui::CWidget* pConsole) :
		gui::CPopup(pContext, CreatorRect, 0, 0, gui::CPopup::ALIGNMENT_INNER)
	{
		SetLevel(gui::CPopup::LEVEL_HIGHEST);
		Add(pConsole);
	}
	
	virtual int GetInputToBlock()
	{
		return CGui::BLOCKEDINPUT_ALL;
	}
	
	virtual void Destroy()
	{
		m_pChild = 0;
		gui::CPopup::Destroy();
	}
};

/* MAIN WIDGET ********************************************************/

CGuiGame::CMainWidget::CMainWidget(CGuiGame* pMenu) :
	gui::CWidget(pMenu),
	m_pMenu(pMenu)
{
	m_WantedPage = CGuiGame::PAGE_START;
	m_CurrentPage = CGuiGame::PAGE_START;
	
	m_pConsole = new CConsole(Menu());
	m_pStart = new CStart(Menu());
	m_pSettings = new CSettings(Menu());
	
	m_pCurrentWidget = m_pStart;
}

void CGuiGame::CMainWidget::Destroy()
{
	m_pConsole->Destroy();
	m_pStart->Destroy();
	m_pSettings->Destroy();
	
	gui::CWidget::Destroy();
}

void CGuiGame::CMainWidget::UpdateBoundingSize()
{
	m_BoundingSizeRect.BSNoConstraint();
	
	if(m_pCurrentWidget)
		m_pCurrentWidget->UpdateBoundingSize();
	
	m_BoundingSizeRect.BSVerticalAdd(m_pCurrentWidget->GetBS());
}

void CGuiGame::CMainWidget::UpdatePosition(const gui::CRect& BoundingRect, const gui::CRect& VisibilityRect)
{
	gui::CWidget::UpdatePosition(BoundingRect, VisibilityRect);
	
	if(m_pCurrentWidget)
		m_pCurrentWidget->UpdatePosition(m_DrawRect, m_VisibilityRect);
}

void CGuiGame::CMainWidget::Update(bool ParentEnabled)
{		
	m_CurrentPage = m_WantedPage;
	
	switch(m_CurrentPage)
	{
		case CGuiGame::PAGE_START:
			m_pCurrentWidget = m_pStart;
			break;
		case CGuiGame::PAGE_SETTINGS:
			m_pCurrentWidget = m_pSettings;
			break;
		case CGuiGame::PAGE_QUIT:
			m_pCurrentWidget = m_pStart;
			m_CurrentPage = CGuiGame::PAGE_START;
			m_WantedPage = CGuiGame::PAGE_START;
			Menu()->Quit();
			break;
	}
	
	if(m_pCurrentWidget)
		m_pCurrentWidget->Update(ParentEnabled);
	
	gui::CWidget::Update(ParentEnabled);
}

void CGuiGame::CMainWidget::Render()
{
	if(m_pCurrentWidget)
		m_pCurrentWidget->Render();
}

void CGuiGame::CMainWidget::OnMouseMove()
{
	if(m_pCurrentWidget)
		m_pCurrentWidget->OnMouseMove();
}

void CGuiGame::CMainWidget::OnButtonClick(int Button)
{
	if(m_pCurrentWidget)
		m_pCurrentWidget->OnButtonClick(Button);
}

void CGuiGame::CMainWidget::OnButtonRelease(int Button)
{
	if(m_pCurrentWidget)
		m_pCurrentWidget->OnButtonRelease(Button);
}

void CGuiGame::CMainWidget::OnInputEvent(const CInput::CEvent& Event)
{
	if(Event.m_Key == KEY_ESCAPE && (Event.m_Flags & CInput::FLAG_RELEASE))
	{
		switch(m_CurrentPage)
		{
			case CGuiGame::PAGE_SETTINGS:
				SetPage(CGuiGame::PAGE_START);
				return;
			case CGuiGame::PAGE_START:
				SetPage(CGuiGame::PAGE_QUIT);
				return;
		}
	}
	
	if(Event.m_Key == KEY_F1 && (Event.m_Flags & CInput::FLAG_RELEASE))
	{
		Context()->DisplayPopup(new CConsolePopup(Context(), m_DrawRect, m_pConsole));
		return;
	}
	if(Input()->KeyIsPressed(KEY_LCTRL) && Event.m_Key == KEY_A && (Event.m_Flags & CInput::FLAG_RELEASE))
	{
		Context()->m_Cfg_Scale -= 4;
		return;
	}
	if(Input()->KeyIsPressed(KEY_LCTRL) && Event.m_Key == KEY_E && (Event.m_Flags & CInput::FLAG_RELEASE))
	{
		Context()->m_Cfg_Scale += 4;
		return;
	}
	
	if(m_pCurrentWidget)
		m_pCurrentWidget->OnInputEvent(Event);
}

CCLI_Output* CGuiGame::CMainWidget::ConsoleOutput()
{
	return m_pConsole;
}


/* CONTEXT ************************************************************/

CGuiGame::CGuiGame(CGameKernel* pGameKernel) :
	CGui(pGameKernel),
	m_pGameKernel(pGameKernel)
{
	SetName("GuiGame");
}

bool CGuiGame::InitConfig(int argc, const char** argv)
{
	m_Cfg_MouseFollowfactor = 60;
	m_Cfg_MouseMaxDistance = 800;
	m_Cfg_MouseDeadzone = 300;
	m_Cfg_AutoswitchWeapons = 0;
	m_Cfg_Showhud = 1;
	m_Cfg_ShowChatFriends = 0;
	m_Cfg_Showsocial = 1;
	m_Cfg_Nameplates = 1;
	m_Cfg_NameplatesAlways = 1;
	m_Cfg_NameplatesSize = 1;
	m_Cfg_NameplatesTeamcolors = 50;
	m_Cfg_AutoDemoRecord = 0;
	m_Cfg_AutoDemoMax = 10;
	m_Cfg_AutoScreenshot = 0;
	m_Cfg_AutoScreenshotMax = 10;
	m_Cfg_PlayerName.copy("nameless tee");
	m_Cfg_PlayerClan.clear();
	m_Cfg_PlayerCountry = -1;
	
	CLI()->RegisterConfigInteger("mouse_followfactor", "", &m_Cfg_MouseFollowfactor, 0, 0);
	CLI()->RegisterConfigInteger("mouse_max_distance", "", &m_Cfg_MouseMaxDistance, 0, 200);
	CLI()->RegisterConfigInteger("mouse_deadzone", "", &m_Cfg_MouseDeadzone, 0, 0);
	CLI()->RegisterConfigInteger("autoswitch_weapons", "Auto switch weapon on pickup", &m_Cfg_AutoswitchWeapons, 0, 1);
	CLI()->RegisterConfigInteger("showhud", "Show ingame HUD", &m_Cfg_Showhud, 0, 1);
	CLI()->RegisterConfigInteger("show_chat_friends", "Show only chat messages from friends", &m_Cfg_ShowChatFriends, 0, 1);
	CLI()->RegisterConfigInteger("showsocial", "Show social data like names, clans, chat etc.", &m_Cfg_Showsocial, 0, 1);
	CLI()->RegisterConfigInteger("nameplates", "Show name plates", &m_Cfg_Nameplates, 0, 100);
	CLI()->RegisterConfigInteger("nameplates_always", "Always show name plates disregarding of distance", &m_Cfg_NameplatesAlways, 0, 1);
	CLI()->RegisterConfigInteger("nameplates_teamcolors", "Use team colors for name plates", &m_Cfg_NameplatesSize, 0, 1);
	CLI()->RegisterConfigInteger("nameplates_size", "Size of the name plates from 0 to 100%", &m_Cfg_NameplatesTeamcolors, 0, 100);
	CLI()->RegisterConfigInteger("auto_demo_record", "Automatically record demos", &m_Cfg_AutoDemoRecord, 0, 1);
	CLI()->RegisterConfigInteger("auto_demo_max", "Maximum number of automatically recorded demos (0 = no limit)", &m_Cfg_AutoDemoMax, 0, 1000);
	CLI()->RegisterConfigInteger("auto_screenshot", "Automatically take game over screenshot", &m_Cfg_AutoScreenshot, 0, 1);
	CLI()->RegisterConfigInteger("auto_screenshot_max", "Maximum number of automatically created screenshots (0 = no limit)", &m_Cfg_AutoScreenshotMax, 0, 1000);
	CLI()->RegisterConfigString("player_name", "Name of the player", &m_Cfg_PlayerName);
	CLI()->RegisterConfigString("player_clan", "Clan of the player", &m_Cfg_PlayerClan);
	CLI()->RegisterConfigInteger("player_country", "Country of the player", &m_Cfg_PlayerCountry, -1, 10000);
	
	return CGui::InitConfig(argc, argv);
}

void CGuiGame::SaveConfig(class CCLI_Output* pOutput)
{
	CGui::SaveConfig(pOutput);
	
	fixed_string128 Buffer;
	
	str_format(Buffer.buffer(), Buffer.maxsize(), "mouse_followfactor %d", m_Cfg_MouseFollowfactor); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "mouse_max_distance %d", m_Cfg_MouseMaxDistance); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "mouse_deadzone %d", m_Cfg_MouseDeadzone); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "autoswitch_weapons %d", m_Cfg_AutoswitchWeapons); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "showhud %d", m_Cfg_Showhud); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "show_chat_friends %d", m_Cfg_ShowChatFriends); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "showsocial %d", m_Cfg_Showsocial); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "nameplates %d", m_Cfg_Nameplates); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "nameplates_always %d", m_Cfg_NameplatesAlways); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "nameplates_teamcolors %d", m_Cfg_NameplatesSize); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "nameplates_size %d", m_Cfg_NameplatesTeamcolors); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "auto_demo_record %d", m_Cfg_AutoDemoRecord); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "auto_demo_max %d", m_Cfg_AutoDemoMax); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "auto_screenshot %d", m_Cfg_AutoScreenshot); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "auto_screenshot_max %d", m_Cfg_AutoScreenshotMax); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "player_name %s", m_Cfg_PlayerName.buffer()); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "player_clan %s", m_Cfg_PlayerClan.buffer()); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "player_country %d", m_Cfg_PlayerCountry); pOutput->Print(Buffer.buffer());
}

void CGuiGame::LoadAssets()
{
	{
		int PackageId = AssetsManager()->Load_AssetsFile("gui_cursor", CStorage::TYPE_ALL);
		
		m_Path_Sprite_CursorPointer = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "cursorPointer");
		m_Path_Sprite_CursorTranslateX = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "cursorTranslateX");
		m_Path_Sprite_CursorTranslateY = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "cursorTranslateY");
		m_Path_Sprite_CursorText = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "cursorText");
	}
	
	{
		int PackageId = AssetsManager()->Load_AssetsFile("gui_game", CStorage::TYPE_ALL);
		
		m_Path_Box_StartMenu = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "startMenu");
		m_Path_Box_Column = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "column");
		m_Path_Box_Section = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "section");
		m_Path_Box_SectionList = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "sectionList");
		m_Path_Box_List = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "list");
		m_Path_Box_Item = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "item");
		m_Path_Box_Console = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "console");

		m_Path_Label_Text = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "default");
		m_Path_Label_Header = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "header");
		m_Path_Label_Compose = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "compose");
		m_Path_Label_Logo = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "logo");
		m_Path_Label_ConsoleHint = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "consoleHint");
		m_Path_Label_ConsoleLine = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "consoleLine");
		m_Path_Label_ConsoleLineError = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "consoleLineError");
		m_Path_Label_ConsoleLineHelp = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "consoleLineHelp");

		m_Path_Button_Default = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "default");
		m_Path_Button_TextEdit = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "textEdit");
		m_Path_Button_Quit = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "buttonQuit");
		m_Path_Button_Page = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "buttonPage");
		m_Path_Button_Universe = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "buttonUniverse");
		m_Path_Button_ConsolePrompt = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "consolePrompt");
		m_Path_Button_Icon = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "icon");
		
		m_Path_Toggle_Default = AssetsManager()->FindAsset<CAsset_GuiToggleStyle>(PackageId, "default");
		
		m_Path_IntEdit_Default = AssetsManager()->FindAsset<CAsset_GuiIntEditStyle>(PackageId, "default");
		
		m_Path_Slider_Default = AssetsManager()->FindAsset<CAsset_GuiSliderStyle>(PackageId, "default");
		
		m_Path_Scrollbar_Default = AssetsManager()->FindAsset<CAsset_GuiScrollbarStyle>(PackageId, "default");
		
		m_Path_Tabs_Pages = AssetsManager()->FindAsset<CAsset_GuiTabsStyle>(PackageId, "pages");
		
		m_Path_Sprite_Void = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "void");
		m_Path_Sprite_SearchIcon = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "searchIcon");
		m_Path_Sprite_Logo = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "logo");
		m_Path_Sprite_Demos = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "demosButton");
		m_Path_Sprite_Settings = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "settingsButton");
		m_Path_Sprite_Editor = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "editorButton");
		m_Path_Sprite_Quit = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "quitButton");
	}
	
	//Init default path to the GUI
	m_CursorPointerPath = m_Path_Sprite_CursorPointer;
	m_CursorTranslateXPath = m_Path_Sprite_CursorTranslateX;
	m_CursorTranslateYPath = m_Path_Sprite_CursorTranslateY;
	m_CursorTextPath = m_Path_Sprite_CursorText;
	
	m_LabelStyle = m_Path_Label_Text;
	m_LabelHeaderStyle = m_Path_Label_Header;
	m_ButtonStyle = m_Path_Button_Default;
	m_TextEntryStyle = m_Path_Button_TextEdit;
	m_IntEditStyle = m_Path_IntEdit_Default;
	m_ToggleStyle = m_Path_Toggle_Default;
	m_SliderStyle = m_Path_Slider_Default;
	m_ScrollbarStyle = m_Path_Scrollbar_Default;
	m_ComposeStyle = m_Path_Label_Compose;
}

gui::CWidget* CGuiGame::CreateMainWidget()
{
	m_pMenuWidget = new CMainWidget(this);
	return m_pMenuWidget;
}
