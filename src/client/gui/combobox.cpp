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

#include "combobox.h"
#include "popup.h"
#include "listlayout.h"
	
namespace gui
{

class CEnumButton : public CButton
{
protected:
	int m_Value;
	CPopup* m_pPopup;
	CComboBox* m_pComboBox;
	
protected:
	virtual void MouseClickAction()
	{
		m_pComboBox->SetValue(m_Value);
		m_pPopup->Close();
	}	

public:
	CEnumButton(CGui *pContext, CComboBox* pComboBox, CPopup* pPopup, const CComboBox::CItem& Item, int Value) :
		CButton(pContext, Item.m_Description, Item.m_IconPath),
		m_Value(Value),
		m_pPopup(pPopup),
		m_pComboBox(pComboBox)
	{
		NoTextClipping();
	}
};

class CComboBoxPopup : public CPopup
{
public:
	CComboBoxPopup(CGui *pContext, CComboBox* pComboBox, const CRect& CreatorRect, const array< 
CComboBox::CItem, allocator_copy<
CComboBox::CItem> >& EnumDescriptions, CAssetPath ButtonStyle) :
		CPopup(pContext, CreatorRect, CreatorRect.w, -1, CPopup::ALIGNMENT_BOTTOM)
	{
		CVListLayout* pLayout = new CVListLayout(Context());
		Add(pLayout);
		
		for(int i=0; i<EnumDescriptions.size(); i++)
		{
			CEnumButton* pButton = new CEnumButton(Context(), pComboBox, this, EnumDescriptions[i], i);
			pButton->SetButtonStyle(ButtonStyle);
			pLayout->Add(pButton, false);
		}
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

/* COMBOBOX ***********************************************************/

CComboBox::CComboBox(CGui *pContext) :
	CButton(pContext, "")
{
	m_ComboBoxStylePath = Context()->GetComboBoxStyle();
}

void CComboBox::Add(const CLocalizableString& LString)
{
	CItem& Item = m_EnumDescriptions.increment();
	Item.m_Description.copy(LString);
}

void CComboBox::Add(const CLocalizableString& LString, const CAssetPath& IconPath)
{
	CItem& Item = m_EnumDescriptions.increment();
	Item.m_Description.copy(LString);
	Item.m_IconPath = IconPath;
}

void CComboBox::RefreshComboBoxStyle()
{
	const CAsset_GuiComboBoxStyle* pStyle = AssetsManager()->GetAsset<CAsset_GuiComboBoxStyle>(m_ComboBoxStylePath);
	if(pStyle)
		SetButtonStyle(pStyle->GetButtonPath());
}
	
void CComboBox::Update(bool ParentEnabled)
{
	RefreshComboBoxStyle();
	
	int Value = GetValue();
	if(Value >= 0 && Value < m_EnumDescriptions.size())
	{
		SetText(m_EnumDescriptions[Value].m_Description);
		SetIcon(m_EnumDescriptions[Value].m_IconPath);
	}
	else
	{
		SetText(_LSTRING("Unknown value"));
	}
	
	CButton::Update(ParentEnabled);
}

void CComboBox::MouseClickAction()
{
	CAssetPath ButtonStylePath;
	CAssetPath PopupStylePath;
	const CAsset_GuiComboBoxStyle* pStyle = AssetsManager()->GetAsset<CAsset_GuiComboBoxStyle>(m_ComboBoxStylePath);
	if(pStyle)
	{
		ButtonStylePath = pStyle->GetEnumPath();
		PopupStylePath = pStyle->GetPopupPath();
	}
	
	CComboBoxPopup* pPopup = new CComboBoxPopup(Context(), this, m_DrawRect, m_EnumDescriptions, ButtonStylePath);
	pPopup->SetBoxStyle(PopupStylePath);
	
	Context()->DisplayPopup(pPopup);
}

}
