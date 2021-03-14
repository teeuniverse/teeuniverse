#
# Copyright (C) 2016 necropotame (necropotame@gmail.com)
#
# This file is part of TeeUniverse.
#
# TeeUniverse is free software: you can redistribute it and/or  modify
# it under the terms of the GNU Affero General Public License, version 3,
# as published by the Free Software Foundation.
#
# TeeUniverse is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>.
#

import sys, os

# TAG_ASSETSVERSION
versionList = ["0.2.0", "0.2.1", "0.2.2", "0.2.3", "0.2.4", "0.3.0"]

class SubPathType:
	def __init__(self, name, enumname, numidx):
		self.name = name
		self.enumname = enumname
		self.numidx = numidx

class GetSetInterface:
	def __init__(self, suffix):
		self.depth = 0
		self.subpath = 0
		self.enum = suffix.upper()
		self.name = suffix
		self.getbyref = 0
		self.noset = 0
		
class GetSetInterface_Simple(GetSetInterface):
	def __init__(self, suffix, returnType, getSetType, paramType, defaultValue):
		GetSetInterface.__init__(self, suffix)
		self.getSetType = getSetType
		self.returnType = returnType
		self.paramType = paramType
		self.defaultValue = defaultValue
	def generateSet(self, var, value):
		return [var + " = " + value + ";"]
	def generateGet(self, var, defaultValue):
		return ["return " + var + ";"]
		
class GetSetInterface_SimpleRef(GetSetInterface):
	def __init__(self, suffix, returnType, paramType):
		GetSetInterface.__init__(self, suffix)
		self.getSetType = returnType
		self.returnType = returnType
		self.paramType = paramType
		self.getbyref = 1
		self.noset = 1
		self.defaultValue = ""
	def generateSet(self, var, value):
		return [var + " = " + value + ";"]
	def generateGet(self, var, defaultValue):
		return ["return " + var + ";"]
		
class GetSetInterface_SimpleGet(GetSetInterface):
	def __init__(self, suffix, returnType, paramType):
		GetSetInterface.__init__(self, suffix)
		self.getSetType = returnType
		self.returnType = returnType
		self.paramType = paramType
		self.noset = 1
	def generateSet(self, var, value):
		return [var + " = " + value + ";"]
	def generateGet(self, var, defaultValue):
		return ["return " + var + ";"]

class GetSetInterface_Struct(GetSetInterface):
	def __init__(self, suffix, returnType, paramType, getMem, setMem, defaultValue):
		GetSetInterface.__init__(self, suffix)
		self.getSetType = returnType
		self.returnType = returnType
		self.paramType = paramType
		self.getMem = getMem
		self.setMem = setMem
		self.defaultValue = defaultValue
	def generateSet(self, var, value):
		return [ var + "." + self.setMem + " = " + value + ";" ]
	def generateGet(self, var, defaultValue):
		return [ "return " + var + "." + self.getMem + ";" ]

class GetSetInterface_String(GetSetInterface):
	def __init__(self):
		GetSetInterface.__init__(self, "")
		self.getSetType = "const char*"
		self.returnType = "const char*"
		self.paramType = "const char*"
		self.defaultValue = "NULL"
	def generateSet(self, var, value):
		return [ var + " = " + value + ";" ]
	def generateGet(self, var, defaultValue):
		return [ "return " + var + ".buffer();" ]

class GetSetInterface_Func(GetSetInterface):
	def __init__(self, suffix, returnType, paramType, getFunc, setFunc, defaultValue):
		GetSetInterface.__init__(self, suffix)
		self.getSetType = returnType
		self.returnType = returnType
		self.paramType = paramType
		self.setFunc = setFunc
		self.getFunc = getFunc
		self.defaultValue = defaultValue
	def generateSet(self, var, value):
		return [ var + "." + self.setFunc + "(" + value + ");" ]
	def generateGet(self, var, defaultValue):
		return [ "return " + var + "." + self.getFunc + "();" ]

class GetSetInterface_GetArray(GetSetInterface):
	def __init__(self, suffix, returnType, defaultValue):
		GetSetInterface.__init__(self, suffix)
		self.getSetType = returnType
		self.returnType = returnType
		self.paramType = returnType
		self.defaultValue = defaultValue
		self.noset = 1
	def generateSet(self, var, value):
		return []
	def generateGet(self, var, defaultValue):
		return [ "return &(" + var + ".front());" ]

class GetSetInterface_GetFunc(GetSetInterface):
	def __init__(self, suffix, returnType, getFunc, defaultValue):
		GetSetInterface.__init__(self, suffix)
		self.getSetType = returnType
		self.returnType = returnType
		self.paramType = returnType
		self.getFunc = getFunc
		self.defaultValue = defaultValue
		self.noset = 1
	def generateSet(self, var, value):
		return []
	def generateGet(self, var, defaultValue):
		return [ "return " + var + "." + self.getFunc + "();" ]

class Type:
	def __init__(self, name):
		self.tname = name
	def fullType(self):
		return self.tname
	def headers(self):
		return []
	def addInterfaces(self):
		return []
	def generateCopy(self, var, value):
		return [var + " = " + value + ";"]
	def generateTransfert(self, var, value):
		return [var + " = std::move(" + value + ");"]
	def generateAssetPathOp(self, var, operation):
		return []
	def generateDeclaration(self, var):
		return [self.tname + " " + var + ";"]
	def tuaType(self, version):
		return ""
	def generateTuaDeclaration(self, var, version):
		if self.tuaType(version):
			return [ self.tuaType(version) + " " + var + ";" ]
		else:
			return []
	def numSubPathId(self):
		return 0
	def generateIterators(self, name, var):
		return []
	def subPathTypes(self):
		return []

class TypeUInt8(Type):
	def __init__(self):
		Type.__init__(self, "uint8")
	def tuaType(self, version):
		return "tua_uint8"
	def getSetInterfaces(self):
		return [GetSetInterface_Simple("", self.tname, "uint32", self.tname)]
	def generateWrite(self, varSys, varTua, version):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteUInt8(" + varSys + ");"]
	def generateRead(self, varSys, varTua, version):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadUInt8(" + varTua + ");"]
	def getSetInterfaces(self):
		return [ GetSetInterface_Simple("", self.tname, "int", self.tname, "0") ]

class TypeUInt16(Type):
	def __init__(self):
		Type.__init__(self, "uint16")
	def tuaType(self, version):
		return "tua_uint16"
	def getSetInterfaces(self):
		return [GetSetInterface_Simple("", self.tname, "uint32", self.tname)]
	def generateWrite(self, varSys, varTua, version):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteUInt16(" + varSys + ");"]
	def generateRead(self, varSys, varTua, version):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadUInt16(" + varTua + ");"]
	def getSetInterfaces(self):
		return [ GetSetInterface_Simple("", self.tname, "int", self.tname, "0") ]

class TypeUInt32(Type):
	def __init__(self):
		Type.__init__(self, "uint32")
	def tuaType(self, version):
		return "tua_uint32"
	def getSetInterfaces(self):
		return [GetSetInterface_Simple("", self.tname, "uint32", self.tname)]
	def generateWrite(self, varSys, varTua, version):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteUInt32(" + varSys + ");"]
	def generateRead(self, varSys, varTua, version):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadUInt32(" + varTua + ");"]
	def getSetInterfaces(self):
		return [ GetSetInterface_Simple("", self.tname, self.tname, self.tname, "0") ]

class TypeInt32(Type):
	def __init__(self):
		Type.__init__(self, "int")
	def tuaType(self, version):
		return "tua_int32"
	def generateWrite(self, varSys, varTua, version):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteInt32(" + varSys + ");"]
	def generateRead(self, varSys, varTua, version):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadInt32(" + varTua + ");"]
	def getSetInterfaces(self):
		return [ GetSetInterface_Simple("", self.tname, self.tname, self.tname, "0") ]

class TypeInt64(Type):
	def __init__(self):
		Type.__init__(self, "int64")
	def tuaType(self, version):
		return "tua_int64"
	def generateWrite(self, varSys, varTua, version):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteInt64(" + varSys + ");"]
	def generateRead(self, varSys, varTua, version):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadInt64(" + varTua + ");"]
	def getSetInterfaces(self):
		return [ GetSetInterface_Simple("", self.tname, self.tname, self.tname, "0") ]

class TypeBool(Type):
	def __init__(self):
		Type.__init__(self, "bool")
	def tuaType(self, version):
		return "tua_uint8"
	def generateWrite(self, varSys, varTua, version):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteBool(" + varSys + ");"]
	def generateRead(self, varSys, varTua, version):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadBool(" + varTua + ");"]
	def getSetInterfaces(self):
		return [ GetSetInterface_Simple("", self.tname, self.tname, self.tname, "false") ]

class TypeFloat(Type):
	def __init__(self):
		Type.__init__(self, "float")
	def tuaType(self, version):
		return "tua_float"
	def generateWrite(self, varSys, varTua, version):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteFloat(" + varSys + ");"]
	def generateRead(self, varSys, varTua, version):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadFloat(" + varTua + ");"]
	def getSetInterfaces(self):
		return [ GetSetInterface_Simple("", self.tname, self.tname, self.tname, "0.0f") ]

class TypeDouble(Type):
	def __init__(self):
		Type.__init__(self, "double")
	def tuaType(self, version):
		return "tua_double"
	def generateWrite(self, varSys, varTua, version):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteDouble(" + varSys + ");"]
	def generateRead(self, varSys, varTua, version):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadDouble(" + varTua + ");"]
	def getSetInterfaces(self):
		return [ GetSetInterface_Simple("", self.tname, self.tname, self.tname, "0.0f") ]

class TypeVec2(Type):
	def __init__(self):
		Type.__init__(self, "vec2")
	def headers(self):
		return ["shared/math/vector.h"]
	def getSetInterfaces(self):
		return [
			GetSetInterface_Simple("", self.tname, self.tname, self.tname, "0.0f"),
			GetSetInterface_Struct("X", "float", "float", "x", "x", "0.0f"),
			GetSetInterface_Struct("Y", "float", "float", "y", "y", "0.0f")
		]
	def tuaType(self, version):
		return "CTuaVec2"
	def generateWrite(self, varSys, varTua, version):
		return [
			varTua + ".m_X = pLoadingContext->ArchiveFile()->WriteFloat(" + varSys + ".x);",
			varTua + ".m_Y = pLoadingContext->ArchiveFile()->WriteFloat(" + varSys + ".y);"
		]
	def generateRead(self, varSys, varTua, version):
		return [
			varSys + ".x = pLoadingContext->ArchiveFile()->ReadFloat(" + varTua + ".m_X);",
			varSys + ".y = pLoadingContext->ArchiveFile()->ReadFloat(" + varTua + ".m_Y);"
		]

class TypeColor(Type):
	def __init__(self):
		Type.__init__(self, "vec4")
	def headers(self):
		return ["shared/math/vector.h"]
	def tuaType(self, version):
		return "tua_uint32"
	def generateWrite(self, varSys, varTua, version):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteColor(" + varSys + ");"]
	def generateRead(self, varSys, varTua, version):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadColor(" + varTua + ");"]
	def getSetInterfaces(self):
		return [ GetSetInterface_Simple("", self.tname, self.tname, self.tname, "1.0f") ]

class TypeAssetPath(Type):
	def __init__(self):
		Type.__init__(self, "CAssetPath")
	def headers(self):
		return ["shared/assets/assetpath.h"]
	def getSetInterfaces(self):
		return [ GetSetInterface_Simple("", self.tname, self.tname, "const "+self.tname+"&", "CAssetPath::Null()") ]
	def tuaType(self, version):
		return "CAssetPath::CTuaType"
	def generateWrite(self, varSys, varTua, version):
		return ["pLoadingContext->WriteAssetPath("+varSys+", "+varTua+");"]
	def generateRead(self, varSys, varTua, version):
		return ["pLoadingContext->ReadAssetPath("+varTua+", "+varSys+");"]
	def generateAssetPathOp(self, var, operation):
		return [operation+".Apply("+var+");"]

class TypeSubPath(Type):
	def __init__(self):
		Type.__init__(self, "CSubPath")
	def headers(self):
		return ["shared/assets/assetpath.h"]
	def getSetInterfaces(self):
		return [ GetSetInterface_Simple("", self.tname, self.tname, "const "+self.tname+"&", "CSubPath::Null()") ]
	def tuaType(self, version):
		return "CSubPath::CTuaType"
	def generateWrite(self, varSys, varTua, version):
		return ["CSubPath::CTuaType::Write(pLoadingContext->ArchiveFile(), "+varSys+", "+varTua+");"]
	def generateRead(self, varSys, varTua, version):
		return ["CSubPath::CTuaType::Read(pLoadingContext->ArchiveFile(), "+varTua+", "+varSys+");"]

class TypeString(Type):
	def __init__(self, size):
		Type.__init__(self, "_dynamic_string<" + str(size) + ">")
		self.size = size
	def headers(self):
		return ["shared/system/string.h"]
	def getSetInterfaces(self):
		return [
			GetSetInterface_String()
		]
	def tuaType(self, version):
		return "tua_stringid"
	def generateWrite(self, varSys, varTua, version):
		return [varTua + " = pLoadingContext->ArchiveFile()->AddString(" + varSys + ".buffer());"]
	def generateRead(self, varSys, varTua, version):
		return [varSys + " = pLoadingContext->ArchiveFile()->GetString(" + varTua + ");"]

class TypeTextureHandle(Type):
	def __init__(self):
		Type.__init__(self, "CTextureHandle")
	def tuaType(self, version):
		return ""
	def getSetInterfaces(self):
		return [
			GetSetInterface_Simple("", self.tname, self.tname, self.tname, "CTextureHandle()"),
			GetSetInterface_Func("Id", "int", "int", "Id", "SetId", "-1")
		]
		
