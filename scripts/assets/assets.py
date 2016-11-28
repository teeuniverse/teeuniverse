#
# Copyright (C) 2016 necropotame (necropotame@gmail.com)
#
# This file is part of TeeUniverses.
#
# TeeUniverses is free software: you can redistribute it and/or  modify
# it under the terms of the GNU Affero General Public License, version 3,
# as published by the Free Software Foundation.
#
# TeeUniverses is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with TeeUniverses.  If not, see <http://www.gnu.org/licenses/>.
#

import sys, os

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
	def __init__(self, suffix, returnType, getSetType, paramType):
		GetSetInterface.__init__(self, suffix)
		self.getSetType = getSetType
		self.returnType = returnType
		self.paramType = paramType
	def generateSet(self, var, value):
		return [var + " = " + value + ";"]
	def generateGet(self, var, value):
		return ["return " + var + ";"]
		
class GetSetInterface_SimpleRef(GetSetInterface_Simple):
	def __init__(self, suffix, returnType, paramType):
		GetSetInterface_Simple.__init__(self, suffix, returnType, returnType, paramType)
		self.getbyref = 1
		self.noset = 1
		
class GetSetInterface_SimpleGet(GetSetInterface_Simple):
	def __init__(self, suffix, returnType, paramType):
		GetSetInterface_Simple.__init__(self, suffix, returnType, returnType, paramType)
		self.noset = 1

class GetSetInterface_Struct(GetSetInterface):
	def __init__(self, suffix, returnType, paramType, getMem, setMem):
		GetSetInterface.__init__(self, suffix)
		self.getSetType = returnType
		self.returnType = returnType
		self.paramType = paramType
		self.getMem = getMem
		self.setMem = setMem
	def generateSet(self, var, value):
		return [ var + "." + self.setMem + " = " + value + ";" ]
	def generateGet(self, var, value):
		return [ "return " + var + "." + self.getMem + ";" ]

class GetSetInterface_Func(GetSetInterface):
	def __init__(self, suffix, returnType, paramType, getFunc, setFunc):
		GetSetInterface.__init__(self, suffix)
		self.getSetType = returnType
		self.returnType = returnType
		self.paramType = paramType
		self.setFunc = setFunc
		self.getFunc = getFunc
	def generateSet(self, var, value):
		return [ var + "." + self.setFunc + "(" + value + ");" ]
	def generateGet(self, var, value):
		return [ "return " + var + "." + self.getFunc + "();" ]

class GetSetInterface_GetFunc(GetSetInterface):
	def __init__(self, suffix, returnType, getFunc):
		GetSetInterface.__init__(self, suffix)
		self.getSetType = returnType
		self.returnType = returnType
		self.paramType = returnType
		self.getFunc = getFunc
		self.noset = 1
	def generateSet(self, var, value):
		return []
	def generateGet(self, var, value):
		return [ "return " + var + "." + self.getFunc + "();" ]

class Type:
	def __init__(self, name):
		self.tname = name
	def fullType(self):
		return self.tname
	def headers(self):
		return []
	def getSetInterfaces(self):
		return [GetSetInterface_Simple("", self.tname, self.tname, self.tname)]
	def addInterfaces(self):
		return []
	def allocator(self):
		return "default"
	def generateCopy(self, var, value):
		if self.allocator() == "copy":
			return [var + ".copy(" + value + ");"]
		else:
			return [var + " = " + value + ";"]
	def generateTransfert(self, var, value):
		if self.allocator() == "copy":
			return [var + ".transfert(" + value + ");"]
		else:
			return [var + " = " + value + ";"]
	def generateAssetPathOp(self, var, operation):
		return []
	def generateDeclaration(self, var):
		return [self.tname + " " + var + ";"]
	def tuaType(self):
		return ""
	def generateTuaDeclaration(self, var):
		return [ self.tuaType() + " " + var + ";" ]
	def numSubPathId(self):
		return 0
	def generateIterators(self, name, var):
		return []
	def subPathTypes(self):
		return []
	#def generateStorage(self, var):
	#def generateWrite(self, varSys, varTua):
	#def generateRead(self, varSys, varTua):

class TypeUInt8(Type):
	def __init__(self):
		Type.__init__(self, "uint8")
	def tuaType(self):
		return "tua_uint8"
	def getSetInterfaces(self):
		return [GetSetInterface_Simple("", self.tname, "uint32", self.tname)]
	def generateWrite(self, varSys, varTua):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteUInt8(" + varSys + ");"]
	def generateRead(self, varSys, varTua):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadUInt8(" + varTua + ");"]

class TypeUInt16(Type):
	def __init__(self):
		Type.__init__(self, "uint16")
	def tuaType(self):
		return "tua_uint16"
	def getSetInterfaces(self):
		return [GetSetInterface_Simple("", self.tname, "uint32", self.tname)]
	def generateWrite(self, varSys, varTua):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteUInt16(" + varSys + ");"]
	def generateRead(self, varSys, varTua):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadUInt16(" + varTua + ");"]

class TypeUInt32(Type):
	def __init__(self):
		Type.__init__(self, "uint32")
	def tuaType(self):
		return "tua_uint32"
	def getSetInterfaces(self):
		return [GetSetInterface_Simple("", self.tname, "uint32", self.tname)]
	def generateWrite(self, varSys, varTua):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteUInt32(" + varSys + ");"]
	def generateRead(self, varSys, varTua):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadUInt32(" + varTua + ");"]

class TypeInt32(Type):
	def __init__(self):
		Type.__init__(self, "int")
	def tuaType(self):
		return "tua_int32"
	def generateWrite(self, varSys, varTua):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteInt32(" + varSys + ");"]
	def generateRead(self, varSys, varTua):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadInt32(" + varTua + ");"]

class TypeBool(Type):
	def __init__(self):
		Type.__init__(self, "bool")
	def tuaType(self):
		return "tua_uint8"
	def generateWrite(self, varSys, varTua):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteBool(" + varSys + ");"]
	def generateRead(self, varSys, varTua):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadBool(" + varTua + ");"]

class TypeFloat(Type):
	def __init__(self):
		Type.__init__(self, "float")
	def tuaType(self):
		return "tua_float"
	def generateWrite(self, varSys, varTua):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteFloat(" + varSys + ");"]
	def generateRead(self, varSys, varTua):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadFloat(" + varTua + ");"]

