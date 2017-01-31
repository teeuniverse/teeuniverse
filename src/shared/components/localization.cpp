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

#include "localization.h"

#include <json-parser/json.h>
#include <shared/components/storage.h>
#include <shared/components/cli.h>

/* LANGUAGE ***********************************************************/

CLocalization::CLanguage::CLanguage() :
	m_Loaded(false),
	m_Direction(CLocalization::DIRECTION_LTR),
	m_pPluralRules(NULL),
	m_pNumberFormater(NULL),
	m_pPercentFormater(NULL),
	m_pTimeUnitFormater(NULL)
{
	m_aName[0] = 0;
	m_aFilename[0] = 0;
	m_aParentFilename[0] = 0;
}

CLocalization::CLanguage::CLanguage(const char* pName, const char* pFilename, const char* pParentFilename) :
	m_Loaded(false),
	m_Direction(CLocalization::DIRECTION_LTR),
	m_pPluralRules(NULL),
	m_pNumberFormater(NULL),
	m_pPercentFormater(NULL)
{
	str_copy(m_aName, pName, sizeof(m_aName));
	str_copy(m_aFilename, pFilename, sizeof(m_aFilename));
	str_copy(m_aParentFilename, pParentFilename, sizeof(m_aParentFilename));
	
	UErrorCode Status;
	
	Status = U_ZERO_ERROR;
	m_pNumberFormater = unum_open(UNUM_DECIMAL, NULL, -1, m_aFilename, NULL, &Status);
	if(U_FAILURE(Status))
	{
		if(m_pNumberFormater)
		{
			unum_close(m_pNumberFormater);
			m_pNumberFormater = NULL;
		}
		dbg_msg("Localization", "Can't create number formater for %s (error #%d)", m_aFilename, Status);
	}
	
	Status = U_ZERO_ERROR;
	m_pPercentFormater = unum_open(UNUM_PERCENT, NULL, -1, m_aFilename, NULL, &Status);
	if(U_FAILURE(Status))
	{
		if(m_pPercentFormater)
		{
			unum_close(m_pPercentFormater);
			m_pPercentFormater = NULL;
		}
		dbg_msg("Localization", "Can't create percent formater for %s (error #%d)", m_aFilename, Status);
	}
	
	Status = U_ZERO_ERROR;
	m_pPluralRules = uplrules_openForType(m_aFilename, UPLURAL_TYPE_CARDINAL, &Status);
	if(U_FAILURE(Status))
	{
		if(m_pPluralRules)
		{
			uplrules_close(m_pPluralRules);
			m_pPluralRules = NULL;
		}
		dbg_msg("Localization", "Can't create plural rules for %s (error #%d)", m_aFilename, Status);
	}
	
	//Time unit for second formater
	Status = U_ZERO_ERROR;
	m_pTimeUnitFormater = new icu::TimeUnitFormat(m_aFilename,  UTMUTFMT_ABBREVIATED_STYLE, Status);
	if(U_FAILURE(Status))
	{
		dbg_msg("Localization", "Can't create timeunit formater %s (error #%d)", pFilename, Status);
		delete m_pTimeUnitFormater;
		m_pTimeUnitFormater = NULL;
	}
}

CLocalization::CLanguage::~CLanguage()
{
	hashtable< CEntry, 128 >::iterator Iter = m_Translations.begin();
	while(Iter != m_Translations.end())
	{
		if(Iter.data())
			Iter.data()->Free();
		
		++Iter;
	}
	
	if(m_pNumberFormater)
		unum_close(m_pNumberFormater);
	
	if(m_pPercentFormater)
		unum_close(m_pPercentFormater);
		
	if(m_pPluralRules)
		uplrules_close(m_pPluralRules);
		
	if(m_pTimeUnitFormater)
		delete m_pTimeUnitFormater;
}

