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

#include <client/maprenderer.h>
#include <client/gui/integer-edit.h>
#include <client/gui/panellayout.h>
#include <client/gui/listlayout.h>
#include <client/gui/boxlayout.h>
#include <editor/gui/zone_picker.h>
#include <editor/components/gui.h>

class CZoneIcon : public gui::CWidget
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_ZoneTypePath;
	CSubPath m_IndexSubPath;
	array2d<CAsset_MapZoneTiles::CTile> m_Tiles;
	int m_Number;
	
public:
	CZoneIcon(CGuiEditor *pAssetsEditor, CAssetPath ZoneTypePath, CSubPath IndexSubPath) :
		gui::CWidget(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_ZoneTypePath(ZoneTypePath),
		m_IndexSubPath(IndexSubPath)
	{
		int Index = 0;
		if(m_IndexSubPath.IsNotNull())
		{
			const CAsset_ZoneType* pZoneType = AssetsManager()->GetAsset<CAsset_ZoneType>(m_ZoneTypePath);
			if(pZoneType && pZoneType->IsValidIndex(m_IndexSubPath))
				Index = m_IndexSubPath.GetId();
		}
		m_Tiles.resize(5, 5);
		
		m_Tiles.get_clamp(0, 0).SetIndex(0);
		m_Tiles.get_clamp(1, 0).SetIndex(0);
		m_Tiles.get_clamp(2, 0).SetIndex(0);
		m_Tiles.get_clamp(3, 0).SetIndex(0);
		m_Tiles.get_clamp(4, 0).SetIndex(0);
		
		m_Tiles.get_clamp(0, 1).SetIndex(0);
		m_Tiles.get_clamp(1, 1).SetIndex(Index);
		m_Tiles.get_clamp(2, 1).SetIndex(Index);
		m_Tiles.get_clamp(3, 1).SetIndex(0);
		m_Tiles.get_clamp(4, 1).SetIndex(0);
		
		m_Tiles.get_clamp(0, 2).SetIndex(0);
		m_Tiles.get_clamp(1, 2).SetIndex(Index);
		m_Tiles.get_clamp(2, 2).SetIndex(Index);
		m_Tiles.get_clamp(3, 2).SetIndex(Index);
		m_Tiles.get_clamp(4, 2).SetIndex(0);
		
		m_Tiles.get_clamp(0, 3).SetIndex(0);
		m_Tiles.get_clamp(1, 3).SetIndex(Index);
		m_Tiles.get_clamp(2, 3).SetIndex(Index);
		m_Tiles.get_clamp(3, 3).SetIndex(Index);
		m_Tiles.get_clamp(4, 3).SetIndex(0);
		
		m_Tiles.get_clamp(0, 4).SetIndex(0);
		m_Tiles.get_clamp(1, 4).SetIndex(0);
		m_Tiles.get_clamp(2, 4).SetIndex(0);
		m_Tiles.get_clamp(3, 4).SetIndex(0);
		m_Tiles.get_clamp(4, 4).SetIndex(0);
	}
	
	virtual void UpdateBoundingSize()
	{
		m_BoundingSizeRect.BSMinimum(32.0f*3, 32.0f*3);
	}
	
	virtual void Render()
	{
		Graphics()->ClipPush(m_DrawRect.x, m_DrawRect.y, m_DrawRect.w, m_DrawRect.h);
		
		CMapRenderer MapRenderer(ClientKernel());
		
		MapRenderer.SetTime(0.0f);
		MapRenderer.SetCanvas(m_DrawRect, vec2(m_DrawRect.x + m_DrawRect.w/2, m_DrawRect.y + m_DrawRect.h/2));
		MapRenderer.SetCamera(0.0f, 1.0f);
		
		MapRenderer.RenderTiles_Zone(m_ZoneTypePath, m_Tiles, vec2(-32.0f*2.0f, -32.0f*2.0f), 1.0f, true);
		
		Graphics()->ClipPop();
	}
};