class GetSetInterface_ArrayChild(GetSetInterface):
	def __init__(self, suffix, interface, t):
		GetSetInterface.__init__(self, suffix)
		self.interface = interface
		self.subpath = 1
		self.t = t
		self.getSetType = interface.getSetType
		self.returnType = interface.returnType
		self.paramType = interface.paramType
		self.defaultValue = interface.defaultValue
		self.depth = interface.depth + 1
		self.enum = interface.enum
		self.name = interface.name
		self.getbyref = interface.getbyref
		self.noset = interface.noset
	def generateSet(self, var, value):
		res = []
		if self.interface.name:
			res.append("if(SubPath.GetId() < " + var + ".size())")
			if self.interface.subpath > 0:
				res.append("	" + var + "[SubPath.GetId()].Set" + self.interface.name + "(SubPath.PopId(), " + value + ");")
			else:
				res.append("	" + var + "[SubPath.GetId()].Set" + self.interface.name + "(" + value + ");")
		else:
			res.append("if(SubPath.GetId() < " + var + ".size())")
			res.append("{")
			for l in self.t.generateCopy(var + "[SubPath.GetId()]", value):
				res.append("	" + l)
			res.append("}")
		return res
	def generateGet(self, var, defaultValue):
		res = []
		if self.interface.name:
			if defaultValue:
				res.append("if(SubPath.GetId() < " + var + ".size())")
				if self.interface.subpath > 0:
					res.append("	return " + var + "[SubPath.GetId()].Get" + self.interface.name + "(SubPath.PopId());")
				else:
					res.append("	return " + var + "[SubPath.GetId()].Get" + self.interface.name + "();")
				res.append("else return " + defaultValue + ";")
			else:
				res.append("assert(SubPath.GetId() < " + var + ".size());")
				if self.interface.subpath > 0:
					res.append("return " + var + "[SubPath.GetId()].Get" + self.interface.name + "(SubPath.PopId());")
				else:
					res.append("return " + var + "[SubPath.GetId()].Get" + self.interface.name + "();")
		else:
			res.append("assert(SubPath.GetId() < " + var + ".size());")
			res.append("{")
			for l in self.interface.generateGet(var + "[SubPath.GetId()]", defaultValue):
				res.append("	" + l)
			res.append("}")
		return res
			
class AddInterface_Array(GetSetInterface):
	def __init__(self, constructor):
		self.name = ""
		self.subpath = 0
		self.enum = ""
		self.constructor = constructor
	def generateAdd(self, var):
		return [ 
			"int Id = "+var+".size();",
			var + ".emplace_back();",
			"return Id;"
		]
	def generateAddAt(self, var):
		return [ var + ".insert("+var+".begin() + Index, "+self.constructor+"());" ]
	def generateDelete(self, var):
		return [ var + ".erase("+var+".begin() + SubPath.GetId());" ]
	def generateRelMove(self, var):
		return [
			"int NewId = relative_move(" + var + ", SubPath.GetId(), RelMove);",
			"SubPath.SetId(NewId);"
		]
	def generateValid(self, var):
		return [ 
			"return (SubPath.IsNotNull() && SubPath.GetId() < "+var+".size());"
		]
			
class AddInterface_ArrayChild(GetSetInterface):
	def __init__(self, interface):
		self.interface = interface
		self.name = interface.name
		self.subpath = 1
		self.enum = interface.enum
	def generateAdd(self, var):
		return [ "return " + var + "[SubPath.GetId()].Add" + self.interface.name + "();" ]
	def generateAddAt(self, var):
		return [ var + "[SubPath.GetId()].AddAt" + self.interface.name + "(Index);" ]
	def generateDelete(self, var):
		return [ var + "[SubPath.GetId()].Delete" + self.interface.name + "(SubPath.PopId());" ]
	def generateRelMove(self, var):
		return [
			"CSubPath ChildSubPath = SubPath.PopId();",
			var + "[SubPath.GetId()].RelMove" + self.interface.name + "(ChildSubPath, RelMove);",
			"SubPath.SetId2(ChildSubPath.GetId());"
		]
	def generateValid(self, var):
		return [ "return (SubPath.IsNotNull() && SubPath.GetId() < "+var+".size() && " + var + "[SubPath.GetId()].IsValid" + self.interface.name + "(SubPath.PopId()));" ]
	
class TypeArray(Type):
	def __init__(self, t):
		Type.__init__(self, "std::vector<"+t.fullType()+">")
		self.t = t
	def numSubPathId(self):
		return 1
	def headers(self):
		return ["vector", "shared/tl/algorithm.h", "cassert"]
	def getSetInterfaces(self):
		res = [
			GetSetInterface_Func("ArraySize", "int", "int", "size", "resize", "0"),
			GetSetInterface_GetArray("Ptr", "const "+self.t.fullType()+"*", "NULL"),
			GetSetInterface_SimpleRef("Array", self.tname, self.tname)
		]
		for inter in self.t.getSetInterfaces():
			res.append(GetSetInterface_ArrayChild("", inter, self.t))
		return res
	def addInterfaces(self):
		res = [ AddInterface_Array(self.t.fullType()) ]
		for inter in self.t.addInterfaces():
			res.append(AddInterface_ArrayChild(inter))
		return res
	def subPathInterfaces(self):
		return [SubPathInterface()]
		
	def subPathTypes(self):
		res = []
		for spt in self.t.subPathTypes():
			res.append(SubPathType(spt.name, spt.enumname, spt.numidx))
		return res
	
	def tuaType(self, version):
		return "CTuaArray"
	def generateWrite(self, varSys, varTua, version):
		res = []
		res.append("{")
		res.append("	" + varTua + ".m_Size = "+varSys+".size();")
		res.append("	" + self.t.tuaType(version) + "* pData = new " + self.t.tuaType(version) + "["+varSys+".size()];")
		res.append("	mem_zero((void*) pData, sizeof(" + self.t.tuaType(version) + ")*"+varSys+".size());")
		res.append("	for(unsigned int i=0; i<"+varSys+".size(); i++)")
		res.append("	{")
		for l in self.t.generateWrite(varSys+"[i]", "pData[i]", version):
			res.append("		"+l)
		res.append("	}")
		res.append("	" + varTua + ".m_Data = pLoadingContext->ArchiveFile()->AddData((uint8*) pData, sizeof(" + self.t.tuaType(version) + ")*"+varSys+".size());")
		res.append("	delete[] pData;")
		res.append("}")
		return res
	def generateRead(self, varSys, varTua, version):
		res = []
		res.append("{")
		res.append("	const "+self.t.tuaType(version)+"* pData = (const "+self.t.tuaType(version)+"*) pLoadingContext->ArchiveFile()->GetData("+varTua+".m_Data);")
		res.append("	uint32 Size = pLoadingContext->ArchiveFile()->ReadUInt32("+varTua+".m_Size);")
		res.append("	"+varSys+".resize(Size);")
		res.append("	for(uint32 i=0; i<Size; i++)")
		res.append("	{")
		for l in self.t.generateRead(varSys+"[i]", "pData[i]", version):
			res.append("		"+l)
		res.append("	}")
		res.append("}")
		res.append("")
		return res
	def generateAssetPathOp(self, var, operation):
		if len(self.t.generateAssetPathOp(var+"[i]", operation)) > 0:
			res = []
			
			if self.t.fullType() == "CAssetPath":
				res.append("{")
				res.append("	int Shift = 0;")
				res.append("	for(unsigned int i=0; i<"+var+".size(); i++)")
				res.append("	{")
				res.append("		if("+operation+".MustBeDeleted("+var+"[i]))")
				res.append("			Shift++;")
				res.append("		else if(Shift > 0)")
				res.append("			"+var+"[i-Shift] = "+var+"[i];")
				res.append("	}")
				res.append("	"+var+".resize("+var+".size()-Shift);")
				res.append("}")
			
			res.append("for(unsigned int i=0; i<"+var+".size(); i++)")
			res.append("{")
			for l in self.t.generateAssetPathOp(var+"[i]", operation):
				res.append("	"+l)
			res.append("}")
			return res
		else:
			return []
	def generateIterators(self, name, var):
		return [
			"class CIterator"+name,
			"{",
			"protected:",
			"	int m_Index;",
			"	bool m_Reverse;",
			"public:",
			"	CIterator"+name+"() : m_Index(0), m_Reverse(false) {}",
			"	CIterator"+name+"(int Index, bool Reverse) : m_Index(Index), m_Reverse(Reverse) {}",
			"	CIterator"+name+"& operator++() { if(m_Reverse) m_Index--; else m_Index++; return *this; }",
			"	CSubPath operator*() { return SubPath_"+name+"(m_Index); }",
			"	bool operator==(const CIterator"+name+"& Iter2) { return Iter2.m_Index == m_Index; }",
			"	bool operator!=(const CIterator"+name+"& Iter2) { return Iter2.m_Index != m_Index; }",
			"};",
			"",
			"CIterator"+name+" Begin"+name+"() const { return CIterator"+name+"(0, false); }",
			"CIterator"+name+" End"+name+"() const { return CIterator"+name+"("+var+".size(), false); }",
			"CIterator"+name+" ReverseBegin"+name+"() const { return CIterator"+name+"("+var+".size()-1, true); }",
			"CIterator"+name+" ReverseEnd"+name+"() const { return CIterator"+name+"(-1, true); }",
			""
		]

class GetSetInterface_Array2dChild(GetSetInterface):
	def __init__(self, suffix, interface):
		GetSetInterface.__init__(self, suffix)
		self.interface = interface
		self.subpath = 1
		self.getSetType = interface.getSetType
		self.returnType = interface.returnType
		self.paramType = interface.paramType
		self.defaultValue = interface.defaultValue
		self.depth = interface.depth + 1
		self.enum = interface.enum
		self.name = interface.name
		self.getbyref = interface.getbyref
		self.noset = interface.noset
	def generateSet(self, var, value):
		if self.interface.name:
			if self.interface.subpath > 0:
				return [ var + ".get_clamp(SubPath.GetId(), SubPath.GetId2()).Set" + self.interface.name + "(SubPath.DoublePopId(), " + value + ");" ]
			else:
				return [ var + ".get_clamp(SubPath.GetId(), SubPath.GetId2()).Set" + self.interface.name + "(" + value + ");" ]
		else:
			return [ var + ".set_clamp(SubPath.GetId(), SubPath.GetId2(), " + value + ");" ]
	def generateGet(self, var, defaultValue):
		if self.interface.name:
			if self.interface.subpath > 0:
				return [ "return " + var + ".get_clamp(SubPath.GetId(), SubPath.GetId2()).Get" + self.interface.name + "(SubPath.DoublePopId());" ]
			else:
				return [ "return " + var + ".get_clamp(SubPath.GetId(), SubPath.GetId2()).Get" + self.interface.name + "();" ]
		else:
			return [ "return " + var + ".get_clamp(SubPath.GetId(), SubPath.GetId2());" ]
			
class AddInterface_Array2dChild(GetSetInterface):
	def __init__(self, interface):
		self.interface = interface
		self.name = interface.name
		self.subpath = 1
		self.enum = interface.enum
	def generateAdd(self, var):
		return [ "return " + var + ".get_clamp(SubPath.GetId(), SubPath.GetId2()).Add" + self.interface.name + "();" ]
	def generateAddAt(self, var):
		return [ var + ".get_clamp(SubPath.GetId(), SubPath.GetId2()).AddAt" + self.interface.name + "(Index);" ]
	def generateDelete(self, var):
		return [ var + ".get_clamp(SubPath.GetId(), SubPath.GetId2()).Delete" + self.interface.name + "(SubPath.DoublePopId());" ]
	def generateRelMove(self, var):
		return [
			"CSubPath ChildSubPath = SubPath.DoublePopId();",
			var + ".get_clamp(SubPath.GetId(), SubPath.GetId2()).RelMove" + self.interface.name + "(ChildSubPath, RelMove);",
			"SubPath.SetId3(ChildSubPath.GetId());"
		]
	def generateValid(self, var):
		return [ "return (SubPath.IsNotNull() && " + var + ".get_clamp(SubPath.GetId(), SubPath.GetId2()).IsValid" + self.interface.name + "(SubPath.DoublePopId()));" ]
		
class GetSetInterface_Array2dDim(GetSetInterface_Func):
	def __init__(self, suffix, returnType, paramType, getFunc, setFunc):
		GetSetInterface_Func.__init__(self, suffix, returnType, paramType, getFunc, setFunc, "0")
	def generateSet(self, var, value):
		return [ var + "." + self.setFunc + "(max(" + value + ", 1));" ]
		
class TypeArray2d(Type):
	def __init__(self, t):
		Type.__init__(self, "array2d< "+t.tname+" >")
		self.t = t
	def numSubPathId(self):
		return 2
	def headers(self):
		return ["shared/tl/array2d.h"]
	def getSetInterfaces(self):
		res = [
			GetSetInterface_Array2dDim("Width", "int", "int", "get_width", "resize_width"),
			GetSetInterface_Array2dDim("Height", "int", "int", "get_height", "resize_height"),
			GetSetInterface_Array2dDim("Depth", "int", "int", "get_depth", "resize_depth"),
			GetSetInterface_GetFunc("Ptr", "const "+self.t.fullType()+"*", "base_ptr", "NULL"),
			GetSetInterface_SimpleRef("Array", self.tname, self.tname)
		]
		for inter in self.t.getSetInterfaces():
			res.append(GetSetInterface_Array2dChild("", inter, ))
		return res
	def addInterfaces(self):
		res = []
		for inter in self.t.addInterfaces():
			res.append(AddInterface_Array2dChild(inter))
		return res
	def subPathInterfaces(self):
		return [SubPathInterface()]
	def tuaType(self, version):
		return "CTuaArray2d"
	def generateWrite(self, varSys, varTua, version):
		res = []
		res.append("{")
		res.append("	" + varTua + ".m_Width = pLoadingContext->ArchiveFile()->WriteUInt32("+varSys+".get_width());")
		res.append("	" + varTua + ".m_Height = pLoadingContext->ArchiveFile()->WriteUInt32("+varSys+".get_height());")
		res.append("	" + varTua + ".m_Depth = pLoadingContext->ArchiveFile()->WriteUInt32("+varSys+".get_depth());")
		if self.t.fullType() == "uint8":
			res.append("	" + varTua + ".m_Data = pLoadingContext->ArchiveFile()->AddData((tua_uint8*) "+varSys+".base_ptr(), "+varSys+".get_linear_size());")
		else:
			res.append("	" + self.t.tuaType(version) + "* pData = new " + self.t.tuaType(version) + "["+varSys+".get_linear_size()];")
			res.append("	mem_zero((void*) pData, sizeof(" + self.t.tuaType(version) + ")*"+varSys+".get_linear_size());")
			res.append("	for(int i=0; i<"+varSys+".get_linear_size(); i++)")
			res.append("	{")
			for l in self.t.generateWrite(varSys+".linear_get_clamp(i)", "pData[i]", version):
				res.append("		"+l)
			res.append("	}")
			res.append("	" + varTua + ".m_Data = pLoadingContext->ArchiveFile()->AddData((tua_uint8*) pData, sizeof(" + self.t.tuaType(version) + ")*"+varSys+".get_linear_size());")
			res.append("	delete[] pData;")
		res.append("}")
		return res
	def generateRead(self, varSys, varTua, version):
		res = []
		res.append("{")
		res.append("	const "+self.t.tuaType(version)+"* pData = (const "+self.t.tuaType(version)+"*) pLoadingContext->ArchiveFile()->GetData("+varTua+".m_Data);")
		res.append("	uint32 Width = pLoadingContext->ArchiveFile()->ReadUInt32("+varTua+".m_Width);")
		res.append("	uint32 Height = pLoadingContext->ArchiveFile()->ReadUInt32("+varTua+".m_Height);")
		res.append("	uint32 Depth = pLoadingContext->ArchiveFile()->ReadUInt32("+varTua+".m_Depth);")
		res.append("	"+varSys+".resize(Width, Height, Depth);")
		if self.t.fullType() == "uint8":
			res.append("	mem_copy((uint8*) "+varSys+".base_ptr(), pData, "+varSys+".get_linear_size());")
		else:
			res.append("	int Size = Width * Height * Depth;")
			res.append("	for(int i=0; i<Size; i++)")
			res.append("	{")
			res.append("		"+self.t.tname+" ReadedValue;" )
			for l in self.t.generateRead("ReadedValue", "pData[i]", version):
				res.append("		"+l)
			res.append("		"+varSys+".linear_set_clamp(i, ReadedValue);")
			res.append("	}")
		res.append("}")
		res.append("")
		return res
	def generateAssetPathOp(self, var, operation):
		if len(self.t.generateAssetPathOp(var+"[i]", operation)) > 0:
			res = []
			res.append("for(int i=0; i<"+var+".get_linear_size(); i++)")
			res.append("{")
			for l in self.t.generateAssetPathOp(var+".linear_get_clamp(i)", operation):
				res.append("	"+l)
			res.append("}")
			return res
		else:
			return []

