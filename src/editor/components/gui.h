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

#ifndef __EDITOR_GUI__
#define __EDITOR_GUI__

#include <client/components/gui.h>
#include <client/gui/listlayout.h>
#include <client/gui/popup.h>
#include <client/gui/label.h>
#include <shared/components/assetsmanager.h>
#include <editor/kernel.h>

class CGuiEditor : public CGui
{
public:
	class CMainWidget : public gui::CVListLayout
	{
	protected:
		CGuiEditor* m_pAssetsEditor;
	
	protected:
		gui::CWidget* CreateToolbar();
		gui::CWidget* CreateStatusbar();
	
	public:
		CMainWidget(CGuiEditor* pAssetsEditor);
		
		virtual void Render();
		virtual void OnInputEvent(const CInput::CEvent& Event);
	};
	
private:
	CMainWidget* m_pAssetsEditorWidget;
	CEditorKernel* m_pEditorKernel;
	
	int m_EditedPackageId;
	CAssetPath m_EditedAssetPath;
	CSubPath m_EditedSubPath;
	
public:
	class CAssetsTree* m_pAssetsTree;
	class CPackagesTree* m_pPackagesTree;
	gui::CLabel* m_pHintLabel;
	gui::CLabel* m_pCoordinatesLabel;
	
public:
	CAssetPath m_Path_Image_ZoneTexture;
	
	CAssetPath m_Path_Rect_TextSelection;
	CAssetPath m_Path_Rect_Selection;
	CAssetPath m_Path_Rect_Border;
	
	CAssetPath m_Path_Line_Separator;
	
	CAssetPath m_Path_Box_View;
	CAssetPath m_Path_Box_Panel;
	CAssetPath m_Path_Box_Menu;
	CAssetPath m_Path_Box_Dialog;
	CAssetPath m_Path_Box_SubList;
	CAssetPath m_Path_Box_Statusbar;
	
	CAssetPath m_Path_Label_Text;
	CAssetPath m_Path_Label_Header;
	CAssetPath m_Path_Label_DialogMessage;
	CAssetPath m_Path_Label_Compose;
	CAssetPath m_Path_Label_InactiveListItem;
	
	CAssetPath m_Path_Button_Default;
	CAssetPath m_Path_Button_Dialog;
	CAssetPath m_Path_Button_TextEdit;
	CAssetPath m_Path_Button_NumericEdit;
	CAssetPath m_Path_Button_ListItem;
	CAssetPath m_Path_Button_ListItemHL;
	CAssetPath m_Path_Button_Menu;
	CAssetPath m_Path_Button_CursorTool;
	CAssetPath m_Path_Button_CursorToolHL;
	CAssetPath m_Path_Button_PaletteIcon;
	
	CAssetPath m_Path_Toggle_Default;
	CAssetPath m_Path_Toggle_Toolbar;
	CAssetPath m_Path_Toggle_Visibility;
	
	CAssetPath m_Path_ComboBox_Default;
	
	CAssetPath m_Path_IntEdit_Default;
	CAssetPath m_Path_ColorEdit_Default;
	
	CAssetPath m_Path_Scrollbar_Default;
	CAssetPath m_Path_Slider_Default;
	
	CAssetPath m_Path_Tabs_Default;
	
