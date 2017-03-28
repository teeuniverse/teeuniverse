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

#include <client/gui/popup.h>
#include <editor/gui/view_map_fill.h>
#include <editor/gui/image_picker.h>
#include <editor/gui/tilingmaterial_picker.h>
#include <editor/components/gui.h>
#include <client/maprenderer.h>
#include <client/components/assetsrenderer.h>
#include <shared/autolayer.h>


/* MATERIAL PALETTE ***************************************************/

namespace
{

class CPopup_MaterialPalette : public gui::CPopup
{
protected:
	class CPaletteMaterialPicker : public CTilingMaterialPicker
	{
	protected:
		CPopup_MaterialPalette* m_pPopup;
		CCursorTool_MapFill* m_pCursorTool;
		
	protected:
		virtual void OnBrushPicked(CSubPath IndexPath)
		{
			m_pCursorTool->SetIndex(IndexPath.GetId());
			m_pPopup->Close();
		}
	
	public:
		CPaletteMaterialPicker(CCursorTool_MapFill* pCursorTool, CPopup_MaterialPalette* pPopup, CAssetPath ImagePath) :
			CTilingMaterialPicker(pCursorTool->AssetsEditor(), ImagePath),
			m_pPopup(pPopup),
			m_pCursorTool(pCursorTool)
		{
			
		}
	};
	
protected:
	CGuiEditor* m_pAssetsEditor;
	
public:
	CPopup_MaterialPalette(CGuiEditor* pAssetsEditor, CCursorTool_MapFill* pCursorTool, const gui::CRect& CreatorRect, CAssetPath MaterialPath) :
		gui::CPopup(pAssetsEditor, CreatorRect, CreatorRect.w-16, CreatorRect.h-16, gui::CPopup::ALIGNMENT_INNER),
		m_pAssetsEditor(pAssetsEditor)
	{
		SetBoxStyle(m_pAssetsEditor->m_Path_Box_Dialog);
		
		CTilingMaterialPicker* pImagePicker = new CPaletteMaterialPicker(pCursorTool, this, MaterialPath);
		Add(pImagePicker);
	}
	
	virtual void OnButtonClick(int Button)
	{
		if(m_DrawRect.IsInside(Context()->GetMousePos()) && m_VisibilityRect.IsInside(Context()->GetMousePos()) && Button == KEY_MOUSE_2)
			Close();
		
		gui::CPopup::OnButtonClick(Button);
	}

