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

#include <game/components/countryflags.h>
#include <shared/components/assetsmanager.h>
#include <shared/components/cli.h>
#include <client/loading_tools.h>

#include <external/json-parser/json.h>
#include <shared/components/storage.h>

/* COUNTRYFLAGS *******************************************************/

CCountryFlags::CCountryFlags(CGameKernel* pKernel) :
	CGameKernel::CComponent(pKernel),
	m_DefaultFlagIndex(0)
{
	SetName("CountryFlags");
}

bool CCountryFlags::InitConfig(int argc, const char** argv)
{
	m_Cfg_FlgLoad = 1;
	
	CLI()->RegisterConfigInteger("flg_load", "Load and show country flags", &m_Cfg_FlgLoad, 0, 1);
	
	return true;
}

void CCountryFlags::SaveConfig(CCLI_Output* pOutput)
{
	fixed_string128 Buffer;
	str_format(Buffer.buffer(), Buffer.maxsize(), "flg_load %d", m_Cfg_FlgLoad);
	pOutput->Print(Buffer.buffer());
}

bool CCountryFlags::Init()
{
	m_Flags.clear();
	for(int i=0; i<LUT_SIZE; i++)
	{
		m_FlagLut[i].clear();
	}
	
	// read file data into buffer
	const char *pFilename = "countryflags/index.json";
	IOHANDLE File = Storage()->OpenFile(pFilename, IOFLAG_READ, CStorage::TYPE_ALL);
	if(!File)
	{
		dbg_msg("CountryFlags", "couldn't open countryflags index file");
		return false;
	}
	
	int FileSize = (int)io_length(File);
	char *pFileData = new char[FileSize+1];
	io_read(File, pFileData, FileSize);
	pFileData[FileSize] = 0;
	io_close(File);

	// parse json data
	json_settings JsonSettings;
	mem_zero(&JsonSettings, sizeof(JsonSettings));
	char aError[256];
	json_value *pJsonData = json_parse_ex(&JsonSettings, pFileData, FileSize, aError);
	if(pJsonData == 0)
	{
		dbg_msg("CountryFlags", "country index file error: %s", aError);
		delete[] pFileData;
		return false;
	}
	
	m_Flags.clear();
	
	int PackageId = -1;
	if(m_Cfg_FlgLoad)
	{
		PackageId = AssetsManager()->NewPackage("countryflags");
	}

	// extract data
	const json_value &rInit = (*pJsonData)["country codes"];
	if(rInit.type == json_object)
	{
		const char* paIndices[3] = {"teeuniverses", "ISO 3166-1", "ISO 3166-2"};
		for(int Index = 0; Index < sizeof(paIndices)/sizeof(const char*); ++Index)
		{
			const json_value &rStart = rInit[(const char*)paIndices[Index]];
			if(rStart.type == json_array)
			{
				for(unsigned i = 0; i < rStart.u.array.length; ++i)
				{
					char aBuf[64];

					int CountryCode = (long)rStart[i]["numeric"];
					
					// add entry
					CCountryFlag CountryFlag;
					CountryFlag.m_Code = CountryCode;
					
					const char* pCountryName = rStart[i]["alpha2"];
					str_copy(CountryFlag.m_aCodeName, pCountryName, sizeof(CountryFlag.m_aCodeName));

					const char* pEnglishName = rStart[i]["englishName"];
					if(pEnglishName)
						str_copy(CountryFlag.m_aEnglishName, pEnglishName, sizeof(CountryFlag.m_aEnglishName));
					else
						str_copy(CountryFlag.m_aEnglishName, pCountryName, sizeof(CountryFlag.m_aEnglishName));

					const char* pLocalName = rStart[i]["localName"];
					if(pEnglishName)
						str_copy(CountryFlag.m_aLocalName, pLocalName, sizeof(CountryFlag.m_aLocalName));
					else
						CountryFlag.m_aLocalName[0] = 0;
					
					CAssetPath SpritePath;
					
					if(PackageId >= 0)
					{
						str_format(aBuf, sizeof(aBuf), "countryflags/%s.png", pCountryName);
						
						CAssetPath ImagePath = CreateNewImage(SharedKernel(), PackageId, pCountryName, aBuf, 1, 1);
						if(!ImagePath.IsNull())
						{
							CAsset_Sprite* pSprite = AssetsManager()->NewAsset<CAsset_Sprite>(&SpritePath, PackageId, CAssetsHistory::NO_TOKEN);
							if(pSprite)
							{
								pSprite->SetName(aBuf);
								pSprite->SetImagePath(ImagePath);
								pSprite->SetX(0);
								pSprite->SetY(0);
								pSprite->SetWidth(1);
								pSprite->SetHeight(1);
							}
						}
					}		
					
					CountryFlag.m_SpritePath = SpritePath;
					m_Flags.add_unsorted(CountryFlag);
		
					// add alias for compatibility
					int Alias = (long)rStart[i]["numericAlias"];
					if(Alias > 0)
					{
						m_CodeAliases.add_by_copy(Alias);
					}
				}
			}
		}
	}

	// clean up
	json_value_free(pJsonData);
	delete[] pFileData;
	m_Flags.sort_range();
	
	for(int i=0; i<m_Flags.size(); i++)
	{
		if(m_Flags[i].m_Code == -1)
			m_DefaultFlagIndex = i;
		else
		{
			int LutIndex = m_Flags[i].m_Code % LUT_SIZE;
			m_FlagLut[LutIndex].add_by_copy(i);
			
				m_DefaultFlagIndex = i;
		}
	}
	
	return true;
}

int CCountryFlags::FindIndex(int Code) const
{
	if(Code >= 0)
	{
		int LutIndex = Code % LUT_SIZE;
		for(int i=0; i<m_FlagLut[LutIndex].size(); i++)
		{
			int FlagIndex = m_FlagLut[LutIndex][i];
			if(m_Flags[FlagIndex].m_Code == Code)
				return FlagIndex;
		}
	}
	return m_DefaultFlagIndex;
}
