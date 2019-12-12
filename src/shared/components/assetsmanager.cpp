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
#include <shared/components/cli.h>
#include <shared/assets/assetssaveloadcontext.h>
#include <shared/archivefile.h>

#include "assetsmanager.h"

/* COMMANDS ***********************************************************/

	//add_package_dir
class CCommand_AddPackageDir : public CCommandLineInterpreter::CCommand
{
protected:
	CAssetsManager* m_pAssetsManager;
	
public:
	CCommand_AddPackageDir(CAssetsManager* pAssetsManager) :
		CCommandLineInterpreter::CCommand(),
		m_pAssetsManager(pAssetsManager)
	{ }

	virtual int Execute(const char* pArgs, CCLI_Output* pOutput)
	{
		dynamic_string Buffer;
		if(!GetString(&pArgs, Buffer))
		{
			if(pOutput)
				pOutput->Print("Missing \"Directory\" parameter", CLI_LINETYPE_ERROR);
			Help(pOutput);
			return CLI_FAILURE_WRONG_PARAMETER;
		}
		
		m_pAssetsManager->AddDirectory(Buffer.buffer());
		
		return CLI_SUCCESS;
	}
	
	virtual const char* Usage() { return "add_package_dir \"Directory\""; }
	virtual const char* Description() { return "Add \"Directory\" as a source of package"; }
};

/* ASSETS MANAGER *****************************************************/
	
CAssetsManager::CAssetsManager(CSharedKernel* pKernel) :
	CSharedKernel::CComponent(pKernel),
	m_PackageId_UnivTeeWorlds(-1),
	m_PackageId_UnivDDNet(-1),
	m_PackageId_UnivOpenFNG(-1),
	m_PackageId_UnivNinslash(-1),
	m_PackageId_UnivSport(-1),
	m_PackageId_EnvClouds(-1),
	m_PackageId_EnvDesert(-1),
	m_PackageId_EnvGeneric(-1),
	m_PackageId_EnvGrass(-1),
	m_PackageId_EnvJungle(-1),
	m_PackageId_EnvMoon(-1),
	m_PackageId_EnvMountains(-1),
	m_PackageId_EnvSnow(-1),
	m_PackageId_EnvStars(-1),
	m_PackageId_EnvSun(-1),
	m_PackageId_EnvWinter(-1),
	m_PackageId_EnvLab(-1),
	m_PackageId_EnvFactory(-1)
{
	SetName("AssetsManager");
}

void CAssetsManager::RequestUpdate(const CAssetPath& AssetPath)
{
	if(AssetPath.GetType() == CAsset_Image::TypeId)
	{
		for(unsigned int i=0; i<m_ImagesToUpdate.size(); i++)
		{
			if(m_ImagesToUpdate[i].m_Path == AssetPath)
				return;
		}
		
		m_ImagesToUpdate.emplace_back();
		CAssetUpdateDesc& Desc = m_ImagesToUpdate.back();
		Desc.m_Path = AssetPath;
		Desc.m_Updated = false;
	}
}

bool CAssetsManager::InitConfig(int argc, const char** argv)
{
	CLI()->Register("add_package_dir", new CCommand_AddPackageDir(this));
	
	return true;
}

bool CAssetsManager::Init()
{
	for(int i=0; i<Storage()->GetNumPaths(); i++)
	{
		m_Directories.emplace_back();
		dynamic_string& Directory = m_Directories.back();
		Directory.clear();
		Storage()->GetCompletePath(i, "packages", Directory);
	}
	
	return true;
}

bool CAssetsManager::PostUpdate()
{
	for(int i=m_ImagesToUpdate.size()-1; i>=0; i--)
	{
		if(m_ImagesToUpdate[i].m_Updated)
			m_ImagesToUpdate.erase(m_ImagesToUpdate.begin() + i);
	}
	
	return true;
}

void CAssetsManager::SaveConfig(CCLI_Output* pOutput)
{
	dynamic_string Buffer;
	
	for(unsigned int i=2; i<m_Directories.size(); i++)
	{
		str_format(Buffer.buffer(), Buffer.maxsize(), "add_package_dir \"%s\"", m_Directories[i].buffer());
		pOutput->Print(Buffer.buffer());
	}
}

int CAssetsManager::NewPackage(const char* pName)
{
	//Search for an empty place
	int PackageId = -1;
	for(unsigned int i=0; i<m_pPackages.size(); i++)
	{
		if(!m_pPackages[i])
		{
			PackageId = i;
			m_pPackages[PackageId].reset(new CAssetsPackage());
		}
	}
	
	if(PackageId < 0)
	{
		PackageId = m_pPackages.size();
		m_pPackages.emplace_back();
		m_pPackages[PackageId].reset(new CAssetsPackage());
	}
	
	if(pName)
		SetPackageName_Hard(PackageId, pName);
		
	if(m_pHistory)
		m_pHistory->Flush();
	
	return PackageId;
}

const char* CAssetsManager::GetPackageName(int PackageId) const
{
	if(!IsValidPackage(PackageId))
		return "";
	
	return m_pPackages[PackageId]->GetName();
}

void CAssetsManager::SetPackageName(int PackageId, const char* pName)
{
	if(!IsValidPackage(PackageId) || IsReadOnlyPackage(PackageId))
		return;
	SetPackageName_Hard(PackageId, pName);
	m_pPackages[PackageId]->SetEdited(true);
}

