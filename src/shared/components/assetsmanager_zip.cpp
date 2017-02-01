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

#include "assetsmanager.h"

#include <shared/components/storage.h>
#include <shared/system/zip.h>

/* ASSETS MANAGER *****************************************************/

bool CAssetsManager::Save_ZipWithDependencies(const char* pFilename, int StorageType, int PackageId)
{
	if(!IsValidPackage(PackageId))
		return false;
	
	std::vector<int> PackageToSave;
	std::vector<int> PackageSaved;
	
	dynamic_string Buffer;
	Storage()->GetCompletePath(StorageType, pFilename, Buffer);
	fs_zipstream_wb ZipStream(Buffer.buffer());
	
	PackageToSave.push_back(PackageId);
	while(PackageToSave.size())
	{
		if(!ZipStream.is_valid())
		{
			debug::ErrorStream("AssetsSaveLoadContext") << "can't write the file " << Buffer.buffer() << std::endl;
			return false;
		}
		
		CArchiveFile ArchiveFile;
		CAssetsSaveLoadContext SaveLoadContext(this, &ArchiveFile, PackageToSave[0]);
		
		m_pPackages[PackageToSave[0]]->Save_AssetsFile(&SaveLoadContext);
		
		dynamic_string PackageFilename;
		int CharIter = 0;
		CharIter = PackageFilename.append_at(CharIter, GetPackageName(PackageToSave[0]));
		CharIter = PackageFilename.append_at(CharIter, ".tup");
		ZipStream.open_file(PackageFilename.buffer());
		if(!ArchiveFile.Write(Storage(), &ZipStream))
		{
			debug::ErrorStream("AssetsSaveLoadContext") << "can't write the file " << Buffer.buffer() << std::endl;
			return false;
		}
		
		int NumDependencies = ArchiveFile.GetNumItems(1);
		for(int i = 0; i < NumDependencies; i++)
		{
			CAssetsPackage::CTuaType_Dependency* pItem = (CAssetsPackage::CTuaType_Dependency*) ArchiveFile.GetItem(1, i);
			const char* pPackageName = ArchiveFile.GetString(pItem->m_PackageName);
			if(pPackageName != NULL)
			{
				int PackageFound = -1;
				for(int p=0; p<static_cast<int>(m_pPackages.size()); p++)
				{
					if(m_pPackages[p] && m_pPackages[p]->Identify(pPackageName, 0))
					{
						PackageFound = p;
						break;
					}
				}
				if(PackageFound >= 0)
				{
					for(int p=0; p<static_cast<int>(PackageSaved.size()); p++)
					{
						if(PackageSaved[p] == PackageFound)
						{
							PackageFound = -1;
							break;
						}
					}
				}
				if(PackageFound >= 0)
				{
					for(int p=0; p<static_cast<int>(PackageToSave.size()); p++)
					{
						if(PackageToSave[p] == PackageFound)
						{
							PackageFound = -1;
							break;
						}
					}
				}
				if(PackageFound >= 0)
					PackageToSave.push_back(PackageFound);
			}
		}
		
		PackageSaved.push_back(PackageToSave[0]);
		PackageToSave.erase(PackageToSave.begin());
		
		ZipStream.close_file();
	}
	
	ZipStream.close();
	
	return true;
}
