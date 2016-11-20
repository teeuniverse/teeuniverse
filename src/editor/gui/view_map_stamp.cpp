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

#include <editor/gui/view_map_stamp.h>
#include <editor/gui/image_picker.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/gui/popup.h>
#include <client/gui/boxlayout.h>
#include <client/maprenderer.h>
#include <generated/assets/maplayertiles.h>
#include <generated/assets/mapzonetiles.h>

/* CURSORTOOL STAMP ***************************************************/

class CPopup_ZonePalette : public gui::CPopup
{
protected:
	class CZoneIcon : public gui::CWidget
	{
	protected:
		CGuiEditor* m_pAssetsEditor;
		CAssetPath m_ZoneTypePath;
		CSubPath m_IndexSubPath;
		array2d< CAsset_MapZoneTiles::CTile, allocator_copy<CAsset_MapZoneTiles::CTile> > m_Tiles;
		int m_Number;
		
	public:
		CZoneIcon(CGuiEditor *pAssetsEditor, CAssetPath ZoneTypePath, CSubPath IndexSubPath) :
			gui::CWidget(pAssetsEditor),
			m_pAssetsEditor(pAssetsEditor),
			m_ZoneTypePath(ZoneTypePath),
			m_IndexSubPath(IndexSubPath),
			m_Number(0)
		{
			if(m_IndexSubPath.IsNotNull())
			{
				const CAsset_ZoneType* pZoneType = AssetsManager()->GetAsset<CAsset_ZoneType>(m_ZoneTypePath);
				if(pZoneType)
					m_Number = pZoneType->GetIndexNumber(m_IndexSubPath);
			}
			
			m_Tiles.resize(3, 2);
			m_Tiles.get_clamp(0, 0).SetIndex(m_Number);
			m_Tiles.get_clamp(1, 0).SetIndex(m_Number);
			m_Tiles.get_clamp(2, 0).SetIndex(0);
			m_Tiles.get_clamp(0, 1).SetIndex(m_Number);
			m_Tiles.get_clamp(1, 1).SetIndex(m_Number);
			m_Tiles.get_clamp(2, 1).SetIndex(m_Number);
		}
		
		virtual void UpdateBoundingSize()
		{
			m_BoundingSizeRect.BSMinimum(32.0f*4, 32.0f*3);
		}
		
		virtual void Render()
		{
			Graphics()->ClipPush(m_DrawRect.x, m_DrawRect.y, m_DrawRect.w, m_DrawRect.h);
			
			CMapRenderer MapRenderer(ClientKernel());
			
			MapRenderer.SetTime(0.0f);
			MapRenderer.SetLocalTime(time_get()/(float)time_freq());
			MapRenderer.SetCanvas(m_DrawRect, vec2(m_DrawRect.x + m_DrawRect.w/2, m_DrawRect.y + m_DrawRect.h/2));
			MapRenderer.SetCamera(0.0f, 1.0f);
			
			MapRenderer.RenderTiles_Zone(m_ZoneTypePath, m_Tiles, vec2(-32.0f*1.5, -32.0f*1.0f), 1.0f, m_pAssetsEditor->m_Path_Image_ZoneTexture, false);
			
			Graphics()->ClipPop();
		}
	};
	
	class CZoneButton : public gui::CButton
	{
	protected:
		CPopup_ZonePalette* m_pPopup;
		CCursorTool_TileStamp* m_pCursorTool;
		CAssetPath m_ZoneTypePath;
		CSubPath m_IndexSubPath;
		
	public:
		CZoneButton(CGuiEditor *pAssetsEditor, CCursorTool_TileStamp* pCursorTool, CPopup_ZonePalette* pPopup, CAssetPath ZoneTypePath, CSubPath IndexSubPath) :
			gui::CButton(pAssetsEditor, ""),
			m_pPopup(pPopup),
			m_pCursorTool(pCursorTool),
			m_ZoneTypePath(ZoneTypePath),
			m_IndexSubPath(IndexSubPath)
		{
			if(IndexSubPath.IsNotNull())
			{
				const CAsset_ZoneType* pZoneType = AssetsManager()->GetAsset<CAsset_ZoneType>(ZoneTypePath);
				if(pZoneType)
					SetText(pZoneType->GetIndexName(m_IndexSubPath));
			}
			else
				SetText("Empty");
			
			SetButtonStyle(pAssetsEditor->m_Path_Button_PaletteIcon);
			SetIconWidget(new CZoneIcon(pAssetsEditor, ZoneTypePath, m_IndexSubPath));
		}
		