bool CLocalization::CLanguage::Load(CStorage* pStorage)
{
	// read file data into buffer
	char aBuf[256];
	str_format(aBuf, sizeof(aBuf), "languages/%s.json", m_aFilename);
	
	IOHANDLE File = pStorage->OpenFile(aBuf, IOFLAG_READ, CStorage::TYPE_ALL);
	if(!File)
		return false;
	
	// load the file as a string
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
		dbg_msg("Localization", "Can't load the localization file %s : %s", aBuf, aError);
		delete[] pFileData;
		return false;
	}

	// extract data
	const json_value &rStart = (*pJsonData)["translation"];
	if(rStart.type == json_array)
	{
		for(unsigned i = 0; i < rStart.u.array.length; ++i)
		{
			const char* pKey = rStart[i]["key"];
			if(pKey && pKey[0])
			{
				CEntry* pEntry = m_Translations.set(pKey);
				
				const char* pSingular = rStart[i]["value"];
				if(pSingular && pSingular[0])
				{
					int Length = str_length(pSingular)+1;
					pEntry->m_apVersions[PLURALTYPE_NONE] = new char[Length];
					str_copy(pEntry->m_apVersions[PLURALTYPE_NONE], pSingular, Length);
				}
				else
				{
					const char* pPlural;
					
					//Zero
					pPlural = rStart[i]["zero"];
					if(pPlural && pPlural[PLURALTYPE_ZERO])
					{
						int Length = str_length(pPlural)+1;
						pEntry->m_apVersions[PLURALTYPE_ZERO] = new char[Length];
						str_copy(pEntry->m_apVersions[PLURALTYPE_ZERO], pPlural, Length);
					}
					//One
					pPlural = rStart[i]["one"];
					if(pPlural && pPlural[PLURALTYPE_ONE])
					{
						int Length = str_length(pPlural)+1;
						pEntry->m_apVersions[PLURALTYPE_ONE] = new char[Length];
						str_copy(pEntry->m_apVersions[PLURALTYPE_ONE], pPlural, Length);
					}
					//Two
					pPlural = rStart[i]["two"];
					if(pPlural && pPlural[PLURALTYPE_TWO])
					{
						int Length = str_length(pPlural)+1;
						pEntry->m_apVersions[PLURALTYPE_TWO] = new char[Length];
						str_copy(pEntry->m_apVersions[PLURALTYPE_TWO], pPlural, Length);
					}
					//Few
					pPlural = rStart[i]["few"];
					if(pPlural && pPlural[PLURALTYPE_FEW])
					{
						int Length = str_length(pPlural)+1;
						pEntry->m_apVersions[PLURALTYPE_FEW] = new char[Length];
						str_copy(pEntry->m_apVersions[PLURALTYPE_FEW], pPlural, Length);
					}
					//Many
					pPlural = rStart[i]["many"];
					if(pPlural && pPlural[PLURALTYPE_MANY])
					{
						int Length = str_length(pPlural)+1;
						pEntry->m_apVersions[PLURALTYPE_MANY] = new char[Length];
						str_copy(pEntry->m_apVersions[PLURALTYPE_MANY], pPlural, Length);
					}
					//Other
					pPlural = rStart[i]["other"];
					if(pPlural && pPlural[PLURALTYPE_OTHER])
					{
						int Length = str_length(pPlural)+1;
						pEntry->m_apVersions[PLURALTYPE_OTHER] = new char[Length];
						str_copy(pEntry->m_apVersions[PLURALTYPE_OTHER], pPlural, Length);
					}
				}
			}
		}
	}

	// clean up
	json_value_free(pJsonData);
	delete[] pFileData;
	
	m_Loaded = true;
	
	return true;
}

const char* CLocalization::CLanguage::Localize(const char* pText) const
{
	const CEntry* pEntry = m_Translations.get(pText);
	if(!pEntry)
		return NULL;
	
	return pEntry->m_apVersions[PLURALTYPE_NONE];
}

