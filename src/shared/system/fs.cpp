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

#include "fs.h"

#include <cstdio>

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

#if defined(CONF_FAMILY_UNIX)	

	#define GETCWD getcwd
	
	#include <dirent.h>
	#include <sys/stat.h>
	#include <errno.h>
	#include <unistd.h>
	
	//I don't know if theses includes are needed
	#if defined(CONF_PLATFORM_MACOSX)
		#include <Carbon/Carbon.h>
	#endif
	
#elif defined(CONF_FAMILY_WINDOWS)

	#define GETCWD _getcwd
	
	//I don't know if theses includes are needed
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <fcntl.h>
	#include <direct.h>
	#include <errno.h>
	#include <wincrypt.h>
	
#else
	#error NOT IMPLEMENTED
#endif

IOHANDLE io_stdin() { return (IOHANDLE)stdin; }
IOHANDLE io_stdout() { return (IOHANDLE)stdout; }
IOHANDLE io_stderr() { return (IOHANDLE)stderr; }

IOHANDLE io_open(const char *filename, int flags)
{
	if(flags == IOFLAG_READ)
	{
	#if defined(CONF_FAMILY_WINDOWS)
		// check for filename case sensitive
		WIN32_FIND_DATA finddata;
		HANDLE handle;
		int length;

		length = str_length(filename);
		if(!filename || !length || filename[length-1] == '\\')
			return 0x0;
		handle = FindFirstFile(filename, &finddata);
		if(handle == INVALID_HANDLE_VALUE)
			return 0x0;
		else if(str_comp(filename+length-str_length(finddata.cFileName), finddata.cFileName) != 0)
		{
			FindClose(handle);
			return 0x0;
		}
		FindClose(handle);
	#endif
		return (IOHANDLE)fopen(filename, "rb");
	}
	if(flags == IOFLAG_WRITE)
		return (IOHANDLE)fopen(filename, "wb");
	return 0x0;
}

unsigned io_read(IOHANDLE io, void *buffer, unsigned size)
{
	return fread(buffer, 1, size, (FILE*)io);
}

unsigned io_skip(IOHANDLE io, int size)
{
	fseek((FILE*)io, size, SEEK_CUR);
	return size;
}

int io_seek(IOHANDLE io, int offset, int origin)
{
	int real_origin;

	switch(origin)
	{
	case IOSEEK_START:
		real_origin = SEEK_SET;
		break;
	case IOSEEK_CUR:
		real_origin = SEEK_CUR;
		break;
	case IOSEEK_END:
		real_origin = SEEK_END;
		break;
	default:
		return -1;
	}

	return fseek((FILE*)io, offset, real_origin);
}

long int io_tell(IOHANDLE io)
{
	return ftell((FILE*)io);
}

long int io_length(IOHANDLE io)
{
	long int length;
	io_seek(io, 0, IOSEEK_END);
	length = io_tell(io);
	io_seek(io, 0, IOSEEK_START);
	return length;
}

unsigned io_write(IOHANDLE io, const void *buffer, unsigned size)
{
	return fwrite(buffer, 1, size, (FILE*)io);
}

unsigned io_write_newline(IOHANDLE io)
{
#if defined(CONF_FAMILY_WINDOWS)
	return fwrite("\r\n", 1, 2, (FILE*)io);
#else
	return fwrite("\n", 1, 1, (FILE*)io);
#endif
}

int io_close(IOHANDLE io)
{
	fclose((FILE*)io);
	return 1;
}

int io_flush(IOHANDLE io)
{
	fflush((FILE*)io);
	return 0;
}

#if defined(CONF_FAMILY_WINDOWS)
class fs_listdir_iterator_impl : public fs_listdir_iterator
{
public:
	WIN32_FIND_DATA m_FindData;
	HANDLE m_Handle;
	int m_PathLength;
	dynamic_string m_Path;
	bool m_First;
	
public:
	virtual ~fs_listdir_iterator_impl()
	{
		if(m_Handle != INVALID_HANDLE_VALUE)
			FindClose(m_Handle);
	}
	
	virtual bool next()
	{
		if(m_Handle == INVALID_HANDLE_VALUE)
			return false;
		
		if(m_First)
		{
			m_First = false;
			return false;
		}
		else
			return (FindNextFileA(m_Handle, &m_FindData) != 0);
	}
	
