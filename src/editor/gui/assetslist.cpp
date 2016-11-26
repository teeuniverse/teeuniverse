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

#include <editor/gui/assetslist.h>
#include <editor/components/gui.h>
#include <shared/components/assetsmanager.h>
#include <client/gui/expand.h>
#include <client/gui/popup.h>
#include <client/gui/toggle.h>
#include <client/gui/text-edit.h>
#include <shared/components/localization.h>
#include <shared/components/storage.h>

/* PACKAGE ITEM *******************************************************/

class CPackageItem : public gui::CButton
{
protected:
	class CCloseButton : public gui::CButton
	{
	protected:
		CGuiEditor* m_pAssetsEditor;
		CContextMenu* m_pContextMenu;
		int m_PackageId;
		
	protected:
		virtual void MouseClickAction()
		{
			AssetsManager()->ClosePackage(m_PackageId);
			m_pContextMenu->Close();
			m_pAssetsEditor->RefreshPackageTree();
		}

	public:
		CCloseButton(CGuiEditor* pAssetsEditor, CContextMenu* pContextMenu, int PackageId) :
			gui::CButton(pAssetsEditor, _GUI("Close"), pAssetsEditor->m_Path_Sprite_IconDelete),
			m_pAssetsEditor(pAssetsEditor),
			m_pContextMenu(pContextMenu),
			m_PackageId(PackageId)
		{
			SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
		}
	};
	
	class CRealOnlyToggle : public gui::CToggle
	{
	protected:
		CGuiEditor* m_pAssetsEditor;
		CContextMenu* m_pContextMenu;
		int m_PackageId;
		
	protected:
		virtual bool GetValue()
		{
			return !AssetsManager()->IsValidPackage(m_PackageId) || AssetsManager()->IsReadOnlyPackage(m_PackageId);
		}
		
		virtual void SetValue(bool Value)
		{
			if(AssetsManager()->IsValidPackage(m_PackageId))
			{
				AssetsManager()->SetPackageReadOnly(m_PackageId, Value);
				m_pAssetsEditor->RefreshPackageTree();
			}
		}

	public:
		CRealOnlyToggle(CGuiEditor* pAssetsEditor, CContextMenu* pContextMenu, int PackageId) :
			gui::CToggle(pAssetsEditor, _GUI("Read-Only")),
			m_pAssetsEditor(pAssetsEditor),
			m_pContextMenu(pContextMenu),
			m_PackageId(PackageId)
		{
			
		}
	};

protected:
	CGuiEditor* m_pAssetsEditor;
	int m_PackageId;
	
protected:
	virtual void MouseClickAction()
	{
		Action();
	}
	
	void Action()
	{
		m_pAssetsEditor->SetEditedPackage(m_PackageId);
		m_pAssetsEditor->RefreshPackageTree();
		m_pAssetsEditor->RefreshAssetsTree();
	}

public:
	CPackageItem(CGuiEditor* pAssetsEditor, int PackageId) :
		gui::CButton(pAssetsEditor, "", CAssetPath::Null()),
		m_pAssetsEditor(pAssetsEditor),
		m_PackageId(PackageId)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItem);
		SetIcon(m_pAssetsEditor->m_Path_Sprite_IconFolder);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		SetText(AssetsManager()->GetPackageName(m_PackageId));
		
		if(m_pAssetsEditor->GetEditedPackageId() == m_PackageId)
			SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItemHL);
		else
			SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItem);
		
		gui::CButton::Update(ParentEnabled);
	}
	
	virtual void OnButtonClick(int Button)
	{
		if(Button == KEY_MOUSE_2)
		{
			if(m_DrawRect.IsInside(Context()->GetMousePos()))
			{
				CContextMenu* pMenu = new CContextMenu(m_pAssetsEditor);
			
				pMenu->List()->Add(new CRealOnlyToggle(m_pAssetsEditor, pMenu, m_PackageId));
				pMenu->List()->AddSeparator();
				pMenu->List()->Add(new CCloseButton(m_pAssetsEditor, pMenu, m_PackageId));
				
				Action();
				m_pAssetsEditor->DisplayPopup(pMenu);
				return;
			}
		}
		
		gui::CButton::OnButtonClick(Button);
	}
};

/* INACTIVE ITEM ******************************************************/

class CInactiveText : public gui::CLabel
{
public:
	CInactiveText(CGuiEditor* pAssetsEditor, const CLocalizableString& Text, const CAssetPath& IconPath) :
		gui::CLabel(pAssetsEditor, Text, IconPath)
	{
		SetLabelStyle(pAssetsEditor->m_Path_Label_InactiveListItem);
	}
};

class CInactiveItem : public gui::CLabel
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_AssetPath;
	CSubPath m_SubPath;

public:
	CInactiveItem(CGuiEditor* pAssetsEditor, CAssetPath AssetPath, CSubPath SubPath) :
		gui::CLabel(pAssetsEditor, "", CAssetPath::Null()),
		m_pAssetsEditor(pAssetsEditor),
		m_AssetPath(AssetPath),
		m_SubPath(SubPath)
	{
		SetLabelStyle(m_pAssetsEditor->m_Path_Label_InactiveListItem);
		SetIcon(m_pAssetsEditor->GetItemIcon(m_AssetPath, m_SubPath));
	}
	
	virtual void Update(bool ParentEnabled)
	{
		SetText(m_pAssetsEditor->GetItemName(m_AssetPath, m_SubPath));
		
		gui::CLabel::Update(ParentEnabled);
	}
};

/* ITEM ***************************************************************/

class CItem : public gui::CHListLayout
{
protected:
	class CMenuButton : public gui::CButton
	{
	protected:
		CGuiEditor* m_pAssetsEditor;
		CContextMenu* m_pContextMenu;
		CAssetPath m_AssetPath;