void CAssetsManager::SetPackageName_Hard(int PackageId, const char* pName)
{
	if(!IsValidPackage(PackageId) || !pName)
		return;
	
	char aNumBuffer[32];
	dynamic_string Buffer;
	Buffer = pName;
	
	int NumDuplicates = 0;
	bool DuplicateFound = true;
	while(DuplicateFound)
	{
		DuplicateFound = false;
		for(int i=0; i<static_cast<int>(m_pPackages.size()); i++)
		{
			if(m_pPackages[i] && i != PackageId)
			{
				if(Buffer == m_pPackages[i]->GetName())
				{
					str_format(aNumBuffer, sizeof(aNumBuffer), "%d", NumDuplicates+2);
					NumDuplicates++;
					
					int BufferIter = 0;
					
					Buffer.clear();
					BufferIter = Buffer.append_at(BufferIter, pName);
					BufferIter = Buffer.append_at(BufferIter, "_");
					BufferIter = Buffer.append_at(BufferIter, aNumBuffer);
					DuplicateFound = true;
					break;
				}
			}
		}
	}
	
	m_pPackages[PackageId]->SetName(Buffer.buffer());
}

const char* CAssetsManager::GetPackageDirectory(int PackageId) const
{
	if(!IsValidPackage(PackageId))
		return "";
	
	return m_pPackages[PackageId]->GetDirectory();
}

void CAssetsManager::SetPackageDirectory(int PackageId, const char* pName)
{
	if(!IsValidPackage(PackageId) || IsReadOnlyPackage(PackageId))
		return;
	m_pPackages[PackageId]->SetDirectory(pName);
}

