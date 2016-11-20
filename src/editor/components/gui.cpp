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

#include <editor/components/gui.h>

#include <shared/components/assetsmanager.h>
#include <shared/components/storage.h>
#include <client/gui/panellayout.h>
#include <client/gui/popup.h>
#include <client/gui/filler.h>
#include <client/gui/text-edit.h>
#include <client/gui/toggle.h>
#include <client/keys.h>
#include <client/components/input.h>
#include <client/gui/panellayout.h>
#include <client/loading_tools.h>
#include <editor/gui/assetslist.h>
#include <editor/gui/view.h>
#include <editor/gui/timeline.h>
#include <editor/gui/assetsinspector.h>

/* CONTEXT MENU *******************************************************/

CContextMenu::CContextMenu(CGuiEditor* pAssetsEditor) :
	gui::CPopup(pAssetsEditor, gui::CRect(pAssetsEditor->GetMousePos().x, pAssetsEditor->GetMousePos().y, 0, 0), 250, -1, gui::CPopup::ALIGNMENT_SIDE),
	m_pAssetsEditor(pAssetsEditor)
{		
	m_pList = new gui::CVListLayout(Context());
	m_pList->SetBoxStyle(m_pAssetsEditor->m_Path_Box_ToobarMenu);
	Add(m_pList);
}

class COpenSavePackageDialog : public gui::CPopup
{
public:
	enum
	{
		FORMAT_PACKAGE=0,
		FORMAT_MAP_TW,
		FORMAT_MAP_INFCLASS,
		FORMAT_MAP_OPENFNG,
		FORMAT_MAP_DDNET,
		FORMAT_IMAGE,
	};
	
protected:
	class CItem_Load : public gui::CButton
	{
	protected:
		COpenSavePackageDialog* m_pPopup;
		CGuiEditor* m_pAssetsEditor;
		dynamic_string m_Filename;
		int m_StorageType;
		
	protected:
		virtual void MouseClickAction()
		{
			m_pPopup->Select(m_Filename.buffer(), m_StorageType);
		}
		
	public:
		CItem_Load(COpenSavePackageDialog* pPopup, const char* pFilename, int StorageType) :
			gui::CButton(pPopup->Context(), pFilename, pPopup->m_pAssetsEditor->m_Path_Sprite_IconAsset),
			m_pPopup(pPopup),
			m_pAssetsEditor(pPopup->m_pAssetsEditor)
		{
			m_Filename.copy(pFilename);
			m_StorageType = StorageType;
		}
		
		virtual void Update(bool ParentEnabled)
		{
			if(m_Filename == m_pPopup->m_SelectedPackage)
				SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItemHL);
			else
				SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItem);
		}
	};
	
	class COpen : public gui::CButton
	{
	protected:
		COpenSavePackageDialog* m_pPopup;
		
	protected:
		virtual void MouseClickAction()
		{
			m_pPopup->Open();
		}
		
	public:
		COpen(COpenSavePackageDialog* pPopup) :
			gui::CButton(pPopup->Context(), "Open"),
			m_pPopup(pPopup)
		{
			SetButtonStyle(pPopup->m_pAssetsEditor->m_Path_Button_Dialog);
		}
	};
	
	class CSave : public gui::CButton
	{
	protected:
		COpenSavePackageDialog* m_pPopup;
		
	protected:
		virtual void MouseClickAction()
		{
			m_pPopup->Save();
		}
		
	public:
		CSave(COpenSavePackageDialog* pPopup) :
			gui::CButton(pPopup->Context(), "Save"),
			m_pPopup(pPopup)
		{
			SetButtonStyle(pPopup->m_pAssetsEditor->m_Path_Button_Dialog);
		}
	};
	
	class CCancel : public gui::CButton
	{
	protected:
		COpenSavePackageDialog* m_pPopup;
		
	protected:
		virtual void MouseClickAction()
		{
			m_pPopup->Close();
		}
		
	public:
		CCancel(COpenSavePackageDialog* pPopup) :
			gui::CButton(pPopup->Context(), "Cancel"),
			m_pPopup(pPopup)
		{
			SetButtonStyle(pPopup->m_pAssetsEditor->m_Path_Button_Dialog);
		}
	};

protected:
	CGuiEditor* m_pAssetsEditor;
	dynamic_string m_SelectedPackage;
	int m_SelectedStorageType;
	gui::CVScrollLayout* m_pFilelist;
	int m_Format;
	bool m_Save;
	
