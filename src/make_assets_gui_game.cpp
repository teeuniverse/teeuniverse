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

#include <shared/system/debug.h>
#include <shared/system/string.h>
#include <shared/kernel.h>
#include <shared/components/assetsmanager.h>
#include <shared/components/storage.h>
#include <client/loading_tools.h>
#include <game/assetspath.h>

#include <cstdlib>

#define CREATE_IMAGE(id, name, gx, gy) CreateNewImage_Path(pKernel, CAssetPath::ImageGui(id), name, name".png", gx, gy);

void Make_Images(CSharedKernel* pKernel)
{
	CAssetsManager* pAssetsManager = pKernel->AssetsManager();
	
	//Images
	CREATE_IMAGE(IMAGE_CURSOR, "images/gui_cursor", 4, 5)
	CREATE_IMAGE(IMAGE_MENU, "images/gui_menu", 16, 8)
	CREATE_IMAGE(IMAGE_LOGO, "images/logo", 1, 1)
	CREATE_IMAGE(IMAGE_MENU_DEMOS, "images/menu_demos", 1, 1)
	CREATE_IMAGE(IMAGE_MENU_SETTINGS, "images/menu_settings", 1, 1)
	CREATE_IMAGE(IMAGE_MENU_ASSETSEDITOR, "images/menu_assetseditor", 1, 1)
	CREATE_IMAGE(IMAGE_MENU_QUIT, "images/menu_quit", 1, 1)
}	

#define CREATE_SPRITE(id, name, image, x, y, w, h) {\
	CAsset_Sprite* pSprite = pAssetsManager->NewAsset_Hard<CAsset_Sprite>(CAssetPath::SpriteGui(id));\
	pSprite->SetName(name);\
	pSprite->SetImagePath(CAssetPath::ImageGui(image));\
	pSprite->SetX(x);\
	pSprite->SetY(y);\
	pSprite->SetWidth(w);\
	pSprite->SetHeight(h);\
}

void Make_Sprites(CSharedKernel* pKernel)
{
	CAssetsManager* pAssetsManager = pKernel->AssetsManager();
	
	//Sprites
	CREATE_SPRITE(SPRITE_CURSOR_POINTER, "cursor", IMAGE_CURSOR, 0, 2, 2, 3);
	CREATE_SPRITE(SPRITE_CURSOR_TRANSLATEX, "cursorTranslateX", IMAGE_CURSOR, 2, 1, 2, 2);
	CREATE_SPRITE(SPRITE_CURSOR_TRANSLATEY, "cursorTranslateY", IMAGE_CURSOR, 2, 3, 2, 2);
	CREATE_SPRITE(SPRITE_CURSOR_TEXT, "cursorText", IMAGE_CURSOR, 0, 0, 2, 2);
	
	CREATE_SPRITE(SPRITE_MENU_VOID, "iconVoid", IMAGE_MENU, 0, 0, 1, 1);
	CREATE_SPRITE(SPRITE_MENU_CHECKBOX_TRUE, "iconMenuCheckBoxTrue", IMAGE_MENU, 1, 0, 1, 1);
	CREATE_SPRITE(SPRITE_MENU_CHECKBOX_FALSE, "iconMenuCheckBoxFalse", IMAGE_MENU, 2, 0, 1, 1);
	CREATE_SPRITE(SPRITE_MENU_SLIDER_CURSOR, "iconMenuSliderCursor", IMAGE_MENU, 3, 0, 1, 1);
	CREATE_SPRITE(SPRITE_MENU_SEARCH, "iconMenuSearch", IMAGE_MENU, 4, 0, 1, 1);
	CREATE_SPRITE(SPRITE_LOGO, "logo", IMAGE_LOGO, 0, 0, 1, 1);
	CREATE_SPRITE(SPRITE_MENU_DEMOS, "menuDemos", IMAGE_MENU_DEMOS, 0, 0, 1, 1);
	CREATE_SPRITE(SPRITE_MENU_SETTINGS, "menuSettings", IMAGE_MENU_SETTINGS, 0, 0, 1, 1);
	CREATE_SPRITE(SPRITE_MENU_ASSETSEDITOR, "menuAssetsEditor", IMAGE_MENU_ASSETSEDITOR, 0, 0, 1, 1);
	CREATE_SPRITE(SPRITE_MENU_QUIT, "menuQuit", IMAGE_MENU_QUIT, 0, 0, 1, 1);
}