void CAssetsManager::TryChangeAssetName(CAssetPath AssetPath, const char* pName, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return;
	
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
	
	char aNumBuffer[32];
	dynamic_string Buffer;
	Buffer = pName;
	
	int NumDuplicates = 0;
	bool DuplicateFound = true;
		
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
		while(DuplicateFound)\
		{\
			DuplicateFound = false;\
			for(int i=0; i<GetNumAssets<CAsset_##Name>(AssetPath.GetPackageId()); i++)\
			{\
				const CAsset_##Name* pAsset = GetAsset<CAsset_##Name>(CAssetPath(CAsset_##Name::TypeId, AssetPath.GetPackageId(), i));\
				if(pAsset)\
				{\
					if(Buffer == pAsset->GetName())\
					{\
						str_format(aNumBuffer, sizeof(aNumBuffer), "%d", NumDuplicates+2);\
						NumDuplicates++;\
						int BufferIter = 0;\
						Buffer.clear();\
						BufferIter = Buffer.append_at(BufferIter, pName);\
						BufferIter = Buffer.append_at(BufferIter, "_");\
						BufferIter = Buffer.append_at(BufferIter, aNumBuffer);\
						DuplicateFound = true;\
						break;\
					}\
				}\
			}\
		}\
		SetAssetValue<const char*>(AssetPath, CSubPath::Null(), CAsset::NAME, Buffer.buffer(), Token);\
		break;

	switch(AssetPath.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);

	#undef MACRO_ASSETTYPE
}

void CAssetsManager::TryChangeAssetName_Hard(CAssetPath AssetPath, const char* pName)
{
	char aNumBuffer[32];
	dynamic_string Buffer;
	Buffer = pName;
	
	int NumDuplicates = 0;
	bool DuplicateFound = true;
		
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
		while(DuplicateFound)\
		{\
			DuplicateFound = false;\
			for(int i=0; i<GetNumAssets<CAsset_##Name>(AssetPath.GetPackageId()); i++)\
			{\
				const CAsset_##Name* pAsset = GetAsset<CAsset_##Name>(CAssetPath(CAsset_##Name::TypeId, AssetPath.GetPackageId(), i));\
				if(pAsset)\
				{\
					if(Buffer == pAsset->GetName())\
					{\
						str_format(aNumBuffer, sizeof(aNumBuffer), "%d", NumDuplicates+2);\
						NumDuplicates++;\
						int BufferIter = 0;\
						Buffer.clear();\
						BufferIter = Buffer.append_at(BufferIter, pName);\
						BufferIter = Buffer.append_at(BufferIter, "_");\
						BufferIter = Buffer.append_at(BufferIter, aNumBuffer);\
						DuplicateFound = true;\
						break;\
					}\
				}\
			}\
		}\
		SetAssetValue_Hard<const char*>(AssetPath, CSubPath::Null(), CAsset::NAME, Buffer.buffer());\
		break;

	switch(AssetPath.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);

	#undef MACRO_ASSETTYPE
}

int CAssetsManager::GetNumPackages() const
{
	return m_pPackages.size();
}

bool CAssetsManager::IsValidPackage(int PackageId) const
{
	return (PackageId >= 0 && PackageId < static_cast<int>(m_pPackages.size()) && m_pPackages[PackageId]);
}

bool CAssetsManager::IsReadOnlyPackage(int PackageId) const
{
	return m_pPackages[PackageId]->IsReadOnly();
}

void CAssetsManager::SetPackageEdited(int PackageId , bool Value)
{
	if(IsValidPackage(PackageId))
		m_pPackages[PackageId]->SetEdited(Value);
}

bool CAssetsManager::IsEditedPackage(int PackageId) const
{
	return m_pPackages[PackageId]->IsEdited();
}

void CAssetsManager::SetPackageReadOnly(int PackageId , bool Value)
{
	if(IsValidPackage(PackageId))
		m_pPackages[PackageId]->SetReadOnly(Value);
}

void CAssetsManager::ClosePackage(int PackageId)
{
	if(!IsValidPackage(PackageId))
		return;
	
	m_pPackages[PackageId].reset();
	
	if(m_pHistory)
		m_pHistory->Flush();
}

int CAssetsManager::AddNameToResolve(const char* pName)
{
	for(unsigned int i=0; i<m_pNamesToResolved.size(); i++)
	{
		if(m_pNamesToResolved[i] == pName)
			return static_cast<int>(i);
	}
	
	m_pNamesToResolved.push_back(pName);
	
	return m_pNamesToResolved.size()-1;
}

bool CAssetsManager::GetPackageSaveFilename(int PackageId, dynamic_string& Filename)
{
	if(!IsValidPackage(PackageId))
		return false;
	
	Filename.clear();
	int CharPos = 0;
	CharPos = Filename.append_at(CharPos, m_pPackages[PackageId]->GetDirectory());
	CharPos = Filename.append_at(CharPos, "/");
	CharPos = Filename.append_at(CharPos, m_pPackages[PackageId]->GetName());
	CharPos = Filename.append_at(CharPos, ".tup");
	
	return true;
}

bool CAssetsManager::Save_AssetsFile(int PackageId, const char* pFilename)
{
	if(!IsValidPackage(PackageId))
		return false;
	
	CArchiveFile ArchiveFile;
	CAssetsSaveLoadContext SaveLoadContext(this, &ArchiveFile, PackageId);
	
	m_pPackages[PackageId]->Save_AssetsFile(&SaveLoadContext);
	
	if(!ArchiveFile.Write(Storage(), pFilename, CStorage::TYPE_ABSOLUTE))
	{
		debug::WarningStream("AssetsManager") << "can't write the file " << pFilename << std::endl;
		return false;
	}
	
	m_pPackages[PackageId]->SetEdited(false);
	
	return true;
}

bool CAssetsManager::Save_AssetsFile_SaveDir(int PackageId)
{
	if(!IsValidPackage(PackageId))
		return false;
	
	dynamic_string Buffer;
	int CharPos = 0;
	CharPos = Buffer.append_at(CharPos, "packages/");
	CharPos = Buffer.append_at(CharPos, m_pPackages[PackageId]->GetName());
	CharPos = Buffer.append_at(CharPos, ".tup");
	
	dynamic_string Filename;
	Storage()->GetCompletePath(CStorage::TYPE_SAVE, Buffer.buffer(), Filename);
	
	return Save_AssetsFile(PackageId, Filename.buffer());
}

bool CAssetsManager::AddDirectory(const char* pDirectory)
{
	for(unsigned int i=0; i<m_Directories.size(); i++)
	{
		if(str_comp(m_Directories[i].buffer(), pDirectory) == 0)
			return false;
	}
	
	m_Directories.emplace_back(pDirectory);
	return true;
}

int CAssetsManager::Load_AssetsFile_Core(const char* pPackageName, CErrorStack* pErrorStack)
{
	// The input filename can be one of the following format:
	// * packagename
	// * packagename.tup
	// * some/path/packagename
	// * some/path/packagename.tup
	//
	// The system must find those packages in the list of accepted directories and add the extension if needed
	
	dynamic_string Filename;
	Filename.clear();
	int CharPos = 0;
	CharPos = Filename.append_at(CharPos, pPackageName);
	
	//Add the extension if needed
	int FilenameLength = Filename.length();
	if(FilenameLength < 4 || str_comp(Filename.buffer()+FilenameLength-4, ".tup") != 0)
		CharPos = Filename.append_at(CharPos, ".tup");
	
	//Check if the path is absolute
	bool IsPath = false;
	
	for(char* pChar = Filename.buffer(); *pChar != 0; pChar++)
	{
		if(*pChar == '/' || *pChar == '\\')
		{
			IsPath = true;
			break;
		}
	}
	
	//Find the name of the package
	dynamic_string PurePackageName;
	FilenameLength = Filename.length();
	int PackageNamePos = -1;
	for(int i=FilenameLength-1; i>=0; i--)
	{
		if(Filename.buffer()[i] == '/' || Filename.buffer()[i] == '\\')
		{
			PackageNamePos = i+1;
			break;
		}
	}
	if(PackageNamePos >= 0)
		PurePackageName = Filename.buffer() + PackageNamePos;
	else
		PurePackageName = Filename.buffer();
	PurePackageName.buffer()[PurePackageName.length()-4] = 0; //Remove ".tup"
	
	//Check if the package is already opened
	for(unsigned int i=0; i<m_pPackages.size(); i++)
	{
		if(m_pPackages[i] && str_comp(m_pPackages[i]->GetName(), PurePackageName.buffer()) == 0)
			return i;
	}
	
	//Find the absolute path
	dynamic_string FullPath;
	if(IsPath)
		FullPath = Filename;
	else
	{
		dynamic_string Buffer;
		bool Found = false;
		for(int i = (int) m_Directories.size()-1; i>=0; i--)
		{
			Buffer.clear();
			CharPos = 0;
			CharPos = Buffer.append_at(CharPos, m_Directories[i].buffer());
			CharPos = Buffer.append_at(CharPos, "/");
			CharPos = Buffer.append_at(CharPos, Filename.buffer());
			
			IOHANDLE Handle = io_open(Buffer.buffer(), IOFLAG_READ);
			if(Handle)
			{
				io_close(Handle);
				FullPath = Buffer.buffer();
				Found = true;
				break;
			}
		}
		
		if(!Found)
		{
			if(pErrorStack)
			{
				CLocalizableString LString(_("Can't open package from this file: {str:Filename}"));
				LString.AddString("Filename", Filename.buffer());
				pErrorStack->AddError(LString);
			}
			else
				debug::WarningStream("AssetsManager") << "Can't open a package with this path: " << FullPath.buffer() << std::endl;
			
			return -1;
		}
	}
	
	//Find package directory
	dynamic_string Directory;
	Directory.append(FullPath.buffer(), FullPath.length() - 4 - PurePackageName.length()); 
	char* pCharSlash = Directory.buffer() + Directory.length() - 1;
	while(pCharSlash > Directory.buffer())
	{
		if(*pCharSlash == '/' || *pCharSlash == '\\')
		{
			*pCharSlash = 0;
			pCharSlash--;
		}
		else
			break;
	}
	
	int PackageId = -1;
	CAssetsPackage* pPackage = NULL;
	
	CArchiveFile ArchiveFile;
	if(!ArchiveFile.Open(Storage(), FullPath.buffer(), CStorage::TYPE_ABSOLUTE))
	{
		if(pErrorStack)
		{
			CLocalizableString LString(_("Can't open the package {str:Filename}"));
			LString.AddString("Filename", FullPath.buffer());
			pErrorStack->AddError(LString);
		}
		debug::WarningStream("AssetsManager") << "can't open the file " << FullPath.buffer() << std::endl;
		return PackageId;
	}
	
	for(unsigned int i=0; i<m_pPackages.size(); i++)
	{
		if(m_pPackages[i] && str_comp(m_pPackages[i]->GetName(), PurePackageName.buffer()) == 0)
		{
			pPackage = m_pPackages[i].get();
			PackageId = i;
			
			if(pPackage->IsJustLoaded())
				return PackageId;
		}
	}
	
	if(!pPackage)
	{
		PackageId = NewPackage(NULL);
		pPackage = m_pPackages[PackageId].get();
		pPackage->SetName(PurePackageName.buffer());
		pPackage->SetDirectory(Directory.buffer());
	}
	
	AddDirectory(Directory.buffer());
	
	CAssetsSaveLoadContext SaveLoadContext(this, &ArchiveFile, PackageId);
	
	int NumDependencies = ArchiveFile.GetNumItems(1);
	for(int i = 0; i < NumDependencies; i++)
	{
		CAssetsPackage::CTuaType_Dependency* pItem = (CAssetsPackage::CTuaType_Dependency*) ArchiveFile.GetItem(1, i);
		const char* pPackageName = ArchiveFile.GetString(pItem->m_PackageName);
		if(pPackageName != NULL)
		{
			SaveLoadContext.AddDependency(Load_AssetsFile_Core(pPackageName, pErrorStack));
		}
	}
	
	if(!pPackage->IsLoaded())
		pPackage->Load_AssetsFile(&SaveLoadContext);
	
	for(int i=0; i<GetNumAssets<CAsset_Image>(PackageId); i++)
		RequestUpdate(CAssetPath(CAsset_Image::TypeId, PackageId, i));
	
	pPackage->SetEdited(false);
	
	if(m_pHistory)
		m_pHistory->Flush();
	
	return PackageId;
}

int CAssetsManager::Load_AssetsFile(const char* pFileName, CErrorStack* pErrorStack)
{	
	//Load all dependencies
	int PackageId = Load_AssetsFile_Core(pFileName, pErrorStack);
	
	//Then resolve assets name and finalize
	for(unsigned int i=0; i<m_pPackages.size(); i++)
	{
		if(m_pPackages[i] && m_pPackages[i]->IsJustLoaded())
		{
			m_pPackages[i]->AssetPathOperation(CAssetPath::COperation::ResolveName(this, &m_pNamesToResolved));
			m_pPackages[i]->LoadingDone();
		}
	}
	
	m_pNamesToResolved.clear();
	
	return PackageId;
}

void CAssetsManager::Load_EnvClouds()
{
	if(m_PackageId_EnvClouds < 0)
	{
		m_PackageId_EnvClouds = Load_AssetsFile("env_clouds");
		if(m_PackageId_EnvClouds >= 0)
		{
			m_Path_Image_Cloud1 = FindAsset<CAsset_Image>(m_PackageId_EnvClouds, "cloud1");
			m_Path_Image_Cloud2 = FindAsset<CAsset_Image>(m_PackageId_EnvClouds, "cloud2");
			m_Path_Image_Cloud3 = FindAsset<CAsset_Image>(m_PackageId_EnvClouds, "cloud3");
		}
	}
}

void CAssetsManager::Load_EnvDesert()
{
	if(m_PackageId_EnvDesert < 0)
	{
		m_PackageId_EnvDesert = Load_AssetsFile("env_desert");
		if(m_PackageId_EnvDesert >= 0)
		{
			m_Path_Image_DesertMain = FindAsset<CAsset_Image>(m_PackageId_EnvDesert, "desertMain");
			m_Path_Image_DesertDoodads = FindAsset<CAsset_Image>(m_PackageId_EnvDesert, "desertDoodads");
			m_Path_Image_DesertMountains1 = FindAsset<CAsset_Image>(m_PackageId_EnvDesert, "desertMountains1");
			m_Path_Image_DesertMountains2 = FindAsset<CAsset_Image>(m_PackageId_EnvDesert, "desertMountains2");
			m_Path_Image_DesertSun = FindAsset<CAsset_Image>(m_PackageId_EnvDesert, "desertSun");
		}
	}
}

void CAssetsManager::Load_EnvGeneric()
{
	if(m_PackageId_EnvGeneric < 0)
	{
		m_PackageId_EnvGeneric = Load_AssetsFile("env_generic");
		if(m_PackageId_EnvGeneric >= 0)
		{
			m_Path_Image_GenericSpikes = FindAsset<CAsset_Image>(m_PackageId_EnvGeneric, "genericSpikes");
			m_Path_Image_GenericUnhookable = FindAsset<CAsset_Image>(m_PackageId_EnvGeneric, "genericUnhookable");
			m_Path_Mat_Unhookable = FindAsset<CAsset_TilingMaterial>(m_PackageId_EnvGeneric, "smallBlueUnhookable");
		}
	}
}

void CAssetsManager::Load_EnvGrass()
{
	if(m_PackageId_EnvGrass < 0)
	{
		m_PackageId_EnvGrass = Load_AssetsFile("env_grass");
		if(m_PackageId_EnvGrass >= 0)
		{
			m_Path_Image_GrassMain = FindAsset<CAsset_Image>(m_PackageId_EnvGrass, "grassMain");
			m_Path_Image_GrassDoodads = FindAsset<CAsset_Image>(m_PackageId_EnvGrass, "grassDoodads");
			m_Path_Mat_GrassAndDirt = FindAsset<CAsset_TilingMaterial>(m_PackageId_EnvGrass, "grassAndDirt");
			m_Path_Mat_Cave = FindAsset<CAsset_TilingMaterial>(m_PackageId_EnvGrass, "cave");
		}
	}
}

void CAssetsManager::Load_EnvJungle()
{
	if(m_PackageId_EnvJungle < 0)
	{
		m_PackageId_EnvJungle = Load_AssetsFile("env_jungle");
		if(m_PackageId_EnvJungle >= 0)
		{
			m_Path_Image_JungleMain = FindAsset<CAsset_Image>(m_PackageId_EnvJungle, "jungleMain");
			m_Path_Image_JungleDoodads = FindAsset<CAsset_Image>(m_PackageId_EnvJungle, "jungleDoodads");
			m_Path_Image_JungleSpikes = FindAsset<CAsset_Image>(m_PackageId_EnvJungle, "jungleSpikes");
			m_Path_Image_JungleUnhookable = FindAsset<CAsset_Image>(m_PackageId_EnvJungle, "jungleUnhookable");
			m_Path_Image_JungleBackground = FindAsset<CAsset_Image>(m_PackageId_EnvJungle, "jungleBackground");
			m_Path_Image_JungleMidground = FindAsset<CAsset_Image>(m_PackageId_EnvJungle, "jungleMidground");
		}
	}
}

void CAssetsManager::Load_EnvMoon()
{
	if(m_PackageId_EnvMoon < 0)
	{
		m_PackageId_EnvMoon = Load_AssetsFile("env_moon");
		if(m_PackageId_EnvMoon >= 0)
		{
			m_Path_Image_Moon = FindAsset<CAsset_Image>(m_PackageId_EnvMoon, "moon");
		}
	}
}

void CAssetsManager::Load_EnvMountains()
{
	if(m_PackageId_EnvMountains < 0)
	{
		m_PackageId_EnvMountains = Load_AssetsFile("env_mountains");
		if(m_PackageId_EnvMountains >= 0)
		{
			m_Path_Image_Mountains = FindAsset<CAsset_Image>(m_PackageId_EnvMountains, "mountains");
		}
	}
}

void CAssetsManager::Load_EnvSnow()
{
	if(m_PackageId_EnvSnow < 0)
	{
		m_PackageId_EnvSnow = Load_AssetsFile("env_snow");
		if(m_PackageId_EnvSnow >= 0)
		{
			m_Path_Image_Snow = FindAsset<CAsset_Image>(m_PackageId_EnvSnow, "snow");
		}
	}
}

void CAssetsManager::Load_EnvStars()
{
	if(m_PackageId_EnvStars < 0)
	{
		m_PackageId_EnvStars = Load_AssetsFile("env_stars");
		if(m_PackageId_EnvStars >= 0)
		{
			m_Path_Image_Stars = FindAsset<CAsset_Image>(m_PackageId_EnvStars, "stars");
		}
	}
}

void CAssetsManager::Load_EnvSun()
{
	if(m_PackageId_EnvSun < 0)
	{
		m_PackageId_EnvSun = Load_AssetsFile("env_sun");
		if(m_PackageId_EnvSun >= 0)
		{
			m_Path_Image_Sun = FindAsset<CAsset_Image>(m_PackageId_EnvSun, "sun");
		}
	}
}

void CAssetsManager::Load_EnvWinter()
{
	if(m_PackageId_EnvWinter < 0)
	{
		m_PackageId_EnvWinter = Load_AssetsFile("env_winter");
		if(m_PackageId_EnvWinter >= 0)
		{
			m_Path_Image_WinterMain = FindAsset<CAsset_Image>(m_PackageId_EnvWinter, "winterMain");
			m_Path_Image_WinterDoodads = FindAsset<CAsset_Image>(m_PackageId_EnvWinter, "winterDoodads");
			m_Path_Image_WinterMountains1 = FindAsset<CAsset_Image>(m_PackageId_EnvWinter, "winterMountains1");
			m_Path_Image_WinterMountains2 = FindAsset<CAsset_Image>(m_PackageId_EnvWinter, "winterMountains2");
			m_Path_Image_WinterMountains3 = FindAsset<CAsset_Image>(m_PackageId_EnvWinter, "winterMountains3");
		}
	}
}

void CAssetsManager::Load_EnvLab()
{
	if(m_PackageId_EnvLab < 0)
	{
		m_PackageId_EnvLab = Load_AssetsFile("env_lab");
		if(m_PackageId_EnvLab >= 0)
		{
			m_Path_Image_LabMisc = FindAsset<CAsset_Image>(m_PackageId_EnvLab, "labMisc");
			m_Path_Image_LabMain = FindAsset<CAsset_Image>(m_PackageId_EnvLab, "labMain");
			m_Path_Image_LabBackground = FindAsset<CAsset_Image>(m_PackageId_EnvLab, "labBackground");
		}
	}
}

void CAssetsManager::Load_EnvFactory()
{
	if(m_PackageId_EnvFactory < 0)
	{
		m_PackageId_EnvFactory = Load_AssetsFile("env_factory");
		if(m_PackageId_EnvFactory >= 0)
		{
			m_Path_Image_FactoryMain = FindAsset<CAsset_Image>(m_PackageId_EnvFactory, "factoryMain");
			m_Path_Image_FactoryBackground = FindAsset<CAsset_Image>(m_PackageId_EnvFactory, "factoryBackground");
		}
	}
}

void CAssetsManager::Load_UnivTeeWorlds()
{
	if(m_PackageId_UnivTeeWorlds < 0)
	{
		m_PackageId_UnivTeeWorlds = Load_AssetsFile("teeworlds");
		if(m_PackageId_UnivTeeWorlds >= 0)
		{
			m_Path_ZoneType_TeeWorlds = FindAsset<CAsset_ZoneType>(m_PackageId_UnivTeeWorlds, "teeworlds");
			
			m_Path_EntityType_TWSpawn = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "spawn");
			m_Path_EntityType_TWSpawnRed = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "redSpawn");
			m_Path_EntityType_TWSpawnBlue = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "blueSpawn");
			m_Path_EntityType_TWGrenade = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "grenade");
			m_Path_EntityType_TWLaserRifle = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "rifle");
			m_Path_EntityType_TWShotgun = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "shotgun");
			m_Path_EntityType_TWNinja = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "ninja");
			m_Path_EntityType_TWHeart = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "health");
			m_Path_EntityType_TWArmor = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "armor");
			m_Path_EntityType_TWFlagBlue = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "blueFlag");
			m_Path_EntityType_TWFlagRed = FindAsset<CAsset_EntityType>(m_PackageId_UnivTeeWorlds, "redFlag");
		}
	}
}

