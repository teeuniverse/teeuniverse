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

#ifndef __SHARED_COMPONENTS_ASSETSMANAGER_HISTORY__
#define __SHARED_COMPONENTS_ASSETSMANAGER_HISTORY__

#include <shared/assets/assetpath.h>
#include <shared/tl/array.h>

class CAssetsHistory
{
private:
	enum
	{
		MAXHISTORYSIZE = 128,
	};
	
	enum
	{
		OPERATION_EDITASSET=0,
		OPERATION_ADDASSET,
	};
	
	struct COperation
	{
		int m_Operation;
		CAssetPath m_AssetPath;
		void* m_pAsset;
	
		COperation() : m_pAsset(0) { }
	};
	
	struct CEntry
	{
		int m_Token;
		array<COperation> m_Operations;
		
		CEntry() : m_Token(NO_TOKEN) { }
		void Reset();
	};
	
	class CAssetsManager* m_pAssetsManager;
	CEntry m_Entries[MAXHISTORYSIZE];
	int m_LastEntry;
	int m_Size;
	int m_LastToken;

public:
	enum
	{
		NO_TOKEN=-2,
		NEW_TOKEN=-1,
	};

private:
	void NewEntry(int Token);

public:
	CAssetsHistory(CAssetsManager* pAssetsManager);
	virtual ~CAssetsHistory();
	
	inline class CAssetsManager* AssetsManager() { return m_pAssetsManager; }
	
	void AddOperation_EditAsset(CAssetPath AssetPath, int Token);
	void AddOperation_AddAsset(CAssetPath AssetPath, int Token);
	
	void Undo();
	int GenerateToken();
};

#endif
