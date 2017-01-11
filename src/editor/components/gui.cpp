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

#include <editor/components/gui.h>

#include <shared/components/assetsmanager.h>
#include <shared/components/storage.h>
#include <shared/components/cli.h>
#include <client/gui/panellayout.h>
#include <client/gui/popup.h>
#include <client/gui/filler.h>
#include <client/gui/text-edit.h>
#include <client/gui/toggle.h>
#include <client/gui/combobox.h>
#include <client/keys.h>
#include <client/components/input.h>
#include <client/gui/panellayout.h>
#include <client/loading_tools.h>
#include <editor/gui/assetslist.h>
#include <editor/gui/view.h>
#include <editor/gui/timeline.h>
#include <editor/gui/assetsinspector.h>

/* COMMANDS ***********************************************************/

class CSaveConfirmationDialog : public CConfirmationDialog
{
protected:
	virtual void OnConfirmation()
	{
		if(!m_pAssetsEditor->AssetsManager()->Save_AssetsFile(m_pAssetsEditor->GetEditedPackageId()))
			m_pAssetsEditor->DisplayPopup(new CErrorDialog(m_pAssetsEditor, _LSTRING("The package can't be saved")));
	}

public:
	CSaveConfirmationDialog(CGuiEditor* pAssetsEditor, const CLocalizableString& LString) :
		CConfirmationDialog(pAssetsEditor, LString)
	{ }
};

	//Echo
class CCommand_SavePackage : public CCommandLineInterpreter::CCommand
{
protected:
	CGuiEditor* m_pAssetsEditor;
	
public:
	CCommand_SavePackage(CGuiEditor* pAssetsEditor) :
		CCommandLineInterpreter::CCommand(),
		m_pAssetsEditor(pAssetsEditor)
	{ }

	virtual int Execute(const char* pArgs, CCLI_Output* pOutput)
	{
		dynamic_string Filename;
		if(!m_pAssetsEditor->AssetsManager()->GetPackageSaveFilename(m_pAssetsEditor->GetEditedPackageId(), Filename))
			return CLI_SUCCESS;
		
		int Type = CStorage::TYPE_SAVE;
			
		if(m_pAssetsEditor->Storage()->FileExists(Filename.buffer(), Type))
		{
			CLocalizableString LString(_("Do you want to overwrite \"{str:Filename}\"?"));
			LString.AddString("Filename", Filename.buffer());
			
			m_pAssetsEditor->DisplayPopup(new CSaveConfirmationDialog(m_pAssetsEditor, LString));
		}
		else
		{
			if(!m_pAssetsEditor->AssetsManager()->Save_AssetsFile(Filename.buffer(), Type, m_pAssetsEditor->GetEditedPackageId()))
				m_pAssetsEditor->DisplayPopup(new CErrorDialog(m_pAssetsEditor, _LSTRING("The package can't be saved")));
		}
		
		return CLI_SUCCESS;
	}
	
	virtual const char* Usage() { return "editor_save"; }
	virtual const char* Description() { return "Save the current package"; }
};

/* CONTEXT MENU *******************************************************/

CContextMenu::CContextMenu(CGuiEditor* pAssetsEditor) :
	gui::CPopup(pAssetsEditor, gui::CRect(pAssetsEditor->GetMousePos().x, pAssetsEditor->GetMousePos().y, 0, 0), 250, -1, gui::CPopup::ALIGNMENT_SIDE),
	m_pAssetsEditor(pAssetsEditor)
{		
	m_pList = new gui::CVListLayout(Context());
	m_pList->SetBoxStyle(m_pAssetsEditor->m_Path_Box_Menu);
	Add(m_pList);
}

class CPackageNameEntry : public gui::CAbstractTextEdit
{
protected:
	CGuiEditor* m_pAssetsEditor;
	int m_PackageId;

protected:
	virtual void SaveFromTextBuffer()
			{ AssetsManager()->SetPackageName(m_PackageId, GetText()); }
	
	virtual void CopyToTextBuffer()
		{
			const char* pName = AssetsManager()->GetPackageName(m_PackageId);
			if(m_Text != pName)
				SetText(pName);
			else if(!pName)
				SetText("");
		}

public:
	CPackageNameEntry(CGuiEditor* pAssetsEditor, int PackageId) :
		gui::CAbstractTextEdit(pAssetsEditor),
		m_PackageId(PackageId)
	{ }
};

class CPackageAuthorEntry : public gui::CAbstractTextEdit
{
protected:
	CGuiEditor* m_pAssetsEditor;
	int m_PackageId;

protected:
	virtual void SaveFromTextBuffer()
			{ AssetsManager()->SetPackageAuthor(m_PackageId, GetText()); }
	
	virtual void CopyToTextBuffer()
		{
			const char* pName = AssetsManager()->GetPackageAuthor(m_PackageId);
			if(m_Text != pName)
				SetText(pName);
			else if(!pName)
				SetText("");
		}

public:
	CPackageAuthorEntry(CGuiEditor* pAssetsEditor, int PackageId) :
		gui::CAbstractTextEdit(pAssetsEditor),
		m_PackageId(PackageId)
	{ }
};

class CPackageCreditsEntry : public gui::CAbstractTextEdit
{
protected:
	CGuiEditor* m_pAssetsEditor;
	int m_PackageId;

protected:
	virtual void SaveFromTextBuffer()
			{ AssetsManager()->SetPackageCredits(m_PackageId, GetText()); }
	
	virtual void CopyToTextBuffer()
		{
			const char* pName = AssetsManager()->GetPackageCredits(m_PackageId);
			if(m_Text != pName)
				SetText(pName);
			else if(!pName)
				SetText("");
		}

public:
	CPackageCreditsEntry(CGuiEditor* pAssetsEditor, int PackageId) :
		gui::CAbstractTextEdit(pAssetsEditor),
		m_PackageId(PackageId)
	{ }
};

class CPackageLicenseEntry : public gui::CAbstractTextEdit
{
protected:
	CGuiEditor* m_pAssetsEditor;
	int m_PackageId;

protected:
	virtual void SaveFromTextBuffer()
			{ AssetsManager()->SetPackageLicense(m_PackageId, GetText()); }
	
	virtual void CopyToTextBuffer()
		{
			const char* pName = AssetsManager()->GetPackageLicense(m_PackageId);
			if(m_Text != pName)
				SetText(pName);
			else if(!pName)
				SetText("");
		}

public:
	CPackageLicenseEntry(CGuiEditor* pAssetsEditor, int PackageId) :
		gui::CAbstractTextEdit(pAssetsEditor),
		m_PackageId(PackageId)
	{ }
};

class CPackageVersionEntry : public gui::CAbstractTextEdit
{
protected:
	CGuiEditor* m_pAssetsEditor;
	int m_PackageId;

protected:
	virtual void SaveFromTextBuffer()
			{ AssetsManager()->SetPackageVersion(m_PackageId, GetText()); }
	
	virtual void CopyToTextBuffer()
		{
			const char* pName = AssetsManager()->GetPackageVersion(m_PackageId);
			if(m_Text != pName)
				SetText(pName);
			else if(!pName)
				SetText("");
		}

public:
	CPackageVersionEntry(CGuiEditor* pAssetsEditor, int PackageId) :
		gui::CAbstractTextEdit(pAssetsEditor),
		m_PackageId(PackageId)
	{ }
};

CMessageDialog::CMessageDialog(CGuiEditor* pAssetsEditor) :
	gui::CPopup(pAssetsEditor, pAssetsEditor->GetDrawRect(), 400, 300, gui::CPopup::ALIGNMENT_INNER),
	m_pAssetsEditor(pAssetsEditor)
{
	
}

