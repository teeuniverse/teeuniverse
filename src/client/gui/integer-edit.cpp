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

#include <client/components/assetsrenderer.h>
#include <client/components/graphics.h>

#include "integer-edit.h"
	
namespace gui
{

/* ABSTRACT INTEGER EDIT **********************************************/

	//Entry
CAbstractIntegerEdit::CEntry::CEntry(CAbstractIntegerEdit *pIntegerEdit) :
	CAbstractTextEdit(pIntegerEdit->Context()),
	m_pIntegerEdit(pIntegerEdit)
{
	
}

void CAbstractIntegerEdit::CEntry::SaveFromTextBuffer()
{
	int Value = Localization()->ParseInteger(NULL, GetText());
	m_pIntegerEdit->SetValue(Value);
}

void CAbstractIntegerEdit::CEntry::CopyToTextBuffer()
{
	CLocalizableString LString("{int:v}");
	LString.AddInteger("v", m_pIntegerEdit->GetValue());
	SetText(LString);
}

	//Decrease Button
CAbstractIntegerEdit::CDecreaseButton::CDecreaseButton(CAbstractIntegerEdit *pIntegerEdit) :
	CButton(pIntegerEdit->Context(), ""),
	m_pIntegerEdit(pIntegerEdit)
{
	
}

void CAbstractIntegerEdit::CDecreaseButton::MouseClickAction()
{
	m_pIntegerEdit->SetValue(m_pIntegerEdit->GetValue()-1);
	m_pIntegerEdit->m_pEntry->RemoveChanges();
}

	//Increase Button
CAbstractIntegerEdit::CIncreaseButton::CIncreaseButton(CAbstractIntegerEdit *pIntegerEdit) :
	CButton(pIntegerEdit->Context(), ""),
	m_pIntegerEdit(pIntegerEdit)
{
	
}

void CAbstractIntegerEdit::CIncreaseButton::MouseClickAction()
{
	m_pIntegerEdit->SetValue(m_pIntegerEdit->GetValue()+1);
	m_pIntegerEdit->m_pEntry->RemoveChanges();
}

	//Integer Edit
CAbstractIntegerEdit::CAbstractIntegerEdit(CGui *pContext) :
	CHListLayout(pContext),
	m_pEntry(NULL)
{
	m_pEntry = new CAbstractIntegerEdit::CEntry(this);
	m_pEntry->SetLabelStyle(Context()->GetNumericEntryStyle());
	
	m_pDecreaseButton = new CAbstractIntegerEdit::CDecreaseButton(this);
	m_pIncreaseButton = new CAbstractIntegerEdit::CIncreaseButton(this);
	
	SetIntEditStyle(Context()->GetIntEditStyle());
	
	Add(m_pEntry, true);
	Add(m_pDecreaseButton, false);
	Add(m_pIncreaseButton, false);
}

void CAbstractIntegerEdit::Editable(bool Value)
{
	m_pEntry->Editable(Value);
	
	if(Value)
	{
		m_pDecreaseButton->Enable();
		m_pIncreaseButton->Enable();
	}
	else
	{
		m_pDecreaseButton->Disable();
		m_pIncreaseButton->Disable();
	}
}

void CAbstractIntegerEdit::SetIntEditStyle(CAssetPath Path)
{
	m_IntEditStylePath = Path;
	
	const CAsset_GuiIntEditStyle* pStyle = AssetsManager()->GetAsset<CAsset_GuiIntEditStyle>(m_IntEditStylePath);
	if(pStyle)
	{
		m_pDecreaseButton->SetButtonStyle(pStyle->GetDecreaseButtonStylePath());
		m_pIncreaseButton->SetButtonStyle(pStyle->GetIncreaseButtonStylePath());
	}
}

/* INTEGER EDIT *******************************************************/

CIntegerEdit::CIntegerEdit(CGui* pContext, int DefaultValue) :
	CAbstractIntegerEdit(pContext),
	m_Value(DefaultValue)
{
	
}
	
void CIntegerEdit::SetValue(int Value)
{
	m_Value = Value;
}
	
int CIntegerEdit::GetValue() const
{
	return m_Value;
}

/* EXTERNAL INTEGER EDIT **********************************************/

CExternalIntegerEdit::CExternalIntegerEdit(CGui* pContext, int* Memory) :
	CAbstractIntegerEdit(pContext),
	m_Memory(Memory)
{
	
}
	
void CExternalIntegerEdit::SetValue(int v)
{
	*m_Memory = v;
}
	
int CExternalIntegerEdit::GetValue() const
{
	return *m_Memory;
}

}