void CAssetsManager::Load_UnivDDNet()
{
	if(m_PackageId_UnivDDNet < 0)
	{
		m_PackageId_UnivDDNet = Load_AssetsFile("ddnet");
		if(m_PackageId_UnivDDNet >= 0)
		{
			m_Path_ZoneType_DDGame = FindAsset<CAsset_ZoneType>(m_PackageId_UnivDDNet, "ddGame");
			m_Path_ZoneType_DDFront = FindAsset<CAsset_ZoneType>(m_PackageId_UnivDDNet, "ddFront");
			m_Path_ZoneType_DDTele = FindAsset<CAsset_ZoneType>(m_PackageId_UnivDDNet, "ddTele");
			m_Path_ZoneType_DDSwitch = FindAsset<CAsset_ZoneType>(m_PackageId_UnivDDNet, "ddSwitch");
			m_Path_ZoneType_DDTune = FindAsset<CAsset_ZoneType>(m_PackageId_UnivDDNet, "ddTune");
		}
	}
}

void CAssetsManager::Load_UnivOpenFNG()
{
	if(m_PackageId_UnivOpenFNG < 0)
	{
		m_PackageId_UnivOpenFNG = Load_AssetsFile("openfng");
		if(m_PackageId_UnivOpenFNG >= 0)
		{
			m_Path_ZoneType_OpenFNG = FindAsset<CAsset_ZoneType>(m_PackageId_UnivOpenFNG, "openfng");
		}
	}
}

