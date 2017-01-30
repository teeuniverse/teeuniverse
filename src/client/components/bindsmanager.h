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

#ifndef __CLIENT_BINDSMANAGER__
#define __CLIENT_BINDSMANAGER__

#include <client/kernel.h>
#include <client/keys.h>
#include <shared/tl/array.h>
#include <shared/tl/hashtable.h>

class CBindsManager : public CClientKernel::CComponent
{
public:
	enum
	{
		MODIFIER_NONE=0,
		MODIFIER_CTRL,
		NUM_MODIFIER,
	};
	
	struct CKey
	{
		int m_KeyID;
		int m_Modifier;
	};
	
protected:
	struct CBind
	{
		CKey m_Keys[2];
	};
	
protected:
	dynamic_string m_aKeyToCommand[NUM_MODIFIER][KEY_LAST];
	hashtable<CBind, KEY_LAST> m_CommandToKey;

public:
	CBindsManager(CClientKernel* pKernel);
	
	virtual bool InitConfig(int argc, const char** argv);
	virtual void SaveConfig(class CCLI_Output* pOutput);
	virtual bool PreUpdate();
	
	void Bind(int Key, int Modifier, const char* pCommand, int BindNum = 0);
	void UnbindKey(int Key, int Modifier);
	void UnbindCommand(const char* pCommand, int BindNum = -1);
	void UnbindAll();
	
	bool CommandToKey(const char* pCommand, CKey& Key, int BindNum = 0) const;
	
	void OnConfigSave();
};

#endif
