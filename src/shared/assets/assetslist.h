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

#ifndef __SHARED_ASSETLIST__
#define __SHARED_ASSETLIST__

#include <shared/assets/assetpath.h>
#include <shared/assets/assetssaveloadcontext.h>
#include <shared/archivefile.h>

template<typename ASSET, typename STATE>
class CAssetsList
{
public:
	struct CEntry
	{
		ASSET m_Asset;
		STATE m_State;
		
		void copy(const CEntry& Entry)
		{
			m_Asset.copy(Entry.m_Asset);
			m_State = Entry.m_State;
		}
		
		void transfert(CEntry& Entry)
		{
			m_Asset.transfert(Entry.m_Asset);
			m_State = Entry.m_State;
		}
	};
	
public:
	array< CEntry, allocator_copy<CEntry> > m_Assets;

public:
	inline int GetNumAssets() const { return m_Assets.size(); }

	void InitAssetState(const STATE& State)
	{
		for(int i=0; i<m_Assets.size(); i++)
		{
			m_Assets[i].m_State = State;
		}
	}
	
	const ASSET* GetAsset(const CAssetPath& Path) const
	{
		if(Path.GetType() != ASSET::TypeId)
			return 0;
			
		if(Path.GetId() >= 0 && Path.GetId() < m_Assets.size())
			return &m_Assets[Path.GetId()].m_Asset;
		else
			return 0;
	}
	
	ASSET* GetAsset(const CAssetPath& Path)
	{
		if(Path.GetType() != ASSET::TypeId)
			return 0;
			
		if(Path.GetId() >= 0 && Path.GetId() < m_Assets.size())
			return &m_Assets[Path.GetId()].m_Asset;
		else
			return 0;
	}
	
	STATE* GetAssetState(const CAssetPath& Path)
	{
		if(Path.GetType() != ASSET::TypeId)
			return 0;
			
		if(Path.GetId() >= 0 && Path.GetId() < m_Assets.size())
			return &m_Assets[Path.GetId()].m_State;
		else
			return 0;
	}
	
	ASSET* NewAsset(class CAssetsManager* pAssetsManager, CAssetPath* pPath)
	{
		int Id = m_Assets.size();
		m_Assets.increment();
		pPath->SetType(ASSET::TypeId);
		pPath->SetId(Id);
		
		m_Assets[Id].m_Asset.SetAssetsManager(pAssetsManager);
		
		return &m_Assets[Id].m_Asset;
	}
	
	ASSET* NewAsset(class CAssetsManager* pAssetsManager, const CAssetPath& Path)
	{
		if(Path.GetType() != ASSET::TypeId)
			return 0;
		
		int Id = Path.GetId();
		if(Id < 0)
			return 0;
		
		unsigned int OldSize = m_Assets.size();
		unsigned int NewSize = max(OldSize, Path.GetId()+1);
		m_Assets.resize(NewSize);
		
		for(int i=OldSize; i<NewSize; i++)
			m_Assets[i].m_Asset.SetAssetsManager(pAssetsManager);
		
		return &m_Assets[Id].m_Asset;
	}
	
	void Load_AssetsFile(class CAssetsSaveLoadContext* pLoadingContext)
	{
		int Type = ASSET::TypeId+2;
		int Num = pLoadingContext->ArchiveFile()->GetNumItems(Type);
		m_Assets.resize(Num);
		for(int i = 0; i < Num; i++)
		{
			class ASSET::CTuaType* pItem = (class ASSET::CTuaType*) pLoadingContext->ArchiveFile()->GetItem(Type, i);
			ASSET* pAsset = &m_Assets[i].m_Asset;
			pAsset->SetAssetsManager(pLoadingContext->AssetsManager());
			ASSET::CTuaType::Read(pLoadingContext, *pItem, m_Assets[i].m_Asset);
		}
	}
	
	void Save_AssetsFile(class CAssetsSaveLoadContext* pLoadingContext)
	{
		int Type = ASSET::TypeId+2;
		pLoadingContext->ArchiveFile()->SetItemType(Type, sizeof(class ASSET::CTuaType), m_Assets.size());
		
		for(int i=0; i<m_Assets.size(); i++)
		{
			class ASSET::CTuaType* pItem = (class ASSET::CTuaType*) pLoadingContext->ArchiveFile()->GetItem(Type, i);
			ASSET::CTuaType::Write(pLoadingContext, m_Assets[i].m_Asset, *pItem);
		}
	}
	
	void DeleteAsset(const CAssetPath& Path)
	{
		if(!Path.IsNull() && Path.GetType() == ASSET::TypeId)
		{
			m_Assets.remove_index(Path.GetId());
		}
	}
	
	void AssetPathOperation(const CAssetPath::COperation& Operation)
	{
		for(int i=0; i<m_Assets.size(); i++)
			m_Assets[i].m_Asset.AssetPathOperation(Operation);
	}
	
	void SubPathOperation(const CAssetPath& Path, const CSubPath::COperation& Operation)
	{
		for(int i=0; i<m_Assets.size(); i++)
			m_Assets[i].m_Asset.SubPathOperation(Path, Operation);
	}
	
	bool FindAsset(const char* pAssetName, CAssetPath* pPath) const
	{
		for(int i=0; i<m_Assets.size(); i++)
		{
			if(str_comp(pAssetName, m_Assets[i].m_Asset.GetName()) == 0)
			{
				pPath->SetType(ASSET::TypeId);
				pPath->SetId(i);
				return true;
			}
		}
		return false;
	}
};

#endif