	public:
		CMenuButton(CGuiEditor* pAssetsEditor, CContextMenu* pContextMenu, const CAssetPath& AssetPath, const CLocalizableString& Name, const CAssetPath& IconPath) :
			gui::CButton(pAssetsEditor, Name, IconPath),
			m_pAssetsEditor(pAssetsEditor),
			m_pContextMenu(pContextMenu),
			m_AssetPath(AssetPath)
		{
			SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
		}
	};
	
	class CDeleteButton : public CMenuButton
	{
	protected:
		virtual void MouseClickAction()
		{
			array<CAssetPath> AssetsToDelete;
			
			AssetsToDelete.increment() = m_AssetPath;
			
			if(m_AssetPath.GetType() == CAsset_Map::TypeId)
			{
				const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(m_AssetPath);
				if(pMap)
				{
					{
						CAsset_Map::CIteratorZoneLayer Iter;
						for(Iter = pMap->BeginZoneLayer(); Iter != pMap->EndZoneLayer(); ++Iter)
							AssetsToDelete.increment() = pMap->GetZoneLayer(*Iter);
					}
					{
						CAsset_Map::CIteratorEntityLayer Iter;
						for(Iter = pMap->BeginEntityLayer(); Iter != pMap->EndEntityLayer(); ++Iter)
							AssetsToDelete.increment() = pMap->GetEntityLayer(*Iter);
					}
					{
						CAsset_Map::CIteratorBgGroup Iter;
						for(Iter = pMap->BeginBgGroup(); Iter != pMap->EndBgGroup(); ++Iter)
						{
							AssetsToDelete.increment() = pMap->GetBgGroup(*Iter);
							
							const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(pMap->GetBgGroup(*Iter));
							if(pGroup)
							{
								CAsset_MapGroup::CIteratorLayer GroupIter;
								for(GroupIter = pGroup->BeginLayer(); GroupIter != pGroup->EndLayer(); ++GroupIter)
								{
									AssetsToDelete.increment() = pGroup->GetLayer(*GroupIter);
								}
							}
						}
					}
					{
						CAsset_Map::CIteratorFgGroup Iter;
						for(Iter = pMap->BeginFgGroup(); Iter != pMap->EndFgGroup(); ++Iter)
						{
							AssetsToDelete.increment() = pMap->GetFgGroup(*Iter);
							
							const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(pMap->GetFgGroup(*Iter));
							if(pGroup)
							{
								CAsset_MapGroup::CIteratorLayer GroupIter;
								for(GroupIter = pGroup->BeginLayer(); GroupIter != pGroup->EndLayer(); ++GroupIter)
								{
									AssetsToDelete.increment() = pGroup->GetLayer(*GroupIter);
								}
							}
						}
					}
				}
			}
			else if(m_AssetPath.GetType() == CAsset_MapGroup::TypeId)
			{
				const CAsset_MapGroup* pGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(m_AssetPath);
				if(pGroup)
				{
					CAsset_MapGroup::CIteratorLayer Iter;
					for(Iter = pGroup->BeginLayer(); Iter != pGroup->EndLayer(); ++Iter)
						AssetsToDelete.increment() = pGroup->GetLayer(*Iter);
				}
			}
			
			AssetsManager()->DeleteAssets(AssetsToDelete);
			
			m_pAssetsEditor->SetEditedAsset(CAssetPath::Null(), CSubPath::Null());
			m_pAssetsEditor->RefreshAssetsTree();
			m_pContextMenu->Close();
		}

	public:
		CDeleteButton(CGuiEditor* pAssetsEditor, CContextMenu* pContextMenu, const CAssetPath& AssetPath) :
			CMenuButton(pAssetsEditor, pContextMenu, AssetPath, _GUI("Delete"), pAssetsEditor->m_Path_Sprite_IconDelete)
		{ }
	};
	
	class CAddSpriteButton : public CMenuButton
	{
	protected:
		virtual void MouseClickAction()
		{
			CAssetPath SpritePath;
			int Token = AssetsManager()->GenerateToken();
			
			const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(m_AssetPath);
			if(pImage)
			{
				CAsset_Sprite* pSprite = AssetsManager()->NewAsset<CAsset_Sprite>(&SpritePath, m_AssetPath.GetPackageId(), Token);
				if(pSprite)
				{
					AssetsManager()->TryChangeAssetName(SpritePath, "sprite", Token);
					pSprite->SetImagePath(m_AssetPath);
					pSprite->SetX(0);
					pSprite->SetY(0);
					pSprite->SetWidth(1);
					pSprite->SetHeight(1);
					
					m_pAssetsEditor->RefreshAssetsTree();
				}
			}
			
			m_pContextMenu->Close();
		}

	public:
		CAddSpriteButton(CGuiEditor* pAssetsEditor, CContextMenu* pContextMenu, const CAssetPath& AssetPath) :
			CMenuButton(pAssetsEditor, pContextMenu, AssetPath, _GUI("Add Sprite"), pAssetsEditor->m_Path_Sprite_IconSprite)
		{ }
	};
	
	class CAddZoneLayerButton : public CMenuButton
	{
	protected:
		virtual void MouseClickAction()
		{
			CAssetPath ZoneLayerPath;
			int Token = AssetsManager()->GenerateToken();
			
			const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(m_AssetPath);
			if(pMap)
			{
				CAsset_MapZoneTiles* pLayer = AssetsManager()->NewAsset<CAsset_MapZoneTiles>(&ZoneLayerPath, m_AssetPath.GetPackageId(), Token);
				if(pLayer)
				{
					AssetsManager()->TryChangeAssetName(ZoneLayerPath, "zone", Token);
					pLayer->SetTileWidth(64);
					pLayer->SetTileHeight(64);
					
					int Id = AssetsManager()->AddSubItem(m_AssetPath, CSubPath::Null(), CAsset_Map::TYPE_ZONELAYER);
					if(Id >= 0)
					{
						CSubPath SubPath = CAsset_Map::SubPath_ZoneLayer(Id);
						AssetsManager()->SetAssetValue<CAssetPath>(m_AssetPath, SubPath, CAsset_Map::ZONELAYER, ZoneLayerPath, Token);
					}
					m_pAssetsEditor->RefreshAssetsTree();
				}
			}
			
			m_pContextMenu->Close();
		}