class CZoneButton : public gui::CButton
{
protected:
	CZonePicker* m_pPicker;
	CAssetPath m_ZoneTypePath;
	CSubPath m_IndexSubPath;
	
public:
	CZoneButton(CGuiEditor *pAssetsEditor, CZonePicker* pPicker, CAssetPath ZoneTypePath, CSubPath IndexSubPath) :
		gui::CButton(pAssetsEditor, ""),
		m_pPicker(pPicker),
		m_ZoneTypePath(ZoneTypePath),
		m_IndexSubPath(IndexSubPath)
	{
		SetButtonStyle(pAssetsEditor->m_Path_Button_PaletteIcon);
		SetIconWidget(new CZoneIcon(pAssetsEditor, m_ZoneTypePath, m_IndexSubPath));
	}
	
	virtual void MouseClickAction()
	{
		m_pPicker->OnZonePicked(m_IndexSubPath);
	}
	
	virtual void OnMouseMove()
	{
		if(m_pPicker->GetTitleLabel() && m_VisibilityRect.IsInside(Context()->GetMousePos()))
		{
			const CAsset_ZoneType* pZoneType = AssetsManager()->GetAsset<CAsset_ZoneType>(m_ZoneTypePath);
			if(pZoneType && pZoneType->IsValidIndex(m_IndexSubPath) && pZoneType->GetIndexUsed(m_IndexSubPath))
			{
				if(m_pPicker->GetTitleLabel())
					m_pPicker->GetTitleLabel()->SetText(pZoneType->GetIndexTitle(m_IndexSubPath));
					
				if(m_pPicker->GetDescriptionLabel())
					m_pPicker->GetDescriptionLabel()->SetText(pZoneType->GetIndexDescription(m_IndexSubPath));
			}
			else
			{
				if(m_pPicker->GetTitleLabel())
					m_pPicker->GetTitleLabel()->SetText(_LSTRING("Empty"));
					
				if(m_pPicker->GetDescriptionLabel())
					m_pPicker->GetDescriptionLabel()->SetText(_LSTRING("Can be used as an eraser"));
			}
		}
		
		gui::CButton::OnMouseMove();
	}
};