CConfirmationDialog::CConfirmationDialog(CGuiEditor* pAssetsEditor, const CLocalizableString& LString) :
	gui::CPopup(pAssetsEditor, pAssetsEditor->GetDrawRect(), 400, 300, gui::CPopup::ALIGNMENT_INNER),
	m_pAssetsEditor(pAssetsEditor)
{
	gui::CVScrollLayout* pLayout = new gui::CVScrollLayout(Context());
	pLayout->SetBoxStyle(m_pAssetsEditor->m_Path_Box_Dialog);
	Add(pLayout);
	
	pLayout->Add(new gui::CLabelHeader(Context(), _LSTRING("Confirmation")), false);
	
	gui::CLabel* pMessage = new gui::CLabel(Context(), LString);
	pMessage->SetLabelStyle(m_pAssetsEditor->m_Path_Label_DialogMessage);
	pMessage->NoTextClipping();
	pLayout->Add(pMessage, true);
	
	pLayout->AddSeparator();
	
	//Buttonlist
	{
		gui::CHListLayout* pHList = new gui::CHListLayout(Context());
		pLayout->Add(pHList, false);
		
		pHList->Add(new CCancel(this), false);
		pHList->Add(new gui::CFiller(Context()), true);
		pHList->Add(new CConfirm(this), false);
	}
}

PackagePropertiesDialog::PackagePropertiesDialog(CGuiEditor* pAssetsEditor, int PackageId) :
	CMessageDialog(pAssetsEditor),
	m_PackageId(PackageId)
{
	gui::CVScrollLayout* pLayout = new gui::CVScrollLayout(Context());
	pLayout->SetBoxStyle(m_pAssetsEditor->m_Path_Box_Dialog);
	Add(pLayout);
	
	pLayout->Add(new gui::CLabelHeader(Context(), _LSTRING("Package Properties")), false);
	pLayout->Add(new gui::CFiller(Context()), true);
	
	{
		gui::CHListLayout* pHList = new gui::CHListLayout(Context());
		pLayout->Add(pHList, false);
		
		pHList->Add(new gui::CLabel(Context(), _LSTRING("Name")), true);
		pHList->Add(new CPackageNameEntry(m_pAssetsEditor, m_PackageId), true);
	}
	{
		gui::CHListLayout* pHList = new gui::CHListLayout(Context());
		pLayout->Add(pHList, false);
		
		pHList->Add(new gui::CLabel(Context(), _LSTRING("Version")), true);
		pHList->Add(new CPackageVersionEntry(m_pAssetsEditor, m_PackageId), true);
	}
	{
		gui::CHListLayout* pHList = new gui::CHListLayout(Context());
		pLayout->Add(pHList, false);
		
		pHList->Add(new gui::CLabel(Context(), _LSTRING("Author")), true);
		pHList->Add(new CPackageAuthorEntry(m_pAssetsEditor, m_PackageId), true);
	}
	{
		gui::CHListLayout* pHList = new gui::CHListLayout(Context());
		pLayout->Add(pHList, false);
		
		pHList->Add(new gui::CLabel(Context(), _LSTRING("Credits")), true);
		pHList->Add(new CPackageCreditsEntry(m_pAssetsEditor, m_PackageId), true);
	}
	{
		gui::CHListLayout* pHList = new gui::CHListLayout(Context());
		pLayout->Add(pHList, false);
		
		pHList->Add(new gui::CLabel(Context(), _LSTRING("License")), true);
		pHList->Add(new CPackageLicenseEntry(m_pAssetsEditor, m_PackageId), true);
	}
	
	pLayout->Add(new gui::CFiller(Context()), true);
	pLayout->AddSeparator();
	
	//Buttonlist
	{
		gui::CHListLayout* pHList = new gui::CHListLayout(Context());
		pLayout->Add(pHList, false);
		
		pHList->Add(new gui::CFiller(Context()), true);
		pHList->Add(new CClose(this), false);
	}
}

CErrorDialog::CErrorDialog(CGuiEditor* pAssetsEditor, const CLocalizableString& LString) :
	CMessageDialog(pAssetsEditor)
{
	gui::CVScrollLayout* pLayout = new gui::CVScrollLayout(Context());
	pLayout->SetBoxStyle(m_pAssetsEditor->m_Path_Box_Dialog);
	Add(pLayout);
	
	pLayout->Add(new gui::CLabelHeader(Context(), _LSTRING("Error")), false);
	
	gui::CLabel* pMessage = new gui::CLabel(Context(), LString);
	pMessage->SetLabelStyle(m_pAssetsEditor->m_Path_Label_DialogMessage);
	pMessage->NoTextClipping();
	pLayout->Add(pMessage, true);
	
	pLayout->AddSeparator();
	
	//Buttonlist
	{
		gui::CHListLayout* pHList = new gui::CHListLayout(Context());
		pLayout->Add(pHList, false);
		
		pHList->Add(new gui::CFiller(Context()), true);
		pHList->Add(new CClose(this), false);
	}
}

class COpenSavePackageDialog : public gui::CPopup
{
public:
	enum
	{
		FORMAT_PACKAGE=0,
		FORMAT_MAP_TW,
		FORMAT_IMAGE,
	};
	
protected:
	class CItem_Directory : public gui::CButton
	{
	protected:
		COpenSavePackageDialog* m_pPopup;
		CGuiEditor* m_pAssetsEditor;
		dynamic_string m_Directory;
		
	protected:
		virtual void MouseClickAction()
		{
			m_pPopup->SelectDirectory(m_Directory.buffer());
		}
		
	public:
		CItem_Directory(COpenSavePackageDialog* pPopup, const char* pFilename, const char* pDir) :
			gui::CButton(pPopup->Context(), pFilename, pPopup->m_pAssetsEditor->m_Path_Sprite_IconFolder),
			m_pPopup(pPopup),
			m_pAssetsEditor(pPopup->m_pAssetsEditor)
		{
			m_Directory.copy(pDir);
			SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItem);
		}
		