void CAssetsManager::Load_UnivNinslash()
{
	if(m_PackageId_UnivNinslash < 0)
	{
		m_PackageId_UnivNinslash = Load_AssetsFile("ninslash");
		if(m_PackageId_UnivNinslash >= 0)
		{
			m_Path_ZoneType_Ninslash = FindAsset<CAsset_ZoneType>(m_PackageId_UnivNinslash, "ninslash");
			
			m_Path_EntityType_NSSpawn = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsSpawn");
			m_Path_EntityType_NSSpawnRed = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsRedSpawn");
			m_Path_EntityType_NSSpawnBlue = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsBlueSpawn");
			m_Path_EntityType_NSGrenade = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsGrenade");
			m_Path_EntityType_NSRifle = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsRifle");
			m_Path_EntityType_NSLaser = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsLaser");
			m_Path_EntityType_NSShotgun = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsShotgun");
			m_Path_EntityType_NSChainsaw = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsChainsaw");
			m_Path_EntityType_NSElectric = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsElectric");
			m_Path_EntityType_NSFlamer = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsFlamer");
			m_Path_EntityType_NSHeart = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsHealth");
			m_Path_EntityType_NSFlagBlue = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsBlueFlag");
			m_Path_EntityType_NSFlagRed = FindAsset<CAsset_EntityType>(m_PackageId_UnivNinslash, "nsRedFlag");
		}
	}
}

