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

#ifndef __CLIENT_GUI_INTEGEREDIT__
#define __CLIENT_GUI_INTEGEREDIT__

#include "listlayout.h"
#include "text-edit.h"
#include "button.h"
	
namespace gui
{

class CAbstractIntegerEdit : public CHListLayout
{
	class CEntry : public CAbstractTextEdit
	{
	protected:
		CAbstractIntegerEdit* m_pIntegerEdit;
		virtual void SaveFromTextBuffer();
		virtual void CopyToTextBuffer();
		
	public:
		CEntry(CAbstractIntegerEdit *pIntegerEdit);
	};
	
	class CDecreaseButton : public CButton
	{
	protected:
		CAbstractIntegerEdit* m_pIntegerEdit;
		virtual void MouseClickAction();
		
	public:
		CDecreaseButton(CAbstractIntegerEdit *pIntegerEdit);
	};
	
	class CIncreaseButton : public CButton
	{
	protected:
		CAbstractIntegerEdit* m_pIntegerEdit;
		virtual void MouseClickAction();
		
	public:
		CIncreaseButton(CAbstractIntegerEdit *pIntegerEdit);
	};
	
protected:
	CEntry* m_pEntry;
	CButton* m_pDecreaseButton;
	CButton* m_pIncreaseButton;
	CAssetPath m_IntEditStylePath;

protected:
	virtual int GetValue() const = 0;
	virtual void SetValue(int Value) = 0;
	
public:
	CAbstractIntegerEdit(class CGui *pConfig);
	
	void Editable(bool Value);
	
	void SetIntEditStyle(CAssetPath Path);
	inline CAssetPath GetIntEditStyle() const { return m_IntEditStylePath; }
};

class CIntegerEdit : public CAbstractIntegerEdit
{
protected:
	int m_Value;

protected:
	virtual int GetValue() const;
	virtual void SetValue(int Value);
	
public:
	CIntegerEdit(class CGui *pConfig, int DefaultValue);
};

class CExternalIntegerEdit : public CAbstractIntegerEdit
{
protected:
	int* m_pMemory;

protected:
	virtual int GetValue() const;
	virtual void SetValue(int Value);
	
public:
	CExternalIntegerEdit(class CGui *pConfig, int* pMemory);
};

class CBoundedExternalIntegerEdit : public CAbstractIntegerEdit
{
protected:
	int* m_pMemory;
	int m_MinVal;
	int m_MaxVal;

protected:
	virtual int GetValue() const;
	virtual void SetValue(int Value);
	
public:
	CBoundedExternalIntegerEdit(class CGui *pConfig, int* pMemory, int MinVal, int MaxVal);
};

}

#endif