void Make_GuiRectStyle(CSharedKernel* pKernel)
{
	CAssetsManager* pAssetsManager = pKernel->AssetsManager();
	
	//GuiRectStyles
	{
		CAsset_GuiRectStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiRectStyle>(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_COMPOSING));
		pAsset->SetName("composing");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_ROUNDCORNER);
		pAsset->SetBackgroundColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		pAsset->SetCornerRadius(3.0f);
		pAsset->SetCornerFlags(CAsset_GuiRectStyle::CORNERFLAG_ALL);
	}
	{
		CAsset_GuiRectStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiRectStyle>(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_CONSOLE));
		pAsset->SetName("console");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_BORDER);
		pAsset->SetBackgroundColor(vec4(57.0f/255.0f, 57.0f/255.0f, 57.0f/255.0f, 0.7f));
		pAsset->SetBorderColor(vec4(40.0f/255.0f, 40.0f/255.0f, 40.0f/255.0f, 1.0f));
		pAsset->SetBorderFlags(CAsset_GuiRectStyle::BORDERFLAG_ALL);
	}
	{
		CAsset_GuiRectStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiRectStyle>(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_CONSOLE_PROMPT_IDLE));
		pAsset->SetName("consolePromptIdle");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_BORDER);
		pAsset->SetBackgroundColor(vec4(83.0f/255.0f, 83.0f/255.0f, 83.0f/255.0f, 0.7f));
		pAsset->SetBorderColor(vec4(40.0f/255.0f, 40.0f/255.0f, 40.0f/255.0f, 1.0f));
		pAsset->SetBorderFlags(CAsset_GuiRectStyle::BORDERFLAG_ALL);
	}
	{
		CAsset_GuiRectStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiRectStyle>(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_CONSOLE_PROMPT_MO));
		pAsset->SetName("consolePromptMO");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_BORDER);
		pAsset->SetBackgroundColor(vec4(128.0f/255.0f, 128.0f/255.0f, 128.0f/255.0f, 0.7f));
		pAsset->SetBorderColor(vec4(40.0f/255.0f, 40.0f/255.0f, 40.0f/255.0f, 1.0f));
		pAsset->SetBorderFlags(CAsset_GuiRectStyle::BORDERFLAG_ALL);
	}
	{
		CAsset_GuiRectStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiRectStyle>(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_CONSOLE_HINT));
		pAsset->SetName("consoleHint");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_BORDER);
		pAsset->SetBackgroundColor(vec4(83.0f/255.0f, 83.0f/255.0f, 83.0f/255.0f, 0.7f));
		pAsset->SetBorderColor(vec4(40.0f/255.0f, 40.0f/255.0f, 40.0f/255.0f, 1.0f));
		pAsset->SetBorderFlags(CAsset_GuiRectStyle::BORDERFLAG_ALL);
	}
	{
		CAsset_GuiRectStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiRectStyle>(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_BACKGROUND));
		pAsset->SetName("menuRect");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_ROUNDCORNER);
		pAsset->SetBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.2f));
		pAsset->SetCornerRadius(5.0f);
		pAsset->SetCornerFlags(CAsset_GuiRectStyle::CORNERFLAG_ALL);
	}
	{
		CAsset_GuiRectStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiRectStyle>(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_FOCUS));
		pAsset->SetName("menuRectFocus");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_BORDER | CAsset_GuiRectStyle::FLAG_ROUNDCORNER);
		pAsset->SetBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.2f));
		pAsset->SetBorderColor(vec4(1.0f, 1.0f, 1.0f, 0.4f));
		pAsset->SetBorderFlags(CAsset_GuiRectStyle::BORDERFLAG_ALL);
		pAsset->SetCornerRadius(5.0f);
		pAsset->SetCornerFlags(CAsset_GuiRectStyle::CORNERFLAG_ALL);
	}
	{
		CAsset_GuiRectStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiRectStyle>(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_MOUSEOVER));
		pAsset->SetName("menuRectMO");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_ROUNDCORNER);
		pAsset->SetBackgroundColor(vec4(1.0f, 1.0f, 1.0f, 0.4f));
		pAsset->SetCornerRadius(5.0f);
		pAsset->SetCornerFlags(CAsset_GuiRectStyle::CORNERFLAG_ALL);
	}
	{
		CAsset_GuiRectStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiRectStyle>(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_HIGHLIGHT));
		pAsset->SetName("menuRectHL");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_ROUNDCORNER);
		pAsset->SetBackgroundColor(vec4(1.0f, 1.0f, 1.0f, 0.7f));
		pAsset->SetCornerRadius(5.0f);
		pAsset->SetCornerFlags(CAsset_GuiRectStyle::CORNERFLAG_ALL);
	}
	{
		CAsset_GuiRectStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiRectStyle>(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_STARTBUTTON));
		pAsset->SetName("menuStartButton");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_ROUNDCORNER);
		pAsset->SetBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));
		pAsset->SetCornerRadius(10.0f);
		pAsset->SetCornerFlags(CAsset_GuiRectStyle::CORNERFLAG_ALL);
	}
	{
		CAsset_GuiRectStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiRectStyle>(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_STARTBUTTON_MO));
		pAsset->SetName("menuStartButtonMO");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_ROUNDCORNER);
		pAsset->SetBackgroundColor(vec4(1.0f, 1.0f, 1.0f, 0.4f));
		pAsset->SetCornerRadius(10.0f);
		pAsset->SetCornerFlags(CAsset_GuiRectStyle::CORNERFLAG_ALL);
	}
}
	