void CAssetsManager::Load_UnivSport()
{
	if(m_PackageId_UnivSport < 0)
	{
		m_PackageId_UnivSport = Load_AssetsFile("sport");
		if(m_PackageId_UnivSport >= 0)
		{
			m_Path_ZoneType_Sport = FindAsset<CAsset_ZoneType>(m_PackageId_UnivSport, "sport");
			m_Path_EntityType_SportBall = FindAsset<CAsset_EntityType>(m_PackageId_UnivSport, "spBall");
			m_Path_EntityType_SportRedBall = FindAsset<CAsset_EntityType>(m_PackageId_UnivSport, "spRedBall");
			m_Path_EntityType_SportBlueBall = FindAsset<CAsset_EntityType>(m_PackageId_UnivSport, "spBlueBall");
		}
	}
}

int CAssetsManager::AddSubItem_Hard(CAssetPath AssetPath, CSubPath SubPath, int Type)
{
	if(!IsValidPackage(AssetPath.GetPackageId()))
		return -1;
	
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
	{\
		CAsset_##Name* pAsset = GetAsset_Hard<CAsset_##Name>(AssetPath);\
		if(pAsset)\
		{\
			m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);\
			return pAsset->AddSubItem(Type, SubPath);\
		}\
		else\
			return -1;\
	}
	
	switch(AssetPath.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	#undef MACRO_ASSETTYPE
	
	return -1;
}

