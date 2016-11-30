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

#include <editor/gui/assetsinspector.h>
#include <editor/components/gui.h>
#include <shared/components/assetsmanager.h>
#include <client/gui/text-edit.h>
#include <client/gui/integer-edit.h>
#include <client/gui/float-edit.h>
#include <client/gui/color-edit.h>
#include <client/gui/toggle.h>
#include <client/gui/popup.h>
#include <client/gui/expand.h>
#include <shared/components/localization.h>

class CSubItemEditor : public gui::CVListLayout
{
protected:
	CGuiEditor* m_pAssetsEditor;
	int m_SubPathType;
	
public:
	CSubItemEditor(CGuiEditor* pAssetsEditor, int SubPathType) :
		gui::CVListLayout(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_SubPathType(SubPathType)
	{
		
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(ParentEnabled)
		{
			if(m_pAssetsEditor->GetEditedSubPath().GetType() == m_SubPathType)
				Enable();
			else
				Disable();
		}
		
		gui::CVListLayout::Update(ParentEnabled);
	}
};

class CSubItem : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CSubPath m_SubPath;
	
protected:
	virtual void MouseClickAction()
	{
		m_pAssetsEditor->SetEditedAsset(m_pAssetsEditor->GetEditedAssetPath(), m_SubPath);
	}

public:
	CSubItem(CGuiEditor* pAssetsEditor, CSubPath SubPath, const char* pName, CAssetPath IconPath) :
		gui::CButton(pAssetsEditor, pName, IconPath),
		m_pAssetsEditor(pAssetsEditor),
		m_SubPath(SubPath)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItem);
	}
	
	CSubItem(CGuiEditor* pAssetsEditor, CSubPath SubPath, const CLocalizableString& Text, CAssetPath IconPath) :
		gui::CButton(pAssetsEditor, Text, IconPath),
		m_pAssetsEditor(pAssetsEditor),
		m_SubPath(SubPath)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItem);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(m_pAssetsEditor->GetEditedSubPath() == m_SubPath)
			SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItemHL);
		else
			SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItem);
		
		gui::CButton::Update(ParentEnabled);
	}
};

/* ASSETS INSPECTOR ***************************************************/

CAssetsInspector::CAssetsInspector(CGuiEditor* pAssetsEditor) :
	gui::CTabs(pAssetsEditor),
	m_pAssetsEditor(pAssetsEditor)
{
	for(int i=0; i<NUM_TABS; i++)
		m_pTabs[TAB_IMAGE_ASSET] = 0;
	
	//TAG_NEW_ASSET
	m_pTabs[TAB_GENERIC_ASSET] = CreateTab_Generic_Asset();
	m_pTabs[TAB_GUIRECTSTYLE_ASSET] = CreateTab_GuiRectStyle_Asset();
	m_pTabs[TAB_GUILINESTYLE_ASSET] = CreateTab_GuiLineStyle_Asset();
	m_pTabs[TAB_GUIBOXSTYLE_ASSET] = CreateTab_GuiBoxStyle_Asset();
	m_pTabs[TAB_GUILABELSTYLE_ASSET] = CreateTab_GuiLabelStyle_Asset();
	m_pTabs[TAB_GUIBUTTONSTYLE_ASSET] = CreateTab_GuiButtonStyle_Asset();
	m_pTabs[TAB_GUIINTEDITSTYLE_ASSET] = CreateTab_GuiIntEditStyle_Asset();
	m_pTabs[TAB_GUITOGGLESTYLE_ASSET] = CreateTab_GuiToggleStyle_Asset();
	m_pTabs[TAB_GUISLIDERSTYLE_ASSET] = CreateTab_GuiSliderStyle_Asset();
	m_pTabs[TAB_GUISCROLLBARSTYLE_ASSET] = CreateTab_GuiScrollbarStyle_Asset();
	m_pTabs[TAB_GUITABSSTYLE_ASSET] = CreateTab_GuiTabsStyle_Asset();
	m_pTabs[TAB_IMAGE_ASSET] = CreateTab_Image_Asset();
	m_pTabs[TAB_MAP_ASSET] = CreateTab_Map_Asset();
	m_pTabs[TAB_MAPGROUP_ASSET] = CreateTab_MapGroup_Asset();
	m_pTabs[TAB_MAPLAYERTILES_ASSET] = CreateTab_MapLayerTiles_Asset();
	m_pTabs[TAB_MAPLAYERQUADS_ASSET] = CreateTab_MapLayerQuads_Asset();
	m_pTabs[TAB_MAPZONETILES_ASSET] = CreateTab_MapZoneTiles_Asset();
	m_pTabs[TAB_MAPENTITIES_ASSET] = CreateTab_MapEntities_Asset();
	m_pTabs[TAB_ZONETYPE_ASSET] = CreateTab_ZoneType_Asset();
	m_pTabs[TAB_ENTITYTYPE_ASSET] = CreateTab_EntityType_Asset();
	m_pTabs[TAB_IMAGE_ASSET] = CreateTab_Image_Asset();
	m_pTabs[TAB_SPRITE_ASSET] = CreateTab_Sprite_Asset();
}