	public:
		CAddZoneLayerButton(CGuiEditor* pAssetsEditor, CContextMenu* pContextMenu, const CAssetPath& AssetPath) :
			CMenuButton(pAssetsEditor, pContextMenu, AssetPath, _GUI("Add Zone Layer"), pAssetsEditor->m_Path_Sprite_IconZoneTiles)
		{ }
	};
	
	class CAddEntityLayerButton : public CMenuButton
	{
	protected:
		virtual void MouseClickAction()
		{
			CAssetPath EntityLayerPath;
			int Token = AssetsManager()->GenerateToken();
			
			const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(m_AssetPath);
			if(pMap)
			{
				CAsset_MapEntities* pLayer = AssetsManager()->NewAsset<CAsset_MapEntities>(&EntityLayerPath, m_AssetPath.GetPackageId(), Token);
				if(pLayer)
				{
					AssetsManager()->TryChangeAssetName(EntityLayerPath, "entities", Token);
					
					int Id = AssetsManager()->AddSubItem(m_AssetPath, CSubPath::Null(), CAsset_Map::TYPE_ENTITYLAYER);
					if(Id >= 0)
					{
						CSubPath SubPath = CAsset_Map::SubPath_EntityLayer(Id);
						AssetsManager()->SetAssetValue<CAssetPath>(m_AssetPath, SubPath, CAsset_Map::ENTITYLAYER, EntityLayerPath, Token);
					}
					m_pAssetsEditor->RefreshAssetsTree();
				}
			}
			
			m_pContextMenu->Close();
		}

	public:
		CAddEntityLayerButton(CGuiEditor* pAssetsEditor, CContextMenu* pContextMenu, const CAssetPath& AssetPath) :
			CMenuButton(pAssetsEditor, pContextMenu, AssetPath, _GUI("Add Entities"), pAssetsEditor->m_Path_Sprite_IconEntities)
		{ }
	};
	
	class CAddBgGroupButton : public CMenuButton
	{
	protected:
		virtual void MouseClickAction()
		{
			CAssetPath GroupPath;
			int Token = AssetsManager()->GenerateToken();
			
			const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(m_AssetPath);
			if(pMap)
			{
				CAsset_MapGroup* pLayer = AssetsManager()->NewAsset<CAsset_MapGroup>(&GroupPath, m_AssetPath.GetPackageId(), Token);
				if(pLayer)
				{
					AssetsManager()->TryChangeAssetName(GroupPath, "group", Token);
					
					int Id = AssetsManager()->AddSubItem(m_AssetPath, CSubPath::Null(), CAsset_Map::TYPE_BGGROUP);
					if(Id >= 0)
					{
						CSubPath SubPath = CAsset_Map::SubPath_BgGroup(Id);
						AssetsManager()->SetAssetValue<CAssetPath>(m_AssetPath, SubPath, CAsset_Map::BGGROUP, GroupPath, Token);
					}
					m_pAssetsEditor->RefreshAssetsTree();
				}
			}
			
			m_pContextMenu->Close();
		}

	public:
		CAddBgGroupButton(CGuiEditor* pAssetsEditor, CContextMenu* pContextMenu, const CAssetPath& AssetPath) :
			CMenuButton(pAssetsEditor, pContextMenu, AssetPath, _GUI("Add Background Group"), pAssetsEditor->m_Path_Sprite_IconFolder)
		{ }
	};
	
	
	class CAddFgGroupButton : public CMenuButton
	{
	protected:
		virtual void MouseClickAction()
		{
			CAssetPath GroupPath;
			int Token = AssetsManager()->GenerateToken();
			
			const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(m_AssetPath);
			if(pMap)
			{
				CAsset_MapGroup* pLayer = AssetsManager()->NewAsset<CAsset_MapGroup>(&GroupPath, m_AssetPath.GetPackageId(), Token);
				if(pLayer)
				{
					AssetsManager()->TryChangeAssetName(GroupPath, "group", Token);
					
					int Id = AssetsManager()->AddSubItem(m_AssetPath, CSubPath::Null(), CAsset_Map::TYPE_FGGROUP);
					if(Id >= 0)
					{
						CSubPath SubPath = CAsset_Map::SubPath_FgGroup(Id);
						AssetsManager()->SetAssetValue<CAssetPath>(m_AssetPath, SubPath, CAsset_Map::FGGROUP, GroupPath, Token);
					}
					m_pAssetsEditor->RefreshAssetsTree();
				}
			}
			
			m_pContextMenu->Close();
		}

	public:
		CAddFgGroupButton(CGuiEditor* pAssetsEditor, CContextMenu* pContextMenu, const CAssetPath& AssetPath) :
			CMenuButton(pAssetsEditor, pContextMenu, AssetPath, _GUI("Add Foreground Group"), pAssetsEditor->m_Path_Sprite_IconFolder)
		{ }
	};
	
