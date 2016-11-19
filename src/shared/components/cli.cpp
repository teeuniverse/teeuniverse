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

#include <shared/kernel.h>
#include <shared/components/cli.h>
#include <shared/components/storage.h>

/* OUTPUTS ************************************************************/

class CFileOutput : public CCLI_Output
{
private:
	IOHANDLE& m_File;
	
public:
	CFileOutput(IOHANDLE& File) :
		m_File(File)
	{ }
	
	virtual void Print(const char* pText, int Type = CLI_LINETYPE_NORMAL)
	{
		io_write(m_File, pText, str_length(pText));
		io_write(m_File, "\n", 1);
	}
};

/* COMMANDS ***********************************************************/

bool CCommandLineInterpreter::CCommand::GetString(const char** ppArgs, char* pBuf, int MaxLength)
{
	const char* pArgIter = str_skip_whitespaces((char*) *ppArgs);
	
	if(!(*pArgIter))
	{
		*ppArgs = pArgIter;
		return false;
	}
	
	int BufIter = 0;
	char StringDelimiter = 0;
	bool EscapeNextChar = false;
	while(*pArgIter && BufIter < MaxLength)
	{
		if(EscapeNextChar)
		{
			pBuf[BufIter] = *pArgIter;
			BufIter++;
			EscapeNextChar = false;
		}
		else if(*pArgIter == '\\')
		{
			pArgIter++;
			EscapeNextChar = true;
		}
		else if(*pArgIter == StringDelimiter)
		{
			StringDelimiter = 0;
			pArgIter++;
		}
		else if(*pArgIter == '"' || *pArgIter == '\'')
		{
			StringDelimiter = *pArgIter;
			pArgIter++;
		}
		else if(*pArgIter == ' ' || *pArgIter == '\t' || *pArgIter == '\n' || *pArgIter == '\r')
		{
			break;
		}
		else
		{
			pBuf[BufIter] = *pArgIter;
			BufIter++;
			pArgIter++;
		}
	}
	
	pBuf[BufIter] = 0;
	
	*ppArgs = pArgIter;
	return true;
}

bool CCommandLineInterpreter::CCommand::GetInteger(const char** ppArgs, int* pResult)
{
	fixed_string128 Buffer;
	if(!GetString(ppArgs, Buffer.buffer(), Buffer.maxsize()))
	{
		*pResult = 0;
		return false;
	}
	
	*pResult = str_toint(Buffer.buffer());
	return true;
}

void CCommandLineInterpreter::CCommand::Help(CCLI_Output* pOutput)
{
	if(pOutput)
	{
		pOutput->Print(Description(), CLI_LINETYPE_HELP);
		pOutput->Print(Usage(), CLI_LINETYPE_HELP);
	}
}

	//Echo
class CCommand_Echo : public CCommandLineInterpreter::CCommand
{
public:
	virtual int Execute(const char* pArgs, CCLI_Output* pOutput)
	{
		if(pOutput)
			pOutput->Print(pArgs, CLI_LINETYPE_NORMAL);
		return CLI_SUCCESS;
	}
	
	virtual const char* Usage() { return "echo \"Text\""; }
	virtual const char* Description() { return "Print a text in the console"; }
};

	//Help
class CCommand_Help : public CCommandLineInterpreter::CCommand
{
protected:
	CCommandLineInterpreter* m_pCLI;
	
public:
	CCommand_Help(CCommandLineInterpreter* pCLI) :
		m_pCLI(pCLI)
	{ }
	
	virtual int Execute(const char* pArgs, CCLI_Output* pOutput)
	{
		m_pCLI->Help(pArgs, pOutput);
		return CLI_SUCCESS;
	}
	
	virtual const char* Usage() { return "help \"Command\""; }
	virtual const char* Description() { return "Print the help of a command"; }
};

	//Config Integer