CZonePicker::CZonePicker(CGuiEditor* pAssetsEditor, CAssetPath ZoneTypePath, const std::vector<int>* pDataInt) :
	gui::CVListLayout(pAssetsEditor),
	m_pAssetsEditor(pAssetsEditor),
	m_ZoneTypePath(ZoneTypePath),
	m_pZoneTitle(nullptr),
	m_pZoneDescription(nullptr)
{
	const CAsset_ZoneType* pZoneType = AssetsManager()->GetAsset<CAsset_ZoneType>(m_ZoneTypePath);
	if(!pZoneType)
		return;
	
	gui::CVListLayout* pVList = new gui::CVListLayout(Context());
	Add(pVList, true);
	
	//Description of the current zone
	{
		gui::CVListLayout* pHeader = new gui::CVListLayout(Context());
		pHeader->SetBoxStyle(pAssetsEditor->m_Path_Box_PanelHeader);
		pVList->Add(pHeader);
		
		m_pZoneTitle = new gui::CLabel(Context(), _LSTRING("Select a zone type by clicking on it"));
		m_pZoneTitle->SetLabelStyle(pAssetsEditor->m_Path_Label_Group);
		pHeader->Add(m_pZoneTitle, false);
		m_pZoneDescription = new gui::CLabel(Context(), _LSTRING(""));
		pHeader->Add(m_pZoneDescription, false);
	}
	
	gui::CHPanelLayout* pPanels = new gui::CHPanelLayout(Context());
	pVList->Add(pPanels, true);
	
	//List of zones
	{
		gui::CBoxLayout* pList = new gui::CBoxLayout(Context());
		pPanels->Add(pList, -1);
		pList->SetBoxStyle(pAssetsEditor->m_Path_Box_Panel);
		
		if(!pZoneType->IsValidIndex(CAsset_ZoneType::SubPath_Index(0)) || !pZoneType->GetIndexUsed(CAsset_ZoneType::SubPath_Index(0)))
			pList->Add(new CZoneButton(pAssetsEditor, this, m_ZoneTypePath, CSubPath::Null()));
		
		CAsset_ZoneType::CIteratorGroup GroupIter;
		CAsset_ZoneType::CIteratorIndex IndexIter;
		
		//Show default group
		bool UnknownGroupNeed = false;
		for(IndexIter = pZoneType->BeginIndex(); IndexIter != pZoneType->EndIndex(); ++IndexIter)
		{
			if(pZoneType->GetIndexUsed(*IndexIter))
			{
				if(pZoneType->GetIndexGroup(*IndexIter) == -1)
					pList->Add(new CZoneButton(pAssetsEditor, this, m_ZoneTypePath, *IndexIter));
				else if(pZoneType->IsValidGroup(CAsset_ZoneType::SubPath_Group(pZoneType->GetIndexGroup(*IndexIter))))
					UnknownGroupNeed = true;
			}
		}
		
		//Show groups
		for(GroupIter = pZoneType->BeginGroup(); GroupIter != pZoneType->EndGroup(); ++GroupIter)
		{
			gui::CLabel* pLabel = new gui::CLabel(Context(), pZoneType->GetGroup(*GroupIter));
			pLabel->SetLabelStyle(pAssetsEditor->m_Path_Label_Group);
			pList->Add(pLabel, true);
			
			for(IndexIter = pZoneType->BeginIndex(); IndexIter != pZoneType->EndIndex(); ++IndexIter)
			{
				if(pZoneType->GetIndexUsed(*IndexIter) && pZoneType->GetIndexGroup(*IndexIter) == (*GroupIter).GetId())
					pList->Add(new CZoneButton(pAssetsEditor, this, m_ZoneTypePath, *IndexIter));
			}
		}
		
		//Show unknown groups
		if(UnknownGroupNeed)
		{
			gui::CLabel* pLabel = new gui::CLabel(Context(), _LSTRING("Other zones"));
			pLabel->SetLabelStyle(pAssetsEditor->m_Path_Label_Group);
			pList->Add(pLabel, true);
			
			for(IndexIter = pZoneType->BeginIndex(); IndexIter != pZoneType->EndIndex(); ++IndexIter)
			{
				if(pZoneType->GetIndexUsed(*IndexIter) && pZoneType->GetIndexGroup(*IndexIter) != -1 && !pZoneType->IsValidGroup(CAsset_ZoneType::SubPath_Group(pZoneType->GetIndexGroup(*IndexIter))))
					pList->Add(new CZoneButton(pAssetsEditor, this, m_ZoneTypePath, *IndexIter));
			}
		}
	}
	
	//DataInt editor
	if(pZoneType->GetDataIntArraySize())
	{
		m_DataInt.resize(pZoneType->GetDataIntArraySize());
		
		gui::CVScrollLayout* pDataList = new gui::CVScrollLayout(Context());
		pPanels->Add(pDataList, 200);
		pDataList->SetBoxStyle(pAssetsEditor->m_Path_Box_Panel);
		
		int Counter = 0;
		CAsset_ZoneType::CIteratorDataInt Iter;
		for(Iter = pZoneType->BeginDataInt(); Iter != pZoneType->EndDataInt(); ++Iter)
		{
			m_DataInt[(*Iter).GetId()] = pZoneType->GetDataIntDefaultValue(*Iter);
			
			if(pDataInt && pDataInt->size() > (unsigned int) Counter)
				m_DataInt[(*Iter).GetId()] = (*pDataInt)[Counter];
			
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pHList->Add(new gui::CLabel(Context(), pZoneType->GetDataIntTitle(*Iter)), true);
			pHList->Add(new gui::CBoundedExternalIntegerEdit(Context(), &m_DataInt[(*Iter).GetId()], pZoneType->GetDataIntMinValue(*Iter), pZoneType->GetDataIntMaxValue(*Iter)), true);
			pDataList->Add(pHList, false);
			
			Counter++;
		}
	}
}