void CAssetsInspector::Update(bool ParentEnabled)
{
	for(int i=0; i<NUM_TABS; i++)
		m_pTabs[i]->Disable();
	
	if(AssetsManager()->GetAssetState(AssetsEditor()->GetEditedAssetPath()))
	{
		Enable();
		
		//TAG_NEW_ASSET
		switch(AssetsEditor()->GetEditedAssetPath().GetType())
		{
			case CAsset_GuiRectStyle::TypeId:
				m_pTabs[TAB_GUIRECTSTYLE_ASSET]->Enable();
				break;
			case CAsset_GuiLineStyle::TypeId:
				m_pTabs[TAB_GUILINESTYLE_ASSET]->Enable();
				break;
			case CAsset_GuiBoxStyle::TypeId:
				m_pTabs[TAB_GUIBOXSTYLE_ASSET]->Enable();
				break;
			case CAsset_GuiLabelStyle::TypeId:
				m_pTabs[TAB_GUILABELSTYLE_ASSET]->Enable();
				break;
			case CAsset_GuiButtonStyle::TypeId:
				m_pTabs[TAB_GUIBUTTONSTYLE_ASSET]->Enable();
				break;
			case CAsset_GuiIntEditStyle::TypeId:
				m_pTabs[TAB_GUIINTEDITSTYLE_ASSET]->Enable();
				break;
			case CAsset_GuiToggleStyle::TypeId:
				m_pTabs[TAB_GUITOGGLESTYLE_ASSET]->Enable();
				break;
			case CAsset_GuiSliderStyle::TypeId:
				m_pTabs[TAB_GUISLIDERSTYLE_ASSET]->Enable();
				break;
			case CAsset_GuiScrollbarStyle::TypeId:
				m_pTabs[TAB_GUISCROLLBARSTYLE_ASSET]->Enable();
				break;
			case CAsset_GuiTabsStyle::TypeId:
				m_pTabs[TAB_GUITABSSTYLE_ASSET]->Enable();
				break;
			case CAsset_Image::TypeId:
				m_pTabs[TAB_IMAGE_ASSET]->Enable();
				break;
			case CAsset_Map::TypeId:
				m_pTabs[TAB_MAP_ASSET]->Enable();
				break;
			case CAsset_MapGroup::TypeId:
				m_pTabs[TAB_MAPGROUP_ASSET]->Enable();
				break;
			case CAsset_MapLayerTiles::TypeId:
				m_pTabs[TAB_MAPLAYERTILES_ASSET]->Enable();
				break;
			case CAsset_MapLayerQuads::TypeId:
				m_pTabs[TAB_MAPLAYERQUADS_ASSET]->Enable();
				break;
			case CAsset_MapZoneTiles::TypeId:
				m_pTabs[TAB_MAPZONETILES_ASSET]->Enable();
				break;
			case CAsset_MapEntities::TypeId:
				m_pTabs[TAB_MAPENTITIES_ASSET]->Enable();
				break;
			case CAsset_ZoneType::TypeId:
				m_pTabs[TAB_ZONETYPE_ASSET]->Enable();
				break;
			case CAsset_EntityType::TypeId:
				m_pTabs[TAB_ENTITYTYPE_ASSET]->Enable();
				break;
			case CAsset_Sprite::TypeId:
				m_pTabs[TAB_SPRITE_ASSET]->Enable();
				break;
			default:
				m_pTabs[TAB_GENERIC_ASSET]->Enable();
				break;
		}
		
		gui::CTabs::Update(ParentEnabled);
	}
	else
	{
		Disable();
	
		return;
	}
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_Generic_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	
	pTab->Disable();
	AddTab(pTab, _GUI("Asset"), AssetsEditor()->m_Path_Sprite_IconAsset);
	
	AddField_AssetProperties(pTab);
	
	return pTab;
}

/* IMAGE **************************************************************/
	
class CImageTilingToggle : public gui::CAbstractToggle
{
protected:
	CGuiEditor* m_pAssetsEditor;
	
	virtual bool GetValue()
	{
		return m_pAssetsEditor->AssetsManager()->GetAssetValue<bool>(
			m_pAssetsEditor->GetEditedAssetPath(),
			CSubPath::Null(),
			CAsset_Image::TILINGENABLED,
			false
		);
	}
	
	virtual void SetValue(bool Value)
	{
		m_pAssetsEditor->AssetsManager()->SetAssetValue<bool>(
			m_pAssetsEditor->GetEditedAssetPath(),
			CSubPath::Null(),
			CAsset_Image::TILINGENABLED,
			Value
		);
		if(Value)
		{
			m_pAssetsEditor->AssetsManager()->SetAssetValue<int>(
				m_pAssetsEditor->GetEditedAssetPath(),
				CSubPath::Null(),
				CAsset_Image::GRIDWIDTH,
				16
			);
			m_pAssetsEditor->AssetsManager()->SetAssetValue<int>(
				m_pAssetsEditor->GetEditedAssetPath(),
				CSubPath::Null(),
				CAsset_Image::GRIDHEIGHT,
				16
			);
		}
		AssetsManager()->RequestUpdate(m_pAssetsEditor->GetEditedAssetPath());
	}
	
public:
	CImageTilingToggle(CGuiEditor* pAssetsEditor) :
		gui::CAbstractToggle(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor)
	{ }
};

gui::CVScrollLayout* CAssetsInspector::CreateTab_Image_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Image"), AssetsEditor()->m_Path_Sprite_IconImage);
	
	AddField_AssetProperties(pTab);
	
	AddField_Integer_NoEdit(pTab, CAsset_Image::DATA_WIDTH, _GUI("Width"));
	AddField_Integer_NoEdit(pTab, CAsset_Image::DATA_HEIGHT, _GUI("Height"));
	AddField_Integer(pTab, CAsset_Image::TEXELSIZE, _GUI("Texel size"));
	AddField_Integer(pTab, CAsset_Image::GRIDWIDTH, _GUI("Grid width"));
	AddField_Integer(pTab, CAsset_Image::GRIDHEIGHT, _GUI("Grid height"));
	AddField_Integer(pTab, CAsset_Image::GRIDSPACING, _GUI("Grid spacing"));
	
	CImageTilingToggle* pWidget = new CImageTilingToggle(m_pAssetsEditor);
	AddField(pTab, pWidget, _GUI("Compatible with tiles"));
	
	return pTab;
}

/* SPRITE *************************************************************/

