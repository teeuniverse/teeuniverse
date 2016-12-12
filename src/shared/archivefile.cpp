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

#include "archivefile.h"

#include <shared/system/types.h>
#include <shared/math/math.h>
#include <shared/components/storage.h>

#include <zlib.h>

CArchiveFile::CArchiveFile()
{
	//Init
	for(int i=0; i<256; i++)
	{
		m_ItemTypes[i].m_ItemSize = 0;
		m_ItemTypes[i].m_NumItems = 0;
		m_ItemTypes[i].m_pData = NULL;
	}
}

CArchiveFile::~CArchiveFile()
{
	Close();
}

bool CArchiveFile::Open(CStorage* pStorage, const char* pFilename, int StorageType)
{
	//Open file
	IOHANDLE File = pStorage->OpenFile(pFilename, IOFLAG_READ, StorageType);
	if(!File)
	{
		dbg_msg("ArchiveFile", "could not open '%s'", pFilename);
		return false;
	}

	//Get CRC
	m_Crc = crc32(0L, 0x0, 0);
	{
		enum
		{
			BUFFER_SIZE = 64*1024
		};

		unsigned char aBuffer[BUFFER_SIZE];

		while(1)
		{
			unsigned Bytes = io_read(File, aBuffer, BUFFER_SIZE);
			if(Bytes <= 0)
				break;
			m_Crc = crc32(m_Crc, aBuffer, Bytes);
		}

		io_seek(File, 0, IOSEEK_START);
	}

	//Check signature
	char aSignature[5];
	io_read(File, &aSignature, sizeof(aSignature));
	if(str_comp_num(aSignature, "TUA1", 4) != 0)
	{
		dbg_msg("datafile", "wrong signature. %s", aSignature[0], aSignature[1], aSignature[2], aSignature[3]);
		io_close(File);
		return false;
	}
	
	//Get Header
	CArchiveFile_Header StoredHeader;
	io_read(File, &StoredHeader, sizeof(StoredHeader));
	
	//Get items
	uint32 NumTypes = ReadUInt32(StoredHeader.m_NumTypes);
	for(uint32 i=0; i<NumTypes; i++)
	{
		CArchiveFile_ItemType StoredType;
		io_read(File, &StoredType, sizeof(StoredType));
		
	
		uint32 TypeId = ReadUInt32(StoredType.m_TypeId);
		CItemType& Type = m_ItemTypes[TypeId];
		Type.m_ItemSize = ReadUInt32(StoredType.m_ItemSize);
		Type.m_NumItems = ReadUInt32(StoredType.m_NumItems);
		
		Type.m_pData = new uint8[Type.m_ItemSize * Type.m_NumItems];
		
		io_read(File, Type.m_pData, Type.m_ItemSize * Type.m_NumItems * sizeof(uint8));
	}
	
	//Get raw datas
	uint32 NumRawDatas = ReadUInt32(StoredHeader.m_NumRawDatas);
	for(uint32 i=0; i<NumRawDatas; i++)
	{
		CArchiveFile_RawData StoredRawData;
		io_read(File, &StoredRawData, sizeof(StoredRawData));
		
		CRawData& RawData = m_RawDatas.increment();
		RawData.m_CompressedSize = ReadUInt32(StoredRawData.m_CompressedSize);
		RawData.m_UncompressedSize = ReadUInt32(StoredRawData.m_UncompressedSize);
		
		if(RawData.m_CompressedSize > 0)
		{
			RawData.m_pCompressedData = new uint8[RawData.m_CompressedSize];
			io_read(File, RawData.m_pCompressedData, RawData.m_CompressedSize * sizeof(uint8));
			RawData.m_pUncompressedData = NULL;
		}
		else
		{
			RawData.m_pUncompressedData = new uint8[RawData.m_UncompressedSize];
			io_read(File, RawData.m_pUncompressedData, RawData.m_UncompressedSize * sizeof(uint8));
			RawData.m_pCompressedData = NULL;
		}
	}
	
	//Get strings
	uint32 NumStrings = ReadUInt32(StoredHeader.m_NumStrings);
	for(uint32 i=0; i<NumStrings; i++)
	{
		tua_uint32 StoredLength;
		io_read(File, &StoredLength, sizeof(StoredLength));
		uint32 Length = ReadUInt32(StoredLength);
		
		CString& String = m_Strings.increment();
		String.m_pText = new char[Length+1];
		io_read(File, String.m_pText, sizeof(char)*(Length+1));
	}
	
	io_close(File);
	dbg_msg("ArchiveFile", "'%s' loaded", pFilename);
	
	return true;
}

