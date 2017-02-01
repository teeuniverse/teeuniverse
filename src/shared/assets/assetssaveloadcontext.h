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

#ifndef __SHARED_ASSETSSAVELOADCONTEXT__
#define __SHARED_ASSETSSAVELOADCONTEXT__

#include <shared/assets/assetpath.h>
#include <shared/archivefile.h>

#include <vector>

class CAssetsSaveLoadContext
{
private:
	class CAssetsManager* m_pAssetsManager;
	CArchiveFile* m_pArchiveFile;
	int m_PackageId;
	std::vector<int> m_Dependencies;
	
public:
	CAssetsSaveLoadContext(CAssetsManager* pAssetsManager, CArchiveFile* pArchiveFile, int PackageId);
	
	void WriteAssetPath(const CAssetPath& SysType, CAssetPath::CTuaType& TuaType);
	void ReadAssetPath(const CAssetPath::CTuaType& TuaType, CAssetPath& SysType);
	
	inline CAssetsManager* AssetsManager() { return m_pAssetsManager; }
	inline CArchiveFile* ArchiveFile() { return m_pArchiveFile; }
	
	inline void SetPackageId(int PackageId) { m_PackageId = PackageId; }
	inline void AddDependency(int PackageId) { m_Dependencies.push_back(PackageId); }
	inline const std::vector<int>& Dependencies() const { return m_Dependencies; }
};

#endif