	virtual void OnInputEvent(const CInput::CEvent& Event)
	{
		if((Event.m_Key == KEY_SPACE) && (Event.m_Flags & CInput::FLAG_PRESS) && !Input()->IsTextEdited())
		{
			Close();
			return;
		}
		else
			CPopup::OnInputEvent(Event);
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

/* IMAGE PALETTE ******************************************************/

class CPopup_TilePalette : public gui::CPopup
{
protected:
	class CPaletteTilePicker : public CImagePicker
	{
	protected:
		CPopup_TilePalette* m_pPopup;
		CCursorTool_MapFill* m_pCursorTool;
		
	protected:
		virtual void OnImagePicked(int MinX, int MinY, int MaxX, int MaxY)
		{
			m_pCursorTool->SetIndex(MinY*16+MinX);
			m_pPopup->Close();
		}
	
	public:
		CPaletteTilePicker(CCursorTool_MapFill* pCursorTool, CPopup_TilePalette* pPopup, CAssetPath ImagePath) :
			CImagePicker(pCursorTool->AssetsEditor(), ImagePath),
			m_pPopup(pPopup),
			m_pCursorTool(pCursorTool)
		{
			
		}
	};
	
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_ImagePath;
	
public:
	CPopup_TilePalette(CGuiEditor* pAssetsEditor, CCursorTool_MapFill* pCursorTool, const gui::CRect& CreatorRect, CAssetPath ImagePath) :
		gui::CPopup(pAssetsEditor, CreatorRect, CreatorRect.w-16, CreatorRect.h-16, gui::CPopup::ALIGNMENT_INNER),
		m_pAssetsEditor(pAssetsEditor),
		m_ImagePath(ImagePath)
	{
		SetBoxStyle(m_pAssetsEditor->m_Path_Box_Dialog);
		
		CImagePicker* pImagePicker = new CPaletteTilePicker(pCursorTool, this, m_ImagePath);
		pImagePicker->SetSelectionType(CImagePicker::SELECTTYPE_TILE);
		Add(pImagePicker);
	}
	
	virtual void OnButtonClick(int Button)
	{
		if(m_DrawRect.IsInside(Context()->GetMousePos()) && m_VisibilityRect.IsInside(Context()->GetMousePos()) && Button == KEY_MOUSE_2)
			Close();
		
		gui::CPopup::OnButtonClick(Button);
	}

	virtual void OnInputEvent(const CInput::CEvent& Event)
	{
		if((Event.m_Key == KEY_SPACE) && (Event.m_Flags & CInput::FLAG_PRESS) && !Input()->IsTextEdited())
		{
			Close();
			return;
		}
		else
			CPopup::OnInputEvent(Event);
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

}

/* CURSOR TOOL ********************************************************/

CCursorTool_MapFill::CCursorTool_MapFill(CViewMap* pViewMap) :
	CCursorTool(pViewMap, _LSTRING("Filling"), pViewMap->AssetsEditor()->m_Path_Sprite_IconFill),
	m_DragEnabled(false),
	m_Index(-1)
{
	
}

void CCursorTool_MapFill::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(Button == KEY_MOUSE_1)
	{
		m_DragEnabled = true;
		
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		m_Pivot = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
	}
	else if(Button == KEY_MOUSE_2)
	{
		AltButtonAction();
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}

void CCursorTool_MapFill::OnViewInputEvent(const CInput::CEvent& Event)
{
	if((Event.m_Key == KEY_SPACE) && (Event.m_Flags & CInput::FLAG_PRESS) && !Input()->IsTextEdited())
	{
		AltButtonAction();
		return;
	}
	else
		CCursorTool::OnViewInputEvent(Event);
}

void CCursorTool_MapFill::AltButtonAction()
{
	m_DragEnabled = false;
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
		if(pLayer && pLayer->GetSourcePath().IsNull())
		{
			if(pLayer->GetStylePath().GetType() == CAsset_Image::TypeId)
				Context()->DisplayPopup(new CPopup_TilePalette(AssetsEditor(), this, ViewMap()->GetViewRect(), pLayer->GetStylePath()));
			else if(pLayer->GetStylePath().GetType() == CAsset_TilingMaterial::TypeId)
				Context()->DisplayPopup(new CPopup_MaterialPalette(AssetsEditor(), this, ViewMap()->GetViewRect(), pLayer->GetStylePath()));
		}
	}
}
	
void CCursorTool_MapFill::OnViewButtonRelease(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(m_DragEnabled)
	{
		m_DragEnabled = false;
		
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		
		if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
		{
			//Copy tiles
			const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer && pLayer->GetSourcePath().IsNull())
			{
				vec2 PivotTilePos = ViewMap()->MapRenderer()->MapPosToTilePos(m_Pivot);
				vec2 CursorTilePos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
				int TileX0 = std::floor(PivotTilePos.x) - pLayer->GetPositionX();
				int TileY0 = std::floor(PivotTilePos.y) - pLayer->GetPositionY();
				int TileX1 = std::floor(CursorTilePos.x) - pLayer->GetPositionX();
				int TileY1 = std::floor(CursorTilePos.y) - pLayer->GetPositionY();
			
				int X0 = min(TileX0, TileX1);
				int Y0 = min(TileY0, TileY1);
				int X1 = max(TileX0, TileX1) + 1;
				int Y1 = max(TileY0, TileY1) + 1;
				
				int Token = AssetsManager()->GenerateToken();
				
				int Index = (m_Index >= 0 ? m_Index : 1);
				
				for(int j=Y0; j<Y1; j++)
				{
					for(int i=X0; i<X1; i++)
					{
						CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(i, j);
						AssetsManager()->SetAssetValue<int>(
							AssetsEditor()->GetEditedAssetPath(),
							TilePath,
							CAsset_MapLayerTiles::TILE_INDEX,
							(pLayer->GetStylePath().GetId() == CAsset_TilingMaterial::TypeId ? 0 : Index),
							Token
						);
						AssetsManager()->SetAssetValue<int>(
							AssetsEditor()->GetEditedAssetPath(),
							TilePath,
							CAsset_MapLayerTiles::TILE_FLAGS,
							0x0,
							Token
						);
						AssetsManager()->SetAssetValue<int>(
							AssetsEditor()->GetEditedAssetPath(),
							TilePath,
							CAsset_MapLayerTiles::TILE_BRUSH,
							(pLayer->GetStylePath().GetId() == CAsset_TilingMaterial::TypeId ? m_Index : (Index > 0 ? 1 : 0)),
							Token
						);
					}
				}
				
				//Apply auto-tiling
				if(pLayer->GetStylePath().GetType() == CAsset_TilingMaterial::TypeId)
					ApplyTilingMaterials_Layer(AssetsManager(), AssetsEditor()->GetEditedAssetPath(), X0, Y0, X1-X0, Y1-Y0, Token);
			}
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
		{
			//Copy tiles
			const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer)
			{
				vec2 PivotTilePos = ViewMap()->MapRenderer()->MapPosToTilePos(m_Pivot);
				vec2 CursorTilePos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
				int TileX0 = std::floor(PivotTilePos.x) - pLayer->GetPositionX();
				int TileY0 = std::floor(PivotTilePos.y) - pLayer->GetPositionY();
				int TileX1 = std::floor(CursorTilePos.x) - pLayer->GetPositionX();
				int TileY1 = std::floor(CursorTilePos.y) - pLayer->GetPositionY();
			
				int X0 = min(TileX0, TileX1);
				int Y0 = min(TileY0, TileY1);
				int X1 = max(TileX0, TileX1) + 1;
				int Y1 = max(TileY0, TileY1) + 1;
				
				int Index = m_Index;
				if(Index < 0)
				{
					Index = 1;
					const CAsset_ZoneType* pZoneType = AssetsManager()->GetAsset<CAsset_ZoneType>(pLayer->GetZoneTypePath());
					if(pZoneType)
					{
						CAsset_ZoneType::CIteratorIndex Iter = pZoneType->BeginIndex();
						while(Iter != pZoneType->EndIndex())
						{
							if(pZoneType->IsValidIndex(*Iter) && pZoneType->GetIndexUsed(*Iter) && (*Iter).GetId() > 0)
							{
								Index = (*Iter).GetId();
								break;
							}
							++Iter;
						}
					}
				}
				
				int Token = AssetsManager()->GenerateToken();
				
				for(int j=Y0; j<Y1; j++)
				{
					for(int i=X0; i<X1; i++)
					{
						CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
						AssetsManager()->SetAssetValue<int>(
							AssetsEditor()->GetEditedAssetPath(),
							TilePath,
							CAsset_MapZoneTiles::TILE_INDEX,
							Index,
							Token
						);
						AssetsManager()->SetAssetValue<int>(
							AssetsEditor()->GetEditedAssetPath(),
							TilePath,
							CAsset_MapZoneTiles::TILE_FLAGS,
							0x0,
							Token
						);
					}
				}
	
				//Update tile layers that use this zone as source
				for(int p=0; p<AssetsManager()->GetNumPackages(); p++)
				{
					for(int l=0; l<AssetsManager()->GetNumAssets<CAsset_MapLayerTiles>(p); l++)
					{
						CAssetPath AutoLayerPath = CAssetPath(CAsset_MapLayerTiles::TypeId, p, l);
						const CAsset_MapLayerTiles* pAutoLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AutoLayerPath);
						if(pAutoLayer && pAutoLayer->GetSourcePath() == AssetsEditor()->GetEditedAssetPath())
						{
							ApplyTilingMaterials_Layer(AssetsManager(), AutoLayerPath, X0, Y0, X1-X0, Y1-Y0, Token);
						}
					}
				}
			}
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapFill::RenderView()
{
	CAssetPath ImagePath;
	vec4 Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
		if(!pLayer)
			return;
		
		ImagePath = pLayer->GetStylePath();
		Color = pLayer->GetColor();
	}
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	float Time = fmod(ViewMap()->MapRenderer()->GetLocalTime(), 1.0f);
	Color.a *= 0.3f + 0.7f*(1.0f+cos(2.0f*Pi*Time))/2.0f;
	
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	vec2 CursorTilePos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
	vec2 PivotTilePos = ViewMap()->MapRenderer()->MapPosToTilePos(m_Pivot);
	
	if(m_DragEnabled)
	{
		int TileX0 = std::floor(PivotTilePos.x);
		int TileY0 = std::floor(PivotTilePos.y);
		int TileX1 = std::floor(CursorTilePos.x);
		int TileY1 = std::floor(CursorTilePos.y);
	
		int X0 = min(TileX0, TileX1);
		int Y0 = min(TileY0, TileY1);
		int X1 = max(TileX0, TileX1) + 1;
		int Y1 = max(TileY0, TileY1) + 1;
		
		vec2 SelectionRectPos0 = ViewMap()->MapRenderer()->MapPosToScreenPos(vec2(X0*32.0f, Y0*32.0f));
		vec2 SelectionRectPos1 = ViewMap()->MapRenderer()->MapPosToScreenPos(vec2(X1*32.0f, Y1*32.0f));
		vec2 SelectionRectSize = SelectionRectPos1 - SelectionRectPos0;
		
		gui::CRect Rect;
		Rect.x = SelectionRectPos0.x;
		Rect.y = SelectionRectPos0.y;
		Rect.w = SelectionRectSize.x;
		Rect.h = SelectionRectSize.y;
	
		AssetsRenderer()->DrawGuiRect(&Rect, AssetsEditor()->m_Path_Rect_Selection);
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}

void CCursorTool_MapFill::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerTiles::TypeId:
			{
				Enable();
				
				const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
				if(!pLayer || pLayer->GetSourcePath().IsNotNull())
					Editable(false);
				else
					Editable(true);
				
				if(pLayer)
				{
					if(m_SelectedStyle != pLayer->GetStylePath())
					{
						m_SelectedStyle = pLayer->GetStylePath();
						m_Index = -1;
					}
				}
			}
			break;
		case CAsset_MapZoneTiles::TypeId:
			{
				Enable();
				
				const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
				{
					if(m_SelectedStyle != pLayer->GetZoneTypePath())
					{
						m_SelectedStyle = pLayer->GetZoneTypePath();
						m_Index = -1;
					}
				}
			}
			break;
		default:
			Disable();
	}
	
	if(ParentEnabled && IsEnabled() && IsEditable())
	{
		if(AssetsEditor()->m_BindCall_ToolFill > 0)
		{
			AssetsEditor()->m_BindCall_ToolFill = 0;
			ViewMap()->SetCursorTool(this);
		}
	}
	
	CCursorTool::Update(ParentEnabled);
}

void CCursorTool_MapFill::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_LSTRING("Filling Tool: Fill area of the map."));
	
	CViewMap::CCursorTool::OnMouseMove();
}
