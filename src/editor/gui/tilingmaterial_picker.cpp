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

#include <shared/autolayer.h>
#include <client/maprenderer.h>
#include <client/gui/boxlayout.h>
#include <editor/gui/tilingmaterial_picker.h>
#include <editor/components/gui.h>

class CBrushIcon : public gui::CWidget
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_MaterialPath;
	CSubPath m_IndexSubPath;
	array2d<CAsset_MapLayerTiles::CTile> m_Tiles;
	int m_Number;
	
public:
	CBrushIcon(CGuiEditor *pAssetsEditor, CAssetPath MaterialPath, CSubPath IndexSubPath) :
		gui::CWidget(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_MaterialPath(MaterialPath),
		m_IndexSubPath(IndexSubPath)
	{
		int Index = IndexSubPath.GetId();
		m_Tiles.resize(5, 5);
		
		m_Tiles.get_clamp(0, 0).SetBrush(0);
		m_Tiles.get_clamp(1, 0).SetBrush(0);
		m_Tiles.get_clamp(2, 0).SetBrush(0);
		m_Tiles.get_clamp(3, 0).SetBrush(0);
		m_Tiles.get_clamp(4, 0).SetBrush(0);
		
		m_Tiles.get_clamp(0, 1).SetBrush(0);
		m_Tiles.get_clamp(1, 1).SetBrush(Index);
		m_Tiles.get_clamp(2, 1).SetBrush(Index);
		m_Tiles.get_clamp(3, 1).SetBrush(0);
		m_Tiles.get_clamp(4, 1).SetBrush(0);
		
		m_Tiles.get_clamp(0, 2).SetBrush(0);
		m_Tiles.get_clamp(1, 2).SetBrush(Index);
		m_Tiles.get_clamp(2, 2).SetBrush(Index);
		m_Tiles.get_clamp(3, 2).SetBrush(Index);
		m_Tiles.get_clamp(4, 2).SetBrush(0);
		
		m_Tiles.get_clamp(0, 3).SetBrush(0);
		m_Tiles.get_clamp(1, 3).SetBrush(Index);
		m_Tiles.get_clamp(2, 3).SetBrush(Index);
		m_Tiles.get_clamp(3, 3).SetBrush(Index);
		m_Tiles.get_clamp(4, 3).SetBrush(0);
		
		m_Tiles.get_clamp(0, 4).SetBrush(0);
		m_Tiles.get_clamp(1, 4).SetBrush(0);
		m_Tiles.get_clamp(2, 4).SetBrush(0);
		m_Tiles.get_clamp(3, 4).SetBrush(0);
		m_Tiles.get_clamp(4, 4).SetBrush(0);
		
		ApplyTilingMaterials_Tiles(AssetsManager(), m_Tiles, m_MaterialPath, 0, 0, m_Tiles.get_width(), m_Tiles.get_height(), 0);
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
		MapRenderer.SetLocalTime(time_get()/(float)time_freq());
		MapRenderer.SetCanvas(m_DrawRect, vec2(m_DrawRect.x + m_DrawRect.w/2, m_DrawRect.y + m_DrawRect.h/2));
		MapRenderer.SetCamera(0.0f, 1.0f);
		
		MapRenderer.RenderTiles_Style(m_Tiles, vec2(-32.0f*2.0f, -32.0f*2.0f), m_MaterialPath, 1.0f, true);
		
		Graphics()->ClipPop();
	}
};

class CBrushButton : public gui::CButton
{
protected:
	CTilingMaterialPicker* m_Picker;
	CAssetPath m_MaterialPath;
	CSubPath m_IndexSubPath;
	
public:
	CBrushButton(CGuiEditor *pAssetsEditor, CTilingMaterialPicker* Picker, CAssetPath MaterialPath, CSubPath IndexSubPath) :
		gui::CButton(pAssetsEditor, ""),
		m_Picker(Picker),
		m_MaterialPath(MaterialPath),
		m_IndexSubPath(IndexSubPath)
	{
		SetButtonStyle(pAssetsEditor->m_Path_Button_PaletteIcon);
		SetIconWidget(new CBrushIcon(pAssetsEditor, m_MaterialPath, m_IndexSubPath));
	}
	
	virtual void MouseClickAction()
	{
		m_Picker->OnBrushPicked(m_IndexSubPath);
	}
	
	virtual void OnMouseMove()
	{
		if(m_Picker->GetTitleLabel() && m_VisibilityRect.IsInside(Context()->GetMousePos()))
		{
			const CAsset_TilingMaterial* pMaterial = AssetsManager()->GetAsset<CAsset_TilingMaterial>(m_MaterialPath);
			if(pMaterial && pMaterial->IsValidIndex(m_IndexSubPath))
			{
				m_Picker->GetTitleLabel()->SetText(pMaterial->GetIndexTitle(m_IndexSubPath));
			}
			else
			{
				m_Picker->GetTitleLabel()->SetText(_LSTRING("Empty"));
			}
		}
		
		gui::CButton::OnMouseMove();
	}
};

CTilingMaterialPicker::CTilingMaterialPicker(CGuiEditor* pAssetsEditor, CAssetPath MaterialPath) :
	gui::CVListLayout(pAssetsEditor),
	m_pAssetsEditor(pAssetsEditor),
	m_MaterialPath(MaterialPath),
	m_pIndexTitle(nullptr)
{
	const CAsset_TilingMaterial* pMaterial = AssetsManager()->GetAsset<CAsset_TilingMaterial>(m_MaterialPath);
	if(!pMaterial)
		return;
		
	//Description of the current brush
	{
		gui::CVListLayout* pHeader = new gui::CVListLayout(Context());
		pHeader->SetBoxStyle(pAssetsEditor->m_Path_Box_PanelHeader);
		Add(pHeader);
		
		m_pIndexTitle = new gui::CLabel(Context(), _LSTRING("Select a brush type by clicking on it"));
		m_pIndexTitle->SetLabelStyle(pAssetsEditor->m_Path_Label_Group);
		pHeader->Add(m_pIndexTitle, false);
	}
		
	//List of brushes
	{
		gui::CBoxLayout* pList = new gui::CBoxLayout(Context());
		Add(pList, -1);
		pList->SetBoxStyle(pAssetsEditor->m_Path_Box_Panel);
		
		if(!pMaterial->IsValidIndex(CAsset_TilingMaterial::SubPath_Index(0)))
			pList->Add(new CBrushButton(pAssetsEditor, this, m_MaterialPath, CSubPath::Null()));
		
		CAsset_TilingMaterial::CIteratorIndex IndexIter;
		
		gui::CLabel* pLabel = new gui::CLabel(Context(), _LSTRING("Brushes"));
		pLabel->SetLabelStyle(pAssetsEditor->m_Path_Label_Group);
		pList->Add(pLabel, true);
		
		for(IndexIter = pMaterial->BeginIndex(); IndexIter != pMaterial->EndIndex(); ++IndexIter)
		{
			pList->Add(new CBrushButton(pAssetsEditor, this, m_MaterialPath, *IndexIter));
		}
	}
}