public:
	COpenSavePackageDialog(CGuiEditor* pAssetsEditor, bool Save, int Format) :
		gui::CPopup(pAssetsEditor, pAssetsEditor->GetDrawRect(), 600, 450, gui::CPopup::ALIGNMENT_INNER),
		m_pAssetsEditor(pAssetsEditor),
		m_pFilelist(NULL),
		m_Save(Save),
		m_Format(Format)
	{
		gui::CVScrollLayout* pLayout = new gui::CVScrollLayout(Context());
		pLayout->SetBoxStyle(m_pAssetsEditor->m_Path_Box_Dialog);
		Add(pLayout);
		
		m_pFilelist = new gui::CVScrollLayout(Context());
		pLayout->Add(m_pFilelist, true);
		
		if(m_Format == FORMAT_IMAGE)
		{
			if(m_Save)
			{
				m_pFilelist->Add(new gui::CLabelHeader(Context(), "Export Image"), false);
				ListFiles("images", CStorage::TYPE_SAVE);
			}
			else
			{
				m_pFilelist->Add(new gui::CLabelHeader(Context(), "Import Image"), false);
				ListFiles("images", CStorage::TYPE_ALL);
			}
		}
		else if(m_Format == FORMAT_MAP_TW)
		{
			if(m_Save)
			{
				m_pFilelist->Add(new gui::CLabelHeader(Context(), "Export TeeWorlds Map"), false);
				ListFiles("maps", CStorage::TYPE_SAVE);
			}
			else
			{
				m_pFilelist->Add(new gui::CLabelHeader(Context(), "Import TeeWorlds Map"), false);
				ListFiles("maps", CStorage::TYPE_ALL);
			}
		}
		else if(m_Format == FORMAT_MAP_INFCLASS)
		{
			if(m_Save)
			{
				m_pFilelist->Add(new gui::CLabelHeader(Context(), "Export InfClass Map"), false);
				ListFiles("maps", CStorage::TYPE_SAVE);
			}
			else
			{
				m_pFilelist->Add(new gui::CLabelHeader(Context(), "Import InfClass Map"), false);
				ListFiles("maps", CStorage::TYPE_ALL);
			}
		}
		else if(m_Format == FORMAT_MAP_DDNET)
		{
			if(m_Save)
			{
				m_pFilelist->Add(new gui::CLabelHeader(Context(), "Export DDNet Map"), false);
				ListFiles("maps", CStorage::TYPE_SAVE);
			}
			else
			{
				m_pFilelist->Add(new gui::CLabelHeader(Context(), "Import DDNet Map"), false);
				ListFiles("maps", CStorage::TYPE_ALL);
			}
		}
		else if(m_Format == FORMAT_MAP_OPENFNG)
		{
			if(m_Save)
			{
				m_pFilelist->Add(new gui::CLabelHeader(Context(), "Export OpenFNG Map"), false);
				ListFiles("maps", CStorage::TYPE_SAVE);
			}
			else
			{
				m_pFilelist->Add(new gui::CLabelHeader(Context(), "Import OpenFNG Map"), false);
				ListFiles("maps", CStorage::TYPE_ALL);
			}
		}
		else
		{
			if(m_Save)
			{
				m_pFilelist->Add(new gui::CLabelHeader(Context(), "Save Package"), false);
				ListFiles("assets", CStorage::TYPE_SAVE);
			}
			else
			{
				m_pFilelist->Add(new gui::CLabelHeader(Context(), "Open Package"), false);
				ListFiles("assets", CStorage::TYPE_ALL);
			}
		}
		
		//Filename
		if(m_Save)
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pLayout->Add(pHList, false);
			
			gui::CLabel* pLabel = new gui::CLabel(Context(), "Filename");
			pLabel->NoTextClipping();
			pHList->Add(pLabel, false);
			pHList->Add(new gui::CExternalTextEdit_DynamicString(Context(), &m_SelectedPackage), true);
			
		}
		
		pLayout->AddSeparator();
		
		//Buttonlist
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pLayout->Add(pHList, false);
			
			pHList->Add(new CCancel(this), false);
			pHList->Add(new gui::CFiller(Context()), true);
			
			if(m_Save)
				pHList->Add(new CSave(this), false);
			else
				pHList->Add(new COpen(this), false);
		}
	}
	
	void ListFiles(const char* pDir, int StorageType)
	{
		CStorage::CListDirIterator* pIter = Storage()->CreateListDirIterator(pDir, StorageType);
		
		if(!pIter)
			return;
		
		while(pIter->Next())
		{
			dynamic_string Buffer;
			pIter->GetFilename(Buffer);
			
			int Length = Buffer.length();
			if(Buffer.buffer()[0] == '.' && (Buffer.buffer()[1] == 0))
				continue;
			
			bool Found = false;
			switch(m_Format)
			{
				case FORMAT_IMAGE:
					if(Length >= 4 && str_comp(Buffer.buffer()+Length-4, ".png") == 0)
					{
						Found = true;
						Buffer.buffer()[Length-4] = 0;
					}
					break;
				case FORMAT_MAP_TW:
				case FORMAT_MAP_INFCLASS:
				case FORMAT_MAP_DDNET:
				case FORMAT_MAP_OPENFNG:
					if(Length >= 4 && str_comp(Buffer.buffer()+Length-4, ".map") == 0)
					{
						Found = true;
						Buffer.buffer()[Length-4] = 0;
					}
					break;
				case FORMAT_PACKAGE:
					if(Length >= 7 && str_comp(Buffer.buffer()+Length-7, ".assets") == 0)
					{
						Found = true;
						Buffer.buffer()[Length-7] = 0;
					}
					break;
			}
			
			if(Found)
				m_pFilelist->Add(new CItem_Load(this, Buffer.buffer(), StorageType), false);
		}
		
		delete pIter;
	}
	
	void Select(const char* pFilename, int StorageType)
	{
		m_SelectedStorageType = StorageType;
		m_SelectedPackage.copy(pFilename);
	}
	
	void Save()
	{
		AssetsManager()->Save_AssetsFile(m_SelectedPackage.buffer(), m_pAssetsEditor->GetEditedPackageId());
		Close();
	}
	
	void Open()
	{
		switch(m_Format)
		{
			case FORMAT_IMAGE:
			{
				dynamic_string Buffer;
				Buffer.append("images/");
				Buffer.append(m_SelectedPackage);
				Buffer.append(".png");
				
				CAssetPath ImageEditorPath = CreateNewImage(m_pAssetsEditor->SharedKernel(), m_pAssetsEditor->GetEditedPackageId(), m_SelectedPackage.buffer(), Buffer.buffer(), -1, -1);
				m_pAssetsEditor->SetEditedAsset(ImageEditorPath, CSubPath::Null());
				break;
			}
			case FORMAT_MAP_TW:
			{
				int PackageId = AssetsManager()->Load_Map(m_SelectedPackage.buffer(), m_SelectedStorageType, CAssetsManager::MAPFORMAT_TW);
				AssetsManager()->SetPackageReadOnly(PackageId, false);
				m_pAssetsEditor->SetEditedPackage(PackageId);
				m_pAssetsEditor->RefreshAssetsTree();
				break;
			}
			case FORMAT_MAP_DDNET:
			{
				int PackageId = AssetsManager()->Load_Map(m_SelectedPackage.buffer(), m_SelectedStorageType, CAssetsManager::MAPFORMAT_DDNET);
				AssetsManager()->SetPackageReadOnly(PackageId, false);
				m_pAssetsEditor->SetEditedPackage(PackageId);
				m_pAssetsEditor->RefreshAssetsTree();
				break;
			}
			case FORMAT_MAP_INFCLASS:
			{
				int PackageId = AssetsManager()->Load_Map(m_SelectedPackage.buffer(), m_SelectedStorageType, CAssetsManager::MAPFORMAT_INFCLASS);
				AssetsManager()->SetPackageReadOnly(PackageId, false);
				m_pAssetsEditor->SetEditedPackage(PackageId);
				m_pAssetsEditor->RefreshAssetsTree();
				break;
			}
			case FORMAT_MAP_OPENFNG:
			{
				int PackageId = AssetsManager()->Load_Map(m_SelectedPackage.buffer(), m_SelectedStorageType, CAssetsManager::MAPFORMAT_OPENFNG);
				AssetsManager()->SetPackageReadOnly(PackageId, false);
				m_pAssetsEditor->SetEditedPackage(PackageId);
				m_pAssetsEditor->RefreshAssetsTree();
				break;
			}
			case FORMAT_PACKAGE:
			{
				int PackageId = AssetsManager()->Load_AssetsFile(m_SelectedPackage.buffer(), m_SelectedStorageType);
				AssetsManager()->SetPackageReadOnly(PackageId, false);
				m_pAssetsEditor->SetEditedPackage(PackageId);
				m_pAssetsEditor->RefreshAssetsTree();
				break;
			}
		}
			
		Close();
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

/* TOOLBAR MENU *******************************************************/

class CPopup_ToolbarMenu : public gui::CPopup
{	
protected:
	CGuiEditor* m_pAssetsEditor;
	char m_aFilename[256];
	int m_PackageId;
	int m_Mode;
	gui::CVListLayout* m_pList;
	
public:
	CPopup_ToolbarMenu(CGuiEditor* pAssetsEditor, const gui::CRect& CreatorRect) :
		gui::CPopup(pAssetsEditor, CreatorRect, 250, -1, gui::CPopup::ALIGNMENT_BOTTOM),
		m_pAssetsEditor(pAssetsEditor)
	{		
		m_pList = new gui::CVListLayout(Context());
		m_pList->SetBoxStyle(m_pAssetsEditor->m_Path_Box_ToobarMenu);
		Add(m_pList);
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
	
	gui::CVListLayout* List() { return m_pList; }
};

/* BUTTONS ************************************************************/

class CNewPackageButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_ToolbarMenu* m_pPopupMenu;
	
protected:
	virtual void MouseClickAction()
	{
		int PackageId = AssetsManager()->NewPackage("mypackage");
		AssetsManager()->SetPackageReadOnly(PackageId, false);
		m_pAssetsEditor->SetEditedPackage(PackageId);
		
		m_pPopupMenu->Close();
	}

public:
	CNewPackageButton(CGuiEditor* pAssetsEditor, CPopup_ToolbarMenu* pPopupMenu) :
		gui::CButton(pAssetsEditor, "New Package"),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Toolbar);
	}
};

class COpenPackageButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_ToolbarMenu* m_pPopupMenu;
	
protected:
	virtual void MouseClickAction()
	{
		m_pAssetsEditor->DisplayPopup(new COpenSavePackageDialog(m_pAssetsEditor, false, COpenSavePackageDialog::FORMAT_PACKAGE));
		m_pPopupMenu->Close();
	}

public:
	COpenPackageButton(CGuiEditor* pAssetsEditor, CPopup_ToolbarMenu* pPopupMenu) :
		gui::CButton(pAssetsEditor, "Open Package"),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Toolbar);
	}
};

class CImportButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_ToolbarMenu* m_pPopupMenu;
	int m_Format;
	
protected:
	virtual void MouseClickAction()
	{
		m_pAssetsEditor->DisplayPopup(new COpenSavePackageDialog(m_pAssetsEditor, false, m_Format));
		m_pPopupMenu->Close();
	}

public:
	CImportButton(CGuiEditor* pAssetsEditor, CPopup_ToolbarMenu* pPopupMenu, int Format) :
		gui::CButton(pAssetsEditor, ""),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu),
		m_Format(Format)
	{
		switch(m_Format)
		{
			case COpenSavePackageDialog::FORMAT_IMAGE:
				SetText("Import Image");
				break;
			case COpenSavePackageDialog::FORMAT_MAP_TW:
				SetText("Import TeeWorlds Map");
				break;
			case COpenSavePackageDialog::FORMAT_MAP_DDNET:
				SetText("Import DDNet Map");
				break;
			case COpenSavePackageDialog::FORMAT_MAP_INFCLASS:
				SetText("Import InfClass Map");
				break;
			case COpenSavePackageDialog::FORMAT_MAP_OPENFNG:
				SetText("Import OpenFNG");
				break;
		}
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Toolbar);
	}
};

class CSavePackageButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_ToolbarMenu* m_pPopupMenu;
	
protected:
	virtual void MouseClickAction()
	{
		AssetsManager()->Save_AssetsFile(m_pAssetsEditor->GetEditedPackageId());
		m_pPopupMenu->Close();
	}

public:
	CSavePackageButton(CGuiEditor* pAssetsEditor, CPopup_ToolbarMenu* pPopupMenu) :
		gui::CButton(pAssetsEditor, "Save Package"),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Toolbar);
	}
};

class CSavePackageAsButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_ToolbarMenu* m_pPopupMenu;
	
protected:
	virtual void MouseClickAction()
	{
		m_pAssetsEditor->DisplayPopup(new COpenSavePackageDialog(m_pAssetsEditor, true, COpenSavePackageDialog::FORMAT_PACKAGE));
		m_pPopupMenu->Close();
	}

public:
	CSavePackageAsButton(CGuiEditor* pAssetsEditor, CPopup_ToolbarMenu* pPopupMenu) :
		gui::CButton(pAssetsEditor, "Save Package As..."),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Toolbar);
	}
};

class CQuitButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_ToolbarMenu* m_pPopupMenu;
	
protected:
	virtual void MouseClickAction()
	{
		m_pAssetsEditor->Quit();
		m_pPopupMenu->Close();
	}

public:
	CQuitButton(CGuiEditor* pAssetsEditor, CPopup_ToolbarMenu* pPopupMenu) :
		gui::CButton(pAssetsEditor, "Quit"),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Toolbar);
	}
};

class CNewAsset : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_ToolbarMenu* m_pPopupMenu;
	int m_AssetType;
	
protected:
	virtual void MouseClickAction()
	{
		CAssetPath AssetPath;
		switch(m_AssetType)
		{
			case CAsset_Map::TypeId:
			{
				AssetsManager()->Load_UnivTeeWorlds();
				
				int Tokken = AssetsManager()->GenerateToken();
				CAsset_Map* pMap = AssetsManager()->NewAsset<CAsset_Map>(&AssetPath, m_pAssetsEditor->GetEditedPackageId(), Tokken);
				AssetsManager()->TryChangeAssetName(AssetPath, "map", Tokken);
				
				CAssetPath SubAssetPath;
				CAsset_MapGroup* pMapGroup;
				CAsset_MapLayerTiles* pMapLayerTiles;
				CAsset_MapLayerQuads* pMapLayerQuads;
				CAsset_MapZoneTiles* pMapZoneTiles;
				CSubPath SubPath;
				
				//Zone, Physics
				pMapZoneTiles = AssetsManager()->NewAsset<CAsset_MapZoneTiles>(&SubAssetPath, m_pAssetsEditor->GetEditedPackageId(), Tokken);
				AssetsManager()->TryChangeAssetName(SubAssetPath, "physics", Tokken);
				SubPath = CAsset_Map::SubPath_ZoneLayer(pMap->AddZoneLayer());
				pMap->SetZoneLayer(SubPath, SubAssetPath);
				
				{
					array2d< CAsset_MapZoneTiles::CTile, allocator_copy<CAsset_MapZoneTiles::CTile> >& Data = pMapZoneTiles->GetTileArray();
					Data.resize(64, 64);
					
					pMapZoneTiles->SetZoneTypePath(AssetsManager()->m_Path_ZoneType_TWPhysics);
				}
				
				//Zone, Damage
				pMapZoneTiles = AssetsManager()->NewAsset<CAsset_MapZoneTiles>(&SubAssetPath, m_pAssetsEditor->GetEditedPackageId(), Tokken);
				AssetsManager()->TryChangeAssetName(SubAssetPath, "damage", Tokken);
				SubPath = CAsset_Map::SubPath_ZoneLayer(pMap->AddZoneLayer());
				pMap->SetZoneLayer(SubPath, SubAssetPath);
				
				{
					array2d< CAsset_MapZoneTiles::CTile, allocator_copy<CAsset_MapZoneTiles::CTile> >& Data = pMapZoneTiles->GetTileArray();
					Data.resize(64, 64);
					
					pMapZoneTiles->SetZoneTypePath(AssetsManager()->m_Path_ZoneType_TWDamage);
				}
				
				//Background
				pMapGroup = AssetsManager()->NewAsset<CAsset_MapGroup>(&SubAssetPath, m_pAssetsEditor->GetEditedPackageId(), Tokken);
				AssetsManager()->TryChangeAssetName(SubAssetPath, "background", Tokken);
				SubPath = CAsset_Map::SubPath_BgGroup(pMap->AddBgGroup());
				pMap->SetBgGroup(SubPath, SubAssetPath);
				
					//Sky
				pMapLayerQuads = AssetsManager()->NewAsset<CAsset_MapLayerQuads>(&SubAssetPath, m_pAssetsEditor->GetEditedPackageId(), Tokken);
				AssetsManager()->TryChangeAssetName(SubAssetPath, "sky", Tokken);
				SubPath = CAsset_MapGroup::SubPath_Layer(pMapGroup->AddLayer());
				pMapGroup->SetLayer(SubPath, SubAssetPath);
				
						//Quad
				SubPath = CAsset_MapLayerQuads::SubPath_Quad(pMapLayerQuads->AddQuad());
				pMapLayerQuads->SetQuadPivot(SubPath, 0.0f);
				
				pMapLayerQuads->SetQuadVertex0(SubPath, vec2(-800.0f, -600.0f));
				pMapLayerQuads->SetQuadVertex1(SubPath, vec2(800.0f, -600.0f));
				pMapLayerQuads->SetQuadVertex2(SubPath, vec2(-800.0f, 600.0f));
				pMapLayerQuads->SetQuadVertex3(SubPath, vec2(800.0f, 600.0f));
				
				pMapLayerQuads->SetQuadUV0(SubPath, vec2(0.0f, 0.0f));
				pMapLayerQuads->SetQuadUV1(SubPath, vec2(1.0f, 0.0f));
				pMapLayerQuads->SetQuadUV2(SubPath, vec2(0.0f, 1.0f));
				pMapLayerQuads->SetQuadUV3(SubPath, vec2(1.0f, 1.0f));
				
				vec4 ColorTop = vec4(94.0f/255.0f, 132.0f/255.0f, 174.0f/255.0f, 1.0f);
				vec4 ColorBottom = vec4(204.0f/255.0f, 232.0f/255.0f, 1.0f, 1.0f);
				
				pMapLayerQuads->SetQuadColor0(SubPath, ColorTop);
				pMapLayerQuads->SetQuadColor1(SubPath, ColorTop);
				pMapLayerQuads->SetQuadColor2(SubPath, ColorBottom);
				pMapLayerQuads->SetQuadColor3(SubPath, ColorBottom);
				break;
			}
		}
		
		m_pAssetsEditor->SetEditedAsset(AssetPath, CSubPath::Null());
		m_pAssetsEditor->RefreshAssetsTree();
		
		m_pPopupMenu->Close();
	}