		virtual void MouseClickAction()
		{
			m_pCursorTool->PaletteCallback_SelectZoneType(m_ZoneTypePath, m_IndexSubPath);
			m_pPopup->Close();
		}
	};
	
protected:
	CAssetPath m_ZoneTypePath;
	
public:
	CPopup_ZonePalette(CGuiEditor* pAssetsEditor, CCursorTool_TileStamp* pCursorTool, const gui::CRect& CreatorRect, CAssetPath ZoneTypePath) :
		gui::CPopup(pAssetsEditor, CreatorRect, CreatorRect.w-16, CreatorRect.h-16, gui::CPopup::ALIGNMENT_INNER),
		m_ZoneTypePath(ZoneTypePath)
	{
		SetBoxStyle(pAssetsEditor->m_Path_Box_Dialog);
		
		gui::CBoxLayout* pList = new gui::CBoxLayout(Context());
		Add(pList);		
		
		const CAsset_ZoneType* pZoneType = AssetsManager()->GetAsset<CAsset_ZoneType>(m_ZoneTypePath);
		if(!pZoneType)
			return;
		
		bool Index0Found = false;
		CAsset_ZoneType::CIteratorIndex Iter;
		for(Iter = pZoneType->BeginIndex(); Iter != pZoneType->EndIndex(); ++Iter)
		{
			if(pZoneType->GetIndexNumber(*Iter) == 0)
				Index0Found = true;
		}
		
		if(!Index0Found)
			pList->Add(new CZoneButton(pAssetsEditor, pCursorTool, this, m_ZoneTypePath, CSubPath::Null()));
		
		for(Iter = pZoneType->BeginIndex(); Iter != pZoneType->EndIndex(); ++Iter)
		{
			pList->Add(new CZoneButton(pAssetsEditor, pCursorTool, this, m_ZoneTypePath, *Iter));
		}
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

class CPopup_TilePalette : public gui::CPopup
{
protected:
	class CPaletteImagePicker : public CImagePicker
	{
	protected:
		CPopup_TilePalette* m_pPopup;
		CCursorTool_TileStamp* m_pCursorTool;
		
	protected:
		virtual void OnImagePicked(int MinX, int MinY, int MaxX, int MaxY)
		{
			m_pCursorTool->PaletteCallback_SelectImage(m_ImagePath, MinX, MinY, MaxX, MaxY);
			m_pPopup->Close();
		}
	
	public:
		CPaletteImagePicker(CCursorTool_TileStamp* pCursorTool, CPopup_TilePalette* pPopup, CAssetPath ImagePath) :
			CImagePicker(pCursorTool->AssetsEditor(), ImagePath),
			m_pCursorTool(pCursorTool),
			m_pPopup(pPopup)
		{
			
		}
	};
	
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_ImagePath;
	
public:
	CPopup_TilePalette(CGuiEditor* pAssetsEditor, CCursorTool_TileStamp* pCursorTool, const gui::CRect& CreatorRect, CAssetPath ImagePath) :
		gui::CPopup(pAssetsEditor, CreatorRect, CreatorRect.w-16, CreatorRect.h-16, gui::CPopup::ALIGNMENT_INNER),
		m_pAssetsEditor(pAssetsEditor),
		m_ImagePath(ImagePath)
	{
		SetBoxStyle(m_pAssetsEditor->m_Path_Box_Dialog);
		
		CImagePicker* pImagePicker = new CPaletteImagePicker(pCursorTool, this, m_ImagePath);
		pImagePicker->EnableSelection();
		Add(pImagePicker);
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

CCursorTool_TileStamp::CCursorTool_TileStamp(CViewMap* pViewMap) :
	CCursorTool(pViewMap, "Stamp", pViewMap->AssetsEditor()->m_Path_Sprite_IconStamp)
{
	m_SelectionEnabled = false;
	m_DragEnabled = false;
}

void CCursorTool_TileStamp::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(Button == KEY_MOUSE_1)
	{
		m_DragEnabled = true;
		
		if(m_SelectionEnabled)
			m_Token = AssetsManager()->GenerateToken();
		
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		m_Pivot = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
	}
	else if(Button == KEY_MOUSE_2)
	{
		if(m_SelectionEnabled)
			m_SelectionEnabled = false;
		else
		{
			if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
			{
				const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
					Context()->DisplayPopup(new CPopup_ZonePalette(AssetsEditor(), this, ViewMap()->GetViewRect(), pLayer->GetZoneTypePath()));
			}
			else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
			{
				const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
					Context()->DisplayPopup(new CPopup_TilePalette(AssetsEditor(), this, ViewMap()->GetViewRect(), pLayer->GetImagePath()));
			}
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_TileStamp::OnViewButtonRelease(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(m_DragEnabled)
	{
		m_DragEnabled = false;
		
		if(!m_SelectionEnabled)
		{
			vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
			vec2 CursorTilePos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
		
			int TileX0 = static_cast<int>(m_Pivot.x);
			int TileY0 = static_cast<int>(m_Pivot.y);
			int TileX1 = static_cast<int>(CursorTilePos.x);
			int TileY1 = static_cast<int>(CursorTilePos.y);
		
			int X0 = min(TileX0, TileX1);
			int Y0 = min(TileY0, TileY1);
			int X1 = max(TileX0, TileX1) + 1;
			int Y1 = max(TileY0, TileY1) + 1;
			
			if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
			{
				const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
				{
					m_Selection.resize(X1-X0, Y1-Y0);
				
					for(int j=0; j<m_Selection.get_height(); j++)
					{
						for(int i=0; i<m_Selection.get_width(); i++)
						{
							CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(clamp(X0+i, 0, pLayer->GetTileWidth()-1), clamp(Y0+j, 0, pLayer->GetTileHeight()-1));
							m_Selection.get_clamp(i, j).SetIndex(pLayer->GetTileIndex(TilePath));
							m_Selection.get_clamp(i, j).SetFlags(pLayer->GetTileFlags(TilePath));
						}
					}
					
					m_SelectionEnabled = true;
				}
			}
			else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
			{
				const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
				{
					m_Selection.resize(X1-X0, Y1-Y0);
				
					for(int j=0; j<m_Selection.get_height(); j++)
					{
						for(int i=0; i<m_Selection.get_width(); i++)
						{
							CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(clamp(X0+i, 0, pLayer->GetTileWidth()-1), clamp(Y0+j, 0, pLayer->GetTileHeight()-1));
							m_Selection.get_clamp(i, j).SetIndex(pLayer->GetTileIndex(TilePath));
						}
					}
					
					m_SelectionEnabled = true;
				}
			}
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
	
	m_Token = CAssetsHistory::NEW_TOKEN;
}
	
void CCursorTool_TileStamp::OnViewMouseMove()
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(!m_SelectionEnabled || !m_DragEnabled)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);

	int TileX = CursorMapPos.x/32.0f;
	int TileY = CursorMapPos.y/32.0f;
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
		if(pLayer)
		{
			int MinX = clamp(TileX, 0, pLayer->GetTileWidth()-1);
			int MinY = clamp(TileY, 0, pLayer->GetTileHeight()-1);
			int MaxX = clamp(TileX + m_Selection.get_width(), 1, pLayer->GetTileWidth());
			int MaxY = clamp(TileY + m_Selection.get_height(), 1, pLayer->GetTileHeight());
			
			for(int j=MinY; j<MaxY; j++)
			{
				for(int i=MinX; i<MaxX; i++)
				{
					CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(i, j);
					AssetsManager()->SetAssetValue<uint32>(AssetsEditor()->GetEditedAssetPath(), TilePath, CAsset_MapLayerTiles::TILE_INDEX, m_Selection.get_clamp(i-TileX, j-TileY).GetIndex(), m_Token);
					AssetsManager()->SetAssetValue<uint32>(AssetsEditor()->GetEditedAssetPath(), TilePath, CAsset_MapLayerTiles::TILE_FLAGS, m_Selection.get_clamp(i-TileX, j-TileY).GetFlags(), m_Token);
				}
			}
		}
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
	{
		const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
		if(pLayer)
		{
			int MinX = clamp(TileX, 0, pLayer->GetTileWidth()-1);
			int MinY = clamp(TileY, 0, pLayer->GetTileHeight()-1);
			int MaxX = clamp(TileX + m_Selection.get_width(), 1, pLayer->GetTileWidth());
			int MaxY = clamp(TileY + m_Selection.get_height(), 1, pLayer->GetTileHeight());
			
			for(int j=MinY; j<MaxY; j++)
			{
				for(int i=MinX; i<MaxX; i++)
				{
					CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
					AssetsManager()->SetAssetValue<uint32>(AssetsEditor()->GetEditedAssetPath(), TilePath, CAsset_MapZoneTiles::TILE_INDEX, m_Selection.get_clamp(i-TileX, j-TileY).GetIndex(), m_Token);
				}
			}
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_TileStamp::RenderView()
{	
	CAssetPath ImagePath;
	vec4 Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
		if(!pLayer)
			return;
		
		ImagePath = pLayer->GetImagePath();
		Color = pLayer->GetColor();
	}
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	float Time = fmod(ViewMap()->MapRenderer()->GetLocalTime(), 1.0f);
	Color.a *= 0.3f + 0.7f*(1.0f+cos(2.0f*pi*Time))/2.0f;
	
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	vec2 CursorTilePos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
	
	if(m_SelectionEnabled)
	{
		vec2 LayerPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(CursorTilePos.x), floor(CursorTilePos.y)));
		
		if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
		{
			ViewMap()->MapRenderer()->RenderTiles_Image(m_Selection, LayerPos, ImagePath, Color, false);
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
		{
			const CAsset_MapZoneTiles* pZone = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
			if(!pZone)
			{
				ViewMap()->MapRenderer()->UnsetGroup();
				return;
			}
				
			ViewMap()->MapRenderer()->RenderTiles_Zone(
				pZone->GetZoneTypePath(),
				m_Selection,
				LayerPos,
				1.0f, 
				AssetsEditor()->m_Path_Image_ZoneTexture,
				false
			);
		}
		
		vec2 LayerScreenPos0 = ViewMap()->MapRenderer()->MapPosToScreenPos(LayerPos);
		vec2 LayerScreenPos1 = ViewMap()->MapRenderer()->MapPosToScreenPos(LayerPos + vec2(m_Selection.get_width(), m_Selection.get_height())*32.0f);
		vec2 LayerScreenSize = LayerScreenPos1 - LayerScreenPos0;
		
		gui::CRect Rect;
		Rect.x = LayerScreenPos0.x;
		Rect.y = LayerScreenPos0.y;
		Rect.w = LayerScreenSize.x;
		Rect.h = LayerScreenSize.y;
		
		AssetsRenderer()->DrawGuiRect(&Rect, AssetsEditor()->m_Path_Rect_Selection);
	}
	else if(m_DragEnabled)
	{
		int TileX0 = static_cast<int>(m_Pivot.x);
		int TileY0 = static_cast<int>(m_Pivot.y);
		int TileX1 = static_cast<int>(CursorTilePos.x);
		int TileY1 = static_cast<int>(CursorTilePos.y);
	
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
	
void CCursorTool_TileStamp::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerTiles::TypeId:
		case CAsset_MapZoneTiles::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	CCursorTool::Update(ParentEnabled);
}
	
void CCursorTool_TileStamp::PaletteCallback_SelectImage(CAssetPath ImagePath, int MinX, int MinY, int MaxX, int MaxY)
{
	const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(ImagePath);
	if(pImage)
	{
		int Width = MaxX-MinX;
		int Height = MaxY-MinY;
		
		m_Selection.resize(Width, Height);
		
		for(int j=0; j<Height; j++)
		{
			for(int i=0; i<Width; i++)
			{
				int Index = (MinY + j) * pImage->GetGridWidth() + MinX + i;
				m_Selection.get_clamp(i, j).SetIndex(Index);
				m_Selection.get_clamp(i, j).SetFlags(0x0);
			}
		}
	}
	else
	{
		m_Selection.resize(1, 1);
		m_Selection.get_clamp(0, 0).SetIndex(0);	
		m_Selection.get_clamp(0, 0).SetFlags(0x0);
	}
	m_SelectionEnabled = true;
}
	
void CCursorTool_TileStamp::PaletteCallback_SelectZoneType(CAssetPath ZoneTypePath, CSubPath Index)
{
	int Number = 0;
	
	const CAsset_ZoneType* pZoneType = AssetsManager()->GetAsset<CAsset_ZoneType>(ZoneTypePath);
	if(pZoneType)
		Number = pZoneType->GetIndexNumber(Index);
	
	m_Selection.resize(1, 1);
	m_Selection.get_clamp(0, 0).SetIndex(Number);
	m_Selection.get_clamp(0, 0).SetFlags(0x0);		
	m_SelectionEnabled = true;
}
