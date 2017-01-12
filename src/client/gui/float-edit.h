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

#ifndef __CLIENT_GUI_FLOATEDIT__
#define __CLIENT_GUI_FLOATEDIT__

#include "text-edit.h"
	
namespace gui
{

class CAbstractFloatEdit : public CAbstractTextEdit
{
protected:
	float m_FloatValue;
	bool m_Percent;
	
	virtual void SaveFromTextBuffer();
	virtual void CopyToTextBuffer();
	
	virtual float GetValue() const = 0;
	virtual void SetValue(float Value) = 0;
	
public:
	CAbstractFloatEdit(class CGui *pConfig);
	
	inline void Percent() { m_Percent = true; }
};

class CFloatEdit : public CAbstractFloatEdit
{
protected:
	float m_FloatValue;
	
	virtual float GetValue() const;
	virtual void SetValue(float Value);
	
public:
	CFloatEdit(class CGui *pConfig, float DefaultValue);
};

class CExternalFloatEdit : public CAbstractFloatEdit
{
protected:
	float* m_pFloat;
	
	virtual float GetValue() const;
	virtual void SetValue(float Value);
	
public:
	CExternalFloatEdit(class CGui *pConfig, float* m_pFloat);
};

}

#endif