	virtual const char* get_filename()
	{
		m_Path.buffer()[m_PathLength] = 0;
		m_Path.append((const char*) m_FindData.cFileName);
		return m_Path.buffer();
	}
};
#else
class fs_listdir_iterator_impl : public fs_listdir_iterator
{
public:
	struct dirent* m_DirEntry;
	DIR *m_pDir;
	int m_PathLength;
	dynamic_string m_Path;
	
public:
	virtual ~fs_listdir_iterator_impl()
	{
		if(m_pDir)
			closedir(m_pDir);
	}
	
	virtual bool next()
	{
		if(!m_pDir)
			return false;
		
		m_DirEntry = readdir(m_pDir);
		return (m_DirEntry != NULL);
	}
	
	virtual const char* get_filename()
	{
		m_Path.buffer()[m_PathLength] = 0;
		m_Path.append((const char*) m_DirEntry->d_name);
		return m_Path.buffer();
	}
};
#endif

fs_listdir_iterator* fs_create_listdir_iterator(const char *pDir)
{
	fs_listdir_iterator_impl* pIter = new fs_listdir_iterator_impl();
	
#if defined(CONF_FAMILY_WINDOWS)

	dynamic_string SearchPath;
	SearchPath.append(pDir);
	SearchPath.append("/*");

	pIter->m_Handle = FindFirstFileA(SearchPath.buffer(), &pIter->m_FindData);
	pIter->m_First = true;
	
#else	

	pIter->m_pDir = opendir(pDir);
	if(!pIter->m_pDir)
		return pIter;
	
#endif

	pIter->m_Path.clear();
	pIter->m_Path.append(pDir);
	pIter->m_Path.append("/");
	pIter->m_PathLength = pIter->m_Path.length();
	return pIter;
}

bool fs_storage_path(const char *appname, dynamic_string& result)
{
#if defined(CONF_FAMILY_WINDOWS)
	const char *home = getenv("APPDATA");
#else
	const char *home = getenv("HOME");
#endif
	if(!home)
		return false;
	
	result.append(home);
#if defined(CONF_FAMILY_WINDOWS)
	result.append("/");
	result.append(appname);
#elif defined(CONF_PLATFORM_MACOSX)
	result.append("/Library/Application Support/");
	result.append(appname);
#else
	result.append("/.");
	
	//by conviension, configuration folders on linux are lower-case
	int CursorPos = result.length();
	result.append(appname);
	char* pChar = result.buffer() + CursorPos;
	while(*pChar)
	{
		*pChar = char_lower(*pChar);
		pChar++;
	}
#endif
	
	return true;
}

bool fs_makedir(const char *path)
{
#if defined(CONF_FAMILY_WINDOWS)
	if(_mkdir(path) == 0)
			return true;
	if(errno == EEXIST)
		return true;
	return false;
#else
	if(mkdir(path, 0755) == 0)
		return true;
	if(errno == EEXIST)
		return true;
	return false;
#endif
}

int fs_is_dir(const char *path)
{
#if defined(CONF_FAMILY_WINDOWS)
	/* TODO: do this smarter */
	WIN32_FIND_DATA finddata;
	HANDLE handle;
	char buffer[1024*2];
	str_format(buffer, sizeof(buffer), "%s/*", path);

	if ((handle = FindFirstFileA(buffer, &finddata)) == INVALID_HANDLE_VALUE)
		return 0;

	FindClose(handle);
	return 1;
#else
	struct stat sb;
	if (stat(path, &sb) == -1)
		return 0;

	if (S_ISDIR(sb.st_mode))
		return 1;
	else
		return 0;
#endif
}

int fs_chdir(const char *path)
{
	if(fs_is_dir(path))
	{
		if(chdir(path))
			return 1;
		else
			return 0;
	}
	else
		return 1;
}

bool fs_getcwd(dynamic_string& result)
{
	char aBuf[512];
	if(GETCWD(aBuf, sizeof(aBuf)) == NULL)
	{
		result.clear();
		return false;
	}
		
	result.copy((const char*) aBuf);
	return true;
}

int fs_parent_dir(char *path)
{
	char *parent = 0;
	for(; *path; ++path)
	{
		if(*path == '/' || *path == '\\')
			parent = path;
	}

	if(parent)
	{
		*parent = 0;
		return 0;
	}
	return 1;
}

bool fs_remove(const char *filename)
{
	if(remove(filename) != 0)
		return false;
	return true;
}

bool fs_rename(const char *oldname, const char *newname)
{
	if(rename(oldname, newname) != 0)
		return false;
	return true;
}

/* FOREIGN CODE END: TeeWorlds ****************************************/