void Make_GuiLineStyle(CSharedKernel* pKernel)
{
	CAssetsManager* pAssetsManager = pKernel->AssetsManager();
	
	//GuiLineStyles
	{
		CAsset_GuiLineStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLineStyle>(CAssetPath::GuiLineStyleGui(GUILINESTYLE_MENU_SCROLLBAR_RAIL));
		pAsset->SetName("menuScrollbarRail");
		pAsset->SetFlags(CAsset_GuiLineStyle::FLAG_IMAGE);
		pAsset->SetImagePath(CAssetPath::Gui(CAsset_Image::TypeId, IMAGE_MENU));
		pAsset->SetImageUV_Min(vec2((512-32*3)/512.0f, 1.0f));
		pAsset->SetImageUV_Max(vec2(1.0f, (256-32)/256.0f));
	}
	{
		CAsset_GuiLineStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLineStyle>(CAssetPath::GuiLineStyleGui(GUILINESTYLE_MENU_SCROLLBAR_SLIDER));
		pAsset->SetName("menuScrollbarSlider");
		pAsset->SetFlags(CAsset_GuiLineStyle::FLAG_IMAGE);
		pAsset->SetImagePath(CAssetPath::Gui(CAsset_Image::TypeId, IMAGE_MENU));
		pAsset->SetImageUV_Min(vec2((512-32*6)/512.0f, 1.0f));
		pAsset->SetImageUV_Max(vec2((512-32*3)/512.0f, (256-32)/256.0f));
	}
	{
		CAsset_GuiLineStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLineStyle>(CAssetPath::GuiLineStyleGui(GUILINESTYLE_MENU_SLIDER_RAIL));
		pAsset->SetName("menuSliderRail");
		pAsset->SetFlags(CAsset_GuiLineStyle::FLAG_IMAGE);
		pAsset->SetImagePath(CAssetPath::Gui(CAsset_Image::TypeId, IMAGE_MENU));
		pAsset->SetImageUV_Min(vec2((512-32*3)/512.0f, (256-2*32)/256.0f));
		pAsset->SetImageUV_Max(vec2(1.0f, (256-32)/256.0f));
	}
}

