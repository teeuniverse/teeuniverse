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

#ifndef __SHARED_SYSTEM_FS__
#define __SHARED_SYSTEM_FS__

#include <shared/system/memory.h>
#include <shared/system/string.h>

#include <cstddef>

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

enum
{
	IOFLAG_READ = 1,
	IOFLAG_WRITE = 2,
	IOFLAG_RANDOM = 4,

	IOSEEK_START = 0,
	IOSEEK_CUR = 1,
	IOSEEK_END = 2
};

typedef struct IOINTERNAL *IOHANDLE;

#if defined(CONF_FAMILY_WINDOWS)
	#define FS_DIRECTORY_SEPARATOR "\\"
#else
	#define FS_DIRECTORY_SEPARATOR "/"
#endif

/*
	Function: io_open
		Opens a file.

	Parameters:
		filename - File to open.
		flags - A set of flags. IOFLAG_READ, IOFLAG_WRITE, IOFLAG_RANDOM.

	Returns:
		Returns a handle to the file on success and 0 on failure.

*/
IOHANDLE io_open(const char *filename, int flags);

/*
	Function: io_read
		Reads data into a buffer from a file.

	Parameters:
		io - Handle to the file to read data from.
		buffer - Pointer to the buffer that will recive the data.
		size - Number of bytes to read from the file.

	Returns:
		Number of bytes read.

*/
unsigned io_read(IOHANDLE io, void *buffer, unsigned size);

/*
	Function: io_skip
		Skips data in a file.

	Parameters:
		io - Handle to the file.
		size - Number of bytes to skip.

	Returns:
		Number of bytes skipped.
*/
unsigned io_skip(IOHANDLE io, int size);

/*
	Function: io_write
		Writes data from a buffer to file.

	Parameters:
		io - Handle to the file.
		buffer - Pointer to the data that should be written.
		size - Number of bytes to write.

	Returns:
		Number of bytes written.
*/
unsigned io_write(IOHANDLE io, const void *buffer, unsigned size);

/*
	Function: io_write_newline
		Writes newline to file.

	Parameters:
		io - Handle to the file.

	Returns:
		Number of bytes written.
*/
unsigned io_write_newline(IOHANDLE io);

/*
	Function: io_seek
		Seeks to a specified offset in the file.

	Parameters:
		io - Handle to the file.
		offset - Offset from pos to stop.
		origin - Position to start searching from.

	Returns:
		Returns 0 on success.
*/
int io_seek(IOHANDLE io, int offset, int origin);

/*
	Function: io_tell
		Gets the current position in the file.

	Parameters:
		io - Handle to the file.

	Returns:
		Returns the current position. -1L if an error occured.
*/
long int io_tell(IOHANDLE io);

/*
	Function: io_length
		Gets the total length of the file. Resetting cursor to the beginning

	Parameters:
		io - Handle to the file.

	Returns:
		Returns the total size. -1L if an error occured.
*/
long int io_length(IOHANDLE io);

/*
	Function: io_close
		Closes a file.

	Parameters:
		io - Handle to the file.

	Returns:
		Returns 0 on success.
*/
int io_close(IOHANDLE io);

/*
	Function: io_flush
		Empties all buffers and writes all pending data.

	Parameters:
		io - Handle to the file.

	Returns:
		Returns 0 on success.
*/
int io_flush(IOHANDLE io);


/*
	Function: io_stdin
		Returns an <IOHANDLE> to the standard input.
*/
IOHANDLE io_stdin();

/*
	Function: io_stdout
		Returns an <IOHANDLE> to the standard output.
*/
IOHANDLE io_stdout();

/*
	Function: io_stderr
		Returns an <IOHANDLE> to the standard error.
*/
IOHANDLE io_stderr();

/*
	Function: fs_makedir
		Creates a directory

	Parameters:
		path - Directory to create

	Remarks:
		Does not create several directories if needed. "a/b/c" will result
		in a failure if b or a does not exist.
*/
bool fs_makedir(const char *path);

/*
	Function: fs_home_path
		Fetches per user home directory.

	Remarks:
		- Returns ~/.appname on UNIX based systems
		- Returns ~/Library/Applications Support/appname on Mac OS X
		- Returns %APPDATA%/Appname on Windows based systems
*/
bool fs_home_path(dynamic_string& result);

/*
	Function: fs_storage_path
		Fetches per user configuration directory.

	Remarks:
		- Returns ~/.appname on UNIX based systems
		- Returns ~/Library/Applications Support/appname on Mac OS X
		- Returns %APPDATA%/Appname on Windows based systems
*/
bool fs_storage_path(const char *appname, dynamic_string& result);

/*
	Function: fs_is_dir
		Checks if directory exists

	Returns:
		Returns 1 on success, 0 on failure.
*/
int fs_is_dir(const char *path);