	class CAddTileLayerButton : public CMenuButton
	{
	protected:
		virtual void MouseClickAction()
		{
			CAssetPath LayerPath;
			int Token = AssetsManager()->GenerateToken();
			
			const CAsset_MapGroup* pMapGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(m_AssetPath);
			if(pMapGroup)
			{
				CAsset_MapLayerTiles* pLayer = AssetsManager()->NewAsset<CAsset_MapLayerTiles>(&LayerPath, m_AssetPath.GetPackageId(), Token);
				if(pLayer)
				{
					AssetsManager()->TryChangeAssetName(LayerPath, "tiles", Token);
					pLayer->SetTileWidth(64);
					pLayer->SetTileHeight(64);
					
					int Id = AssetsManager()->AddSubItem(m_AssetPath, CSubPath::Null(), CAsset_MapGroup::TYPE_LAYER);
					if(Id >= 0)
					{
						CSubPath SubPath = CAsset_MapGroup::SubPath_Layer(Id);
						AssetsManager()->SetAssetValue<CAssetPath>(m_AssetPath, SubPath, CAsset_MapGroup::LAYER, LayerPath, Token);
					}
					m_pAssetsEditor->RefreshAssetsTree();
				}
			}
			
			m_pContextMenu->Close();
		}

	public:
		CAddTileLayerButton(CGuiEditor* pAssetsEditor, CContextMenu* pContextMenu, const CAssetPath& AssetPath) :
			CMenuButton(pAssetsEditor, pContextMenu, AssetPath, _GUI("Add Tile Layer"), pAssetsEditor->m_Path_Sprite_IconTiles)
		{ }
	};
	
	class CAddQuadLayerButton : public CMenuButton
	{
	protected:
		virtual void MouseClickAction()
		{
			CAssetPath LayerPath;
			int Token = AssetsManager()->GenerateToken();
			
			const CAsset_MapGroup* pMapGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(m_AssetPath);
			if(pMapGroup)
			{
				CAsset_MapLayerQuads* pLayer = AssetsManager()->NewAsset<CAsset_MapLayerQuads>(&LayerPath, m_AssetPath.GetPackageId(), Token);
				if(pLayer)
				{
					AssetsManager()->TryChangeAssetName(LayerPath, "quads", Token);
					
					int Id = AssetsManager()->AddSubItem(m_AssetPath, CSubPath::Null(), CAsset_MapGroup::TYPE_LAYER);
					if(Id >= 0)
					{
						CSubPath SubPath = CAsset_MapGroup::SubPath_Layer(Id);
						AssetsManager()->SetAssetValue<CAssetPath>(m_AssetPath, SubPath, CAsset_MapGroup::LAYER, LayerPath, Token);
					}
					m_pAssetsEditor->RefreshAssetsTree();
				}
			}
			
			m_pContextMenu->Close();
		}

	public:
		CAddQuadLayerButton(CGuiEditor* pAssetsEditor, CContextMenu* pContextMenu, const CAssetPath& AssetPath) :
			CMenuButton(pAssetsEditor, pContextMenu, AssetPath, _GUI("Add Quad Layer"), pAssetsEditor->m_Path_Sprite_IconQuad)
		{ }
	};
	
	class CViewButton : public gui::CButton
	{
	protected:
		CGuiEditor* m_pAssetsEditor;
		CAssetPath m_AssetPath;
	
	protected:
		virtual void MouseClickAction()
		{
			CAssetState* pState = AssetsManager()->GetAssetState(m_AssetPath);
			if(pState)
				pState->m_Visible = !pState->m_Visible;
		}

	public:
		CViewButton(CGuiEditor* pAssetsEditor, const CAssetPath& AssetPath) :
			gui::CButton(pAssetsEditor, "", pAssetsEditor->m_Path_Sprite_IconView),
			m_pAssetsEditor(pAssetsEditor),
			m_AssetPath(AssetPath)
		{
			
		}
	
		virtual void Update(bool ParentEnabled)
		{
			CAssetState* pState = AssetsManager()->GetAssetState(m_AssetPath);
			if(pState && pState->m_Visible)
				SetIcon(m_pAssetsEditor->m_Path_Sprite_IconView);
			else
				SetIcon(m_pAssetsEditor->m_Path_Sprite_IconHidden);
			
			gui::CButton::Update(ParentEnabled);
		}
	};
	
	class CTitleButton : public gui::CButton
	{
	protected:
		CGuiEditor* m_pAssetsEditor;
		CAssetPath m_AssetPath;
		CSubPath m_SubPath;
	
	protected:
		virtual void MouseClickAction() { Action(); }

	public:
		CTitleButton(CGuiEditor* pAssetsEditor, const CAssetPath& AssetPath, CSubPath SubPath) :
			gui::CButton(pAssetsEditor, ""),
			m_pAssetsEditor(pAssetsEditor),
			m_AssetPath(AssetPath),
			m_SubPath(SubPath)
		{
			
		}
	
		virtual void Update(bool ParentEnabled)
		{
			SetIcon(m_pAssetsEditor->GetItemIcon(m_AssetPath, m_SubPath));
			SetText(m_pAssetsEditor->GetItemName(m_AssetPath, m_SubPath));
			
			if(m_pAssetsEditor->GetEditedAssetPath() == m_AssetPath)
				SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItemHL);
			else
				SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItem);
			
			gui::CButton::Update(ParentEnabled);
		}
	
		virtual void Action()
		{
			m_pAssetsEditor->SetEditedAsset(m_AssetPath, m_SubPath);
		}
	};
	
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_AssetPath;
	CSubPath m_SubPath;
	CTitleButton* m_pTitle;