		CItem_Directory(COpenSavePackageDialog* pPopup, const CLocalizableString& Title, const char* pDir) :
			gui::CButton(pPopup->Context(), Title, pPopup->m_pAssetsEditor->m_Path_Sprite_IconFolder),
			m_pPopup(pPopup),
			m_pAssetsEditor(pPopup->m_pAssetsEditor)
		{
			m_Directory.copy(pDir);
			SetButtonStyle(m_pAssetsEditor->m_Path_Button_ListItem);
		}
	};
	
	class CItem_Load : public gui::CButton
	{
	protected:
		COpenSavePackageDialog* m_pPopup;
		CGuiEditor* m_pAssetsEditor;
		dynamic_string m_Filename;
		
	protected:
		virtual void MouseClickAction()
		{
			m_pPopup->SelectName(m_Filename.buffer());
		}
		
	public:
		CItem_Load(COpenSavePackageDialog* pPopup, const char* pFilename) :
			gui::CButton(pPopup->Context(), pFilename),
			m_pPopup(pPopup),
			m_pAssetsEditor(pPopup->m_pAssetsEditor)
		{
			switch(m_pPopup->GetFormat())
			{
				case FORMAT_IMAGE:
					SetIcon(pPopup->m_pAssetsEditor->m_Path_Sprite_IconImage);
					break;
				case FORMAT_MAP_TW:
					SetIcon(pPopup->m_pAssetsEditor->m_Path_Sprite_IconMap);
					break;
				default:
					SetIcon(pPopup->m_pAssetsEditor->m_Path_Sprite_IconAsset);
					break;
			}
			m_Filename.copy(pFilename);
		}
		
		virtual void Update(bool ParentEnabled)
		{
			if(m_Filename == m_pPopup->m_Filename)
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
			gui::CButton(pPopup->Context(), _LSTRING("Open")),
			m_pPopup(pPopup)
		{
			SetButtonStyle(pPopup->m_pAssetsEditor->m_Path_Button_Dialog);
		}
		
		virtual void Update(bool ParentEnabled)
		{
			if(ParentEnabled)
			{
				if(m_pPopup->m_Filename.empty())
					Editable(false);
				else
					Editable(true);
			}
			
			gui::CButton::Update(ParentEnabled);
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
			gui::CButton(pPopup->Context(), _LSTRING("Save")),
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
			gui::CButton(pPopup->Context(), _LSTRING("Cancel")),
			m_pPopup(pPopup)
		{
			SetButtonStyle(pPopup->m_pAssetsEditor->m_Path_Button_Dialog);
		}
	};
	
	class CFormatToggle : public gui::CToggle
	{
	protected:
		int* m_pValueContainer;
		int m_Value;
		
	protected:
		virtual bool GetValue()
		{
			return (*m_pValueContainer == m_Value);
		}
		
		virtual void SetValue(bool Value)
		{
			*m_pValueContainer = m_Value;
		}
		
	public:
		CFormatToggle(CGui* pContext, const CLocalizableString& LString, int* pValueContainer, int Value) :
			gui::CToggle(pContext, LString),
			m_pValueContainer(pValueContainer),
			m_Value(Value)
		{
			
		}
	};

protected:
	CGuiEditor* m_pAssetsEditor;
	dynamic_string m_Filename;
	dynamic_string m_Directory;
	gui::CVScrollLayout* m_pFilelist;
	int m_Format;
	bool m_Save;
	bool m_RefreshList;
	int m_CompatibilityMode;
	
public:
	COpenSavePackageDialog(CGuiEditor* pAssetsEditor, bool Save, int Format) :
		gui::CPopup(pAssetsEditor, pAssetsEditor->GetDrawRect(), 600, 450, gui::CPopup::ALIGNMENT_INNER),
		m_pAssetsEditor(pAssetsEditor),
		m_pFilelist(NULL),
		m_Format(Format),
		m_Save(Save),
		m_RefreshList(true),
		m_CompatibilityMode(CAssetsManager::MAPFORMAT_DDNET)
	{
		gui::CVScrollLayout* pLayout = new gui::CVScrollLayout(Context());
		pLayout->SetBoxStyle(m_pAssetsEditor->m_Path_Box_Dialog);
		Add(pLayout);
		
		switch(m_Format)
		{
			case FORMAT_IMAGE:
				if(m_Save)
					pLayout->Add(new gui::CLabelHeader(Context(), _LSTRING("Export Image")), false);
				else
					pLayout->Add(new gui::CLabelHeader(Context(), _LSTRING("Import Image")), false);
				break;
			case FORMAT_MAP_TW:
				if(m_Save)
					pLayout->Add(new gui::CLabelHeader(Context(), _LSTRING("Export TeeWorlds Map")), false);
				else
					pLayout->Add(new gui::CLabelHeader(Context(), _LSTRING("Import TeeWorlds Map")), false);
				break;
			default:
				if(m_Save)
					pLayout->Add(new gui::CLabelHeader(Context(), _LSTRING("Save Package")), false);
				else
					pLayout->Add(new gui::CLabelHeader(Context(), _LSTRING("Open Package")), false);
				break;
		}
		
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pLayout->Add(pHList, false);
			
			gui::CLabel* pLabel = new gui::CLabel(Context(), _LSTRING("Directory"));
			pLabel->NoTextClipping();
			pHList->Add(pLabel, false);
			pHList->Add(new gui::CExternalTextEdit_DynamicString(Context(), &m_Directory), true);
			
		}
		
		{
			gui::CHPanelLayout* pPanel = new gui::CHPanelLayout(Context());
			pLayout->Add(pPanel, true);
			
			gui::CVScrollLayout* pPlaces = new gui::CVScrollLayout(Context());
			pPanel->Add(pPlaces, 200);
			pPlaces->SetBoxStyle(m_pAssetsEditor->m_Path_Box_SubList);
			
			dynamic_string Buffer;
			{
				Buffer.clear();
				Storage()->GetCompletePath(CStorage::TYPE_SAVE, "assets", Buffer);
				pPlaces->Add(new CItem_Directory(this, _LSTRING("My packages"), Buffer.buffer()), false);			
			}
			{
				Buffer.clear();
				Storage()->GetCompletePath(CStorage::TYPE_DATA, "assets", Buffer);
				pPlaces->Add(new CItem_Directory(this, _LSTRING("Default packages"), Buffer.buffer()), false);			
			}
			for(int i=2; i<Storage()->GetNumPaths(); i++)
			{
				Buffer.clear();
				Storage()->GetCompletePath(i, "assets", Buffer);
				
				CLocalizableString LString(_("Alternative Data Directory {int:Id}"));
				LString.AddInteger("Id", i-1);
				
				pPlaces->Add(new CItem_Directory(this, LString, Buffer.buffer()), false);
			}
			pPlaces->AddSeparator();
			{
				Buffer.clear();
				fs_storage_path("teeworlds", Buffer);
				Buffer.append("/maps");
				pPlaces->Add(new CItem_Directory(this, _LSTRING("TeeWorlds Maps"), Buffer.buffer()), false);
			}
			{
				Buffer.clear();
				fs_storage_path("teeworlds", Buffer);
				Buffer.append("/downloadedmaps");
				pPlaces->Add(new CItem_Directory(this, _LSTRING("TeeWorlds Downloaded Maps"), Buffer.buffer()), false);
			}
			
			m_pFilelist = new gui::CVScrollLayout(Context());
			m_pFilelist->SetBoxStyle(m_pAssetsEditor->m_Path_Box_SubList);
			pPanel->Add(m_pFilelist, -1);
		}
		
		//Filename
		if(m_Save)
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pLayout->Add(pHList, false);
			
			gui::CLabel* pLabel = new gui::CLabel(Context(), _LSTRING("Filename"));
			pLabel->NoTextClipping();
			pHList->Add(pLabel, false);
			pHList->Add(new gui::CExternalTextEdit_DynamicString(Context(), &m_Filename), true);
			
		}
		
		pLayout->AddSeparator();
		
		if(!m_Save && m_Format == FORMAT_MAP_TW)
		{
			gui::CHListLayout* pHList = new gui::CHListLayout(Context());
			pLayout->Add(pHList, false);
			
			gui::CLabel* pLabel = new gui::CLabel(Context(), _LSTRING("Compatibility mode:"));
			pLabel->NoTextClipping();
			pHList->Add(pLabel, false);
			
			pHList->Add(new CFormatToggle(Context(), _LSTRING("TeeWorlds"), &m_CompatibilityMode, CAssetsManager::MAPFORMAT_TW), true);
			pHList->Add(new CFormatToggle(Context(), _LSTRING("DDNet"), &m_CompatibilityMode, CAssetsManager::MAPFORMAT_DDNET), true);
			pHList->Add(new CFormatToggle(Context(), _LSTRING("OpenFNG"), &m_CompatibilityMode, CAssetsManager::MAPFORMAT_OPENFNG), true);
		
			pLayout->AddSeparator();
		}
		
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
		
		switch(m_Format)
		{
			case FORMAT_IMAGE:
				SelectDirectory("images", CStorage::TYPE_SAVE);
				break;
			case FORMAT_MAP_TW:
				SelectDirectory("maps", CStorage::TYPE_SAVE);
				break;
			default:
				SelectDirectory("assets", CStorage::TYPE_SAVE);
				break;
		}
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(m_RefreshList)
		{
			ListFiles();
			m_RefreshList = false;
		}
		
		gui::CPopup::Update(ParentEnabled);
	}
	
	void ListFiles()
	{
		m_pFilelist->Clear();
		
		dynamic_string Buffer;
		
		fs_listdir_iterator* pIter = fs_create_listdir_iterator(m_Directory.buffer());
		int PathSize = str_length(m_Directory.buffer());
		
		if(!pIter)
			return;
		
		sorted_array< dynamic_string, allocator_copy<dynamic_string> > Directories;
		sorted_array< dynamic_string, allocator_copy<dynamic_string> > Files;
		bool ParentFolder = false;
		
		while(pIter->next())
		{
			const char* pFilename = pIter->get_filename();
			
			if(str_length(pFilename) > PathSize+1 && str_comp_num(pFilename, m_Directory.buffer(), PathSize) == 0)
				Buffer.copy(pFilename + PathSize +1);
			else
				Buffer.copy(pFilename);
			
			int Length = Buffer.length();
			if(Length > 0 && Buffer.buffer()[0] == '.')
			{
				if(Buffer.buffer()[1] == 0)
					continue;
				if(Length > 1 && Buffer.buffer()[1] == '.' && (Buffer.buffer()[2] == 0))
				{
					ParentFolder = true;
					continue;
				}
			}
			
			if(fs_is_dir(pFilename))
				Directories.add_by_copy(pFilename);
			else
			{
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
						if(Length >= 4 && str_comp(Buffer.buffer()+Length-4, ".map") == 0)
						{
							Found = true;
							Buffer.buffer()[Length-4] = 0;
						}
						break;
					case FORMAT_PACKAGE:
						if(Length >= 4 && str_comp(Buffer.buffer()+Length-4, ".tup") == 0)
						{
							Found = true;
							Buffer.buffer()[Length-4] = 0;
						}
						break;
				}
				
				if(Found)
					Files.add_by_copy(Buffer);
			}
		}
		
		if(ParentFolder)
		{
			int i=m_Directory.length()-1;
			for(; i>=0; i--)
				if(m_Directory.buffer()[i] != '/' && m_Directory.buffer()[i] != '\\')
					break;
			for(; i>=0; i--)
				if(m_Directory.buffer()[i] == '/' || m_Directory.buffer()[i] == '\\')
					break;
			for(; i>=0; i--)
				if(m_Directory.buffer()[i] != '/' && m_Directory.buffer()[i] != '\\')
					break;
				
			if(i>=0)
			{
				Buffer.clear();
				Buffer.append_at_num(0, m_Directory.buffer(), i+1);
				m_pFilelist->Add(new CItem_Directory(this, _LSTRING("Parent Directory"), Buffer.buffer()), false);
			}
		}
		for(int i=0; i<Directories.size(); i++)
		{
			const char* pName = Directories[i].buffer();
			if(Directories[i].length() > PathSize+1 && Directories[i].comp_num(m_Directory, PathSize) == 0)
				pName += PathSize+1;
			
			m_pFilelist->Add(new CItem_Directory(this, pName, Directories[i].buffer()), false);
		}
		for(int i=0; i<Files.size(); i++)
			m_pFilelist->Add(new CItem_Load(this, Files[i].buffer()), false);
		
		delete pIter;
	}
	
	int GetFormat() const
	{
		return m_Format;
	}
	
	void SelectName(const char* pFilename)
	{
		m_Filename.copy(pFilename);
	}
	
	void SelectDirectory(const char* pDirectory)
	{
		m_Directory.copy(pDirectory);
		m_RefreshList = true;
	}
	
	void SelectDirectory(const char* pDirectory, int StorageType)
	{
		Storage()->GetCompletePath(StorageType, pDirectory, m_Directory);
		m_RefreshList = true;
	}
	
	void Save()
	{
		Context()->ShowLoadingCursor();
		dynamic_string Buffer;
		int TextIter = 0;
		switch(m_Format)
		{
			case FORMAT_MAP_TW:
			{
				TextIter = Buffer.append_at(TextIter, m_Directory.buffer());
				TextIter = Buffer.append_at(TextIter, "/");
				TextIter = Buffer.append_at(TextIter, m_Filename.buffer());
				TextIter = Buffer.append_at(TextIter, ".map");
				if(!AssetsManager()->Save_Map(Buffer.buffer(), CStorage::TYPE_ABSOLUTE, m_pAssetsEditor->GetEditedPackageId(), CAssetsManager::MAPFORMAT_TW))
				{
					m_pAssetsEditor->DisplayPopup(new CErrorDialog(m_pAssetsEditor, _LSTRING("The map can't be saved")));
				}
				break;
			}
		}
			
		m_pAssetsEditor->RefreshPackageTree();
		Close();
	}
	
	void Open()
	{
		Context()->ShowLoadingCursor();
		dynamic_string Buffer;
		int TextIter = 0;
		switch(m_Format)
		{
			case FORMAT_IMAGE:
			{
				TextIter = Buffer.append_at(TextIter, m_Directory.buffer());
				TextIter = Buffer.append_at(TextIter, "/");
				TextIter = Buffer.append_at(TextIter, m_Filename.buffer());
				TextIter = Buffer.append_at(TextIter, ".png");
				
				CAssetPath ImagePath = CreateNewImage(
					m_pAssetsEditor->SharedKernel(),
					m_pAssetsEditor->GetEditedPackageId(),
					m_Filename.buffer(),
					Buffer.buffer(),
					CStorage::TYPE_ABSOLUTE,
					-1, -1
				);
				if(ImagePath.IsNull())
					m_pAssetsEditor->DisplayPopup(new CErrorDialog(m_pAssetsEditor, _LSTRING("The image can't be loaded")));
				else
				{
					m_pAssetsEditor->SetEditedAsset(ImagePath, CSubPath::Null());
					m_pAssetsEditor->RefreshAssetsTree();
				}
				
				break;
			}
			case FORMAT_MAP_TW:
			{
				TextIter = Buffer.append_at(TextIter, m_Directory.buffer());
				TextIter = Buffer.append_at(TextIter, "/");
				TextIter = Buffer.append_at(TextIter, m_Filename.buffer());
				TextIter = Buffer.append_at(TextIter, ".map");
				int PackageId = AssetsManager()->Load_Map(Buffer.buffer(), CStorage::TYPE_ABSOLUTE, m_CompatibilityMode);
				if(PackageId < 0)
					m_pAssetsEditor->DisplayPopup(new CErrorDialog(m_pAssetsEditor, _LSTRING("The map can't be imported")));
				else
				{
					m_pAssetsEditor->SetEditedPackage(PackageId);
		
					//Search for maps
					if(AssetsManager()->GetNumAssets<CAsset_Map>(PackageId))
						m_pAssetsEditor->SetEditedAsset(CAssetPath(CAsset_Map::TypeId, PackageId, 0), CSubPath::Null());
					
					AssetsManager()->SetPackageReadOnly(PackageId, false);
					m_pAssetsEditor->RefreshAssetsTree();
				}
				break;
			}
			case FORMAT_PACKAGE:
			{
				TextIter = Buffer.append_at(TextIter, m_Directory.buffer());
				TextIter = Buffer.append_at(TextIter, "/");
				TextIter = Buffer.append_at(TextIter, m_Filename.buffer());
				TextIter = Buffer.append_at(TextIter, ".tup");
				int PackageId = AssetsManager()->Load_AssetsFile(Buffer.buffer(), CStorage::TYPE_ABSOLUTE);
				if(PackageId < 0)
					m_pAssetsEditor->DisplayPopup(new CErrorDialog(m_pAssetsEditor, _LSTRING("The package can't be loaded")));
				else
				{
					m_pAssetsEditor->SetEditedPackage(PackageId);
					
					//Search for maps
					if(AssetsManager()->GetNumAssets<CAsset_Map>(PackageId))
						m_pAssetsEditor->SetEditedAsset(CAssetPath(CAsset_Map::TypeId, PackageId, 0), CSubPath::Null());
					
					AssetsManager()->SetPackageReadOnly(PackageId, false);
					m_pAssetsEditor->RefreshAssetsTree();
				}
				break;
			}
		}
			
		m_pAssetsEditor->RefreshPackageTree();
		Close();
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

/* MENU ***************************************************************/

class CMenuBar : public gui::CHListLayout
{
class CPopup_Menu* m_pPopup;

public:
	CMenuBar(CGuiEditor* pAssetsEditor) :
		gui::CHListLayout(pAssetsEditor),
		m_pPopup(NULL)
	{
		
	}
};

class CPopup_Menu : public gui::CPopup
{	
protected:
	CGuiEditor* m_pAssetsEditor;
	char m_aFilename[256];
	int m_PackageId;
	int m_Mode;
	gui::CVListLayout* m_pList;
	
public:
	CPopup_Menu(CGuiEditor* pAssetsEditor, const gui::CRect& CreatorRect) :
		gui::CPopup(pAssetsEditor, CreatorRect, 250, -1, gui::CPopup::ALIGNMENT_BOTTOM),
		m_pAssetsEditor(pAssetsEditor)
	{		
		m_pList = new gui::CVListLayout(Context());
		m_pList->SetBoxStyle(m_pAssetsEditor->m_Path_Box_Menu);
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
	CPopup_Menu* m_pPopupMenu;
	
protected:
	virtual void MouseClickAction()
	{
		int PackageId = AssetsManager()->NewPackage("mypackage");
		AssetsManager()->SetPackageReadOnly(PackageId, false);
		AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
		m_pAssetsEditor->SetEditedPackage(PackageId);
		m_pAssetsEditor->RefreshPackageTree();
		m_pAssetsEditor->RefreshAssetsTree();
		
		m_pAssetsEditor->DisplayPopup(new PackagePropertiesDialog(m_pAssetsEditor, PackageId));
		
		m_pPopupMenu->Close();
	}

public:
	CNewPackageButton(CGuiEditor* pAssetsEditor, CPopup_Menu* pPopupMenu) :
		gui::CButton(pAssetsEditor, _LSTRING("New Package")),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
		SetIcon(m_pAssetsEditor->m_Path_Sprite_IconNewFolder);
	}
};

class COpenPackageButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_Menu* m_pPopupMenu;
	
protected:
	virtual void MouseClickAction()
	{
		m_pAssetsEditor->DisplayPopup(new COpenSavePackageDialog(m_pAssetsEditor, false, COpenSavePackageDialog::FORMAT_PACKAGE));
		m_pPopupMenu->Close();
	}

public:
	COpenPackageButton(CGuiEditor* pAssetsEditor, CPopup_Menu* pPopupMenu) :
		gui::CButton(pAssetsEditor, _LSTRING("Open Package")),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
		SetIcon(m_pAssetsEditor->m_Path_Sprite_IconLoad);
	}
};

class CImportButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_Menu* m_pPopupMenu;
	int m_Format;
	
protected:
	virtual void MouseClickAction()
	{
		m_pAssetsEditor->DisplayPopup(new COpenSavePackageDialog(m_pAssetsEditor, false, m_Format));
		m_pPopupMenu->Close();
	}

public:
	CImportButton(CGuiEditor* pAssetsEditor, CPopup_Menu* pPopupMenu, int Format) :
		gui::CButton(pAssetsEditor, ""),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu),
		m_Format(Format)
	{
		switch(m_Format)
		{
			case COpenSavePackageDialog::FORMAT_IMAGE:
				SetText(_LSTRING("Import Image"));
				break;
			case COpenSavePackageDialog::FORMAT_MAP_TW:
				SetText(_LSTRING("Import TeeWorlds Map"));
				break;
		}
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
		SetIcon(m_pAssetsEditor->m_Path_Sprite_IconLoad);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(m_Format == COpenSavePackageDialog::FORMAT_IMAGE)
		{
			if(!AssetsManager()->IsValidPackage(m_pAssetsEditor->GetEditedPackageId()) || AssetsManager()->IsReadOnlyPackage(m_pAssetsEditor->GetEditedPackageId()))
				Editable(false);
			else
				Editable(true);
		}
		
		gui::CButton::Update(ParentEnabled);
	}
};

class CExportButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_Menu* m_pPopupMenu;
	int m_Format;
	
protected:
	virtual void MouseClickAction()
	{
		m_pAssetsEditor->DisplayPopup(new COpenSavePackageDialog(m_pAssetsEditor, true, m_Format));
		m_pPopupMenu->Close();
	}

public:
	CExportButton(CGuiEditor* pAssetsEditor, CPopup_Menu* pPopupMenu, int Format) :
		gui::CButton(pAssetsEditor, ""),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu),
		m_Format(Format)
	{
		switch(m_Format)
		{
			case COpenSavePackageDialog::FORMAT_MAP_TW:
				SetText(_LSTRING("Export TeeWorlds Map"));
				break;
		}
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
		SetIcon(m_pAssetsEditor->m_Path_Sprite_IconSave);
	}
};

class CSavePackageButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_Menu* m_pPopupMenu;
	
protected:
	virtual void MouseClickAction()
	{
		dynamic_string Filename;
		if(AssetsManager()->GetPackageSaveFilename(m_pAssetsEditor->GetEditedPackageId(), Filename))
		{
			CLI()->Execute("editor_save");
		}
		
		m_pPopupMenu->Close();
	}

public:
	CSavePackageButton(CGuiEditor* pAssetsEditor, CPopup_Menu* pPopupMenu) :
		gui::CButton(pAssetsEditor, _LSTRING("Save Package")),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
		SetIcon(m_pAssetsEditor->m_Path_Sprite_IconSave);
	}
};

class CQuitButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_Menu* m_pPopupMenu;
	
protected:
	virtual void MouseClickAction()
	{
		m_pAssetsEditor->Quit();
		m_pPopupMenu->Close();
	}

public:
	CQuitButton(CGuiEditor* pAssetsEditor, CPopup_Menu* pPopupMenu) :
		gui::CButton(pAssetsEditor, _LSTRING("Quit")),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
		SetIcon(m_pAssetsEditor->m_Path_Sprite_IconDelete);
	}
};

class CUndoButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_Menu* m_pPopupMenu;
	
protected:
	virtual void MouseClickAction()
	{
		AssetsManager()->Undo();
		m_pAssetsEditor->RefreshPackageTree();
		m_pAssetsEditor->RefreshAssetsTree();
	}

public:
	CUndoButton(CGuiEditor* pAssetsEditor, CPopup_Menu* pPopupMenu) :
		gui::CButton(pAssetsEditor, _LSTRING("Undo")),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
		SetIcon(m_pAssetsEditor->m_Path_Sprite_IconErase);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(AssetsManager()->GetHistorySize())
			Editable(true);
		else
			Editable(false);
		
		gui::CButton::Update(ParentEnabled);
	}
};

class CNewAsset : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_Menu* m_pPopupMenu;
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
				if(!pMap)
					break;
				AssetsManager()->TryChangeAssetName(AssetPath, "map", Tokken);
				pMap->SetCameraPosition(vec2(64*32.0f, 64*32.0f)/2.0f);
				
				CAssetPath GroupPath;
				CAssetPath SubAssetPath;
				CAsset_MapGroup* pMapGroup;
				CAsset_MapLayerQuads* pMapLayerQuads;
				CAsset_MapZoneTiles* pMapZoneTiles;
				CAsset_MapEntities* pMapEntities;
				CSubPath SubPath;
				
				//Zone, Physics
				pMapZoneTiles = AssetsManager()->NewAsset<CAsset_MapZoneTiles>(&SubAssetPath, m_pAssetsEditor->GetEditedPackageId(), Tokken);
				AssetsManager()->TryChangeAssetName(SubAssetPath, "teeworlds", Tokken);
				SubPath = CAsset_Map::SubPath_ZoneLayer(pMap->AddZoneLayer());
				pMap->SetZoneLayer(SubPath, SubAssetPath);
				
				{
					array2d< CAsset_MapZoneTiles::CTile, allocator_copy<CAsset_MapZoneTiles::CTile> >& Data = pMapZoneTiles->GetTileArray();
					Data.resize(64, 64);
					
					pMapZoneTiles->SetZoneTypePath(AssetsManager()->m_Path_ZoneType_TeeWorlds);
					pMapZoneTiles->SetParentPath(AssetPath);
				}
				
				//Entites
				pMapEntities = AssetsManager()->NewAsset<CAsset_MapEntities>(&SubAssetPath, m_pAssetsEditor->GetEditedPackageId(), Tokken);
				AssetsManager()->TryChangeAssetName(SubAssetPath, "entities", Tokken);
				SubPath = CAsset_Map::SubPath_EntityLayer(pMap->AddEntityLayer());
				pMap->SetEntityLayer(SubPath, SubAssetPath);
				pMapEntities->SetParentPath(AssetPath);
				
				//Background
				pMapGroup = AssetsManager()->NewAsset<CAsset_MapGroup>(&GroupPath, m_pAssetsEditor->GetEditedPackageId(), Tokken);
				AssetsManager()->TryChangeAssetName(GroupPath, "background", Tokken);
				SubPath = CAsset_Map::SubPath_BgGroup(pMap->AddBgGroup());
				pMap->SetBgGroup(SubPath, GroupPath);
				pMapGroup->SetHardParallax(vec2(0.0f, 0.0f));
				pMapGroup->SetParentPath(AssetPath);
				
					//Sky
				pMapLayerQuads = AssetsManager()->NewAsset<CAsset_MapLayerQuads>(&SubAssetPath, m_pAssetsEditor->GetEditedPackageId(), Tokken);
				AssetsManager()->TryChangeAssetName(SubAssetPath, "sky", Tokken);
				SubPath = CAsset_MapGroup::SubPath_Layer(pMapGroup->AddLayer());
				pMapGroup->SetLayer(SubPath, SubAssetPath);
				pMapLayerQuads->SetParentPath(GroupPath);
				
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
			case CAsset_Material::TypeId:
			{
				int Tokken = AssetsManager()->GenerateToken();
				CAsset_Material* pMaterial = AssetsManager()->NewAsset<CAsset_Material>(&AssetPath, m_pAssetsEditor->GetEditedPackageId(), Tokken);
				if(!pMaterial)
					break;
				AssetsManager()->TryChangeAssetName(AssetPath, "material", Tokken);
				break;
			}
		}
		
		m_pAssetsEditor->SetEditedAsset(AssetPath, CSubPath::Null());
		m_pAssetsEditor->RefreshPackageTree();
		m_pAssetsEditor->RefreshAssetsTree();
		
		m_pPopupMenu->Close();
	}