gui::CVScrollLayout* CAssetsInspector::CreateTab_Sprite_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Sprite"), AssetsEditor()->m_Path_Sprite_IconSprite);
	
	AddField_AssetProperties(pTab);
	
	AddField_Asset(pTab, CAsset_Sprite::IMAGEPATH, CAsset_Image::TypeId, _GUI("Image"));
	AddField_Integer(pTab, CAsset_Sprite::X, _GUI("X"));
	AddField_Integer(pTab, CAsset_Sprite::Y, _GUI("Y"));
	AddField_Integer(pTab, CAsset_Sprite::WIDTH, _GUI("Width"));
	AddField_Integer(pTab, CAsset_Sprite::HEIGHT, _GUI("Height"));
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_Map_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Map"), AssetsEditor()->m_Path_Sprite_IconMap);
	
	AddField_AssetProperties(pTab);
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_MapGroup_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Map Group"), AssetsEditor()->m_Path_Sprite_IconFolder);
	
	AddField_AssetProperties(pTab);
	
	AddField_Vec2(pTab, CAsset_MapGroup::POSITION_X, CAsset_MapGroup::POSITION_Y, _GUI("Position"));	
	AddField_Vec2(pTab, CAsset_MapGroup::HARDPARALLAX_X, CAsset_MapGroup::HARDPARALLAX_Y, _GUI("Parallax"));	
	pTab->AddSeparator();
	AddField_Bool(pTab, CAsset_MapGroup::CLIPPING, _GUI("Clipping"));	
	AddField_Vec2(pTab, CAsset_MapGroup::CLIPPOSITION_X, CAsset_MapGroup::CLIPPOSITION_Y, _GUI("Clip Position"));	
	AddField_Vec2(pTab, CAsset_MapGroup::CLIPSIZE_X, CAsset_MapGroup::CLIPSIZE_Y, _GUI("Clip Size"));	
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_MapLayerTiles_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Map Layer Tiles"), AssetsEditor()->m_Path_Sprite_IconTiles);
	
	AddField_AssetProperties(pTab);
		
	AddField_Integer(pTab, CAsset_MapLayerTiles::TILE_WIDTH, _GUI("Width"));	
	AddField_Integer(pTab, CAsset_MapLayerTiles::TILE_HEIGHT, _GUI("Height"));	
	AddField_ImageTiles(pTab, CAsset_MapLayerTiles::IMAGEPATH, CAsset_Image::TypeId, _GUI("Image"));
	AddField_Color(pTab, CAsset_MapLayerTiles::COLOR, _GUI("Color"));	
	
	return pTab;
}

/* MAP LAYER QUADS ****************************************************/

class CSubItemList_MapLayerQuads : public gui::CVScrollLayout
{
protected:
	CAssetPath m_AssetPath;
	CGuiEditor* m_pAssetsEditor;
	bool m_UpdateNeeded;
	
public:
	CSubItemList_MapLayerQuads(CGuiEditor* pAssetsEditor) :
		gui::CVScrollLayout(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_UpdateNeeded(true)
	{
		SetBoxStyle(m_pAssetsEditor->m_Path_Box_SubList);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(ParentEnabled && IsEnabled())
		{
			if(m_pAssetsEditor->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId && m_AssetPath != m_pAssetsEditor->GetEditedAssetPath())
			{
				m_AssetPath = m_pAssetsEditor->GetEditedAssetPath();
				m_UpdateNeeded = true;
			}
			
			if(m_UpdateNeeded)
			{
				Clear();
				char aBuf[128];
					
				const CAsset_MapLayerQuads* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(m_pAssetsEditor->GetEditedAssetPath());
				if(pLayer)
				{
					CAsset_MapLayerQuads::CIteratorQuad Iter;
					int Counter = 1;
					for(Iter = pLayer->BeginQuad(); Iter != pLayer->EndQuad(); ++Iter)
					{
						str_format(aBuf, sizeof(aBuf), "Quad %d", Counter);
						Add(new CSubItem(m_pAssetsEditor, *Iter, aBuf, m_pAssetsEditor->m_Path_Sprite_IconQuad), false);
						Counter++;
					}
					
					m_UpdateNeeded = false;
				}
			}
		}
		
		gui::CVScrollLayout::Update(ParentEnabled);
	}
};

gui::CVScrollLayout* CAssetsInspector::CreateTab_MapLayerQuads_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Map Layer Quads"), AssetsEditor()->m_Path_Sprite_IconQuad);
	
	AddField_AssetProperties(pTab);
	
	AddField_Asset(pTab, CAsset_MapLayerQuads::IMAGEPATH, CAsset_Image::TypeId, _GUI("Image"));
	AddField_Integer_NoEdit(pTab, CAsset_MapLayerQuads::QUAD_ARRAYSIZE, _GUI("Number of quads"));
	
	pTab->Add(new CSubItemList_MapLayerQuads(AssetsEditor()), true);
	
	gui::CVListLayout* pQuadEditor = new CSubItemEditor(AssetsEditor(), CAsset_MapLayerQuads::TYPE_QUAD);
	pTab->Add(pQuadEditor);
	
	AddField_Vec2(pQuadEditor, CAsset_MapLayerQuads::QUAD_PIVOT_X, CAsset_MapLayerQuads::QUAD_PIVOT_Y, _GUI("Position"));
	AddField_Vec2(pQuadEditor, CAsset_MapLayerQuads::QUAD_SIZE_X, CAsset_MapLayerQuads::QUAD_SIZE_Y, _GUI("Size"));
	AddField_Angle(pQuadEditor, CAsset_MapLayerQuads::QUAD_ANGLE, _GUI("Angle"));
	AddField_Asset(pQuadEditor, CAsset_MapLayerQuads::QUAD_ANIMATIONPATH, CAsset_SkeletonAnimation::TypeId, _GUI("Animation"));
	
	return pTab;
}

/* MAP ZONE TILES *****************************************************/

gui::CVScrollLayout* CAssetsInspector::CreateTab_MapZoneTiles_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Map Zone Tiles"), AssetsEditor()->m_Path_Sprite_IconTiles);
	
	AddField_AssetProperties(pTab);
		
	AddField_Integer(pTab, CAsset_MapZoneTiles::TILE_WIDTH, _GUI("Width"));	
	AddField_Integer(pTab, CAsset_MapZoneTiles::TILE_HEIGHT, _GUI("Height"));
	AddField_Asset(pTab, CAsset_MapZoneTiles::ZONETYPEPATH, CAsset_ZoneType::TypeId, _GUI("Zone type"));
	
	return pTab;
}

