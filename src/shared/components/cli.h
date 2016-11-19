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

#ifndef __SHARED_CLI__
#define __SHARED_CLI__

#include <shared/kernel.h>
#include <shared/tl/hashtable.h>

#define CONSOLE_TAB "    "

enum
{
	CLI_SUCCESS,
	CLI_FAILURE_WRONG_PARAMETER,
	CLI_FAILURE_UNKNOWN_COMMAND,
	CLI_FAILURE_INVALID_ENCODING,
};

enum
{
	CLI_LINETYPE_NORMAL,
	CLI_LINETYPE_ERROR,
	CLI_LINETYPE_HELP,
};

class CCLI_Output
{
public:
	virtual void Print(const char* pText, int Type = CLI_LINETYPE_NORMAL) = 0;
};

class CCommandLineInterpreter : public CSharedKernel::CComponent
{
public:
	class CCommand
	{
	protected:
		bool GetString(const char** ppArgs, char* pBuf, int MaxLength);
		bool GetInteger(const char** ppArgs, int* pResult);
		
	public:
		virtual ~CCommand() {}
		
		virtual int Execute(const char* pArgs, CCLI_Output* pOutput) = 0;
		virtual void Help(CCLI_Output* pOutput);
		virtual const char* Usage() = 0;
		virtual const char* Description() = 0;
	};

	//Config String
	template<typename STRING>
	class CCommand_ConfigString : public CCommand
	{
	protected:
		STRING* m_pValue;
		fixed_string128 m_Usage;
		const char* m_pDescription;
		
	public:
		CCommand_ConfigString(const char* pCommandName, const char* pDescription, STRING* pValue) :
			m_pDescription(pDescription),
			m_pValue(pValue)
		{
			m_Usage.append(pCommandName);
			m_Usage.append(" Value");
		}
		
		virtual ~CCommand_ConfigString() {}
		
		virtual int Execute(const char* pArgs, CCLI_Output* pOutput)
		{
			const char* pArgsIter = pArgs;
			fixed_string128 Buffer;
			
			if(!GetString(&pArgsIter, Buffer.buffer(), Buffer.maxsize()))
			{
				if(pOutput)
				{
					pOutput->Print("Invalid value", CLI_LINETYPE_ERROR);
					Help(pOutput);
				}
				return CLI_FAILURE_WRONG_PARAMETER;
			}
			
			m_pValue->copy(Buffer);
			return CLI_SUCCESS;
		}
		
		virtual const char* Usage() { return m_Usage.buffer(); }
		virtual const char* Description() { return m_pDescription; }
	};

protected:
	typedef hashtable<CCommand*, 128> CHashTable;
	CHashTable m_Commands;

protected:
	int ExecuteImpl(const char* pCommandLine, CCLI_Output* pOutput, bool Help);
	
public:
	CCommandLineInterpreter(CSharedKernel* pKernel);
	virtual ~CCommandLineInterpreter();
	
	virtual bool InitConfig(int argc, const char** argv);
	virtual bool Init();
	virtual void Shutdown();
	
	void Register(const char* pCommandName, CCommand* pCommand);
	void RegisterConfigInteger(const char* pCommandName, const char* pDescription, int* pValue, int Min, int Max);
	
	template<typename STRING>
	void RegisterConfigString(const char* pCommandName, const char* pDescription, STRING* pValue)
	{
		CCommand_ConfigString<STRING>* pCommand = new CCommand_ConfigString<STRING>(pCommandName, pDescription, pValue);
		Register(pCommandName, pCommand);
	}
	
	void Unregister(const char* pCommandName);
	
	void ParseCommandLine(const char* pCommandLine, char* pCommandBuf, int CommandBufSize, const char** ppArgs);
	
	const char* Usage(const char* pCommandLine);
	const char* Description(const char* pCommandLine);
	int Execute(const char* pCommandLine, CCLI_Output* pOutput = 0);
	int Help(const char* pCommandLine, CCLI_Output* pOutput = 0);
	bool ExecuteFile(const char* pFilename, CCLI_Output* pOutput = 0);

public:
	class CIterator
	{
	protected:
		CHashTable::iterator m_Iter;
		
	public:
		CIterator() {}
		CIterator(const CHashTable::iterator& Iter) : m_Iter(Iter) {}
		CIterator& operator++() { ++m_Iter; return *this; }
		const char* GetName() const { return m_Iter.key(); }
		bool operator==(const CIterator& Iter2) const { return Iter2.m_Iter == m_Iter; }
		bool operator!=(const CIterator& Iter2) const { return Iter2.m_Iter != m_Iter; }
	};
	CIterator BeginCommand() { return CIterator(m_Commands.begin()); }
	CIterator EndCommand() { return CIterator(m_Commands.end()); }
};

#endif
