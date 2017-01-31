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

#include <editor/gui/preferences.h>
#include <editor/components/gui.h>
#include <client/gui/tabs.h>
#include <client/gui/text-edit.h>
#include <client/gui/filler.h>
#include <client/gui/slider.h>
#include <client/gui/bind-edit.h>

class CCloseButton : public gui::CButton
{
protected:
	CPreferences* m_pPopup;
	
protected:
	virtual void MouseClickAction() { m_pPopup->Close(); }
	
public:
	CCloseButton(CPreferences* pPopup) :
		gui::CButton(pPopup->Context(), _LSTRING("Close")),
		m_pPopup(pPopup)
	{
		SetButtonStyle(pPopup->AssetsEditor()->m_Path_Button_Dialog);
	}
};

CPreferences::CPreferences(CGuiEditor* pAssetsEditor) :
	gui::CPopup(pAssetsEditor, pAssetsEditor->GetDrawRect(), std::min(800, pAssetsEditor->GetDrawRect().w), std::min(600, pAssetsEditor->GetDrawRect().h), ALIGNMENT_INNER),
	m_pAssetsEditor(pAssetsEditor)
{
	gui::CVListLayout* pMainList = new gui::CVListLayout(Context());
	Add(pMainList);
	
	gui::CTabs* pTabs = new gui::CTabs(Context());
	pMainList->Add(pTabs, true);
	
	//General
	{
		gui::CVScrollLayout* pLayout = new gui::CVScrollLayout(Context());
		
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pHList->Add(new gui::CLabel(Context(), _LSTRING("Default author name")), true);
			pHList->Add(new gui::CExternalTextEdit_DynamicString(Context(), &AssetsEditor()->m_Cfg_DefaultAuthor), true);
			pLayout->Add(pHList, false);
		}
		
		pTabs->AddTab(pLayout, _LSTRING("General"), AssetsEditor()->m_Path_Sprite_IconSystem);
	}
	
	//Keybindings
	{
		gui::CVScrollLayout* pLayout = new gui::CVScrollLayout(Context());
		
		pLayout->Add(new gui::CLabelHeader(Context(), "General"), false);
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pHList->Add(new gui::CLabel(Context(), _LSTRING("Open package"), AssetsEditor()->m_Path_Sprite_IconSave), true);
			pHList->Add(new gui::CBindEdit(Context(), "editor_open", 0, true), true);
			pLayout->Add(pHList, false);
		}
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pHList->Add(new gui::CLabel(Context(), _LSTRING("Save package"), AssetsEditor()->m_Path_Sprite_IconSave), true);
			pHList->Add(new gui::CBindEdit(Context(), "editor_save", 0, true), true);
			pLayout->Add(pHList, false);
		}
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pHList->Add(new gui::CLabel(Context(), _LSTRING("Quit the program"), AssetsEditor()->m_Path_Sprite_IconDelete), true);
			pHList->Add(new gui::CBindEdit(Context(), "editor_quit", 0, true), true);
			pLayout->Add(pHList, false);
		}
		pLayout->Add(new gui::CLabelHeader(Context(), "Editing"), false);
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pHList->Add(new gui::CLabel(Context(), _LSTRING("Apply vertical flip to the selection"), AssetsEditor()->m_Path_Sprite_IconVFlip), true);
			pHList->Add(new gui::CBindEdit(Context(), "vflip", 0, true), true);
			pLayout->Add(pHList, false);
		}
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pHList->Add(new gui::CLabel(Context(), _LSTRING("Apply horizontal flip to the selection"), AssetsEditor()->m_Path_Sprite_IconHFlip), true);
			pHList->Add(new gui::CBindEdit(Context(), "hflip", 0, true), true);
			pLayout->Add(pHList, false);
		}
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pHList->Add(new gui::CLabel(Context(), _LSTRING("Apply clock-wise rotation to the selection"), AssetsEditor()->m_Path_Sprite_IconRotateCW), true);
			pHList->Add(new gui::CBindEdit(Context(), "cwrotation", 0, true), true);
			pLayout->Add(pHList, false);
		}
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pHList->Add(new gui::CLabel(Context(), _LSTRING("Apply counter clock-wise rotation to the selection"), AssetsEditor()->m_Path_Sprite_IconRotateCCW), true);
			pHList->Add(new gui::CBindEdit(Context(), "ccwrotation", 0, true), true);
			pHList->Add(new gui::CBindEdit(Context(), "ccwrotation", 1, true), true);
			pLayout->Add(pHList, false);
		}
		
		pTabs->AddTab(pLayout, _LSTRING("Keybindings"), AssetsEditor()->m_Path_Sprite_IconSystem);
	}
	
	//Graphics
	{
		gui::CVScrollLayout* pLayout = new gui::CVScrollLayout(Context());
		
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pHList->Add(new gui::CLabel(Context(), _LSTRING("FSAA Samples")), true);
			pHList->Add(new gui::CExternalHSlider(Context(), &Graphics()->m_Cfg_FsaaSamples, 0, 16), true);
			pLayout->Add(pHList, false);
		}
		
		pTabs->AddTab(pLayout, _LSTRING("Graphics"), AssetsEditor()->m_Path_Sprite_IconSystem);
	}
	
	//Buttons
	{
		pMainList->AddSeparator();
		
		gui::CHListLayout* pHList = new gui::CHListLayout(Context());
		pHList->Add(new gui::CFiller(Context()), true);
		pHList->Add(new CCloseButton(this), false);
		
		pMainList->Add(pHList, false);
	}
}