public:
	CNewAsset(CGuiEditor* pAssetsEditor, CPopup_ToolbarMenu* pPopupMenu, int AssetType, const char* pName) :
		gui::CButton(pAssetsEditor, pName),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu),
		m_AssetType(AssetType)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Toolbar);
	}
};

class CToggleFullscreen : public gui::CToggle
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_ToolbarMenu* m_pPopupMenu;
	
protected:
	virtual bool GetValue()
	{
		return Graphics()->m_Cfg_Fullscreen;
	}
	
	virtual void SetValue(bool Value)
	{
		if(Value != (Graphics()->m_Cfg_Fullscreen > 0))
		{
			Graphics()->m_Cfg_Fullscreen = (Value ? 1 : 0);
			Graphics()->Fullscreen(Value);
		}
	}
	
public:
	CToggleFullscreen(CGuiEditor* pAssetsEditor, CPopup_ToolbarMenu* pPopupMenu) :
		gui::CToggle(pAssetsEditor, "Fullscreen"),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu)
	{
		SetToggleStyle(m_pAssetsEditor->m_Path_Toggle_Toolbar);
	}
};

class CFileButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	
protected:
	virtual void MouseClickAction()
	{
		CPopup_ToolbarMenu* pMenu = new CPopup_ToolbarMenu(m_pAssetsEditor, m_DrawRect);
		pMenu->List()->Add(new CNewPackageButton(m_pAssetsEditor, pMenu));
		pMenu->List()->Add(new COpenPackageButton(m_pAssetsEditor, pMenu));
		pMenu->List()->Add(new CSavePackageButton(m_pAssetsEditor, pMenu));
		pMenu->List()->Add(new CSavePackageAsButton(m_pAssetsEditor, pMenu));
		pMenu->List()->AddSeparator();
		pMenu->List()->Add(new CImportButton(m_pAssetsEditor, pMenu, COpenSavePackageDialog::FORMAT_MAP_TW));
		pMenu->List()->Add(new CImportButton(m_pAssetsEditor, pMenu, COpenSavePackageDialog::FORMAT_MAP_DDNET));
		pMenu->List()->Add(new CImportButton(m_pAssetsEditor, pMenu, COpenSavePackageDialog::FORMAT_MAP_INFCLASS));
		pMenu->List()->Add(new CImportButton(m_pAssetsEditor, pMenu, COpenSavePackageDialog::FORMAT_MAP_OPENFNG));
		pMenu->List()->AddSeparator();
		pMenu->List()->Add(new CQuitButton(m_pAssetsEditor, pMenu));
		
		m_pAssetsEditor->DisplayPopup(pMenu);
	}

public:
	CFileButton(CGuiEditor* pAssetsEditor) :
		gui::CButton(pAssetsEditor, "File"),
		m_pAssetsEditor(pAssetsEditor)
	{
		NoTextClipping();
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Toolbar);
	}
};

class CAssetButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	
protected:
	virtual void MouseClickAction()
	{
		CPopup_ToolbarMenu* pMenu = new CPopup_ToolbarMenu(m_pAssetsEditor, m_DrawRect);
		
		pMenu->List()->Add(new CImportButton(m_pAssetsEditor, pMenu, COpenSavePackageDialog::FORMAT_IMAGE));
		pMenu->List()->Add(new CNewAsset(m_pAssetsEditor, pMenu, CAsset_Map::TypeId, "New Map"));
		
		m_pAssetsEditor->DisplayPopup(pMenu);
	}

public:
	CAssetButton(CGuiEditor* pAssetsEditor) :
		gui::CButton(pAssetsEditor, "Asset"),
		m_pAssetsEditor(pAssetsEditor)
	{
		NoTextClipping();
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Toolbar);
	}
};

class CViewButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	
protected:
	virtual void MouseClickAction()
	{
		CPopup_ToolbarMenu* pMenu = new CPopup_ToolbarMenu(m_pAssetsEditor, m_DrawRect);
		
		pMenu->List()->Add(new CToggleFullscreen(m_pAssetsEditor, pMenu));
		
		m_pAssetsEditor->DisplayPopup(pMenu);
	}

public:
	CViewButton(CGuiEditor* pAssetsEditor) :
		gui::CButton(pAssetsEditor, "View"),
		m_pAssetsEditor(pAssetsEditor)
	{
		NoTextClipping();
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Toolbar);
	}
};

/* MAIN WIDGET ********************************************************/

CGuiEditor::CMainWidget::CMainWidget(CGuiEditor* pAssetsEditor) :
	gui::CVListLayout(pAssetsEditor),
	m_pAssetsEditor(pAssetsEditor),
	m_pToolbar(NULL)
{
	//Toolbar
	m_pToolbar = CreateToolbar();
	Add(m_pToolbar, false);
	
	//Panel
	gui::CHPanelLayout* pPanel = new gui::CHPanelLayout(pAssetsEditor);
	Add(pPanel, true);
	
	gui::CVPanelLayout* pVPanel = new gui::CVPanelLayout(pAssetsEditor);
	
	pPanel->Add(new CAssetsOrganizer(pAssetsEditor), 280);
	
	pVPanel->Add(new CViewManager(pAssetsEditor), -1);
	//pVPanel->Add(new CTimeLine(pAssetsEditor), 200);
	pPanel->Add(pVPanel, -1);
	
	pPanel->Add(new CAssetsInspector(pAssetsEditor), 280);
}

void CGuiEditor::CMainWidget::Render()
{
	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(58.0f/255.0f, 58.0f/255.0f, 58.0f/255.0f, 1.0f);
	
	CGraphics::CQuadItem QuadItem(m_DrawRect.w/2, m_DrawRect.h/2, m_DrawRect.w, m_DrawRect.h);
	Graphics()->QuadsDraw(&QuadItem, 1);
	
	Graphics()->QuadsEnd();
	
	gui::CVListLayout::Render();
}

void CGuiEditor::CMainWidget::OnInputEvent(const CInput::CEvent& Event)
{
	if(Event.m_Key == KEY_ESCAPE && (Event.m_Flags & CInput::FLAG_RELEASE))
	{
		m_pAssetsEditor->Quit();
		return;
	}
	
	if(Input()->KeyIsPressed(KEY_LCTRL) && Event.m_Key == KEY_Z && (Event.m_Flags & CInput::FLAG_RELEASE))
	{
		AssetsManager()->Undo();
		return;
	}
	if(Input()->KeyIsPressed(KEY_LCTRL) && Event.m_Key == KEY_A && (Event.m_Flags & CInput::FLAG_RELEASE))
	{
		Context()->m_Cfg_Scale -= 4;
		return;
	}
	if(Input()->KeyIsPressed(KEY_LCTRL) && Event.m_Key == KEY_E && (Event.m_Flags & CInput::FLAG_RELEASE))
	{
		Context()->m_Cfg_Scale += 4;
		return;
	}
	
	gui::CVListLayout::OnInputEvent(Event);
}

gui::CWidget* CGuiEditor::CMainWidget::CreateToolbar()
{
	gui::CHListLayout* pToolbar = new gui::CHListLayout(m_pAssetsEditor);
	
	pToolbar->Add(new CFileButton(m_pAssetsEditor), false);
	pToolbar->Add(new CAssetButton(m_pAssetsEditor), false);
	pToolbar->Add(new CViewButton(m_pAssetsEditor), false);
	
	return pToolbar;
}


/* CONTEXT ************************************************************/

CGuiEditor::CGuiEditor(CEditorKernel* pEditorKernel) :
	CGui(pEditorKernel),
	m_pEditorKernel(pEditorKernel),
	m_EditedSubPath(CSubPath::Null()),
	m_pAssetsTree(NULL)
{
	SetName("GuiEditor");
}

CGuiEditor::~CGuiEditor()
{
	
}