const char* CLocalization::CLanguage::Localize_P(int Number, const char* pText) const
{
	const CEntry* pEntry = m_Translations.get(pText);
	if(!pEntry)
		return NULL;
	
	UChar aPluralKeyWord[6];
	UErrorCode Status = U_ZERO_ERROR;
	uplrules_select(m_pPluralRules, static_cast<double>(Number), aPluralKeyWord, 6, &Status);
	
	if(U_FAILURE(Status))
		return NULL;
	
	int PluralCode = PLURALTYPE_NONE;
	
	if(aPluralKeyWord[0] == 0x007A) //z
		PluralCode = PLURALTYPE_ZERO;
	else if(aPluralKeyWord[0] == 0x0074) //t
		PluralCode = PLURALTYPE_TWO;
	else if(aPluralKeyWord[0] == 0x0066) //f
		PluralCode = PLURALTYPE_FEW;
	else if(aPluralKeyWord[0] == 0x006D) //m
		PluralCode = PLURALTYPE_MANY;
	else if(aPluralKeyWord[0] == 0x006F) //o
	{
		if(aPluralKeyWord[1] == 0x0074) //t
			PluralCode = PLURALTYPE_OTHER;
		else if(aPluralKeyWord[1] == 0x006E) //n
			PluralCode = PLURALTYPE_ONE;
	}
	
	return pEntry->m_apVersions[PluralCode];
}

/* LOCALIZATION *******************************************************/

CLocalization::CLocalization(CSharedKernel* pKernel) :
	CSharedKernel::CComponent(pKernel),
	m_pMainLanguage(NULL),
	m_pUtf8Converter(NULL)
{
	SetName("Localization");
}

CLocalization::~CLocalization()
{
	for(int i=0; i<m_pLanguages.size(); i++)
		delete m_pLanguages[i];
	
	if(m_pUtf8Converter)
		ucnv_close(m_pUtf8Converter);
}

bool CLocalization::InitConfig(int argc, const char** argv)
{
	m_Cfg_MainLanguage = "en";
	
	CLI()->RegisterConfigString("language", "What language to use", &m_Cfg_MainLanguage);
	
	return true;
}

void CLocalization::SaveConfig(CCLI_Output* pOutput)
{
	dynamic_string Buffer;
	str_format(Buffer.buffer(), Buffer.maxsize(), "language %s", m_Cfg_MainLanguage.buffer());
	pOutput->Print(Buffer.buffer());
}

