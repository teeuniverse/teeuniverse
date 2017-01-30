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

#include <client/components/bindsmanager.h>
#include <shared/components/cli.h>
#include <client/components/input.h>

/* COMMANDS ***********************************************************/

	//Bind
class CCommand_Bind : public CCommandLineInterpreter::CCommand
{
protected:
	CBindsManager* m_pBindManager;

public:
	CCommand_Bind(CBindsManager* pBindManager) :
		m_pBindManager(pBindManager)
	{
		
	}
	
	virtual int Execute(const char* pArgs, CCLI_Output* pOutput)
	{
		dynamic_string Buffer;
		if(!GetString(&pArgs, Buffer))
		{
			if(pOutput)
				pOutput->Print("Missing \"Key\" parameter", CLI_LINETYPE_ERROR);
			Help(pOutput);
			return CLI_FAILURE_WRONG_PARAMETER;
		}
		
		char* pKeyName = Buffer.buffer();
		int Modifier = CBindsManager::MODIFIER_NONE;
		if(str_comp_num(Buffer.buffer(), "ctrl+", 5) == 0)
		{
			Modifier = CBindsManager::MODIFIER_CTRL;
			pKeyName += 5;
		}
		
		int Key = m_pBindManager->Input()->KeyID(pKeyName);
		if(Key == KEY_UNKNOWN)
		{
			if(pOutput)
				pOutput->Print("Unknown key", CLI_LINETYPE_ERROR);
			return CLI_FAILURE_WRONG_PARAMETER;
		}
		
		if(!GetString(&pArgs, Buffer))
		{
			if(pOutput)
				pOutput->Print("Missing \"Command\" parameter", CLI_LINETYPE_ERROR);
			Help(pOutput);
			return CLI_FAILURE_WRONG_PARAMETER;
		}
		
		m_pBindManager->Bind(Key, Modifier, Buffer.buffer());
		
		return CLI_SUCCESS;
	}
	
	virtual const char* Usage() { return "bind \"Key\" \"Command\""; }
	virtual const char* Description() { return "Associate a command to a key"; }
};

	//UnbindAll
class CCommand_UnbindAll : public CCommandLineInterpreter::CCommand
{
protected:
	CBindsManager* m_pBindManager;

public:
	CCommand_UnbindAll(CBindsManager* pBindManager) :
		m_pBindManager(pBindManager)
	{
		
	}
	
	virtual int Execute(const char* pArgs, CCLI_Output* pOutput)
	{
		m_pBindManager->UnbindAll();
		
		return CLI_SUCCESS;
	}
	
	virtual const char* Usage() { return "unbind_all"; }
	virtual const char* Description() { return "Remove all binds"; }
};

/* BINDS MANAGER ******************************************************/

CBindsManager::CBindsManager(CClientKernel* pKernel) :
	CClientKernel::CComponent(pKernel)
{
	SetName("BindsManager");
}

bool CBindsManager::InitConfig(int argc, const char** argv)
{
	//TODO: Add default binds
	
	CLI()->Register("bind", new CCommand_Bind(this));
	CLI()->Register("unbind_all", new CCommand_UnbindAll(this));
	
	return true;
}

void CBindsManager::SaveConfig(CCLI_Output* pOutput)
{
	fixed_string128 Buffer;
	
	pOutput->Print("unbind_all");
	for(int i=0; i<KEY_LAST; i++)
	{
		if(m_aKeyToCommand[MODIFIER_NONE][i].length() > 0)
		{
			str_format(Buffer.buffer(), Buffer.maxsize(), "bind %s %s", Input()->KeyName(i), m_aKeyToCommand[MODIFIER_NONE][i].buffer());
			pOutput->Print(Buffer.buffer());
		}
		if(m_aKeyToCommand[MODIFIER_CTRL][i].length() > 0)
		{
			str_format(Buffer.buffer(), Buffer.maxsize(), "bind ctrl+%s %s", Input()->KeyName(i), m_aKeyToCommand[MODIFIER_CTRL][i].buffer());
			pOutput->Print(Buffer.buffer());
		}
	}
}

bool CBindsManager::PreUpdate()
{
	for(int i=0; i<Input()->NumEvents(); i++)
	{
		CInput::CEvent Event = Input()->GetEvent(i);
		if(Event.m_Flags & CInput::FLAG_RELEASE && Event.m_Key != KEY_UNKNOWN)
		{
			if(Input()->KeyIsPressed(KEY_LCTRL))
			{
				if(m_aKeyToCommand[MODIFIER_CTRL][Event.m_Key].length() > 0)
					CLI()->Execute(m_aKeyToCommand[MODIFIER_CTRL][Event.m_Key].buffer());
			}
			else
			{
				if(m_aKeyToCommand[MODIFIER_NONE][Event.m_Key].length() > 0)
					CLI()->Execute(m_aKeyToCommand[MODIFIER_NONE][Event.m_Key].buffer());
			}
		}
	}
	
	return true;
}