public:
	CItem(CGuiEditor* pAssetsEditor, CAssetPath AssetPath, CSubPath SubPath) :
		gui::CHListLayout(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_AssetPath(AssetPath),
		m_SubPath(SubPath),
		m_pTitle(NULL)
	{
		CAssetState* pState = AssetsManager()->GetAssetState(m_AssetPath);
		if(pState)
			pState->m_ListedInEditor = true;
		
		m_pTitle = new CTitleButton(m_pAssetsEditor, AssetPath, SubPath);
		
		Add(m_pTitle, true);
		
		switch(m_AssetPath.GetType())
		{
			case CAsset_Map::TypeId:
			case CAsset_MapGroup::TypeId:
			case CAsset_MapLayerTiles::TypeId:
			case CAsset_MapLayerQuads::TypeId:
			case CAsset_MapZoneTiles::TypeId:
			case CAsset_MapEntities::TypeId:
				Add(new CViewButton(m_pAssetsEditor, AssetPath), false);
				break;
		}
	}
	
	virtual void OnButtonClick(int Button)
	{
		if(Button == KEY_MOUSE_2 && AssetsManager()->IsValidPackage(m_AssetPath.GetPackageId()) && !AssetsManager()->IsReadOnlyPackage(m_AssetPath.GetPackageId()))
		{
			if(m_DrawRect.IsInside(Context()->GetMousePos()))
			{
				CContextMenu* pMenu = new CContextMenu(m_pAssetsEditor);
				
				if(m_AssetPath.GetType() == CAsset_Image::TypeId)
				{
					pMenu->List()->Add(new CAddSpriteButton(m_pAssetsEditor, pMenu, m_AssetPath));
					pMenu->List()->AddSeparator();
				}
				else if(m_AssetPath.GetType() == CAsset_Map::TypeId)
				{
					pMenu->List()->Add(new CAddZoneLayerButton(m_pAssetsEditor, pMenu, m_AssetPath));
					pMenu->List()->Add(new CAddEntityLayerButton(m_pAssetsEditor, pMenu, m_AssetPath));
					pMenu->List()->Add(new CAddBgGroupButton(m_pAssetsEditor, pMenu, m_AssetPath));
					pMenu->List()->Add(new CAddFgGroupButton(m_pAssetsEditor, pMenu, m_AssetPath));
					pMenu->List()->AddSeparator();
				}
				else if(m_AssetPath.GetType() == CAsset_MapGroup::TypeId)
				{
					pMenu->List()->Add(new CAddTileLayerButton(m_pAssetsEditor, pMenu, m_AssetPath));
					pMenu->List()->Add(new CAddQuadLayerButton(m_pAssetsEditor, pMenu, m_AssetPath));
					pMenu->List()->AddSeparator();
				}
				
				pMenu->List()->Add(new CDeleteButton(m_pAssetsEditor, pMenu, m_AssetPath));
				
				m_pAssetsEditor->DisplayPopup(pMenu);
				m_pTitle->Action();
				return;
			}
		}
		
		gui::CHListLayout::OnButtonClick(Button);
	}
};

/* IMAGE ITEM *********************************************************/

class CImageItem : public gui::CExpand
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_AssetPath;
	int m_PackageId;
	bool m_PackageIdFound;

public:
	CImageItem(CGuiEditor* pAssetsEditor, CAssetPath AssetPath, int PackageId) :
		gui::CExpand(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_PackageId(PackageId),
		m_AssetPath(AssetPath),
		m_PackageIdFound(false)
	{
		if(m_PackageId == m_AssetPath.GetPackageId())
			SetTitle(new CItem(m_pAssetsEditor, m_AssetPath, CSubPath::Null()));
		else
			SetTitle(new CInactiveItem(m_pAssetsEditor, m_AssetPath, CSubPath::Null()));
		
		for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_Sprite>(m_PackageId); i++)
		{
			CAssetPath SpritePath = CAssetPath(CAsset_Sprite::TypeId, m_PackageId, i);
			CAssetPath ImagePath = AssetsManager()->GetAssetValue<CAssetPath>(SpritePath, CSubPath::Null(), CAsset_Sprite::IMAGEPATH, CAssetPath::Null());
			if(ImagePath == m_AssetPath)
			{
				Add(new CItem(m_pAssetsEditor, SpritePath, CSubPath::Null()));
				m_PackageIdFound = true;
			}
		}
	}
	
	bool PackageIdFound()
	{
		return m_PackageIdFound || (m_AssetPath.GetPackageId() == m_PackageId);
	}
};

/* SKELETON ITEM ******************************************************/

class CSkeletonItem : public gui::CExpand
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_AssetPath;
	int m_PackageId;
	bool m_PackageIdFound;

public:
	CSkeletonItem(CGuiEditor* pAssetsEditor, CAssetPath AssetPath, int PackageId) :
		gui::CExpand(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_PackageId(PackageId),
		m_AssetPath(AssetPath),
		m_PackageIdFound(false)
	{
		if(m_PackageId == m_AssetPath.GetPackageId())
			SetTitle(new CItem(m_pAssetsEditor, m_AssetPath, CSubPath::Null()));
		else
			SetTitle(new CInactiveItem(m_pAssetsEditor, m_AssetPath, CSubPath::Null()));
		
		Clear();
		
		for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_SkeletonAnimation>(m_PackageId); i++)
		{
			CAssetPath Path = CAssetPath(CAsset_SkeletonAnimation::TypeId, m_PackageId, i);
			CAssetPath SkeletonPath = AssetsManager()->GetAssetValue<CAssetPath>(Path, CSubPath::Null(), CAsset_SkeletonAnimation::SKELETONPATH, CAssetPath::Null());
			if(SkeletonPath == m_AssetPath)
			{
				Add(new CItem(m_pAssetsEditor, Path, CSubPath::Null()));
				m_PackageIdFound = true;
			}
		}
		for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_SkeletonSkin>(m_PackageId); i++)
		{
			CAssetPath Path = CAssetPath(CAsset_SkeletonSkin::TypeId, m_PackageId, i);
			CAssetPath SkeletonPath = AssetsManager()->GetAssetValue<CAssetPath>(Path, CSubPath::Null(), CAsset_SkeletonSkin::SKELETONPATH, CAssetPath::Null());
			if(SkeletonPath == m_AssetPath)
			{
				Add(new CItem(m_pAssetsEditor, Path, CSubPath::Null()));
				m_PackageIdFound = true;
			}
		}
	}
	
	bool PackageIdFound()
	{
		return m_PackageIdFound || (m_AssetPath.GetPackageId() == m_PackageId);
	}
};

