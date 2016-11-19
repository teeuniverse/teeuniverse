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

#include <game/gui/console.h>
#include <game/components/gui.h>
#include <client/gui/listlayout.h>
#include <client/gui/filler.h>
#include <client/gui/text-edit.h>
#include <shared/components/cli.h>

class CPrompt : public gui::CAbstractTextEdit
{
protected:
	CGuiGame* m_pMenu;
	
public:
	CPrompt(CGuiGame* pMenu) :
		gui::CAbstractTextEdit(pMenu),
		m_pMenu(pMenu)
	{
		SetButtonStyle(m_pMenu->m_Path_Button_ConsolePrompt);
	}


	virtual void OnInputEvent(const CInput::CEvent& Event)
	{
		if(Context()->HasFocus(this))
		{
			if((Event.m_Key == KEY_RETURN || Event.m_Key == KEY_KP_ENTER) && (Event.m_Flags & CInput::FLAG_RELEASE))
			{
				CLI()->Execute(GetText(), m_pMenu->ConsoleOutput());
				Clear();
				return;
			}
		}
		
		gui::CAbstractTextEdit::OnInputEvent(Event);
	}
	
	virtual void SaveFromTextBuffer() { }
	virtual void CopyToTextBuffer() { }
};


/* CONSOLE ************************************************************/

CConsole::CConsole(CGuiGame* pMenu) :
	gui::CVPanelLayout(pMenu),
	m_pMenu(pMenu)
{
	m_aCommandLine[0] = 0;
	m_pConsoleList = new gui::CVScrollLayout(Menu());
	m_pPrompt = new CPrompt(Menu());
	m_pHelpList = new gui::CHListLayout(Menu());
	
	gui::CVListLayout* pVList = new gui::CVListLayout(Context());
	Add(pVList, -1);
	pVList->SetBoxStyle(Menu()->m_Path_Box_Console);
	pVList->Add(m_pConsoleList, true);
	pVList->Add(m_pPrompt, false);
	pVList->Add(m_pHelpList, false);
	
	Add(new gui::CFiller(Context()), -1);
}

void CConsole::Update(bool ParentEnabled)
{
	char aBuf[256];
	
	if(str_comp(m_aCommandLine, m_pPrompt->GetText()) != 0)
	{
		str_copy(m_aCommandLine, m_pPrompt->GetText(), sizeof(m_aCommandLine));
		
		m_pHelpList->Clear();
		
		bool NoArgs = true;
		for(int i=0; m_aCommandLine[i]; i++)
		{
			if(m_aCommandLine[i] == ' ' || m_aCommandLine[i] == '\t')
				NoArgs = false;
		}
		
		if(NoArgs)
		{
			CCommandLineInterpreter::CIterator Iter;
			for(Iter = CLI()->BeginCommand(); Iter != CLI()->EndCommand(); ++Iter)
			{
				const char* pCandidate = Iter.GetName();
				if(pCandidate && str_find_nocase(pCandidate, m_aCommandLine))
				{
					gui::CLabel* pLabel = new gui::CLabel(Context(), pCandidate);
					pLabel->SetLabelStyle(Menu()->m_Path_Label_ConsoleHint);
					pLabel->NoTextClipping();
					m_pHelpList->Add(pLabel);
				}
			}
		}
		else
		{
			const char* pDescription = CLI()->Description(m_aCommandLine);
			if(pDescription)
			{
				str_format(aBuf, sizeof(aBuf), "Description: %s", pDescription);
				gui::CLabel* pLabel = new gui::CLabel(Context(), aBuf);
				pLabel->SetLabelStyle(Menu()->m_Path_Label_ConsoleHint);
				pLabel->NoTextClipping();
				m_pHelpList->Add(pLabel);
			}
			
			const char* pUsage = CLI()->Usage(m_aCommandLine);
			if(pUsage)
			{
				str_format(aBuf, sizeof(aBuf), "Usage: %s", pUsage);
				gui::CLabel* pLabel = new gui::CLabel(Context(), aBuf);
				pLabel->SetLabelStyle(Menu()->m_Path_Label_ConsoleHint);
				pLabel->NoTextClipping();
				m_pHelpList->Add(pLabel);
			}
		}
	}
	
	gui::CVPanelLayout::Update(ParentEnabled);
}
	
void CConsole::Print(const char* pText, int Type)
{
	gui::CLabel* pLabel = new gui::CLabel(Context(), pText);
	switch(Type)
	{
		case CLI_LINETYPE_ERROR:
			pLabel->SetLabelStyle(Menu()->m_Path_Label_ConsoleLineError);
			break;
		case CLI_LINETYPE_HELP:
			pLabel->SetLabelStyle(Menu()->m_Path_Label_ConsoleLineHelp);
			break;
		case CLI_LINETYPE_NORMAL:
		default:
			pLabel->SetLabelStyle(Menu()->m_Path_Label_ConsoleLine);
	}
	m_pConsoleList->Add(pLabel);
}
