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

#include <shared/system/debug.h>
#include <shared/system/string.h>
#include <shared/kernel.h>
#include <shared/components/assetsmanager.h>
#include <shared/components/storage.h>
#include <client/loading_tools.h>

#include <cstdlib>

#define CREATE_SPRITE(packageid, name, image, x, y, w, h) {\
	CAssetPath SpritePath;\
	CAsset_Sprite* pSprite = pKernel->AssetsManager()->NewAsset_Hard<CAsset_Sprite>(&SpritePath, packageid);\
	pSprite->SetName(name);\
	pSprite->SetImagePath(image);\
	pSprite->SetX(x);\
	pSprite->SetY(y);\
	pSprite->SetWidth(w);\
	pSprite->SetHeight(h);\
}

#define CREATE_SPRITE_PATH(path, packageid, name, image, x, y, w, h) {\
	CAsset_Sprite* pSprite = pKernel->AssetsManager()->NewAsset_Hard<CAsset_Sprite>(&path, packageid);\
	pSprite->SetName(name);\
	pSprite->SetImagePath(image);\
	pSprite->SetX(x);\
	pSprite->SetY(y);\
	pSprite->SetWidth(w);\
	pSprite->SetHeight(h);\
}

int main(int argc, const char **argv)
{
	CSharedKernel* pKernel = new CSharedKernel();
	if(!pKernel->Init(argc, argv))
	{
		dbg_msg("Kernel", "unable to initialize shared kernel");
		exit(EXIT_FAILURE);
	}
	
	int PackageId = pKernel->AssetsManager()->NewPackage("gui/editor");
	
	//Colors
	vec4 ColorBorder = vec4(40.0f/255.0f, 40.0f/255.0f, 40.0f/255.0f, 1.0f);
	vec4 ColorDarkBG = vec4(58.0f/255.0f, 58.0f/255.0f, 58.0f/255.0f, 1.0f);
	
	vec4 ColorLightBG = vec4(83.0f/255.0f, 83.0f/255.0f, 83.0f/255.0f, 1.0f);
	vec4 ColorLightHL = vec4(108.0f/255.0f, 108.0f/255.0f, 108.0f/255.0f, 1.0f);
	
	//Images
	CAssetPath ImageEditorPath = CreateNewImage(pKernel, PackageId, "editor", "datasrc/images/gui_editor.png", 32, 16);
	CAssetPath ImageBoxesPath = CreateNewImage(pKernel, PackageId, "boxes", "datasrc/images/gui_editor_box.png", 16, 16);
	CAssetPath ImageZoneTexturePath = CreateNewImage(pKernel, PackageId, "zoneTexture", "datasrc/images/gui_editor_zonetexture.png", 1, 1);
	
	//Sprites
	CAssetPath IconDecreasePath;
	CAssetPath IconIncreasePath;
	
	CREATE_SPRITE(PackageId, "iconVoid", ImageEditorPath, 0, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconDefault", ImageEditorPath, 1, 0, 1, 1);
	CREATE_SPRITE_PATH(IconDecreasePath, PackageId, "iconDecrease", ImageEditorPath, 2, 0, 1, 1);
	CREATE_SPRITE_PATH(IconIncreasePath, PackageId, "iconIncrease", ImageEditorPath, 3, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconDelete", ImageEditorPath, 4, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconEdit", ImageEditorPath, 9, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconView", ImageEditorPath, 10, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconUp", ImageEditorPath, 11, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconDown", ImageEditorPath, 12, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconDuplicate", ImageEditorPath, 13, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconSave", ImageEditorPath, 14, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconLoad", ImageEditorPath, 15, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconFirstFrame", ImageEditorPath, 16, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconPrevFrame", ImageEditorPath, 17, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconPlay", ImageEditorPath, 18, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconPause", ImageEditorPath, 19, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconNextFram", ImageEditorPath, 20, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconLastFrame", ImageEditorPath, 21, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconVFlip", ImageEditorPath, 22, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconHFlip", ImageEditorPath, 23, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconRotateCW", ImageEditorPath, 24, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconRotateCCW", ImageEditorPath, 25, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconStamp", ImageEditorPath, 26, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconFill", ImageEditorPath, 27, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconGrid", ImageEditorPath, 28, 0, 1, 1);
	CREATE_SPRITE(PackageId, "iconNone", ImageEditorPath, 29, 0, 1, 1);
	
	CREATE_SPRITE(PackageId, "iconFolder", ImageEditorPath, 0, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconTiles", ImageEditorPath, 1, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconQuad", ImageEditorPath, 2, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconWeapon", ImageEditorPath, 3, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconMap", ImageEditorPath, 4, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconZoneTiles", ImageEditorPath, 5, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconZoneType", ImageEditorPath, 6, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconEntityType", ImageEditorPath, 7, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconAsset", ImageEditorPath, 8, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconNewAsset", ImageEditorPath, 9, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconHidden", ImageEditorPath, 10, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconEntities", ImageEditorPath, 11, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconNewFolder", ImageEditorPath, 12, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconLayer", ImageEditorPath, 13, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconBone", ImageEditorPath, 14, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconSprite", ImageEditorPath, 15, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconImage", ImageEditorPath, 16, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconSkeleton", ImageEditorPath, 17, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconSkeletonSkin", ImageEditorPath, 18, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconSkeletonAnimation", ImageEditorPath, 19, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconLayerAnimation", ImageEditorPath, 20, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconCharacter", ImageEditorPath, 21, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconCharacterPart", ImageEditorPath, 22, 1, 1, 1);
	
	CREATE_SPRITE(PackageId, "iconMove", ImageEditorPath, 0, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconTranslate", ImageEditorPath, 1, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconTranslateX", ImageEditorPath, 2, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconTranslateY", ImageEditorPath, 3, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconRotate", ImageEditorPath, 4, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconScale", ImageEditorPath, 5, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconScaleX", ImageEditorPath, 6, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconScaleY", ImageEditorPath, 7, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconBoneLength", ImageEditorPath, 8, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconBoneAdd", ImageEditorPath, 9, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconBoneDelete", ImageEditorPath, 10, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconAttach", ImageEditorPath, 11, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconGuiRect", ImageEditorPath, 12, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconGuiLine", ImageEditorPath, 13, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconGuiBox", ImageEditorPath, 14, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconGuiScrollbar", ImageEditorPath, 15, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconGuiTabs", ImageEditorPath, 16, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconSystem", ImageEditorPath, 17, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconButton", ImageEditorPath, 18, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconLabel", ImageEditorPath, 19, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconToggle", ImageEditorPath, 20, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconSlider", ImageEditorPath, 21, 2, 1, 1);
	CREATE_SPRITE(PackageId, "iconKeyFrameMove", ImageEditorPath, 23, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconKeyFrameAdd", ImageEditorPath, 24, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconKeyFrameDelete", ImageEditorPath, 25, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconKeyFrames", ImageEditorPath, 26, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconKeyFrameColor", ImageEditorPath, 27, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconKeyFrameBone", ImageEditorPath, 28, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconKeyFrameLayer", ImageEditorPath, 29, 1, 1, 1);
	CREATE_SPRITE(PackageId, "iconCrop", ImageEditorPath, 30, 1, 1, 1);
	
	CREATE_SPRITE(PackageId, "gizmoVertexBg", ImageEditorPath, 0, 11, 1, 1);
	CREATE_SPRITE(PackageId, "gizmoVertexFg", ImageEditorPath, 1, 11, 1, 1);
	CREATE_SPRITE(PackageId, "gizmoPivot", ImageEditorPath, 2, 11, 1, 1);
	CREATE_SPRITE(PackageId, "gizmoScale", ImageEditorPath, 3, 11, 1, 1);
	CREATE_SPRITE(PackageId, "gizmoRotate", ImageEditorPath, 4, 11, 1, 1);
	
	//Rect
	CAssetPath RectStampPath;
	CAssetPath RectSelectionPath;
	CAssetPath RectDialogPath;
	CAssetPath RectFocusPath;
	CAssetPath RectButtonPath;
	CAssetPath RectButtonMoPath;
	CAssetPath RectEditPath;
	{
		CAssetPath SpriteTL;
		CAssetPath SpriteT;
		CAssetPath SpriteTR;
		CAssetPath SpriteL;
		CAssetPath SpriteR;
		CAssetPath SpriteBL;
		CAssetPath SpriteB;
		CAssetPath SpriteBR;
		
		CREATE_SPRITE_PATH(SpriteTL, PackageId, "boxSelectionTL", ImageBoxesPath, 6, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteT , PackageId, "boxSelectionT" , ImageBoxesPath, 7, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteTR, PackageId, "boxSelectionTR", ImageBoxesPath, 8, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteL,  PackageId, "boxSelectionL" , ImageBoxesPath, 6, 1, 1, 1);
		CREATE_SPRITE_PATH(SpriteR,  PackageId, "boxSelectionR" , ImageBoxesPath, 8, 1, 1, 1);
		CREATE_SPRITE_PATH(SpriteBL, PackageId, "boxSelectionBL", ImageBoxesPath, 6, 2, 1, 1);
		CREATE_SPRITE_PATH(SpriteB,  PackageId, "boxSelectionB" , ImageBoxesPath, 7, 2, 1, 1);
		CREATE_SPRITE_PATH(SpriteBR, PackageId, "boxSelectionBR", ImageBoxesPath, 8, 2, 1, 1);
	
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectSelectionPath, PackageId);
		pAsset->SetName("selection");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_IMAGE);
		pAsset->SetImageTLPath(SpriteTL);
		pAsset->SetImageTPath(SpriteT);
		pAsset->SetImageTRPath(SpriteTR);
		pAsset->SetImageLPath(SpriteL);
		pAsset->SetImageRPath(SpriteR);
		pAsset->SetImageBLPath(SpriteBL);
		pAsset->SetImageBPath(SpriteB);
		pAsset->SetImageBRPath(SpriteBR);
		pAsset->SetImagePadding(8.0f);
	}
	{
		CAssetPath SpriteTL;
		CAssetPath SpriteT;
		CAssetPath SpriteTR;
		CAssetPath SpriteL;
		CAssetPath SpriteR;
		CAssetPath SpriteBL;
		CAssetPath SpriteB;
		CAssetPath SpriteBR;
		
		CREATE_SPRITE_PATH(SpriteTL, PackageId, "boxBorderTL", ImageBoxesPath, 9, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteT , PackageId, "boxBorderT" , ImageBoxesPath, 10, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteTR, PackageId, "boxBorderTR", ImageBoxesPath, 11, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteL,  PackageId, "boxBorderL" , ImageBoxesPath, 9, 1, 1, 1);
		CREATE_SPRITE_PATH(SpriteR,  PackageId, "boxBorderR" , ImageBoxesPath, 11, 1, 1, 1);
		CREATE_SPRITE_PATH(SpriteBL, PackageId, "boxBorderBL", ImageBoxesPath, 9, 2, 1, 1);
		CREATE_SPRITE_PATH(SpriteB,  PackageId, "boxBorderB" , ImageBoxesPath, 10, 2, 1, 1);
		CREATE_SPRITE_PATH(SpriteBR, PackageId, "boxBorderBR", ImageBoxesPath, 11, 2, 1, 1);
	
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectStampPath, PackageId);
		pAsset->SetName("border");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_IMAGE);
		pAsset->SetImageTLPath(SpriteTL);
		pAsset->SetImageTPath(SpriteT);
		pAsset->SetImageTRPath(SpriteTR);
		pAsset->SetImageLPath(SpriteL);
		pAsset->SetImageRPath(SpriteR);
		pAsset->SetImageBLPath(SpriteBL);
		pAsset->SetImageBPath(SpriteB);
		pAsset->SetImageBRPath(SpriteBR);
		pAsset->SetImagePadding(1.0f);
	}
	{
		CAssetPath SpriteTL;
		CAssetPath SpriteT;
		CAssetPath SpriteTR;
		CAssetPath SpriteL;
		CAssetPath SpriteR;
		CAssetPath SpriteBL;
		CAssetPath SpriteB;
		CAssetPath SpriteBR;
		
		CREATE_SPRITE_PATH(SpriteTL, PackageId, "boxFocusTL", ImageBoxesPath, 12, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteT , PackageId, "boxFocusT" , ImageBoxesPath, 13, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteTR, PackageId, "boxFocusTR", ImageBoxesPath, 14, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteL,  PackageId, "boxFocusL" , ImageBoxesPath, 12, 1, 1, 1);
		CREATE_SPRITE_PATH(SpriteR,  PackageId, "boxFocusR" , ImageBoxesPath, 14, 1, 1, 1);
		CREATE_SPRITE_PATH(SpriteBL, PackageId, "boxFocusBL", ImageBoxesPath, 12, 2, 1, 1);
		CREATE_SPRITE_PATH(SpriteB,  PackageId, "boxFocusB" , ImageBoxesPath, 13, 2, 1, 1);
		CREATE_SPRITE_PATH(SpriteBR, PackageId, "boxFocusBR", ImageBoxesPath, 14, 2, 1, 1);
	
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectFocusPath, PackageId);
		pAsset->SetName("focus");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_IMAGE);
		pAsset->SetImageTLPath(SpriteTL);
		pAsset->SetImageTPath(SpriteT);
		pAsset->SetImageTRPath(SpriteTR);
		pAsset->SetImageLPath(SpriteL);
		pAsset->SetImageRPath(SpriteR);
		pAsset->SetImageBLPath(SpriteBL);
		pAsset->SetImageBPath(SpriteB);
		pAsset->SetImageBRPath(SpriteBR);
		pAsset->SetImagePadding(1.0f);
	}
	{
		CAssetPath SpriteTL;
		CAssetPath SpriteT;
		CAssetPath SpriteTR;
		CAssetPath SpriteL;
		CAssetPath SpriteR;
		CAssetPath SpriteBL;
		CAssetPath SpriteB;
		CAssetPath SpriteBR;
		
		CREATE_SPRITE_PATH(SpriteTL, PackageId, "boxDialogTL", ImageBoxesPath, 0, 6, 1, 1);
		CREATE_SPRITE_PATH(SpriteT , PackageId, "boxDialogT" , ImageBoxesPath, 1, 6, 1, 1);
		CREATE_SPRITE_PATH(SpriteTR, PackageId, "boxDialogTR", ImageBoxesPath, 2, 6, 1, 1);
		CREATE_SPRITE_PATH(SpriteL,  PackageId, "boxDialogL" , ImageBoxesPath, 0, 7, 1, 1);
		CREATE_SPRITE_PATH(SpriteR,  PackageId, "boxDialogR" , ImageBoxesPath, 2, 7, 1, 1);
		CREATE_SPRITE_PATH(SpriteBL, PackageId, "boxDialogBL", ImageBoxesPath, 0, 8, 1, 1);
		CREATE_SPRITE_PATH(SpriteB,  PackageId, "boxDialogB" , ImageBoxesPath, 1, 8, 1, 1);
		CREATE_SPRITE_PATH(SpriteBR, PackageId, "boxDialogBR", ImageBoxesPath, 2, 8, 1, 1);
	
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectDialogPath, PackageId);
		pAsset->SetName("dialog");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_IMAGE);
		pAsset->SetBackgroundColor(ColorLightBG);
		pAsset->SetImageTLPath(SpriteTL);
		pAsset->SetImageTPath(SpriteT);
		pAsset->SetImageTRPath(SpriteTR);
		pAsset->SetImageLPath(SpriteL);
		pAsset->SetImageRPath(SpriteR);
		pAsset->SetImageBLPath(SpriteBL);
		pAsset->SetImageBPath(SpriteB);
		pAsset->SetImageBRPath(SpriteBR);
	}
	
	{
		CAssetPath SpriteTL;
		CAssetPath SpriteT;
		CAssetPath SpriteTR;
		CAssetPath SpriteL;
		CAssetPath SpriteR;
		CAssetPath SpriteBL;
		CAssetPath SpriteB;
		CAssetPath SpriteBR;
		
		CREATE_SPRITE_PATH(SpriteTL, PackageId, "boxButtonTL", ImageBoxesPath, 0, 3, 1, 1);
		CREATE_SPRITE_PATH(SpriteT , PackageId, "boxButtonT" , ImageBoxesPath, 1, 3, 1, 1);
		CREATE_SPRITE_PATH(SpriteTR, PackageId, "boxButtonTR", ImageBoxesPath, 2, 3, 1, 1);
		CREATE_SPRITE_PATH(SpriteL,  PackageId, "boxButtonL" , ImageBoxesPath, 0, 4, 1, 1);
		CREATE_SPRITE_PATH(SpriteR,  PackageId, "boxButtonR" , ImageBoxesPath, 2, 4, 1, 1);
		CREATE_SPRITE_PATH(SpriteBL, PackageId, "boxButtonBL", ImageBoxesPath, 0, 5, 1, 1);
		CREATE_SPRITE_PATH(SpriteB,  PackageId, "boxButtonB" , ImageBoxesPath, 1, 5, 1, 1);
		CREATE_SPRITE_PATH(SpriteBR, PackageId, "boxButtonBR", ImageBoxesPath, 2, 5, 1, 1);
	
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectButtonPath, PackageId);
		pAsset->SetName("button");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_IMAGE);
		pAsset->SetBackgroundColor(ColorLightBG);
		pAsset->SetImageTLPath(SpriteTL);
		pAsset->SetImageTPath(SpriteT);
		pAsset->SetImageTRPath(SpriteTR);
		pAsset->SetImageLPath(SpriteL);
		pAsset->SetImageRPath(SpriteR);
		pAsset->SetImageBLPath(SpriteBL);
		pAsset->SetImageBPath(SpriteB);
		pAsset->SetImageBRPath(SpriteBR);
		pAsset->SetImagePadding(6.0f);
	}
	{
		CAssetPath SpriteTL;
		CAssetPath SpriteT;
		CAssetPath SpriteTR;
		CAssetPath SpriteL;
		CAssetPath SpriteR;
		CAssetPath SpriteBL;
		CAssetPath SpriteB;
		CAssetPath SpriteBR;
		
		CREATE_SPRITE_PATH(SpriteTL, PackageId, "boxButtonMoTL", ImageBoxesPath, 3, 3, 1, 1);
		CREATE_SPRITE_PATH(SpriteT , PackageId, "boxButtonMoT" , ImageBoxesPath, 4, 3, 1, 1);
		CREATE_SPRITE_PATH(SpriteTR, PackageId, "boxButtonMoTR", ImageBoxesPath, 5, 3, 1, 1);
		CREATE_SPRITE_PATH(SpriteL,  PackageId, "boxButtonMoL" , ImageBoxesPath, 3, 4, 1, 1);
		CREATE_SPRITE_PATH(SpriteR,  PackageId, "boxButtonMoR" , ImageBoxesPath, 5, 4, 1, 1);
		CREATE_SPRITE_PATH(SpriteBL, PackageId, "boxButtonMoBL", ImageBoxesPath, 3, 5, 1, 1);
		CREATE_SPRITE_PATH(SpriteB,  PackageId, "boxButtonMoB" , ImageBoxesPath, 4, 5, 1, 1);
		CREATE_SPRITE_PATH(SpriteBR, PackageId, "boxButtonMoBR", ImageBoxesPath, 5, 5, 1, 1);
	
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectButtonMoPath, PackageId);
		pAsset->SetName("buttonMo");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_IMAGE);
		pAsset->SetBackgroundColor(ColorLightHL);
		pAsset->SetImageTLPath(SpriteTL);
		pAsset->SetImageTPath(SpriteT);
		pAsset->SetImageTRPath(SpriteTR);
		pAsset->SetImageLPath(SpriteL);
		pAsset->SetImageRPath(SpriteR);
		pAsset->SetImageBLPath(SpriteBL);
		pAsset->SetImageBPath(SpriteB);
		pAsset->SetImageBRPath(SpriteBR);
		pAsset->SetImagePadding(6.0f);
	}
	{
		CAssetPath SpriteTL;
		CAssetPath SpriteT;
		CAssetPath SpriteTR;
		CAssetPath SpriteL;
		CAssetPath SpriteR;
		CAssetPath SpriteBL;
		CAssetPath SpriteB;
		CAssetPath SpriteBR;
		
		CREATE_SPRITE_PATH(SpriteTL, PackageId, "boxEditTL", ImageBoxesPath, 0, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteT , PackageId, "boxEditT" , ImageBoxesPath, 1, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteTR, PackageId, "boxEditTR", ImageBoxesPath, 2, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteL,  PackageId, "boxEditL" , ImageBoxesPath, 0, 1, 1, 1);
		CREATE_SPRITE_PATH(SpriteR,  PackageId, "boxEditR" , ImageBoxesPath, 2, 1, 1, 1);
		CREATE_SPRITE_PATH(SpriteBL, PackageId, "boxEditBL", ImageBoxesPath, 0, 2, 1, 1);
		CREATE_SPRITE_PATH(SpriteB,  PackageId, "boxEditB" , ImageBoxesPath, 1, 2, 1, 1);
		CREATE_SPRITE_PATH(SpriteBR, PackageId, "boxEditBR", ImageBoxesPath, 2, 2, 1, 1);
	
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectEditPath, PackageId);
		pAsset->SetName("entry");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_IMAGE);
		pAsset->SetBackgroundColor(ColorDarkBG);
		pAsset->SetImageTLPath(SpriteTL);
		pAsset->SetImageTPath(SpriteT);
		pAsset->SetImageTRPath(SpriteTR);
		pAsset->SetImageLPath(SpriteL);
		pAsset->SetImageRPath(SpriteR);
		pAsset->SetImageBLPath(SpriteBL);
		pAsset->SetImageBPath(SpriteB);
		pAsset->SetImageBRPath(SpriteBR);
		pAsset->SetImagePadding(6.0f);
	}
	
	CAssetPath RectListItemHlPath;
	{
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectListItemHlPath, PackageId);
		pAsset->SetName("listItemHl");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND);
		pAsset->SetBackgroundColor(ColorLightHL);
	}
	
	//Text
	{
		CAssetPath AssetPath;
		CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&AssetPath, PackageId);
		pAsset->SetName("text");
		pAsset->SetPadding(2);
		pAsset->SetSpacing(4);
		pAsset->SetFontSize(12);
		pAsset->SetTextColor(1.0f);
	}
	{
		CAssetPath AssetPath;
		CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&AssetPath, PackageId);
		pAsset->SetName("header");
		pAsset->SetPadding(2);
		pAsset->SetSpacing(4);
		pAsset->SetFontSize(16);
		pAsset->SetTextColor(vec4(1.0f, 1.0f, 0.5f, 1.0f));
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
	}
	
	//Composing
	{
		//Rect
		CAssetPath RectPath;
		{
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectPath, PackageId);
			pAsset->SetName("compose");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_ROUNDCORNER);
			pAsset->SetBackgroundColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));
			pAsset->SetCornerRadius(3.0f);
			pAsset->SetCornerFlags(CAsset_GuiRectStyle::CORNERFLAG_ALL);
		}
		
		//Label
		CAssetPath LabelPath;
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelPath, PackageId);
			pAsset->SetName("compose");
			pAsset->SetRectPath(RectPath);
			pAsset->SetPadding(4);
			pAsset->SetTextColor(vec4(15.0f/255.0f, 103.0f/255.0f, 146.0f/255.0f, 1.0f));
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_LEFT);
		}
	}
	
	//Button
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelMoPath;
		CAssetPath LabelRoPath;
		CAssetPath ButtonPath;
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIdlePath, PackageId);
			pAsset->SetName("editorButtonIdle");
			pAsset->SetRectPath(RectButtonPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelMoPath, PackageId);
			pAsset->SetName("editorButtonMO");
			pAsset->SetRectPath(RectButtonMoPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelRoPath, PackageId);
			pAsset->SetName("editorButtonRO");
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(vec4(0.8f, 0.8f, 0.8f, 1.0f));
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonPath, PackageId);
			pAsset->SetName("default");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelMoPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
	}
	
	//Cursor Tools and toggle
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelMoPath;
		CAssetPath LabelHlPath;
		CAssetPath LabelRoPath;
		CAssetPath ButtonPath;
		CAssetPath ButtonHlPath;
		CAssetPath TogglePath;
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIdlePath, PackageId);
			pAsset->SetName("cursorToolIdle");
			pAsset->SetRectPath(RectButtonPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelMoPath, PackageId);
			pAsset->SetName("cursorToolMO");
			pAsset->SetRectPath(RectButtonMoPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelHlPath, PackageId);
			pAsset->SetName("cursorToolHL");
			pAsset->SetRectPath(RectButtonMoPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelRoPath, PackageId);
			pAsset->SetName("cursorToolRO");
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(vec4(0.8f, 0.8f, 0.8f, 1.0f));
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonPath, PackageId);
			pAsset->SetName("cursorTool");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelMoPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonHlPath, PackageId);
			pAsset->SetName("cursorToolHL");
			pAsset->SetIdleStylePath(LabelHlPath);
			pAsset->SetMouseOverStylePath(LabelHlPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
		
		{
			CAsset_GuiToggleStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiToggleStyle>(&TogglePath, PackageId);
			pAsset->SetName("toolbar");
			pAsset->SetIdleTrueStylePath(LabelHlPath);
			pAsset->SetIdleFalseStylePath(LabelIdlePath);
			pAsset->SetMouseOverTrueStylePath(LabelMoPath);
			pAsset->SetMouseOverFalseStylePath(LabelMoPath);
		}
	}
	
	//Dialog Button
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelMoPath;
		CAssetPath LabelRoPath;
		CAssetPath ButtonPath;
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIdlePath, PackageId);
			pAsset->SetName("dialogButtonIdle");
			pAsset->SetRectPath(RectButtonPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
			pAsset->SetMinWidth(150);
			pAsset->SetMinHeight(20);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelMoPath, PackageId);
			pAsset->SetName("dialogButtonMO");
			pAsset->SetRectPath(RectButtonMoPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
			pAsset->SetMinWidth(150);
			pAsset->SetMinHeight(20);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelRoPath, PackageId);
			pAsset->SetName("dialogButtonRO");
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(vec4(0.8f, 0.8f, 0.8f, 1.0f));
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
			pAsset->SetMinWidth(150);
			pAsset->SetMinHeight(20);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonPath, PackageId);
			pAsset->SetName("dialog");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelMoPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
	}
	
	//Text Edit
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelRoPath;
		CAssetPath EditPath;
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIdlePath, PackageId);
			pAsset->SetName("textEditIdle");
			pAsset->SetRectPath(RectEditPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetMinHeight(16);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelRoPath, PackageId);
			pAsset->SetName("textEditRO");
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(vec4(0.8f, 0.8f, 0.8f, 1.0f));
			pAsset->SetMinHeight(16);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&EditPath, PackageId);
			pAsset->SetName("textEdit");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelIdlePath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
		
	}
	
	//Numeric Edit
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelRoPath;
		CAssetPath EditPath;
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIdlePath, PackageId);
			pAsset->SetName("numericEditIdle");
			pAsset->SetRectPath(RectEditPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelRoPath, PackageId);
			pAsset->SetName("numericEditRO");
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(vec4(0.8f, 0.8f, 0.8f, 1.0f));
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&EditPath, PackageId);
			pAsset->SetName("numericEdit");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelIdlePath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
	}
	
	//Asset Edit
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelMoPath;
		CAssetPath LabelRoPath;
		CAssetPath EditPath;
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIdlePath, PackageId);
			pAsset->SetName("assetEditIdle");
			pAsset->SetRectPath(RectButtonPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelMoPath, PackageId);
			pAsset->SetName("assetEditMO");
			pAsset->SetRectPath(RectButtonMoPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelRoPath, PackageId);
			pAsset->SetName("assetEditRO");
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(vec4(0.8f, 0.8f, 0.8f, 1.0f));
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&EditPath, PackageId);
			pAsset->SetName("assetEdit");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelMoPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
	}
	
	//Integer Edit
	{
		CAssetPath LabelIncrIdlePath;
		CAssetPath LabelIncrMoPath;
		CAssetPath ButtonIncrPath;
		CAssetPath LabelDecrIdlePath;
		CAssetPath LabelDecrMoPath;
		CAssetPath ButtonDecrPath;
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIncrIdlePath, PackageId);
			pAsset->SetName("intEditIncr");
			pAsset->SetRectPath(RectButtonPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
			pAsset->SetIconPath(IconIncreasePath);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIncrMoPath, PackageId);
			pAsset->SetName("intEditIncrMo");
			pAsset->SetRectPath(RectButtonMoPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
			pAsset->SetIconPath(IconIncreasePath);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonIncrPath, PackageId);
			pAsset->SetName("intEditIncr");
			pAsset->SetIdleStylePath(LabelIncrIdlePath);
			pAsset->SetMouseOverStylePath(LabelIncrMoPath);
		}
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelDecrIdlePath, PackageId);
			pAsset->SetName("intEditDecr");
			pAsset->SetRectPath(RectButtonPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
			pAsset->SetIconPath(IconDecreasePath);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelDecrMoPath, PackageId);
			pAsset->SetName("intEditDecrMo");
			pAsset->SetRectPath(RectButtonMoPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
			pAsset->SetIconPath(IconDecreasePath);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonDecrPath, PackageId);
			pAsset->SetName("intEditDecr");
			pAsset->SetIdleStylePath(LabelDecrIdlePath);
			pAsset->SetMouseOverStylePath(LabelDecrMoPath);
		}
		
		CAssetPath AssetPath;
		CAsset_GuiIntEditStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiIntEditStyle>(&AssetPath, PackageId);
		pAsset->SetName("default");
		pAsset->SetIncreaseButtonStylePath(ButtonIncrPath);
		pAsset->SetDecreaseButtonStylePath(ButtonDecrPath);
	}
	
	//Active List Item
	{
		CAssetPath LabelIdlePath;
		CAssetPath ButtonPath;
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIdlePath, PackageId);
			pAsset->SetName("listItemIdle");
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetTextColor(1.0f);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonPath, PackageId);
			pAsset->SetName("listItem");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelIdlePath);
			pAsset->SetReadOnlyStylePath(LabelIdlePath);
		}
	}
	
	//Active List Item HL
	{
		CAssetPath LabelIdlePath;
		CAssetPath ButtonPath;
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIdlePath, PackageId);
			pAsset->SetName("listItemHL");
			pAsset->SetRectPath(RectListItemHlPath);
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetTextColor(1.0f);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonPath, PackageId);
			pAsset->SetName("listItemHL");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelIdlePath);
			pAsset->SetReadOnlyStylePath(LabelIdlePath);
		}
	}
	
	//Inactive List Item
	{
		CAssetPath AssetPath;
		CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&AssetPath, PackageId);
		pAsset->SetName("inactiveListItem");
		pAsset->SetPadding(4);
		pAsset->SetSpacing(4);
		pAsset->SetTextColor(vec4(0.6f, 0.6f, 0.6f, 1.0f));
	}
	
	//Menu Item button and default toggle
	{
		CAssetPath IconFalsePath;
		CAssetPath IconTruePath;
		CAssetPath LabelIdlePath;
		CAssetPath LabelMoPath;
		CAssetPath LabelRoPath;
		CAssetPath ButtonPath;
		CAssetPath TogglePath;
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIdlePath, PackageId);
			pAsset->SetName("menuItem");
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetTextColor(1.0f);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelMoPath, PackageId);
			pAsset->SetName("menuItemMo");
			pAsset->SetRectPath(RectListItemHlPath);
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetTextColor(1.0f);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelRoPath, PackageId);
			pAsset->SetName("menuItemRo");
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetTextColor(vec4(0.8f, 0.8f, 0.8f, 1.0f));
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonPath, PackageId);
			pAsset->SetName("menuItem");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelMoPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(IconFalsePath, PackageId, "iconCheckboxFalse", ImageEditorPath, 30, 0, 1, 1)
		CREATE_SPRITE_PATH(IconTruePath, PackageId, "iconCheckboxTrue", ImageEditorPath, 31, 0, 1, 1)
		
		{
			CAsset_GuiToggleStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiToggleStyle>(&TogglePath, PackageId);
			pAsset->SetName("default");
			pAsset->SetIdleTrueStylePath(LabelIdlePath);
			pAsset->SetIdleFalseStylePath(LabelIdlePath);
			pAsset->SetMouseOverTrueStylePath(LabelMoPath);
			pAsset->SetMouseOverFalseStylePath(LabelMoPath);
			pAsset->SetSwitchIcon(true);
			pAsset->SetIconTruePath(IconTruePath);
			pAsset->SetIconFalsePath(IconFalsePath);
		}
	}
	
	//Scrollbar
	CAssetPath RailPath;
	{
		CAssetPath SpriteRailL;
		CAssetPath SpriteRailM;
		CAssetPath SpriteRailR;
		CAssetPath SpriteSliderL;
		CAssetPath SpriteSliderM;
		CAssetPath SpriteSliderR;
		CAssetPath SliderPath;
		CAssetPath ScrollbarPath;
		
		CREATE_SPRITE_PATH(SpriteRailL, PackageId, "scrollbarRailL", ImageBoxesPath, 0, 15, 1, 1);
		CREATE_SPRITE_PATH(SpriteRailM, PackageId, "scrollbarRailM" , ImageBoxesPath, 1, 15, 1, 1);
		CREATE_SPRITE_PATH(SpriteRailR, PackageId, "scrollbarRailR", ImageBoxesPath, 2, 15, 1, 1);
		CREATE_SPRITE_PATH(SpriteSliderL, PackageId, "scrollbarSliderL", ImageBoxesPath, 3, 15, 1, 1);
		CREATE_SPRITE_PATH(SpriteSliderM, PackageId, "scrollbarSliderM" , ImageBoxesPath, 4, 15, 1, 1);
		CREATE_SPRITE_PATH(SpriteSliderR, PackageId, "scrollbarSliderR", ImageBoxesPath, 5, 15, 1, 1);
		
		{
			CAsset_GuiLineStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLineStyle>(&RailPath, PackageId);
			pAsset->SetName("scrollbarRail");
			pAsset->SetFlags(CAsset_GuiLineStyle::FLAG_BORDER | CAsset_GuiLineStyle::FLAG_IMAGE);
			pAsset->SetBorderColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
			pAsset->SetImageLPath(SpriteRailL);
			pAsset->SetImageMPath(SpriteRailM);
			pAsset->SetImageRPath(SpriteRailR);
		}
		
		{
			CAsset_GuiLineStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLineStyle>(&SliderPath, PackageId);
			pAsset->SetName("scrollbarSlider");
			pAsset->SetFlags(CAsset_GuiLineStyle::FLAG_IMAGE);
			pAsset->SetImageLPath(SpriteSliderL);
			pAsset->SetImageMPath(SpriteSliderM);
			pAsset->SetImageRPath(SpriteSliderR);
		}
		
		{
			CAsset_GuiScrollbarStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiScrollbarStyle>(&ScrollbarPath, PackageId);
			pAsset->SetName("default");
			pAsset->SetDefaultRailPath(RailPath);
			pAsset->SetDefaultSliderPath(SliderPath);
			pAsset->SetMouseOverSliderPath(SliderPath);
			pAsset->SetRectPath(CAssetPath::Null());
			pAsset->SetMargin(0);
			pAsset->SetPadding(12);
		}
	}
		
	//GuiSliderStyles
	{
		CAssetPath CursorPath;
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(CursorPath, PackageId, "sliderCursor", ImageBoxesPath, 6, 15, 1, 1);
		
		{
			CAsset_GuiSliderStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiSliderStyle>(&AssetPath, PackageId);
			pAsset->SetName("default");
			pAsset->SetDefaultRailPath(RailPath);
			pAsset->SetCursorPath(CursorPath);
			pAsset->SetPadding(12);
		}
	}
	
	//View
	{
		CAssetPath RectPath;
		CAssetPath BoxPath;
		
		{
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectPath, PackageId);
			pAsset->SetName("view");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_BORDER);
			pAsset->SetBackgroundColor(ColorDarkBG);
			pAsset->SetBorderColor(ColorBorder);
			pAsset->SetBorderFlags(CAsset_GuiRectStyle::BORDERFLAG_ALL);
		}
		
		{
			CAsset_GuiBoxStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiBoxStyle>(&BoxPath, PackageId);
			pAsset->SetName("view");
			pAsset->SetRectPath(RectPath);
			pAsset->SetPadding(1);
		}
	}
	
	//SubList
	{
		CAssetPath RectPath;
		CAssetPath BoxPath;
		
		{
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectPath, PackageId);
			pAsset->SetName("subList");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_BORDER);
			pAsset->SetBackgroundColor(ColorLightBG);
			pAsset->SetBorderColor(ColorBorder);
			pAsset->SetBorderFlags(CAsset_GuiRectStyle::BORDERFLAG_ALL);
		}
		
		{
			CAsset_GuiBoxStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiBoxStyle>(&BoxPath, PackageId);
			pAsset->SetName("subList");
			pAsset->SetRectPath(RectPath);
			pAsset->SetPadding(0);
			pAsset->SetMargin(4);
		}
	}
	
	//Zone Icon
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelMoPath;
		CAssetPath ButtonPath;
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIdlePath, PackageId);
			pAsset->SetName("paletteIcon");
			pAsset->SetRectPath(RectButtonPath);
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetMargin(4);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelMoPath, PackageId);
			pAsset->SetName("paletteIconMO");
			pAsset->SetRectPath(RectButtonMoPath);
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetMargin(4);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER);
		}
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonPath, PackageId);
			pAsset->SetName("paletteIcon");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelMoPath);
		}
	}
	
	//Panel box
	{
		CAssetPath RectPath;
		CAssetPath BoxPath;
		
		{
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectPath, PackageId);
			pAsset->SetName("panel");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_BORDER);
			pAsset->SetBackgroundColor(ColorLightBG);
			pAsset->SetBorderColor(ColorBorder);
			pAsset->SetBorderFlags(CAsset_GuiRectStyle::BORDERFLAG_ALL);
		}
		
		{
			CAsset_GuiBoxStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiBoxStyle>(&BoxPath, PackageId);
			pAsset->SetName("panel");
			pAsset->SetRectPath(RectPath);
			pAsset->SetPadding(1);
		}
	}
	
	//Dialog
	CAssetPath DialogBoxPath;
	{
		
		{
			CAsset_GuiBoxStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiBoxStyle>(&DialogBoxPath, PackageId);
			pAsset->SetName("dialog");
			pAsset->SetRectPath(RectDialogPath);
			pAsset->SetPadding(6);
		}
	}
	
	//ColorEdit
	{
		CAssetPath SpriteRGBIconPath;
		CAssetPath SpriteHSVIconPath;
		CAssetPath SpriteSquareIconPath;
		CAssetPath SpriteWheelIconPath;
		CAssetPath LabelIdlePath;
		CAssetPath LabelMoPath;
		CAssetPath LabelRoPath;
		CAssetPath ButtonPath;
		CAssetPath ColorEditPath;
		
		CREATE_SPRITE_PATH(SpriteRGBIconPath, PackageId, "iconRGB", ImageEditorPath, 5, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteHSVIconPath, PackageId, "iconHSV", ImageEditorPath, 6, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteSquareIconPath, PackageId, "iconColorSquare", ImageEditorPath, 7, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteWheelIconPath, PackageId, "iconColorWheel", ImageEditorPath, 8, 0, 1, 1);
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIdlePath, PackageId);
			pAsset->SetName("colorEditButtonIdle");
			pAsset->SetRectPath(RectButtonPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetMinHeight(16);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelMoPath, PackageId);
			pAsset->SetName("colorEditButtonMO");
			pAsset->SetRectPath(RectButtonMoPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetMinHeight(16);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelRoPath, PackageId);
			pAsset->SetName("colorEditButtonRO");
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(vec4(0.8f, 0.8f, 0.8f, 1.0f));
			pAsset->SetMinHeight(16);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_BELOW_CENTER_EXTENDED);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonPath, PackageId);
			pAsset->SetName("colorEditButton");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelMoPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
		
		{
			CAsset_GuiColorEditStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiColorEditStyle>(&ColorEditPath, PackageId);
			pAsset->SetName("default");
			pAsset->SetButtonStylePath(ButtonPath);
			pAsset->SetPopupStylePath(DialogBoxPath);
			pAsset->SetRGBIconPath(SpriteRGBIconPath);
			pAsset->SetHSVIconPath(SpriteHSVIconPath);
			pAsset->SetSquareIconPath(SpriteSquareIconPath);
			pAsset->SetWheelIconPath(SpriteWheelIconPath);
		}
	}
	
	//Tabs
	{
		CAssetPath TabsPath;
		CAssetPath ActiveRectPath;
		CAssetPath ActiveButtonPath;
		CAssetPath ActiveLabelIdlePath;
		CAssetPath InactiveRectPath;
		CAssetPath InactiveButtonPath;
		CAssetPath InactiveLabelIdlePath;
		CAssetPath LayoutRectPath;
		CAssetPath LayoutBoxPath;
		CAssetPath ContentRectPath;
		CAssetPath ContentBoxPath;
		CAssetPath ListBoxPath;
		
		{
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&InactiveRectPath, PackageId);
			pAsset->SetName("tabsInactiveRect");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND);
			pAsset->SetBackgroundColor(ColorDarkBG);
		}
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&InactiveLabelIdlePath, PackageId);
			pAsset->SetName("tabsInactiveLabel");
			pAsset->SetRectPath(InactiveRectPath);
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetTextColor(1.0f);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&InactiveButtonPath, PackageId);
			pAsset->SetName("tabsInactiveButton");
			pAsset->SetIdleStylePath(InactiveLabelIdlePath);
			pAsset->SetMouseOverStylePath(InactiveLabelIdlePath);
			pAsset->SetReadOnlyStylePath(InactiveLabelIdlePath);
		}
		
		{
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&ActiveRectPath, PackageId);
			pAsset->SetName("tabsActiveRect");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND);
			pAsset->SetBackgroundColor(ColorLightBG);
		}
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&ActiveLabelIdlePath, PackageId);
			pAsset->SetName("tabsActiveLabel");
			pAsset->SetRectPath(ActiveRectPath);
			pAsset->SetMargin(1);
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetTextColor(1.0f);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ActiveButtonPath, PackageId);
			pAsset->SetName("tabsActiveButton");
			pAsset->SetIdleStylePath(ActiveLabelIdlePath);
			pAsset->SetMouseOverStylePath(ActiveLabelIdlePath);
			pAsset->SetReadOnlyStylePath(ActiveLabelIdlePath);
		}
		
		{
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&LayoutRectPath, PackageId);
			pAsset->SetName("tabsLayoutRect");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BORDER);
			pAsset->SetBorderFlags(CAsset_GuiRectStyle::BORDERFLAG_ALL);
			pAsset->SetBorderColor(ColorBorder);
		}
		
		{
			CAsset_GuiBoxStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiBoxStyle>(&LayoutBoxPath, PackageId);
			pAsset->SetName("tabsLayout");
			pAsset->SetRectPath(LayoutRectPath);
		}
		
		{
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&ContentRectPath, PackageId);
			pAsset->SetName("tabsContentRect");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND);
			pAsset->SetBackgroundColor(ColorLightBG);
		}
		
		{
			CAsset_GuiBoxStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiBoxStyle>(&ContentBoxPath, PackageId);
			pAsset->SetName("tabsContent");
			pAsset->SetRectPath(ContentRectPath);
			pAsset->SetPadding(5);
		}
		
		{
			CAsset_GuiBoxStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiBoxStyle>(&ListBoxPath, PackageId);
			pAsset->SetName("tabsList");
			pAsset->SetMargin(-1);
		}
		
		{
			CAsset_GuiTabsStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiTabsStyle>(&TabsPath, PackageId);
			pAsset->SetName("default");
			pAsset->SetInactiveButtonPath(InactiveButtonPath);
			pAsset->SetActiveButtonPath(ActiveButtonPath);
			pAsset->SetButtonListPath(ListBoxPath);
			pAsset->SetContentPath(ContentBoxPath);
			pAsset->SetLayoutPath(LayoutBoxPath);
			pAsset->SetButtonListFill(true);
			pAsset->SetButtonListText(true);
		}
	}
	
	//Menu
	{
		CAssetPath BoxPath;
		
		{
			CAsset_GuiBoxStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiBoxStyle>(&BoxPath, PackageId);
			pAsset->SetName("menu");
			pAsset->SetRectPath(RectDialogPath);
			pAsset->SetPadding(0);
			pAsset->SetMargin(0);
			pAsset->SetSpacing(0);
		}
	}
	
	//~ CREATE_SPRITE(PackageId, "gizmoFrame", ImageEditorPath, 0, 8, 2, 3);
	//~ CREATE_SPRITE(PackageId, "gizmoFrameSelect", ImageEditorPath, 2, 8, 2, 3);
	//~ CREATE_SPRITE(PackageId, "gizmoFrameAdd", ImageEditorPath, 4, 8, 2, 3);
	//~ CREATE_SPRITE(PackageId, "gizmoFrameDelete", ImageEditorPath, 6, 8, 2, 3);
	//~ CREATE_SPRITE(PackageId, "gizmoFrameColor", ImageEditorPath, 8, 8, 2, 3);
	//~ CREATE_SPRITE(PackageId, "gizmoFrameHidden", ImageEditorPath, 10, 8, 2, 3);
	//~ CREATE_SPRITE(PackageId, "gizmoFrameColoredFg", ImageEditorPath, 12, 8, 2, 3);
	//~ CREATE_SPRITE(PackageId, "gizmoFrameColoredBg", ImageEditorPath, 14, 8, 2, 3);
	
	//~ CREATE_SPRITE(PackageId, "gizmoDirAim", ImageEditorPath, 0, 12, 4, 4);
	//~ CREATE_SPRITE(PackageId, "gizmoDirMove", ImageEditorPath, 4, 12, 4, 4);
	//~ CREATE_SPRITE(PackageId, "gizmoDirHook", ImageEditorPath, 8, 12, 4, 4);
	
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	pKernel->Shutdown();
	delete pKernel;

	exit(EXIT_SUCCESS);
}