bool CLocalization::Init()
{
	UErrorCode Status = U_ZERO_ERROR;
	m_pUtf8Converter = ucnv_open("utf8", &Status);
	if(U_FAILURE(Status))
	{
		dbg_msg("Localization", "Can't create UTF8/UTF16 convertert");
		return false;
	}
	
	// read file data into buffer
	const char *pFilename = "languages/index.json";
	IOHANDLE File = Storage()->OpenFile(pFilename, IOFLAG_READ, CStorage::TYPE_ALL);
	if(!File)
	{
		dbg_msg("Localization", "can't open languages/index.json");
		return true; //return true because it's not a critical error
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
	if(pJsonData == NULL)
	{
		delete[] pFileData;
		return true; //return true because it's not a critical error
	}

	// extract data
	m_pMainLanguage = NULL;
	const json_value& rStart = (*pJsonData)["language indices"];
	
	if(rStart.type == json_array)
	{
		for(unsigned i = 0; i < rStart.u.array.length; ++i)
		{
			CLanguage*& pLanguage = m_pLanguages.increment();
			pLanguage = new CLanguage((const char *)rStart[i]["name"], (const char *)rStart[i]["file"], (const char *)rStart[i]["parent"]);
				
			if((const char *)rStart[i]["direction"] && str_comp((const char *)rStart[i]["direction"], "rtl") == 0)
				pLanguage->SetWritingDirection(DIRECTION_RTL);
				
			if(m_Cfg_MainLanguage == pLanguage->GetFilename())
			{
				pLanguage->Load(Storage());
				
				m_pMainLanguage = pLanguage;
			}
		}
	}

	// clean up
	json_value_free(pJsonData);
	delete[] pFileData;
	
	return true;
}
	
void CLocalization::AddListener(IListener* pListener)
{
	m_pListeners.increment() = pListener;
}

void CLocalization::RemoveListener(IListener* pListener)
{
	for(int i=0; i<m_pListeners.size(); i++)
	{
		if(m_pListeners[i] == pListener)
			m_pListeners.remove_index(i);
	}
}

bool CLocalization::PreUpdate()
{
	if(!m_pMainLanguage || m_Cfg_MainLanguage != m_pMainLanguage->GetFilename())
	{
		CLanguage* pLanguage = 0;
		
		for(int i=0; i<m_pLanguages.size(); i++)
		{
			if(m_Cfg_MainLanguage == m_pLanguages[i]->GetFilename())
			{
				pLanguage = m_pLanguages[i];
				break;
			}
		}
		
		if(m_pMainLanguage != pLanguage)
		{
			m_pMainLanguage = pLanguage;
			
			for(int i=0; i<m_pListeners.size(); i++)
				m_pListeners[i]->OnLocalizationModified();
		}
	}
	
	return true;
}

const char* CLocalization::LocalizeWithDepth(const char* pLanguageCode, const char* pText, int Depth)
{
	CLanguage* pLanguage = m_pMainLanguage;
	if(pLanguageCode)
	{
		for(int i=0; i<m_pLanguages.size(); i++)
		{
			if(str_comp(m_pLanguages[i]->GetFilename(), pLanguageCode) == 0)
			{
				pLanguage = m_pLanguages[i];
				break;
			}
		}
	}
	
	if(!pLanguage)
		return pText;
	
	if(!pLanguage->IsLoaded())
		pLanguage->Load(Storage());
	
	const char* pResult = pLanguage->Localize(pText);
	if(pResult)
		return pResult;
	else if(pLanguage->GetParentFilename()[0] && Depth < 4)
		return LocalizeWithDepth(pLanguage->GetParentFilename(), pText, Depth+1);
	else
		return pText;
}

const char* CLocalization::Localize(const char* pLanguageCode, const char* pText)
{
	return LocalizeWithDepth(pLanguageCode, pText, 0);
}

const char* CLocalization::LocalizeWithDepth_P(const char* pLanguageCode, int Number, const char* pText, int Depth)
{
	CLanguage* pLanguage = m_pMainLanguage;
	if(pLanguageCode)
	{
		for(int i=0; i<m_pLanguages.size(); i++)
		{
			if(str_comp(m_pLanguages[i]->GetFilename(), pLanguageCode) == 0)
			{
				pLanguage = m_pLanguages[i];
				break;
			}
		}
	}
	
	if(!pLanguage)
		return pText;
	
	if(!pLanguage->IsLoaded())
		pLanguage->Load(Storage());
	
	const char* pResult = pLanguage->Localize_P(Number, pText);
	if(pResult)
		return pResult;
	else if(pLanguage->GetParentFilename()[0] && Depth < 4)
		return LocalizeWithDepth_P(pLanguage->GetParentFilename(), Number, pText, Depth+1);
	else
		return pText;
}

const char* CLocalization::Localize_P(const char* pLanguageCode, int Number, const char* pText)
{
	return LocalizeWithDepth_P(pLanguageCode, Number, pText, 0);
}

void CLocalization::AppendInteger(dynamic_string& Buffer, int& BufferIter, CLanguage* pLanguage, int Number)
{
	UChar aBufUtf16[128];
	
	UErrorCode Status = U_ZERO_ERROR;
	unum_format(pLanguage->m_pNumberFormater, Number, aBufUtf16, sizeof(aBufUtf16), NULL, &Status);
	if(U_FAILURE(Status))
		BufferIter = Buffer.append_at(BufferIter, "_NUMBER_");
	else
	{
		//Update buffer size
		int SrcLength = u_strlen(aBufUtf16);
		int NeededSize = UCNV_GET_MAX_BYTES_FOR_STRING(SrcLength, ucnv_getMaxCharSize(m_pUtf8Converter));
		
		while(Buffer.maxsize() - BufferIter <= NeededSize)
			Buffer.resize_buffer(Buffer.maxsize()*2);
		
		int Length = ucnv_fromUChars(m_pUtf8Converter, Buffer.buffer()+BufferIter, Buffer.maxsize() - BufferIter, aBufUtf16, SrcLength, &Status);
		if(U_FAILURE(Status))
			BufferIter = Buffer.append_at(BufferIter, "_NUMBER_");
		else
			BufferIter += Length;
	}
}

void CLocalization::AppendFloat(dynamic_string& Buffer, int& BufferIter, CLanguage* pLanguage, float Number)
{
	UChar aBufUtf16[128];
	
	UErrorCode Status = U_ZERO_ERROR;
	unum_formatDouble(pLanguage->m_pNumberFormater, Number, aBufUtf16, sizeof(aBufUtf16), NULL, &Status);
	if(U_FAILURE(Status))
		BufferIter = Buffer.append_at(BufferIter, "_NUMBER_");
	else
	{
		//Update buffer size
		int SrcLength = u_strlen(aBufUtf16);
		int NeededSize = UCNV_GET_MAX_BYTES_FOR_STRING(SrcLength, ucnv_getMaxCharSize(m_pUtf8Converter));
		
		while(Buffer.maxsize() - BufferIter <= NeededSize)
			Buffer.resize_buffer(Buffer.maxsize()*2);
		
		int Length = ucnv_fromUChars(m_pUtf8Converter, Buffer.buffer()+BufferIter, Buffer.maxsize() - BufferIter, aBufUtf16, SrcLength, &Status);
		if(U_FAILURE(Status))
			BufferIter = Buffer.append_at(BufferIter, "_NUMBER_");
		else
			BufferIter += Length;
	}
}

void CLocalization::AppendPercent(dynamic_string& Buffer, int& BufferIter, CLanguage* pLanguage, double Number)
{
	UChar aBufUtf16[128];
	
	UErrorCode Status = U_ZERO_ERROR;
	unum_formatDouble(pLanguage->m_pPercentFormater, Number, aBufUtf16, sizeof(aBufUtf16), NULL, &Status);
	if(U_FAILURE(Status))
		BufferIter = Buffer.append_at(BufferIter, "_PERCENT_");
	else
	{
		//Update buffer size
		int SrcLength = u_strlen(aBufUtf16);
		int NeededSize = UCNV_GET_MAX_BYTES_FOR_STRING(SrcLength, ucnv_getMaxCharSize(m_pUtf8Converter));
		
		while(Buffer.maxsize() - BufferIter <= NeededSize)
			Buffer.resize_buffer(Buffer.maxsize()*2);
		
		int Length = ucnv_fromUChars(m_pUtf8Converter, Buffer.buffer()+BufferIter, Buffer.maxsize() - BufferIter, aBufUtf16, SrcLength, &Status);
		if(U_FAILURE(Status))
			BufferIter = Buffer.append_at(BufferIter, "_PERCENT_");
		else
			BufferIter += Length;
	}
}

void CLocalization::AppendDuration(dynamic_string& Buffer, int& BufferIter, CLanguage* pLanguage, int Number, icu::TimeUnit::UTimeUnitFields Type)
{
	UErrorCode Status = U_ZERO_ERROR;
	icu::UnicodeString BufUTF16;
	
	icu::TimeUnitAmount* pAmount = new icu::TimeUnitAmount((double) Number, Type, Status);
	icu::Formattable Formattable;
	Formattable.adoptObject(pAmount);
	pLanguage->m_pTimeUnitFormater->format(Formattable, BufUTF16, Status);
	if(U_FAILURE(Status))
	{
		BufferIter = Buffer.append_at(BufferIter, "_DURATION_");
	}
	else
	{
		int SrcLength = BufUTF16.length();
		int NeededSize = UCNV_GET_MAX_BYTES_FOR_STRING(SrcLength, ucnv_getMaxCharSize(m_pUtf8Converter));
		
		while(Buffer.maxsize() - BufferIter <= NeededSize)
			Buffer.resize_buffer(Buffer.maxsize()*2);
		
		Status = U_ZERO_ERROR;
		int Length = ucnv_fromUChars(m_pUtf8Converter, Buffer.buffer()+BufferIter, Buffer.maxsize() - BufferIter, BufUTF16.getBuffer(), SrcLength, &Status);
		if(U_FAILURE(Status))
			BufferIter = Buffer.append_at(BufferIter, "_DURATION_");
		else
			BufferIter += Length;
	}
}

void CLocalization::Format(dynamic_string& Buffer, const char* pLanguageCode, const CLocalizableString& LString)
{
	const char* pText = Localize(pLanguageCode, LString.GetText());
	
	CLanguage* pLanguage = m_pMainLanguage;	
	if(pLanguageCode)
	{
		for(int i=0; i<m_pLanguages.size(); i++)
		{
			if(str_comp(m_pLanguages[i]->GetFilename(), pLanguageCode) == 0)
			{
				pLanguage = m_pLanguages[i];
				break;
			}
		}
	}
	if(!pLanguage)
	{
		Buffer.append(pText);
		return;
	}
	
	const array<CLocalizableString::CParameter>& Parameters = LString.GetParameters();
		
	int Iter = 0;
	int Start = Iter;
	int ParamTypeStart = -1;
	int ParamTypeLength = 0;
	int ParamNameStart = -1;
	int ParamNameLength = 0;
	
	int BufferIter = Buffer.length();
	
	while(pText[Iter])
	{
		if(ParamNameStart >= 0)
		{
			if(pText[Iter] == '}') //End of the macro, try to apply it
			{
				//Try to find an argument with this name
				for(int PIter=0; PIter<Parameters.size(); PIter++)
				{
					const char* pVarArgName = Parameters[PIter].m_Name.buffer();
					
					if(str_comp_num(pText+ParamNameStart, pVarArgName, ParamNameLength) == 0)
					{
						//Get argument type
						if(str_comp_num("str:", pText+ParamTypeStart, 4) == 0)
						{
							const char* pValue = Parameters[PIter].m_String.m_pValue;
							BufferIter = Buffer.append_at(BufferIter, pValue);
						}
						else if(str_comp_num("int:", pText+ParamTypeStart, 4) == 0)
						{
							AppendInteger(Buffer, BufferIter, pLanguage, Parameters[PIter].m_Integer.m_Value);
						}
						else if(str_comp_num("percent:", pText+ParamTypeStart, 8) == 0)
						{
							AppendPercent(Buffer, BufferIter, pLanguage, Parameters[PIter].m_Float.m_Value);
						}
						else if(str_comp_num("float:", pText+ParamTypeStart, 6) == 0)
						{
							AppendFloat(Buffer, BufferIter, pLanguage, Parameters[PIter].m_Float.m_Value);
						}
						else if(str_comp_num("sec:", pText+ParamTypeStart, 4) == 0)
						{
							int Duration = Parameters[PIter].m_Seconds.m_Value;
							int Minutes = Duration / 60;
							int Seconds = Duration - Minutes*60;
							if(Minutes > 0)
							{
								AppendDuration(Buffer, BufferIter, pLanguage, Minutes, icu::TimeUnit::UTIMEUNIT_MINUTE);
								if(Seconds > 0)
								{
									BufferIter = Buffer.append_at(BufferIter, ", ");
									AppendDuration(Buffer, BufferIter, pLanguage, Seconds, icu::TimeUnit::UTIMEUNIT_SECOND);
								}
							}
							else
								AppendDuration(Buffer, BufferIter, pLanguage, Seconds, icu::TimeUnit::UTIMEUNIT_SECOND);
						}
						break;
					}
				}
				
				//Close the macro
				Start = Iter+1;
				ParamTypeStart = -1;
				ParamNameStart = -1;
			}
			else
				ParamNameLength++;
		}
		else if(ParamTypeStart >= 0)
		{
			if(pText[Iter] == ':') //End of the type, start of the name
			{
				ParamNameStart = Iter+1;
				ParamNameLength = 0;
			}
			else if(pText[Iter] == '}') //Invalid: no name found
			{
				//Close the macro
				Start = Iter+1;
				ParamTypeStart = -1;
				ParamNameStart = -1;
			}
			else
				ParamTypeLength++;
		}
		else
		{
			if(pText[Iter] == '{')
			{
				//Flush the content of pText in the buffer
				BufferIter = Buffer.append_at_num(BufferIter, pText+Start, Iter-Start);
				Iter++;
				ParamTypeStart = Iter;
			}
		}
		
		Iter = str_utf8_forward(pText, Iter);
	}
	
	if(ParamTypeStart == -1 && ParamNameStart == -1)
	{
		BufferIter = Buffer.append_at_num(BufferIter, pText+Start, Iter-Start);
	}
}

void CLocalization::FormatInteger(dynamic_string& Buffer, const char* pLanguageCode, int Number)
{
	CLanguage* pLanguage = m_pMainLanguage;	
	if(pLanguageCode)
	{
		for(int i=0; i<m_pLanguages.size(); i++)
		{
			if(str_comp(m_pLanguages[i]->GetFilename(), pLanguageCode) == 0)
			{
				pLanguage = m_pLanguages[i];
				break;
			}
		}
	}
	if(!pLanguage)
		return;
	
	int BufferIter = Buffer.length();
	AppendInteger(Buffer, BufferIter, pLanguage, Number);
}

int CLocalization::ParseInteger(const char* pLanguageCode, const char* pText)
{
	CLanguage* pLanguage = m_pMainLanguage;	
	if(pLanguageCode)
	{
		for(int i=0; i<m_pLanguages.size(); i++)
		{
			if(str_comp(m_pLanguages[i]->GetFilename(), pLanguageCode) == 0)
			{
				pLanguage = m_pLanguages[i];
				break;
			}
		}
	}
	
	if(!pLanguage)
		return 0;
	
	UChar aBufUtf16[128];
	
	UErrorCode Status = U_ZERO_ERROR;
	int Length = ucnv_toUChars(m_pUtf8Converter, aBufUtf16, sizeof(aBufUtf16), pText, -1, &Status);
	
	return unum_parse(pLanguage->m_pNumberFormater, aBufUtf16, Length, NULL, &Status);
}

float CLocalization::ParseFloat(const char* pLanguageCode, const char* pText)
{
	CLanguage* pLanguage = m_pMainLanguage;	
	if(pLanguageCode)
	{
		for(int i=0; i<m_pLanguages.size(); i++)
		{
			if(str_comp(m_pLanguages[i]->GetFilename(), pLanguageCode) == 0)
			{
				pLanguage = m_pLanguages[i];
				break;
			}
		}
	}
	
	if(!pLanguage)
		return 0.0f;
	
	UChar aBufUtf16[128];
	
	UErrorCode Status = U_ZERO_ERROR;
	int Length = ucnv_toUChars(m_pUtf8Converter, aBufUtf16, sizeof(aBufUtf16), pText, -1, &Status);
	
	return unum_parseDouble(pLanguage->m_pNumberFormater, aBufUtf16, Length, NULL, &Status);
}

float CLocalization::ParsePercent(const char* pLanguageCode, const char* pText)
{
	CLanguage* pLanguage = m_pMainLanguage;	
	if(pLanguageCode)
	{
		for(int i=0; i<m_pLanguages.size(); i++)
		{
			if(str_comp(m_pLanguages[i]->GetFilename(), pLanguageCode) == 0)
			{
				pLanguage = m_pLanguages[i];
				break;
			}
		}
	}
	
	if(!pLanguage)
		return 0.0f;
	
	UChar aBufUtf16[128];
	
	UErrorCode Status = U_ZERO_ERROR;
	int Length = ucnv_toUChars(m_pUtf8Converter, aBufUtf16, sizeof(aBufUtf16), pText, -1, &Status);
	
	double Result = unum_parseDouble(pLanguage->m_pPercentFormater, aBufUtf16, Length, NULL, &Status);
	if(Status == U_PARSE_ERROR)
	{
		UErrorCode Status = U_ZERO_ERROR;
		return unum_parseDouble(pLanguage->m_pNumberFormater, aBufUtf16, Length, NULL, &Status)/100.0;
	}
	else
		return Result;
}
