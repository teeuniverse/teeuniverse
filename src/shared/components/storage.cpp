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

/*
 * Some parts of this file comes from other projects.
 * These parts are itendified in this file by the following block:
 * 
 * FOREIGN CODE BEGIN: ProjectName *************************************
 * 
 * FOREIGN CODE END: ProjectName ***************************************
 * 
 * If ProjectName is "TeeWorlds", then this part of the code follows the
 * TeeWorlds licence:
 *      (c) Magnus Auvinen. See LICENSE_TEEWORLDS in the root of the
 *      distribution for more information. If you are missing that file,
 *      acquire a complete release at teeworlds.com.
 */
 
#include <shared/system/fs.h>
#include <shared/system/string.h>
#include <shared/system/debug.h>
#include <shared/components/storage.h>
#include <zlib.h>

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

// compiled-in data-dir path
#define DATA_DIR "data"

CStorage::CStorage(CSharedKernel* pKernel) :
	CSharedKernel::CComponent(pKernel),
	m_InitializeSaveDir(true)
{
	SetName("Storage");
}

bool CStorage::InitConfig(int argc, const char** argv)
{
	// search for custom save directory
	{
		for(int i=1; i<argc; i++)
		{
			if(str_comp(argv[i], "--save-dir") == 0)
			{
				if(i+1<argc)
				{
					if(fs_is_dir(argv[i+1]))
					{
						m_SaveDir.copy(argv[i+1]);
						dbg_msg("Storage", "Save directory: %s", argv[i+1]);
						i++;
					}
					else
					{
						dbg_msg("Storage", "Value specified with the --save-dir parameter is not a valid directory: %s", argv[i+1]);
						return false;
					}
				}
				else
				{
					dbg_msg("Storage", "Missing value for --save-dir parameter");
					return false;
				}
			}
			else if(str_comp(argv[i], "--save-dir-noinit") == 0)
			{
				m_InitializeSaveDir = false;
			}
		}
	}
	
	// get userdir
	if(m_SaveDir.empty())
		fs_storage_path("TeeUniverses", m_SaveDir);
	
	// get datadir
	FindDatadir(argv[0]);
	
	return true;
}

bool CStorage::Init()
{
	if(m_StoragePaths.size() == 0)
	{
		dbg_msg("Storage", "using standard paths");
		AddDefaultPaths();
	}

	// add save directories
	if(m_StoragePaths.size() < TYPE_SAVE || m_StoragePaths[TYPE_SAVE].empty())
	{
		dbg_msg("Storage", "no save directory specified");
		return false;
	}
	
	if(fs_makedir(m_StoragePaths[TYPE_SAVE].buffer()))
	{
		if(m_InitializeSaveDir)
		{
			dynamic_string Buf;
			
			fs_makedir(GetPath(TYPE_SAVE, "assets", Buf).buffer());
			fs_makedir(GetPath(TYPE_SAVE, "config", Buf).buffer());
		}
	}
	else
	{
		dbg_msg("Storage", "unable to create save directory (%s)", m_StoragePaths[TYPE_SAVE].buffer());
		return false;
	}

	return m_StoragePaths.size() ? true : false;
}

void CStorage::LoadPaths(const char *pArgv0)
{
	// check current directory
	IOHANDLE File = io_open("config/storage.cfg", IOFLAG_READ);
	if(!File)
	{
		// check usable path in argv[0]
		unsigned int Pos = ~0U;
		for(unsigned i = 0; pArgv0[i]; i++)
			if(pArgv0[i] == '/' || pArgv0[i] == '\\')
				Pos = i;
		
		dynamic_string Filename(pArgv0);
		Filename.append("/config/storage.cfg");
		File = io_open(Filename.buffer(), IOFLAG_READ);

		if(!File)
		{
			dbg_msg("Storage", "couldn't open config/storage.cfg");
			return;
		}
	}

	char *pLine;
	linereader LineReader(File);

	while((pLine = LineReader.get()))
	{
		if(str_length(pLine) > 9 && !str_comp_num(pLine, "add_path ", 9))
			AddPath(pLine+9);
	}

	io_close(File);

	if(m_StoragePaths.size() == 0)
		dbg_msg("Storage", "no paths found in storage.cfg");
}

void CStorage::AddDefaultPaths()
{
	AddPath("$USERDIR");
	AddPath("$DATADIR");
	AddPath("$CURRENTDIR");
}

void CStorage::AddPath(const char *pPath)
{
	if(!pPath[0])
		return;
	
	if(str_comp(pPath, "$USERDIR") == 0)
	{
		if(!m_SaveDir.empty())
		{
			dynamic_string& NewString = m_StoragePaths.increment();
			NewString.copy(m_SaveDir);
		}
	}
	else if(str_comp(pPath, "$DATADIR") == 0)
	{
		if(!m_DataDir.empty())
		{
			dynamic_string& NewString = m_StoragePaths.increment();
			NewString.copy(m_DataDir);
		}
	}
	else if(str_comp(pPath, "$CURRENTDIR") == 0)
	{
		dynamic_string& NewString = m_StoragePaths.increment();
		NewString.copy(m_CurrentDir);
	}
	else
	{
		if(fs_is_dir(pPath))
		{
			dynamic_string& NewString = m_StoragePaths.increment();
			NewString.copy(pPath);
			dbg_msg("Storage", "added path '%s'", pPath);
		}
	}
}