int CAssetsManager::AddSubItem(CAssetPath AssetPath, CSubPath SubPath, int Type, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return -1;
		
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
		
	return AddSubItem_Hard(AssetPath, SubPath, Type);
}

int CAssetsManager::AddSubItemAt(CAssetPath AssetPath, CSubPath SubPath, int Type, int Index, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return -1;
		
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
	
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
	{\
		CAsset_##Name* pAsset = GetAsset_Hard<CAsset_##Name>(AssetPath);\
		if(pAsset)\
		{\
			m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);\
			return pAsset->AddSubItemAt(Type, SubPath, Index);\
		}\
		else\
			return -1;\
	}
	
	switch(AssetPath.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	#undef MACRO_ASSETTYPE
	
	return -1;
}

bool CAssetsManager::DeleteSubItem(CAssetPath AssetPath, CSubPath SubPath, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return false;
		
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
	
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
	{\
		CAsset_##Name* pAsset = GetAsset_Hard<CAsset_##Name>(AssetPath);\
		if(pAsset)\
		{\
			m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);\
			pAsset->DeleteSubItem(SubPath);\
			return true;\
		}\
		return false;\
	}
	
	switch(AssetPath.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	#undef MACRO_ASSETTYPE
	
	return false;
}

bool CAssetsManager::RelMoveSubItem(CAssetPath AssetPath, CSubPath& SubPath, int RelMove, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return false;
		
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
	
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
	{\
		CAsset_##Name* pAsset = GetAsset_Hard<CAsset_##Name>(AssetPath);\
		if(pAsset)\
		{\
			m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);\
			pAsset->RelMoveSubItem(SubPath, RelMove);\
			return true;\
		}\
		return false;\
	}
	
	switch(AssetPath.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	#undef MACRO_ASSETTYPE
	
	return false;
}
	