class GetSetInterface_Member(GetSetInterface):
	def __init__(self, m, interface):
		GetSetInterface.__init__(self, m.name)
		self.interface = interface
		self.getSetType = interface.getSetType
		self.returnType = interface.returnType
		self.paramType = interface.paramType
		self.defaultValue = interface.defaultValue
		self.depth = interface.depth
		self.subpath = interface.subpath
		self.noset = interface.noset
		self.getbyref = interface.getbyref
		if interface.enum:
			self.enum = self.enum + "_" + interface.enum
		if interface.enum:
			self.name = self.name + interface.name
			
class AddInterface_Member(GetSetInterface):
	def __init__(self, m, interface):
		self.subpath = interface.subpath
		self.interface = interface
		if interface.enum:
			self.enum = m.name.upper() + "_" + interface.enum
		else: 
			self.enum = m.name.upper()
		if interface.name:
			self.name = m.name + interface.name
		else: 
			self.name = m.name
	def generateAdd(self, var):
		return [ "return " + var + ".Add" + self.interface.name + "();" ]
	def generateAddAt(self, var):
		return [ var + ".AddAt" + self.interface.name + "(Index);" ]
	def generateDelete(self, var):
		return [ var + ".Delete" + self.interface.name + "(SubPath);" ]
	def generateRelMove(self, var):
		return [ var + ".RelMove" + self.interface.name + "(SubPath, RelMove);" ]
	def generateValid(self, var):
		return [ "return SubPath.IsNotNull() && " + var + ".IsValid" + self.interface.name + "();" ]
		
class Member:
	def __init__(self, version, name, t):
		self.version = version
		self.name = name
		self.t = t
	def memberName(self):
		return "m_" + self.name
		
	def headers(self):
		return self.t.headers()
	def generateReturn(self):
		return self.t.generateReturn(self.memberName())
	def generateDeclaration(self):
		return self.t.generateDeclaration(self.memberName())
	def generateTuaDeclaration(self, version):
		return self.t.generateTuaDeclaration(self.memberName(), version)
	def generateCopy(self, value):
		return self.t.generateCopy(self.memberName(), value)
	def generateTransfert(self, value):
		return self.t.generateTransfert(self.memberName(), value)
	def generateStorage(self, var):
		return self.t.generateStorage(self.memberName())
	def generateWrite(self, version):
		if self.t.tuaType(version):
			return self.t.generateWrite("SysType."+self.memberName(), "TuaType."+self.memberName(), version)
		else:
			return []
	def generateRead(self, version):
		if self.t.tuaType(version):
			return self.t.generateRead("SysType."+self.memberName(), "TuaType."+self.memberName(), version)
		else:
			return []
	def generateAssetPathOp(self, operation):
		return self.t.generateAssetPathOp(self.memberName(), operation)
		
	def generateGetSetEnum(self):
		res = []
		memberEnum = 0
		for i in self.t.getSetInterfaces():
			if i.enum:
				res.append(self.name.upper()+"_"+i.enum)
			else:
				memberEnum = memberEnum+1
		if memberEnum:
			res.append(self.name.upper())
		return res
	def generateGet(self):
		res = []
		for i in self.t.getSetInterfaces():
			params = ""
			if i.subpath > 0:
				params = "const CSubPath& SubPath"
				
			if i.getbyref:
				if len(i.generateGet(self.memberName(), i.defaultValue)) > 1:
					res.append("inline const "+i.returnType+"& Get"+self.name+i.name+"("+params+") const")
					res.append("{")
					for l in i.generateGet(self.memberName(), i.defaultValue):
						res.append("	"+l)
					res.append("}")
					res.append("inline "+i.returnType+"& Get"+self.name+i.name+"("+params+")")
					res.append("{")
					for l in i.generateGet(self.memberName(), i.defaultValue):
						res.append("	"+l)
					res.append("}")
				else:
					res.append("inline const "+i.returnType+"& Get"+self.name+i.name+"("+params+") const { "+i.generateGet(self.memberName(), i.defaultValue)[0]+" }")
					res.append("inline "+i.returnType+"& Get"+self.name+i.name+"("+params+") { "+i.generateGet(self.memberName(), i.defaultValue)[0]+" }")
			else:
				if len(i.generateGet(self.memberName(), i.defaultValue)) > 1:
					res.append("inline "+i.returnType+" Get"+self.name+i.name+"("+params+") const")
					res.append("{")
					for l in i.generateGet(self.memberName(), i.defaultValue):
						res.append("	"+l)
					res.append("}")
				else:
					res.append("inline "+i.returnType+" Get"+self.name+i.name+"("+params+") const { "+i.generateGet(self.memberName(), i.defaultValue)[0]+" }")	
			
			res.append("")
		return res
	def generateSet(self):
		res = []
		for i in self.t.getSetInterfaces():
			if i.noset == 0:
				params = i.paramType+" Value"
				if i.subpath > 0:
					params = "const CSubPath& SubPath, " + params
				if len(i.generateSet(self.memberName(), "Value")) > 1:
					res.append("inline void Set"+self.name+i.name+"("+params+")")
					res.append("{")
					for l in i.generateSet(self.memberName(), "Value"):
						res.append("	"+l)
					res.append("}")
				else:
					res.append("inline void Set"+self.name+i.name+"("+params+") { "+i.generateSet(self.memberName(), "Value")[0]+" }")
				res.append("")
		return res
	def generateAdd(self):
		res = []
		for i in self.t.addInterfaces():
			params = ""
			if i.subpath:
				params = "const CSubPath& SubPath"
				
			if len(i.generateAdd(self.memberName())) > 1:
				res.append("inline int Add"+self.name+i.name+"("+params+")")
				res.append("{")
				for l in i.generateAdd(self.memberName()):
					res.append("	"+l)
				res.append("}")
			else:
				res.append("inline int Add"+self.name+i.name+"("+params+") { "+i.generateAdd(self.memberName())[0]+" }")
			
			res.append("")
		return res
	def generateAddAt(self):
		res = []
		for i in self.t.addInterfaces():
			params = "int Index"
			if i.subpath:
				params = "const CSubPath& SubPath, " + params
				
			if len(i.generateAddAt(self.memberName())) > 1:
				res.append("inline void AddAt"+self.name+i.name+"("+params+")")
				res.append("{")
				for l in i.generateAddAt(self.memberName()):
					res.append("	"+l)
				res.append("}")
			else:
				res.append("inline void AddAt"+self.name+i.name+"("+params+") { "+i.generateAddAt(self.memberName())[0]+" }")
			
			res.append("")
		return res
	def generateDelete(self):
		res = []
		for i in self.t.addInterfaces():
			
			if len(i.generateDelete(self.memberName())) > 1:
				res.append("inline void Delete"+self.name+i.name+"(const CSubPath& SubPath)")
				res.append("{")
				for l in i.generateDelete(self.memberName()):
					res.append("	"+l)
				res.append("}")
			else:
				res.append("inline void Delete"+self.name+i.name+"(const CSubPath& SubPath) { "+i.generateDelete(self.memberName())[0]+" }")
				
			res.append("")
		return res
	def generateRelMove(self):
		res = []
		for i in self.t.addInterfaces():
			
			if len(i.generateRelMove(self.memberName())) > 1:
				res.append("inline void RelMove"+self.name+i.name+"(CSubPath& SubPath, int RelMove)")
				res.append("{")
				for l in i.generateRelMove(self.memberName()):
					res.append("	"+l)
				res.append("}")
			else:
				res.append("inline void RelMove"+self.name+i.name+"(CSubPath& SubPath, int RelMove) { "+i.generateRelMove(self.memberName())[0]+" }")
				
			res.append("")
		return res
	def generateValid(self):
		res = []
		for i in self.t.addInterfaces():
			params = ""
			if i.subpath:
				params = ""
			if len(i.generateValid(self.memberName())) > 1:
				res.append("inline bool IsValid"+self.name+i.name+"(const CSubPath& SubPath) const")
				res.append("{")
				for l in i.generateValid(self.memberName()):
					res.append("	"+l)
				res.append("}")
			else:
				res.append("inline bool IsValid"+self.name+i.name+"(const CSubPath& SubPath) const { "+i.generateValid(self.memberName())[0]+" }")
			res.append("")
		return res
	def getSetInterfaces(self):
		res = []
		for inter in self.t.getSetInterfaces():
			res.append(GetSetInterface_Member(self, inter))
		return res
	def addInterfaces(self):
		res = []
		for inter in self.t.addInterfaces():
			res.append(AddInterface_Member(self, inter))
		return res
		
	def subPathTypes(self):
		res = []
		for spt in self.t.subPathTypes():
			res.append(SubPathType(self.name + spt.name, self.name.upper() + "_" + spt.enumname, spt.numidx + self.t.numSubPathId()))
		if self.t.numSubPathId():
			res.append(SubPathType(self.name, self.name.upper(), self.t.numSubPathId()))
		return res
		
	def generateIterators(self):
		if len(self.t.generateIterators(self.name, self.memberName())) > 0:
			return self.t.generateIterators(self.name, self.memberName())
		else:
			return []

class GetSetInterface_Class(GetSetInterface):
	def __init__(self, t):
		GetSetInterface.__init__(self, "")
		self.getSetType = "const "+t.fullType()+"&"
		self.returnType = "const "+t.fullType()+"&"
		self.paramType = "const "+t.fullType()+"&"
		self.defaultValue = ""
		self.subpath = 1
		self.t = t
	def generateSet(self, var, value):
		return self.t.generateCopy(var, value)
	def generateGet(self, var, defaultValue):
		return [ "return " + var + ";"]

