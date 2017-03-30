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
#include <shared/system/debug.h>

#include <unicode/fmtable.h>

/* LANGUAGE ***********************************************************/

CLocalization::CLanguage::CLanguage(const char* pName, const char* pFilename, const char* pParentFilename) :
	m_Loaded(false),
	m_Direction(CLocalization::DIRECTION_LTR),
	m_Locale(pFilename),
	m_pPluralRules(NULL),
	m_pNumberFormater(NULL),
	m_pPercentFormater(NULL)
{
	str_copy(m_aName, pName, sizeof(m_aName));
	str_copy(m_aFilename, pFilename, sizeof(m_aFilename));
	str_copy(m_aParentFilename, pParentFilename, sizeof(m_aParentFilename));
	
	UErrorCode Status;
	
	Status = U_ZERO_ERROR;
	
	m_pNumberFormater = (DecimalFormat*)icu::NumberFormat::createInstance(m_Locale, UNUM_DECIMAL, Status);
	if(U_FAILURE(Status))
	{
		if(m_pNumberFormater)
		{
			delete m_pNumberFormater;
			m_pNumberFormater = NULL;
		}
		debug::ErrorStream("Localization") << "Can't create number formater for " << m_aFilename << " (error #" << Status << ")" << std::endl;
	}
	
	m_pPercentFormater = (DecimalFormat*)icu::NumberFormat::createInstance(m_Locale, UNUM_PERCENT , Status);
	if(U_FAILURE(Status))
	{
		if(m_pPercentFormater)
		{
			delete m_pPercentFormater;
			m_pPercentFormater = NULL;
		}
		debug::ErrorStream("Localization") << "Can't create percent formater for " << m_aFilename << " (error #" << Status << ")" << std::endl;
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
		debug::ErrorStream("Localization") << "Can't create plural rules for " << m_aFilename << " (error #" << Status << ")" << std::endl;
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
		delete m_pNumberFormater;
	
	if(m_pPercentFormater)
		delete m_pPercentFormater;
		
	if(m_pPluralRules)
		uplrules_close(m_pPluralRules);
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
		debug::ErrorStream("Localization") << "Can't load the localization file " << aBuf << " (" << aError << ")" << std::endl;
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
		debug::ErrorStream("Localization") << "Can't create UTF8/UTF16 converter" << std::endl;
		return false;
	}
	
	// read file data into buffer
	const char *pFilename = "languages/index.json";
	IOHANDLE File = Storage()->OpenFile(pFilename, IOFLAG_READ, CStorage::TYPE_ALL);
	if(!File)
	{
		debug::ErrorStream("Localization") << "can't open languages/index.json" << std::endl;
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
			m_pLanguages.emplace_back(new CLanguage((const char *)rStart[i]["name"], (const char *)rStart[i]["file"], (const char *)rStart[i]["parent"]));
			CLanguage* pLanguage = m_pLanguages.back().get();
			
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

bool CLocalization::PreUpdate()
{
	if(!m_pMainLanguage || m_Cfg_MainLanguage != m_pMainLanguage->GetFilename())
	{
		CLanguage* pLanguage = 0;
		
		for(unsigned int i=0; i<m_pLanguages.size(); i++)
		{
			if(m_Cfg_MainLanguage == m_pLanguages[i]->GetFilename())
			{
				pLanguage = m_pLanguages[i].get();
				break;
			}
		}
		
		if(m_pMainLanguage != pLanguage)
		{
			m_pMainLanguage = pLanguage;
			
			for(unsigned int i=0; i<m_pListeners.size(); i++)
				m_pListeners[i]->OnLocalizationModified();
		}
	}
	
	return true;
}

/* ----- Listener Managment ----------------------------------------- */
	
void CLocalization::AddListener(IListener* pListener)
{
	m_pListeners.push_back(pListener);
}

void CLocalization::RemoveListener(IListener* pListener)
{
	for(unsigned int i=0; i<m_pListeners.size(); i++)
	{
		if(m_pListeners[i] == pListener)
			m_pListeners.erase(m_pListeners.begin() + i);
	}
}

/* ----- Language Managment ----------------------------------------- */

CLocalization::CLanguage* CLocalization::GetCurrentLanguage(const char* pLanguageCode)
{
	CLanguage* pLanguage = m_pMainLanguage;	
	if(pLanguageCode)
	{
		for(unsigned int i=0; i<m_pLanguages.size(); i++)
		{
			if(str_comp(m_pLanguages[i]->GetFilename(), pLanguageCode) == 0)
			{
				pLanguage = m_pLanguages[i].get();
				break;
			}
		}
	}
	return pLanguage;
}

/* ----- Localization of text --------------------------------------- */

const char* CLocalization::LocalizeWithDepth(const char* pLanguageCode, const char* pText, int Depth)
{
	CLanguage* pLanguage = GetCurrentLanguage(pLanguageCode);
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
	CLanguage* pLanguage = GetCurrentLanguage(pLanguageCode);
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

/* ----- Localization of Integers ----------------------------------- */

void CLocalization::AppendInteger(dynamic_string& Buffer, int& BufferIter, CLanguage* pLanguage, int64 Number)
{
	icu::UnicodeString Utf16Buffer;
	
	UErrorCode Status = U_ZERO_ERROR;
	pLanguage->m_pNumberFormater->format((double) Number, Utf16Buffer, NULL, Status);
	if(U_FAILURE(Status))
		BufferIter = Buffer.append_at(BufferIter, "{0}");
	else
	{
		//Update buffer size
		dynamic_string Result;
		icu::StringByteSink<dynamic_string> ByteSink(&Result);
		Utf16Buffer.toUTF8(ByteSink);
		
		BufferIter = Buffer.append_at(BufferIter, Result.buffer());
	}
}

void CLocalization::FormatInteger(dynamic_string& Buffer, const char* pLanguageCode, int64 Number)
{
	CLanguage* pLanguage = GetCurrentLanguage(pLanguageCode);
	if(!pLanguage)
		return;
	
	int BufferIter = Buffer.length();
	AppendInteger(Buffer, BufferIter, pLanguage, Number);
}

int64 CLocalization::ParseInteger(const char* pLanguageCode, const char* pText)
{
	CLanguage* pLanguage = GetCurrentLanguage(pLanguageCode);
	if(!pLanguage)
		return 0;
	
	icu::UnicodeString Utf16Buffer = icu::UnicodeString::fromUTF8(pText);
	
	icu::Formattable Result;
	UErrorCode Status = U_ZERO_ERROR;
	pLanguage->m_pNumberFormater->parse(Utf16Buffer, Result, Status);
	
	if(U_FAILURE(Status))
		return 0;
	else if(Result.getType() == icu::Formattable::kDouble)
		return Result.getDouble();
	else if(Result.getType() == icu::Formattable::kLong)
		return Result.getLong();
	else if(Result.getType() == icu::Formattable::kInt64)
		return Result.getInt64();
	else
		return 0;
}

/* ----- Localization of Doubles ------------------------------------ */

void CLocalization::AppendDouble(dynamic_string& Buffer, int& BufferIter, CLanguage* pLanguage, double Number)
{
	icu::UnicodeString Utf16Buffer;
	
	UErrorCode Status = U_ZERO_ERROR;
	pLanguage->m_pNumberFormater->format((double) Number, Utf16Buffer, NULL, Status);
	if(U_FAILURE(Status))
		BufferIter = Buffer.append_at(BufferIter, "{0.0}");
	else
	{
		//Update buffer size
		dynamic_string Result;
		icu::StringByteSink<dynamic_string> ByteSink(&Result);
		Utf16Buffer.toUTF8(ByteSink);
		
		BufferIter = Buffer.append_at(BufferIter, Result.buffer());
	}
}

void CLocalization::FormatDouble(dynamic_string& Buffer, const char* pLanguageCode, double Number)
{
	CLanguage* pLanguage = GetCurrentLanguage(pLanguageCode);
	if(!pLanguage)
		return;
	
	int BufferIter = Buffer.length();
	AppendDouble(Buffer, BufferIter, pLanguage, Number);
}

double CLocalization::ParseDouble(const char* pLanguageCode, const char* pText)
{
	CLanguage* pLanguage = GetCurrentLanguage(pLanguageCode);
	if(!pLanguage)
		return 0;
	
	icu::UnicodeString Utf16Buffer = icu::UnicodeString::fromUTF8(pText);
	
	icu::Formattable Result;
	UErrorCode Status = U_ZERO_ERROR;
	pLanguage->m_pNumberFormater->parse(Utf16Buffer, Result, Status);
	
	if(U_FAILURE(Status))
		return 0;
	else if(Result.getType() == icu::Formattable::kDouble)
		return Result.getDouble();
	else if(Result.getType() == icu::Formattable::kLong)
		return Result.getLong();
	else if(Result.getType() == icu::Formattable::kInt64)
		return Result.getInt64();
	else
		return 0;
}

/* ----- Localization of Percents ----------------------------------- */

void CLocalization::AppendPercent(dynamic_string& Buffer, int& BufferIter, CLanguage* pLanguage, double Number)
{
	icu::UnicodeString Utf16Buffer;
	
	UErrorCode Status = U_ZERO_ERROR;
	pLanguage->m_pPercentFormater->format((double) Number, Utf16Buffer, NULL, Status);
	if(U_FAILURE(Status))
		BufferIter = Buffer.append_at(BufferIter, "{0%}");
	else
	{
		//Update buffer size
		dynamic_string Result;
		icu::StringByteSink<dynamic_string> ByteSink(&Result);
		Utf16Buffer.toUTF8(ByteSink);
		
		BufferIter = Buffer.append_at(BufferIter, Result.buffer());
	}
}

double CLocalization::ParsePercent(const char* pLanguageCode, const char* pText)
{
	CLanguage* pLanguage = GetCurrentLanguage(pLanguageCode);
	if(!pLanguage)
		return 0;
	
	icu::UnicodeString Utf16Buffer = icu::UnicodeString::fromUTF8(pText);
	
	{
		icu::Formattable Result;
		UErrorCode Status = U_ZERO_ERROR;
		pLanguage->m_pPercentFormater->parse(Utf16Buffer, Result, Status);
		
		if(U_SUCCESS(Status))
		{
			if(Result.getType() == icu::Formattable::kDouble)
				return Result.getDouble();
			else if(Result.getType() == icu::Formattable::kLong)
				return Result.getLong();
			else if(Result.getType() == icu::Formattable::kInt64)
				return Result.getInt64();
		}
	}
	
	//fallback if '%' is missing
	{
		icu::Formattable Result;
		UErrorCode Status = U_ZERO_ERROR;
		pLanguage->m_pNumberFormater->parse(Utf16Buffer, Result, Status);
		
		if(U_FAILURE(Status))
			return 0;
		else if(Result.getType() == icu::Formattable::kDouble)
			return Result.getDouble()/100.0;
		else if(Result.getType() == icu::Formattable::kLong)
			return Result.getLong()/100.0;
		else if(Result.getType() == icu::Formattable::kInt64)
			return Result.getInt64()/100.0;
		else
			return 0;
	}
}

/* ----- Localization of Durations ---------------------------------- */

void CLocalization::AppendDuration(dynamic_string& Buffer, int& BufferIter, CLanguage* pLanguage, int64 Duration)
{
	AppendDouble(Buffer, BufferIter, pLanguage, (double)Duration/1000.0);
}

void CLocalization::FormatDuration(dynamic_string& Buffer, const char* pLanguageCode, int64 Duration)
{
	CLanguage* pLanguage = GetCurrentLanguage(pLanguageCode);
	if(!pLanguage)
		return;
	
	int BufferIter = Buffer.length();
	AppendDuration(Buffer, BufferIter, pLanguage, Duration);
}

int64 CLocalization::ParseDuration(const char* pLanguageCode, const char* pText)
{
	double Result = ParseDouble(pLanguageCode, pText);
	return Result*1000.0;
}

/* ----- Format ----------------------------------------------------- */

void CLocalization::Format(dynamic_string& Buffer, const char* pLanguageCode, const CLocalizableString& LString)
{
	const char* pText = Localize(pLanguageCode, LString.GetText());
	
	CLanguage* pLanguage = GetCurrentLanguage(pLanguageCode);
	if(!pLanguage)
	{
		Buffer.append(pText);
		return;
	}
	
	const std::vector<CLocalizableString::CParameter>& Parameters = LString.GetParameters();
		
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
				for(unsigned int PIter=0; PIter<Parameters.size(); PIter++)
				{
					const char* pVarArgName = Parameters[PIter].m_Name.buffer();
					
					if(str_comp_num(pText+ParamNameStart, pVarArgName, ParamNameLength) == 0)
					{
						//Get argument type
						if(str_comp_num("str:", pText+ParamTypeStart, 4) == 0)
						{
							const char* pValue = Parameters[PIter].m_String.m_pValue;
							if(pValue)
								BufferIter = Buffer.append_at(BufferIter, pValue);
						}
						else if(str_comp_num("int:", pText+ParamTypeStart, 4) == 0)
						{
							AppendInteger(Buffer, BufferIter, pLanguage, Parameters[PIter].m_Integer.m_Value);
						}
						else if(str_comp_num("percent:", pText+ParamTypeStart, 8) == 0)
						{
							AppendPercent(Buffer, BufferIter, pLanguage, Parameters[PIter].m_Double.m_Value);
						}
						else if(str_comp_num("float:", pText+ParamTypeStart, 6) == 0)
						{
							AppendDouble(Buffer, BufferIter, pLanguage, Parameters[PIter].m_Double.m_Value);
						}
						else if(str_comp_num("duration:", pText+ParamTypeStart, 4) == 0)
						{
							AppendDuration(Buffer, BufferIter, pLanguage, Parameters[PIter].m_Duration.m_Value);
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