bool CArchiveFile::Write(CStorage* pStorage, const char* pFilename)
{
	//Open file
	IOHANDLE File = pStorage->OpenFile(pFilename, IOFLAG_WRITE, CStorage::TYPE_SAVE);
	if(!File)
	{
		dbg_msg("ArchiveFile", "could not open '%s'", pFilename);
		return false;
	}

	//Write signature
	const char aSignature[] = "TUA1";
	io_write(File, &aSignature, sizeof(aSignature));
	
	//Counte number of types
	int NumTypes = 0;
	for(int i=0; i<256; i++)
	{
		if(m_ItemTypes[i].m_NumItems > 0)
			NumTypes++;
	}
	
	//Write Header
	CArchiveFile_Header StoredHeader;
	StoredHeader.m_NumTypes = WriteUInt32(NumTypes);
	StoredHeader.m_NumStrings = WriteUInt32(m_Strings.size());
	StoredHeader.m_NumRawDatas = WriteUInt32(m_RawDatas.size());
	io_write(File, &StoredHeader, sizeof(StoredHeader));
	
	//Write items
	for(uint32 i=0; i<256; i++)
	{
		CItemType& Type = m_ItemTypes[i];
		
		if(Type.m_NumItems == 0)
			continue;
		
		CArchiveFile_ItemType StoredType;
		StoredType.m_ItemSize = WriteUInt32(Type.m_ItemSize);
		StoredType.m_NumItems = WriteUInt32(Type.m_NumItems);
		StoredType.m_TypeId = WriteUInt32(i);
		io_write(File, &StoredType, sizeof(StoredType));
		
		io_write(File, Type.m_pData, StoredType.m_NumItems * StoredType.m_ItemSize * sizeof(uint8));
	}
	
	//Write raw datas
	for(uint32 i=0; i<m_RawDatas.size(); i++)
	{
		CRawData& RawData = m_RawDatas[i];
		CArchiveFile_RawData StoredRawData;
		if(RawData.m_pCompressedData)
		{
			StoredRawData.m_CompressedSize = WriteUInt32(RawData.m_CompressedSize);
			StoredRawData.m_UncompressedSize = WriteUInt32(RawData.m_UncompressedSize);
			io_write(File, &StoredRawData, sizeof(StoredRawData));
			io_write(File, RawData.m_pCompressedData, RawData.m_CompressedSize * sizeof(uint8));
		}
		else
		{
			StoredRawData.m_CompressedSize = WriteUInt32(0);
			StoredRawData.m_UncompressedSize = WriteUInt32(RawData.m_UncompressedSize);
			io_write(File, &StoredRawData, sizeof(StoredRawData));
			io_write(File, RawData.m_pUncompressedData, RawData.m_UncompressedSize * sizeof(uint8));
		}
	}
	
	//Write strings
	for(uint32 i=0; i<m_Strings.size(); i++)
	{
		
		CString& String = m_Strings[i];
		uint32 Length = str_length(String.m_pText);
		tua_uint32 StoredLength = WriteUInt32(Length);
		io_write(File, &StoredLength, sizeof(StoredLength));
		io_write(File, String.m_pText, sizeof(char)*(Length+1));
	}
	
	io_close(File);
	dbg_msg("ArchiveFile", "'%s' saved", pFilename);
	
	return true;
}

