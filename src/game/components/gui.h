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

#ifndef __CLIENT_GUI_MENU_GUI__
#define __CLIENT_GUI_MENU_GUI__

#include <client/components/gui.h>
#include <client/gui/widget.h>
#include <shared/components/cli.h>
#include <game/kernel.h>

class CGuiGame : public CGui
{
public:
	enum
	{
		PAGE_START=0,
		PAGE_DEMOS,
		PAGE_SETTINGS,
		PAGE_ASSETSEDITOR,
		PAGE_QUIT,
	};
	
	class CMainWidget : public gui::CWidget
	{	
	protected:
		CGuiGame* m_pMenu;
		int m_CurrentPage;
		int m_WantedPage;
		class gui::CWidget* m_pCurrentWidget;
		class gui::CWidget* m_pStart;
		class gui::CWidget* m_pSettings;
		class CConsole* m_pConsole;
		
	public:
		CMainWidget(CGuiGame* pMenu);

		virtual void Destroy();
		virtual void UpdateBoundingSize();
		virtual void UpdatePosition(const gui::CRect& BoundingRect, const gui::CRect& VisibilityRect);
		virtual void Update(bool ParentEnabled);
		virtual void Render();
		virtual void OnMouseMove();
		virtual void OnButtonClick(int Button);
		virtual void OnButtonRelease(int Button);
		virtual void OnInputEvent(const CInput::CEvent& Event);
		
		void SetPage(int Page) { m_WantedPage = Page; }
		
		inline CGuiGame* Menu() { return m_pMenu; }
		CCLI_Output* ConsoleOutput();
	};

protected:
	CMainWidget* m_pMenuWidget;
	CGameKernel* m_pGameKernel;

public:
	int m_Cfg_MouseFollowfactor;
	int m_Cfg_MouseMaxDistance;
	int m_Cfg_MouseDeadzone;
	int m_Cfg_AutoswitchWeapons;
	int m_Cfg_Showhud;
	int m_Cfg_ShowChatFriends;
	int m_Cfg_Showsocial;
	int m_Cfg_Nameplates;
	int m_Cfg_NameplatesAlways;
	int m_Cfg_NameplatesSize;
	int m_Cfg_NameplatesTeamcolors;
	int m_Cfg_AutoDemoRecord;
	int m_Cfg_AutoDemoMax;
	int m_Cfg_AutoScreenshot;
	int m_Cfg_AutoScreenshotMax;
	fixed_string16 m_Cfg_PlayerName;
	fixed_string12 m_Cfg_PlayerClan;
	int m_Cfg_PlayerCountry;

public:
	CAssetPath m_Path_Box_StartMenu;
	CAssetPath m_Path_Box_Column;
	CAssetPath m_Path_Box_Section;
	CAssetPath m_Path_Box_SectionList;
	CAssetPath m_Path_Box_List;
	CAssetPath m_Path_Box_Item;
	CAssetPath m_Path_Box_Console;
	
	CAssetPath m_Path_Label_Text;
	CAssetPath m_Path_Label_Header;
	CAssetPath m_Path_Label_Compose;
	CAssetPath m_Path_Label_Logo;
	CAssetPath m_Path_Label_ConsoleHint;
	CAssetPath m_Path_Label_ConsoleLine;
	CAssetPath m_Path_Label_ConsoleLineError;
	CAssetPath m_Path_Label_ConsoleLineHelp;
	
	CAssetPath m_Path_Button_Default;
	CAssetPath m_Path_Button_TextEdit;
	CAssetPath m_Path_Button_Quit;
	CAssetPath m_Path_Button_Page;
	CAssetPath m_Path_Button_Universe;
	CAssetPath m_Path_Button_ConsolePrompt;
	CAssetPath m_Path_Button_Icon;
	
	CAssetPath m_Path_Toggle_Default;
	
	CAssetPath m_Path_IntEdit_Default;
	
	CAssetPath m_Path_Slider_Default;
	
	CAssetPath m_Path_Scrollbar_Default;
	
	CAssetPath m_Path_Tabs_Pages;
	
	CAssetPath m_Path_Sprite_CursorPointer;
	CAssetPath m_Path_Sprite_CursorTranslateX;
	CAssetPath m_Path_Sprite_CursorTranslateY;
	CAssetPath m_Path_Sprite_CursorText;
	CAssetPath m_Path_Sprite_Void;
	CAssetPath m_Path_Sprite_SearchIcon;
	CAssetPath m_Path_Sprite_Logo;
	CAssetPath m_Path_Sprite_Demos;
	CAssetPath m_Path_Sprite_Settings;
	CAssetPath m_Path_Sprite_Editor;
	CAssetPath m_Path_Sprite_Quit;

public:
	CGuiGame(CGameKernel* pKernel);
	
	virtual bool InitConfig(int argc, const char** argv);
	virtual void SaveConfig(class CCLI_Output* pOutput);
	virtual void LoadAssets();
	virtual gui::CWidget* CreateMainWidget();
	
	inline void SetPage(int Page) { if(m_pMainWidget) m_pMenuWidget->SetPage(Page); }
	CCLI_Output* ConsoleOutput() { return (m_pMainWidget ? m_pMenuWidget->ConsoleOutput() : 0); };
	
	inline CGameKernel* GameKernel() { return m_pGameKernel; }
	inline const CGameKernel* GameKernel() const { return m_pGameKernel; }
};

#endif
