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

int main(int argc, char* argv[])
{
	CSharedKernel* pKernel = new CSharedKernel();
	if(!pKernel->Init(argc, (const char**) argv))
	{
		dbg_msg("Kernel", "unable to initialize shared kernel");
		exit(EXIT_FAILURE);
	}
	
	int PackageId = pKernel->AssetsManager()->NewPackage("gui_editor");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	
	//Colors
	vec4 ColorBorder = vec4(40.0f/255.0f, 40.0f/255.0f, 40.0f/255.0f, 1.0f);
	vec4 ColorDarkBG = vec4(58.0f/255.0f, 58.0f/255.0f, 58.0f/255.0f, 1.0f);
	
	vec4 ColorLightBG = vec4(83.0f/255.0f, 83.0f/255.0f, 83.0f/255.0f, 1.0f);
	vec4 ColorLightHL = vec4(108.0f/255.0f, 108.0f/255.0f, 108.0f/255.0f, 1.0f);
	vec4 ColorFocus = vec4(0.0f, 172.0f/255.0f, 1.0f, 45.0f/255.0f);
	vec4 ColorFocusHL = vec4(0.0f, 172.0f/255.0f, 1.0f, 83.0f/255.0f);
	
	//Images
	CAssetPath ImageEditorPath = CreateNewImage(pKernel, PackageId, "editor", "datasrc/images/gui_editor.png", CStorage::TYPE_ABSOLUTE, 64, 32);
	CAssetPath ImageBoxesPath = CreateNewImage(pKernel, PackageId, "boxes", "datasrc/images/gui_editor_box.png", CStorage::TYPE_ABSOLUTE, 16, 16);
	CAssetPath ImageZoneTexturePath = CreateNewImage(pKernel, PackageId, "zoneTexture", "datasrc/images/gui_editor_zonetexture.png", CStorage::TYPE_ABSOLUTE, 1, 1);
	
	//Sprites
	CAssetPath IconDecreasePath;
	CAssetPath IconIncreasePath;
	CAssetPath IconVisibilePath;
	CAssetPath IconInvisibilePath;
	
	CREATE_SPRITE(PackageId, "iconVoid", ImageEditorPath, 0*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconDefault", ImageEditorPath, 1*2, 0*2, 1, 1);
	CREATE_SPRITE_PATH(IconDecreasePath, PackageId, "iconDecrease", ImageEditorPath, 2*2, 0*2, 2, 2);
	CREATE_SPRITE_PATH(IconIncreasePath, PackageId, "iconIncrease", ImageEditorPath, 3*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconDelete", ImageEditorPath, 4*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconEdit", ImageEditorPath, 9*2, 0*2, 2, 2);
	CREATE_SPRITE_PATH(IconVisibilePath, PackageId, "iconView", ImageEditorPath, 10*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconUp", ImageEditorPath, 11*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconDown", ImageEditorPath, 12*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconDuplicate", ImageEditorPath, 13*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconSave", ImageEditorPath, 14*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconLoad", ImageEditorPath, 15*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconFirstFrame", ImageEditorPath, 16*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconPrevFrame", ImageEditorPath, 17*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconPlay", ImageEditorPath, 18*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconPause", ImageEditorPath, 19*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconNextFram", ImageEditorPath, 20*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconLastFrame", ImageEditorPath, 21*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconVFlip", ImageEditorPath, 22*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconHFlip", ImageEditorPath, 23*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconRotateCW", ImageEditorPath, 24*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconRotateCCW", ImageEditorPath, 25*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconMoveBack", ImageEditorPath, 26*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconMoveFront", ImageEditorPath, 27*2, 0*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconNone", ImageEditorPath, 29*2, 0*2, 2, 2);
	
	CREATE_SPRITE(PackageId, "iconFolder", ImageEditorPath, 0*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconTiles", ImageEditorPath, 1*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconQuad", ImageEditorPath, 2*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconWeapon", ImageEditorPath, 3*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconMap", ImageEditorPath, 4*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconZoneTiles", ImageEditorPath, 5*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconZoneType", ImageEditorPath, 6*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconEntityType", ImageEditorPath, 7*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconAsset", ImageEditorPath, 8*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconNewAsset", ImageEditorPath, 9*2, 1*2, 2, 2);
	CREATE_SPRITE_PATH(IconInvisibilePath, PackageId, "iconHidden", ImageEditorPath, 10*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconEntities", ImageEditorPath, 11*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconNewFolder", ImageEditorPath, 12*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconLayer", ImageEditorPath, 13*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconBone", ImageEditorPath, 14*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconSprite", ImageEditorPath, 15*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconImage", ImageEditorPath, 16*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconSkeleton", ImageEditorPath, 17*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconSkeletonSkin", ImageEditorPath, 18*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconSkeletonAnimation", ImageEditorPath, 19*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconLayerAnimation", ImageEditorPath, 20*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconCharacter", ImageEditorPath, 21*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconCharacterPart", ImageEditorPath, 22*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconKeyFrameMove", ImageEditorPath, 23*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconKeyFrameAdd", ImageEditorPath, 24*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconKeyFrameDelete", ImageEditorPath, 25*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconKeyFrames", ImageEditorPath, 26*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconKeyFrameColor", ImageEditorPath, 27*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconKeyFrameBone", ImageEditorPath, 28*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconKeyFrameLayer", ImageEditorPath, 29*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconFolderEdited", ImageEditorPath, 30*2, 1*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconFolderReadOnly", ImageEditorPath, 31*2, 1*2, 2, 2);
	
	CREATE_SPRITE(PackageId, "iconTranslate", ImageEditorPath, 1*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconTranslateX", ImageEditorPath, 2*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconTranslateY", ImageEditorPath, 3*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconRotate", ImageEditorPath, 4*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconScale", ImageEditorPath, 5*2, 2, 2*2, 2);
	CREATE_SPRITE(PackageId, "iconScaleX", ImageEditorPath, 6*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconScaleY", ImageEditorPath, 7*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconBoneLength", ImageEditorPath, 8*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconBoneAdd", ImageEditorPath, 9*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconBoneDelete", ImageEditorPath, 10*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconAttach", ImageEditorPath, 11*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconGuiRect", ImageEditorPath, 12*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconGuiLine", ImageEditorPath, 13*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconGuiBox", ImageEditorPath, 14*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconGuiScrollbar", ImageEditorPath, 15*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconGuiTabs", ImageEditorPath, 16*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconSystem", ImageEditorPath, 17*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconGuiButton", ImageEditorPath, 18*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconGuiLabel", ImageEditorPath, 19*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconGuiToggle", ImageEditorPath, 20*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconGuiSlider", ImageEditorPath, 21*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconVertexCorner", ImageEditorPath, 22*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconVertexSmooth", ImageEditorPath, 23*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconPolygon", ImageEditorPath, 24*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconVertexFree", ImageEditorPath, 25*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconDeleteVertex", ImageEditorPath, 26*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconAddVertex", ImageEditorPath, 27*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconVertexAligned", ImageEditorPath, 28*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconVertex", ImageEditorPath, 29*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconVertexSymetric", ImageEditorPath, 30*2, 2*2, 2, 2);
	CREATE_SPRITE(PackageId, "iconLineStyle", ImageEditorPath, 31*2, 2*2, 2, 2);
	
	CREATE_SPRITE(PackageId, "iconMatLayerRepeat", ImageEditorPath, 0*2, 3*2, 4, 2);
	CREATE_SPRITE(PackageId, "iconMatLayerStretch", ImageEditorPath, 2*2, 3*2, 4, 2);
	
	CREATE_SPRITE(PackageId, "iconSpriteCreator", ImageEditorPath, 46, 26, 3, 3);
	CREATE_SPRITE(PackageId, "iconBigVFlip", ImageEditorPath, 49, 26, 3, 3);
	CREATE_SPRITE(PackageId, "iconBigHFlip", ImageEditorPath, 52, 26, 3, 3);
	CREATE_SPRITE(PackageId, "iconBigRotateCW", ImageEditorPath, 55, 26, 3, 3);
	CREATE_SPRITE(PackageId, "iconBigRotateCCW", ImageEditorPath, 58, 26, 3, 3);
	CREATE_SPRITE(PackageId, "iconBigMesh", ImageEditorPath, 61, 26, 3, 3);
	CREATE_SPRITE(PackageId, "iconGrid", ImageEditorPath, 40, 29, 3, 3);
	CREATE_SPRITE(PackageId, "iconCrop", ImageEditorPath, 43, 29, 3, 3);
	CREATE_SPRITE(PackageId, "iconErase", ImageEditorPath, 46, 29, 3, 3);
	CREATE_SPRITE(PackageId, "iconWeightVertex", ImageEditorPath, 49, 29, 3, 3);
	CREATE_SPRITE(PackageId, "iconPencil", ImageEditorPath, 52, 29, 3, 3);
	CREATE_SPRITE(PackageId, "iconMoveVertex", ImageEditorPath, 55, 29, 3, 3);
	CREATE_SPRITE(PackageId, "iconMove", ImageEditorPath, 58, 29, 3, 3);
	CREATE_SPRITE(PackageId, "iconStamp", ImageEditorPath, 61, 29,  3, 3);
	
	CREATE_SPRITE(PackageId, "gizmoVertexCornerBg", ImageEditorPath, 0*2, 11*2, 2, 2);
	CREATE_SPRITE(PackageId, "gizmoVertexCornerFg", ImageEditorPath, 1*2, 11*2, 2, 2);
	CREATE_SPRITE(PackageId, "gizmoPivot", ImageEditorPath, 2*2, 11*2, 2, 2);
	CREATE_SPRITE(PackageId, "gizmoScale", ImageEditorPath, 3*2, 11*2, 2, 2);
	CREATE_SPRITE(PackageId, "gizmoRotate", ImageEditorPath, 4*2, 11*2, 2, 2);
	CREATE_SPRITE(PackageId, "gizmoVertexSmoothBg", ImageEditorPath, 5*2, 11*2, 2, 2);
	CREATE_SPRITE(PackageId, "gizmoVertexSmoothFg", ImageEditorPath, 6*2, 11*2, 2, 2);
	CREATE_SPRITE(PackageId, "gizmoVertexBezierBg", ImageEditorPath, 7*2, 11*2, 2, 2);
	CREATE_SPRITE(PackageId, "gizmoVertexBezierFg", ImageEditorPath, 8*2, 11*2, 2, 2);
	CREATE_SPRITE(PackageId, "gizmoVertexControl", ImageEditorPath, 9*2, 11*2, 2, 2);
	
	//Rect
	CAssetPath RectStampPath;
	CAssetPath RectSelectionPath;
	CAssetPath RectDialogPath;
	CAssetPath RectButtonPath;
	CAssetPath RectButtonMoPath;
	CAssetPath RectButtonFocusPath;
	CAssetPath RectEditPath;
	CAssetPath RectEditMoPath;
	CAssetPath RectEditFocusPath;
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
		
		CREATE_SPRITE_PATH(SpriteTL, PackageId, "boxDialogTL", ImageBoxesPath, 12, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteT , PackageId, "boxDialogT" , ImageBoxesPath, 13, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteTR, PackageId, "boxDialogTR", ImageBoxesPath, 14, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteL,  PackageId, "boxDialogL" , ImageBoxesPath, 12, 1, 1, 1);
		CREATE_SPRITE_PATH(SpriteR,  PackageId, "boxDialogR" , ImageBoxesPath, 14, 1, 1, 1);
		CREATE_SPRITE_PATH(SpriteBL, PackageId, "boxDialogBL", ImageBoxesPath, 12, 2, 1, 1);
		CREATE_SPRITE_PATH(SpriteB,  PackageId, "boxDialogB" , ImageBoxesPath, 13, 2, 1, 1);
		CREATE_SPRITE_PATH(SpriteBR, PackageId, "boxDialogBR", ImageBoxesPath, 14, 2, 1, 1);
	
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
		
		CREATE_SPRITE_PATH(SpriteTL, PackageId, "boxButtonFocusTL", ImageBoxesPath, 3, 6, 1, 1);
		CREATE_SPRITE_PATH(SpriteT , PackageId, "boxButtonFocusT" , ImageBoxesPath, 4, 6, 1, 1);
		CREATE_SPRITE_PATH(SpriteTR, PackageId, "boxButtonFocusTR", ImageBoxesPath, 5, 6, 1, 1);
		CREATE_SPRITE_PATH(SpriteL,  PackageId, "boxButtonFocusL" , ImageBoxesPath, 3, 7, 1, 1);
		CREATE_SPRITE_PATH(SpriteR,  PackageId, "boxButtonFocusR" , ImageBoxesPath, 5, 7, 1, 1);
		CREATE_SPRITE_PATH(SpriteBL, PackageId, "boxButtonFocusBL", ImageBoxesPath, 3, 8, 1, 1);
		CREATE_SPRITE_PATH(SpriteB,  PackageId, "boxButtonFocusB" , ImageBoxesPath, 4, 8, 1, 1);
		CREATE_SPRITE_PATH(SpriteBR, PackageId, "boxButtonFocusBR", ImageBoxesPath, 5, 8, 1, 1);
	
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectButtonFocusPath, PackageId);
		pAsset->SetName("buttonFocus");
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
	{
		CAssetPath SpriteTL;
		CAssetPath SpriteT;
		CAssetPath SpriteTR;
		CAssetPath SpriteL;
		CAssetPath SpriteR;
		CAssetPath SpriteBL;
		CAssetPath SpriteB;
		CAssetPath SpriteBR;
		
		CREATE_SPRITE_PATH(SpriteTL, PackageId, "boxEditMoTL", ImageBoxesPath, 3, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteT , PackageId, "boxEditMoT" , ImageBoxesPath, 4, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteTR, PackageId, "boxEditMoTR", ImageBoxesPath, 5, 0, 1, 1);
		CREATE_SPRITE_PATH(SpriteL,  PackageId, "boxEditMoL" , ImageBoxesPath, 3, 1, 1, 1);
		CREATE_SPRITE_PATH(SpriteR,  PackageId, "boxEditMoR" , ImageBoxesPath, 5, 1, 1, 1);
		CREATE_SPRITE_PATH(SpriteBL, PackageId, "boxEditMoBL", ImageBoxesPath, 3, 2, 1, 1);
		CREATE_SPRITE_PATH(SpriteB,  PackageId, "boxEditMoB" , ImageBoxesPath, 4, 2, 1, 1);
		CREATE_SPRITE_PATH(SpriteBR, PackageId, "boxEditMoBR", ImageBoxesPath, 5, 2, 1, 1);
	
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectEditMoPath, PackageId);
		pAsset->SetName("editMo");
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
	{
		CAssetPath SpriteTL;
		CAssetPath SpriteT;
		CAssetPath SpriteTR;
		CAssetPath SpriteL;
		CAssetPath SpriteR;
		CAssetPath SpriteBL;
		CAssetPath SpriteB;
		CAssetPath SpriteBR;
		
		CREATE_SPRITE_PATH(SpriteTL, PackageId, "boxEditFocusTL", ImageBoxesPath, 0, 6, 1, 1);
		CREATE_SPRITE_PATH(SpriteT , PackageId, "boxEditFocusT" , ImageBoxesPath, 1, 6, 1, 1);
		CREATE_SPRITE_PATH(SpriteTR, PackageId, "boxEditFocusTR", ImageBoxesPath, 2, 6, 1, 1);
		CREATE_SPRITE_PATH(SpriteL,  PackageId, "boxEditFocusL" , ImageBoxesPath, 0, 7, 1, 1);
		CREATE_SPRITE_PATH(SpriteR,  PackageId, "boxEditFocusR" , ImageBoxesPath, 2, 7, 1, 1);
		CREATE_SPRITE_PATH(SpriteBL, PackageId, "boxEditFocusBL", ImageBoxesPath, 0, 8, 1, 1);
		CREATE_SPRITE_PATH(SpriteB,  PackageId, "boxEditFocusB" , ImageBoxesPath, 1, 8, 1, 1);
		CREATE_SPRITE_PATH(SpriteBR, PackageId, "boxEditFocusBR", ImageBoxesPath, 2, 8, 1, 1);
	
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectEditFocusPath, PackageId);
		pAsset->SetName("editFocus");
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
	
	CAssetPath RectListItemFocusPath;
	CAssetPath RectListItemHlPath;
	CAssetPath RectListItemHlFocusPath;
	{
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectListItemFocusPath, PackageId);
		pAsset->SetName("listItemFocus");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND);
		pAsset->SetBackgroundColor(ColorFocus);
	}
	{
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectListItemHlPath, PackageId);
		pAsset->SetName("listItemHl");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND);
		pAsset->SetBackgroundColor(ColorLightHL);
	}
	{
		CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectListItemHlFocusPath, PackageId);
		pAsset->SetName("listItemHlFocus");
		pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND);
		pAsset->SetBackgroundColor(ColorFocusHL);
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
	{
		CAssetPath AssetPath;
		CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&AssetPath, PackageId);
		pAsset->SetName("dialogMessage");
		pAsset->SetPadding(2);
		pAsset->SetSpacing(4);
		pAsset->SetFontSize(12);
		pAsset->SetTextColor(1.0f);
		pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
	}
	
	//Text Selection
	{
		//Rect
		CAssetPath RectPath;
		{
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectPath, PackageId);
			pAsset->SetName("textSelection");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_ROUNDCORNER);
			pAsset->SetBackgroundColor(ColorLightHL);
			pAsset->SetCornerRadius(3.0f);
			pAsset->SetCornerFlags(CAsset_GuiRectStyle::CORNERFLAG_ALL);
		}
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
		CAssetPath LabelFocusPath;
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelFocusPath, PackageId);
			pAsset->SetName("editorButtonFocus");
			pAsset->SetRectPath(RectButtonFocusPath);
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
			pAsset->SetFocusStylePath(LabelFocusPath);
		}
	}
	
	//Cursor Tools and toggle
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelMoPath;
		CAssetPath LabelHlPath;
		CAssetPath LabelRoPath;
		CAssetPath LabelFocusPath;
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelFocusPath, PackageId);
			pAsset->SetName("cursorToolFocus");
			pAsset->SetRectPath(RectButtonFocusPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonPath, PackageId);
			pAsset->SetName("cursorTool");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelMoPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
			pAsset->SetFocusStylePath(LabelFocusPath);
		}
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonHlPath, PackageId);
			pAsset->SetName("cursorToolHL");
			pAsset->SetIdleStylePath(LabelHlPath);
			pAsset->SetMouseOverStylePath(LabelHlPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
			pAsset->SetFocusStylePath(LabelFocusPath);
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
		CAssetPath LabelFocusPath;
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelFocusPath, PackageId);
			pAsset->SetName("dialogButtonFocus");
			pAsset->SetRectPath(RectButtonFocusPath);
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
			pAsset->SetFocusStylePath(LabelFocusPath);
		}
	}
	
	//Text Edit
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelMoPath;
		CAssetPath LabelFocusPath;
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelMoPath, PackageId);
			pAsset->SetName("textEditMo");
			pAsset->SetRectPath(RectEditMoPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
			pAsset->SetMinHeight(16);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelFocusPath, PackageId);
			pAsset->SetName("textEditFocus");
			pAsset->SetRectPath(RectEditFocusPath);
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
			pAsset->SetMouseOverStylePath(LabelMoPath);
			pAsset->SetFocusStylePath(LabelFocusPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
		
	}
	
	//Numeric Edit
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelMoPath;
		CAssetPath LabelFocusPath;
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelMoPath, PackageId);
			pAsset->SetName("numericEditMo");
			pAsset->SetRectPath(RectEditMoPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(vec4(0.8f, 0.8f, 0.8f, 1.0f));
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelFocusPath, PackageId);
			pAsset->SetName("numericEditFocus");
			pAsset->SetRectPath(RectEditFocusPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(vec4(0.8f, 0.8f, 0.8f, 1.0f));
			pAsset->SetTextAlignment(CAsset_GuiLabelStyle::TEXTALIGNMENT_CENTER);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelRoPath, PackageId);
			pAsset->SetName("numericEditRo");
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
			pAsset->SetMouseOverStylePath(LabelMoPath);
			pAsset->SetFocusStylePath(LabelFocusPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
	}
	
	//Asset Edit
	CAssetPath AssetEditPath;
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelMoPath;
		CAssetPath LabelFocusPath;
		CAssetPath LabelRoPath;
		
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
			pAsset->SetName("assetEditMo");
			pAsset->SetRectPath(RectButtonMoPath);
			pAsset->SetMargin(2);
			pAsset->SetPadding(2);
			pAsset->SetSpacing(4);
			pAsset->SetFontSize(12);
			pAsset->SetTextColor(1.0f);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelFocusPath, PackageId);
			pAsset->SetName("assetEditFocus");
			pAsset->SetRectPath(RectButtonFocusPath);
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
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&AssetEditPath, PackageId);
			pAsset->SetName("assetEdit");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelMoPath);
			pAsset->SetFocusStylePath(LabelFocusPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
	}
	
	//Integer Edit
	{
		CAssetPath LabelIncrIdlePath;
		CAssetPath LabelIncrMoPath;
		CAssetPath LabelIncrFocusPath;
		CAssetPath ButtonIncrPath;
		CAssetPath LabelDecrIdlePath;
		CAssetPath LabelDecrMoPath;
		CAssetPath LabelDecrFocusPath;
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIncrFocusPath, PackageId);
			pAsset->SetName("intEditIncrFocus");
			pAsset->SetRectPath(RectButtonFocusPath);
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
			pAsset->SetFocusStylePath(LabelIncrFocusPath);
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelDecrFocusPath, PackageId);
			pAsset->SetName("intEditDecrFocus");
			pAsset->SetRectPath(RectButtonFocusPath);
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
			pAsset->SetFocusStylePath(LabelDecrFocusPath);
		}
		
		CAssetPath AssetPath;
		CAsset_GuiIntEditStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiIntEditStyle>(&AssetPath, PackageId);
		pAsset->SetName("default");
		pAsset->SetIncreaseButtonStylePath(ButtonIncrPath);
		pAsset->SetDecreaseButtonStylePath(ButtonDecrPath);
	}
	
	//Active List Item
	CAssetPath ButtonActiveListItemPath;
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelFocusPath;
		CAssetPath LabelRoPath;
		CAssetPath ButtonPath;
		CAssetPath TogglePath;
		
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelIdlePath, PackageId);
			pAsset->SetName("listItemIdle");
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetTextColor(1.0f);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelFocusPath, PackageId);
			pAsset->SetName("listItemFocus");
			pAsset->SetRectPath(RectListItemFocusPath);
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetTextColor(1.0f);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelRoPath, PackageId);
			pAsset->SetName("listItemIdleRo");
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetTextColor(vec4(0.6f, 0.6f, 0.6f, 1.0f));
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonActiveListItemPath, PackageId);
			pAsset->SetName("listItem");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelIdlePath);
			pAsset->SetFocusStylePath(LabelFocusPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
		
		{
			CAsset_GuiToggleStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiToggleStyle>(&TogglePath, PackageId);
			pAsset->SetName("listVisibility");
			pAsset->SetIdleTrueStylePath(LabelIdlePath);
			pAsset->SetMouseOverTrueStylePath(LabelIdlePath);
			pAsset->SetIconTruePath(IconVisibilePath);
			pAsset->SetIdleFalseStylePath(LabelIdlePath);
			pAsset->SetMouseOverFalseStylePath(LabelIdlePath);
			pAsset->SetIconFalsePath(IconInvisibilePath);
			pAsset->SetSwitchIcon(true);
		}
	}
	
	//Active List Item HL
	{
		CAssetPath LabelIdlePath;
		CAssetPath LabelFocusPath;
		CAssetPath LabelRoPath;
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelFocusPath, PackageId);
			pAsset->SetName("listItemHLFocus");
			pAsset->SetRectPath(RectListItemHlFocusPath);
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetTextColor(1.0f);
		}
		{
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelRoPath, PackageId);
			pAsset->SetName("listItemHLRo");
			pAsset->SetRectPath(RectListItemHlPath);
			pAsset->SetPadding(4);
			pAsset->SetSpacing(4);
			pAsset->SetTextColor(vec4(0.6f, 0.6f, 0.6f, 1.0f));
		}
		
		{
			CAsset_GuiButtonStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiButtonStyle>(&ButtonPath, PackageId);
			pAsset->SetName("listItemHL");
			pAsset->SetIdleStylePath(LabelIdlePath);
			pAsset->SetMouseOverStylePath(LabelIdlePath);
			pAsset->SetFocusStylePath(LabelFocusPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
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
		CAssetPath LabelFocusPath;
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelFocusPath, PackageId);
			pAsset->SetName("menuItemFocus");
			pAsset->SetRectPath(RectListItemFocusPath);
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
			pAsset->SetFocusStylePath(LabelFocusPath);
			pAsset->SetReadOnlyStylePath(LabelRoPath);
		}
		CAssetPath AssetPath;
		
		CREATE_SPRITE_PATH(IconFalsePath, PackageId, "iconCheckboxFalse", ImageEditorPath, 30*2, 0, 2, 2)
		CREATE_SPRITE_PATH(IconTruePath, PackageId, "iconCheckboxTrue", ImageEditorPath, 31*2, 0, 2, 2)
		
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
	
	//Menu
	CAssetPath MenuBoxPath;
	{
		{
			CAsset_GuiBoxStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiBoxStyle>(&MenuBoxPath, PackageId);
			pAsset->SetName("menu");
			pAsset->SetRectPath(RectDialogPath);
			pAsset->SetPadding(0);
			pAsset->SetMargin(0);
			pAsset->SetSpacing(0);
		}
	}
	
	//Statusbar
	{
		CAssetPath RectPath;
		CAssetPath StatusBoxPath;
		
		{
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&RectPath, PackageId);
			pAsset->SetName("statusbar");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND | CAsset_GuiRectStyle::FLAG_BORDER);
			pAsset->SetBackgroundColor(ColorDarkBG);
			pAsset->SetBorderColor(ColorBorder);
			pAsset->SetBorderFlags(CAsset_GuiRectStyle::BORDERFLAG_ALL);
		}
		
		{
			CAsset_GuiBoxStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiBoxStyle>(&StatusBoxPath, PackageId);
			pAsset->SetName("statusbar");
			pAsset->SetRectPath(RectPath);
			pAsset->SetPadding(2);
			pAsset->SetMargin(0);
		}
	}
	
	//ComboBox
	{
		CAssetPath AssetPath;
		{
			CAsset_GuiComboBoxStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiComboBoxStyle>(&AssetPath, PackageId);
			pAsset->SetName("default");
			pAsset->SetButtonPath(AssetEditPath);
			pAsset->SetPopupPath(MenuBoxPath);
			pAsset->SetEnumPath(ButtonActiveListItemPath);
		}
	}
	
	//Separator
	{
		CAssetPath SpritePath;
		CAssetPath ScrollbarPath;
		
		CREATE_SPRITE_PATH(SpritePath, PackageId, "separator", ImageBoxesPath, 7, 15, 1, 1);
		
		{
			CAsset_GuiLineStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLineStyle>(&ScrollbarPath, PackageId);
			pAsset->SetName("separator");
			pAsset->SetFlags(CAsset_GuiLineStyle::FLAG_BORDER | CAsset_GuiLineStyle::FLAG_IMAGE);
			pAsset->SetBorderColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
			pAsset->SetImageMPath(SpritePath);
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
		CAssetPath LabelFocusPath;
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelFocusPath, PackageId);
			pAsset->SetName("paletteIconFocus");
			pAsset->SetRectPath(RectButtonFocusPath);
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
			pAsset->SetFocusStylePath(LabelFocusPath);
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
		CAssetPath LabelFocusPath;
		CAssetPath LabelRoPath;
		CAssetPath ButtonPath;
		CAssetPath ColorEditPath;
		
		CREATE_SPRITE_PATH(SpriteRGBIconPath, PackageId, "iconRGB", ImageEditorPath, 5*2, 0*2, 2, 2);
		CREATE_SPRITE_PATH(SpriteHSVIconPath, PackageId, "iconHSV", ImageEditorPath, 6*2, 0*2, 2, 2);
		CREATE_SPRITE_PATH(SpriteSquareIconPath, PackageId, "iconColorSquare", ImageEditorPath, 7*2, 0*2, 2, 2);
		CREATE_SPRITE_PATH(SpriteWheelIconPath, PackageId, "iconColorWheel", ImageEditorPath, 8*2, 0*2, 2, 2);
		
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&LabelFocusPath, PackageId);
			pAsset->SetName("colorEditButtonFocus");
			pAsset->SetRectPath(RectButtonFocusPath);
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
			pAsset->SetFocusStylePath(LabelFocusPath);
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
		CAssetPath ActiveRectFocusPath;
		CAssetPath ActiveButtonPath;
		CAssetPath ActiveLabelIdlePath;
		CAssetPath ActiveLabelFocusPath;
		CAssetPath InactiveRectPath;
		CAssetPath InactiveRectFocusPath;
		CAssetPath InactiveButtonPath;
		CAssetPath InactiveLabelIdlePath;
		CAssetPath InactiveLabelFocusPath;
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
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&InactiveRectFocusPath, PackageId);
			pAsset->SetName("tabsInactiveRectFocus");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND);
			pAsset->SetBackgroundColor(ColorFocus);
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&InactiveLabelFocusPath, PackageId);
			pAsset->SetName("tabsInactiveLabelFocus");
			pAsset->SetRectPath(InactiveRectFocusPath);
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
			pAsset->SetFocusStylePath(InactiveLabelFocusPath);
		}
		
		{
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&ActiveRectPath, PackageId);
			pAsset->SetName("tabsActiveRect");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND);
			pAsset->SetBackgroundColor(ColorLightBG);
		}
		{
			CAsset_GuiRectStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiRectStyle>(&ActiveRectFocusPath, PackageId);
			pAsset->SetName("tabsActiveRectFocus");
			pAsset->SetFlags(CAsset_GuiRectStyle::FLAG_BACKGROUND);
			pAsset->SetBackgroundColor(ColorFocusHL);
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
			CAsset_GuiLabelStyle* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_GuiLabelStyle>(&ActiveLabelFocusPath, PackageId);
			pAsset->SetName("tabsActiveLabelFocus");
			pAsset->SetRectPath(ActiveRectFocusPath);
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
			pAsset->SetFocusStylePath(ActiveLabelFocusPath);
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