class TypeVec2(Type):
	def __init__(self):
		Type.__init__(self, "vec2")
	def headers(self):
		return ["shared/math/vector.h"]
	def getSetInterfaces(self):
		return [
			GetSetInterface_Simple("", self.tname, self.tname, self.tname),
			GetSetInterface_Struct("X", "float", "float", "x", "x"),
			GetSetInterface_Struct("Y", "float", "float", "y", "y")
		]
	def tuaType(self):
		return "CTuaVec2"
	def generateWrite(self, varSys, varTua):
		return [
			varTua + ".m_X = pLoadingContext->ArchiveFile()->WriteFloat(" + varSys + ".x);",
			varTua + ".m_Y = pLoadingContext->ArchiveFile()->WriteFloat(" + varSys + ".y);"
		]
	def generateRead(self, varSys, varTua):
		return [
			varSys + ".x = pLoadingContext->ArchiveFile()->ReadFloat(" + varTua + ".m_X);",
			varSys + ".y = pLoadingContext->ArchiveFile()->ReadFloat(" + varTua + ".m_Y);"
		]

class TypeColor(Type):
	def __init__(self):
		Type.__init__(self, "vec4")
	def headers(self):
		return ["shared/math/vector.h"]
	def tuaType(self):
		return "tua_uint32"
	def generateWrite(self, varSys, varTua):
		return [varTua + " = pLoadingContext->ArchiveFile()->WriteColor(" + varSys + ");"]
	def generateRead(self, varSys, varTua):
		return [varSys + " = pLoadingContext->ArchiveFile()->ReadColor(" + varTua + ");"]

class TypeAssetPath(Type):
	def __init__(self):
		Type.__init__(self, "CAssetPath")
	def headers(self):
		return ["shared/assets/assetpath.h"]
	def getSetInterfaces(self):
		return [
			GetSetInterface_Simple("", self.tname, self.tname, "const "+self.tname+"&")
		]
	def tuaType(self):
		return "CAssetPath::CTuaType"
	def generateWrite(self, varSys, varTua):
		return ["pLoadingContext->WriteAssetPath("+varSys+", "+varTua+");"]
	def generateRead(self, varSys, varTua):
		return ["pLoadingContext->ReadAssetPath("+varTua+", "+varSys+");"]
	def generateAssetPathOp(self, var, operation):
		return [operation+".Apply("+var+");"]

class TypeSubPath(Type):
	def __init__(self):
		Type.__init__(self, "CSubPath")
	def headers(self):
		return ["shared/assets/assetpath.h"]
	def getSetInterfaces(self):
		return [
			GetSetInterface_Simple("", self.tname, self.tname, "const "+self.tname+"&")
		]
	def tuaType(self):
		return "CSubPath::CTuaType"
	def generateWrite(self, varSys, varTua):
		return ["CSubPath::CTuaType::Write(pLoadingContext->ArchiveFile(), "+varSys+", "+varTua+");"]
	def generateRead(self, varSys, varTua):
		return ["CSubPath::CTuaType::Read(pLoadingContext->ArchiveFile(), "+varTua+", "+varSys+");"]

class TypeString(Type):
	def __init__(self, size):
		Type.__init__(self, "string< _fixed_string_core<" + str(size) + "> >")
		self.size = size
	def headers(self):
		return ["shared/system/string.h"]
	def allocator(self):
		return "copy"
	def getSetInterfaces(self):
		return [
			GetSetInterface_Func("", "const char*", "const char*", "buffer", "copy")
		]
	def tuaType(self):
		return "tua_stringid"
	def generateWrite(self, varSys, varTua):
		return [varTua + " = pLoadingContext->ArchiveFile()->AddString(" + varSys + ".buffer());"]
	def generateRead(self, varSys, varTua):
		return [varSys + ".copy(pLoadingContext->ArchiveFile()->GetString(" + varTua + "));"]
		
class GetSetInterface_ArrayChild(GetSetInterface):
	def __init__(self, suffix, interface, t):
		GetSetInterface.__init__(self, suffix)
		self.interface = interface
		self.subpath = 1
		self.t = t
		self.getSetType = interface.getSetType
		self.returnType = interface.returnType
		self.paramType = interface.paramType
		self.depth = interface.depth + 1
		self.enum = interface.enum
		self.name = interface.name
		self.getbyref = interface.getbyref
		self.noset = interface.noset
	def generateSet(self, var, value):
		if self.interface.name:
			if self.interface.subpath > 0:
				return [ var + "[SubPath.GetId()].Set" + self.interface.name + "(SubPath.PopId(), " + value + ");" ]
			else:
				return [ var + "[SubPath.GetId()].Set" + self.interface.name + "(" + value + ");" ]
		else:
			return self.t.generateCopy(var + "[SubPath.GetId()]", value)
	def generateGet(self, var, value):
		if self.interface.name:
			if self.interface.subpath > 0:
				return [ "return " + var + "[SubPath.GetId()].Get" + self.interface.name + "(SubPath.PopId());" ]
			else:
				return [ "return " + var + "[SubPath.GetId()].Get" + self.interface.name + "();" ]
		else:
			return [ "return " + var + "[SubPath.GetId()];" ]
			
class AddInterface_Array(GetSetInterface):
	def __init__(self):
		self.name = ""
		self.subpath = 0
		self.enum = ""
	def generateAdd(self, var):
		return [ 
			"int Id = "+var+".size();",
			var + ".increment();",
			"return Id;"
		]
	def generateValid(self, var):
		return [ 
			"return (SubPath.GetId() >= 0 && SubPath.GetId() < "+var+".size());"
		]
			
class AddInterface_ArrayChild(GetSetInterface):
	def __init__(self, interface):
		self.interface = interface
		self.name = interface.name
		self.subpath = 1
		self.enum = interface.enum
	def generateAdd(self, var):
		return [ "return " + var + "[SubPath.GetId()].Add" + self.interface.name + "();" ]
	def generateValid(self, var):
		return [ "return (SubPath.GetId() >= 0 && SubPath.GetId() < "+var+".size() && " + var + "[SubPath.GetId()].IsValid" + self.interface.name + "(SubPath.PopId()));" ]
	