/*
	Function: fs_chdir
		Changes current working directory

	Returns:
		Returns 0 on success, 1 on failure.
*/
int fs_chdir(const char *path);

/*
	Function: fs_getcwd
		Gets the current working directory.

	Returns:
		Returns a pointer to the buffer on success, 0 on failure.
*/
bool fs_getcwd(dynamic_string& result);

/*
	Function: fs_parent_dir
		Get the parent directory of a directory

	Parameters:
		path - The directory string

	Returns:
		Returns 0 on success, 1 on failure.

	Remarks:
		- The string is treated as zero-terminated string.
*/
int fs_parent_dir(char *path);

/*
	Function: fs_remove
		Deletes the file with the specified name.

	Parameters:
		filename - The file to delete

	Remarks:
		- The strings are treated as zero-terminated strings.
*/
bool fs_remove(const char *filename);

/*
	Function: fs_rename
		Renames the file or directory. If the paths differ the file will be moved.

	Parameters:
		oldname - The actual name
		newname - The new name

	Remarks:
		- The strings are treated as zero-terminated strings.
*/
bool fs_rename(const char *oldname, const char *newname);

class linereader
{
private:
	char m_aBuffer[4*1024];
	unsigned m_BufferPos;
	unsigned m_BufferSize;
	unsigned m_BufferMaxSize;
	IOHANDLE m_File;
	
public:
	linereader(IOHANDLE File) { init(File); }
	
	inline void init(IOHANDLE File)
	{
		m_BufferMaxSize = sizeof(m_aBuffer);
		m_BufferSize = 0;
		m_BufferPos = 0;
		m_File = File;
	}
	
	char* get()
	{
		unsigned LineStart = m_BufferPos;
		bool CRLFBreak = false;

		while(1)
		{
			if(m_BufferPos >= m_BufferSize)
			{
				// fetch more

				// move the remaining part to the front
				unsigned Read;
				unsigned Left = m_BufferSize - LineStart;

				if(LineStart > m_BufferSize)
					Left = 0;
				if(Left)
					mem_move(m_aBuffer, &m_aBuffer[LineStart], Left);
				m_BufferPos = Left;

				// fill the buffer
				Read = io_read(m_File, &m_aBuffer[m_BufferPos], m_BufferMaxSize-m_BufferPos);
				m_BufferSize = Left + Read;
				LineStart = 0;

				if(!Read)
				{
					if(Left)
					{
						m_aBuffer[Left] = 0; // return the last line
						m_BufferPos = Left;
						m_BufferSize = Left;
						return m_aBuffer;
					}
					else
						return 0x0; // we are done!
				}
			}
			else
			{
				if(m_aBuffer[m_BufferPos] == '\n' || m_aBuffer[m_BufferPos] == '\r')
				{
					// line found
					if(m_aBuffer[m_BufferPos] == '\r')
					{
						if(m_BufferPos+1 >= m_BufferSize)
						{
							// read more to get the connected '\n'
							CRLFBreak = true;
							++m_BufferPos;
							continue;
						}
						else if(m_aBuffer[m_BufferPos+1] == '\n')
							m_aBuffer[m_BufferPos++] = 0;
					}
					m_aBuffer[m_BufferPos++] = 0;
					return &m_aBuffer[LineStart];
				}
				else if(CRLFBreak)
				{
					if(m_aBuffer[m_BufferPos] == '\n')
						m_aBuffer[m_BufferPos++] = 0;
					return &m_aBuffer[LineStart];
				}
				else
					m_BufferPos++;
			}
		}
	}
};

/* FOREIGN CODE END: TeeWorlds ****************************************/

class fs_listdir_iterator
{
public:
	virtual ~fs_listdir_iterator() {};
	virtual bool next() = 0;
	virtual const char* get_filename() = 0;
};

/**
 * Lists the files in a directory
 * @param dir Directory to list
 * @return an iterator
 */
fs_listdir_iterator* fs_create_listdir_iterator(const char *pDir);

class fs_stream_wb
{
public:
	virtual bool is_valid() = 0;
	virtual void close() = 0;
	virtual void write(const void* pData, unsigned DataSize) = 0;
};

class fs_filestream_wb : public fs_stream_wb
{
protected:
	IOHANDLE m_pFile;
	
public:
	fs_filestream_wb(const char* pFilename) :
		m_pFile(NULL)
	{
		m_pFile = io_open(pFilename, IOFLAG_WRITE);
	}
	
	virtual bool is_valid()
	{
		return (m_pFile);
	}

	virtual void write(const void* pData, unsigned DataSize)
	{
		io_write(m_pFile, pData, DataSize);
	}
	
	virtual void close()
	{
		io_close(m_pFile);
	}
};

#endif
