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

#include "asset.h"
#include <shared/assets/assetssaveloadcontext.h>
#include <shared/archivefile.h>

void CAsset::CTuaType::Read(CAssetsSaveLoadContext* pLoadingContext, const CTuaType& TuaType, CAsset& SysType)
{
	SysType.SetName(pLoadingContext->ArchiveFile()->GetString(TuaType.m_Name));
}

void CAsset::CTuaType::Write(CAssetsSaveLoadContext* pLoadingContext, const CAsset& SysType, CTuaType& TuaType)
{
	TuaType.m_Name = pLoadingContext->ArchiveFile()->AddString(SysType.GetName());
}

template<>
const char* CAsset::GetValue(int ValueType, const CSubPath& SubPath, const char* DefaultValue) const
{
	switch(ValueType)
	{
		case NAME:
			return GetName();
	}
	
	return DefaultValue;
}

template<>
bool CAsset::SetValue(int ValueType, const CSubPath& SubPath, const char* pText)
{
	switch(ValueType)
	{
		case NAME:
			SetName(pText);
			return true;
	}
	
	return false;
}