class Class(Type):
	def __init__(self, name):
		Type.__init__(self, "C"+name)
		self.name = name
		self.members = []
		self.classes = []
		self.privateLines = []
		self.publicLines = []
		self.publicFunc = []
		self.constructor = []
		self.inheritance = None
		self.parent = None
		self.externalheaders = []
		
	def fullType(self):
		tname = self.tname
		p = self.parent
		while p:
			tname = p.tname + "::" + tname
			p = p.parent
		return tname
	def addHeader(self, h):
		self.externalheaders.append(h)
	def headers(self):
		res = self.externalheaders
		for Mem in self.members:
			res.extend(Mem.headers());
		return res
	def getSetInterfaces(self):
		res = [ GetSetInterface_Class(self) ]
		if self.inheritance:
			for m in self.inheritance.members:
				res.extend(m.getSetInterfaces())
		for m in self.members:
			res.extend(m.getSetInterfaces())
		return res
	def addInterfaces(self):
		res = []
		for m in self.members:
			res.extend(m.addInterfaces())
		return res
		
	def subPathTypes(self):
		res = []
		for m in self.members:
			for spt in m.subPathTypes():
				res.append(SubPathType(spt.name, spt.enumname, spt.numidx))
		return res
	
	def tuaType(self, version):
		return self.fullType()+"::CTuaType_"+version.replace(".", "_")
	def generateWrite(self, varSys, varTua, version):
		return [self.fullType()+"::CTuaType_"+version.replace(".", "_")+"::Write(pLoadingContext, "+varSys+", "+varTua+");"]
	def generateRead(self, varSys, varTua, version):
		return [self.fullType()+"::CTuaType_"+version.replace(".", "_")+"::Read(pLoadingContext, "+varTua+", "+varSys+");"]
	def generateTuaType(self):
		res = []
		
		acceptedVersion = []
		for v in versionList:
			acceptedVersion.append(v)
			vName = v.replace(".", "_")
			
			if self.inheritance:
				res.append("class CTuaType_"+vName+" : public "+self.inheritance.fullType()+"::CTuaType_"+vName+"")
			else:
				res.append("class CTuaType_"+vName)
			
			res.append("{")
			res.append("public:")
			for Mem in self.members:
				if Mem.version in acceptedVersion:
					for l in Mem.generateTuaDeclaration(v):
						res.append("	"+l)
			res.append("	static void Read(class CAssetsSaveLoadContext* pLoadingContext, const CTuaType_"+vName+"& TuaType, "+self.fullType()+"& SysType);")
			res.append("	static void Write(class CAssetsSaveLoadContext* pLoadingContext, const "+self.fullType()+"& SysType, CTuaType_"+vName+"& TuaType);")
			res.append("};")
			res.append("")
		
		return res
	def generateReadImpl(self, acceptedVersion):
		res = []
		
		vName = acceptedVersion[-1].replace(".", "_")
		ns = self.fullType()+"::CTuaType_"+vName+"::"
		
		for c in self.classes:
			res.extend(c.generateReadImpl(acceptedVersion))
			res.append("")
			
		res.append("void "+ns+"Read(CAssetsSaveLoadContext* pLoadingContext, const CTuaType_"+vName+"& TuaType, "+self.fullType()+"& SysType)")
		res.append("{")
		if self.inheritance:
			res.append("	"+self.inheritance.fullType()+"::CTuaType_"+vName+"::Read(pLoadingContext, TuaType, SysType);")
			res.append("")
		for Mem in self.members:
			if Mem.version in acceptedVersion:
				for l in Mem.generateRead(acceptedVersion[-1]):
					res.append("	"+l)
		res.append("}")
		res.append("")
			
		return res
	
	def generateWriteImpl(self, acceptedVersion):
		res = []
		
		vName = acceptedVersion[-1].replace(".", "_")
		ns = self.fullType()+"::CTuaType_"+vName+"::"
		
		for c in self.classes:
			res.extend(c.generateWriteImpl(acceptedVersion))
			res.append("")
			
		res.append("void "+ns+"Write(CAssetsSaveLoadContext* pLoadingContext, const "+self.fullType()+"& SysType, CTuaType_"+vName+"& TuaType)")
		res.append("{")
		if self.inheritance:
			res.append("	"+self.inheritance.fullType()+"::CTuaType_"+vName+"::Write(pLoadingContext, SysType, TuaType);")
			res.append("")
		for Mem in self.members:
			if Mem.version in acceptedVersion:
				for l in Mem.generateWrite(acceptedVersion[-1]):
					res.append("	"+l)
		res.append("}")
			
		return res
		
	def generateAssetPathOp(self, var, operation):
		return [var+".AssetPathOperation("+operation+");"]
		
	def setParent(self, parent):
		self.parent = parent
	def setInheritance(self, inheritance):
		self.inheritance = inheritance
	def addMember(self, version, memberName, t, defaultValue=None):
		Mem = Member(version, memberName, t)
		self.members.append(Mem)
		if defaultValue:
			self.constructor.extend(t.generateCopy(Mem.memberName(), defaultValue))
	def addClass(self, c):
		c.setParent(self)
		self.classes.append(c)
	def addPrivateLines(self, lines):
		self.privateLines.extend(lines)
	def addPublicLines(self, lines):
		self.publicLines.extend(lines)
	def addPublicFunc(self, lines):
		self.publicFunc.extend(lines)
	
	def generateConstructor(self):
		res = []
		
		for c in self.classes:
			res2 = c.generateConstructor()
			if len(res2) > 0:
				res.extend(res2)
				res.append("")
		
		if len(self.constructor) > 0:
			p = self.parent
			storageType = self.fullType()
			ns = self.fullType()
			
			res.append(""+ns+"::"+self.tname+"()")
			res.append("{")
			for l in self.constructor:
				res.append("	"+l)
			res.append("}")
			
		return res
	
	def generateClassDefinition(self):		
		res = []
		
		if self.inheritance:
			res.append("class "+self.tname+" : public "+self.inheritance.fullType())
		else:
			res.append("class "+self.tname)
		
		res.append("{")
		
		#Private lines
		if self.privateLines:
			res.append("private:")
			for l in self.privateLines:
				res.append("	"+l)
			res.append("")
		
		#Public lines
		for c in self.classes:
			self.addPublicLines(c.generateClassDefinition())
		
		self.addPublicLines(self.generateTuaType())
		if self.publicLines:
			res.append("public:")
			for l in self.publicLines:
				res.append("	"+l)
			res.append("")
		
		#Members
		res.append("private:")
		for Mem in self.members:
			for l in Mem.generateDeclaration():
				res.append("	"+l)
		res.append("")
		
		#Public func
		if len(self.constructor) > 0:
			self.addPublicFunc([self.tname+"();"])
		
		for Mem in self.members:
			self.addPublicFunc(Mem.generateGet())
		
		for Mem in self.members:
			self.addPublicFunc(Mem.generateSet())
		
		for Mem in self.members:
			self.addPublicFunc(Mem.generateAdd())
		
		for Mem in self.members:
			self.addPublicFunc(Mem.generateAddAt())
		
		for Mem in self.members:
			self.addPublicFunc(Mem.generateDelete())
		
		for Mem in self.members:
			self.addPublicFunc(Mem.generateRelMove())
		
		for Mem in self.members:
			self.addPublicFunc(Mem.generateValid())
		
		assetPathOperation = [
			"void AssetPathOperation(const CAssetPath::COperation& Operation)",
			"{"
		]
		for Mem in self.members:
			for l in Mem.generateAssetPathOp("Operation"):
				assetPathOperation.append("	"+l)
		assetPathOperation.append("}")
		assetPathOperation.append("")
		self.addPublicFunc(assetPathOperation)
		
		if self.publicFunc:
			res.append("public:")
			for l in self.publicFunc:
				res.append("	"+l)
		
		res.append("};")
		
		return res

class ClassAsset(Class):
	def __init__(self, name, typeid):
		Class.__init__(self, name)
		self.subpathSources = []
		self.typeid = typeid
		self.tname = "CAsset_" + self.name		
	def generateGetSetEnum(self):
		res = [
			"enum",
			"{",
			"	NAME = CAsset::NAME,"
		]
		for Mem in self.members:
			for e in Mem.generateGetSetEnum():
				res.append("	"+e+",")
		res.append("};")
		res.append("")
		return res
	
	def generateLoadSaveDecl(self):
		return [
			"void Load_AssetsFile(class CAssetsSaveLoadContext* pLoadingContext, const CStorageType* pItem);",
			"void Save_AssetsFile(class CAssetsSaveLoadContext* pLoadingContext, CStorageType* pItem);"
		]
	
	def generateSubPath(self):
		types = []
		for Mem in self.members:
			types.extend(Mem.subPathTypes())
		res = []
		if types:
			res.append("enum")
			res.append("{")
			for t in types:
				res.append("	TYPE_"+t.enumname+",")	
			res.append("};")
			res.append("")
			for t in types:
				if t.numidx == 1:
					res.append("static inline CSubPath SubPath_"+t.name+"(int Id0) { return CSubPath(TYPE_"+t.enumname+", Id0, 0, 0); }")
				elif t.numidx == 2:
					res.append("static inline CSubPath SubPath_"+t.name+"(int Id0, int Id1) { return CSubPath(TYPE_"+t.enumname+", Id0, Id1, 0); }")
				else:
					res.append("static inline CSubPath SubPath_"+t.name+"(int Id0, int Id1, int Id2) { return CSubPath(TYPE_"+t.enumname+", Id0, Id1, Id2); }")
			res.append("")
		return res
	
	def generateGetSpe(self, t):
		counter = 0
		for Mem in self.members: 
			for inter in Mem.getSetInterfaces():
				if inter.getSetType == t:
					counter = counter+1
		
		if counter > 0:
			return [ "template<> "+t+" "+self.fullType()+"::GetValue(int ValueType, const CSubPath& SubPath, "+t+" DefaultValue) const;" ]
		else:
			return []
	
	def generateGetImpl(self, t):
		counter = 0
		for Mem in self.members: 
			for inter in Mem.getSetInterfaces():
				if inter.getSetType == t:
					counter = counter+1
		
		if counter > 0:
			res = []
			res.append("template<>")
			res.append(t+" "+self.fullType()+"::GetValue(int ValueType, const CSubPath& SubPath, "+t+" DefaultValue) const")
			res.append("{")
			res.append("	switch(ValueType)")
			res.append("	{")
			
			for Mem in self.members: 
				for inter in Mem.getSetInterfaces():
					if inter.getSetType == t:
						res.append("		case "+inter.enum+":")
						if inter.subpath:
							res.append("			return Get"+inter.name+"(SubPath);")
						else:
							res.append("			return Get"+inter.name+"();")
			
			res.append("	}")
			res.append("	return CAsset::GetValue<"+t+">(ValueType, SubPath, DefaultValue);")
			
			res.append("}")
			res.append("")
			return res
		else:
			return []
	
	def generateSetSpe(self, t):
		counter = 0
		for Mem in self.members: 
			for inter in Mem.getSetInterfaces():
				if inter.getSetType == t:
					counter = counter+1
		
		if counter > 0:
			return [ "template<> bool "+self.fullType()+"::SetValue(int ValueType, const CSubPath& SubPath, "+t+" Value);" ]
		else:
			return []
	
	def generateSetImpl(self, t):
		counter = 0
		for Mem in self.members: 
			for inter in Mem.getSetInterfaces():
				if inter.getSetType == t:
					counter = counter+1
		
		if counter > 0:
			res = []
			res.append("template<>")
			res.append("bool "+self.fullType()+"::SetValue(int ValueType, const CSubPath& SubPath, "+t+" Value)")
			res.append("{")
			res.append("	switch(ValueType)")
			res.append("	{")
			
			for Mem in self.members: 
				for inter in Mem.getSetInterfaces():
					if inter.getSetType == t:
						res.append("		case "+inter.enum+":")
						if inter.subpath:
							res.append("			Set"+inter.name+"(SubPath, Value);")
							res.append("			return true;")
						else:
							res.append("			Set"+inter.name+"(Value);")
							res.append("			return true;")
			
			res.append("	}")
			res.append("	return CAsset::SetValue<"+t+">(ValueType, SubPath, Value);")
			res.append("}")
			res.append("")
			return res
		else:
			return []
	
	def generateAddImpl(self):
		counter = 0
		for Mem in self.members: 
			for inter in Mem.addInterfaces():
				counter = counter+1
		
		res = []
		res.append("int "+self.fullType()+"::AddSubItem(int Type, const CSubPath& SubPath)")
		res.append("{")
		if counter > 0:
			res.append("	switch(Type)")
			res.append("	{")
			
			for Mem in self.members: 
				for inter in Mem.addInterfaces():
					res.append("		case TYPE_"+inter.enum+":")
					if inter.subpath:
						res.append("			return Add"+inter.name+"(SubPath);")
					else:
						res.append("			return Add"+inter.name+"();")
			res.append("	}")		
		res.append("	return -1;")
		res.append("}")
		res.append("")
		return res
	
	def generateAddAtImpl(self):
		counter = 0
		for Mem in self.members: 
			for inter in Mem.addInterfaces():
				counter = counter+1
		
		res = []
		res.append("int "+self.fullType()+"::AddSubItemAt(int Type, const CSubPath& SubPath, int Index)")
		res.append("{")
		if counter > 0:
			res.append("	switch(Type)")
			res.append("	{")
			
			for Mem in self.members: 
				for inter in Mem.addInterfaces():
					res.append("		case TYPE_"+inter.enum+":")
					if inter.subpath:
						res.append("			AddAt"+inter.name+"(SubPath, Index);")
						res.append("			return Index;")
					else:
						res.append("			AddAt"+inter.name+"(Index);")
						res.append("			return Index;")
			res.append("	}")		
		res.append("	return -1;")
		res.append("}")
		res.append("")
		return res
	
	def generateDeleteImpl(self):
		counter = 0
		for Mem in self.members: 
			for inter in Mem.addInterfaces():
				counter = counter+1
		
		res = []
		res.append("void "+self.fullType()+"::DeleteSubItem(const CSubPath& SubPath)")
		res.append("{")
		if counter > 0:
			res.append("	switch(SubPath.GetType())")
			res.append("	{")
			
			for Mem in self.members: 
				for inter in Mem.addInterfaces():
					res.append("		case TYPE_"+inter.enum+":")
					res.append("			Delete"+inter.name+"(SubPath);")
					res.append("			break;")
			res.append("	}")
		res.append("}")
		res.append("")
		return res
	
	def generateRelMoveImpl(self):
		counter = 0
		for Mem in self.members: 
			for inter in Mem.addInterfaces():
				counter = counter+1
		
		res = []
		res.append("void "+self.fullType()+"::RelMoveSubItem(CSubPath& SubPath, int RelMove)")
		res.append("{")
		if counter > 0:
			res.append("	switch(SubPath.GetType())")
			res.append("	{")
			
			for Mem in self.members: 
				for inter in Mem.addInterfaces():
					res.append("		case TYPE_"+inter.enum+":")
					res.append("			RelMove"+inter.name+"(SubPath, RelMove);")
					res.append("			break;")
			res.append("	}")
		res.append("}")
		res.append("")
		return res
	
	def generateClassDefinition(self):
		self.addPublicLines([
			"static const int TypeId = "+str(self.typeid)+";",
			""
		])
		self.addPublicLines(self.generateSubPath())
		self.addPublicLines(self.generateGetSetEnum())
		for Mem in self.members:
			self.addPublicLines(Mem.generateIterators())
		
		self.addPublicFunc(["virtual ~"+self.tname+"() {}", ""])
		
		self.addPublicFunc([
			"template<typename T>",
			"T GetValue(int ValueType, const CSubPath& SubPath, T DefaultValue) const",
			"{",
			"	return CAsset::GetValue<T>(ValueType, SubPath, DefaultValue);",
			"}",
			"",
			"template<typename T>",
			"bool SetValue(int ValueType, const CSubPath& SubPath, T Value)",
			"{",
			"	return CAsset::SetValue<T>(ValueType, SubPath, Value);",
			"}",
			""
		])
		
		counter = 0
		for Mem in self.members: 
			for inter in Mem.addInterfaces():
				counter = counter+1
		
		self.addPublicFunc(["int AddSubItem(int Type, const CSubPath& SubPath);", ""])
		self.addPublicFunc(["int AddSubItemAt(int Type, const CSubPath& SubPath, int Index);", ""])
		self.addPublicFunc(["void DeleteSubItem(const CSubPath& SubPath);", ""])
		self.addPublicFunc(["void RelMoveSubItem(CSubPath& SubPath, int RelMove);", ""])
		
		return Class.generateClassDefinition(self)

