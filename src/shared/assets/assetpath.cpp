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

#include "assetpath.h"

#include <generated/assets/allassets.h>
#include <shared/components/assetsmanager.h>

void CAssetPath::COperation::Apply(CAssetPath& Path) const
{
	if(m_OperationId == OPERATION_DELETE)
	{
		if(Path.GetPackageId() != m_OpDelete.m_PackageId)
			return;
		if(Path.GetType() != m_OpDelete.m_Type)
			return;
		if(Path.GetId() < m_OpDelete.m_Id)
			return;
			
		if(Path.GetId() == m_OpDelete.m_Id)
			Path = CAssetPath::Null();
		else
			Path.SetId(Path.GetId()-1);
	}
	else if(m_OperationId == OPERATION_RESOLVE_NAME)
	{
		int StringId = Path.GetRawId();
		if(StringId >= 0 && StringId < m_OpResolveName.m_pNames->size())
		{
			const char* pName = (*m_OpResolveName.m_pNames)[StringId];
			
			#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
				Path = m_OpResolveName.m_pAssetsManager->FindAsset<CAsset_##Name>(Path.GetPackageId(), pName);\
				return;
			
			switch(Path.GetType())
			{
				#include <generated/assets/assetsmacro.h>
			}
			
			#undef MACRO_ASSETTYPE
		}
		
		Path = CAssetPath::Null();
	}
}

void CSubPath::COperation::Apply(CSubPath& Path) const
{
	if(Path.GetType() != m_Type)
		return;
	if(Path.GetId() < m_IdStart || Path.GetId() >= m_IdEnd)
		return;
	
	switch(m_Type)
	{
		case OPERATION_SHIFTID:
			Path.SetId(Path.GetId()+m_Shift);
			break;
	}
}
