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

#ifndef __CLIENT_ASSETS_ASSETPATH__
#define __CLIENT_ASSETS_ASSETPATH__

#include <shared/archivefile.h>
#include <shared/system/types.h>
#include <shared/math/math.h>

#include <vector>
#include <string>

class CAssetPath
{
public:
	class CTuaType
	{
	public:
		tua_stringid m_Name;
		tua_uint16 m_PackageId;
		tua_uint16 m_Type;
	};
	
private:
	uint32 m_Id;
	uint16 m_PackageId; //(packageid == 0 means self)
	uint16 m_Type;		//(type == 0 means null)
	
public:
	CAssetPath() : m_Id(0), m_PackageId(0), m_Type(0) { }
	CAssetPath(uint16 Type, uint16 PackageId, uint32 Id) : m_Id(Id), m_PackageId(PackageId), m_Type(Type+1) { }
	
	inline uint16 GetType() const { return m_Type-1; }
	inline uint16 GetPackageId() const { return m_PackageId; }
	inline uint32 GetId() const { return m_Id; }
	
	inline uint16 GetRawType() const { return m_Type; }
	inline uint16 GetRawPackageId() const { return m_PackageId; }
	inline uint32 GetRawId() const { return m_Id; }
	
	inline void SetType(uint16 Type) { m_Type = Type+1; }
	inline void SetPackageId(uint16 PackageId) { m_PackageId = PackageId; }
	inline void SetId(uint32 Id) { m_Id = Id; }
	
	inline void SetRawType(uint16 Type) { m_Type = Type; }
	inline void SetRawPackageId(uint16 PackageId) { m_PackageId = PackageId; }
	inline void SetRawId(uint32 Id) { m_Id = Id; }
	
	inline bool IsNull() const { return m_Type == 0; }
	inline bool IsNotNull() const { return m_Type != 0; }
	
	inline bool operator==(const CAssetPath& Path) const { return (m_PackageId == Path.m_PackageId && m_Id == Path.m_Id && m_Type == Path.m_Type); }
	inline bool operator!=(const CAssetPath& Path) const { return !(*this == Path); }
	
	static inline CAssetPath Null() { return CAssetPath(); }

public:
	enum
	{
		OPERATION_DELETE=0,
		OPERATION_RESOLVE_NAME,
	};
	
	class COperation
	{
	private:
		int m_OperationId;
		union
		{
			struct
			{
				uint32 m_Id;
				uint16 m_PackageId;
				uint16 m_Type;
			} m_OpDelete;
			struct
			{
				class CAssetsManager* m_pAssetsManager;
				std::vector<std::string>* m_pNames;
			} m_OpResolveName;
		};
	
	public:
		static inline COperation DeleteAsset(const CAssetPath& Path)
		{
			COperation Operation;
			Operation.m_OperationId = OPERATION_DELETE;
			Operation.m_OpDelete.m_PackageId = Path.GetPackageId();
			Operation.m_OpDelete.m_Type = Path.GetType();
			Operation.m_OpDelete.m_Id = Path.GetId();
			return Operation;
		}
		
		static inline COperation ResolveName(class CAssetsManager* pAssetsManager, std::vector<std::string>* pNames)
		{
			COperation Operation;
			Operation.m_OperationId = OPERATION_RESOLVE_NAME;
			Operation.m_OpResolveName.m_pNames = pNames;
			Operation.m_OpResolveName.m_pAssetsManager = pAssetsManager;
			return Operation;
		}
	
		inline bool MustBeDeleted(CAssetPath& Path) const
		{
			return (
				m_OperationId == OPERATION_DELETE &&
				Path.GetPackageId() == m_OpDelete.m_PackageId &&
				Path.GetType() == m_OpDelete.m_Type &&
				Path.GetId() == m_OpDelete.m_Id
			);
		}
	
		void Apply(CAssetPath& Path) const;
	};
};

class CSubPath
{	
public:
	class CTuaType
	{
	public:
		tua_uint16 m_Id0;
		tua_uint16 m_Id1;
		tua_uint16 m_Id2;
		tua_uint16 m_Type;
		
		static inline void Read(CArchiveFile* pArchiveFile, const CTuaType& TuaPath, CSubPath& SysPath)
		{
			SysPath.SetType(pArchiveFile->ReadUInt16(TuaPath.m_Type));
			SysPath.SetId(pArchiveFile->ReadUInt16(TuaPath.m_Id0));
			SysPath.SetId2(pArchiveFile->ReadUInt16(TuaPath.m_Id1));
			SysPath.SetId3(pArchiveFile->ReadUInt16(TuaPath.m_Id2));
		}
		
		static inline void Write(CArchiveFile* pArchiveFile, const CSubPath& SysPath, CTuaType& TuaPath)
		{
			TuaPath.m_Type = pArchiveFile->WriteUInt16(SysPath.GetType());
			TuaPath.m_Id0 = pArchiveFile->WriteUInt16(SysPath.GetId());
			TuaPath.m_Id1 = pArchiveFile->WriteUInt16(SysPath.GetId2());
			TuaPath.m_Id2 = pArchiveFile->WriteUInt16(SysPath.GetId3());
		}
	};
	
private:
	uint16 m_Type;
	uint16 m_Id0;
	uint16 m_Id1;
	uint16 m_Id2;
	
public:
	CSubPath() : m_Type(0), m_Id0(0), m_Id1(0), m_Id2(0) {  }
	CSubPath(int Type, int Id0, int Id1, int Id2) : m_Type(Type+1), m_Id0(Id0), m_Id1(Id1), m_Id2(Id2) {  }
	
public:
	inline uint16 GetType() const { return m_Type-1; }
	inline uint16 GetId() const { return m_Id0; }
	inline uint16 GetId2() const { return m_Id1; }
	inline uint16 GetId3() const { return m_Id2; }
	
	inline void SetType(uint16 Value) { m_Type = Value+1; }
	inline void SetId(uint16 Value) { m_Id0 = Value; }
	inline void SetId2(uint16 Value) { m_Id1 = Value; }
	inline void SetId3(uint16 Value) { m_Id2 = Value; }

	inline CSubPath PopId() const { return CSubPath(GetType(), GetId2(), GetId3(), 0); }
	inline CSubPath DoublePopId() const { return CSubPath(GetType(), GetId3(), 0, 0); }
	
	inline bool IsNull() const { return m_Type == 0; }
	inline bool IsNotNull() const { return m_Type != 0; }
	
	inline bool operator==(const CSubPath& Path) const { return (mem_comp(this, &Path, sizeof(CSubPath)) == 0); }
	inline bool operator!=(const CSubPath& Path) const { return (mem_comp(this, &Path, sizeof(CSubPath)) != 0); }
	
	static inline CSubPath Null() { return CSubPath(); }
	
public:
	enum
	{
		OPERATION_SHIFTID=0,
	};
	
	class COperation
	{
	private:
		int m_Type;
		int m_PackageId;
		int m_IdStart;
		int m_IdEnd;
		int m_Shift;
	
	public:
		void Apply(CSubPath& Path) const;
	};
};

#endif