void Make_GuiBoxStyle(CSharedKernel* pKernel)
{
	CAssetsManager* pAssetsManager = pKernel->AssetsManager();
		
	//GuiBoxStyles
	{
		CAsset_GuiBoxStyle* pGuiBoxStyle = pAssetsManager->NewAsset_Hard<CAsset_GuiBoxStyle>(CAssetPath::GuiBoxStyleGui(GUIBOXSTYLE_CONSOLE));
		pGuiBoxStyle->SetName("console");
		pGuiBoxStyle->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_CONSOLE));
	}
	{
		CAsset_GuiBoxStyle* pGuiBoxStyle = pAssetsManager->NewAsset_Hard<CAsset_GuiBoxStyle>(CAssetPath::GuiBoxStyleGui(GUIBOXSTYLE_MENU_START));
		pGuiBoxStyle->SetName("menuStart");
		pGuiBoxStyle->SetSpacing(20);
		pGuiBoxStyle->SetMargin(40);
	}
	{
		CAsset_GuiBoxStyle* pGuiBoxStyle = pAssetsManager->NewAsset_Hard<CAsset_GuiBoxStyle>(CAssetPath::GuiBoxStyleGui(GUIBOXSTYLE_MENU_COLUMNS));
		pGuiBoxStyle->SetName("menuColumns");
		pGuiBoxStyle->SetSpacing(20);
	}
	{
		CAsset_GuiBoxStyle* pGuiBoxStyle = pAssetsManager->NewAsset_Hard<CAsset_GuiBoxStyle>(CAssetPath::GuiBoxStyleGui(GUIBOXSTYLE_MENU_SECTIONLIST));
		pGuiBoxStyle->SetName("menuSectionList");
		pGuiBoxStyle->SetSpacing(20);
	}
	{
		CAsset_GuiBoxStyle* pGuiBoxStyle = pAssetsManager->NewAsset_Hard<CAsset_GuiBoxStyle>(CAssetPath::GuiBoxStyleGui(GUIBOXSTYLE_MENU_SECTION));
		pGuiBoxStyle->SetName("menuSection");
		pGuiBoxStyle->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_BACKGROUND));
		pGuiBoxStyle->SetSpacing(4);
	}
	{
		CAsset_GuiBoxStyle* pGuiBoxStyle = pAssetsManager->NewAsset_Hard<CAsset_GuiBoxStyle>(CAssetPath::GuiBoxStyleGui(GUIBOXSTYLE_MENU_LIST));
		pGuiBoxStyle->SetName("menuList");
		pGuiBoxStyle->SetSpacing(4);
	}
	{
		CAsset_GuiBoxStyle* pGuiBoxStyle = pAssetsManager->NewAsset_Hard<CAsset_GuiBoxStyle>(CAssetPath::GuiBoxStyleGui(GUIBOXSTYLE_MENU_MAINTABS_LAYOUT));
		pGuiBoxStyle->SetName("menuMainTabsLayout");
		pGuiBoxStyle->SetMargin(50);
		pGuiBoxStyle->SetSpacing(50);
	}
	{
		CAsset_GuiBoxStyle* pGuiBoxStyle = pAssetsManager->NewAsset_Hard<CAsset_GuiBoxStyle>(CAssetPath::GuiBoxStyleGui(GUIBOXSTYLE_MENU_MAINTABS_BUTTONLIST));
		pGuiBoxStyle->SetName("menuMainTabsButtonList");
		pGuiBoxStyle->SetSpacing(10);
	}
	{
		CAsset_GuiBoxStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiBoxStyle>(CAssetPath::GuiBoxStyleGui(GUIBOXSTYLE_MENU_ITEM));
		pAsset->SetName("menuItem");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_BACKGROUND));
		pAsset->SetSpacing(4);
		pAsset->SetMinHeight(24);
	}
}	