def generateHeader(asset):
	f = file("src/generated/assets/" + asset.name.lower() + ".h", "w")
	print >>f, "/*"
	print >>f, " * Copyright (C) 2016 necropotame (necropotame@gmail.com)"
	print >>f, " * "
	print >>f, " * This file is part of TeeUniverse."
	print >>f, " * "
	print >>f, " * TeeUniverse is free software: you can redistribute it and/or  modify"
	print >>f, " * it under the terms of the GNU Affero General Public License, version 3,"
	print >>f, " * as published by the Free Software Foundation."
	print >>f, " *"
	print >>f, " * TeeUniverse is distributed in the hope that it will be useful,"
	print >>f, " * but WITHOUT ANY WARRANTY; without even the implied warranty of"
	print >>f, " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
	print >>f, " * GNU Affero General Public License for more details."
	print >>f, " *"
	print >>f, " * You should have received a copy of the GNU Affero General Public License"
	print >>f, " * along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>."
	print >>f, " */"
	print >>f, ""
	print >>f, "/*"
	print >>f, " * THIS FILE HAS BEEN GENERATED BY A SCRIPT."
	print >>f, " * DO NOT EDIT MANUALLY!"
	print >>f, " *"
	print >>f, " * Please use the script \"scripts/assets/assets.py\" to regenerate it."
	print >>f, " *"
	print >>f, " * Why this file is generated by a script?"
	print >>f, " * Because there is more than 10 files that follow the same format."
	print >>f, " * In addition, each time a new member is added, enums, getter, setters,"
	print >>f, " * copy/transfert functions and storage structure must be updated."
	print >>f, " * It's too much to avoid mistakes."
	print >>f, " */"
	print >>f, ""
	print >>f, "#ifndef __CLIENT_ASSETS_" + asset.name.upper() + "__"
	print >>f, "#define __CLIENT_ASSETS_" + asset.name.upper() + "__"
	print >>f, ""
	print >>f, "#include <shared/assets/asset.h>"
	
	for h in list(set(asset.headers())):
		print >>f, "#include <"+h+">"
	
	print >>f, ""
	print >>f, "\n".join(asset.generateClassDefinition())
	print >>f, ""
	for l in asset.generateGetSpe("int"):
		print >>f, l
	for l in asset.generateSetSpe("int"):
		print >>f, l
	for l in asset.generateGetSpe("uint32"):
		print >>f, l
	for l in asset.generateSetSpe("uint32"):
		print >>f, l
	for l in asset.generateGetSpe("int64"):
		print >>f, l
	for l in asset.generateSetSpe("int64"):
		print >>f, l
	for l in asset.generateGetSpe("bool"):
		print >>f, l
	for l in asset.generateSetSpe("bool"):
		print >>f, l
	for l in asset.generateGetSpe("const char*"):
		print >>f, l
	for l in asset.generateSetSpe("const char*"):
		print >>f, l
	for l in asset.generateGetSpe("float"):
		print >>f, l
	for l in asset.generateSetSpe("float"):
		print >>f, l
	for l in asset.generateGetSpe("double"):
		print >>f, l
	for l in asset.generateSetSpe("double"):
		print >>f, l
	for l in asset.generateGetSpe("vec2"):
		print >>f, l
	for l in asset.generateSetSpe("vec2"):
		print >>f, l
	for l in asset.generateGetSpe("vec4"):
		print >>f, l
	for l in asset.generateSetSpe("vec4"):
		print >>f, l
	for l in asset.generateGetSpe("CAssetPath"):
		print >>f, l
	for l in asset.generateSetSpe("CAssetPath"):
		print >>f, l
	for l in asset.generateGetSpe("CSubPath"):
		print >>f, l
	for l in asset.generateSetSpe("CSubPath"):
		print >>f, l
	print >>f, ""
	print >>f, "#endif"
	
def generateImpl(asset):
	f = file("src/generated/assets/" + asset.name.lower() + ".cpp", "w")
	print >>f, "/*"
	print >>f, " * Copyright (C) 2016 necropotame (necropotame@gmail.com)"
	print >>f, " * "
	print >>f, " * This file is part of TeeUniverse."
	print >>f, " * "
	print >>f, " * TeeUniverse is free software: you can redistribute it and/or  modify"
	print >>f, " * it under the terms of the GNU Affero General Public License, version 3,"
	print >>f, " * as published by the Free Software Foundation."
	print >>f, " *"
	print >>f, " * TeeUniverse is distributed in the hope that it will be useful,"
	print >>f, " * but WITHOUT ANY WARRANTY; without even the implied warranty of"
	print >>f, " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
	print >>f, " * GNU Affero General Public License for more details."
	print >>f, " *"
	print >>f, " * You should have received a copy of the GNU Affero General Public License"
	print >>f, " * along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>."
	print >>f, " */"
	print >>f, ""
	print >>f, "/*"
	print >>f, " * THIS FILE HAS BEEN GENERATED BY A SCRIPT."
	print >>f, " * DO NOT EDIT MANUALLY!"
	print >>f, " *"
	print >>f, " * Please use the script \"scripts/assets/assets.py\" to regenerate it."
	print >>f, " *"
	print >>f, " * Why this file is generated by a script?"
	print >>f, " * Because there is more than 10 files that follow the same format."
	print >>f, " * In addition, each time a new member is added, enums, getter, setters,"
	print >>f, " * copy/transfert functions and storage structure must be updated."
	print >>f, " * It's too much to avoid mistakes."
	print >>f, " */"
	print >>f, ""
	print >>f, "#include <generated/assets/"+asset.name.lower()+".h>"
	print >>f, "#include <shared/assets/assetssaveloadcontext.h>"
	print >>f, "#include <shared/archivefile.h>"
	print >>f, ""
	for l in asset.generateConstructor():
		print >>f, l
	print >>f, ""
	lAcceptedVersion = []
	for v in versionList:
		lAcceptedVersion.append(v)
		for l in asset.generateReadImpl(lAcceptedVersion):
			print >>f, l
		print >>f, ""
		for l in asset.generateWriteImpl(lAcceptedVersion):
			print >>f, l
		print >>f, ""
	for l in asset.generateGetImpl("int"):
		print >>f, l
	for l in asset.generateSetImpl("int"):
		print >>f, l
	for l in asset.generateGetImpl("uint32"):
		print >>f, l
	for l in asset.generateSetImpl("uint32"):
		print >>f, l
	for l in asset.generateGetImpl("int64"):
		print >>f, l
	for l in asset.generateSetImpl("int64"):
		print >>f, l
	for l in asset.generateGetImpl("bool"):
		print >>f, l
	for l in asset.generateSetImpl("bool"):
		print >>f, l
	for l in asset.generateGetImpl("const char*"):
		print >>f, l
	for l in asset.generateSetImpl("const char*"):
		print >>f, l
	for l in asset.generateGetImpl("float"):
		print >>f, l
	for l in asset.generateSetImpl("float"):
		print >>f, l
	for l in asset.generateGetImpl("double"):
		print >>f, l
	for l in asset.generateSetImpl("double"):
		print >>f, l
	for l in asset.generateGetImpl("vec2"):
		print >>f, l
	for l in asset.generateSetImpl("vec2"):
		print >>f, l
	for l in asset.generateGetImpl("vec4"):
		print >>f, l
	for l in asset.generateSetImpl("vec4"):
		print >>f, l
	for l in asset.generateGetImpl("CAssetPath"):
		print >>f, l
	for l in asset.generateSetImpl("CAssetPath"):
		print >>f, l
	for l in asset.generateGetImpl("CSubPath"):
		print >>f, l
	for l in asset.generateSetImpl("CSubPath"):
		print >>f, l
	for l in asset.generateAddImpl():
		print >>f, l
	for l in asset.generateAddAtImpl():
		print >>f, l
	for l in asset.generateDeleteImpl():
		print >>f, l
	for l in asset.generateRelMoveImpl():
		print >>f, l
	print >>f, ""
	

assetsList = []

mainAsset = Class("Asset")

# CHARACTER ############################################################
character_part = Class("Part")
character_part.addMember("0.2.0", "Name", TypeString(128))
character_part.addMember("0.2.0", "DefaultPath", TypeAssetPath())

character = ClassAsset("Character", len(assetsList))
character.setInheritance(mainAsset)
character.addClass(character_part)
character.addMember("0.2.0", "IdlePath", TypeAssetPath())
character.addMember("0.2.0", "WalkPath", TypeAssetPath())
character.addMember("0.2.0", "ControlledJumpPath", TypeAssetPath())
character.addMember("0.2.0", "UncontrolledJumpPath", TypeAssetPath())
character.addMember("0.2.0", "Part", TypeArray(character_part))

assetsList.append(character)

# CHARACTER PART #######################################################
characterPart = ClassAsset("CharacterPart", len(assetsList))
characterPart.setInheritance(mainAsset)
characterPart.addMember("0.2.0", "CharacterPath", TypeAssetPath())
characterPart.addMember("0.2.0", "CharacterPart", TypeSubPath())
characterPart.addMember("0.2.0", "SkeletonSkinPath", TypeAssetPath())

assetsList.append(characterPart)

# GUI BOX STYLE ########################################################
guiBoxStyle = ClassAsset("GuiBoxStyle", len(assetsList))
guiBoxStyle.setInheritance(mainAsset)
guiBoxStyle.addMember("0.2.0", "MinWidth", TypeInt32(), "0")
guiBoxStyle.addMember("0.2.0", "MinHeight", TypeInt32(), "0")
guiBoxStyle.addMember("0.2.0", "Margin", TypeInt32(), "0")
guiBoxStyle.addMember("0.2.0", "Padding", TypeInt32(), "0")
guiBoxStyle.addMember("0.2.0", "Spacing", TypeInt32(), "0")
guiBoxStyle.addMember("0.2.0", "RectPath", TypeAssetPath())

assetsList.append(guiBoxStyle)

# GUI BUTTON STYLE #####################################################
guiButtonStyle = ClassAsset("GuiButtonStyle", len(assetsList))
guiButtonStyle.setInheritance(mainAsset)
guiButtonStyle.addMember("0.2.0", "IdleStylePath", TypeAssetPath())
guiButtonStyle.addMember("0.2.0", "MouseOverStylePath", TypeAssetPath())
guiButtonStyle.addMember("0.2.0", "ReadOnlyStylePath", TypeAssetPath())
guiButtonStyle.addMember("0.2.0", "FocusStylePath", TypeAssetPath())

assetsList.append(guiButtonStyle)

# GUI COLOR EDIT STYLE #################################################
guiColorEditStyle = ClassAsset("GuiColorEditStyle", len(assetsList))
guiColorEditStyle.setInheritance(mainAsset)
guiColorEditStyle.addMember("0.2.0", "ButtonStylePath", TypeAssetPath())
guiColorEditStyle.addMember("0.2.0", "PopupStylePath", TypeAssetPath())
guiColorEditStyle.addMember("0.2.0", "RGBIconPath", TypeAssetPath())
guiColorEditStyle.addMember("0.2.0", "HSVIconPath", TypeAssetPath())
guiColorEditStyle.addMember("0.2.0", "SquareIconPath", TypeAssetPath())
guiColorEditStyle.addMember("0.2.0", "WheelIconPath", TypeAssetPath())

assetsList.append(guiColorEditStyle)

# GUI LABEL STYLE ######################################################
guiLabelStyle = ClassAsset("GuiLabelStyle", len(assetsList))
guiLabelStyle.setInheritance(mainAsset)
guiLabelStyle.addMember("0.2.0", "MinWidth", TypeInt32(), "0")
guiLabelStyle.addMember("0.2.0", "MinHeight", TypeInt32(), "0")
guiLabelStyle.addMember("0.2.0", "Margin", TypeInt32(), "0")
guiLabelStyle.addMember("0.2.0", "Padding", TypeInt32(), "0")
guiLabelStyle.addMember("0.2.0", "Spacing", TypeInt32(), "0")
guiLabelStyle.addMember("0.2.0", "FontSize", TypeInt32(), "12")
guiLabelStyle.addMember("0.2.0", "TextColor", TypeColor(), "1.0f")
guiLabelStyle.addMember("0.2.0", "TextAlignment", TypeInt32(), "TEXTALIGNMENT_LEFT")
guiLabelStyle.addMember("0.2.0", "RectPath", TypeAssetPath())
guiLabelStyle.addMember("0.2.0", "IconPath", TypeAssetPath())
guiLabelStyle.addPublicLines([
	"enum",
	"{",
	"	TEXTALIGNMENT_LEFT=0,",
	"	TEXTALIGNMENT_CENTER,",
	"	TEXTALIGNMENT_RIGHT,",
	"	TEXTALIGNMENT_BELOW_CENTER,",
	"	TEXTALIGNMENT_BELOW_CENTER_EXTENDED,",
	"};",
	""
])

assetsList.append(guiLabelStyle)

# GUI INT EDIT #########################################################
guiIntEditStyle = ClassAsset("GuiIntEditStyle", len(assetsList))
guiIntEditStyle.setInheritance(mainAsset)
guiIntEditStyle.addMember("0.2.0", "IncreaseButtonStylePath", TypeAssetPath())
guiIntEditStyle.addMember("0.2.0", "DecreaseButtonStylePath", TypeAssetPath())

assetsList.append(guiIntEditStyle)

# GUI LINE STYLE #######################################################
guiLineStyle = ClassAsset("GuiLineStyle", len(assetsList))
guiLineStyle.setInheritance(mainAsset)
guiLineStyle.addMember("0.2.0", "Flags", TypeInt32(), "0x0")
guiLineStyle.addMember("0.2.0", "BorderColor", TypeColor(), "1.0f")
guiLineStyle.addMember("0.2.0", "ImageLPath", TypeAssetPath())
guiLineStyle.addMember("0.2.0", "ImageMPath", TypeAssetPath())
guiLineStyle.addMember("0.2.0", "ImageRPath", TypeAssetPath())
guiLineStyle.addPublicLines([
	"enum",
	"{",
	"	FLAG_BORDER=1,",
	"	FLAG_IMAGE=2,",
	"};",
	""
])
assetsList.append(guiLineStyle)

#~ # GUI RECT STYLE #######################################################
guiRectStyle = ClassAsset("GuiRectStyle", len(assetsList))
guiRectStyle.setInheritance(mainAsset)
guiRectStyle.addMember("0.2.0", "Flags", TypeInt32(), "0x0")
guiRectStyle.addMember("0.2.0", "BackgroundColor", TypeColor(), "1.0f")
guiRectStyle.addMember("0.2.0", "BackgroundPadding", TypeFloat(), "0")
guiRectStyle.addMember("0.2.0", "BorderColor", TypeColor(), "1.0f")
guiRectStyle.addMember("0.2.0", "BorderFlags", TypeInt32(), "0x0")
guiRectStyle.addMember("0.2.0", "CornerRadius", TypeFloat(), "0")
guiRectStyle.addMember("0.2.0", "CornerFlags", TypeInt32(), "0x0")
guiRectStyle.addMember("0.2.0", "ImagePadding", TypeInt32(), "0")
guiRectStyle.addMember("0.2.0", "ImageTPath", TypeAssetPath())
guiRectStyle.addMember("0.2.0", "ImageRPath", TypeAssetPath())
guiRectStyle.addMember("0.2.0", "ImageBPath", TypeAssetPath())
guiRectStyle.addMember("0.2.0", "ImageLPath", TypeAssetPath())
guiRectStyle.addMember("0.2.0", "ImageTRPath", TypeAssetPath())
guiRectStyle.addMember("0.2.0", "ImageBRPath", TypeAssetPath())
guiRectStyle.addMember("0.2.0", "ImageBLPath", TypeAssetPath())
guiRectStyle.addMember("0.2.0", "ImageTLPath", TypeAssetPath())
guiRectStyle.addPublicLines([
	"enum",
	"{",
	"	FLAG_BACKGROUND=1,",
	"	FLAG_BORDER=2,",
	"	FLAG_ROUNDCORNER=4,",
	"	FLAG_IMAGE=8,",
	"};",
	"",
	"enum",
	"{",
	"	BORDERFLAG_T=1,",
	"	BORDERFLAG_B=2,",
	"	BORDERFLAG_L=4,",
	"	BORDERFLAG_R=8,",
	"	",
	"	BORDERFLAG_ALL=BORDERFLAG_T|BORDERFLAG_B|BORDERFLAG_L|BORDERFLAG_R,",
	"};",
	"",
	"enum",
	"{",
	"	CORNERFLAG_TL=1,",
	"	CORNERFLAG_TR=2,",
	"	CORNERFLAG_BL=4,",
	"	CORNERFLAG_BR=8,",
	"",
	"	CORNERFLAG_T=CORNERFLAG_TL|CORNERFLAG_TR,",
	"	CORNERFLAG_B=CORNERFLAG_BL|CORNERFLAG_BR,",
	"	CORNERFLAG_L=CORNERFLAG_TL|CORNERFLAG_BL,",
	"	CORNERFLAG_R=CORNERFLAG_TR|CORNERFLAG_BR,",
	"	",
	"	CORNERFLAG_ALL=CORNERFLAG_B|CORNERFLAG_T,",
	"};",
	""
])

