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

#include <game/kernel.h>
#include <client/keys.h>
#include <shared/tl/array.h>
#include <shared/tl/hashtable.h>

class CBindsManager : public CGameKernel::CComponent
{
protected:
	struct CBind
	{
		int m_Keys[2];
	};
	
protected:
	dynamic_string m_aKeyToCommand[KEY_LAST];
	hashtable<CBind, KEY_LAST> m_CommandToKey;

public:
	CBindsManager(CGameKernel* pKernel);
	
	virtual bool InitConfig(int argc, const char** argv);
	virtual void SaveConfig(class CCLI_Output* pOutput);
	
	void Bind(int Key, const char* pCommand, int BindNum = 0);
	void UnbindKey(int Key);
	void UnbindCommand(const char* pCommand, int BindNum = -1);
	void UnbindAll();
	
	int CommandToKey(const char* pCommand, int BindNum = 0) const;
	
	void OnConfigSave();
};

#endif