void Make_GuiLabelStyle(CSharedKernel* pKernel)
{
	CAssetsManager* pAssetsManager = pKernel->AssetsManager();
	
	//GuiLabelStyles
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_COMPOSING));
		pAsset->SetName("composing");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_COMPOSING));
		pAsset->SetPadding(4);
		pAsset->SetTextColor(vec4(15.0f/255.0f, 103.0f/255.0f, 146.0f/255.0f, 1.0f));
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_LEFT);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_LOGO));
		pAsset->SetName("menuLogo");
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(18);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_CONSOLE_LINE));
		pAsset->SetName("consoleLine");
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(14);
		pAsset->SetMinHeight(22);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_CONSOLE_LINE_HELP));
		pAsset->SetName("consoleLineHelp");
		pAsset->SetTextColor(vec4(155.0f/255.0f, 236.0f/255.0f, 24.0f/255.0f, 1.0f));
		pAsset->SetFontSize(14);
		pAsset->SetMinHeight(22);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_CONSOLE_LINE_ERROR));
		pAsset->SetName("consoleLineError");
		pAsset->SetTextColor(vec4(207.0f/255.0f, 20.0f/255.0f, 16.0f/255.0f, 1.0f));
		pAsset->SetFontSize(14);
		pAsset->SetMinHeight(22);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_CONSOLE_HINT));
		pAsset->SetName("consoleHint");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_CONSOLE_HINT));
		pAsset->SetPadding(4);
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(14);
		pAsset->SetMinHeight(18);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_CONSOLE_PROMPT_IDLE));
		pAsset->SetName("consolePromptIdle");
		pAsset->SetPadding(2);
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_CONSOLE_PROMPT_IDLE));
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(14);
		pAsset->SetMinHeight(22);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_CONSOLE_PROMPT_MO));
		pAsset->SetName("consolePromptMO");
		pAsset->SetPadding(2);
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_CONSOLE_PROMPT_MO));
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(14);
		pAsset->SetMinHeight(22);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_TEXT));
		pAsset->SetName("menuText");
		pAsset->SetPadding(2);
		pAsset->SetSpacing(4);
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(18);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_HEADER));
		pAsset->SetName("menuHeader");
		pAsset->SetPadding(4);
		pAsset->SetSpacing(4);
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(18);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_BUTTON_IDLE));
		pAsset->SetName("menuButton");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_BACKGROUND));
		pAsset->SetPadding(2);
		pAsset->SetSpacing(4);
		pAsset->SetMinHeight(24);
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(18);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_BUTTON_FOCUS));
		pAsset->SetName("menuButtonFocus");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_FOCUS));
		pAsset->SetPadding(2);
		pAsset->SetSpacing(4);
		pAsset->SetMinHeight(24);
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(18);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_BUTTON_MO));
		pAsset->SetName("menuButtonMO");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_MOUSEOVER));
		pAsset->SetPadding(2);
		pAsset->SetSpacing(4);
		pAsset->SetMinHeight(24);
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(18);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_ICON_IDLE));
		pAsset->SetName("menuIcon");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_BACKGROUND));
		pAsset->SetPadding(4);
		pAsset->SetMargin(4);
		pAsset->SetSpacing(4);
		pAsset->SetTextColor(1.0f);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER);
		pAsset->SetFontSize(14);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_ICON_MO));
		pAsset->SetName("menuIconMO");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_MOUSEOVER));
		pAsset->SetPadding(4);
		pAsset->SetMargin(4);
		pAsset->SetSpacing(4);
		pAsset->SetTextColor(1.0f);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER);
		pAsset->SetFontSize(14);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_START_IDLE));
		pAsset->SetName("menuStartButton");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_STARTBUTTON));
		pAsset->SetMinWidth(256);
		pAsset->SetMinHeight(96);
		pAsset->SetTextColor(1.0f);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED);
		pAsset->SetFontSize(18);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_START_MO));
		pAsset->SetName("menuStartButtonMO");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_STARTBUTTON_MO));
		pAsset->SetMinWidth(256);
		pAsset->SetMinHeight(96);
		pAsset->SetTextColor(1.0f);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED);
		pAsset->SetFontSize(18);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_QUIT_IDLE));
		pAsset->SetName("menuQuitButton");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_STARTBUTTON));
		pAsset->SetMinWidth(64);
		pAsset->SetMinHeight(96);
		pAsset->SetTextColor(1.0f);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED);
		pAsset->SetFontSize(18);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_QUIT_MO));
		pAsset->SetName("menuQuitButtonMO");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_STARTBUTTON_MO));
		pAsset->SetMinWidth(64);
		pAsset->SetMinHeight(96);
		pAsset->SetTextColor(1.0f);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED);
		pAsset->SetFontSize(18);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_UNIVERSE_IDLE));
		pAsset->SetName("menuUniverseButton");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_STARTBUTTON));
		pAsset->SetMinWidth(196);
		pAsset->SetMinHeight(128);
		pAsset->SetMargin(12);
		pAsset->SetSpacing(4);
		pAsset->SetTextColor(1.0f);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		pAsset->SetFontSize(18);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_UNIVERSE_MO));
		pAsset->SetName("menuUniverseButtonMO");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_STARTBUTTON_MO));
		pAsset->SetMinWidth(196);
		pAsset->SetMinHeight(128);
		pAsset->SetMargin(12);
		pAsset->SetSpacing(4);
		pAsset->SetTextColor(1.0f);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		pAsset->SetFontSize(18);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_BUTTON_IDLE));
		pAsset->SetName("menuMainTabsActiveButton");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_BACKGROUND));
		pAsset->SetPadding(2);
		pAsset->SetSpacing(4);
		pAsset->SetMinHeight(24);
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(18);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_MAINTABS_INACTIVEBUTTON));
		pAsset->SetName("menuMainTabsInactiveButton");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_BACKGROUND));
		pAsset->SetPadding(8);
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(22);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_MAINTABS_INACTIVEBUTTON_MO));
		pAsset->SetName("menuMainTabsInactiveButtonMO");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_MOUSEOVER));
		pAsset->SetPadding(8);
		pAsset->SetTextColor(1.0f);
		pAsset->SetFontSize(22);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
	}
	{
		CAsset_GuiLabelStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiLabelStyle>(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_MAINTABS_ACTIVEBUTTON));
		pAsset->SetName("menuMainTabsActiveButton");
		pAsset->SetRectPath(CAssetPath::GuiRectStyleGui(GUIRECTSTYLE_MENU_HIGHLIGHT));
		pAsset->SetPadding(8);
		pAsset->SetTextColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		pAsset->SetFontSize(22);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
	}
}