public:
	CNewAsset(CGuiEditor* pAssetsEditor, CPopup_Menu* pPopupMenu, int AssetType, const CLocalizableString& Text) :
		gui::CButton(pAssetsEditor, Text),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu),
		m_AssetType(AssetType)
	{
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
		
		switch(m_AssetType)
		{
			case CAsset_Map::TypeId:
				SetIcon(m_pAssetsEditor->m_Path_Sprite_IconMap);
				break;
			case CAsset_Material::TypeId:
				SetIcon(m_pAssetsEditor->m_Path_Sprite_IconLineStyle);
				break;
		}
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(!AssetsManager()->IsValidPackage(m_pAssetsEditor->GetEditedPackageId()) || AssetsManager()->IsReadOnlyPackage(m_pAssetsEditor->GetEditedPackageId()))
			Editable(false);
		else
			Editable(true);
		
		gui::CButton::Update(ParentEnabled);
	}
};

class CToggleFullscreen : public gui::CToggle
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CPopup_Menu* m_pPopupMenu;
	
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
	CToggleFullscreen(CGuiEditor* pAssetsEditor, CPopup_Menu* pPopupMenu) :
		gui::CToggle(pAssetsEditor, _LSTRING("Fullscreen")),
		m_pAssetsEditor(pAssetsEditor),
		m_pPopupMenu(pPopupMenu)
	{
		SetToggleStyle(m_pAssetsEditor->m_Path_Toggle_Default);
	}
};

class CFileButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	
protected:
	virtual void MouseClickAction()
	{
		CPopup_Menu* pMenu = new CPopup_Menu(m_pAssetsEditor, m_DrawRect);
		pMenu->List()->Add(new CNewPackageButton(m_pAssetsEditor, pMenu));
		pMenu->List()->Add(new COpenPackageButton(m_pAssetsEditor, pMenu));
		pMenu->List()->Add(new CSavePackageButton(m_pAssetsEditor, pMenu));
		pMenu->List()->AddSeparator();
		pMenu->List()->Add(new CImportButton(m_pAssetsEditor, pMenu, COpenSavePackageDialog::FORMAT_MAP_TW));
		pMenu->List()->Add(new CExportButton(m_pAssetsEditor, pMenu, COpenSavePackageDialog::FORMAT_MAP_TW));
		pMenu->List()->AddSeparator();
		pMenu->List()->Add(new CQuitButton(m_pAssetsEditor, pMenu));
		
		m_pAssetsEditor->DisplayPopup(pMenu);
	}

public:
	CFileButton(CGuiEditor* pAssetsEditor) :
		gui::CButton(pAssetsEditor, _LSTRING("File")),
		m_pAssetsEditor(pAssetsEditor)
	{
		NoTextClipping();
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
	}
};

class CEditButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	
protected:
	virtual void MouseClickAction()
	{
		CPopup_Menu* pMenu = new CPopup_Menu(m_pAssetsEditor, m_DrawRect);
		
		pMenu->List()->Add(new CUndoButton(m_pAssetsEditor, pMenu));
		pMenu->List()->AddSeparator();
		pMenu->List()->Add(new CNewAsset(m_pAssetsEditor, pMenu, CAsset_Map::TypeId, _LSTRING("New Map")));
		pMenu->List()->AddSeparator();
		pMenu->List()->Add(new CImportButton(m_pAssetsEditor, pMenu, COpenSavePackageDialog::FORMAT_IMAGE));
		pMenu->List()->Add(new CNewAsset(m_pAssetsEditor, pMenu, CAsset_Material::TypeId, _LSTRING("New Material")));
		
		m_pAssetsEditor->DisplayPopup(pMenu);
	}

public:
	CEditButton(CGuiEditor* pAssetsEditor) :
		gui::CButton(pAssetsEditor, _LSTRING("Edit")),
		m_pAssetsEditor(pAssetsEditor)
	{
		NoTextClipping();
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
	}
};

class CViewButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	
protected:
	virtual void MouseClickAction()
	{
		CPopup_Menu* pMenu = new CPopup_Menu(m_pAssetsEditor, m_DrawRect);
		
		pMenu->List()->Add(new CToggleFullscreen(m_pAssetsEditor, pMenu));
		
		m_pAssetsEditor->DisplayPopup(pMenu);
	}

public:
	CViewButton(CGuiEditor* pAssetsEditor) :
		gui::CButton(pAssetsEditor, _LSTRING("View")),
		m_pAssetsEditor(pAssetsEditor)
	{
		NoTextClipping();
		SetButtonStyle(m_pAssetsEditor->m_Path_Button_Menu);
	}
};

/* MAIN WIDGET ********************************************************/