void CBindsManager::UnbindAll()
{
	for(int m=0; m<NUM_MODIFIER; m++)
	{
		for(int i=0; i<KEY_LAST; i++)
			m_aKeyToCommand[m][i].clear();
	}
		
	m_CommandToKey.clear();
}

void CBindsManager::UnbindKey(int Key, int Modifier)
{
	if(Key < 0 || Key >= KEY_LAST)
		return;
	
	if(m_aKeyToCommand[Modifier][Key].length() == 0)
		return;
	
	const char* pHashTableKey = m_aKeyToCommand[Modifier][Key].buffer();
	
	CBind* pBind = m_CommandToKey.get(pHashTableKey);
	if(pBind)
	{
		if(pBind->m_Keys[0].m_KeyID < 0 || pBind->m_Keys[1].m_KeyID < 0)
			m_CommandToKey.unset(pHashTableKey);
		else if(pBind->m_Keys[0].m_KeyID == Key && pBind->m_Keys[0].m_Modifier == Modifier)
		{
			pBind->m_Keys[0].m_KeyID = -1;
			pBind->m_Keys[0].m_Modifier = MODIFIER_NONE;
		}
		else
		{
			pBind->m_Keys[1].m_KeyID = -1;
			pBind->m_Keys[1].m_Modifier = MODIFIER_NONE;
		}
	}
	
	m_aKeyToCommand[Modifier][Key].clear();
}

void CBindsManager::UnbindCommand(const char* pCommand, int BindNum)
{
	CBind* pBind = m_CommandToKey.get(pCommand);
	if(pBind)
	{
		if(BindNum == 0 || BindNum < 0)
		{
			if(pBind->m_Keys[0].m_KeyID >= 0)
			{
				m_aKeyToCommand[pBind->m_Keys[0].m_Modifier][pBind->m_Keys[0].m_KeyID].clear();
				pBind->m_Keys[0].m_KeyID = -1;
				pBind->m_Keys[0].m_Modifier = MODIFIER_NONE;
			}
		}
		if(BindNum == 1 || BindNum < 0)
		{
			if(pBind->m_Keys[1].m_KeyID >= 0)
			{
				m_aKeyToCommand[pBind->m_Keys[1].m_Modifier][pBind->m_Keys[1].m_KeyID].clear();
				pBind->m_Keys[1].m_KeyID = -1;
				pBind->m_Keys[1].m_Modifier = MODIFIER_NONE;
			}
		}
		
		if(pBind->m_Keys[0].m_KeyID < 0 && pBind->m_Keys[1].m_KeyID < 0)
			m_CommandToKey.unset(pCommand);
	}
}

void CBindsManager::Bind(int Key, int Modifier, const char* pCommand, int BindNum)
{
	if(Key < 0 || Key >= KEY_LAST)
		return;
	
	UnbindKey(Key, Modifier);
	
	CBind* pBind = m_CommandToKey.get(pCommand);
	if(pBind)
	{
		if(BindNum == 0)
		{
			if(pBind->m_Keys[0].m_KeyID >= 0)
				m_aKeyToCommand[pBind->m_Keys[0].m_Modifier][pBind->m_Keys[0].m_KeyID].clear();
		}
		else
		{
			if(pBind->m_Keys[1].m_KeyID >= 0)
				m_aKeyToCommand[pBind->m_Keys[1].m_Modifier][pBind->m_Keys[1].m_KeyID].clear();
		}
		
		pBind->m_Keys[BindNum].m_KeyID = Key;
		pBind->m_Keys[BindNum].m_Modifier = Modifier;
	}
	else
	{
		CBind Bind;
		Bind.m_Keys[BindNum].m_KeyID = Key;
		Bind.m_Keys[BindNum].m_Modifier = Modifier;
		Bind.m_Keys[(BindNum+1)%2].m_KeyID = -1;
		m_CommandToKey.set(pCommand, Bind);
	}
	
	m_aKeyToCommand[Modifier][Key].copy(pCommand);
}

bool CBindsManager::CommandToKey(const char* pCommand, CKey& Key, int BindNum) const
{
	const CBind* pBind = m_CommandToKey.get(pCommand);
	if(pBind)
	{
		Key = pBind->m_Keys[BindNum];
		return false;
	}
	
	return false;
}