/* CHARACTER ITEM *****************************************************/

class CCharacterPartItem : public gui::CExpand
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_AssetPath;
	CSubPath m_SubPath;
	int m_PackageId;
	bool m_PackageIdFound;

public:
	CCharacterPartItem(CGuiEditor* pAssetsEditor, CAssetPath AssetPath, CSubPath SubPath, int PackageId) :
		gui::CExpand(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_PackageId(PackageId),
		m_AssetPath(AssetPath),
		m_SubPath(SubPath),
		m_PackageIdFound(false)
	{
		if(m_PackageId == m_AssetPath.GetPackageId())
			SetTitle(new CItem(m_pAssetsEditor, m_AssetPath, m_SubPath));
		else
			SetTitle(new CInactiveItem(m_pAssetsEditor, m_AssetPath, m_SubPath));
		
		Clear();
		
		for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_SkeletonSkin>(m_PackageId); i++)
		{
			CAssetPath Path = CAssetPath(CAsset_CharacterPart::TypeId, m_PackageId, i);
			CAssetPath CharacterPath = AssetsManager()->GetAssetValue<CAssetPath>(Path, CSubPath::Null(), CAsset_CharacterPart::CHARACTERPATH, CAssetPath::Null());
			CSubPath CharacterPart = AssetsManager()->GetAssetValue<CSubPath>(Path, CSubPath::Null(), CAsset_CharacterPart::CHARACTERPART, CSubPath::Null());
			if(CharacterPath == m_AssetPath && CharacterPart == m_SubPath)
			{
				Add(new CItem(m_pAssetsEditor, Path, CSubPath::Null()));
				m_PackageIdFound = true;
			}
		}
	}
	
	bool PackageIdFound()
	{
		return m_PackageIdFound || (m_AssetPath.GetPackageId() == m_PackageId);
	}
};

class CCharacterItem : public gui::CExpand
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_AssetPath;
	int m_PackageId;
	bool m_PackageIdFound;

public:
	CCharacterItem(CGuiEditor* pAssetsEditor, CAssetPath AssetPath, int PackageId) :
		gui::CExpand(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_PackageId(PackageId),
		m_AssetPath(AssetPath),
		m_PackageIdFound(false)
	{
		if(m_PackageId == m_AssetPath.GetPackageId())
			SetTitle(new CItem(m_pAssetsEditor, m_AssetPath, CSubPath::Null()));
		else
			SetTitle(new CInactiveItem(m_pAssetsEditor, m_AssetPath, CSubPath::Null()));
		
		Clear();
		
		const CAsset_Character* pCharacter = AssetsManager()->GetAsset<CAsset_Character>(m_AssetPath);
		if(pCharacter)
		{
			CAsset_Character::CIteratorPart Iter;
			for(Iter = pCharacter->BeginPart(); Iter != pCharacter->EndPart(); ++Iter)
			{
				CCharacterPartItem* pItem = new CCharacterPartItem(m_pAssetsEditor, m_AssetPath, *Iter, m_PackageId);
				
				if(pItem->PackageIdFound())
				{
					Add(pItem);
					m_PackageIdFound = true;
				}
				else
					delete pItem;
			}
		}
		for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_Weapon>(m_PackageId); i++)
		{
			CAssetPath Path = CAssetPath(CAsset_Weapon::TypeId, m_PackageId, i);
			CAssetPath CharacterPath = AssetsManager()->GetAssetValue<CAssetPath>(Path, CSubPath::Null(), CAsset_Weapon::CHARACTERPATH, CAssetPath::Null());
			if(CharacterPath == m_AssetPath)
			{
				Add(new CItem(m_pAssetsEditor, Path, CSubPath::Null()));
				m_PackageIdFound = true;
			}
		}
	}
	
	bool PackageIdFound()
	{
		return m_PackageIdFound || (m_AssetPath.GetPackageId() == m_PackageId);
	}
};

/* MAPGROUP ITEM ******************************************************/

class CMapGroupItem : public gui::CExpand
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_AssetPath;
	int m_PackageId;
	bool m_PackageIdFound;

public:
	CMapGroupItem(CGuiEditor* pAssetsEditor, CAssetPath AssetPath, int PackageId) :
		gui::CExpand(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_PackageId(PackageId),
		m_AssetPath(AssetPath),
		m_PackageIdFound(false)
	{
		if(m_PackageId == m_AssetPath.GetPackageId())
			SetTitle(new CItem(m_pAssetsEditor, m_AssetPath, CSubPath::Null()));
		else
			SetTitle(new CInactiveItem(m_pAssetsEditor, m_AssetPath, CSubPath::Null()));
		
		Clear();
		
		const CAsset_MapGroup* pMapGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(m_AssetPath);
		if(pMapGroup)
		{
			CAsset_MapGroup::CIteratorLayer IterLayer;
			for(IterLayer = pMapGroup->BeginLayer(); IterLayer != pMapGroup->EndLayer(); ++IterLayer)
			{
				CAssetPath Child = pMapGroup->GetLayer(*IterLayer);
				if(Child.GetPackageId() == m_PackageId)
				{
					Add(new CItem(m_pAssetsEditor, Child, CSubPath::Null()));
					m_PackageIdFound = true;
				}
			}
		}
		
	}
	
	bool PackageIdFound()
	{
		return m_PackageIdFound || (m_AssetPath.GetPackageId() == m_PackageId);
	}
};


/* MAP ITEM ***********************************************************/

class CMapItem : public gui::CExpand
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_AssetPath;
	int m_PackageId;
	bool m_PackageIdFound;