CGuiEditor::CMainWidget::CMainWidget(CGuiEditor* pAssetsEditor) :
	gui::CVListLayout(pAssetsEditor),
	m_pAssetsEditor(pAssetsEditor)
{
	//Toolbar
	Add(CreateToolbar(), false);
	
	//Panel
	gui::CHPanelLayout* pPanel = new gui::CHPanelLayout(pAssetsEditor);
	Add(pPanel, true);
	
	gui::CVPanelLayout* pVPanel = new gui::CVPanelLayout(pAssetsEditor);
	
	pPanel->Add(new CAssetsOrganizer(pAssetsEditor), 280);
	
	pVPanel->Add(new CViewManager(pAssetsEditor), -1);
	//pVPanel->Add(new CTimeLine(pAssetsEditor), 200);
	pPanel->Add(pVPanel, -1);
	
	pPanel->Add(new CAssetsInspector(pAssetsEditor), 280);
	
	//Toolbar
	Add(CreateStatusbar(), false);
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
	if(Input()->KeyIsPressed(KEY_LCTRL) && Event.m_Key == KEY_Z && (Event.m_Flags & CInput::FLAG_RELEASE))
	{
		AssetsManager()->Undo();
		m_pAssetsEditor->RefreshPackageTree();
		m_pAssetsEditor->RefreshAssetsTree();
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
	if(Input()->KeyIsPressed(KEY_LCTRL) && Event.m_Key == KEY_S && (Event.m_Flags & CInput::FLAG_RELEASE))
	{
		CLI()->Execute("editor_save");
		return;
	}
	
	gui::CVListLayout::OnInputEvent(Event);
}

gui::CWidget* CGuiEditor::CMainWidget::CreateToolbar()
{
	CMenuBar* pMenuBar = new CMenuBar(m_pAssetsEditor);
	
	pMenuBar->Add(new CFileButton(m_pAssetsEditor), false);
	pMenuBar->Add(new CEditButton(m_pAssetsEditor), false);
	pMenuBar->Add(new CViewButton(m_pAssetsEditor), false);
	
	return pMenuBar;
}

gui::CWidget* CGuiEditor::CMainWidget::CreateStatusbar()
{
	gui::CHListLayout* pStatusBar = new gui::CHListLayout(Context());
	pStatusBar->SetBoxStyle(m_pAssetsEditor->m_Path_Box_Statusbar);
	
	m_pAssetsEditor->m_pCoordinatesLabel = new gui::CLabel(Context(), "");
	m_pAssetsEditor->m_pCoordinatesLabel->NoTextClipping();
	pStatusBar->Add(m_pAssetsEditor->m_pCoordinatesLabel, false);
	
	pStatusBar->Add(new gui::CFiller(Context()), true);
	
	m_pAssetsEditor->m_pHintLabel = new gui::CLabel(Context(), "");
	m_pAssetsEditor->m_pHintLabel->NoTextClipping();
	pStatusBar->Add(m_pAssetsEditor->m_pHintLabel, false);
	
	return pStatusBar;
}


/* CONTEXT ************************************************************/

CGuiEditor::CGuiEditor(CEditorKernel* pEditorKernel) :
	CGui(pEditorKernel),
	m_pEditorKernel(pEditorKernel),
	m_EditedSubPath(CSubPath::Null()),
	m_pAssetsTree(NULL),
	m_pPackagesTree(NULL),
	m_pHintLabel(NULL),
	m_pCoordinatesLabel(NULL)
{
	SetName("GuiEditor");
}

CGuiEditor::~CGuiEditor()
{
	
}

bool CGuiEditor::InitConfig(int argc, const char** argv)
{
	if(!CGui::InitConfig(argc, argv))
		return false;
	
	CLI()->Register("editor_save", new CCommand_SavePackage(this));
	
	return true;
}

void CGuiEditor::LoadAssets()
{
	int PackageId;
	
	PackageId = AssetsManager()->Load_AssetsFile("gui_editor", CStorage::TYPE_ALL);
	if(PackageId >= 0)
	{
		m_Path_Rect_TextSelection = AssetsManager()->FindAsset<CAsset_GuiRectStyle>(PackageId, "textSelection");
		m_Path_Rect_Selection = AssetsManager()->FindAsset<CAsset_GuiRectStyle>(PackageId, "selection");
		m_Path_Rect_Border = AssetsManager()->FindAsset<CAsset_GuiRectStyle>(PackageId, "border");
		m_Path_Rect_GuiBox = AssetsManager()->FindAsset<CAsset_GuiRectStyle>(PackageId, "guiBox");
		
		m_Path_Line_Separator = AssetsManager()->FindAsset<CAsset_GuiLineStyle>(PackageId, "separator");
		
		m_Path_Box_View = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "view");
		m_Path_Box_Panel = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "panel");
		m_Path_Box_Menu = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "menu");
		m_Path_Box_Dialog = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "dialog");
		m_Path_Box_SubList = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "subList");
		m_Path_Box_Statusbar = AssetsManager()->FindAsset<CAsset_GuiBoxStyle>(PackageId, "statusbar");
		
		m_Path_Label_Text = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "text");
		m_Path_Label_Header = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "header");
		m_Path_Label_DialogMessage = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "dialogMessage");
		m_Path_Label_Compose = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "compose");
		m_Path_Label_InactiveListItem = AssetsManager()->FindAsset<CAsset_GuiLabelStyle>(PackageId, "inactiveListItem");
		
		m_Path_Button_Default = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "default");
		m_Path_Button_Dialog = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "dialog");
		m_Path_Button_TextEdit = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "textEdit");
		m_Path_Button_NumericEdit = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "numericEdit");
		m_Path_Button_ListItem = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "listItem");
		m_Path_Button_ListItemHL = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "listItemHL");
		m_Path_Button_Menu = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "menuItem");
		m_Path_Button_CursorTool = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "cursorTool");
		m_Path_Button_CursorToolHL = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "cursorToolHL");
		m_Path_Button_PaletteIcon = AssetsManager()->FindAsset<CAsset_GuiButtonStyle>(PackageId, "paletteIcon");
		
		m_Path_Toggle_Default = AssetsManager()->FindAsset<CAsset_GuiToggleStyle>(PackageId, "default");
		m_Path_Toggle_Toolbar = AssetsManager()->FindAsset<CAsset_GuiToggleStyle>(PackageId, "toolbar");
		m_Path_Toggle_Visibility = AssetsManager()->FindAsset<CAsset_GuiToggleStyle>(PackageId, "listVisibility");
		
		m_Path_ComboBox_Default = AssetsManager()->FindAsset<CAsset_GuiComboBoxStyle>(PackageId, "default");
		
		m_Path_IntEdit_Default = AssetsManager()->FindAsset<CAsset_GuiIntEditStyle>(PackageId, "default");
		m_Path_ColorEdit_Default = AssetsManager()->FindAsset<CAsset_GuiColorEditStyle>(PackageId, "default");
		
		m_Path_Scrollbar_Default = AssetsManager()->FindAsset<CAsset_GuiScrollbarStyle>(PackageId, "default");
		m_Path_Slider_Default = AssetsManager()->FindAsset<CAsset_GuiSliderStyle>(PackageId, "default");
		
		m_Path_Tabs_Default = AssetsManager()->FindAsset<CAsset_GuiTabsStyle>(PackageId, "default");

		m_Path_Sprite_IconNone = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconNone");
		m_Path_Sprite_IconDefault = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconDefault");
		m_Path_Sprite_IconFolder = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconFolder");
		m_Path_Sprite_IconFolderEdited = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconFolderEdited");
		m_Path_Sprite_IconFolderReadOnly = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconFolderReadOnly");
		m_Path_Sprite_IconLoad = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLoad");
		m_Path_Sprite_IconSave = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconSave");
		m_Path_Sprite_IconAsset = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconAsset");
		m_Path_Sprite_IconNewAsset = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconNewAsset");
		m_Path_Sprite_IconImage = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconImage");
		m_Path_Sprite_IconSprite = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconSprite");
		m_Path_Sprite_IconSkeleton = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconSkeleton");
		m_Path_Sprite_IconSkeletonSkin = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconSkeletonSkin");
		m_Path_Sprite_IconSkeletonAnimation = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconSkeletonAnimation");
		m_Path_Sprite_IconCharacter = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconCharacter");
		m_Path_Sprite_IconCharacterPart = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconCharacterPart");
		m_Path_Sprite_IconWeapon = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconWeapon");
		m_Path_Sprite_IconMap = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconMap");
		m_Path_Sprite_IconQuad = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconQuad");
		m_Path_Sprite_IconTiles = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconTiles");
		m_Path_Sprite_IconZoneTiles = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconZoneTiles");
		m_Path_Sprite_IconZoneType = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconZoneType");
		m_Path_Sprite_IconEntityType = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconEntityType");
		m_Path_Sprite_IconGuiRect = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiRect");
		m_Path_Sprite_IconGuiLine = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiLine");
		m_Path_Sprite_IconGuiBox = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiBox");
		m_Path_Sprite_IconGuiLabel = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiLabel");
		m_Path_Sprite_IconGuiButton = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiButton");
		m_Path_Sprite_IconGuiToggle = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiToggle");
		m_Path_Sprite_IconGuiSlider = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiSlider");
		m_Path_Sprite_IconGuiScrollbar = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiScrollbar");
		m_Path_Sprite_IconGuiTabs = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGuiTabs");
		m_Path_Sprite_IconSystem = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconSystem");
		m_Path_Sprite_IconStamp = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconStamp");
		m_Path_Sprite_IconMove = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconMove");
		m_Path_Sprite_IconView = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconView");
		m_Path_Sprite_IconHidden = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconHidden");
		m_Path_Sprite_IconEntities = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconEntities");
		m_Path_Sprite_IconNewFolder = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconNewFolder");
		m_Path_Sprite_IconDelete = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconDelete");
		m_Path_Sprite_IconCrop = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconCrop");
		m_Path_Sprite_IconMoveBack = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconMoveBack");
		m_Path_Sprite_IconMoveFront = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconMoveFront");
		m_Path_Sprite_IconUp = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconUp");
		m_Path_Sprite_IconDown = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconDown");
		m_Path_Sprite_IconVFlip = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconVFlip");
		m_Path_Sprite_IconHFlip = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconHFlip");
		m_Path_Sprite_IconRotateCW = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconRotateCW");
		m_Path_Sprite_IconRotateCCW = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconRotateCCW");
		m_Path_Sprite_IconGrid = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGrid");
		m_Path_Sprite_IconErase = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconErase");
		m_Path_Sprite_IconPolygon = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconPolygon");
		m_Path_Sprite_IconMoveVertex = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconMoveVertex");
		m_Path_Sprite_IconDeleteVertex = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconDeleteVertex");
		m_Path_Sprite_IconAddVertex = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconAddVertex");
		m_Path_Sprite_IconWeightVertex = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconWeightVertex");
		m_Path_Sprite_IconVertex = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconVertex");
		m_Path_Sprite_IconPencil = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconPencil");
		m_Path_Sprite_IconLineStyle = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLineStyle");
		m_Path_Sprite_IconLayer = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLayer");
		m_Path_Sprite_IconBigMesh = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconBigMesh");
		m_Path_Sprite_IconBigVFlip = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconBigVFlip");
		m_Path_Sprite_IconBigHFlip = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconBigHFlip");
		m_Path_Sprite_IconBigRotateCW = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconBigRotateCW");
		m_Path_Sprite_IconBigRotateCCW = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconBigRotateCCW");
		m_Path_Sprite_IconSpriteCreator = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconSpriteCreator");
		m_Path_Sprite_IconVertexCorner = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconVertexCorner");
		m_Path_Sprite_IconVertexSmooth = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconVertexSmooth");
		m_Path_Sprite_IconVertexFree = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconVertexFree");
		m_Path_Sprite_IconVertexSymetric = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconVertexSymetric");
		m_Path_Sprite_IconVertexAligned = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconVertexAligned");
		m_Path_Sprite_IconMatSpriteAlignLine = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconMatSpriteAlignLine");
		m_Path_Sprite_IconMatSpriteAlignObject = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconMatSpriteAlignObject");
		m_Path_Sprite_IconMatSpriteAlignStretched = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconMatSpriteAlignStretched");
		m_Path_Sprite_IconLabel = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLabel");
		m_Path_Sprite_IconPathOpen = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconPathOpen");
		m_Path_Sprite_IconPathClosed = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconPathClosed");
		m_Path_Sprite_IconPathInfinite = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconPathInfinite");
		m_Path_Sprite_IconFillNone = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconFillNone");
		m_Path_Sprite_IconFillInner = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconFillInner");
		m_Path_Sprite_IconFillOuter = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconFillOuter");
		m_Path_Sprite_IconLineHide = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLineHide");
		m_Path_Sprite_IconLineShow = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLineShow");
	
		m_Path_Sprite_IconLineTileBg = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLineTileBg");
		m_Path_Sprite_IconLineTileFg = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLineTileFg");
		m_Path_Sprite_IconLineTileCapEnd = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLineTileCapEnd");
		m_Path_Sprite_IconLineTileCapBegin = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLineTileCapBegin");
		m_Path_Sprite_IconLineTileCornerConvex = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLineTileCornerConvex");
		m_Path_Sprite_IconLineTileCornerConcave = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLineTileCornerConcave");
		m_Path_Sprite_IconLineTileLine = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconLineTileLine");
		m_Path_Sprite_IconFillingScaling = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconFillingScaling");
		m_Path_Sprite_IconFillingStretching = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconFillingStretching");
		m_Path_Sprite_IconFillingSpacing = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconFillingSpacing");
	
		m_Path_Sprite_IconShapeStar = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconShapeStar");
		m_Path_Sprite_IconShapeHexagon = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconShapeHexagon");
		m_Path_Sprite_IconShapeHexagon2 = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconShapeHexagon2");
		m_Path_Sprite_IconShapePentagon = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconShapePentagon");
		m_Path_Sprite_IconShapeTriangle = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconShapeTriangle");
		m_Path_Sprite_IconShapeSquare = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconShapeSquare");
		m_Path_Sprite_IconShapeArcWeight = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconShapeArcWeight");
		m_Path_Sprite_IconShapeArc = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconShapeArc");
		m_Path_Sprite_IconShapeBow = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconShapeBow");
		m_Path_Sprite_IconShapeCircle = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconShapeCircle");
		m_Path_Sprite_IconGridAligned = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconGridAligned");
		m_Path_Sprite_IconDisplaySettings = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconDisplaySettings");
		m_Path_Sprite_IconZoneRatio0 = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconZoneRatio0");
		m_Path_Sprite_IconZoneRatio1 = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconZoneRatio1");
		m_Path_Sprite_IconZoneRatio2 = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconZoneRatio2");
		m_Path_Sprite_IconZoneRatio3 = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconZoneRatio3");
		m_Path_Sprite_IconZoneRatio4 = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconZoneRatio4");
		m_Path_Sprite_IconEntityRatio0 = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconEntityRatio0");
		m_Path_Sprite_IconEntityRatio1 = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconEntityRatio1");
		m_Path_Sprite_IconEntityRatio2= AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconEntityRatio2");
		m_Path_Sprite_IconEntityRatio3= AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "iconEntityRatio3");
	
		m_Path_Sprite_GizmoScale = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoScale");
		m_Path_Sprite_GizmoRotate = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoRotate");
		m_Path_Sprite_GizmoPivot = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoPivot");
		m_Path_Sprite_GizmoVertexCornerBg = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoVertexCornerBg");
		m_Path_Sprite_GizmoVertexCornerFg = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoVertexCornerFg");
		m_Path_Sprite_GizmoVertexSmoothBg = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoVertexSmoothBg");
		m_Path_Sprite_GizmoVertexSmoothFg = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoVertexSmoothFg");
		m_Path_Sprite_GizmoVertexBezierBg = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoVertexBezierBg");
		m_Path_Sprite_GizmoVertexBezierFg = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoVertexBezierFg");
		m_Path_Sprite_GizmoVertexControl = AssetsManager()->FindAsset<CAsset_Sprite>(PackageId, "gizmoVertexControl");
	}
	
	//Init default path to the GUI
	m_LabelStyle = m_Path_Label_Text;
	m_LabelHeaderStyle = m_Path_Label_Header;
	m_ButtonStyle = m_Path_Button_Default;
	m_ToggleStyle = m_Path_Toggle_Default;
	m_IntEditStyle = m_Path_IntEdit_Default;
	m_ColorEditStyle = m_Path_ColorEdit_Default;
	m_TextEntryStyle = m_Path_Button_TextEdit;
	m_NumericEntryStyle = m_Path_Button_NumericEdit;
	m_ScrollbarStyle = m_Path_Scrollbar_Default;
	m_SliderStyle = m_Path_Slider_Default;
	m_TabsStyle = m_Path_Tabs_Default;
	m_PopupStyle = m_Path_Box_Menu;
	m_ComposeStyle = m_Path_Label_Compose;
	m_TextSelectionStyle = m_Path_Rect_TextSelection;
	m_ComboBoxStyle = m_Path_ComboBox_Default;
	m_SeparatorStyle = m_Path_Line_Separator;
	
	//~ PackageId = AssetsManager()->NewPackage("mypackage");
	//~ AssetsManager()->SetPackageReadOnly(PackageId, false);
	
	SetEditedPackage(-1);
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
			case CAsset_Map::TypeId:
				IconPath = m_Path_Sprite_IconMap;
				break;
			case CAsset_MapGroup::TypeId:
				IconPath = m_Path_Sprite_IconFolder;
				break;
			case CAsset_MapLayerTiles::TypeId:
				IconPath = m_Path_Sprite_IconTiles;
				break;
			case CAsset_MapLayerObjects::TypeId:
				IconPath = m_Path_Sprite_IconPolygon;
				break;
			case CAsset_MapLayerQuads::TypeId:
				IconPath = m_Path_Sprite_IconQuad;
				break;
			case CAsset_MapZoneTiles::TypeId:
				IconPath = m_Path_Sprite_IconZoneTiles;
				break;
			case CAsset_MapEntities::TypeId:
				IconPath = m_Path_Sprite_IconEntities;
				break;
			case CAsset_ZoneType::TypeId:
				IconPath = m_Path_Sprite_IconZoneType;
				break;
			case CAsset_EntityType::TypeId:
				IconPath = m_Path_Sprite_IconEntityType;
				break;
			case CAsset_Material::TypeId:
				IconPath = m_Path_Sprite_IconLineStyle;
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
				IconPath = m_Path_Sprite_IconGuiButton;
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

void CGuiEditor::RefreshPackageTree()
{
	if(m_pPackagesTree)
		m_pPackagesTree->Refresh();
}

void CGuiEditor::RefreshAssetsTree()
{
	if(m_pAssetsTree)
		m_pAssetsTree->Refresh();
}

void CGuiEditor::SetHint(const CLocalizableString& LString)
{
	m_pHintLabel->SetText(LString);
}

void CGuiEditor::SetCoordinates(const CLocalizableString& LString)
{
	m_pCoordinatesLabel->SetText(LString);
}
