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

/*
 * THIS FILE HAS BEEN GENERATED BY A SCRIPT.
 * DO NOT EDIT MANUALLY!
 *
 * Please use the script "scripts/assets/assets.py" to regenerate it.
 *
 * Why this file is generated by a script?
 * Because there is more than 10 files that follow the same format.
 * In addition, each time a new member is added, enums, getter, setters,
 * copy/transfert functions and storage structure must be updated.
 * It's too much to avoid mistakes.
 */

#include <generated/assets/entitytype.h>
#include <shared/assets/assetssaveloadcontext.h>
#include <shared/archivefile.h>


void CAsset_EntityType::CTuaType::Read(CAssetsSaveLoadContext* pLoadingContext, const CTuaType& TuaType, CAsset_EntityType& SysType)
{
	CAsset::CTuaType::Read(pLoadingContext, TuaType, SysType);

	pLoadingContext->ReadAssetPath(TuaType.m_GizmoPath, SysType.m_GizmoPath);
	SysType.m_CollisionRadius = pLoadingContext->ArchiveFile()->ReadFloat(TuaType.m_CollisionRadius);
}

void CAsset_EntityType::CTuaType::Write(CAssetsSaveLoadContext* pLoadingContext, const CAsset_EntityType& SysType, CTuaType& TuaType)
{
	CAsset::CTuaType::Write(pLoadingContext, SysType, TuaType);

	pLoadingContext->WriteAssetPath(SysType.m_GizmoPath, TuaType.m_GizmoPath);
	TuaType.m_CollisionRadius = pLoadingContext->ArchiveFile()->WriteFloat(SysType.m_CollisionRadius);
}

template<>
float CAsset_EntityType::GetValue(int ValueType, const CSubPath& SubPath, float DefaultValue) const
{
	switch(ValueType)
	{
		case COLLISIONRADIUS:
			return GetCollisionRadius();
	}
	return CAsset::GetValue<float>(ValueType, SubPath, DefaultValue);
}

template<>
bool CAsset_EntityType::SetValue(int ValueType, const CSubPath& SubPath, float Value)
{
	switch(ValueType)
	{
		case COLLISIONRADIUS:
			SetCollisionRadius(Value);
			return true;
	}
	return CAsset::SetValue<float>(ValueType, SubPath, Value);
}

template<>
CAssetPath CAsset_EntityType::GetValue(int ValueType, const CSubPath& SubPath, CAssetPath DefaultValue) const
{
	switch(ValueType)
	{
		case GIZMOPATH:
			return GetGizmoPath();
	}
	return CAsset::GetValue<CAssetPath>(ValueType, SubPath, DefaultValue);
}

template<>
bool CAsset_EntityType::SetValue(int ValueType, const CSubPath& SubPath, CAssetPath Value)
{
	switch(ValueType)
	{
		case GIZMOPATH:
			SetGizmoPath(Value);
			return true;
	}
	return CAsset::SetValue<CAssetPath>(ValueType, SubPath, Value);
}

int CAsset_EntityType::AddSubItem(int Type, const CSubPath& SubPath)
{
	return -1;
}


