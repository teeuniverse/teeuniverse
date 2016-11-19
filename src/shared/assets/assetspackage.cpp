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

#include <shared/assets/assetspackage.h>
#include <shared/components/assetsmanager.h>

CAssetsPackage::CAssetsPackage() :
	m_State(STATE_CREATED),
	m_ReadOnly(true)
{
	
}

void CAssetsPackage::InitAssetState(const CAssetState& State)
{
	#define MACRO_ASSETTYPE(Name) m_##Name.InitAssetState(State);
	#include <generated/assets/assetsmacro.h>
	#undef MACRO_ASSETTYPE
}
	
void CAssetsPackage::Load_AssetsFile(class CAssetsSaveLoadContext* pLoadingContext)
{
	#define MACRO_ASSETTYPE(Name) m_##Name.Load_AssetsFile(pLoadingContext);
	#include <generated/assets/assetsmacro.h>
	#undef MACRO_ASSETTYPE
	
	m_State = STATE_JUST_LOADED;
}
	
void CAssetsPackage::Save_AssetsFile(class CAssetsSaveLoadContext* pLoadingContext)
{
	pLoadingContext->ArchiveFile()->SetItemType(0, sizeof(CTuaType_Info), 1);
	CTuaType_Info* pItem = reinterpret_cast<CTuaType_Info*>(pLoadingContext->ArchiveFile()->GetItem(0, 0));
	pItem->m_Version = pLoadingContext->ArchiveFile()->WriteUInt32((uint32) 0);
	
	#define MACRO_ASSETTYPE(Name) m_##Name.Save_AssetsFile(pLoadingContext);
	#include <generated/assets/assetsmacro.h>
	#undef MACRO_ASSETTYPE
	
	pLoadingContext->ArchiveFile()->SetItemType(1, sizeof(CAssetsPackage::CTuaType_Dependency), pLoadingContext->Dependencies().size());
	for(int i = 0; i < pLoadingContext->Dependencies().size(); i++)
	{
		const char* pPackageName = pLoadingContext->AssetsManager()->GetPackageName(pLoadingContext->Dependencies()[i]);
		CAssetsPackage::CTuaType_Dependency* pItem = reinterpret_cast<CAssetsPackage::CTuaType_Dependency*>(pLoadingContext->ArchiveFile()->GetItem(1, i));
		pItem->m_PackageName = pLoadingContext->ArchiveFile()->AddString(pPackageName);
		pItem->m_PackageCrc = pLoadingContext->ArchiveFile()->WriteUInt32(0);
	}
}

void CAssetsPackage::DeleteAsset(const CAssetPath& Path)
{
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
		m_##Name.DeleteAsset(Path);\
		break;
	switch(Path.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	#undef MACRO_ASSETTYPE
}

void CAssetsPackage::AssetPathOperation(const CAssetPath::COperation& Operation)
{
	#define MACRO_ASSETTYPE(Name) m_##Name.AssetPathOperation(Operation);
	#include <generated/assets/assetsmacro.h>
	#undef MACRO_ASSETTYPE
}

void CAssetsPackage::SubPathOperation(const CAssetPath& Path, const CSubPath::COperation& Operation)
{
	#define MACRO_ASSETTYPE(Name) m_##Name.SubPathOperation(Path, Operation);
	#include <generated/assets/assetsmacro.h>
	#undef MACRO_ASSETTYPE
}

#define MACRO_ASSETTYPE(Name) template<> CAssetsList<CAsset_##Name, CAssetState>& CAssetsPackage::GetList() { return m_##Name; }
#include <generated/assets/assetsmacro.h>
#undef MACRO_ASSETTYPE

#define MACRO_ASSETTYPE(Name) template<> const CAssetsList<CAsset_##Name, CAssetState>& CAssetsPackage::GetList() const { return m_##Name; }
#include <generated/assets/assetsmacro.h>
#undef MACRO_ASSETTYPE