CAssetPath CAssetsManager::DuplicateAsset(const CAssetPath& Path, int PackageId, int Token)
{
	if(!IsValidPackage(PackageId) || IsReadOnlyPackage(PackageId))
		return CAssetPath::Null();
	
	CAssetPath NewAssetPath;
	char aBuf[128];
	dynamic_string Buffer;
	
	#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
	{\
		if(!GetAsset<CAsset_##Name>(Path))\
			return CAssetPath::Null();\
		CAsset_##Name* pNewAsset = NewAsset<CAsset_##Name>(&NewAssetPath, PackageId, Token);\
		if(!pNewAsset)\
			return CAssetPath::Null();\
		const CAsset_##Name* pOldAsset = GetAsset<CAsset_##Name>(Path);\
		*pNewAsset = *pOldAsset;\
		\
		int DuplicateNum = ((PackageId == Path.GetPackageId()) ? 1 : 0);\
		const char* pName = pOldAsset->GetName();\
		const char* pCharIter = pName + str_length(pName) - 1;\
		while(pCharIter > pName && *pCharIter >= '0' && *pCharIter <= '9')\
		{\
			pCharIter--;\
		}\
		if(pCharIter >= pName)\
		{\
			Buffer.append(pName, pCharIter - pName + 1);\
			DuplicateNum = str_to_int(pCharIter + 1);\
		}\
		else\
			Buffer = pName;\
		bool NameFound;\
		do\
		{\
			NameFound = false;\
			DuplicateNum++;\
			if(DuplicateNum < 2)\
				str_copy(aBuf, Buffer.buffer(), sizeof(aBuf));\
			else\
				str_format(aBuf, sizeof(aBuf), "%s%d", Buffer.buffer(), DuplicateNum);\
			for(int i=0; i<GetNumAssets<CAsset_##Name>(PackageId); i++)\
			{\
				const CAsset_##Name* pTestedAsset = GetAsset<CAsset_##Name>(CAssetPath(Path.GetType(), PackageId, i));\
				if(str_comp(pTestedAsset->GetName(), aBuf) == 0)\
				{\
					NameFound = true;\
					break;\
				}\
			}\
		}\
		while(NameFound);\
		pNewAsset->SetName(aBuf);\
		m_pPackages[PackageId]->SetEdited(true);\
		break;\
	}
	
	switch(Path.GetType())
	{
		#include <generated/assets/assetsmacro.h>
	}
	
	#undef MACRO_ASSETTYPE
	
	return NewAssetPath;
}

void CAssetsManager::DeleteAsset_Hard(const CAssetPath& Path)
{
	if(IsValidPackage(Path.GetPackageId()))
	{
		m_pPackages[Path.GetPackageId()]->DeleteAsset(Path);

		CAssetPath::COperation Operation = CAssetPath::COperation::DeleteAsset(Path);
		
		for(unsigned int i=0; i<m_pPackages.size(); i++)
		{
			if(m_pPackages[i])
				m_pPackages[i]->AssetPathOperation(Operation);
		}
	}
}

void CAssetsManager::DeleteAsset(const CAssetPath& Path, int Token)
{
	DeleteAsset_Hard(Path);
	if(m_pHistory)
		m_pHistory->Flush();
}

void CAssetsManager::DeleteAssets(std::vector<CAssetPath>& Pathes, int Token)
{
	for(unsigned int a=0; a<Pathes.size(); a++)
	{
		if(IsValidPackage(Pathes[a].GetPackageId()))
		{
			m_pPackages[Pathes[a].GetPackageId()]->DeleteAsset(Pathes[a]);

			CAssetPath::COperation Operation = CAssetPath::COperation::DeleteAsset(Pathes[a]);
				
			for(unsigned int i=0; i<m_pPackages.size(); i++)
			{
				if(m_pPackages[i])
					m_pPackages[i]->AssetPathOperation(Operation);
			}
			
			for(unsigned int b=a+1; b<Pathes.size(); b++)
				Operation.Apply(Pathes[b]);
		}
	}
	if(m_pHistory)
		m_pHistory->Flush();
}
	
void CAssetsManager::MapLayerTiles_HFlip(const CAssetPath& AssetPath, int Token)
{
	const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetPath);
	if(!pLayer)
		return;
	
	array2d<CAsset_MapLayerTiles::CTile> Tiles;
	Tiles = pLayer->GetTileArray();
	
	for(int j=0; j<Tiles.get_height(); j++)
	{
		for(int i=0; i<Tiles.get_width(); i++)
		{
			const CAsset_MapLayerTiles::CTile& Tile = Tiles.get_clamp(i, j);
			int Flags = Tile.GetFlags() ^ CAsset_MapLayerTiles::TILEFLAG_HFLIP;
			
			CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(i, Tiles.get_height()-j-1);
			AssetsManager()->SetAssetValue<int>(AssetPath, TilePath, CAsset_MapLayerTiles::TILE_INDEX, Tile.GetIndex(), Token);
			AssetsManager()->SetAssetValue<int>(AssetPath, TilePath, CAsset_MapLayerTiles::TILE_FLAGS, Flags, Token);
		}
	}
}

void CAssetsManager::MapLayerTiles_VFlip(const CAssetPath& AssetPath, int Token)
{
	const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetPath);
	if(!pLayer)
		return;
	
	array2d<CAsset_MapLayerTiles::CTile> Tiles;
	Tiles = pLayer->GetTileArray();
	
	for(int j=0; j<Tiles.get_height(); j++)
	{
		for(int i=0; i<Tiles.get_width(); i++)
		{
			const CAsset_MapLayerTiles::CTile& Tile = Tiles.get_clamp(i, j);
			int Flags = Tile.GetFlags() ^ CAsset_MapLayerTiles::TILEFLAG_VFLIP;
			
			CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(Tiles.get_width()-i-1, j);
			AssetsManager()->SetAssetValue<int>(AssetPath, TilePath, CAsset_MapLayerTiles::TILE_INDEX, Tile.GetIndex(), Token);
			AssetsManager()->SetAssetValue<int>(AssetPath, TilePath, CAsset_MapLayerTiles::TILE_FLAGS, Flags, Token);
		}
	}
}

CAssetState* CAssetsManager::GetAssetState(CAssetPath Path)
{
	if(IsValidPackage(Path.GetPackageId()))
	{
		#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
			return m_pPackages[Path.GetPackageId()]->GetAssetState<CAsset_##Name>(Path);

		switch(Path.GetType())
		{
			#include <generated/assets/assetsmacro.h>
		}
		
		#undef MACRO_ASSETTYPE
	}
	return 0;
}

void CAssetsManager::InitAssetState(int PackageId, const CAssetState& State)
{
	if(PackageId >= 0 && PackageId < static_cast<int>(m_pPackages.size()) && m_pPackages[PackageId])
		m_pPackages[PackageId]->InitAssetState(State);
}

bool CAssetsManager::SaveAssetInHistory(CAssetPath AssetPath, int Token)
{
	if(!IsValidPackage(AssetPath.GetPackageId()) || IsReadOnlyPackage(AssetPath.GetPackageId()))
		return false;
	
	if(m_pHistory)
		m_pHistory->AddOperation_EditAsset(AssetPath, Token);
	
	m_pPackages[AssetPath.GetPackageId()]->SetEdited(true);
	
	return true;
}

void CAssetsManager::EnableAssetsHistory()
{
	m_pHistory.reset(new CAssetsHistory(this));
}

void CAssetsManager::Undo()
{
	if(m_pHistory)
		m_pHistory->Undo();
}

int CAssetsManager::GenerateToken()
{
	if(m_pHistory)
		return m_pHistory->GenerateToken();
	else
		return CAssetsHistory::NO_TOKEN;
}

int CAssetsManager::GetHistorySize()
{
	if(m_pHistory)
		return m_pHistory->GetHistorySize();
	else
		return 0;
}