class CCommand_ConfigInteger : public CCommandLineInterpreter::CCommand
{
protected:
	int* m_pValue;
	int m_DefaultValue;
	int m_Min;
	int m_Max;
	fixed_string128 m_Usage;
	const char* m_pDescription;
	
public:
	CCommand_ConfigInteger(const char* pCommandName, const char* pDescription, int* pValue, int DefaultValue, int Min, int Max) :
		m_pDescription(pDescription),
		m_pValue(pValue),
		m_DefaultValue(DefaultValue),
		m_Min(Min),
		m_Max(Max)
	{
		m_Usage.append(pCommandName);
		m_Usage.append(" Value");
	}
	
	virtual int Execute(const char* pArgs, CCLI_Output* pOutput)
	{
		const char* pArgsIter = pArgs;
		int Value;
		
		if(!GetInteger(&pArgsIter, &Value) || Value < m_Min || Value > m_Max)
		{
			if(pOutput)
			{
				pOutput->Print("Invalid value", CLI_LINETYPE_ERROR);
				Help(pOutput);
			}
			return CLI_FAILURE_WRONG_PARAMETER;
		}
		
		*m_pValue = Value;
		return CLI_SUCCESS;
	}
	
	virtual const char* Usage() { return m_Usage.buffer(); }
	virtual const char* Description() { return m_pDescription; }
};

	//SaveOutput
class CCommand_SaveOutput : public CCommandLineInterpreter::CCommand
{
protected:
	CCommandLineInterpreter* m_pCLI;
	
public:
	CCommand_SaveOutput(CCommandLineInterpreter* pCLI) :
		m_pCLI(pCLI)
	{ }
	
	virtual int Execute(const char* pArgs, CCLI_Output* pOutput)
	{
		const char* pArgsIter = pArgs;
		
		fixed_string128 Filename;
		if(!GetString(&pArgsIter, Filename.buffer(), Filename.maxsize()))
		{
			if(pOutput)
			{
				pOutput->Print("Missing \"Filename\" parameter", CLI_LINETYPE_ERROR);
				Help(pOutput);
			}
			return CLI_FAILURE_WRONG_PARAMETER;
		}
		
		IOHANDLE File = m_pCLI->Storage()->OpenFile(Filename.buffer(), IOFLAG_WRITE, CStorage::TYPE_SAVE);
		if(!File)
		{
			if(pOutput)
			{
				pOutput->Print("Can't open the specified file", CLI_LINETYPE_ERROR);
				Help(pOutput);
			}
			return CLI_FAILURE_WRONG_PARAMETER;
		}
		
		CFileOutput Output(File);
		m_pCLI->Execute(pArgsIter, &Output);
		
		io_close(File);
		
		return CLI_SUCCESS;
	}
	
	virtual const char* Usage() { return "save_output \"Filename\" \"Command\""; }
	virtual const char* Description() { return "Write the output of \"Command\" in the file \"Filename\""; }
};

	//SaveConfig
class CCommand_SaveConfig : public CCommandLineInterpreter::CCommand
{
protected:
	CSharedKernel* m_pKernel;
	
public:
	CCommand_SaveConfig(CSharedKernel* pKernel) :
		m_pKernel(pKernel)
	{ }
	
	virtual int Execute(const char* pArgs, CCLI_Output* pOutput)
	{
		const char* pArgsIter = pArgs;
		
		fixed_string128 Filename;
		if(!GetString(&pArgsIter, Filename.buffer(), Filename.maxsize()))
		{
			if(pOutput)
			{
				pOutput->Print("Missing \"Filename\" parameter", CLI_LINETYPE_ERROR);
				Help(pOutput);
			}
			return CLI_FAILURE_WRONG_PARAMETER;
		}
		
		IOHANDLE File = m_pKernel->Storage()->OpenFile(Filename.buffer(), IOFLAG_WRITE, CStorage::TYPE_SAVE);
		if(!File)
		{
			if(pOutput)
			{
				pOutput->Print("Can't open the specified file", CLI_LINETYPE_ERROR);
				Help(pOutput);
			}
			return CLI_FAILURE_WRONG_PARAMETER;
		}
		
		CFileOutput Output(File);
		m_pKernel->Save(&Output);
		io_close(File);
		
		return CLI_SUCCESS;
	}
	