void Make_GuiButtonStyle(CSharedKernel* pKernel)
{
	CAssetsManager* pAssetsManager = pKernel->AssetsManager();
		
	//GuiButtonStyles
	{
		CAsset_GuiButtonStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiButtonStyle>(CAssetPath::GuiButtonStyleGui(GUIBUTTONSTYLE_CONSOLE_PROMPT));
		pAsset->SetName("consolePrompt");
		pAsset->SetIdleStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_CONSOLE_PROMPT_IDLE));
		pAsset->SetFocusStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_CONSOLE_PROMPT_IDLE));
		pAsset->SetMouseOverStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_CONSOLE_PROMPT_MO));
	}
	{
		CAsset_GuiButtonStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiButtonStyle>(CAssetPath::GuiButtonStyleGui(GUIBUTTONSTYLE_MENU_DEFAULT));
		pAsset->SetName("menuButton");
		pAsset->SetIdleStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_BUTTON_IDLE));
		pAsset->SetFocusStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_BUTTON_FOCUS));
		pAsset->SetMouseOverStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_BUTTON_MO));
	}
	{
		CAsset_GuiButtonStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiButtonStyle>(CAssetPath::GuiButtonStyleGui(GUIBUTTONSTYLE_MENU_ICON));
		pAsset->SetName("menuIcon");
		pAsset->SetIdleStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_ICON_IDLE));
		pAsset->SetFocusStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_ICON_IDLE));
		pAsset->SetMouseOverStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_ICON_MO));
	}
	{
		CAsset_GuiButtonStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiButtonStyle>(CAssetPath::GuiButtonStyleGui(GUIBUTTONSTYLE_MENU_START));
		pAsset->SetName("menuStartButton");
		pAsset->SetIdleStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_START_IDLE));
		pAsset->SetFocusStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_START_IDLE));
		pAsset->SetMouseOverStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_START_MO));
	}
	{
		CAsset_GuiButtonStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiButtonStyle>(CAssetPath::GuiButtonStyleGui(GUIBUTTONSTYLE_MENU_QUIT));
		pAsset->SetName("menuQuitButton");
		pAsset->SetIdleStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_QUIT_IDLE));
		pAsset->SetFocusStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_QUIT_IDLE));
		pAsset->SetMouseOverStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_QUIT_MO));
	}
	{
		CAsset_GuiButtonStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiButtonStyle>(CAssetPath::GuiButtonStyleGui(GUIBUTTONSTYLE_MENU_UNIVERSE));
		pAsset->SetName("menuUniverseButton");
		pAsset->SetIdleStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_UNIVERSE_IDLE));
		pAsset->SetFocusStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_UNIVERSE_IDLE));
		pAsset->SetMouseOverStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_UNIVERSE_MO));
	}
	{
		CAsset_GuiButtonStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiButtonStyle>(CAssetPath::GuiButtonStyleGui(GUIBUTTONSTYLE_MENU_MAINTABS_INACTIVEBUTTON));
		pAsset->SetName("menuMainTabsInactiveButton");
		pAsset->SetIdleStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_MAINTABS_INACTIVEBUTTON));
		pAsset->SetFocusStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_MAINTABS_INACTIVEBUTTON));
		pAsset->SetMouseOverStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_MAINTABS_INACTIVEBUTTON_MO));
	}
	{
		CAsset_GuiButtonStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiButtonStyle>(CAssetPath::GuiButtonStyleGui(GUIBUTTONSTYLE_MENU_MAINTABS_ACTIVEBUTTON));
		pAsset->SetName("menuMainTabsActiveButton");
		pAsset->SetIdleStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_MAINTABS_ACTIVEBUTTON));
		pAsset->SetFocusStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_MAINTABS_ACTIVEBUTTON));
		pAsset->SetMouseOverStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_MAINTABS_ACTIVEBUTTON));
	}
}