/* MAP ENTITIES *******************************************************/

class CSubItemList_MapEntities : public gui::CVScrollLayout
{
protected:
	CAssetPath m_AssetPath;
	CGuiEditor* m_pAssetsEditor;
	bool m_UpdateNeeded;
	
public:
	CSubItemList_MapEntities(CGuiEditor* pAssetsEditor) :
		gui::CVScrollLayout(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_UpdateNeeded(true)
	{
		SetBoxStyle(m_pAssetsEditor->m_Path_Box_SubList);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(ParentEnabled && IsEnabled())
		{
			if(m_pAssetsEditor->GetEditedAssetPath().GetType() == CAsset_MapEntities::TypeId && m_AssetPath != m_pAssetsEditor->GetEditedAssetPath())
			{
				m_AssetPath = m_pAssetsEditor->GetEditedAssetPath();
				m_UpdateNeeded = true;
			}
			
			if(m_UpdateNeeded)
			{
				Clear();
				char aBuf[128];
					
				const CAsset_MapEntities* pEntities = AssetsManager()->GetAsset<CAsset_MapEntities>(m_pAssetsEditor->GetEditedAssetPath());
				if(pEntities)
				{
					CAsset_MapEntities::CIteratorEntity Iter;
					int Counter = 1;
					for(Iter = pEntities->BeginEntity(); Iter != pEntities->EndEntity(); ++Iter)
					{
						CAssetPath TypePath = pEntities->GetEntityTypePath(*Iter);
						const CAsset_EntityType* pEntityType = AssetsManager()->GetAsset<CAsset_EntityType>(TypePath);
						if(pEntityType)
							Add(new CSubItem(m_pAssetsEditor, *Iter, pEntityType->GetName(), m_pAssetsEditor->m_Path_Sprite_IconEntities), false);
						else
							Add(new CSubItem(m_pAssetsEditor, *Iter, _GUI("Unknown entity"), m_pAssetsEditor->m_Path_Sprite_IconEntities), false);
						Counter++;
					}
					
					m_UpdateNeeded = false;
				}
			}
		}
		
		gui::CVScrollLayout::Update(ParentEnabled);
	}
};

gui::CVScrollLayout* CAssetsInspector::CreateTab_MapEntities_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Map Entities"), AssetsEditor()->m_Path_Sprite_IconEntities);
	
	AddField_AssetProperties(pTab);
	
	pTab->Add(new CSubItemList_MapEntities(AssetsEditor()), true);
	
	return pTab;
}

/* ZONE TYPE **********************************************************/

class CSubItemList_ZoneTypeIndices : public gui::CVScrollLayout
{
protected:
	CAssetPath m_AssetPath;
	CGuiEditor* m_pAssetsEditor;
	bool m_UpdateNeeded;
	
public:
	CSubItemList_ZoneTypeIndices(CGuiEditor* pAssetsEditor) :
		gui::CVScrollLayout(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_UpdateNeeded(true)
	{
		SetBoxStyle(m_pAssetsEditor->m_Path_Box_SubList);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(ParentEnabled && IsEnabled())
		{
			if(m_pAssetsEditor->GetEditedAssetPath().GetType() == CAsset_ZoneType::TypeId && m_AssetPath != m_pAssetsEditor->GetEditedAssetPath())
			{
				m_AssetPath = m_pAssetsEditor->GetEditedAssetPath();
				m_UpdateNeeded = true;
			}
			
			if(m_UpdateNeeded)
			{
				Clear();
				char aBuf[128];
					
				const CAsset_ZoneType* pZoneType = AssetsManager()->GetAsset<CAsset_ZoneType>(m_pAssetsEditor->GetEditedAssetPath());
				if(pZoneType)
				{
					CAsset_ZoneType::CIteratorIndex Iter;
					for(Iter = pZoneType->BeginIndex(); Iter != pZoneType->EndIndex(); ++Iter)
					{
						Add(new CSubItem(m_pAssetsEditor, *Iter, pZoneType->GetIndexDescription(*Iter), m_pAssetsEditor->m_Path_Sprite_IconZoneTiles), false);
					}
					
					m_UpdateNeeded = false;
				}
			}
		}
		
		gui::CVScrollLayout::Update(ParentEnabled);
	}
};

gui::CVScrollLayout* CAssetsInspector::CreateTab_ZoneType_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Zone Type"), AssetsEditor()->m_Path_Sprite_IconZoneType);
	
	AddField_AssetProperties(pTab);
	
	pTab->Add(new CSubItemList_ZoneTypeIndices(AssetsEditor()), true);
	
	gui::CVListLayout* pEditor = new CSubItemEditor(AssetsEditor(), CAsset_ZoneType::TYPE_INDEX);
	pTab->Add(pEditor);
	
	AddField_Bool(pEditor, CAsset_ZoneType::INDEX_USED, _GUI("Is used"));
	AddField_Text(pEditor, CAsset_ZoneType::INDEX_DESCRIPTION, _GUI("Description"));
	AddField_Color(pEditor, CAsset_ZoneType::INDEX_COLOR, _GUI("Color"));
	
	return pTab;
}

/* ENTITY TYPE ********************************************************/

