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

#ifndef __TW07_DATAFILE__
#define __TW07_DATAFILE__

#include <shared/system/fs.h>

class CStorage;

namespace tw07
{

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

// raw datafile access
class CDataFileReader
{
	struct CDatafile *m_pDataFile;
	void *GetDataImpl(int Index, int Swap);
public:
	CDataFileReader() : m_pDataFile(0) {}
	~CDataFileReader() { Close(); }

	bool IsOpen() const { return m_pDataFile != 0; }

	bool Open(CStorage *pStorage, const char *pFilename, int StorageType);
	bool Close();

	void *GetData(int Index);
	void *GetDataSwapped(int Index); // makes sure that the data is 32bit LE ints when saved
	int GetDataSize(int Index) const;
	void UnloadData(int Index);
	void *GetItem(int Index, int *pType, int *pID);
	int GetItemSize(int Index) const;
	void GetType(int Type, int *pStart, int *pNum);
	void *FindItem(int Type, int ID);
	int NumItems() const;
	int NumData() const;
	void Unload();

	unsigned Crc() const;
};

// write access
class CDataFileWriter
{
	struct CDataInfo
	{
		int m_UncompressedSize;
		int m_CompressedSize;
		void *m_pCompressedData;
	};

	struct CItemInfo
	{
		int m_Type;
		int m_ID;
		int m_Size;
		int m_Next;
		int m_Prev;
		void *m_pData;
	};

	struct CItemTypeInfo
	{
		int m_Num;
		int m_First;
		int m_Last;
	};

	enum
	{
		MAX_ITEM_TYPES=0xffff,
		MAX_ITEMS=1024,
		MAX_DATAS=1024,
	};

	IOHANDLE m_File;
	int m_NumItems;
	int m_NumDatas;
	int m_NumItemTypes;
	CItemTypeInfo *m_pItemTypes;
	CItemInfo *m_pItems;
	CDataInfo *m_pDatas;

public:
	CDataFileWriter();
	~CDataFileWriter();
	bool Open(CStorage *pStorage, int StorageType, const char *Filename);
	int AddData(int Size, void *pData);
	int AddDataSwapped(int Size, void *pData);
	int AddItem(int Type, int ID, int Size, void *pData);
	int Finish();
};

/* FOREIGN CODE END: TeeWorlds ****************************************/

}

#endif