void Make_GuiToggleStyle(CSharedKernel* pKernel)
{
	CAssetsManager* pAssetsManager = pKernel->AssetsManager();
	
	//GuiToggleStyles
	{
		CAsset_GuiToggleStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiToggleStyle>(CAssetPath::GuiToggleStyleGui(GUITOGGLESTYLE_MENU_CHECKBOX));
		pAsset->SetName("menuCheckbox");
		pAsset->SetIdleTrueStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_BUTTON_IDLE));
		pAsset->SetIdleFalseStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_BUTTON_IDLE));
		pAsset->SetMouseOverTrueStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_BUTTON_MO));
		pAsset->SetMouseOverFalseStylePath(CAssetPath::GuiLabelStyleGui(GUILABELSTYLE_MENU_BUTTON_MO));
		pAsset->SetSwitchIcon(true);
		pAsset->SetIconTruePath(CAssetPath::SpriteGui(SPRITE_MENU_CHECKBOX_TRUE));
		pAsset->SetIconFalsePath(CAssetPath::SpriteGui(SPRITE_MENU_CHECKBOX_FALSE));
	}
}

void Make_GuiSliderStyle(CSharedKernel* pKernel)
{
	CAssetsManager* pAssetsManager = pKernel->AssetsManager();
		
	//GuiSliderStyles
	{
		CAsset_GuiSliderStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiSliderStyle>(CAssetPath::GuiSliderStyleGui(GUISLIDERSTYLE_MENU_DEFAULT));
		pAsset->SetName("menuSlider");
		pAsset->SetDefaultRailPath(CAssetPath::GuiLineStyleGui(GUILINESTYLE_MENU_SLIDER_RAIL));
		pAsset->SetCursorPath(CAssetPath::SpriteGui(SPRITE_MENU_SLIDER_CURSOR));
		pAsset->SetRectPath(CAssetPath::Null());
		pAsset->SetMargin(0);
		pAsset->SetPadding(12);
	}
	
	//GuiScrollbarStyles
	{
		CAsset_GuiScrollbarStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiScrollbarStyle>(CAssetPath::GuiScrollbarStyleGui(GUISCROLLBARSTYLE_MENU));
		pAsset->SetName("menuScrollbar");
		pAsset->SetDefaultRailPath(CAssetPath::GuiLineStyleGui(GUILINESTYLE_MENU_SCROLLBAR_RAIL));
		pAsset->SetDefaultSliderPath(CAssetPath::GuiLineStyleGui(GUILINESTYLE_MENU_SCROLLBAR_SLIDER));
		pAsset->SetMouseOverSliderPath(CAssetPath::GuiLineStyleGui(GUILINESTYLE_MENU_SCROLLBAR_SLIDER));
		pAsset->SetRectPath(CAssetPath::Null());
		pAsset->SetMargin(0);
		pAsset->SetPadding(24);
	}
}