gui::CVScrollLayout* CAssetsInspector::CreateTab_EntityType_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Entity Type"), AssetsEditor()->m_Path_Sprite_IconEntityType);
	
	AddField_AssetProperties(pTab);
	
	AddField_Asset(pTab, CAsset_EntityType::GIZMOPATH, CAsset_Sprite::TypeId, _GUI("Gizmo"));	
	AddField_Float(pTab, CAsset_EntityType::COLLISIONRADIUS, _GUI("Collision Radius"));	
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_GuiRectStyle_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Rectangle Style (GUI)"), AssetsEditor()->m_Path_Sprite_IconGuiRect);
	
	AddField_AssetProperties(pTab);
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_GuiLineStyle_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Line Style (GUI)"), AssetsEditor()->m_Path_Sprite_IconGuiLine);
	
	//~ AddField_Asset(pTab, CAsset_GuiLineStyle::RECTPATH, CAsset_GuiRectStyle::TypeId, "Rectangle");
	
	AddField_AssetProperties(pTab);
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_GuiBoxStyle_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Box Style (GUI)"), AssetsEditor()->m_Path_Sprite_IconGuiBox);
	
	AddField_AssetProperties(pTab);
	
	AddField_Asset(pTab, CAsset_GuiBoxStyle::RECTPATH, CAsset_GuiRectStyle::TypeId, _GUI("Rectangle"));
	AddField_Integer(pTab, CAsset_GuiBoxStyle::MINWIDTH, _GUI("Width"));
	AddField_Integer(pTab, CAsset_GuiBoxStyle::MINHEIGHT, _GUI("Height"));
	AddField_Integer(pTab, CAsset_GuiBoxStyle::MARGIN, _GUI("Margin"));
	AddField_Integer(pTab, CAsset_GuiBoxStyle::PADDING, _GUI("Padding"));
	AddField_Integer(pTab, CAsset_GuiBoxStyle::SPACING, _GUI("Spacing"));
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_GuiLabelStyle_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Label Style (GUI)"), AssetsEditor()->m_Path_Sprite_IconGuiLabel);
	
	AddField_AssetProperties(pTab);
	
	AddField_Asset(pTab, CAsset_GuiLabelStyle::RECTPATH, CAsset_GuiRectStyle::TypeId, _GUI("Rectangle"));
	AddField_Integer(pTab, CAsset_GuiLabelStyle::MINWIDTH, _GUI("Width"));
	AddField_Integer(pTab, CAsset_GuiLabelStyle::MINHEIGHT, _GUI("Height"));
	AddField_Integer(pTab, CAsset_GuiLabelStyle::MARGIN, _GUI("Margin"));
	AddField_Integer(pTab, CAsset_GuiLabelStyle::PADDING, _GUI("Padding"));
	AddField_Integer(pTab, CAsset_GuiLabelStyle::SPACING, _GUI("Spacing"));
	AddField_Integer(pTab, CAsset_GuiLabelStyle::FONTSIZE, _GUI("Font Size"));
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_GuiButtonStyle_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Button Style (GUI)"), AssetsEditor()->m_Path_Sprite_IconGuiButton);
	
	AddField_AssetProperties(pTab);
	
	AddField_Asset(pTab, CAsset_GuiButtonStyle::IDLESTYLEPATH, CAsset_GuiLabelStyle::TypeId, _GUI("Idle Style"));
	AddField_Asset(pTab, CAsset_GuiButtonStyle::MOUSEOVERSTYLEPATH, CAsset_GuiLabelStyle::TypeId, _GUI("Mouseover Style"));
	AddField_Asset(pTab, CAsset_GuiButtonStyle::READONLYSTYLEPATH, CAsset_GuiLabelStyle::TypeId, _GUI("Read Only Style"));
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_GuiIntEditStyle_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Integer Edit Style (GUI)"), AssetsEditor()->m_Path_Sprite_IconGuiIntEdit);
	
	AddField_AssetProperties(pTab);
	
	AddField_Asset(pTab, CAsset_GuiIntEditStyle::INCREASEBUTTONSTYLEPATH, CAsset_GuiButtonStyle::TypeId, _GUI("Increase Button"));
	AddField_Asset(pTab, CAsset_GuiIntEditStyle::DECREASEBUTTONSTYLEPATH, CAsset_GuiButtonStyle::TypeId, _GUI("Decrease Button"));
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_GuiToggleStyle_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Toggle Style (GUI)"), AssetsEditor()->m_Path_Sprite_IconGuiToggle);
	
	AddField_AssetProperties(pTab);
	
	AddField_Asset(pTab, CAsset_GuiToggleStyle::IDLETRUESTYLEPATH, CAsset_GuiLabelStyle::TypeId, _GUI("Idle Style (true)"));
	AddField_Asset(pTab, CAsset_GuiToggleStyle::MOUSEOVERTRUESTYLEPATH, CAsset_GuiLabelStyle::TypeId, _GUI("Mouseover Style (true)"));
	AddField_Asset(pTab, CAsset_GuiToggleStyle::IDLEFALSESTYLEPATH, CAsset_GuiLabelStyle::TypeId, _GUI("Idle Style (false)"));
	AddField_Asset(pTab, CAsset_GuiToggleStyle::MOUSEOVERFALSESTYLEPATH, CAsset_GuiLabelStyle::TypeId, _GUI("Mouseover Style (false)"));
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_GuiSliderStyle_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Slider Style"), AssetsEditor()->m_Path_Sprite_IconGuiSlider);
	
	AddField_AssetProperties(pTab);
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_GuiScrollbarStyle_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Scrollbar Style"), AssetsEditor()->m_Path_Sprite_IconGuiScrollbar);
	
	AddField_AssetProperties(pTab);
	
	return pTab;
}

gui::CVScrollLayout* CAssetsInspector::CreateTab_GuiTabsStyle_Asset()
{
	gui::CVScrollLayout* pTab = new gui::CVScrollLayout(Context());
	pTab->Disable();
	AddTab(pTab, _GUI("Tabs Style"), AssetsEditor()->m_Path_Sprite_IconGuiTabs);
	
	AddField_AssetProperties(pTab);
	
	return pTab;
}

void CAssetsInspector::AddField(gui::CVListLayout* pList, gui::CWidget* pWidget, const CLocalizableString& Text)
{
	gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
	pList->Add(pLayout, false);
	
	gui::CLabel* pLabel = new gui::CLabel(Context(), Text);
	pLayout->Add(pLabel, true);
	
	pLayout->Add(pWidget, true);
}

void CAssetsInspector::AddField(gui::CVListLayout* pList, gui::CWidget* pWidget)
{
	pList->Add(pWidget, false);
}

