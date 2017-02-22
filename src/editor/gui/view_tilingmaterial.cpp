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

#include <editor/gui/view_tilingmaterial.h>

#include <editor/components/gui.h>
#include <client/maprenderer.h>
#include <shared/autolayer.h>

#include <random>

class CRegenerateZones : public gui::CButton
{
protected:
	CViewTilingMaterial* m_pView;
	
protected:
	virtual void MouseClickAction()
	{
		m_pView->GenerateMapPreview();
		m_pView->RefreshMapPreview();
	}
	
public:
	CRegenerateZones(CGuiEditor* pAssetsEditor, CViewTilingMaterial* pView) :
		gui::CButton(pAssetsEditor, _LSTRING("Regenerate zones"), pAssetsEditor->m_Path_Sprite_IconSystem),
		m_pView(pView)
	{
		NoTextClipping();
	}
};

class CApplyRules : public gui::CButton
{
protected:
	CViewTilingMaterial* m_pView;
	
protected:
	virtual void MouseClickAction()
	{
		m_pView->RefreshMapPreview();
	}
	
public:
	CApplyRules(CGuiEditor* pAssetsEditor, CViewTilingMaterial* pView) :
		gui::CButton(pAssetsEditor, _LSTRING("Apply rules"), pAssetsEditor->m_Path_Sprite_IconSystem),
		m_pView(pView)
	{
		NoTextClipping();
	}
};

/* VIEW TILING MATERIAL ***********************************************/

CViewTilingMaterial::CViewTilingMaterial(CGuiEditor* pAssetsEditor) :
	CViewManager::CView(pAssetsEditor)
{	
	m_pMapRenderer.reset(new CMapRenderer(AssetsEditor()->EditorKernel()));
	
	m_pToolbar->Add(new CRegenerateZones(AssetsEditor(), this));
	m_pToolbar->Add(new CApplyRules(AssetsEditor(), this));
	
	GenerateMapPreview();
}

void CViewTilingMaterial::GenerateMapPreview()
{
	int Width=256;
	int Height=256;
	
	m_MapPreview.resize(Width, Height);
	
	std::random_device RandomDevice;
	std::mt19937 RandomEngine(RandomDevice());
	std::uniform_int_distribution<int> Distribution(0, 1);	
	
	for(int j=0; j<Height; j++)
	{
		for(int i=0; i<Width; i++)
		{
			if(j==0 || i==0 || j==Height-1 || i==Width-1)
				m_MapPreview.get_clamp(i, j).SetBrush(0);
			else
			{
				if(j%2==0 || i%2==0)
					m_MapPreview.get_clamp(i, j).SetBrush(m_MapPreview.get_clamp(i - (1 - i%2), j - (1 - j%2)).GetBrush());
				else
					m_MapPreview.get_clamp(i, j).SetBrush(Distribution(RandomEngine));
			}
		}
	}
}

void CViewTilingMaterial::RefreshMapPreview()
{
	ApplyTilingMaterials_Tiles(AssetsManager(), m_MapPreview, AssetsEditor()->GetEditedAssetPath(), 0, 0, m_MapPreview.get_width(), m_MapPreview.get_height(), rand());
	m_MaterialApplied = AssetsEditor()->GetEditedAssetPath();
}

