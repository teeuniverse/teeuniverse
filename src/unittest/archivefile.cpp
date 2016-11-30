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

#include "unittest.h"

#include <shared/kernel.h>
#include <shared/archivefile.h>
#include <shared/components/storage.h>

TEST_BEGIN

dynamic_string Dir;

CSharedKernel* pKernel = new CSharedKernel();
TEST(pKernel->Init(argc, argv))

pKernel->Storage()->GetCompletePath(CStorage::TYPE_SAVE, "__unittestdir__", Dir);
TEST(fs_makedir(Dir.buffer()))

dynamic_string AssetsFile;
AssetsFile.append(Dir);
AssetsFile.append("/test.tup");

{
	CArchiveFile File;
	
	File.SetItemType(38, sizeof(tua_stringid), 4);
	
	{
		tua_stringid* pItem = (tua_stringid*) File.GetItem(38, 0);
		TEST(pItem != NULL);
		*pItem = File.AddString("Hello");
		TEST_WITH_OUTPUT(File.ReadUInt32(*pItem) == 0, File.ReadUInt32(*pItem));
	}
	
	{
		tua_stringid* pItem = (tua_stringid*) File.GetItem(38, 1);
		TEST(pItem != NULL);
		*pItem = File.AddString("World");
		TEST(File.ReadUInt32(*pItem) == 1);
	}
	
	{
		tua_stringid* pItem = (tua_stringid*) File.GetItem(38, 2);
		TEST(pItem != NULL);
		*pItem = File.AddString("Hello");
		TEST(File.ReadUInt32(*pItem) == 0);
	}
	
	{
		tua_stringid* pItem = (tua_stringid*) File.GetItem(38, 3);
		TEST(pItem != NULL);
		*pItem = File.AddString("Universe");
		TEST(File.ReadUInt32(*pItem) == 2);
	}
	
	File.SetItemType(52, sizeof(CTuaArray), 3);
	
	//Compressed data
	{
		int aData[128];
		for(int i=0; i<sizeof(aData)/sizeof(int); i++)
			aData[i] = i;
		
		CTuaArray* pItem = (CTuaArray*) File.GetItem(52, 0);
		TEST(pItem != NULL);
		pItem->m_Size = File.WriteUInt32(sizeof(aData)/sizeof(int));
		pItem->m_Data = File.AddData((uint8*) aData, sizeof(aData));
		TEST(File.ReadUInt32(pItem->m_Data) == 0);
	}
	
	//Compressed data
	{
		int aData[2048];
		for(int i=0; i<sizeof(aData)/sizeof(int); i++)
			aData[i] = 0;
		
		CTuaArray* pItem = (CTuaArray*) File.GetItem(52, 1);
		TEST(pItem != NULL);
		pItem->m_Size = File.WriteUInt32(sizeof(aData)/sizeof(int));
		pItem->m_Data = File.AddData((uint8*) aData, sizeof(aData));
		TEST(File.ReadUInt32(pItem->m_Data) == 1);
	}
	
	//Uncompressed data
	{
		int aData[1];
		for(int i=0; i<sizeof(aData)/sizeof(int); i++)
			aData[i] = 1;
		
		CTuaArray* pItem = (CTuaArray*) File.GetItem(52, 2);
		TEST(pItem != NULL);
		pItem->m_Size = File.WriteUInt32(sizeof(aData)/sizeof(int));
		pItem->m_Data = File.AddData((uint8*) aData, sizeof(aData));
		TEST(File.ReadUInt32(pItem->m_Data) == 2);
	}
	
	TEST(File.Write(pKernel->Storage(), "__unittestdir__/test.tup"))
	
	File.Close();
}

{
	CArchiveFile File;
	
	TEST(File.Open(pKernel->Storage(), "__unittestdir__/test.tup", CStorage::TYPE_SAVE))
	
	TEST(File.GetNumItems(38) == 4)
	TEST(File.GetNumItems(52) == 3)
	
	{
		tua_stringid* pItem = (tua_stringid*) File.GetItem(38, 0);
		TEST_WITH_OUTPUT(File.ReadUInt32(*pItem) == 0, File.ReadUInt32(*pItem))
		TEST_WITH_OUTPUT(str_comp(File.GetString(*pItem), "Hello") == 0, File.GetString(*pItem))
	}
	
	{
		tua_stringid* pItem = (tua_stringid*) File.GetItem(38, 1);
		TEST_WITH_OUTPUT(File.ReadUInt32(*pItem) == 1, File.ReadUInt32(*pItem))
		TEST_WITH_OUTPUT(str_comp(File.GetString(*pItem), "World") == 0, File.GetString(*pItem))
	}
	
	{
		tua_stringid* pItem = (tua_stringid*) File.GetItem(38, 2);
		TEST_WITH_OUTPUT(File.ReadUInt32(*pItem) == 0, File.ReadUInt32(*pItem))
		TEST_WITH_OUTPUT(str_comp(File.GetString(*pItem), "Hello") == 0, File.GetString(*pItem))
	}
	
	{
		tua_stringid* pItem = (tua_stringid*) File.GetItem(38, 3);
		TEST_WITH_OUTPUT(File.ReadUInt32(*pItem) == 2, File.ReadUInt32(*pItem))
		TEST_WITH_OUTPUT(str_comp(File.GetString(*pItem), "Universe") == 0, File.GetString(*pItem))
	}
	
	{
		CTuaArray* pItem = (CTuaArray*) File.GetItem(52, 0);
		int Size = File.ReadUInt32(pItem->m_Size);
		TEST_WITH_OUTPUT(Size == 128, Size)
		TEST_WITH_OUTPUT(pItem->m_Data == 0, pItem->m_Data)
		int* pData = (int*) File.GetData(pItem->m_Data);
		TEST_WITH_OUTPUT(pData[0] == 0, pData[0])
		TEST_WITH_OUTPUT(pData[37] == 37, pData[37])
		bool AllDataAreOk = true;
		for(int i=0; i<Size; i++)
		{
			if(pData[i] != i)
				AllDataAreOk = false;
		}
		TEST(AllDataAreOk)
	}
	
	{
		CTuaArray* pItem = (CTuaArray*) File.GetItem(52, 1);
		int Size = File.ReadUInt32(pItem->m_Size);
		TEST_WITH_OUTPUT(Size == 2048, Size)
		TEST_WITH_OUTPUT(pItem->m_Data == 1, pItem->m_Data)
		int* pData = (int*) File.GetData(pItem->m_Data);
		TEST_WITH_OUTPUT(pData[0] == 0, pData[0])
		TEST_WITH_OUTPUT(pData[467] == 0, pData[467])
		bool AllDataAreOk = true;
		for(int i=0; i<Size; i++)
		{
			if(pData[i] != 0)
				AllDataAreOk = false;
		}
		TEST(AllDataAreOk)
	}
	
	{
		CTuaArray* pItem = (CTuaArray*) File.GetItem(52, 2);
		int Size = File.ReadUInt32(pItem->m_Size);
		TEST_WITH_OUTPUT(Size == 1, Size)
		TEST_WITH_OUTPUT(pItem->m_Data == 2, pItem->m_Data)
		int* pData = (int*) File.GetData(pItem->m_Data);
		TEST_WITH_OUTPUT(pData[0] == 1, pData[0])
	}
	
	File.Close();
}

TEST(fs_remove(AssetsFile.buffer()))
TEST(fs_remove(Dir.buffer()))

pKernel->Shutdown();
delete pKernel;

TEST_END