	virtual const char* Usage() { return "save_config \"Filename\""; }
	virtual const char* Description() { return "Write current configuration in the file \"Filename\""; }
};

	//CmdList
class CCommand_CmdList : public CCommandLineInterpreter::CCommand
{
protected:
	CCommandLineInterpreter* m_pCLI;
	
public:
	CCommand_CmdList(CCommandLineInterpreter* pCLI) :
		m_pCLI(pCLI)
	{ }
	
	virtual int Execute(const char* pArgs, CCLI_Output* pOutput)
	{
		if(pOutput)
		{
			CCommandLineInterpreter::CIterator Iter;
			for(Iter = m_pCLI->BeginCommand(); Iter != m_pCLI->EndCommand(); ++Iter)
			{
				pOutput->Print(Iter.GetName());
			}
		}
		
		return CLI_SUCCESS;
	}
	
	virtual const char* Usage() { return "cmdlist"; }
	virtual const char* Description() { return "List all available commands"; }
};

/* COMMAND LINE INTERPRETER *******************************************/

CCommandLineInterpreter::CCommandLineInterpreter(CSharedKernel* pKernel) :
	CSharedKernel::CComponent(pKernel)
{
	SetName("CLI");
}

CCommandLineInterpreter::~CCommandLineInterpreter()
{
	CHashTable::iterator Iter = m_Commands.begin();
	while(Iter != m_Commands.end())
	{
		if(Iter.data() && *Iter.data())
			delete (*Iter.data());
		
		++Iter;
	}
}

bool CCommandLineInterpreter::InitConfig(int argc, const char** argv)
{
	Register("echo", new CCommand_Echo());
	Register("help", new CCommand_Help(this));
	Register("save_output", new CCommand_SaveOutput(this));
	Register("save_config", new CCommand_SaveConfig(SharedKernel()));
	Register("cmdlist", new CCommand_CmdList(this));
	
	return true;
}

bool CCommandLineInterpreter::Init()
{
	if(SharedKernel()->Type() == KERNEL_GAME)
		ExecuteFile("config/settings.cfg");
	else if(SharedKernel()->Type() == KERNEL_EDITOR)
		ExecuteFile("config/settings_editor.cfg");
	
	return true;
}

void CCommandLineInterpreter::Shutdown()
{
	fixed_string128 Buffer;
	if(SharedKernel()->Type() == KERNEL_GAME)
		str_format(Buffer.buffer(), Buffer.maxsize(), "save_config %s", "config/settings.cfg");
	else if(SharedKernel()->Type() == KERNEL_EDITOR)
		str_format(Buffer.buffer(), Buffer.maxsize(), "save_config %s", "config/settings_editor.cfg");
	Execute(Buffer.buffer());
}
	
void CCommandLineInterpreter::Register(const char* pCommandName, CCommand* pCommand)
{
	CCommand** pOldCommand = m_Commands.get(pCommandName);
	if(pOldCommand && *pOldCommand)
		delete *pOldCommand;
	
	m_Commands.set(pCommandName, pCommand);
}

void CCommandLineInterpreter::RegisterConfigInteger(const char* pCommandName, const char* pDescription, int* pValue, int Min, int Max)
{
	CCommand_ConfigInteger* pCommand = new CCommand_ConfigInteger(pCommandName, pDescription, pValue, *pValue, Min, Max);
	Register(pCommandName, pCommand);
}

void CCommandLineInterpreter::Unregister(const char* pCommandName)
{
	CCommand** ppCommand = m_Commands.get(pCommandName);
	if(ppCommand)
	{
		delete (*ppCommand);
		m_Commands.unset(pCommandName);
	}
}