	CAssetPath m_Path_Sprite_IconNone;
	CAssetPath m_Path_Sprite_IconDefault;
	CAssetPath m_Path_Sprite_IconAsset;
	CAssetPath m_Path_Sprite_IconLoad;
	CAssetPath m_Path_Sprite_IconSave;
	CAssetPath m_Path_Sprite_IconNewAsset;
	CAssetPath m_Path_Sprite_IconFolder;
	CAssetPath m_Path_Sprite_IconImage;
	CAssetPath m_Path_Sprite_IconSprite;
	CAssetPath m_Path_Sprite_IconSkeleton;
	CAssetPath m_Path_Sprite_IconSkeletonSkin;
	CAssetPath m_Path_Sprite_IconSkeletonAnimation;
	CAssetPath m_Path_Sprite_IconCharacter;
	CAssetPath m_Path_Sprite_IconCharacterPart;
	CAssetPath m_Path_Sprite_IconWeapon;
	CAssetPath m_Path_Sprite_IconMap;
	CAssetPath m_Path_Sprite_IconQuad;
	CAssetPath m_Path_Sprite_IconTiles;
	CAssetPath m_Path_Sprite_IconZoneTiles;
	CAssetPath m_Path_Sprite_IconZoneType;
	CAssetPath m_Path_Sprite_IconEntityType;
	CAssetPath m_Path_Sprite_IconGuiRect;
	CAssetPath m_Path_Sprite_IconGuiLine;
	CAssetPath m_Path_Sprite_IconGuiBox;
	CAssetPath m_Path_Sprite_IconGuiLabel;
	CAssetPath m_Path_Sprite_IconGuiButton;
	CAssetPath m_Path_Sprite_IconGuiToggle;
	CAssetPath m_Path_Sprite_IconGuiSlider;
	CAssetPath m_Path_Sprite_IconGuiScrollbar;
	CAssetPath m_Path_Sprite_IconGuiTabs;
	CAssetPath m_Path_Sprite_IconSystem;
	CAssetPath m_Path_Sprite_IconStamp;
	CAssetPath m_Path_Sprite_IconMove;
	CAssetPath m_Path_Sprite_IconView;
	CAssetPath m_Path_Sprite_IconHidden;
	CAssetPath m_Path_Sprite_IconEntities;
	CAssetPath m_Path_Sprite_IconNewFolder;
	CAssetPath m_Path_Sprite_IconDelete;
	CAssetPath m_Path_Sprite_IconCrop;
	CAssetPath m_Path_Sprite_IconMoveBack;
	CAssetPath m_Path_Sprite_IconMoveFront;
	CAssetPath m_Path_Sprite_IconUp;
	CAssetPath m_Path_Sprite_IconDown;
	CAssetPath m_Path_Sprite_IconVFlip;
	CAssetPath m_Path_Sprite_IconHFlip;
	CAssetPath m_Path_Sprite_IconRotateCW;
	CAssetPath m_Path_Sprite_IconRotateCCW;
	CAssetPath m_Path_Sprite_IconGrid;
	CAssetPath m_Path_Sprite_IconFolderEdited;
	CAssetPath m_Path_Sprite_IconFolderReadOnly;
	CAssetPath m_Path_Sprite_IconErase;
	CAssetPath m_Path_Sprite_IconPolygon;
	CAssetPath m_Path_Sprite_IconMoveVertex;
	CAssetPath m_Path_Sprite_IconDeleteVertex;
	CAssetPath m_Path_Sprite_IconAddVertex;
	CAssetPath m_Path_Sprite_IconWeightVertex;
	CAssetPath m_Path_Sprite_IconVertex;
	CAssetPath m_Path_Sprite_IconPencil;
	CAssetPath m_Path_Sprite_IconLineStyle;
	CAssetPath m_Path_Sprite_IconLayer;
	CAssetPath m_Path_Sprite_IconBigMesh;
	CAssetPath m_Path_Sprite_IconBigHFlip;
	CAssetPath m_Path_Sprite_IconBigVFlip;
	CAssetPath m_Path_Sprite_IconBigRotateCW;
	CAssetPath m_Path_Sprite_IconBigRotateCCW;
	CAssetPath m_Path_Sprite_IconSpriteCreator;
	CAssetPath m_Path_Sprite_IconVertexCorner;
	CAssetPath m_Path_Sprite_IconVertexSmooth;
	CAssetPath m_Path_Sprite_IconVertexFree;
	CAssetPath m_Path_Sprite_IconVertexAligned;
	CAssetPath m_Path_Sprite_IconVertexSymetric;
	CAssetPath m_Path_Sprite_IconMatLayerRepeat;
	CAssetPath m_Path_Sprite_IconMatLayerStretch;
	
	CAssetPath m_Path_Sprite_GizmoScale;
	CAssetPath m_Path_Sprite_GizmoRotate;
	CAssetPath m_Path_Sprite_GizmoPivot;
	CAssetPath m_Path_Sprite_GizmoVertexCornerBg;
	CAssetPath m_Path_Sprite_GizmoVertexCornerFg;
	CAssetPath m_Path_Sprite_GizmoVertexSmoothBg;
	CAssetPath m_Path_Sprite_GizmoVertexSmoothFg;
	CAssetPath m_Path_Sprite_GizmoVertexBezierBg;
	CAssetPath m_Path_Sprite_GizmoVertexBezierFg;
	CAssetPath m_Path_Sprite_GizmoVertexControl;

public:
	CGuiEditor(CEditorKernel* pKernel);
	virtual ~CGuiEditor();
	
	virtual void LoadAssets();
	virtual gui::CWidget* CreateMainWidget();
	
	CAssetPath GetItemIcon(const CAssetPath& AssetPath, const CSubPath& SubPath);
	const char* GetItemName(const CAssetPath& AssetPath, const CSubPath& SubPath);
	
	void SetEditedPackage(int PackageId);
	void SetEditedAsset(const CAssetPath& Path, const CSubPath& SubPath);
	
	inline int GetEditedPackageId() const { return m_EditedPackageId; }
	inline CAssetPath GetEditedAssetPath() const { return m_EditedAssetPath; }
	inline CSubPath GetEditedSubPath() const { return m_EditedSubPath; }
	
	void RefreshPackageTree();
	void RefreshAssetsTree();
	
	void SetHint(const CLocalizableString& LString);
	void SetCoordinates(const CLocalizableString& LString);
	
	inline CEditorKernel* EditorKernel() { return m_pEditorKernel; }
	inline const CEditorKernel* EditorKernel() const { return m_pEditorKernel; }
};

class CContextMenu : public gui::CPopup
{	
protected:
	CGuiEditor* m_pAssetsEditor;
	gui::CVListLayout* m_pList;
	
public:
	CContextMenu(CGuiEditor* pAssetsEditor);
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
	inline gui::CVListLayout* List() { return m_pList; }
};

class CDialog : public gui::CPopup
{
protected:
	class CClose : public gui::CButton
	{
	protected:
		CDialog* m_pPopup;
		
	protected:
		virtual void MouseClickAction() { m_pPopup->Close(); }
		
	public:
		CClose(CDialog* pPopup) :
			gui::CButton(pPopup->Context(), _LSTRING("Close")),
			m_pPopup(pPopup)
		{
			SetButtonStyle(pPopup->m_pAssetsEditor->m_Path_Button_Dialog);
		}
	};

protected:
	CGuiEditor* m_pAssetsEditor;
	
public:
	CDialog(CGuiEditor* pAssetsEditor);
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

class PackagePropertiesDialog : public CDialog
{
protected:
	int m_PackageId;
	
public:
	PackagePropertiesDialog(CGuiEditor* pAssetsEditor, int PackageId);
};

class CErrorDialog : public CDialog
{
public:
	CErrorDialog(CGuiEditor* pAssetsEditor, const CLocalizableString& LString);
};

#endif