assetsList.append(guiRectStyle)

# GUI SCROLLBAR STYLE ##################################################
guiScrollbarStyle = ClassAsset("GuiScrollbarStyle", len(assetsList))
guiScrollbarStyle.setInheritance(mainAsset)
guiScrollbarStyle.addMember("0.2.0", "RectPath", TypeAssetPath())
guiScrollbarStyle.addMember("0.2.0", "DefaultRailPath", TypeAssetPath())
guiScrollbarStyle.addMember("0.2.0", "DefaultSliderPath", TypeAssetPath())
guiScrollbarStyle.addMember("0.2.0", "MouseOverSliderPath", TypeAssetPath())
guiScrollbarStyle.addMember("0.2.0", "Margin", TypeInt32(), "0")
guiScrollbarStyle.addMember("0.2.0", "Padding", TypeInt32(), "0")

assetsList.append(guiScrollbarStyle)

# GUI SCROLLBAR STYLE ##################################################
guiSliderStyle = ClassAsset("GuiSliderStyle", len(assetsList))
guiSliderStyle.setInheritance(mainAsset)
guiSliderStyle.addMember("0.2.0", "RectPath", TypeAssetPath())
guiSliderStyle.addMember("0.2.0", "DefaultRailPath", TypeAssetPath())
guiSliderStyle.addMember("0.2.0", "CursorPath", TypeAssetPath())
guiSliderStyle.addMember("0.2.0", "Margin", TypeInt32(), "0")
guiSliderStyle.addMember("0.2.0", "Padding", TypeInt32(), "0")

assetsList.append(guiSliderStyle)

# GUI TABS STYLE #######################################################
guiTabsStyle = ClassAsset("GuiTabsStyle", len(assetsList))
guiTabsStyle.setInheritance(mainAsset)
guiTabsStyle.addMember("0.2.0", "LayoutPath", TypeAssetPath())
guiTabsStyle.addMember("0.2.0", "ContentPath", TypeAssetPath())
guiTabsStyle.addMember("0.2.0", "ButtonListPath", TypeAssetPath())
guiTabsStyle.addMember("0.2.0", "InactiveButtonPath", TypeAssetPath())
guiTabsStyle.addMember("0.2.0", "ActiveButtonPath", TypeAssetPath())
guiTabsStyle.addMember("0.2.0", "ButtonListFill", TypeBool(), "false")
guiTabsStyle.addMember("0.2.0", "ButtonListText", TypeBool(), "false")

assetsList.append(guiTabsStyle)

# GUI TOGGLE STYLE #####################################################
guiToggleStyle = ClassAsset("GuiToggleStyle", len(assetsList))
guiToggleStyle.setInheritance(mainAsset)
guiToggleStyle.addMember("0.2.0", "IdleTrueStylePath", TypeAssetPath())
guiToggleStyle.addMember("0.2.0", "MouseOverTrueStylePath", TypeAssetPath())
guiToggleStyle.addMember("0.2.0", "IconTruePath", TypeAssetPath())
guiToggleStyle.addMember("0.2.0", "IdleFalseStylePath", TypeAssetPath())
guiToggleStyle.addMember("0.2.0", "MouseOverFalseStylePath", TypeAssetPath())
guiToggleStyle.addMember("0.2.0", "IconFalsePath", TypeAssetPath())
guiToggleStyle.addMember("0.2.0", "SwitchIcon", TypeBool(), "false")

assetsList.append(guiToggleStyle)

# IMAGE ################################################################
image = ClassAsset("Image", len(assetsList))
image.setInheritance(mainAsset)
image.addMember("0.2.0", "GridWidth", TypeInt32(), "1")
image.addMember("0.2.0", "GridHeight", TypeInt32(), "1")
image.addMember("0.2.0", "GridSpacing", TypeInt32(), "0")
image.addMember("0.2.0", "TexelSize", TypeInt32(), "1024")
image.addMember("0.2.0", "TilingEnabled", TypeBool(), "false")
image.addMember("0.2.0", "Data", TypeArray2d(TypeUInt8()))
image.addMember("0.2.0", "Texture", TypeTextureHandle())
image.addHeader("shared/graphics.h")
image.addPublicFunc([
	"vec4 GetColor(int x, int y) const;"
	"vec4 Sample(vec2 UV) const;"
])

assetsList.append(image)

# MAP ##################################################################
_map = ClassAsset("Map", len(assetsList))
_map.setInheritance(mainAsset)
_map.addMember("0.2.0", "BgGroup", TypeArray(TypeAssetPath()))
_map.addMember("0.2.0", "FgGroup", TypeArray(TypeAssetPath()))
_map.addMember("0.2.0", "ZoneLayer", TypeArray(TypeAssetPath()))
_map.addMember("0.2.0", "EntityLayer", TypeArray(TypeAssetPath()))
_map.addMember("0.2.0", "CameraPosition", TypeVec2())
_map.addMember("0.2.0", "CameraZoom", TypeFloat(), "1.0f")
_map.addMember("0.2.0", "ShowEntities", TypeBool(), "true")
_map.addMember("0.2.0", "ShowZones", TypeBool(), "true")

assetsList.append(_map)

# MAP GROUP ############################################################
mapGroup = ClassAsset("MapGroup", len(assetsList))
mapGroup.setInheritance(mainAsset)
mapGroup.addMember("0.2.0", "ParentPath", TypeAssetPath())
mapGroup.addMember("0.2.0", "Layer", TypeArray(TypeAssetPath()))
mapGroup.addMember("0.2.0", "Position", TypeVec2(), "vec2(0.0f, 0.0f)")
mapGroup.addMember("0.2.0", "HardParallax", TypeVec2(), "vec2(1.0f, 1.0f)")
mapGroup.addMember("0.2.0", "Clipping", TypeBool(), "false")
mapGroup.addMember("0.2.0", "ClipPosition", TypeVec2(), "vec2(0.0f, 0.0f)")
mapGroup.addMember("0.2.0", "ClipSize", TypeVec2(), "vec2(64.0f, 64.0f)")
mapGroup.addMember("0.2.0", "Visibility", TypeBool(), "true")

assetsList.append(mapGroup)

