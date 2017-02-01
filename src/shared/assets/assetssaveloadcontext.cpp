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

#include <shared/components/storage.h>
#include <shared/archivefile.h>
#include <shared/components/assetsmanager.h>

#include "assetssaveloadcontext.h"

/* LOADING CONTEXT ****************************************************/

CAssetsSaveLoadContext::CAssetsSaveLoadContext(CAssetsManager* pAssetsManager, CArchiveFile* pArchiveFile, int PackageId) :
	m_pAssetsManager(pAssetsManager),
	m_pArchiveFile(pArchiveFile),
	m_PackageId(PackageId)
{
	
}
		
void CAssetsSaveLoadContext::WriteAssetPath(const CAssetPath& SysType, CAssetPath::CTuaType& TuaType)
{
	const char* pAssetName = m_pAssetsManager->GetAssetValue<const char*>(SysType, CSubPath::Null(), CAsset::NAME, NULL);
	if(!pAssetName)
	{
		if(SysType.IsNotNull())
		{
			debug::ErrorStream("AssetsSaveLoadContext") << "can't store asset name " << SysType.GetPackageId() << ", " << SysType.GetType() << ", "  << SysType.GetId() << ", "  << std::endl;
		}
		
		TuaType.m_PackageId = m_pArchiveFile->WriteUInt16(0);
		TuaType.m_Type = m_pArchiveFile->WriteUInt16(CAssetPath::Null().GetRawType());
		TuaType.m_Name = m_pArchiveFile->AddString(NULL);
		return;
	}
	
	TuaType.m_Type = m_pArchiveFile->WriteUInt16(SysType.GetRawType());
	TuaType.m_Name = m_pArchiveFile->AddString(pAssetName);
	
	int AssetPkgId = SysType.GetPackageId();
	int DependencyId = 0;
	
	if(AssetPkgId == m_PackageId)
		TuaType.m_PackageId = m_pArchiveFile->WriteUInt16(0);
	else
	{
		for(unsigned int i=0; i<m_Dependencies.size(); i++)
		{
			if(m_Dependencies[i] == AssetPkgId)
			{
				DependencyId = i+1; //zero is reserved for self
				break;
			}
		}
		
		if(DependencyId == 0)
		{
			m_Dependencies.push_back(AssetPkgId);
			DependencyId = m_Dependencies.size(); //The new ID + 1
		}
		
		TuaType.m_PackageId = m_pArchiveFile->WriteUInt16(DependencyId);
	}
}

void CAssetsSaveLoadContext::ReadAssetPath(const CAssetPath::CTuaType& TuaType, CAssetPath& SysType)
{
	const char* pAssetName = m_pArchiveFile->GetString(TuaType.m_Name);
	if(!pAssetName)
	{
		SysType = CAssetPath::Null();
		return;
	}
	
	int StringID = m_pAssetsManager->AddNameToResolve(pAssetName);
	
	SysType.SetRawId(StringID);
	SysType.SetRawType(m_pArchiveFile->ReadUInt16(TuaType.m_Type));
	
	int AssetPkgId = m_pArchiveFile->ReadUInt16(TuaType.m_PackageId);
	int DependencyIndex = AssetPkgId-1;
	
	if(AssetPkgId == 0)
		SysType.SetPackageId(m_PackageId);
	else if(DependencyIndex >= 0 && DependencyIndex < static_cast<int>(m_Dependencies.size()))
		SysType.SetPackageId(m_Dependencies[DependencyIndex]);
	else
		SysType = CAssetPath::Null();
}
