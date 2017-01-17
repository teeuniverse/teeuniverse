#include "fs.h"

#include <external/minizip/zip.h>

class fs_zipstream_wb : public fs_stream_wb
{
protected:
	zipFile m_pFile;
	
public:
	fs_zipstream_wb(const char* pFilename) :
		m_pFile(NULL)
	{
		m_pFile = zipOpen(pFilename, APPEND_STATUS_CREATE);
	}
	
	void open_file(const char* pFilename)
	{
		zip_fileinfo ZipFileInfo;
		ZipFileInfo.dosDate = 1;
		ZipFileInfo.internal_fa = 0; //No documentation found
		ZipFileInfo.external_fa = 0; //No documentation found
		
		zipOpenNewFileInZip(
			m_pFile,
			pFilename,
			&ZipFileInfo,
			NULL, 0,
			NULL, 0,
			NULL,
			Z_DEFLATED,
			Z_DEFAULT_COMPRESSION
		);
	}
	
	void close_file()
	{
		zipCloseFileInZip(m_pFile);
	}
	
	virtual bool is_valid()
	{
		return (m_pFile);
	}

	virtual void write(const void* pData, unsigned DataSize)
	{
		zipWriteInFileInZip(m_pFile, pData, DataSize);
	}
	
	virtual void close()
	{
		zipClose(m_pFile, NULL);
	}
};