class TypeArray(Type):
	def __init__(self, t):
		Type.__init__(self, "array< "+t.fullType()+", allocator_"+t.allocator()+"<"+t.fullType()+"> >")
		self.t = t
	def numSubPathId(self):
		return 1
	def headers(self):
		return ["shared/tl/array.h"]
	def allocator(self):
		return "copy"
	def getSetInterfaces(self):
		res = [
			GetSetInterface_Func("ArraySize", "int", "int", "size", "resize"),
			GetSetInterface_GetFunc("Ptr", "const "+self.t.fullType()+"*", "base_ptr"),
			GetSetInterface_SimpleRef("Array", self.tname, self.tname)
		]
		for inter in self.t.getSetInterfaces():
			res.append(GetSetInterface_ArrayChild("", inter, self.t))
		return res
	def addInterfaces(self):
		res = [ AddInterface_Array() ]
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
	
	def tuaType(self):
		return "CTuaArray"
	def generateWrite(self, varSys, varTua):
		res = []
		res.append("{")
		res.append("	" + varTua + ".m_Size = "+varSys+".size();")
		res.append("	" + self.t.tuaType() + "* pData = new " + self.t.tuaType() + "["+varSys+".size()];")
		res.append("	for(int i=0; i<"+varSys+".size(); i++)")
		res.append("	{")
		for l in self.t.generateWrite(varSys+"[i]", "pData[i]"):
			res.append("		"+l)
		res.append("	}")
		res.append("	" + varTua + ".m_Data = pLoadingContext->ArchiveFile()->AddData((uint8*) pData, sizeof(" + self.t.tuaType() + ")*"+varSys+".size());")
		res.append("	delete[] pData;")
		res.append("}")
		return res
	def generateRead(self, varSys, varTua):
		res = []
		res.append("{")
		res.append("	const "+self.t.tuaType()+"* pData = (const "+self.t.tuaType()+"*) pLoadingContext->ArchiveFile()->GetData("+varTua+".m_Data);")
		res.append("	uint32 Size = pLoadingContext->ArchiveFile()->ReadUInt32("+varTua+".m_Size);")
		res.append("	"+varSys+".resize(Size);")
		res.append("	for(int i=0; i<Size; i++)")
		res.append("	{")
		for l in self.t.generateRead(varSys+"[i]", "pData[i]"):
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
				res.append("	for(int i=0; i<"+var+".size(); i++)")
				res.append("	{")
				res.append("		if("+operation+".MustBeDeleted("+var+"[i]))")
				res.append("			Shift++;")
				res.append("		else if(Shift > 0)")
				res.append("			"+var+"[i-Shift] = "+var+"[i];")
				res.append("	}")
				res.append("	"+var+".resize("+var+".size()-Shift);")
				res.append("}")
			
			res.append("for(int i=0; i<"+var+".size(); i++)")
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
	def generateGet(self, var, value):
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
	def generateValid(self, var):
		return [ "return " + var + ".get_clamp(SubPath.GetId(), SubPath.GetId2()).IsValid" + self.interface.name + "(SubPath.DoublePopId());" ]
		
class GetSetInterface_Array2dDim(GetSetInterface_Func):
	def __init__(self, suffix, returnType, paramType, getFunc, setFunc):
		GetSetInterface_Func.__init__(self, suffix, returnType, paramType, getFunc, setFunc)
	def generateSet(self, var, value):
		return [ var + "." + self.setFunc + "(max(" + value + ", 1));" ]
		