void CAssetsInspector::AddField_AssetProperties(gui::CVScrollLayout* pTab)
{
	AddField_Text(pTab, CAsset::NAME, _GUI("Name"));
	
	pTab->AddSeparator();
}

/* TEXT EDIT **********************************************************/

class CMemberTextEdit : public gui::CAbstractTextEdit
{
protected:
	CGuiEditor* m_pAssetsEditor;
	int m_Member;
	
	virtual void SaveFromTextBuffer()
	{
		AssetsManager()->SetAssetValue<const char*>(
			m_pAssetsEditor->GetEditedAssetPath(),
			m_pAssetsEditor->GetEditedSubPath(),
			m_Member,
			GetText()
		);
	}
	
	virtual void CopyToTextBuffer()
	{
		const char* pName = AssetsManager()->GetAssetValue<const char*>(
			m_pAssetsEditor->GetEditedAssetPath(),
			m_pAssetsEditor->GetEditedSubPath(),
			m_Member,
			NULL
		);
		
		if(pName)
		{
			if(m_Text != pName)
				SetText(pName);
		}
		else
			SetText("");
	}
	
public:
	CMemberTextEdit(CGuiEditor* pAssetsEditor, int Member) :
		gui::CAbstractTextEdit(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_Member(Member)
	{ }
	
	virtual void Update(bool ParentEnabled)
	{
		if(IsEnabled() && ParentEnabled)
		{
			if(!AssetsManager()->IsValidPackage(m_pAssetsEditor->GetEditedPackageId()) || AssetsManager()->IsReadOnlyPackage(m_pAssetsEditor->GetEditedPackageId()))
				Editable(false);
			else
				Editable(true);
		}
		
		gui::CAbstractTextEdit::Update(ParentEnabled);
	}
};

void CAssetsInspector::AddField_Text(gui::CVListLayout* pList, int Member, const CLocalizableString& Text)
{
	CMemberTextEdit* pWidget = new CMemberTextEdit(
		m_pAssetsEditor,
		Member
	);
	
	AddField(pList, pWidget, Text);
}

/* INTEGER EDIT *******************************************************/
	
class CMemberIntegerEdit : public gui::CAbstractIntegerEdit
{
protected:
	CGuiEditor* m_pAssetsEditor;
	int m_Member;
	bool m_NoEdit;
	
	virtual int GetValue() const
	{
		return m_pAssetsEditor->AssetsManager()->GetAssetValue<int>(
			m_pAssetsEditor->GetEditedAssetPath(),
			m_pAssetsEditor->GetEditedSubPath(),
			m_Member,
			0
		);
	}
	
	virtual void SetValue(int Value)
	{
		m_pAssetsEditor->AssetsManager()->SetAssetValue<int>(
			m_pAssetsEditor->GetEditedAssetPath(),
			m_pAssetsEditor->GetEditedSubPath(),
			m_Member,
			Value
		);
	}
	
public:
	CMemberIntegerEdit(CGuiEditor* pAssetsEditor, int Member, bool NoEdit = false) :
		gui::CAbstractIntegerEdit(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_Member(Member),
		m_NoEdit(NoEdit)
	{
		if(m_NoEdit)
			Editable(false);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(!m_NoEdit && IsEnabled() && ParentEnabled)
		{
			if(!AssetsManager()->IsValidPackage(m_pAssetsEditor->GetEditedPackageId()) || AssetsManager()->IsReadOnlyPackage(m_pAssetsEditor->GetEditedPackageId()))
				Editable(false);
			else
				Editable(true);
		}
		
		gui::CAbstractIntegerEdit::Update(ParentEnabled);
	}
};

void CAssetsInspector::AddField_Integer(gui::CVListLayout* pList, int Member, const CLocalizableString& Text)
{
	CMemberIntegerEdit* pWidget = new CMemberIntegerEdit(
		m_pAssetsEditor,
		Member
	);
	
	AddField(pList, pWidget, Text);
}

void CAssetsInspector::AddField_Integer_NoEdit(gui::CVListLayout* pList, int Member, const CLocalizableString& Text)
{
	CMemberIntegerEdit* pWidget = new CMemberIntegerEdit(
		m_pAssetsEditor,
		Member,
		true
	);
	
	AddField(pList, pWidget, Text);
}

/* BOOL EDIT **********************************************************/
	
class CMemberBoolEdit : public gui::CAbstractToggle
{
protected:
	CGuiEditor* m_pAssetsEditor;
	int m_Member;
	
	virtual bool GetValue()
	{
		return m_pAssetsEditor->AssetsManager()->GetAssetValue<bool>(
			m_pAssetsEditor->GetEditedAssetPath(),
			m_pAssetsEditor->GetEditedSubPath(),
			m_Member,
			0
		);
	}
	
	virtual void SetValue(bool Value)
	{
		m_pAssetsEditor->AssetsManager()->SetAssetValue<bool>(
			m_pAssetsEditor->GetEditedAssetPath(),
			m_pAssetsEditor->GetEditedSubPath(),
			m_Member,
			Value
		);
	}
	
public:
	CMemberBoolEdit(CGuiEditor* pAssetsEditor, int Member) :
		gui::CAbstractToggle(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_Member(Member)
	{ }
};

void CAssetsInspector::AddField_Bool(gui::CVListLayout* pList, int Member, const CLocalizableString& Text)
{
	CMemberBoolEdit* pWidget = new CMemberBoolEdit(
		m_pAssetsEditor,
		Member
	);
	
	AddField(pList, pWidget, Text);
}

/* FLOAT EDIT *********************************************************/
	
class CMemberFloatEdit : public gui::CAbstractFloatEdit
{
protected:
	CGuiEditor* m_pAssetsEditor;
	int m_Member;
	
	virtual float GetValue() const
	{
		return m_pAssetsEditor->AssetsManager()->GetAssetValue<float>(
			m_pAssetsEditor->GetEditedAssetPath(),
			m_pAssetsEditor->GetEditedSubPath(),
			m_Member,
			0
		);
	}
	