# MAP LAYER QUADS ######################################################
mapLayerQuads_quad = Class("Quad")
mapLayerQuads_quad.addMember("0.2.0", "Pivot", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("0.2.0", "Size", TypeVec2(), "1.0f")
mapLayerQuads_quad.addMember("0.2.0", "Angle", TypeFloat(), "0.0f")
mapLayerQuads_quad.addMember("0.2.0", "Vertex0", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("0.2.0", "Vertex1", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("0.2.0", "Vertex2", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("0.2.0", "Vertex3", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("0.2.0", "UV0", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("0.2.0", "UV1", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("0.2.0", "UV2", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("0.2.0", "UV3", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("0.2.0", "Color0", TypeColor(), "1.0f")
mapLayerQuads_quad.addMember("0.2.0", "Color1", TypeColor(), "1.0f")
mapLayerQuads_quad.addMember("0.2.0", "Color2", TypeColor(), "1.0f")
mapLayerQuads_quad.addMember("0.2.0", "Color3", TypeColor(), "1.0f")
mapLayerQuads_quad.addMember("0.2.0", "AnimationPath", TypeAssetPath())
mapLayerQuads_quad.addMember("0.2.2", "Color", TypeColor(), "1.0f")
mapLayerQuads_quad.addMember("0.3.0", "AnimationOffset", TypeInt64(), "0")
mapLayerQuads_quad.addPublicFunc([
	"void GetTransform(CAssetsManager* pAssetsManager, int64 Time, matrix2x2* pMatrix, vec2* pPosition) const;",
	"void GetDrawState(CAssetsManager* pAssetsManager, int64 Time, vec4* pColor, int* pState) const;"
])

mapLayerQuads = ClassAsset("MapLayerQuads", len(assetsList))
mapLayerQuads.setInheritance(mainAsset)
mapLayerQuads.addClass(mapLayerQuads_quad)
mapLayerQuads.addMember("0.2.0", "ParentPath", TypeAssetPath())
mapLayerQuads.addMember("0.2.0", "ImagePath", TypeAssetPath())
mapLayerQuads.addMember("0.2.0", "Quad", TypeArray(mapLayerQuads_quad))
mapLayerQuads.addMember("0.2.0", "Visibility", TypeBool(), "true")
mapLayerQuads.addMember("0.2.3", "LevelOfDetail", TypeInt32(), "0")
mapLayerQuads.addPublicFunc([
	"void GetQuadTransform(const CSubPath& SubPath, int64 Time, matrix2x2* pMatrix, vec2* pPosition) const;",
	"void GetQuadDrawState(const CSubPath& SubPath, int64 Time, vec4* pColor, int* pState) const;"
])
mapLayerQuads.addPublicLines([
	"enum",
	"{",
	"	VERTEX_NONE=0,",
	"	VERTEX_PIVOT,",
	"	VERTEX0,",
	"	VERTEX1,",
	"	VERTEX2,",
	"	VERTEX3,",
	"};",
	""
])

assetsList.append(mapLayerQuads)

# MAP LAYER TILES ######################################################
mapLayerTiles_tile = Class("Tile")
mapLayerTiles_tile.addMember("0.2.0", "Index", TypeUInt8(), "0")
mapLayerTiles_tile.addMember("0.2.0", "Flags", TypeUInt8(), "0x0")
mapLayerTiles_tile.addMember("0.2.3", "Brush", TypeUInt8(), "0")

mapLayerTiles = ClassAsset("MapLayerTiles", len(assetsList))
mapLayerTiles.setInheritance(mainAsset)
mapLayerTiles.addClass(mapLayerTiles_tile)
mapLayerTiles.addMember("0.2.0", "ParentPath", TypeAssetPath())
mapLayerTiles.addMember("0.2.0", "StylePath", TypeAssetPath())
mapLayerTiles.addMember("0.2.0", "Color", TypeColor(), "1.0f")
mapLayerTiles.addMember("0.2.0", "Tile", TypeArray2d(mapLayerTiles_tile))
mapLayerTiles.addMember("0.2.0", "Visibility", TypeBool(), "true")
mapLayerTiles.addMember("0.2.1", "PositionX", TypeInt32(), "0")
mapLayerTiles.addMember("0.2.1", "PositionY", TypeInt32(), "0")
mapLayerTiles.addMember("0.2.3", "LevelOfDetail", TypeInt32(), "0")
mapLayerTiles.addMember("0.2.3", "SourcePath", TypeAssetPath())
mapLayerTiles.addMember("0.2.4", "RandomSeed", TypeInt32())

mapLayerTiles.addPublicLines([
	"enum",
	"{",
	"	TILEFLAG_VFLIP=1,",
	"	TILEFLAG_HFLIP=2,",
	"	TILEFLAG_OPAQUE=4,",
	"	TILEFLAG_ROTATE=8,",
	"};",
	""
])

assetsList.append(mapLayerTiles)

# MAP ZONE TILES #######################################################
mapZoneTiles_tile = Class("Tile")
mapZoneTiles_tile.addMember("0.2.0", "Index", TypeUInt8(), "0")
mapZoneTiles_tile.addMember("0.2.3", "Flags", TypeUInt32(), "0x0")

mapZoneTiles = ClassAsset("MapZoneTiles", len(assetsList))
mapZoneTiles.setInheritance(mainAsset)
mapZoneTiles.addClass(mapZoneTiles_tile)
mapZoneTiles.addMember("0.2.0", "ParentPath", TypeAssetPath())
mapZoneTiles.addMember("0.2.0", "ZoneTypePath", TypeAssetPath())
mapZoneTiles.addMember("0.2.0", "Tile", TypeArray2d(mapZoneTiles_tile))
mapZoneTiles.addMember("0.2.0", "Visibility", TypeBool(), "true")
mapZoneTiles.addMember("0.2.2", "DataInt", TypeArray2d(TypeInt32()))
mapZoneTiles.addMember("0.2.4", "PositionX", TypeInt32(), "0")
mapZoneTiles.addMember("0.2.4", "PositionY", TypeInt32(), "0")

assetsList.append(mapZoneTiles)

# MAP ENTITIES #########################################################
mapEntities_entity = Class("Entity")
mapEntities_entity.addMember("0.2.0", "TypePath", TypeAssetPath())
mapEntities_entity.addMember("0.2.0", "Position", TypeVec2(), "0.0f")

mapEntities = ClassAsset("MapEntities", len(assetsList))
mapEntities.setInheritance(mainAsset)
mapEntities.addClass(mapEntities_entity)
mapEntities.addMember("0.2.0", "ParentPath", TypeAssetPath())
mapEntities.addMember("0.2.0", "Entity", TypeArray(mapEntities_entity))
mapEntities.addMember("0.2.0", "Visibility", TypeBool(), "true")

assetsList.append(mapEntities)

# ZONE TYPE ############################################################
zoneType_dataInt = Class("DataInt")
zoneType_dataInt.addMember("0.2.2", "Title", TypeString(128))
zoneType_dataInt.addMember("0.2.2", "Description", TypeString(128))
zoneType_dataInt.addMember("0.2.2", "DefaultValue", TypeInt32(), "0")
zoneType_dataInt.addMember("0.2.2", "MinValue", TypeInt32(), "0")
zoneType_dataInt.addMember("0.2.2", "MaxValue", TypeInt32(), "255")
zoneType_dataInt.addMember("0.2.2", "NullValue", TypeInt32(), "0")

zoneType_index = Class("Index")
zoneType_index.addMember("0.2.0", "Used", TypeBool(), "true")
zoneType_index.addMember("0.2.0", "Description", TypeString(128))
zoneType_index.addMember("0.2.0", "Color", TypeColor(), "1.0f")
zoneType_index.addMember("0.2.0", "Title", TypeString(128))
zoneType_index.addMember("0.2.0", "BorderIndex", TypeInt32(), "0")
zoneType_index.addMember("0.2.0", "BorderColor", TypeColor(), "1.0f")
zoneType_index.addMember("0.2.2", "Group", TypeInt32(), "-1")
zoneType_index.addMember("0.2.3", "RotationAllowed", TypeBool(), "false")
zoneType_index.addMember("0.2.3", "MirrorAllowed", TypeBool(), "false")

zoneType = ClassAsset("ZoneType", len(assetsList))
zoneType.setInheritance(mainAsset)
zoneType.addClass(zoneType_index)
zoneType.addClass(zoneType_dataInt)
zoneType.addMember("0.2.0", "Index", TypeArray(zoneType_index))
zoneType.addMember("0.2.0", "ImagePath", TypeAssetPath())
zoneType.addMember("0.2.2", "DataInt", TypeArray(zoneType_dataInt))
zoneType.addMember("0.2.2", "Group", TypeArray(TypeString(128)))

assetsList.append(zoneType)

# ENTITY TYPE ##########################################################
entityType = ClassAsset("EntityType", len(assetsList))
entityType.setInheritance(mainAsset)
entityType.addMember("0.2.0", "GizmoPath", TypeAssetPath())
entityType.addMember("0.2.0", "CollisionRadius", TypeFloat())

assetsList.append(entityType)

# SKELETON #############################################################
skeleton_bone = Class("Bone")
skeleton_bone.addMember("0.2.0", "Parent", TypeSubPath())
skeleton_bone.addMember("0.2.0", "Length", TypeFloat(), "32.0f")
skeleton_bone.addMember("0.2.0", "Anchor", TypeFloat(), "0.0f")
skeleton_bone.addMember("0.2.0", "Translation", TypeVec2(), "0.0f")
skeleton_bone.addMember("0.2.0", "Angle", TypeFloat(), "0.0f")
skeleton_bone.addMember("0.2.0", "Name", TypeString(128))
skeleton_bone.addMember("0.2.0", "Color", TypeColor(), "1.0f")

skeleton_layer = Class("Layer")
skeleton_layer.addMember("0.2.0", "Name", TypeString(128))

skeleton = ClassAsset("Skeleton", len(assetsList))
skeleton.setInheritance(mainAsset)
skeleton.addClass(skeleton_bone)
skeleton.addClass(skeleton_layer)
skeleton.addMember("0.2.0", "Bone", TypeArray(skeleton_bone))
skeleton.addMember("0.2.0", "Layer", TypeArray(skeleton_layer))

assetsList.append(skeleton)

# SKELETON ANIMATION ###################################################
skeletonAnim_boneAnim_frame = Class("Frame")
skeletonAnim_boneAnim_frame.addMember("0.2.0", "Translation", TypeVec2(), "0.0f")
skeletonAnim_boneAnim_frame.addMember("0.2.0", "Scale", TypeVec2(), "1.0f")
skeletonAnim_boneAnim_frame.addMember("0.2.0", "Angle", TypeFloat(), "0.0f")
skeletonAnim_boneAnim_frame.addMember("0.2.0", "Alignment", TypeInt32(), "BONEALIGN_PARENTBONE")

skeletonAnim_boneAnim_keyframe = Class("KeyFrame")
skeletonAnim_boneAnim_keyframe.setInheritance(skeletonAnim_boneAnim_frame)
skeletonAnim_boneAnim_keyframe.addMember("0.2.0", "Time", TypeInt64(), "0")
skeletonAnim_boneAnim_keyframe.addMember("0.2.0", "GraphType", TypeInt32(), "GRAPHTYPE_LINEAR")
skeletonAnim_boneAnim_keyframe.addPublicFunc([
	"CKeyFrame& operator=(const CFrame& Frame);"
])

skeletonAnim_boneAnim = Class("BoneAnimation")
skeletonAnim_boneAnim.addClass(skeletonAnim_boneAnim_frame)
skeletonAnim_boneAnim.addClass(skeletonAnim_boneAnim_keyframe)
skeletonAnim_boneAnim.addMember("0.2.0", "KeyFrame", TypeArray(skeletonAnim_boneAnim_keyframe))
skeletonAnim_boneAnim.addMember("0.2.0", "BonePath", TypeSubPath())
skeletonAnim_boneAnim.addMember("0.2.0", "CycleType", TypeInt32(), "CYCLETYPE_CLAMP")
skeletonAnim_boneAnim.addPublicFunc([
	"int64 GetDuration() const;",
	"int TimeToKeyFrame(int64 Time) const;",
	"bool GetFrame(int64 Time, CFrame& Frame) const;",
	""
])

skeletonAnim_layerAnim_frame = Class("Frame")
skeletonAnim_layerAnim_frame.addMember("0.2.0", "Color", TypeColor(), "1.0f")
skeletonAnim_layerAnim_frame.addMember("0.2.0", "State", TypeInt32(), "LAYERSTATE_VISIBLE")

skeletonAnim_layerAnim_keyframe = Class("KeyFrame")
skeletonAnim_layerAnim_keyframe.setInheritance(skeletonAnim_layerAnim_frame)
skeletonAnim_layerAnim_keyframe.addMember("0.2.0", "Time", TypeInt64(), "0")
skeletonAnim_layerAnim_keyframe.addMember("0.2.0", "GraphType", TypeInt32(), "GRAPHTYPE_LINEAR")
skeletonAnim_layerAnim_keyframe.addPublicFunc([
	"CKeyFrame& operator=(const CFrame& Frame);"
])

skeletonAnim_layerAnim = Class("LayerAnimation")
skeletonAnim_layerAnim.addClass(skeletonAnim_layerAnim_frame)
skeletonAnim_layerAnim.addClass(skeletonAnim_layerAnim_keyframe)
skeletonAnim_layerAnim.addMember("0.2.0", "KeyFrame", TypeArray(skeletonAnim_layerAnim_keyframe))
skeletonAnim_layerAnim.addMember("0.2.0", "LayerPath", TypeSubPath())
skeletonAnim_layerAnim.addMember("0.2.0", "CycleType", TypeInt32(), "CYCLETYPE_CLAMP")
skeletonAnim_layerAnim.addPublicFunc([
	"int64 GetDuration() const;",
	"int TimeToKeyFrame(int64 Time) const;",
	"bool GetFrame(int64 Time, CFrame& Frame) const;",
	""
])

skeletonAnim = ClassAsset("SkeletonAnimation", len(assetsList))
skeletonAnim.setInheritance(mainAsset)
skeletonAnim.addClass(skeletonAnim_boneAnim)
skeletonAnim.addClass(skeletonAnim_layerAnim)
skeletonAnim.addMember("0.2.0", "SkeletonPath", TypeAssetPath())
skeletonAnim.addMember("0.2.0", "BoneAnimation", TypeArray(skeletonAnim_boneAnim))
skeletonAnim.addMember("0.2.0", "LayerAnimation", TypeArray(skeletonAnim_layerAnim))

skeletonAnim.addPublicLines([
	"enum",
	"{",
	"	GRAPHTYPE_FREE = 0,",
	"	GRAPHTYPE_STEPSTART,",
	"	GRAPHTYPE_STEPEND,",
	"	GRAPHTYPE_STEPMIDDLE,",
	"	GRAPHTYPE_LINEAR,",
	"	GRAPHTYPE_ACCELERATION,",
	"	GRAPHTYPE_DECELERATION,",
	"	GRAPHTYPE_SMOOTH,",
	"	NUM_GRAPHS,",
	"};",
	"",
	"enum",
	"{",
	"	CYCLETYPE_CLAMP = 0,",
	"	CYCLETYPE_LOOP,",
	"	NUM_CYCLETYPES,",
	"};",
	"",
	"enum",
	"{",
	"	LAYERSTATE_VISIBLE = 0,",
	"	LAYERSTATE_HIDDEN,",
	"	NUM_LAYERSTATES,",
	"};",
	"",
	"enum",
	"{",
	"	BONEALIGN_PARENTBONE = 0,",
	"	BONEALIGN_WORLD,",
	"	NUM_BONEALIGNS,",
	"};",
	""
])
skeletonAnim.addPublicFunc([
	"bool GetBoneAnimFrame(const CSubPath& SubPath, int64 Time, CBoneAnimation::CFrame& Frame) const;",
	"bool GetLayerAnimFrame(const CSubPath& SubPath, int64 Time, CLayerAnimation::CFrame& Frame) const;",
	"CSubPath FindBoneAnim(const CSubPath& BonePath) const;",
	"CSubPath FindLayerAnim(const CSubPath& LayerPath) const;",
	""
])

assetsList.append(skeletonAnim)

# SKELETON SKIN ########################################################
skeletonSkin_sprite = Class("Sprite")
skeletonSkin_sprite.addMember("0.2.0", "SpritePath", TypeAssetPath())
skeletonSkin_sprite.addMember("0.2.0", "BonePath", TypeSubPath())
skeletonSkin_sprite.addMember("0.2.0", "LayerPath", TypeSubPath())
skeletonSkin_sprite.addMember("0.2.0", "Anchor", TypeFloat(), "0.0f")
skeletonSkin_sprite.addMember("0.2.0", "Translation", TypeVec2(), "0.0f")
skeletonSkin_sprite.addMember("0.2.0", "Scale", TypeVec2(), "1.0f")
skeletonSkin_sprite.addMember("0.2.0", "Angle", TypeFloat(), "0.0f")
skeletonSkin_sprite.addMember("0.2.0", "Color", TypeColor(), "1.0f")
skeletonSkin_sprite.addMember("0.2.0", "Alignment", TypeInt32(), "ALIGNMENT_BONE")

skeletonSkin = ClassAsset("SkeletonSkin", len(assetsList))
skeletonSkin.setInheritance(mainAsset)
skeletonSkin.addClass(skeletonSkin_sprite)
skeletonSkin.addMember("0.2.0", "SkeletonPath", TypeAssetPath())
skeletonSkin.addMember("0.2.0", "Sprite", TypeArray(skeletonSkin_sprite))

skeletonSkin.addPublicLines([
	"enum",
	"{",
	"	ALIGNMENT_WORLD=0,",
	"	ALIGNMENT_BONE,",
	"};",
	""
])

assetsList.append(skeletonSkin)

# SPRITE ###############################################################
sprite = ClassAsset("Sprite", len(assetsList))
sprite.setInheritance(mainAsset)
sprite.addMember("0.2.0", "ImagePath", TypeAssetPath())
sprite.addMember("0.2.0", "X", TypeInt32(), "0")
sprite.addMember("0.2.0", "Y", TypeInt32(), "0")
sprite.addMember("0.2.0", "Width", TypeInt32(), "1")
sprite.addMember("0.2.0", "Height", TypeInt32(), "1")
sprite.addPublicLines([
	"enum",
	"{",
	"	FLAG_FLIP_Y = (0x1 << 0),",
	"	FLAG_FLIP_X = (0x1 << 1),",
	"	FLAG_FLIP_ANIM_Y = (0x1 << 2),",
	"	FLAG_FLIP_ANIM_X = (0x1 << 3),",
	"	FLAG_SIZE_HEIGHT = (0x1 << 4),",
	"};",
	""
])
sprite.addPublicFunc([
	"int GetPixelWidth() const;",
	"int GetPixelHeight() const;"
])

assetsList.append(sprite)

# WEAPON ###############################################################
weapon = ClassAsset("Weapon", len(assetsList))
weapon.setInheritance(mainAsset)
weapon.addMember("0.2.0", "CharacterPath", TypeAssetPath())
weapon.addMember("0.2.0", "CursorPath", TypeAssetPath())
weapon.addMember("0.2.0", "SkinPath", TypeAssetPath())
weapon.addMember("0.2.0", "AttackAnimationPath", TypeAssetPath())

assetsList.append(weapon)

# MAP LAYER OBJECTS ####################################################
mapLayerObjects_vertex = Class("Vertex")
mapLayerObjects_vertex.addMember("0.2.0", "Position", TypeVec2(), "0.0f")
mapLayerObjects_vertex.addMember("0.2.0", "Weight", TypeFloat(), "1.0f")
mapLayerObjects_vertex.addMember("0.2.0", "Color", TypeColor(), "1.0f")
mapLayerObjects_vertex.addMember("0.2.0", "Smoothness", TypeInt32(), "CBezierVertex::TYPE_CORNER")
mapLayerObjects_vertex.addMember("0.2.0", "ControlPoint0", TypeVec2(), "vec2(-16.0f, 0.0f)")
mapLayerObjects_vertex.addMember("0.2.0", "ControlPoint1", TypeVec2(), "vec2(16.0f, 0.0f)")

mapLayerObjects_object = Class("Object")
mapLayerObjects_object.addMember("0.2.0", "Position", TypeVec2(), "0.0f")
mapLayerObjects_object.addMember("0.2.0", "Size", TypeVec2(), "1.0f")
mapLayerObjects_object.addMember("0.2.0", "Angle", TypeFloat(), "0.0f")
mapLayerObjects_object.addMember("0.2.0", "StylePath", TypeAssetPath())
mapLayerObjects_object.addMember("0.2.0", "Vertex", TypeArray(mapLayerObjects_vertex))
mapLayerObjects_object.addMember("0.2.0", "PathType", TypeInt32(), "PATHTYPE_OPEN")
mapLayerObjects_object.addMember("0.2.0", "FillType", TypeInt32(), "FILLTYPE_NONE")
mapLayerObjects_object.addMember("0.2.0", "LineType", TypeInt32(), "LINETYPE_SHOW")
mapLayerObjects_object.addMember("0.2.0", "OrthoTesselation", TypeInt32(), "1")
mapLayerObjects_object.addMember("0.3.0", "AnimationPath", TypeAssetPath())
mapLayerObjects_object.addMember("0.3.0", "AnimationOffset", TypeInt64(), "0")
mapLayerObjects_object.addPublicFunc([
	"void GetTransform(CAssetsManager* pAssetsManager, float Time, matrix2x2* pMatrix, vec2* pPosition) const;",
	"void GetDrawState(CAssetsManager* pAssetsManager, float Time, vec4* pColor, int* pState) const;"
])

mapLayerObjects = ClassAsset("MapLayerObjects", len(assetsList))
mapLayerObjects.addHeader("shared/geometry/bezier.h")
mapLayerObjects.setInheritance(mainAsset)
mapLayerObjects.addClass(mapLayerObjects_vertex)
mapLayerObjects.addClass(mapLayerObjects_object)
mapLayerObjects.addMember("0.2.0", "ParentPath", TypeAssetPath())
mapLayerObjects.addMember("0.2.0", "Object", TypeArray(mapLayerObjects_object))
mapLayerObjects.addMember("0.2.0", "Visibility", TypeBool(), "true")
mapLayerObjects.addMember("0.2.3", "LevelOfDetail", TypeInt32(), "0")
mapLayerObjects.addPublicFunc([
	"void GetObjectTransform(const CSubPath& SubPath, float Time, matrix2x2* pMatrix, vec2* pPosition) const;",
	"void GetObjectDrawState(const CSubPath& SubPath, float Time, vec4* pColor, int* pState) const;"
])
mapLayerObjects.addPublicLines([
	"enum",
	"{",
	"	LINETYPE_HIDE = 0,",
	"	LINETYPE_SHOW,",
	
	"	FILLTYPE_NONE = 0,",
	"	FILLTYPE_INSIDE,",
	"	FILLTYPE_OUTSIDE,",
	
	"	PATHTYPE_OPEN = 0,",
	"	PATHTYPE_CLOSED,",
	"	PATHTYPE_INFINITE,",
	"};",
	""
])

assetsList.append(mapLayerObjects)

# MATERIAL #############################################################
material_label = Class("Label")
material_label.addMember("0.2.0", "Color", TypeColor(), "1.0f")
material_label.addMember("0.2.0", "AngleStart", TypeFloat(), "0.0f")
material_label.addMember("0.2.0", "AngleEnd", TypeFloat(), "2.0f*Pi")

material_sprite = Class("Sprite")
material_sprite.addMember("0.2.0", "Path", TypeAssetPath())
material_sprite.addMember("0.2.0", "Size", TypeVec2(), "1.0f")
material_sprite.addMember("0.2.0", "Color", TypeColor(), "1.0f")
material_sprite.addMember("0.2.0", "Flags", TypeInt32(), "0x0")
material_sprite.addMember("0.2.0", "Position", TypeVec2(), "0.0f")
material_sprite.addMember("0.2.0", "Alignment", TypeInt32(), "SPRITEALIGN_LINE")
material_sprite.addMember("0.2.0", "Filling", TypeInt32(), "SPRITEFILLING_SCALING")
material_sprite.addMember("0.2.0", "TileType", TypeInt32(), "SPRITETILE_LINE")
material_sprite.addMember("0.2.0", "TileLabel0", TypeInt32(), "0")
material_sprite.addMember("0.2.0", "TileLabel1", TypeInt32(), "0")

material_layer = Class("Layer")
material_layer.addMember("0.2.0", "Sprite", TypeArray(material_sprite))

material = ClassAsset("PathMaterial", len(assetsList))
material.setInheritance(mainAsset)
material.addClass(material_label)
material.addClass(material_sprite)
material.addClass(material_layer)
material.addMember("0.2.0", "Label", TypeArray(material_label))
material.addMember("0.2.0", "Layer", TypeArray(material_layer))
material.addMember("0.2.0", "TexturePath", TypeAssetPath())
material.addMember("0.2.0", "TextureColor", TypeColor())
material.addMember("0.2.0", "TextureSize", TypeVec2(), "1.0f")
material.addMember("0.2.0", "TextureAngle", TypeFloat(), "0.0f")
material.addMember("0.2.0", "TextureSpacing", TypeFloat(), "0.0f")
material.addMember("0.2.0", "TextureEnabled", TypeBool(), "false")
material.addPublicLines([
	"enum",
	"{",
	"	SPRITEFLAG_VFLIP = 1,",
	"	SPRITEFLAG_HFLIP = 2,",
	"	SPRITEFLAG_ROTATION = 4,",
	"",
	"	SPRITEFILLING_SCALING = 0,",
	"	SPRITEFILLING_STRETCHING,",
	"	SPRITEFILLING_SPACING,",
	"",
	"	SPRITEALIGN_LINE = 0,",
	"	SPRITEALIGN_OBJECT,",
	"	SPRITEALIGN_STRETCHED,",
	"",
	"	SPRITETILE_LINE = 0,",
	"	SPRITETILE_CAP_START,",
	"	SPRITETILE_CAP_END,",
	"	SPRITETILE_CORNER_CONVEX,",
	"	SPRITETILE_CORNER_CONCAVE,",
	"};",
	""
])

assetsList.append(material)

# GUI COMBO BOX STYLE ##################################################
guiComboBoxStyle = ClassAsset("GuiComboBoxStyle", len(assetsList))
guiComboBoxStyle.setInheritance(mainAsset)
guiComboBoxStyle.addMember("0.2.0", "ButtonPath", TypeAssetPath())
guiComboBoxStyle.addMember("0.2.0", "PopupPath", TypeAssetPath())
guiComboBoxStyle.addMember("0.2.0", "EnumPath", TypeAssetPath())

assetsList.append(guiComboBoxStyle)

# MAP ZONE POLYGON #####################################################
mapZoneObjects_vertex = Class("Vertex")
mapZoneObjects_vertex.addMember("0.2.2", "Position", TypeVec2(), "0.0f")
mapZoneObjects_vertex.addMember("0.2.2", "Smoothness", TypeInt32(), "CBezierVertex::TYPE_CORNER")
mapZoneObjects_vertex.addMember("0.2.2", "ControlPoint0", TypeVec2(), "vec2(-16.0f, 0.0f)")
mapZoneObjects_vertex.addMember("0.2.2", "ControlPoint1", TypeVec2(), "vec2(16.0f, 0.0f)")
mapZoneObjects_vertex.addMember("0.2.2", "Weight", TypeFloat(), "1.0f")

mapZoneObjects_object = Class("Object")
mapZoneObjects_object.addMember("0.2.2", "Position", TypeVec2(), "0.0f")
mapZoneObjects_object.addMember("0.2.2", "Size", TypeVec2(), "1.0f")
mapZoneObjects_object.addMember("0.2.2", "Angle", TypeFloat(), "0.0f")
mapZoneObjects_object.addMember("0.2.2", "Vertex", TypeArray(mapZoneObjects_vertex))
mapZoneObjects_object.addMember("0.2.2", "PathType", TypeInt32(), "PATHTYPE_OPEN")
mapZoneObjects_object.addMember("0.2.2", "FillType", TypeInt32(), "FILLTYPE_NONE")
mapZoneObjects_object.addMember("0.2.2", "ZoneIndex", TypeUInt8(), "1")
mapZoneObjects_object.addMember("0.2.3", "ZoneFlags", TypeUInt32(), "0x0")
mapZoneObjects_object.addMember("0.3.0", "AnimationPath", TypeAssetPath())
mapZoneObjects_object.addMember("0.3.0", "AnimationOffset", TypeInt64(), "0")
mapZoneObjects_object.addPublicFunc([
	"void GetTransform(CAssetsManager* pAssetsManager, float Time, matrix2x2* pMatrix, vec2* pPosition) const;",
	"void GetDrawState(CAssetsManager* pAssetsManager, float Time, vec4* pColor, int* pState) const;"
])

mapZoneObjects = ClassAsset("MapZoneObjects", len(assetsList))
mapZoneObjects.addHeader("shared/geometry/bezier.h")
mapZoneObjects.setInheritance(mainAsset)
mapZoneObjects.addClass(mapZoneObjects_vertex)
mapZoneObjects.addClass(mapZoneObjects_object)
mapZoneObjects.addMember("0.2.2", "ParentPath", TypeAssetPath())
mapZoneObjects.addMember("0.2.2", "ZoneTypePath", TypeAssetPath())
mapZoneObjects.addMember("0.2.2", "Object", TypeArray(mapZoneObjects_object))
mapZoneObjects.addMember("0.2.2", "Visibility", TypeBool(), "true")
mapZoneObjects.addPublicFunc([
	"void GetObjectTransform(const CSubPath& SubPath, float Time, matrix2x2* pMatrix, vec2* pPosition) const;",
	"void GetObjectDrawState(const CSubPath& SubPath, float Time, vec4* pColor, int* pState) const;"
])
mapZoneObjects.addPublicLines([
	"enum",
	"{",
	"	FILLTYPE_NONE = 0,",
	"	FILLTYPE_INSIDE,",
	"	FILLTYPE_OUTSIDE,",
	
	"	PATHTYPE_OPEN = 0,",
	"	PATHTYPE_CLOSED,",
	"	PATHTYPE_INFINITE,",
	"};",
	""
])

assetsList.append(mapZoneObjects)

# TILING MATERIAL ######################################################
tilingMaterial_zone = Class("ZoneConverter")
tilingMaterial_zone.addMember("0.2.3", "ZoneTypePath", TypeAssetPath())
tilingMaterial_zone.addMember("0.2.3", "OldIndex", TypeUInt8())
tilingMaterial_zone.addMember("0.2.3", "NewIndex", TypeUInt8())

tilingMaterial_index = Class("Index")
tilingMaterial_index.addMember("0.2.3", "Title", TypeString(128))

tilingMaterial_label = Class("Label")
tilingMaterial_label.addMember("0.2.3", "Title", TypeString(128))
tilingMaterial_label.addMember("0.2.3", "Index", TypeArray(TypeUInt8()))

tilingMaterial_cond = Class("Condition")
tilingMaterial_cond.addMember("0.2.3", "Type", TypeInt32(), "CONDITIONTYPE_INDEX")
tilingMaterial_cond.addMember("0.2.3", "Value", TypeInt32(), "1")
tilingMaterial_cond.addMember("0.2.3", "RelPosX", TypeInt32(), "0")
tilingMaterial_cond.addMember("0.2.3", "RelPosY", TypeInt32(), "0")

tilingMaterial_rule = Class("Rule")
tilingMaterial_rule.addClass(tilingMaterial_cond)
tilingMaterial_rule.addMember("0.2.3", "Condition", TypeArray(tilingMaterial_cond))
tilingMaterial_rule.addMember("0.2.3", "Probability", TypeFloat(), "1.0f")
tilingMaterial_rule.addMember("0.2.3", "TileIndex", TypeUInt8(), "1")
tilingMaterial_rule.addMember("0.2.3", "TileFlags", TypeUInt8(), "0x0")

tilingMaterial = ClassAsset("TilingMaterial", len(assetsList))
tilingMaterial.setInheritance(mainAsset)
tilingMaterial.addClass(tilingMaterial_zone)
tilingMaterial.addClass(tilingMaterial_rule)
tilingMaterial.addClass(tilingMaterial_index)
tilingMaterial.addClass(tilingMaterial_label)
tilingMaterial.addMember("0.2.3", "ImagePath", TypeAssetPath())
tilingMaterial.addMember("0.2.3", "ZoneConverter", TypeArray(tilingMaterial_zone))
tilingMaterial.addMember("0.2.3", "Rule", TypeArray(tilingMaterial_rule))
tilingMaterial.addMember("0.2.3", "Index", TypeArray(tilingMaterial_index))
tilingMaterial.addMember("0.2.3", "Label", TypeArray(tilingMaterial_label))

tilingMaterial.addPublicLines([
	"enum",
	"{",
	"	CONDITIONTYPE_INDEX=0,",
	"	CONDITIONTYPE_NOTINDEX,",
	"	CONDITIONTYPE_LABEL,",
	"	CONDITIONTYPE_NOTLABEL,",
	"	CONDITIONTYPE_NOBORDER,",
	"	NUM_CONDITIONTYPES,",
	"};",
	""
])

assetsList.append(tilingMaterial)

#########################################

f = file("src/generated/assets/allassets.h", "w")
print >>f, "/*"
print >>f, " * Copyright (C) 2016 necropotame (necropotame@gmail.com)"
print >>f, " * "
print >>f, " * This file is part of TeeUniverse."
print >>f, " * "
print >>f, " * TeeUniverse is free software: you can redistribute it and/or  modify"
print >>f, " * it under the terms of the GNU Affero General Public License, version 3,"
print >>f, " * as published by the Free Software Foundation."
print >>f, " *"
print >>f, " * TeeUniverse is distributed in the hope that it will be useful,"
print >>f, " * but WITHOUT ANY WARRANTY; without even the implied warranty of"
print >>f, " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
print >>f, " * GNU Affero General Public License for more details."
print >>f, " *"
print >>f, " * You should have received a copy of the GNU Affero General Public License"
print >>f, " * along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>."
print >>f, " */"
print >>f, ""
print >>f, "/*"
print >>f, " * THIS FILE HAS BEEN GENERATED BY A SCRIPT."
print >>f, " * DO NOT EDIT MANUALLY!"
print >>f, " *"
print >>f, " * Please use the script \"scripts/assets/assets.py\" to regenerate it."
print >>f, " *"
print >>f, " * Why this file is generated by a script?"
print >>f, " * Because there is more than 10 files that follow the same format."
print >>f, " * In addition, each time a new member is added, enums, getter, setters,"
print >>f, " * copy/transfert functions and storage structure must be updated."
print >>f, " * It's too much to avoid mistakes."
print >>f, " */"
print >>f, ""
print >>f, "#ifndef __GENERATED_ASSETS_ALLASSETS__"
print >>f, "#define __GENERATED_ASSETS_ALLASSETS__"
print >>f, ""
for asset in assetsList:
	print >>f, "#include <generated/assets/"+asset.name.lower()+".h>"
print >>f, ""
print >>f, "#endif"

f = file("src/generated/assets/assetsmacro.h", "w")
print >>f, "/*"
print >>f, " * Copyright (C) 2016 necropotame (necropotame@gmail.com)"
print >>f, " * "
print >>f, " * This file is part of TeeUniverse."
print >>f, " * "
print >>f, " * TeeUniverse is free software: you can redistribute it and/or  modify"
print >>f, " * it under the terms of the GNU Affero General Public License, version 3,"
print >>f, " * as published by the Free Software Foundation."
print >>f, " *"
print >>f, " * TeeUniverse is distributed in the hope that it will be useful,"
print >>f, " * but WITHOUT ANY WARRANTY; without even the implied warranty of"
print >>f, " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
print >>f, " * GNU Affero General Public License for more details."
print >>f, " *"
print >>f, " * You should have received a copy of the GNU Affero General Public License"
print >>f, " * along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>."
print >>f, " */"
print >>f, ""
print >>f, "/*"
print >>f, " * THIS FILE HAS BEEN GENERATED BY A SCRIPT."
print >>f, " * DO NOT EDIT MANUALLY!"
print >>f, " *"
print >>f, " * Please use the script \"scripts/assets/assets.py\" to regenerate it."
print >>f, " *"
print >>f, " * Why this file is generated by a script?"
print >>f, " * Because there is more than 10 files that follow the same format."
print >>f, " * In addition, each time a new member is added, enums, getter, setters,"
print >>f, " * copy/transfert functions and storage structure must be updated."
print >>f, " * It's too much to avoid mistakes."
print >>f, " */"
print >>f, ""
for asset in assetsList:
	print >>f, "MACRO_ASSETTYPE("+asset.name+")"
print >>f, ""
	
for asset in assetsList:
	generateHeader(asset)
	generateImpl(asset)