void CArchiveFile::Close()
{
	for(int i=0; i<256; i++)
	{
		if(m_ItemTypes[i].m_pData)
		{
			delete[] m_ItemTypes[i].m_pData;
			m_ItemTypes[i].m_pData = NULL;
		}
	}
	
	for(int i=0; i<m_RawDatas.size(); i++)
	{
		if(m_RawDatas[i].m_pCompressedData)
			delete[] m_RawDatas[i].m_pCompressedData;
		if(m_RawDatas[i].m_pUncompressedData)
			delete[] m_RawDatas[i].m_pUncompressedData;
	}
	m_RawDatas.clear();
	
	for(int i=0; i<m_Strings.size(); i++)
	{
		if(m_Strings[i].m_pText)
			delete[] m_Strings[i].m_pText;
	}
	m_Strings.clear();
}

void CArchiveFile::SetItemType(uint32 Type, uint32 ItemSize, uint32 NumItems)
{
	if(m_ItemTypes[Type].m_pData)
		delete[] m_ItemTypes[Type].m_pData;
	
	m_ItemTypes[Type].m_ItemSize = ItemSize;
	m_ItemTypes[Type].m_NumItems = NumItems;
	m_ItemTypes[Type].m_pData = new uint8[ItemSize * NumItems];
}

uint8* CArchiveFile::GetData(tua_dataid StoredDataId)
{
	uint32 DataId = ReadDataId(StoredDataId);
	
	if(DataId < 0 || DataId >= m_RawDatas.size())
	{
		dbg_msg("ArchiveFile", "can't read the data #%d", DataId);
		return NULL;
	}
	
	if(m_RawDatas[DataId].m_pUncompressedData)
	{
		return m_RawDatas[DataId].m_pUncompressedData;
	}
	else
	{
		unsigned long Size = m_RawDatas[DataId].m_UncompressedSize;
		m_RawDatas[DataId].m_pUncompressedData = new uint8[m_RawDatas[DataId].m_UncompressedSize];
		
		int Result = uncompress(
			(Bytef*)m_RawDatas[DataId].m_pUncompressedData,
			&Size,
			(Bytef*)m_RawDatas[DataId].m_pCompressedData,
			m_RawDatas[DataId].m_CompressedSize
		);
		
		if(Result != Z_OK)
			dbg_msg("ArchiveFile", "compression error: %s", zError(Result));
			
		return m_RawDatas[DataId].m_pUncompressedData;
	}
}

tua_dataid CArchiveFile::AddData(uint8* pData, uint32 Size)
{
	unsigned long CompressedSize = compressBound(Size);
	CRawData& RawData = m_RawDatas.increment();
	RawData.m_UncompressedSize = Size;
	RawData.m_pUncompressedData = NULL;
	
	RawData.m_pCompressedData = new uint8[CompressedSize];
	
	int Result = compress((Bytef*)RawData.m_pCompressedData, &CompressedSize, (Bytef*)pData, Size);
	if(Result != Z_OK)
		dbg_msg("ArchiveFile", "compression error: %s", zError(Result));
	
	if(CompressedSize >= Size)
	{
		RawData.m_CompressedSize = 0;
		delete[] RawData.m_pCompressedData;
		RawData.m_pCompressedData = NULL;
		
		RawData.m_pUncompressedData = new uint8[Size];
		mem_copy(RawData.m_pUncompressedData, pData, Size);
	}
	else
	{
		RawData.m_CompressedSize = CompressedSize;
	}
	
	return WriteDataId((uint32) m_RawDatas.size()-1);
}

const char* CArchiveFile::GetString(tua_stringid StoredStringId)
{
	if(StoredStringId == -1)
		return NULL;
	
	uint32 StringId = ReadStringId(StoredStringId);
	if(StringId >= m_Strings.size())
		return NULL;
	else
		return m_Strings[StringId].m_pText;
}

tua_stringid CArchiveFile::AddString(const char* pText)
{
	if(!pText)
		return -1;
	
	//Search if the string already exists
	for(int i=0; i<m_Strings.size(); i++)
	{
		if(str_comp(pText, m_Strings[i].m_pText) == 0)
			return WriteStringId((uint32) i);
	}
	
	int Length = str_length(pText);
	CString& String = m_Strings.increment();
	String.m_pText = new char[Length+1];
	str_copy(String.m_pText, pText, sizeof(char)*(Length+1));
	
	return WriteStringId((uint32) m_Strings.size()-1);
}