	virtual void SetValue(float Value)
	{
		m_pAssetsEditor->AssetsManager()->SetAssetValue<float>(
			m_pAssetsEditor->GetEditedAssetPath(),
			m_pAssetsEditor->GetEditedSubPath(),
			m_Member,
			Value
		);
	}
	
public:
	CMemberFloatEdit(CGuiEditor* pAssetsEditor, int Member) :
		gui::CAbstractFloatEdit(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_Member(Member)
	{ }
	
	virtual void Update(bool ParentEnabled)
	{
		if(IsEnabled() && ParentEnabled)
		{
			if(!AssetsManager()->IsValidPackage(m_pAssetsEditor->GetEditedPackageId()) || AssetsManager()->IsReadOnlyPackage(m_pAssetsEditor->GetEditedPackageId()))
				Editable(false);
			else
				Editable(true);
		}
		
		gui::CAbstractFloatEdit::Update(ParentEnabled);
	}
};

void CAssetsInspector::AddField_Float(gui::CVListLayout* pList, int Member, const CLocalizableString& Text)
{
	CMemberFloatEdit* pWidget = new CMemberFloatEdit(
		m_pAssetsEditor,
		Member
	);
	
	AddField(pList, pWidget, Text);
}

void CAssetsInspector::AddField_Vec2(gui::CVListLayout* pList, int Member, int Member2, const CLocalizableString& Text)
{
	gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
	
	pLayout->Add(new CMemberFloatEdit(
		m_pAssetsEditor,
		Member
	), true);
	pLayout->Add(new CMemberFloatEdit(
		m_pAssetsEditor,
		Member2
	), true);
	
	AddField(pList, pLayout, Text);
}

/* FLOAT EDIT *********************************************************/
	
class CMemberAngleEdit : public gui::CAbstractFloatEdit
{
protected:
	CGuiEditor* m_pAssetsEditor;
	int m_Member;
	
	virtual float GetValue() const
	{
		return 180.0f*m_pAssetsEditor->AssetsManager()->GetAssetValue<float>(
			m_pAssetsEditor->GetEditedAssetPath(),
			m_pAssetsEditor->GetEditedSubPath(),
			m_Member,
			0
		)/pi;
	}
	
	virtual void SetValue(float Value)
	{
		m_pAssetsEditor->AssetsManager()->SetAssetValue<float>(
			m_pAssetsEditor->GetEditedAssetPath(),
			m_pAssetsEditor->GetEditedSubPath(),
			m_Member,
			Value * pi/180.0f
		);
	}
	
public:
	CMemberAngleEdit(CGuiEditor* pAssetsEditor, int Member) :
		gui::CAbstractFloatEdit(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_Member(Member)
	{ }
};

void CAssetsInspector::AddField_Angle(gui::CVListLayout* pList, int Member, const CLocalizableString& Text)
{
	CMemberAngleEdit* pWidget = new CMemberAngleEdit(
		m_pAssetsEditor,
		Member
	);
	
	AddField(pList, pWidget, Text);
}

/* COLOR EDIT *********************************************************/
	
class CMemberColorEdit : public gui::CAbstractColorEdit
{
protected:
	CGuiEditor* m_pAssetsEditor;
	int m_Member;
	
	virtual vec4 GetValue() const
	{
		return m_pAssetsEditor->AssetsManager()->GetAssetValue<vec4>(
			m_pAssetsEditor->GetEditedAssetPath(),
			m_pAssetsEditor->GetEditedSubPath(),
			m_Member,
			vec4(1.0f, 1.0f, 1.0f, 1.0f)
		);
	}
	
	virtual void SetValue(vec4 Value)
	{
		m_pAssetsEditor->AssetsManager()->SetAssetValue<vec4>(
			m_pAssetsEditor->GetEditedAssetPath(),
			m_pAssetsEditor->GetEditedSubPath(),
			m_Member,
			Value
		);
	}
	
public:
	CMemberColorEdit(CGuiEditor* pAssetsEditor, int Member) :
		gui::CAbstractColorEdit(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_Member(Member)
	{ }
};

void CAssetsInspector::AddField_Color(gui::CVListLayout* pList, int Member, const CLocalizableString& Text)
{
	CMemberColorEdit* pWidget = new CMemberColorEdit(
		m_pAssetsEditor,
		Member
	);
	
	AddField(pList, pWidget, Text);
}

/* ASSET EDIT *********************************************************/

class CMemberAssetEdit : public gui::CButton
{
public:
	class CPopup : public gui::CPopup
	{
	public:
		class CItem : public gui::CButton
		{
		protected:
			CAssetPath m_AssetPath;
			CPopup* m_pPopup;
		
		protected:
			virtual void MouseClickAction()
			{
				m_pPopup->SetValue(m_AssetPath);
			}
			
		public:
			CItem(CPopup* pPopup, CAssetPath AssetPath, bool CheckTileCompatibility) :
				gui::CButton(pPopup->Context(), ""),
				m_pPopup(pPopup),
				m_AssetPath(AssetPath)
			{
				if(m_AssetPath.IsNull())
				{
					SetIcon(m_pPopup->m_pAssetsEditor->m_Path_Sprite_IconNone);
					SetText(_GUI("None"));
				}
				else
				{
					SetIcon(m_pPopup->m_pAssetsEditor->GetItemIcon(m_AssetPath, CSubPath::Null()));
					SetText(m_pPopup->m_pAssetsEditor->GetItemName(m_AssetPath, CSubPath::Null()));
				}
				
				if(CheckTileCompatibility)
				{
					const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(m_AssetPath);
					if(!pImage || !pImage->GetTilingEnabled())
						Editable(false);
				}
			}
			
			virtual void Update(bool ParentEnabled)
			{
				if(ParentEnabled)
				{
					if(m_pPopup->GetValue() == m_AssetPath)
						SetButtonStyle(m_pPopup->m_pAssetsEditor->m_Path_Button_ListItemHL);
					else
						SetButtonStyle(m_pPopup->m_pAssetsEditor->m_Path_Button_ListItem);
				}
				
				gui::CButton::Update(ParentEnabled);
			}
		};
	
