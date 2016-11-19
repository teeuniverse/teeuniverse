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

#ifndef __CLIENT_COUNTRYFLAGS__
#define __CLIENT_COUNTRYFLAGS__

#include <game/kernel.h>
#include <shared/assets/assetpath.h>
#include <shared/tl/sorted_array.h>

class CCountryFlags : public CGameKernel::CComponent
{
public:
	enum
	{
		LUT_SIZE=1024,
	};

	class CCountryFlag
	{
	public:
		int m_Code;
		char m_aCodeName[8];
		char m_aEnglishName[128];
		char m_aLocalName[128];
		CAssetPath m_SpritePath;
		
		bool operator<(const CCountryFlag& Flag) const { return (str_comp(m_aCodeName, Flag.m_aCodeName) < 0); }
	};
	
protected:
	int m_DefaultFlagIndex;
	array<int> m_FlagLut[LUT_SIZE];
	sorted_array<CCountryFlag> m_Flags;
	array<int> m_CodeAliases;

public:
	int m_Cfg_FlgLoad;

public:
	CCountryFlags(CGameKernel* pKernel);
	
	virtual bool InitConfig(int argc, const char** argv);
	virtual void SaveConfig(class CCLI_Output* pOutput);
	virtual bool Init();
	
	inline const CCountryFlag& GetFlagFromIndex(int Index) const { return m_Flags[Index]; };
	int FindIndex(int Code) const;

public:
	class CIterator
	{
	protected:
		int m_Part;
	public:
		CIterator() : m_Part(0) {}
		CIterator(int Part) : m_Part(Part) {}
		CIterator& operator++() { m_Part++; return *this; }
		int operator*() { return m_Part; }
		bool operator==(const CIterator& Iter2) { return Iter2.m_Part == m_Part; }
		bool operator!=(const CIterator& Iter2) { return Iter2.m_Part != m_Part; }
	};
	CIterator BeginFlag() const { return CIterator(0); }
	CIterator EndFlag() const { return CIterator(m_Flags.size()); }
	
};

#endif