class TypeArray2d(Type):
	def __init__(self, t):
		Type.__init__(self, "array2d< "+t.tname+", allocator_"+t.allocator()+"<"+t.tname+"> >")
		self.t = t
	def numSubPathId(self):
		return 2
	def headers(self):
		return ["shared/tl/array2d.h"]
	def allocator(self):
		return "copy"
	def getSetInterfaces(self):
		res = [
			GetSetInterface_Array2dDim("Width", "int", "int", "get_width", "resize_width"),
			GetSetInterface_Array2dDim("Height", "int", "int", "get_height", "resize_height"),
			GetSetInterface_GetFunc("Ptr", "const "+self.t.fullType()+"*", "base_ptr"),
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
	def tuaType(self):
		return "CTuaArray2d"
	def generateWrite(self, varSys, varTua):
		res = []
		res.append("{")
		res.append("	" + varTua + ".m_Width = pLoadingContext->ArchiveFile()->WriteUInt32("+varSys+".get_width());")
		res.append("	" + varTua + ".m_Height = pLoadingContext->ArchiveFile()->WriteUInt32("+varSys+".get_height());")
		res.append("	" + varTua + ".m_Depth = pLoadingContext->ArchiveFile()->WriteUInt32("+varSys+".get_depth());")
		if self.t.fullType() == "uint8":
			res.append("	" + varTua + ".m_Data = pLoadingContext->ArchiveFile()->AddData((tua_uint8*) "+varSys+".base_ptr(), "+varSys+".get_linear_size());")
		else:
			res.append("	" + self.t.tuaType() + "* pData = new " + self.t.tuaType() + "["+varSys+".get_linear_size()];")
			res.append("	for(int i=0; i<"+varSys+".get_linear_size(); i++)")
			res.append("	{")
			for l in self.t.generateWrite(varSys+".linear_get_clamp(i)", "pData[i]"):
				res.append("		"+l)
			res.append("	}")
			res.append("	" + varTua + ".m_Data = pLoadingContext->ArchiveFile()->AddData((tua_uint8*) pData, sizeof(" + self.t.tuaType() + ")*"+varSys+".get_linear_size());")
			res.append("	delete[] pData;")
		res.append("}")
		return res
	def generateRead(self, varSys, varTua):
		res = []
		res.append("{")
		res.append("	const "+self.t.tuaType()+"* pData = (const "+self.t.tuaType()+"*) pLoadingContext->ArchiveFile()->GetData("+varTua+".m_Data);")
		res.append("	uint32 Width = pLoadingContext->ArchiveFile()->ReadUInt32("+varTua+".m_Width);")
		res.append("	uint32 Height = pLoadingContext->ArchiveFile()->ReadUInt32("+varTua+".m_Height);")
		res.append("	uint32 Depth = pLoadingContext->ArchiveFile()->ReadUInt32("+varTua+".m_Depth);")
		res.append("	int Size = Width * Height * Depth;")
		res.append("	"+varSys+".resize(Width, Height, Depth);")
		if self.t.fullType() == "uint8":
			res.append("	mem_copy((uint8*) "+varSys+".base_ptr(), pData, "+varSys+".get_linear_size());")
		else:
			res.append("	for(int i=0; i<Size; i++)")
			res.append("	{")
			res.append("		"+self.t.tname+" ReadedValue;" )
			for l in self.t.generateRead("ReadedValue", "pData[i]"):
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
	def generateValid(self, var):
		return [ "return " + var + ".IsValid" + self.interface.name + "();" ]
		
class Member:
	def __init__(self, name, t):
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
	def generateTuaDeclaration(self):
		return self.t.generateTuaDeclaration(self.memberName())
	def allocator(self):
		return self.t.allocator()
	def generateCopy(self, value):
		return self.t.generateCopy(self.memberName(), value)
	def generateTransfert(self, value):
		return self.t.generateTransfert(self.memberName(), value)
	def generateStorage(self, var):
		return self.t.generateStorage(self.memberName())
	def generateWrite(self):
		return self.t.generateWrite("SysType."+self.memberName(), "TuaType."+self.memberName())
	def generateRead(self):
		return self.t.generateRead("SysType."+self.memberName(), "TuaType."+self.memberName())
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
				if len(i.generateGet(self.memberName(), "Value")) > 1:
					res.append("inline const "+i.returnType+"& Get"+self.name+i.name+"("+params+") const")
					res.append("{")
					for l in i.generateGet(self.memberName(), "Value"):
						res.append("	"+l)
					res.append("}")
					res.append("inline "+i.returnType+"& Get"+self.name+i.name+"("+params+")")
					res.append("{")
					for l in i.generateGet(self.memberName(), "Value"):
						res.append("	"+l)
					res.append("}")
				else:
					res.append("inline const "+i.returnType+"& Get"+self.name+i.name+"("+params+") const { "+i.generateGet(self.memberName(), "Value")[0]+" }")
					res.append("inline "+i.returnType+"& Get"+self.name+i.name+"("+params+") { "+i.generateGet(self.memberName(), "Value")[0]+" }")
			else:
				if len(i.generateGet(self.memberName(), "Value")) > 1:
					res.append("inline "+i.returnType+" Get"+self.name+i.name+"("+params+") const")
					res.append("{")
					for l in i.generateGet(self.memberName(), "Value"):
						res.append("	"+l)
					res.append("}")
				else:
					res.append("inline "+i.returnType+" Get"+self.name+i.name+"("+params+") const { "+i.generateGet(self.memberName(), "Value")[0]+" }")	
			
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
		self.subpath = 1
		self.t = t
	def generateSet(self, var, value):
		return self.t.generateCopy(var + "[SubPath.GetId()]", value)
	def generateGet(self, var, value):
		return [ "return " + var + "[SubPath.GetId()];" ]

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
	def allocator(self):
		return "copy"
	def getSetInterfaces(self):
		res = [ GetSetInterface_Class(self) ]
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
	
	def tuaType(self):
		return self.fullType()+"::CTuaType"
	def generateWrite(self, varSys, varTua):
		return [self.fullType()+"::CTuaType::Write(pLoadingContext, "+varSys+", "+varTua+");"]
	def generateRead(self, varSys, varTua):
		return [self.fullType()+"::CTuaType::Read(pLoadingContext, "+varTua+", "+varSys+");"]
	def generateTuaType(self):
		res = []
		if self.inheritance:
			res.append("class CTuaType : public "+self.inheritance.fullType()+"::CTuaType")
		else:
			res.append("class CTuaType")
		
		res.append("{")
		res.append("public:")
		for Mem in self.members:
			for l in Mem.generateTuaDeclaration():
				res.append("	"+l)
		res.append("	static void Read(class CAssetsSaveLoadContext* pLoadingContext, const CTuaType& TuaType, "+self.fullType()+"& SysType);")
		res.append("	static void Write(class CAssetsSaveLoadContext* pLoadingContext, const "+self.fullType()+"& SysType, CTuaType& TuaType);")
		res.append("};")
		res.append("")
		
		return res
	def generateReadImpl(self):
		ns = self.fullType()+"::CTuaType::"
		
		res = []
		
		for c in self.classes:
			res.extend(c.generateReadImpl())
			res.append("")
			
		res.append("void "+ns+"Read(CAssetsSaveLoadContext* pLoadingContext, const CTuaType& TuaType, "+self.fullType()+"& SysType)")
		res.append("{")
		if self.inheritance:
			res.append("	"+self.inheritance.fullType()+"::CTuaType::Read(pLoadingContext, TuaType, SysType);")
			res.append("")
		for Mem in self.members:
			for l in Mem.generateRead():
				res.append("	"+l)
		res.append("}")
		return res
	
	def generateWriteImpl(self):
		ns = self.fullType()+"::CTuaType::"
		
		res = []
		
		for c in self.classes:
			res.extend(c.generateWriteImpl())
			res.append("")
			
		res.append("void "+ns+"Write(CAssetsSaveLoadContext* pLoadingContext, const "+self.fullType()+"& SysType, CTuaType& TuaType)")
		res.append("{")
		if self.inheritance:
			res.append("	"+self.inheritance.fullType()+"::CTuaType::Write(pLoadingContext, SysType, TuaType);")
			res.append("")
		for Mem in self.members:
			for l in Mem.generateWrite():
				res.append("	"+l)
		res.append("}")
		return res
		
	def generateAssetPathOp(self, var, operation):
		return [var+".AssetPathOperation("+operation+");"]
		
	def setParent(self, parent):
		self.parent = parent
	def setInheritance(self, inheritance):
		self.inheritance = inheritance
	def addMember(self, memberName, t, defaultValue=None):
		Mem = Member(memberName, t)
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
	def generateCopyFunc(self):
		res = [
			"void copy(const " + self.fullType() + "& Item)",
			"{"
		]
		if self.inheritance:
			res.append("	" + self.inheritance.fullType()+"::copy(Item);")
		for Mem in self.members:
			for l in Mem.generateCopy("Item." + Mem.memberName()):
				res.append("	" + l)
		res.append("}")
		res.append("")
		return res
	def generateTransfertFunc(self):
		res = [
			"void transfert(" + self.fullType() + "& Item)",
			"{"
		]
		if self.inheritance:
			res.append("	" + self.inheritance.fullType()+"::transfert(Item);")
		for Mem in self.members:
			for l in Mem.generateTransfert("Item." + Mem.memberName()):
				res.append("	" + l)
		res.append("}")
		res.append("")
		return res
	
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
		self.addPublicFunc(self.generateCopyFunc())
		self.addPublicFunc(self.generateTransfertFunc())
		
		for Mem in self.members:
			self.addPublicFunc(Mem.generateGet())
		
		for Mem in self.members:
			self.addPublicFunc(Mem.generateSet())
		
		for Mem in self.members:
			self.addPublicFunc(Mem.generateAdd())
		
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
	
	def generateClassDefinition(self):
		self.addPublicLines([
			"static const int TypeId = "+str(self.typeid)+";",
			""
		])
		self.addPublicLines(self.generateSubPath())
		self.addPublicLines(self.generateGetSetEnum())
		for Mem in self.members:
			self.addPublicLines(Mem.generateIterators())
			
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
		
		return Class.generateClassDefinition(self)

def generateHeader(asset):
	f = file("src/generated/assets/" + asset.name.lower() + ".h", "w")
	print >>f, "/*"
	print >>f, " * Copyright (C) 2016 necropotame (necropotame@gmail.com)"
	print >>f, " * "
	print >>f, " * This file is part of TeeUniverses."
	print >>f, " * "
	print >>f, " * TeeUniverses is free software: you can redistribute it and/or  modify"
	print >>f, " * it under the terms of the GNU Affero General Public License, version 3,"
	print >>f, " * as published by the Free Software Foundation."
	print >>f, " *"
	print >>f, " * TeeUniverses is distributed in the hope that it will be useful,"
	print >>f, " * but WITHOUT ANY WARRANTY; without even the implied warranty of"
	print >>f, " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
	print >>f, " * GNU Affero General Public License for more details."
	print >>f, " *"
	print >>f, " * You should have received a copy of the GNU Affero General Public License"
	print >>f, " * along with TeeUniverses.  If not, see <http://www.gnu.org/licenses/>."
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
	print >>f, " * This file is part of TeeUniverses."
	print >>f, " * "
	print >>f, " * TeeUniverses is free software: you can redistribute it and/or  modify"
	print >>f, " * it under the terms of the GNU Affero General Public License, version 3,"
	print >>f, " * as published by the Free Software Foundation."
	print >>f, " *"
	print >>f, " * TeeUniverses is distributed in the hope that it will be useful,"
	print >>f, " * but WITHOUT ANY WARRANTY; without even the implied warranty of"
	print >>f, " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
	print >>f, " * GNU Affero General Public License for more details."
	print >>f, " *"
	print >>f, " * You should have received a copy of the GNU Affero General Public License"
	print >>f, " * along with TeeUniverses.  If not, see <http://www.gnu.org/licenses/>."
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
	for l in asset.generateReadImpl():
		print >>f, l
	print >>f, ""
	for l in asset.generateWriteImpl():
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
	print >>f, ""
	

assetsList = []

mainAsset = Class("Asset")

# CHARACTER ############################################################
character_part = Class("Part")
character_part.addMember("Name", TypeString(128))
character_part.addMember("DefaultPath", TypeAssetPath())

character = ClassAsset("Character", len(assetsList))
character.setInheritance(mainAsset)
character.addClass(character_part)
character.addMember("IdlePath", TypeAssetPath())
character.addMember("WalkPath", TypeAssetPath())
character.addMember("ControlledJumpPath", TypeAssetPath())
character.addMember("UncontrolledJumpPath", TypeAssetPath())
character.addMember("Part", TypeArray(character_part))

assetsList.append(character)

# CHARACTER PART #######################################################
characterPart = ClassAsset("CharacterPart", len(assetsList))
characterPart.setInheritance(mainAsset)
characterPart.addMember("CharacterPath", TypeAssetPath())
characterPart.addMember("CharacterPart", TypeSubPath())
characterPart.addMember("SkeletonSkinPath", TypeAssetPath())

assetsList.append(characterPart)

# GUI BOX STYLE ########################################################
guiBoxStyle = ClassAsset("GuiBoxStyle", len(assetsList))
guiBoxStyle.setInheritance(mainAsset)
guiBoxStyle.addMember("MinWidth", TypeInt32(), "0")
guiBoxStyle.addMember("MinHeight", TypeInt32(), "0")
guiBoxStyle.addMember("Margin", TypeInt32(), "0")
guiBoxStyle.addMember("Padding", TypeInt32(), "0")
guiBoxStyle.addMember("Spacing", TypeInt32(), "0")
guiBoxStyle.addMember("RectPath", TypeAssetPath())

assetsList.append(guiBoxStyle)

# GUI BUTTON STYLE #####################################################
guiButtonStyle = ClassAsset("GuiButtonStyle", len(assetsList))
guiButtonStyle.setInheritance(mainAsset)
guiButtonStyle.addMember("IdleStylePath", TypeAssetPath())
guiButtonStyle.addMember("MouseOverStylePath", TypeAssetPath())
guiButtonStyle.addMember("ReadOnlyStylePath", TypeAssetPath())

assetsList.append(guiButtonStyle)

# GUI COLOR EDIT STYLE #################################################
guiColorEditStyle = ClassAsset("GuiColorEditStyle", len(assetsList))
guiColorEditStyle.setInheritance(mainAsset)
guiColorEditStyle.addMember("ButtonStylePath", TypeAssetPath())
guiColorEditStyle.addMember("PopupStylePath", TypeAssetPath())
guiColorEditStyle.addMember("RGBIconPath", TypeAssetPath())
guiColorEditStyle.addMember("HSVIconPath", TypeAssetPath())
guiColorEditStyle.addMember("SquareIconPath", TypeAssetPath())
guiColorEditStyle.addMember("WheelIconPath", TypeAssetPath())

assetsList.append(guiColorEditStyle)

# GUI LABEL STYLE ######################################################
guiLabelStyle = ClassAsset("GuiLabelStyle", len(assetsList))
guiLabelStyle.setInheritance(mainAsset)
guiLabelStyle.addMember("MinWidth", TypeInt32(), "0")
guiLabelStyle.addMember("MinHeight", TypeInt32(), "0")
guiLabelStyle.addMember("Margin", TypeInt32(), "0")
guiLabelStyle.addMember("Padding", TypeInt32(), "0")
guiLabelStyle.addMember("Spacing", TypeInt32(), "0")
guiLabelStyle.addMember("FontSize", TypeInt32(), "12")
guiLabelStyle.addMember("TextColor", TypeColor(), "1.0f")
guiLabelStyle.addMember("TextAlignment", TypeInt32(), "TEXTALIGNMENT_LEFT")
guiLabelStyle.addMember("RectPath", TypeAssetPath())
guiLabelStyle.addMember("IconPath", TypeAssetPath())
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
guiIntEditStyle.addMember("IncreaseButtonStylePath", TypeAssetPath())
guiIntEditStyle.addMember("DecreaseButtonStylePath", TypeAssetPath())

assetsList.append(guiIntEditStyle)

# GUI LINE STYLE #######################################################
guiLineStyle = ClassAsset("GuiLineStyle", len(assetsList))
guiLineStyle.setInheritance(mainAsset)
guiLineStyle.addMember("Flags", TypeInt32(), "0x0")
guiLineStyle.addMember("BorderColor", TypeColor(), "1.0f")
guiLineStyle.addMember("ImageLPath", TypeAssetPath())
guiLineStyle.addMember("ImageMPath", TypeAssetPath())
guiLineStyle.addMember("ImageRPath", TypeAssetPath())
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
guiRectStyle.addMember("Flags", TypeInt32(), "0x0")
guiRectStyle.addMember("BackgroundColor", TypeColor(), "1.0f")
guiRectStyle.addMember("BackgroundPadding", TypeFloat(), "0")
guiRectStyle.addMember("BorderColor", TypeColor(), "1.0f")
guiRectStyle.addMember("BorderFlags", TypeInt32(), "0x0")
guiRectStyle.addMember("CornerRadius", TypeFloat(), "0")
guiRectStyle.addMember("CornerFlags", TypeInt32(), "0x0")
guiRectStyle.addMember("ImagePadding", TypeInt32(), "0")
guiRectStyle.addMember("ImageTPath", TypeAssetPath())
guiRectStyle.addMember("ImageRPath", TypeAssetPath())
guiRectStyle.addMember("ImageBPath", TypeAssetPath())
guiRectStyle.addMember("ImageLPath", TypeAssetPath())
guiRectStyle.addMember("ImageTRPath", TypeAssetPath())
guiRectStyle.addMember("ImageBRPath", TypeAssetPath())
guiRectStyle.addMember("ImageBLPath", TypeAssetPath())
guiRectStyle.addMember("ImageTLPath", TypeAssetPath())
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
guiScrollbarStyle.addMember("RectPath", TypeAssetPath())
guiScrollbarStyle.addMember("DefaultRailPath", TypeAssetPath())
guiScrollbarStyle.addMember("DefaultSliderPath", TypeAssetPath())
guiScrollbarStyle.addMember("MouseOverSliderPath", TypeAssetPath())
guiScrollbarStyle.addMember("Margin", TypeInt32(), "0")
guiScrollbarStyle.addMember("Padding", TypeInt32(), "0")

assetsList.append(guiScrollbarStyle)

# GUI SCROLLBAR STYLE ##################################################
guiSliderStyle = ClassAsset("GuiSliderStyle", len(assetsList))
guiSliderStyle.setInheritance(mainAsset)
guiSliderStyle.addMember("RectPath", TypeAssetPath())
guiSliderStyle.addMember("DefaultRailPath", TypeAssetPath())
guiSliderStyle.addMember("CursorPath", TypeAssetPath())
guiSliderStyle.addMember("Margin", TypeInt32(), "0")
guiSliderStyle.addMember("Padding", TypeInt32(), "0")

assetsList.append(guiSliderStyle)

# GUI TABS STYLE #######################################################
guiTabsStyle = ClassAsset("GuiTabsStyle", len(assetsList))
guiTabsStyle.setInheritance(mainAsset)
guiTabsStyle.addMember("LayoutPath", TypeAssetPath())
guiTabsStyle.addMember("ContentPath", TypeAssetPath())
guiTabsStyle.addMember("ButtonListPath", TypeAssetPath())
guiTabsStyle.addMember("InactiveButtonPath", TypeAssetPath())
guiTabsStyle.addMember("ActiveButtonPath", TypeAssetPath())
guiTabsStyle.addMember("ButtonListFill", TypeBool(), "false")
guiTabsStyle.addMember("ButtonListText", TypeBool(), "false")

assetsList.append(guiTabsStyle)

# GUI TOGGLE STYLE #####################################################
guiToggleStyle = ClassAsset("GuiToggleStyle", len(assetsList))
guiToggleStyle.setInheritance(mainAsset)
guiToggleStyle.addMember("IdleTrueStylePath", TypeAssetPath())
guiToggleStyle.addMember("MouseOverTrueStylePath", TypeAssetPath())
guiToggleStyle.addMember("IconTruePath", TypeAssetPath())
guiToggleStyle.addMember("IdleFalseStylePath", TypeAssetPath())
guiToggleStyle.addMember("MouseOverFalseStylePath", TypeAssetPath())
guiToggleStyle.addMember("IconFalsePath", TypeAssetPath())
guiToggleStyle.addMember("SwitchIcon", TypeBool(), "false")

assetsList.append(guiToggleStyle)

# IMAGE ################################################################
image = ClassAsset("Image", len(assetsList))
image.setInheritance(mainAsset)
image.addMember("GridWidth", TypeInt32(), "1")
image.addMember("GridHeight", TypeInt32(), "1")
image.addMember("GridSpacing", TypeInt32(), "0")
image.addMember("TexelSize", TypeInt32(), "1024")
image.addMember("TilingEnabled", TypeBool(), "false")
image.addMember("Data", TypeArray2d(TypeUInt8()))
image.addHeader("shared/graphics.h")
image.addPrivateLines([
	"CTextureHandle m_Texture;",
	""
])
image.addPublicFunc([
	"inline CTextureHandle GetTexture() const { return m_Texture; }",
	"inline void SetTexture(CTextureHandle Texture) { m_Texture = Texture; }"
	"vec4 GetColor(int x, int y) const;"
	"vec4 Sample(vec2 UV) const;"
])

assetsList.append(image)

# MAP ##################################################################
_map = ClassAsset("Map", len(assetsList))
_map.setInheritance(mainAsset)
_map.addMember("BgGroup", TypeArray(TypeAssetPath()))
_map.addMember("FgGroup", TypeArray(TypeAssetPath()))
_map.addMember("ZoneLayer", TypeArray(TypeAssetPath()))
_map.addMember("EntityLayer", TypeArray(TypeAssetPath()))

assetsList.append(_map)

# MAP GROUP ############################################################
mapGroup = ClassAsset("MapGroup", len(assetsList))
mapGroup.setInheritance(mainAsset)
mapGroup.addMember("ParentPath", TypeAssetPath())
mapGroup.addMember("Layer", TypeArray(TypeAssetPath()))
mapGroup.addMember("Position", TypeVec2(), "vec2(0.0f, 0.0f)")
mapGroup.addMember("HardParallax", TypeVec2(), "vec2(1.0f, 1.0f)")
mapGroup.addMember("Clipping", TypeBool(), "false")
mapGroup.addMember("ClipPosition", TypeVec2(), "vec2(0.0f, 0.0f)")
mapGroup.addMember("ClipSize", TypeVec2(), "vec2(64.0f, 64.0f)")
mapGroup.addMember("Visibility", TypeBool(), "true")

assetsList.append(mapGroup)

# MAP LAYER QUADS ######################################################
mapLayerQuads_quad = Class("Quad")
mapLayerQuads_quad.addMember("Pivot", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("Size", TypeVec2(), "1.0f")
mapLayerQuads_quad.addMember("Angle", TypeFloat(), "0.0f")
mapLayerQuads_quad.addMember("Vertex0", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("Vertex1", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("Vertex2", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("Vertex3", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("UV0", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("UV1", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("UV2", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("UV3", TypeVec2(), "0.0f")
mapLayerQuads_quad.addMember("Color0", TypeColor(), "1.0f")
mapLayerQuads_quad.addMember("Color1", TypeColor(), "1.0f")
mapLayerQuads_quad.addMember("Color2", TypeColor(), "1.0f")
mapLayerQuads_quad.addMember("Color3", TypeColor(), "1.0f")
mapLayerQuads_quad.addMember("AnimationPath", TypeAssetPath())
mapLayerQuads_quad.addPublicFunc([
	"void GetTransform(CAssetsManager* pAssetsManager, float Time, matrix2x2* pMatrix, vec2* pPosition) const;",
	"void GetDrawState(CAssetsManager* pAssetsManager, float Time, vec4* pColor, int* pState) const;"
])

mapLayerQuads = ClassAsset("MapLayerQuads", len(assetsList))
mapLayerQuads.setInheritance(mainAsset)
mapLayerQuads.addClass(mapLayerQuads_quad)
mapLayerQuads.addMember("ParentPath", TypeAssetPath())
mapLayerQuads.addMember("ImagePath", TypeAssetPath())
mapLayerQuads.addMember("Quad", TypeArray(mapLayerQuads_quad))
mapLayerQuads.addPublicFunc([
	"void GetQuadTransform(const CSubPath& SubPath, float Time, matrix2x2* pMatrix, vec2* pPosition) const;",
	"void GetQuadDrawState(const CSubPath& SubPath, float Time, vec4* pColor, int* pState) const;"
])
mapLayerQuads.addMember("Visibility", TypeBool(), "true")

assetsList.append(mapLayerQuads)

# MAP LAYER TILES ######################################################
mapLayerTiles_tile = Class("Tile")
mapLayerTiles_tile.addMember("Index", TypeUInt8(), "0")
mapLayerTiles_tile.addMember("Flags", TypeUInt8(), "0x0")

mapLayerTiles = ClassAsset("MapLayerTiles", len(assetsList))
mapLayerTiles.setInheritance(mainAsset)
mapLayerTiles.addClass(mapLayerTiles_tile)
mapLayerTiles.addMember("ParentPath", TypeAssetPath())
mapLayerTiles.addMember("ImagePath", TypeAssetPath())
mapLayerTiles.addMember("Color", TypeColor(), "1.0f")
mapLayerTiles.addMember("Tile", TypeArray2d(mapLayerTiles_tile))
mapLayerTiles.addMember("Visibility", TypeBool(), "true")

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
mapZoneTiles_tile.addMember("Index", TypeUInt8(), "0")

mapZoneTiles = ClassAsset("MapZoneTiles", len(assetsList))
mapZoneTiles.setInheritance(mainAsset)
mapZoneTiles.addClass(mapZoneTiles_tile)
mapZoneTiles.addMember("ParentPath", TypeAssetPath())
mapZoneTiles.addMember("ZoneTypePath", TypeAssetPath())
mapZoneTiles.addMember("Tile", TypeArray2d(mapZoneTiles_tile))
mapZoneTiles.addMember("Visibility", TypeBool(), "true")

assetsList.append(mapZoneTiles)

# MAP ENTITIES #########################################################
mapEntities_entity = Class("Entity")
mapEntities_entity.addMember("TypePath", TypeAssetPath())
mapEntities_entity.addMember("Position", TypeVec2(), "0.0f")

mapEntities = ClassAsset("MapEntities", len(assetsList))
mapEntities.setInheritance(mainAsset)
mapEntities.addClass(mapEntities_entity)
mapEntities.addMember("ParentPath", TypeAssetPath())
mapEntities.addMember("Entity", TypeArray(mapEntities_entity))
mapEntities.addMember("Visibility", TypeBool(), "true")

assetsList.append(mapEntities)

# ZONE TYPE ############################################################
zoneType_index = Class("Index")
zoneType_index.addMember("Used", TypeBool(), "true")
zoneType_index.addMember("Description", TypeString(128))
zoneType_index.addMember("Color", TypeColor(), "1.0f")

zoneType = ClassAsset("ZoneType", len(assetsList))
zoneType.setInheritance(mainAsset)
zoneType.addClass(zoneType_index)
zoneType.addMember("Index", TypeArray(zoneType_index))

assetsList.append(zoneType)

# ENTITY TYPE ##########################################################
entityType = ClassAsset("EntityType", len(assetsList))
entityType.setInheritance(mainAsset)
entityType.addMember("GizmoPath", TypeAssetPath())
entityType.addMember("CollisionRadius", TypeFloat())

assetsList.append(entityType)

# SKELETON #############################################################
skeleton_bone = Class("Bone")
skeleton_bone.addMember("Length", TypeFloat(), "32.0f")
skeleton_bone.addMember("Anchor", TypeFloat(), "0.0f")
skeleton_bone.addMember("Translation", TypeVec2(), "0.0f")
skeleton_bone.addMember("Scale", TypeVec2(), "1.0f")
skeleton_bone.addMember("Angle", TypeFloat(), "0.0f")
skeleton_bone.addMember("Name", TypeString(128))
skeleton_bone.addMember("Color", TypeColor(), "1.0f")

skeleton_layer = Class("Layer")
skeleton_layer.addMember("Name", TypeString(128))

skeleton = ClassAsset("Skeleton", len(assetsList))
skeleton.setInheritance(mainAsset)
skeleton.addClass(skeleton_bone)
skeleton.addClass(skeleton_layer)
skeleton.addMember("ParentPath", TypeAssetPath())
skeleton.addMember("DefaultSkinPath", TypeAssetPath())
skeleton.addMember("Bone", TypeArray(skeleton_bone))
skeleton.addMember("Layer", TypeArray(skeleton_layer))

assetsList.append(skeleton)

# SKELETON ANIMATION ###################################################
skeletonAnim_boneAnim_frame = Class("Frame")
skeletonAnim_boneAnim_frame.addMember("Translation", TypeVec2(), "0.0f")
skeletonAnim_boneAnim_frame.addMember("Scale", TypeVec2(), "1.0f")
skeletonAnim_boneAnim_frame.addMember("Angle", TypeFloat(), "0.0f")
skeletonAnim_boneAnim_frame.addMember("Alignment", TypeInt32(), "BONEALIGN_PARENTBONE")

skeletonAnim_boneAnim_keyframe = Class("KeyFrame")
skeletonAnim_boneAnim_keyframe.setInheritance(skeletonAnim_boneAnim_frame)
skeletonAnim_boneAnim_keyframe.addMember("Time", TypeInt32(), "0")

skeletonAnim_boneAnim = Class("BoneAnimation")
skeletonAnim_boneAnim.addClass(skeletonAnim_boneAnim_frame)
skeletonAnim_boneAnim.addClass(skeletonAnim_boneAnim_keyframe)
skeletonAnim_boneAnim.addMember("KeyFrame", TypeArray(skeletonAnim_boneAnim_keyframe))
skeletonAnim_boneAnim.addMember("BonePath", TypeSubPath())
skeletonAnim_boneAnim.addMember("CycleType", TypeInt32(), "CYCLETYPE_CLAMP")
skeletonAnim_boneAnim.addPublicFunc([
	"float GetDuration() const;",
	"int IntTimeToKeyFrame(int IntTime) const;",
	"int TimeToKeyFrame(float Time) const;",
	"bool GetFrame(float Time, CFrame& Frame) const;"
	""
])

skeletonAnim_layerAnim_frame = Class("Frame")
skeletonAnim_layerAnim_frame.addMember("Color", TypeColor(), "1.0f")
skeletonAnim_layerAnim_frame.addMember("State", TypeInt32(), "LAYERSTATE_VISIBLE")

skeletonAnim_layerAnim_keyframe = Class("KeyFrame")
skeletonAnim_layerAnim_keyframe.setInheritance(skeletonAnim_layerAnim_frame)
skeletonAnim_layerAnim_keyframe.addMember("Time", TypeInt32(), "0")

skeletonAnim_layerAnim = Class("LayerAnimation")
skeletonAnim_layerAnim.addClass(skeletonAnim_layerAnim_frame)
skeletonAnim_layerAnim.addClass(skeletonAnim_layerAnim_keyframe)
skeletonAnim_layerAnim.addMember("KeyFrame", TypeArray(skeletonAnim_layerAnim_keyframe))
skeletonAnim_layerAnim.addMember("LayerPath", TypeSubPath())
skeletonAnim_layerAnim.addMember("CycleType", TypeInt32(), "CYCLETYPE_CLAMP")
skeletonAnim_layerAnim.addPublicFunc([
	"float GetDuration() const;",
	"int IntTimeToKeyFrame(int IntTime) const;",
	"int TimeToKeyFrame(float Time) const;",
	"bool GetFrame(float Time, CFrame& Frame) const;"
	""
])

skeletonAnim = ClassAsset("SkeletonAnimation", len(assetsList))
skeletonAnim.setInheritance(mainAsset)
skeletonAnim.addClass(skeletonAnim_boneAnim)
skeletonAnim.addClass(skeletonAnim_layerAnim)
skeletonAnim.addMember("SkeletonPath", TypeAssetPath())
skeletonAnim.addMember("LocalBoneAnim", TypeArray(skeletonAnim_boneAnim))
skeletonAnim.addMember("ParentBoneAnim", TypeArray(skeletonAnim_boneAnim))
skeletonAnim.addMember("LayerAnimation", TypeArray(skeletonAnim_layerAnim))

skeletonAnim.addPublicLines([
	"enum",
	"{",
	"	TIMESTEP = 30,",
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
	"	BONEALIGN_AIM,",
	"	BONEALIGN_MOTION,",
	"	BONEALIGN_HOOK,",
	"	NUM_BONEALIGNS,",
	"};",
	""
])
skeletonAnim.addPublicFunc([
	"bool GetLocalBoneAnimFrame(int Id, float Time, CBoneAnimation::CFrame& Frame) const;",
	"bool GetParentBoneAnimFrame(int Id, float Time, CBoneAnimation::CFrame& Frame) const;",
	"bool GetLayerAnimFrame(int Id, float Time, CLayerAnimation::CFrame& Frame) const;"
])

assetsList.append(skeletonAnim)

# SKELETON SKIN ########################################################
skeletonSkin_sprite = Class("Bone")
skeletonSkin_sprite.addMember("SpritePath", TypeAssetPath())
skeletonSkin_sprite.addMember("BonePath", TypeSubPath())
skeletonSkin_sprite.addMember("LayerPath", TypeSubPath())
skeletonSkin_sprite.addMember("Anchor", TypeFloat(), "0.0f")
skeletonSkin_sprite.addMember("Translation", TypeVec2(), "0.0f")
skeletonSkin_sprite.addMember("Scale", TypeVec2(), "1.0f")
skeletonSkin_sprite.addMember("Angle", TypeFloat(), "0.0f")
skeletonSkin_sprite.addMember("Color", TypeColor(), "1.0f")
skeletonSkin_sprite.addMember("Alignment", TypeInt32(), "ALIGNMENT_BONE")
skeletonSkin = ClassAsset("SkeletonSkin", len(assetsList))
skeletonSkin.setInheritance(mainAsset)
skeletonSkin.addClass(skeletonSkin_sprite)
skeletonSkin.addMember("SkeletonPath", TypeAssetPath())
skeletonSkin.addMember("Sprite", TypeArray(skeletonSkin_sprite))

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
sprite.addMember("ImagePath", TypeAssetPath())
sprite.addMember("X", TypeInt32(), "0")
sprite.addMember("Y", TypeInt32(), "0")
sprite.addMember("Width", TypeInt32(), "1")
sprite.addMember("Height", TypeInt32(), "1")
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
weapon.addMember("CharacterPath", TypeAssetPath())
weapon.addMember("CursorPath", TypeAssetPath())
weapon.addMember("SkinPath", TypeAssetPath())
weapon.addMember("AttackAnimationPath", TypeAssetPath())

assetsList.append(weapon)

#########################################

f = file("src/generated/assets/allassets.h", "w")
print >>f, "/*"
print >>f, " * Copyright (C) 2016 necropotame (necropotame@gmail.com)"
print >>f, " * "
print >>f, " * This file is part of TeeUniverses."
print >>f, " * "
print >>f, " * TeeUniverses is free software: you can redistribute it and/or  modify"
print >>f, " * it under the terms of the GNU Affero General Public License, version 3,"
print >>f, " * as published by the Free Software Foundation."
print >>f, " *"
print >>f, " * TeeUniverses is distributed in the hope that it will be useful,"
print >>f, " * but WITHOUT ANY WARRANTY; without even the implied warranty of"
print >>f, " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
print >>f, " * GNU Affero General Public License for more details."
print >>f, " *"
print >>f, " * You should have received a copy of the GNU Affero General Public License"
print >>f, " * along with TeeUniverses.  If not, see <http://www.gnu.org/licenses/>."
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
print >>f, " * This file is part of TeeUniverses."
print >>f, " * "
print >>f, " * TeeUniverses is free software: you can redistribute it and/or  modify"
print >>f, " * it under the terms of the GNU Affero General Public License, version 3,"
print >>f, " * as published by the Free Software Foundation."
print >>f, " *"
print >>f, " * TeeUniverses is distributed in the hope that it will be useful,"
print >>f, " * but WITHOUT ANY WARRANTY; without even the implied warranty of"
print >>f, " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
print >>f, " * GNU Affero General Public License for more details."
print >>f, " *"
print >>f, " * You should have received a copy of the GNU Affero General Public License"
print >>f, " * along with TeeUniverses.  If not, see <http://www.gnu.org/licenses/>."
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