	protected:
		CGuiEditor* m_pAssetsEditor;
		int m_Member;
		int m_AssetType;
		bool m_CheckTileCompatibility;
	
	public:
		CPopup(CGuiEditor* pAssetsEditor, int Member, int AssetType, gui::CRect ParentRect, int CheckTileCompatibility) :
			gui::CPopup(pAssetsEditor, ParentRect, 250, 400, gui::CPopup::ALIGNMENT_SIDE),
			m_pAssetsEditor(pAssetsEditor),
			m_Member(Member),
			m_AssetType(AssetType),
			m_CheckTileCompatibility(CheckTileCompatibility)
		{
			
			gui::CVScrollLayout* pLayout = new gui::CVScrollLayout(Context());
			Add(pLayout);
			
			SetBoxStyle(m_pAssetsEditor->m_Path_Box_Dialog);
			pLayout->Add(new CItem(this, CAssetPath::Null(), false), false);
			
			#define MACRO_ASSETTYPE(Name) case CAsset_##Name::TypeId:\
				{\
					gui::CExpand* pExpand = new gui::CExpand(Context());\
					pLayout->Add(pExpand);\
					pExpand->SetTitle(new gui::CLabel(Context(), AssetsManager()->GetPackageName(m_pAssetsEditor->GetEditedPackageId()), m_pAssetsEditor->m_Path_Sprite_IconFolder));\
					for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_##Name>(m_pAssetsEditor->GetEditedPackageId()); i++)\
						pExpand->Add(new CItem(this, CAssetPath(CAsset_##Name::TypeId, m_pAssetsEditor->GetEditedPackageId(), i), m_CheckTileCompatibility));\
				}\
				for(int p=0; p<AssetsManager()->GetNumPackages(); p++)\
				{\
					if(p != m_pAssetsEditor->GetEditedPackageId())\
					{\
						gui::CExpand* pExpand = new gui::CExpand(Context());\
						pLayout->Add(pExpand);\
						pExpand->SetTitle(new gui::CLabel(Context(), AssetsManager()->GetPackageName(p), m_pAssetsEditor->m_Path_Sprite_IconFolder));\
						for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_##Name>(p); i++)\
							pExpand->Add(new CItem(this, CAssetPath(CAsset_##Name::TypeId, p, i), m_CheckTileCompatibility));\
					}\
				}\
				break;
			
			switch(m_AssetType)
			{
				#include <generated/assets/assetsmacro.h>
			}
			
			#undef TU_MACRO_ASSETTYPE
		}
		
		CAssetPath GetValue()
		{
			return m_pAssetsEditor->AssetsManager()->GetAssetValue<CAssetPath>(
				m_pAssetsEditor->GetEditedAssetPath(),
				m_pAssetsEditor->GetEditedSubPath(),
				m_Member,
				CAssetPath::Null()
			);
		}
		
		void SetValue(CAssetPath Value)
		{
			m_pAssetsEditor->AssetsManager()->SetAssetValue<CAssetPath>(
				m_pAssetsEditor->GetEditedAssetPath(),
				m_pAssetsEditor->GetEditedSubPath(),
				m_Member,
				Value
			);
		}
	
		virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
	};

protected:
	CGuiEditor* m_pAssetsEditor;
	int m_Member;
	int m_AssetType;
	bool m_CheckTileCompatibility;
	
protected:
	virtual void MouseClickAction()
	{
		Context()->DisplayPopup(new CPopup(m_pAssetsEditor, m_Member, m_AssetType, m_DrawRect, m_CheckTileCompatibility));
	}

public:
	CMemberAssetEdit(CGuiEditor* pAssetsEditor, int Member, int AssetType) :
		gui::CButton(pAssetsEditor, "", CAssetPath::Null()),
		m_pAssetsEditor(pAssetsEditor),
		m_Member(Member),
		m_AssetType(AssetType),
		m_CheckTileCompatibility(false)
	{
		
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(IsEnabled() && ParentEnabled)
		{
			if(!AssetsManager()->IsValidPackage(m_pAssetsEditor->GetEditedPackageId()) || AssetsManager()->IsReadOnlyPackage(m_pAssetsEditor->GetEditedPackageId()))
				Editable(false);
			else
				Editable(true);
				
			CAssetPath Value = m_pAssetsEditor->AssetsManager()->GetAssetValue<CAssetPath>(
				m_pAssetsEditor->GetEditedAssetPath(),
				m_pAssetsEditor->GetEditedSubPath(),
				m_Member,
				CAssetPath::Null()
			);
			
			if(Value.IsNull())
			{
				SetIcon(m_pAssetsEditor->m_Path_Sprite_IconNone);
				SetText(_GUI("None"));
			}
			else
			{
				SetIcon(m_pAssetsEditor->GetItemIcon(Value, CSubPath::Null()));
				SetText(m_pAssetsEditor->GetItemName(Value, CSubPath::Null()));
			}
		}
		
		gui::CButton::Update(ParentEnabled);
	}
	
	void CheckTileCompatibility()
	{
		m_CheckTileCompatibility = true;
	}
};

void CAssetsInspector::AddField_Asset(gui::CVListLayout* pList, int Member, int AssetType, const CLocalizableString& Text)
{
	CMemberAssetEdit* pWidget = new CMemberAssetEdit(
		m_pAssetsEditor,
		Member,
		AssetType
	);
	
	AddField(pList, pWidget, Text);
}

void CAssetsInspector::AddField_ImageTiles(gui::CVListLayout* pList, int Member, int AssetType, const CLocalizableString& Text)
{
	CMemberAssetEdit* pWidget = new CMemberAssetEdit(
		m_pAssetsEditor,
		Member,
		AssetType
	);
	pWidget->CheckTileCompatibility();
	
	AddField(pList, pWidget, Text);
}