void CGuiEditor::LoadAssets()
{
	int PackageId;
	
	PackageId = AssetsManager()->Load_AssetsFile("gui_cursor", CStorage::TYPE_ALL);
	if(PackageId >= 0)
	{
		m_Path_Sprite_CursorPointer = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "cursorPointer");
		m_Path_Sprite_CursorTranslateX = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "cursorTranslateX");
		m_Path_Sprite_CursorTranslateY = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "cursorTranslateY");
		m_Path_Sprite_CursorText = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "cursorText");
	}
	
	PackageId = AssetsManager()->Load_AssetsFile("gui_editor", CStorage::TYPE_ALL);
	if(PackageId >= 0)
	{
		m_Path_Image_ZoneTexture = AssetsManager()->FindAsset<CAsset_Image>(PackageId, "zoneTexture");
		
		m_Path_Rect_Selection = AssetsManager()->FindAsset<CAsset_GuiRectStyle>(PackageId, "selection");
		m_Path_Rect_Border = AssetsManager()->FindAsset<CAsset_GuiRectStyle>(PackageId, "border");
		
		m_Path_Box_Popup = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "popup");
		m_Path_Box_View = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "view");
		m_Path_Box_Panel = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "panel");
		m_Path_Box_ToobarMenu = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "toolbarMenu");
		m_Path_Box_Dialog = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "dialog");
		m_Path_Box_SubList = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "subList");
		
		m_Path_Label_Text = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "text");
		m_Path_Label_Header = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "header");
		m_Path_Label_Compose = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "compose");
		m_Path_Label_InactiveListItem = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "inactiveListItem");
		
		m_Path_Button_Default = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "default");
		m_Path_Button_Dialog = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "dialog");
		m_Path_Button_TextEdit = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "textEdit");
		m_Path_Button_NumericEdit = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "numericEdit");
		m_Path_Button_ListItem = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "listItem");
		m_Path_Button_ListItemHL = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "listItemHL");
		m_Path_Button_Toolbar = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "toolbar");
		m_Path_Button_CursorTool = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "cursorTool");
		m_Path_Button_CursorToolHL = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "cursorToolHL");
		m_Path_Button_PaletteIcon = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "paletteIcon");
		
		m_Path_Toggle_Toolbar = AssetsManager()->FindAsset<CAsset_GuiToggleStyle>(PackageId, "toolbar");
		
		m_Path_IntEdit_Default = AssetsManager()->FindAsset<CAsset_GuiIntEditStyle>(PackageId, "default");
		m_Path_ColorEdit_Default = AssetsManager()->FindAsset<CAsset_GuiColorEditStyle>(PackageId, "default");
		
		m_Path_Scrollbar_Default = AssetsManager()->FindAsset<CAsset_GuiScrollbarStyle>(PackageId, "default");
		
		m_Path_Tabs_Default = AssetsManager()->FindAsset<CAsset_GuiTabsStyle>(PackageId, "default");

		m_Path_Sprite_IconNone = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconNone");
		m_Path_Sprite_IconDefault = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconDefault");
		m_Path_Sprite_IconFolder = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconFolder");
		m_Path_Sprite_IconLoad = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLoad");
		m_Path_Sprite_IconSave = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconSave");
		m_Path_Sprite_IconAsset = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconAsset");
		m_Path_Sprite_IconNewAsset = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconNewAsset");
		m_Path_Sprite_IconImage = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconImage");
		m_Path_Sprite_IconSprite = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconSprite");
		m_Path_Sprite_IconSkeleton = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconSkeleton");
		m_Path_Sprite_IconSkeletonSkin = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconSkeletonSki");
		m_Path_Sprite_IconSkeletonAnimation = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconSkeletonAnimation");
		m_Path_Sprite_IconCharacter = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconCharacter");
		m_Path_Sprite_IconCharacterPart = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconCharacterPart");
		m_Path_Sprite_IconWeapon = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconWeapon");
		m_Path_Sprite_IconMap = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconMap");
		m_Path_Sprite_IconQuad = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconQuad");
		m_Path_Sprite_IconTiles = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconTiles");
		m_Path_Sprite_IconZoneTiles = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconZoneTiles");
		m_Path_Sprite_IconZoneType = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconZoneType");
		m_Path_Sprite_IconGuiRect = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiRect");
		m_Path_Sprite_IconGuiLine = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiLine");
		m_Path_Sprite_IconGuiBox = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiBox");
		m_Path_Sprite_IconGuiLabel = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiLabel");
		m_Path_Sprite_IconGuiButton = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiButton");
		m_Path_Sprite_IconGuiToggle = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiToggle");
		m_Path_Sprite_IconGuiIntEdit = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiIntEdit");
		m_Path_Sprite_IconGuiSlider = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiSlider");
		m_Path_Sprite_IconGuiScrollbar = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiScrollbar");
		m_Path_Sprite_IconGuiTabs = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiTabs");
		m_Path_Sprite_IconStamp = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconStamp");
		m_Path_Sprite_IconMove = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconMove");
		m_Path_Sprite_IconView = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconView");
		m_Path_Sprite_IconHidden = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconHidden");
		
		m_Path_Sprite_GizmoScale = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoScale");
		m_Path_Sprite_GizmoRotate = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoRotate");
		m_Path_Sprite_GizmoPivot = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoPivot");
		m_Path_Sprite_GizmoVertexBg = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoVertexBg");
		m_Path_Sprite_GizmoVertexFg = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoVertexFg");
	}
	
	//Init default path to the GUI
	m_CursorPointerPath = m_Path_Sprite_CursorPointer;
	m_CursorTranslateXPath = m_Path_Sprite_CursorTranslateX;
	m_CursorTranslateYPath = m_Path_Sprite_CursorTranslateY;
	m_CursorTextPath = m_Path_Sprite_CursorText;
	
	m_LabelStyle = m_Path_Label_Text;
	m_LabelHeaderStyle = m_Path_Label_Header;
	m_ButtonStyle = m_Path_Button_Default;
	m_ToggleStyle = m_Path_Toggle_Toolbar;
	m_IntEditStyle = m_Path_IntEdit_Default;
	m_ColorEditStyle = m_Path_ColorEdit_Default;
	m_TextEntryStyle = m_Path_Button_TextEdit;
	m_NumericEntryStyle = m_Path_Button_NumericEdit;
	m_ScrollbarStyle = m_Path_Scrollbar_Default;
	m_TabsStyle = m_Path_Tabs_Default;
	m_PopupStyle = m_Path_Box_Popup;
	m_ComposeStyle = m_Path_Label_Compose;
}