void CStorage::FindDatadir(const char *pArgv0)
{
	// 1) use data-dir in PWD if present
	if(fs_is_dir("data/languages"))
	{
		m_DataDir.copy("data");
		return;
	}

	// 2) use compiled-in data-dir if present
	if(fs_is_dir(DATA_DIR"/languages"))
	{
		m_DataDir.copy(DATA_DIR);
		return;
	}

	// 3) check for usable path in argv[0]
	{
		int Pos = -1;
		for(int i = 0; pArgv0[i]; i++)
			if(pArgv0[i] == '/' || pArgv0[i] == '\\')
				Pos = i;

		if(Pos >= 0)
		{
			dynamic_string Path;
			Path.append_num(pArgv0, Pos+1);
			Path.append("/data");
			
			dynamic_string TestPath;
			TestPath.append(Path);
			TestPath.append("/languages");
			
			if(fs_is_dir(TestPath.buffer()))
			{
				m_DataDir.copy(Path);
				return;
			}
		}
	}

#if defined(CONF_FAMILY_UNIX)
	// 4) check for all default locations
	{
		const char *aDirs[] = {
			"/usr/share/teeuniverses",
			"/usr/share/games/teeuniverses",
			"/usr/local/share/teeuniverses",
			"/usr/local/share/games/teeuniverses",
			"/usr/pkg/share/teeuniverses",
			"/usr/pkg/share/games/teeuniverses",
			"/opt/teeuniverses"
		};
		const int DirsCount = sizeof(aDirs) / sizeof(aDirs[0]);

		int i;
		for (i = 0; i < DirsCount; i++)
		{
			dynamic_string TestPath;
			TestPath.append(aDirs[i]);
			TestPath.append("/languages");
			
			if(fs_is_dir(TestPath.buffer()))
			{
				m_DataDir.copy(aDirs[i]);
				return;
			}
		}
	}
#endif

	// no data-dir found
	dbg_msg("Storage", "warning no data directory found");
}

const dynamic_string& CStorage::GetPath(int Type, const char *pDir, dynamic_string& Path) const
{
	if(Type >= 0 && Type < m_StoragePaths.size() && !m_StoragePaths[Type].empty())
	{
		Path.copy(m_StoragePaths[Type]);
		Path.append("/");
		Path.append(pDir);
	}
	else
		Path.copy(pDir);
	
	return Path;
}

// Open a file. This checks that the path appears to be a subdirectory
// of one of the storage paths.
IOHANDLE CStorage::OpenFile(const char *pFilename, int Flags, int Type)
{
	dynamic_string FullPath;
	return OpenFile(pFilename, Flags, Type, FullPath);
}

IOHANDLE CStorage::OpenFile(const char *pFilename, int Flags, int Type, dynamic_string& FullPath)
{
	// Check whether the path contains '..' (parent directory) paths. We'd
	// normally still have to check whether it is an absolute path
	// (starts with a path separator (or a drive name on windows)),
	// but since we concatenate this path with another path later
	// on, this can't become absolute.
	//
	// E. g. "/etc/passwd" => "/path/to/storage//etc/passwd", which
	// is safe.
	if(!str_check_pathname(pFilename))
	{
		dbg_msg("Storage", "OpenFile: check failed with %s", pFilename);
		return 0;
	}

	// open file
	if(Flags&IOFLAG_WRITE)
	{
		return io_open(GetPath(Type, pFilename, FullPath).buffer(), Flags);
	}
	else
	{
		if(Type == TYPE_ALL)
		{
			// check all available directories
			for(int i = 0; i < m_StoragePaths.size(); ++i)
			{
				IOHANDLE Handle = io_open(GetPath(i, pFilename, FullPath).buffer(), Flags);
				if(Handle)
					return Handle;
			}
		}
		else if(Type == TYPE_ABSOLUTE)
		{
			// check all available directories
			IOHANDLE Handle = io_open(pFilename, Flags);
			if(Handle)
				return Handle;
		}
		else if(Type >= 0 && Type < m_StoragePaths.size())
		{
			// check wanted directory
			IOHANDLE Handle = io_open(GetPath(Type, pFilename, FullPath).buffer(), Flags);
			if(Handle)
				return Handle;
		}
	}

	return 0;
}

void CStorage::GetCompletePath(int Type, const char *pDir, dynamic_string& Buffer)
{
	if(Type < 0 || Type >= m_StoragePaths.size())
	{
		Buffer.clear();
		return;
	}

	GetPath(Type, pDir, Buffer);
}

/* FOREIGN CODE END: TeeWorlds ****************************************/