public:
	CMapItem(CGuiEditor* pAssetsEditor, CAssetPath AssetPath, int PackageId) :
		gui::CExpand(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_PackageId(PackageId),
		m_AssetPath(AssetPath),
		m_PackageIdFound(false)
	{
		gui::CExpand* pZoneExpand = new gui::CExpand(Context());
		gui::CExpand* pEntityExpand = new gui::CExpand(Context());
		gui::CExpand* pBgExpand = new gui::CExpand(Context());
		gui::CExpand* pFgExpand = new gui::CExpand(Context());
		
		pZoneExpand->SetTitle(new CInactiveText(m_pAssetsEditor, _GUI("Zones"), m_pAssetsEditor->m_Path_Sprite_IconNone));
		pEntityExpand->SetTitle(new CInactiveText(m_pAssetsEditor, _GUI("Entities"), m_pAssetsEditor->m_Path_Sprite_IconNone));
		pBgExpand->SetTitle(new CInactiveText(m_pAssetsEditor, _GUI("Background Layers"), m_pAssetsEditor->m_Path_Sprite_IconNone));
		pFgExpand->SetTitle(new CInactiveText(m_pAssetsEditor, _GUI("Foreground Layers"), m_pAssetsEditor->m_Path_Sprite_IconNone));
		
		Add(pZoneExpand);
		Add(pEntityExpand);
		Add(pBgExpand);
		Add(pFgExpand);
		
		if(m_PackageId == m_AssetPath.GetPackageId())
			SetTitle(new CItem(m_pAssetsEditor, m_AssetPath, CSubPath::Null()));
		else
			SetTitle(new CInactiveItem(m_pAssetsEditor, m_AssetPath, CSubPath::Null()));
		
		const CAsset_Map* pMap = AssetsManager()->GetAsset<CAsset_Map>(m_AssetPath);
		if(pMap)
		{
			CAsset_Map::CIteratorZoneLayer IterZoneLayer;
			for(IterZoneLayer = pMap->BeginZoneLayer(); IterZoneLayer != pMap->EndZoneLayer(); ++IterZoneLayer)
			{
				CAssetPath Child = pMap->GetZoneLayer(*IterZoneLayer);
				if(Child.GetPackageId() == m_PackageId)
				{
					pZoneExpand->Add(new CItem(m_pAssetsEditor, Child, CSubPath::Null()));
					m_PackageIdFound = true;
				}
			}
			
			CAsset_Map::CIteratorEntityLayer IterEntityLayer;
			for(IterEntityLayer = pMap->BeginEntityLayer(); IterEntityLayer != pMap->EndEntityLayer(); ++IterEntityLayer)
			{
				CAssetPath Child = pMap->GetEntityLayer(*IterEntityLayer);
				if(Child.GetPackageId() == m_PackageId)
				{
					pEntityExpand->Add(new CItem(m_pAssetsEditor, Child, CSubPath::Null()));
					m_PackageIdFound = true;
				}
			}
			
			CAsset_Map::CIteratorBgGroup IterBgGroup;
			for(IterBgGroup = pMap->BeginBgGroup(); IterBgGroup != pMap->EndBgGroup(); ++IterBgGroup)
			{
				CAssetPath Child = pMap->GetBgGroup(*IterBgGroup);
				if(Child.GetPackageId() == m_PackageId)
				{
					pBgExpand->Add(new CMapGroupItem(m_pAssetsEditor, Child, m_PackageId));
					m_PackageIdFound = true;
				}
			}
			
			CAsset_Map::CIteratorFgGroup IterFgGroup;
			for(IterFgGroup = pMap->BeginFgGroup(); IterFgGroup != pMap->EndFgGroup(); ++IterFgGroup)
			{
				CAssetPath Child = pMap->GetFgGroup(*IterFgGroup);
				if(Child.GetPackageId() == m_PackageId)
				{
					pFgExpand->Add(new CMapGroupItem(m_pAssetsEditor, Child, m_PackageId));
					m_PackageIdFound = true;
				}
			}
		}
	}
	
	bool PackageIdFound()
	{
		return m_PackageIdFound || (m_AssetPath.GetPackageId() == m_PackageId);
	}
};

/* PACKAGES TREE ******************************************************/

CPackagesTree::CPackagesTree(CAssetsOrganizer* pAssetsOrganizer) :
	gui::CVScrollLayout(pAssetsOrganizer->AssetsEditor()),
	m_pAssetsOrganizer(pAssetsOrganizer),
	m_pAssetsEditor(pAssetsOrganizer->AssetsEditor())
{
	SetBoxStyle(AssetsEditor()->m_Path_Box_Panel);
	Refresh();
}

void CPackagesTree::Refresh()
{
	Clear();
	
	gui::CExpand* pWriteExpand = new gui::CExpand(Context());
	gui::CExpand* pReadExpand = new gui::CExpand(Context());
	
	pWriteExpand->SetTitle(new CInactiveText(m_pAssetsEditor, _GUI("Edited Packages"), m_pAssetsEditor->m_Path_Sprite_IconNone));
	pReadExpand->SetTitle(new CInactiveText(m_pAssetsEditor, _GUI("Read-Only Packages"), m_pAssetsEditor->m_Path_Sprite_IconNone));
	
	Add(pWriteExpand);
	Add(pReadExpand);
	
	for(int i=0; i<AssetsManager()->GetNumPackages(); i++)
	{
		if(AssetsManager()->IsValidPackage(i))
		{
			if(AssetsManager()->IsReadOnlyPackage(i))
				pReadExpand->Add(new CPackageItem(AssetsEditor(), i));
			else
				pWriteExpand->Add(new CPackageItem(AssetsEditor(), i));
		}
	}
}


/* ASSETS TREE ********************************************************/

