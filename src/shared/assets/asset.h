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

#ifndef __CLIENT_ASSETS__
#define __CLIENT_ASSETS__

#include <shared/system/string.h>
#include <shared/assets/assetpath.h>
#include <shared/archivefile.h>

#include <utility>

#define ASSET_TUATYPE_VERSION(Version) class CTuaType_##Version\
{\
public:\
	tua_stringid m_Name;\
	static void Read(class CAssetsSaveLoadContext* pLoadingContext, const CTuaType_##Version& TuaType, CAsset& SysType);\
	static void Write(class CAssetsSaveLoadContext* pLoadingContext, const CAsset& SysType, CTuaType_##Version& TuaType);\
};

class CAsset
{
public:
	//TAG_ASSETSVERSION
	
	ASSET_TUATYPE_VERSION(0_1_0)
	ASSET_TUATYPE_VERSION(0_2_0)
	ASSET_TUATYPE_VERSION(0_2_1)
	ASSET_TUATYPE_VERSION(0_2_2)
	ASSET_TUATYPE_VERSION(0_2_3)

private:
	class CAssetsManager* m_pAssetsManager;
	dynamic_string m_Name;

public:
	CAsset() : m_pAssetsManager(0) { }
	virtual ~CAsset() { }

	void Update() { }
	void Unload() { }
	
	inline const char* GetName() const { return m_Name.buffer(); }
	inline void SetName(const char* pName) { m_Name = pName; }
	
	inline void SetAssetsManager(class CAssetsManager* pAssetsManager) { m_pAssetsManager = pAssetsManager; }
	
	inline class CAssetsManager* AssetsManager() { return m_pAssetsManager; }
	inline class CAssetsManager* AssetsManager() const { return m_pAssetsManager; }

public:
	enum
	{
		NAME = 0, //String
		NUM_MEMBERS,
	};
	
	template<typename T>
	T GetValue(int ValueType, const CSubPath& Path, T DefaultValue) const { return DefaultValue; }
	
	template<typename T>
	bool SetValue(int ValueType, const CSubPath& Path, T Value) { return false; }
	
	void AssetPathOperation(const CAssetPath::COperation& Operation) { }
	void SubPathOperation(const CAssetPath& Path, const CSubPath::COperation& Operation) { }
};

template<> const char* CAsset::GetValue<const char*>(int ValueType, const CSubPath& Path, const char* DefaultValue) const;
template<> bool CAsset::SetValue<const char*>(int ValueType, const CSubPath& Path, const char* Value);

#endif