void CViewTilingMaterial::OnButtonClick(int Button)
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
	{
		if(
			AssetsEditor()->GetEditedSubPath().GetType() == CAsset_TilingMaterial::TYPE_RULE ||
			AssetsEditor()->GetEditedSubPath().GetType() == CAsset_TilingMaterial::TYPE_RULE_CONDITION
		)
		{
			vec2 LayerShift = vec2(-3*16.0f, -3*16.0f);
			
			const CAsset_TilingMaterial* pMaterial = AssetsManager()->GetAsset<CAsset_TilingMaterial>(AssetsEditor()->GetEditedAssetPath());
			CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(AssetsEditor()->GetEditedSubPath().GetId());
			if(pMaterial && pMaterial->IsValidRule(RulePath))
			{
				vec2 CursorMapPos = MapRenderer()->ScreenPosToMapPos(vec2(Context()->GetMousePos().x,Context()->GetMousePos().y));
				vec2 CursorTilePos = MapRenderer()->MapPosToTilePos(CursorMapPos - LayerShift);
				int X=std::floor(CursorTilePos.x)-1;
				int Y=std::floor(CursorTilePos.y)-1;
				
				const std::vector<CAsset_TilingMaterial::CRule::CCondition>& Conditions = pMaterial->GetRuleConditionArray(RulePath);
				int ConditionFound = -1;
				for(unsigned int c=0; c<Conditions.size(); c++)
				{
					if(Conditions[c].GetRelPosX() == X && Conditions[c].GetRelPosY() == Y)
					{
						ConditionFound = c;
						break;
					}
				}
				
				if(ConditionFound >= 0)
				{
					CSubPath CondPath = CAsset_TilingMaterial::SubPath_RuleCondition(RulePath.GetId(), ConditionFound);
					if(Button == KEY_MOUSE_1)
					{
						AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), CondPath);
					}
					else if(Button == KEY_MOUSE_2)
					{
						AssetsManager()->DeleteSubItem(AssetsEditor()->GetEditedAssetPath(), CAsset_TilingMaterial::SubPath_RuleCondition(RulePath.GetId(), ConditionFound));
						
						CAssetState* pState = AssetsManager()->GetAssetState(m_pAssetsEditor->GetEditedAssetPath());
						pState->m_NumUpdates++;
					}
					else if(Button == KEY_MOUSE_3)
					{
						int Type = AssetsManager()->GetAssetValue<int>(m_pAssetsEditor->GetEditedAssetPath(), CondPath, CAsset_TilingMaterial::RULE_CONDITION_TYPE, 0);
						Type = (Type+1)%CAsset_TilingMaterial::NUM_CONDITIONTYPES;
						AssetsManager()->SetAssetValue<int>(m_pAssetsEditor->GetEditedAssetPath(), CondPath, CAsset_TilingMaterial::RULE_CONDITION_TYPE, Type);
						switch(Type)
						{
							case CAsset_TilingMaterial::CONDITIONTYPE_INDEX:
							case CAsset_TilingMaterial::CONDITIONTYPE_NOTINDEX:
								AssetsManager()->SetAssetValue<int>(m_pAssetsEditor->GetEditedAssetPath(), CondPath, CAsset_TilingMaterial::RULE_CONDITION_VALUE, 1);
								break;
							case CAsset_TilingMaterial::CONDITIONTYPE_LABEL:
							case CAsset_TilingMaterial::CONDITIONTYPE_NOTLABEL:
							case CAsset_TilingMaterial::CONDITIONTYPE_NOBORDER:
								AssetsManager()->SetAssetValue<int>(m_pAssetsEditor->GetEditedAssetPath(), CondPath, CAsset_TilingMaterial::RULE_CONDITION_VALUE, 0);
								break;
						}
					}
				}
				else if(Button == KEY_MOUSE_1)
				{
					int Token = AssetsManager()->GenerateToken();
					CSubPath CondPath = CAsset_TilingMaterial::SubPath_RuleCondition(RulePath.GetId(), AssetsManager()->AddSubItem(AssetsEditor()->GetEditedAssetPath(), RulePath, CAsset_TilingMaterial::TYPE_RULE_CONDITION, Token));
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CondPath, CAsset_TilingMaterial::RULE_CONDITION_TYPE, CAsset_TilingMaterial::CONDITIONTYPE_INDEX, Token);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CondPath, CAsset_TilingMaterial::RULE_CONDITION_VALUE, 1, Token);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSX, X, Token);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSY, Y, Token);
					AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), CondPath);
					
					CAssetState* pState = AssetsManager()->GetAssetState(m_pAssetsEditor->GetEditedAssetPath());
					pState->m_NumUpdates++;
				}
			}
		}
	}
	
	CView::OnButtonClick(Button);
}