CAssetsTree::CAssetsTree(CAssetsOrganizer* pAssetsOrganizer) :
	gui::CVScrollLayout(pAssetsOrganizer->AssetsEditor()),
	m_pAssetsOrganizer(pAssetsOrganizer),
	m_pAssetsEditor(pAssetsOrganizer->AssetsEditor()),
	m_PackageId(-1)
{
	SetBoxStyle(AssetsEditor()->m_Path_Box_Panel);
}

void CAssetsTree::Update(bool ParentEnabled)
{
	if(m_PackageId != AssetsEditor()->GetEditedPackageId())
	{
		m_PackageId = AssetsEditor()->GetEditedPackageId();
		Refresh();
	}
	
	gui::CVScrollLayout::Update(ParentEnabled);
}

void CAssetsTree::Refresh()
{
	//Search Tag: TAG_NEW_ASSET
	
	Clear();
	
	{
		CAssetState State;
		State.m_ListedInEditor = false;
		AssetsManager()->InitAssetState(m_PackageId, State);
	}
	
	#define REFRESH_ASSET_LIST(ClassName) {\
		for(int i=0; i<AssetsManager()->GetNumAssets<ClassName>(m_PackageId); i++)\
		{\
			CAssetPath Path = CAssetPath(ClassName::TypeId, m_PackageId, i);\
			CAssetState* pState = AssetsManager()->GetAssetState(Path);\
			if(pState && !pState->m_ListedInEditor)\
			{\
				Add(new CItem(AssetsEditor(), Path, CSubPath::Null()));\
			}\
		}\
	}
	
	//Images
	//	Sprites
	for(int s=0; s<AssetsManager()->GetNumPackages(); s++)
	{
		for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_Image>(s); i++)
		{
			CAssetPath Path = CAssetPath(CAsset_Image::TypeId, s, i);
			CImageItem* pItem = new CImageItem(AssetsEditor(), Path, m_PackageId);
			if(pItem->PackageIdFound())
				Add(pItem);
			else
				delete pItem;
		}
	}
	
	REFRESH_ASSET_LIST(CAsset_Sprite)
	
	//Maps
	for(int s=0; s<AssetsManager()->GetNumPackages(); s++)
	{
		for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_Map>(s); i++)
		{
			CAssetPath Path = CAssetPath(CAsset_Map::TypeId, s, i);
			CMapItem* pItem = new CMapItem(AssetsEditor(), Path, m_PackageId);
			if(pItem->PackageIdFound())
				Add(pItem);
			else
				delete pItem;
		}
	}
	
	REFRESH_ASSET_LIST(CAsset_MapGroup)
	REFRESH_ASSET_LIST(CAsset_MapLayerTiles)
	REFRESH_ASSET_LIST(CAsset_MapLayerQuads)
	REFRESH_ASSET_LIST(CAsset_MapZoneTiles)
	REFRESH_ASSET_LIST(CAsset_MapEntities)
	REFRESH_ASSET_LIST(CAsset_ZoneType)
	REFRESH_ASSET_LIST(CAsset_EntityType)
	
	//Skeletons
	//	SkeletonAnimations
	//	SkeletonSkins
	for(int s=0; s<AssetsManager()->GetNumPackages(); s++)
	{
		for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_Skeleton>(s); i++)
		{
			CAssetPath Path = CAssetPath(CAsset_Skeleton::TypeId, s, i);
			CSkeletonItem* pItem = new CSkeletonItem(AssetsEditor(), Path, m_PackageId);
			if(pItem->PackageIdFound())
				Add(pItem);
			else
				delete pItem;
		}
	}
	
	REFRESH_ASSET_LIST(CAsset_SkeletonAnimation)
	REFRESH_ASSET_LIST(CAsset_SkeletonSkin)
	
	//Charaters
	//	CharacterParts
	//	Weapons
	for(int s=0; s<AssetsManager()->GetNumPackages(); s++)
	{
		for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_Character>(s); i++)
		{
			CAssetPath Path = CAssetPath(CAsset_Character::TypeId, s, i);
			CCharacterItem* pItem = new CCharacterItem(AssetsEditor(), Path, m_PackageId);
			if(pItem->PackageIdFound())
				Add(pItem);
			else
				delete pItem;
		}
	}
	
	REFRESH_ASSET_LIST(CAsset_CharacterPart)
	
	REFRESH_ASSET_LIST(CAsset_GuiRectStyle)
	REFRESH_ASSET_LIST(CAsset_GuiLineStyle)
	REFRESH_ASSET_LIST(CAsset_GuiBoxStyle)
	REFRESH_ASSET_LIST(CAsset_GuiLabelStyle)
	REFRESH_ASSET_LIST(CAsset_GuiButtonStyle)
	REFRESH_ASSET_LIST(CAsset_GuiIntEditStyle)
	REFRESH_ASSET_LIST(CAsset_GuiColorEditStyle)
	REFRESH_ASSET_LIST(CAsset_GuiToggleStyle)
	REFRESH_ASSET_LIST(CAsset_GuiScrollbarStyle)
	REFRESH_ASSET_LIST(CAsset_GuiTabsStyle)
}

/* ASSETS LIST ********************************************************/

CAssetsOrganizer::CAssetsOrganizer(CGuiEditor* pAssetsEditor) :
	gui::CVPanelLayout(pAssetsEditor),
	m_pAssetsEditor(pAssetsEditor)
{
	m_pPackagesTree = new CPackagesTree(this);
	m_pAssetsEditor->m_pPackagesTree = m_pPackagesTree;
	Add(m_pPackagesTree, 200);
	
	m_pAssetsTree = new CAssetsTree(this);
	m_pAssetsEditor->m_pAssetsTree = m_pAssetsTree;
	Add(m_pAssetsTree, -1);
}