void Make_GuiTabsStyle(CSharedKernel* pKernel)
{
	CAssetsManager* pAssetsManager = pKernel->AssetsManager();
	
	//GuiTabsStyle
	{
		CAsset_GuiTabsStyle* pAsset = pAssetsManager->NewAsset_Hard<CAsset_GuiTabsStyle>(CAssetPath::GuiTabsStyleGui(GUITABSSTYLE_MENU_MAIN));
		pAsset->SetName("menuMainTabs");
		pAsset->SetLayoutPath(CAssetPath::GuiBoxStyleGui(GUIBOXSTYLE_MENU_MAINTABS_LAYOUT));
		pAsset->SetButtonListPath(CAssetPath::GuiBoxStyleGui(GUIBOXSTYLE_MENU_MAINTABS_BUTTONLIST));
		pAsset->SetActiveButtonPath(CAssetPath::GuiButtonStyleGui(GUIBUTTONSTYLE_MENU_MAINTABS_ACTIVEBUTTON));
		pAsset->SetInactiveButtonPath(CAssetPath::GuiButtonStyleGui(GUIBUTTONSTYLE_MENU_MAINTABS_INACTIVEBUTTON));
		pAsset->SetButtonListFill(true);
		pAsset->SetButtonListText(true);
	}
}

int main(int argc, char* argv[])
{
	{
		std::unique_ptr<CSharedKernel> pKernel = new CSharedKernel();
		if(!pKernel->Init(argc, (const char**) argv))
		{
			debug::ErrorStream("Kernel") << "unable to initialize shared kernel" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		Make_Images(pKernel.get());
		Make_Sprites(pKernel.get());
		Make_GuiRectStyle(pKernel.get());
		Make_GuiLineStyle(pKernel.get());
		Make_GuiBoxStyle(pKernel.get());
		Make_GuiLabelStyle(pKernel.get());
		Make_GuiButtonStyle(pKernel.get());
		Make_GuiToggleStyle(pKernel.get());
		Make_GuiSliderStyle(pKernel.get());
		Make_GuiTabsStyle(pKernel.get());
		
		pKernel->AssetsManager()->Save_AssetsFile("gui/game", CAssetPath::SRC_GUI);
		
		pKernel->Shutdown();
	}
	
	exit(EXIT_SUCCESS);
}
