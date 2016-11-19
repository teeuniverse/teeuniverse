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

#include <client/components/assetsrenderer.h>
#include <client/components/graphics.h>
#include <client/components/input.h>
#include <client/keys.h>

#include "float-edit.h"
	
namespace gui
{

/* ABSTRACT FLOAT EDIT ************************************************/

CAbstractFloatEdit::CAbstractFloatEdit(CGui* pContext) :
	CAbstractTextEdit(pContext)
{
	SetLabelStyle(Context()->GetNumericEntryStyle());
}

void CAbstractFloatEdit::SaveFromTextBuffer()
{
	SetValue(atof(GetText()));
}

void CAbstractFloatEdit::CopyToTextBuffer()
{
	str_format(m_aText, sizeof(m_aText), "%.02f", GetValue());
	OnTextUpdated();
}

/* FLOAT EDIT *********************************************************/

CFloatEdit::CFloatEdit(CGui* pContext, float DefaultValue) :
	CAbstractFloatEdit(pContext),
	m_FloatValue(DefaultValue)
{
	
}

float CFloatEdit::GetValue() const
{
	return m_FloatValue;
}

void CFloatEdit::SetValue(float Value)
{
	m_FloatValue = Value;
}

/* EXTERNAL FLOAT EDIT ************************************************/

CExternalFloatEdit::CExternalFloatEdit(CGui* pContext, float* pFloat) :
	CAbstractFloatEdit(pContext),
	m_pFloat(pFloat)
{
	
}

float CExternalFloatEdit::GetValue() const
{
	return *m_pFloat;
}

void CExternalFloatEdit::SetValue(float Value)
{
	*m_pFloat = Value;
}

}
