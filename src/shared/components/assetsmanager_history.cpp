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

#include "assetsmanager_history.h"
#include <generated/assets/allassets.h>
#include <shared/components/assetsmanager.h>

void CAssetsHistory::CEntry::Reset()
{
	m_Operations.clear();
	m_Token = NO_TOKEN;
}

CAssetsHistory::CAssetsHistory(CAssetsManager* pAssetsManager) :
	m_pAssetsManager(pAssetsManager),
	m_LastEntry(0),
	m_Size(0),
	m_LastToken(0)
{
	
}

CAssetsHistory::~CAssetsHistory()
{
	Flush();
}

void CAssetsHistory::Flush()
{
	for(int i=0; i<m_Size; i++)
	{
		int Index = (MAXHISTORYSIZE+m_LastEntry-i)%MAXHISTORYSIZE;
		m_Entries[Index].Reset();
	}
	
	m_Size = 0;
	m_LastEntry = 0;
	m_LastToken = 0;
}

void CAssetsHistory::NewEntry(int Token)
{
	m_LastEntry = (m_LastEntry+1)%MAXHISTORYSIZE;
	if(m_Size < MAXHISTORYSIZE)
		m_Size++;
	
	m_Entries[m_LastEntry].Reset();
	
	if(Token == NEW_TOKEN)
		Token = GenerateToken();
	
	m_Entries[m_LastEntry].m_Token = Token;
}

void CAssetsHistory::AddOperation_EditAsset(CAssetPath AssetPath, int Token)
{
	if(Token == NO_TOKEN)
		return;
	
	bool NewOperationNeeded = true;
	bool NewEntryNeeded = true;
	if(m_Size > 0 && Token != NEW_TOKEN && m_Entries[m_LastEntry].m_Token == Token)
	{
		NewEntryNeeded = false;
		for(unsigned int i=0; i<m_Entries[m_LastEntry].m_Operations.size(); i++)
		{
			if(
				m_Entries[m_LastEntry].m_Operations[i].m_AssetPath == AssetPath &&
				m_Entries[m_LastEntry].m_Operations[i].m_Operation == OPERATION_EDITASSET
			)
			{
				NewOperationNeeded = false;
				break;
			}
		}
	}
	
	if(NewOperationNeeded)
	{
		if(NewEntryNeeded)
			NewEntry(Token);
		
		#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
		{\
			const CAsset_##Name* pAsset = AssetsManager()->GetAsset<CAsset_##Name>(AssetPath);\
			if(pAsset)\
			{\
				m_Entries[m_LastEntry].m_Operations.emplace_back();\
				COperation& Operation = m_Entries[m_LastEntry].m_Operations.back();\
				Operation.m_AssetPath = AssetPath;\
				CAsset_##Name* pSavedAsset = new CAsset_##Name;\
				pSavedAsset->copy(*pAsset);\
				Operation.m_pAsset.reset(pSavedAsset);\
				Operation.m_Operation = OPERATION_EDITASSET;\
			}\
			break;\
		}

		switch(AssetPath.GetType())
		{
			#include <generated/assets/assetsmacro.h>
		}
		
		#undef MACRO_ASSETTYPE
	}
}

void CAssetsHistory::AddOperation_AddAsset(CAssetPath AssetPath, int Token)
{
	if(Token == NO_TOKEN)
		return;
	
	bool NewOperationNeeded = true;
	bool NewEntryNeeded = true;
	if(m_Size > 0 && Token != NEW_TOKEN && m_Entries[m_LastEntry].m_Token == Token)
	{
		NewEntryNeeded = false;
		for(unsigned int i=0; i<m_Entries[m_LastEntry].m_Operations.size(); i++)
		{
			if(
				m_Entries[m_LastEntry].m_Operations[i].m_AssetPath == AssetPath &&
				m_Entries[m_LastEntry].m_Operations[i].m_Operation == OPERATION_ADDASSET
			)
			{
				NewOperationNeeded = false;
				break;
			}
		}
	}
	
	if(NewOperationNeeded)
	{
		if(NewEntryNeeded)
			NewEntry(Token);
		
		#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
		{\
			const CAsset_##Name* pAsset = AssetsManager()->GetAsset<CAsset_##Name>(AssetPath);\
			if(pAsset)\
			{\
				m_Entries[m_LastEntry].m_Operations.emplace_back();\
				COperation& Operation = m_Entries[m_LastEntry].m_Operations.back();\
				Operation.m_AssetPath = AssetPath;\
				Operation.m_Operation = OPERATION_ADDASSET;\
			}\
			break;\
		}

		switch(AssetPath.GetType())
		{
			#include <generated/assets/assetsmacro.h>
		}
		
		#undef MACRO_ASSETTYPE
	}
}

void CAssetsHistory::Undo()
{
	if(m_Size <= 0)
		return;
	
	for(int i=m_Entries[m_LastEntry].m_Operations.size()-1; i>=0; i--)\
	{
		#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
		{\
			if(m_Entries[m_LastEntry].m_Operations[i].m_Operation == OPERATION_EDITASSET)\
			{\
				const CAsset_##Name* pOldAsset = dynamic_cast<CAsset_##Name*>(m_Entries[m_LastEntry].m_Operations[i].m_pAsset.get());\
				AssetsManager()->SetAsset_Hard<CAsset_##Name>(m_Entries[m_LastEntry].m_Operations[i].m_AssetPath, pOldAsset);\
			}\
			else if(m_Entries[m_LastEntry].m_Operations[i].m_Operation == OPERATION_ADDASSET)\
			{\
				AssetsManager()->DeleteAsset_Hard(m_Entries[m_LastEntry].m_Operations[i].m_AssetPath);\
			}\
			break;\
		}

		switch(m_Entries[m_LastEntry].m_Operations[i].m_AssetPath.GetType())
		{
			#include <generated/assets/assetsmacro.h>
		}
		
		#undef MACRO_ASSETTYPE
	}
	
	m_Entries[m_LastEntry].Reset();
	
	m_LastEntry = (MAXHISTORYSIZE + m_LastEntry - 1)%MAXHISTORYSIZE;
	m_Size--;
}

int CAssetsHistory::GenerateToken()
{
	m_LastToken++;
	return m_LastToken;
}
