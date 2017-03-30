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

#ifndef __SHARED_DATAFILE__
#define __SHARED_DATAFILE__

#include <shared/system/types.h>
#include <shared/system/memory.h>
#include <shared/system/fs.h>
#include <shared/math/vector.h>
#include <shared/components/storage.h>

#include <vector>

/*
 * An ArchiveFile can contains 3 categories of elements:
 *  * Typed items, that are of fixed size and stored linearly for each
 *         type.
 *  * Raw data, that can be readed only if a pointer is stored somewhere
 *         in the ArchiveFile. Raw data are compressed using zlib.
 *  * Strings, that can be readed only if a pointer is stored somewhere
 *         in the ArchiveFile. Strings are stored linearly and separated
 *         by \0.
 * 
 * TODO:
 *  * Implement a safe way to store floats
 * 
 * Structure of the file:
 *  * SIGNATURE ('T' 'U' 'A' '1'), for TeeUniverse Archive v.1
 *  * HEADER
 * 
 *  * Item Type1, HEADER 
 *  * Item Type1, #1
 *  * Item Type1, #2
 *  * Item Type1, ...
 *  * Item Type1, #<NumItemOfType1>
 * 
 *  * Item Type2, HEADER
 *  * Item Type2, #1
 *  * Item Type2, #2
 *  * Item Type2, ...
 *  * Item Type2, #<NumItemOfType2>
 * 
 *  * Item ..., ...
 * 
 *  * Item Type<NumTypes>, HEADER
 *  * Item Type<NumTypes>, #1
 *  * Item Type<NumTypes>, #2
 *  * Item Type<NumTypes>, ...
 *  * Item Type<NumTypes>, #<NumItemOfType<NumTypes>>
 * 
 *  * RawData #1
 *  * RawData #2
 *  * RawData ...
 *  * RawData #<NumRawData>
 * 
 *  * Strings
 */

typedef int32 tua_int32;
typedef int64 tua_int64;
typedef uint8 tua_uint8;
typedef uint16 tua_uint16;
typedef uint32 tua_uint32;
typedef float tua_float;
typedef double tua_double;

typedef uint32 tua_stringid;
typedef uint32 tua_dataid;

class CTuaVec2
{
public:
	tua_float m_X;
	tua_float m_Y;
};

class CTuaArray
{
public:
	tua_uint32 m_Size;
	tua_dataid m_Data;
};

class CTuaArray2d
{
public:
	tua_uint32 m_Width;
	tua_uint32 m_Height;
	tua_uint32 m_Depth;
	tua_dataid m_Data;
};

class CArchiveFile_Header
{
public:
	tua_uint32 m_NumTypes;
	tua_uint32 m_NumRawDatas;
	tua_uint32 m_NumStrings;
};

class CArchiveFile_ItemType
{
public:
	tua_uint32 m_TypeId;
	tua_uint32 m_ItemSize;
	tua_uint32 m_NumItems;
};

class CArchiveFile_RawData
{
public:
	tua_uint32 m_UncompressedSize;
	tua_uint32 m_CompressedSize;
};

#if defined(CONF_ARCH_ENDIAN_BIG)
	#define TUA_READWRITE(SysType, TuaType, Name) inline SysType Read##Name(TuaType Value) const\
		{\
			swap_endian(&Value, sizeof(SysType), sizeof(Value)/sizeof(SysType));\
			return Value;\
		}\
		inline TuaType Write##Name(SysType Value) const\
		{\
			swap_endian(&Value, sizeof(SysType), sizeof(Value)/sizeof(SysType));\
			return Value;\
		}
#else
	#define TUA_READWRITE(SysType, TuaType, Name) inline SysType Read##Name(TuaType Value) const { return Value; }\
		inline TuaType Write##Name(SysType Value) const { return Value; }
#endif

class CArchiveFile
{
private:
	class CItemType
	{
	public:
		uint32 m_ItemSize;
		uint32 m_NumItems;
		uint8* m_pData;
	};
	
	class CRawData
	{
	public:
		uint32 m_UncompressedSize;
		uint32 m_CompressedSize;
		uint8* m_pCompressedData;
		uint8* m_pUncompressedData;
	};
	
	class CString
	{
	public:
		char* m_pText;
	};
	
	uint32 m_Crc;
	CItemType m_ItemTypes[256];	
	std::vector<CRawData> m_RawDatas;
	std::vector<CString> m_Strings;
	
public:	
	CArchiveFile();
	~CArchiveFile();
	
	bool Open(class CStorage* pStorage, const char* pFilename, int StorageType);
	bool Write(class CStorage* pStorage, const char* pFilename, int Type = CStorage::TYPE_SAVE);
	bool Write(class CStorage* pStorage, fs_stream_wb* pFileStream);
	void Close();
	
	void SetItemType(uint32 Type, uint32 ItemSize, uint32 NumItems);

	inline int GetNumItems(int Type) const { return m_ItemTypes[Type].m_NumItems; }
	inline uint8* GetItem(int Type, int Id) { return &m_ItemTypes[Type].m_pData[m_ItemTypes[Type].m_ItemSize * Id]; }
	inline const uint8* GetItem(int Type, int Id) const { return &m_ItemTypes[Type].m_pData[m_ItemTypes[Type].m_ItemSize * Id]; }
	
	uint8* GetData(tua_dataid StoredDataId);
	tua_dataid AddData(uint8* pData, uint32 Size);
	
	const char* GetString(tua_stringid StoredStringId);
	tua_stringid AddString(const char* pText);
	
	inline uint32 Crc() const { return m_Crc; }
	
	TUA_READWRITE(uint8, tua_uint8, Bool)
	TUA_READWRITE(uint8, tua_uint8, UInt8)
	TUA_READWRITE(uint16, tua_uint16, UInt16)
	TUA_READWRITE(uint32, tua_uint32, UInt32)
	TUA_READWRITE(int32, tua_int32, Int32)
	TUA_READWRITE(int32, tua_int64, Int64)
	TUA_READWRITE(float, tua_float, Float) //TODO: Make sure that IEEE 754 is used to store float
	TUA_READWRITE(double, tua_double, Double) //TODO: Make sure that IEEE 754 is used to store float
	
	TUA_READWRITE(uint32, tua_uint32, DataId)
	TUA_READWRITE(uint32, tua_uint32, StringId)

	inline tua_uint32 WriteColor(const vec4& Color)
	{
		uint32 Value = static_cast<uint32>(Color.r * 255.0f);
		Value += (static_cast<uint32>(Color.g * 255.0f)<<8);
		Value += (static_cast<uint32>(Color.b * 255.0f)<<16);
		Value += (static_cast<uint32>(Color.a * 255.0f)<<24);
		return WriteUInt32(Value);
	}

	inline vec4 ReadColor(tua_uint32 StoredValue)
	{
		uint32 Value = ReadUInt32(StoredValue);
		return vec4(
			static_cast<float>(Value & 255)/255.0f,
			static_cast<float>((Value>>8) & 255)/255.0f,
			static_cast<float>((Value>>16) & 255)/255.0f,
			static_cast<float>((Value>>24) & 255)/255.0f
		);
	}
};

#endif