void CCommandLineInterpreter::ParseCommandLine(const char* pCommandLine, char* pCommandBuf, int CommandBufSize, const char** ppArgs)
{	
	//Get the command in the commandlist
	const char* pCommandIter = pCommandLine;
	char* pCommandEnd = str_skip_to_whitespace((char*) pCommandIter);
	int i=0;
	while(pCommandIter != pCommandEnd && i < (CommandBufSize-1))
	{
		pCommandBuf[i] = *pCommandIter;
		pCommandIter++;
		i++;
	}
	pCommandBuf[i] = 0;
	pCommandEnd = str_skip_whitespaces((char*) pCommandEnd);
	
	*ppArgs = pCommandEnd;
}

const char* CCommandLineInterpreter::Description(const char* pCommandLine)
{
	if(!str_utf8_check(pCommandLine))
		return "This command line is invalid";
	
	pCommandLine = str_skip_whitespaces((char*) pCommandLine);
	
	//Skip comments
	if(*pCommandLine == '#')
		return "This command line is commented by the \"#\" symbol";
	
	//Get the command in the commandlist
	char aCommand[64];
	const char* pArgs;
	ParseCommandLine(pCommandLine, aCommand, sizeof(aCommand), &pArgs);
	
	CCommand** ppCommand = m_Commands.get(aCommand);
	if(ppCommand)
		return (*ppCommand)->Description();
	
	return 0;
}

const char* CCommandLineInterpreter::Usage(const char* pCommandLine)
{
	if(!str_utf8_check(pCommandLine))
		return 0;
	
	pCommandLine = str_skip_whitespaces((char*) pCommandLine);
	
	//Skip comments
	if(*pCommandLine == '#')
		return 0;
	
	//Get the command in the commandlist
	char aCommand[64];
	const char* pArgs;
	ParseCommandLine(pCommandLine, aCommand, sizeof(aCommand), &pArgs);
	
	CCommand** ppCommand = m_Commands.get(aCommand);
	if(ppCommand)
		return (*ppCommand)->Usage();
	
	return 0;
}

int CCommandLineInterpreter::ExecuteImpl(const char* pCommandLine, CCLI_Output* pOutput, bool Help)
{
	if(!str_utf8_check(pCommandLine))
	{
		if(pOutput)
			pOutput->Print("Invalid encoding", CLI_LINETYPE_ERROR);
		return CLI_FAILURE_INVALID_ENCODING;
	}
	
	pCommandLine = str_skip_whitespaces((char*) pCommandLine);
	
	//Skip comments
	if(*pCommandLine == '#')
		return CLI_SUCCESS;
	
	//Get the command in the commandlist
	char aCommand[64];
	const char* pArgs;
	ParseCommandLine(pCommandLine, aCommand, sizeof(aCommand), &pArgs);
	
	CCommand** ppCommand = m_Commands.get(aCommand);
	if(ppCommand)
	{
		if(Help)
		{
			(*ppCommand)->Help(pOutput);
			return CLI_SUCCESS;
		}
		else
			return (*ppCommand)->Execute(pArgs, pOutput);
	}
	else
	{
		if(pOutput)
			pOutput->Print("Unknown command", CLI_LINETYPE_ERROR);
		return CLI_FAILURE_UNKNOWN_COMMAND;
	}
}

int CCommandLineInterpreter::Help(const char* pCommandLine, CCLI_Output* pOutput)
{
	ExecuteImpl(pCommandLine, pOutput, true);
}


int CCommandLineInterpreter::Execute(const char* pCommandLine, CCLI_Output* pOutput)
{
	ExecuteImpl(pCommandLine, pOutput, false);
}

bool CCommandLineInterpreter::ExecuteFile(const char* pFilename, CCLI_Output* pOutput)
{	
	IOHANDLE File = Storage()->OpenFile(pFilename, IOFLAG_READ, CStorage::TYPE_ALL);

	char aBuf[256];
	if(File)
	{
		dbg_msg("CLI", "Execute file %s", pFilename);
	
		char *pLine;
		linereader lr(File);

		while((pLine = lr.get()))
			Execute(pLine, pOutput);

		io_close(File);
		
		return true;
	}
	else
	{
		dbg_msg("CLI", "Can't execute file %s", pFilename);
		return false;
	}
}