void CViewTilingMaterial::RenderView()
{
	MapRenderer()->SetTime(0.0f);
	MapRenderer()->SetLocalTime((double)time_get()/(double)time_freq());
	MapRenderer()->SetCanvas(m_ViewRect, vec2(m_ViewRect.x + m_ViewRect.w/2, m_ViewRect.y + m_ViewRect.h/2));
	
	if(
		AssetsEditor()->GetEditedSubPath().GetType() == CAsset_TilingMaterial::TYPE_RULE ||
		AssetsEditor()->GetEditedSubPath().GetType() == CAsset_TilingMaterial::TYPE_RULE_CONDITION
	)
	{
		const CAsset_TilingMaterial* pMaterial = AssetsManager()->GetAsset<CAsset_TilingMaterial>(AssetsEditor()->GetEditedAssetPath());
		CSubPath RulePath = CAsset_TilingMaterial::SubPath_Rule(AssetsEditor()->GetEditedSubPath().GetId());
		if(pMaterial && pMaterial->IsValidRule(RulePath))
		{
			vec2 LayerShift = vec2(-3*16.0f, -3*16.0f);
			MapRenderer()->SetCamera(0.0f, 2.0f);
			
			int Index = AssetsManager()->GetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), AssetsEditor()->GetEditedSubPath(), CAsset_TilingMaterial::RULE_TILEINDEX, 0);
			int Flags = AssetsManager()->GetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), AssetsEditor()->GetEditedSubPath(), CAsset_TilingMaterial::RULE_TILEFLAGS, 0x0);
			
			array2d<CAsset_MapLayerTiles::CTile> Tiles;
			Tiles.resize(3, 3);
			
			Tiles.get_clamp(0, 0).SetIndex(0);
			Tiles.get_clamp(0, 1).SetIndex(0);
			Tiles.get_clamp(0, 2).SetIndex(0);
			
			Tiles.get_clamp(1, 0).SetIndex(0);
			Tiles.get_clamp(1, 2).SetIndex(0);
			
			Tiles.get_clamp(2, 0).SetIndex(0);
			Tiles.get_clamp(2, 1).SetIndex(0);
			Tiles.get_clamp(2, 2).SetIndex(0);
			
			Tiles.get_clamp(1, 1).SetIndex(Index);
			Tiles.get_clamp(1, 1).SetFlags(Flags);
			MapRenderer()->RenderTiles_Style(Tiles, LayerShift, AssetsEditor()->GetEditedAssetPath(), 1.0f, false);
			
			Graphics()->TextureClear();
			Graphics()->QuadsBegin();
			
			const std::vector<CAsset_TilingMaterial::CRule::CCondition>& Conditions = pMaterial->GetRuleConditionArray(RulePath);
			for(unsigned int c=0; c<Conditions.size(); c++)
			{
				vec2 V0 = MapRenderer()->MapPosToScreenPos(MapRenderer()->TilePosToMapPos(vec2(Conditions[c].GetRelPosX()+1, Conditions[c].GetRelPosY()+1)) + LayerShift);
				vec2 V3 = MapRenderer()->MapPosToScreenPos(MapRenderer()->TilePosToMapPos(vec2(Conditions[c].GetRelPosX()+2, Conditions[c].GetRelPosY()+2)) + LayerShift);
				
				if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_INDEX || Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_LABEL)
					Graphics()->SetColor(vec4(0.7f, 1.0f, 0.7f, 0.5f), true);
				else if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_NOTINDEX || Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_NOTLABEL)
					Graphics()->SetColor(vec4(1.0f, 0.7f, 0.7f, 0.5f), true);
				else if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_NOBORDER)
					Graphics()->SetColor(vec4(1.0f, 0.7f, 1.0f, 0.5f), true);
				
				CGraphics::CQuadItem QuadItem(V0.x, V0.y, V3.x - V0.x, V3.y - V0.y);
				Graphics()->QuadsDrawTL(&QuadItem, 1);
			}
			
			Graphics()->QuadsEnd();
			
			CLocalizableString LString_Index(_("Index {int:Index}"));
			CLocalizableString LString_Label(_("Label {int:Index}"));
			CLocalizableString LString_NoBorder(_("No Border"));
			dynamic_string Buffer;
			
			for(unsigned int c=0; c<Conditions.size(); c++)
			{
				vec2 V0 = MapRenderer()->MapPosToScreenPos(MapRenderer()->TilePosToMapPos(vec2(Conditions[c].GetRelPosX()+1, Conditions[c].GetRelPosY()+1)) + LayerShift);
				vec2 V3 = MapRenderer()->MapPosToScreenPos(MapRenderer()->TilePosToMapPos(vec2(Conditions[c].GetRelPosX()+2, Conditions[c].GetRelPosY()+2)) + LayerShift);
				vec2 Pos = (V0 + V3)/2.0f;
				
				CTextRenderer::CTextCache TextCache;
				TextCache.SetText(Buffer.buffer());
				
				if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_INDEX || Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_NOTINDEX)
				{
					CSubPath IndexPath = CAsset_TilingMaterial::SubPath_Index(Conditions[c].GetValue());
					if(pMaterial->IsValidIndex(IndexPath))
						TextCache.SetText(pMaterial->GetIndexTitle(IndexPath));
					else
					{
						LString_Index.ClearParameters();
						LString_Index.AddInteger("Index", Conditions[c].GetValue());
					
						Buffer.clear();
						Localization()->Format(Buffer, NULL, LString_Index);
						
						TextCache.SetText(Buffer.buffer());
					}
				}
				else if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_LABEL || Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_NOTLABEL)
				{
					CSubPath LabelPath = CAsset_TilingMaterial::SubPath_Label(Conditions[c].GetValue());
					if(pMaterial->IsValidLabel(LabelPath))
						TextCache.SetText(pMaterial->GetLabelTitle(LabelPath));
					else
					{
						LString_Label.ClearParameters();
						LString_Label.AddInteger("Index", Conditions[c].GetValue());
					
						Buffer.clear();
						Localization()->Format(Buffer, NULL, LString_Label);
						
						TextCache.SetText(Buffer.buffer());
					}
				}
				else if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_NOBORDER)
				{
					Buffer.clear();
					Localization()->Format(Buffer, NULL, LString_NoBorder);
					TextCache.SetText(Buffer.buffer());
				}
				
				vec4 TextColor = 1.0f;
				if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_INDEX || Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_LABEL)
					TextColor = vec4(0.0f, 0.5f, 0.0f, 1.0f);
				else if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_NOTINDEX || Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_NOTLABEL)
					TextColor = vec4(0.5f, 0.0f, 0.0f, 1.0f);
				else if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_NOBORDER)
					TextColor = vec4(0.5f, 0.0f, 0.5f, 1.0f);
				
				TextCache.SetBoxSize(ivec2(V3.x - V0.x, V3.y - V0.y));
				TextCache.SetFontSize(12.0f);
				float TextWidth = TextRenderer()->GetTextWidth(&TextCache);
				TextRenderer()->DrawText(&TextCache, ivec2(Pos.x - TextWidth/2.0f, Pos.y - (V3.y - V0.y)/2.0f), TextColor);
			}
		}
	}
	else
	{
		MapRenderer()->SetCamera(0.0f, 1.0f);
		
		if(m_MaterialApplied != AssetsEditor()->GetEditedAssetPath())
			RefreshMapPreview();
		
		MapRenderer()->RenderTiles_Style(m_MapPreview, vec2(-m_MapPreview.get_width(), -m_MapPreview.get_height())*16.0f, AssetsEditor()->GetEditedAssetPath(), 1.0f, false);
	}
}