gui::CWidget* CGuiEditor::CreateMainWidget()
{
	m_pAssetsEditorWidget = new CMainWidget(this);
	return m_pAssetsEditorWidget;
}

CAssetPath CGuiEditor::GetItemIcon(const CAssetPath& AssetPath, const CSubPath& SubPath)
{
	CAssetPath IconPath = m_Path_Sprite_IconDefault;
	
	if(SubPath.IsNull())
	{
		//TAG_NEW_ASSET
		switch(AssetPath.GetType())
		{
			case CAsset_Image::TypeId:
				IconPath = m_Path_Sprite_IconImage;
				break;
			case CAsset_Sprite::TypeId:
				IconPath = m_Path_Sprite_IconSprite;
				break;
			case CAsset_Skeleton::TypeId:
				IconPath = m_Path_Sprite_IconSkeleton;
				break;
			case CAsset_SkeletonSkin::TypeId:
				IconPath = m_Path_Sprite_IconSkeletonSkin;
				break;
			case CAsset_SkeletonAnimation::TypeId:
				IconPath = m_Path_Sprite_IconSkeletonAnimation;
				break;
			case CAsset_Character::TypeId:
				IconPath = m_Path_Sprite_IconCharacter;
				break;
			case CAsset_CharacterPart::TypeId:
				IconPath = m_Path_Sprite_IconCharacterPart;
				break;
			case CAsset_Weapon::TypeId:
				IconPath = m_Path_Sprite_IconWeapon;
				break;
			case CAsset_MapZoneTiles::TypeId:
				IconPath = m_Path_Sprite_IconZoneTiles;
				break;
			case CAsset_Map::TypeId:
				IconPath = m_Path_Sprite_IconMap;
				break;
			case CAsset_MapGroup::TypeId:
				IconPath = m_Path_Sprite_IconFolder;
				break;
			case CAsset_MapLayerTiles::TypeId:
				IconPath = m_Path_Sprite_IconTiles;
				break;
			case CAsset_MapLayerQuads::TypeId:
				IconPath = m_Path_Sprite_IconQuad;
				break;
			case CAsset_ZoneType::TypeId:
				IconPath = m_Path_Sprite_IconZoneType;
				break;
			case CAsset_GuiRectStyle::TypeId:
				IconPath = m_Path_Sprite_IconGuiRect;
				break;
			case CAsset_GuiLineStyle::TypeId:
				IconPath = m_Path_Sprite_IconGuiLine;
				break;
			case CAsset_GuiBoxStyle::TypeId:
				IconPath = m_Path_Sprite_IconGuiBox;
				break;
			case CAsset_GuiIntEditStyle::TypeId:
				IconPath = m_Path_Sprite_IconGuiIntEdit;
				break;
			case CAsset_GuiLabelStyle::TypeId:
				IconPath = m_Path_Sprite_IconGuiLabel;
				break;
			case CAsset_GuiButtonStyle::TypeId:
				IconPath = m_Path_Sprite_IconGuiButton;
				break;
			case CAsset_GuiToggleStyle::TypeId:
				IconPath = m_Path_Sprite_IconGuiToggle;
				break;
			case CAsset_GuiSliderStyle::TypeId:
				IconPath = m_Path_Sprite_IconGuiSlider;
				break;
			case CAsset_GuiScrollbarStyle::TypeId:
				IconPath = m_Path_Sprite_IconGuiScrollbar;
				break;
			case CAsset_GuiTabsStyle::TypeId:
				IconPath = m_Path_Sprite_IconGuiTabs;
				break;
		}
	}
	
	return IconPath;
}

const char* CGuiEditor::GetItemName(const CAssetPath& AssetPath, const CSubPath& SubPath)
{
	if(SubPath.IsNull())
		return AssetsManager()->GetAssetValue<const char*>(AssetPath, SubPath, CAsset::NAME, NULL);
	else if(AssetPath.GetType() == CAsset_Character::TypeId)
		return AssetsManager()->GetAssetValue<const char*>(AssetPath, SubPath, CAsset_Character::PART_NAME, NULL);
	else
		return NULL;
}

void CGuiEditor::SetEditedPackage(int PackageId)
{
	m_EditedPackageId = PackageId;
	m_EditedAssetPath = CAssetPath::Null();
	m_EditedSubPath = CSubPath::Null();
}

void CGuiEditor::SetEditedAsset(const CAssetPath& Path, const CSubPath& SubPath)
{
	m_EditedAssetPath = Path;
	m_EditedSubPath = SubPath;
}

void CGuiEditor::RefreshAssetsTree()
{
	if(m_pAssetsTree)
		m_pAssetsTree->Refresh();
}
