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

#include <game/components/bindsmanager.h>
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
			pOutput->Print("Missing \"Key\" parameter", CLI_LINETYPE_ERROR);
			Help(pOutput);
			return CLI_FAILURE_WRONG_PARAMETER;
		}
		
		int Key = m_pBindManager->Input()->KeyID(Buffer.buffer());
		if(Key == KEY_UNKNOWN)
		{
			pOutput->Print("Unknown key", CLI_LINETYPE_ERROR);
			return CLI_FAILURE_WRONG_PARAMETER;
		}
		
		if(!GetString(&pArgs, Buffer))
		{
			pOutput->Print("Missing \"Command\" parameter", CLI_LINETYPE_ERROR);
			Help(pOutput);
			return CLI_FAILURE_WRONG_PARAMETER;
		}
		
		
		
		return CLI_SUCCESS;
	}
	
	virtual const char* Usage() { return "bind \"Key\" \"Command\""; }
	virtual const char* Description() { return "Associate a command to a key"; }
};

/* BINDS MANAGER ******************************************************/

CBindsManager::CBindsManager(CGameKernel* pKernel) :
	CGameKernel::CComponent(pKernel)
{
	SetName("BindsManager");
}

bool CBindsManager::InitConfig(int argc, const char** argv)
{
	UnbindAll();
	
	//TODO: Add default binds
	
	CLI()->Register("bind", new CCommand_Bind(this));
	
	return true;
}

void CBindsManager::SaveConfig(CCLI_Output* pOutput)
{
	fixed_string128 Buffer;
	
	for(int i=0; i<KEY_LAST; i++)
	{
		if(m_aKeyToCommand[i].length() > 0)
		{
			str_format(Buffer.buffer(), Buffer.maxsize(), "bind %s %s", Input()->KeyName(i), m_aKeyToCommand[i].buffer()); pOutput->Print(Buffer.buffer());
		}
	}
}

void CBindsManager::UnbindAll()
{
	for(int i=0; i<KEY_LAST; i++)
		m_aKeyToCommand[i].clear();
		
	m_CommandToKey.clear();
}

void CBindsManager::UnbindKey(int Key)
{
	if(Key < 0 || Key >= KEY_LAST)
		return;
	
	if(m_aKeyToCommand[Key].length() == 0)
		return;
	
	const char* pHashTableKey = m_aKeyToCommand[Key].buffer();
	
	CBind* pBind = m_CommandToKey.get(pHashTableKey);
	if(pBind)
	{
		if(pBind->m_Keys[0] < 0 || pBind->m_Keys[1] < 0)
			m_CommandToKey.unset(pHashTableKey);
		else if(pBind->m_Keys[0] == Key)
			pBind->m_Keys[0] = -1;
		else
			pBind->m_Keys[1] = -1;
	}
	
	m_aKeyToCommand[Key].clear();
}

void CBindsManager::UnbindCommand(const char* pCommand, int BindNum)
{
	CBind* pBind = m_CommandToKey.get(pCommand);
	if(pBind)
	{
		if(BindNum == 0 || BindNum < 0)
		{
			if(pBind->m_Keys[0] >= 0)
			{
				m_aKeyToCommand[pBind->m_Keys[0]].clear();
				pBind->m_Keys[0] = -1;
			}
		}
		if(BindNum == 1 || BindNum < 0)
		{
			if(pBind->m_Keys[1] >= 0)
			{
				m_aKeyToCommand[pBind->m_Keys[1]].clear();
				pBind->m_Keys[1] = -1;
			}
		}
		
		if(pBind->m_Keys[0] < 0 && pBind->m_Keys[1] < 0)
			m_CommandToKey.unset(pCommand);
	}
}

void CBindsManager::Bind(int Key, const char* pCommand, int BindNum)
{
	if(Key < 0 || Key >= KEY_LAST)
		return;
	
	UnbindKey(Key);
	
	CBind* pBind = m_CommandToKey.get(pCommand);
	if(pBind)
	{
		if(BindNum == 0)
		{
			if(pBind->m_Keys[0] >= 0)
				m_aKeyToCommand[pBind->m_Keys[0]].clear();
		}
		else
		{
			if(pBind->m_Keys[1] >= 0)
				m_aKeyToCommand[pBind->m_Keys[1]].clear();
		}
		
		pBind->m_Keys[BindNum] = Key;
	}
	else
	{
		CBind Bind;
		Bind.m_Keys[BindNum] = Key;
		Bind.m_Keys[(BindNum+1)%2] = -1;
		m_CommandToKey.set(pCommand, Bind);
	}
	
	m_aKeyToCommand[Key].copy(pCommand);
}

int CBindsManager::CommandToKey(const char* pCommand, int BindNum) const
{
	const CBind* pBind = m_CommandToKey.get(pCommand);
	if(pBind)
		return pBind->m_Keys[BindNum];
	
	return -1;
}
