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

#include <editor/gui/view_skeleton_boneeditor.h>
#include <editor/components/gui.h>
#include <client/skeletonrenderer.h>
#include <client/components/assetsrenderer.h>
#include <shared/math/color.h>

/* CURSORTOOL BONE CREATOR ********************************************/

CCursorTool_SkeletonBoneEditor::CCursorTool_SkeletonBoneEditor(CViewSkeleton* pViewSkeleton) :
	CViewSkeleton::CCursorTool(pViewSkeleton, _LSTRING("Bone creator"), pViewSkeleton->AssetsEditor()->m_Path_Sprite_IconBoneCreator),
	m_DragType(DRAGTYPE_NONE),
	m_Token(CAssetsHistory::NEW_TOKEN)
{
	
}

void CCursorTool_SkeletonBoneEditor::OnViewButtonClick(int Button)
{
	if(!ViewSkeleton()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	const CAsset_Skeleton* pSkeleton = AssetsManager()->GetAsset<CAsset_Skeleton>(AssetsEditor()->GetEditedAssetPath());
	if(!pSkeleton)
		return;
	
	m_DragType = DRAGTYPE_NONE;
	
	vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	
	if(AssetsEditor()->GetFirstEditedSubPath().GetType() == CAsset_Skeleton::TYPE_BONE && pSkeleton->IsValidBone(AssetsEditor()->GetFirstEditedSubPath()))
	{
		vec2 TranslationGizmo = ViewSkeleton()->SkeletonRenderer()->BonePosToScreenPos(AssetsEditor()->GetFirstEditedSubPath(), vec2(0.0f, 0.0f));
		float Length = pSkeleton->GetBoneLength(AssetsEditor()->GetFirstEditedSubPath());
		vec2 LengthGizmo = ViewSkeleton()->SkeletonRenderer()->BonePosToScreenPos(AssetsEditor()->GetFirstEditedSubPath(), vec2(Length, 0.0f));
		
		//Check translation gizmo
		if(distance(TranslationGizmo, MousePos) < 16.0f)
		{
			m_DragType = DRAGTYPE_TRANSLATION;
		}
		else if(distance(LengthGizmo, MousePos) < 16.0f)
		{
			m_DragType = DRAGTYPE_LENGTH;
		}
	}
	
	if(m_DragType == DRAGTYPE_NONE)
	{
		CSubPath PickedBone = ViewSkeleton()->PickBone(MousePos);
		
		if(Input()->KeyIsPressed(KEY_LSHIFT))
		{
			CSubPath CreatedBone;
			
			if(PickedBone.IsNull())
			{
				//Create new bone parent
				
					//Find a color
				int NumParentBones = 0;
				CAsset_Skeleton::CIteratorBone Iter;
				for(Iter = pSkeleton->BeginBone(); Iter != pSkeleton->EndBone(); ++Iter)
				{
					if(pSkeleton->GetBoneParent(*Iter).IsNull())
						NumParentBones++;
				}
				vec4 Color = ColorSequence(NumParentBones, 1.0f, 1.0f, 1.0f);
				
					//Create
				m_Token = AssetsManager()->GenerateToken();
				CreatedBone = CAsset_Skeleton::SubPath_Bone(AssetsManager()->AddSubItem(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_Skeleton::TYPE_BONE, m_Token));
				
					//Set default values
				vec2 Position = ViewSkeleton()->SkeletonRenderer()->ScreenPosToSkeletonPos(MousePos);
				AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), CreatedBone, CAsset_Skeleton::BONE_TRANSLATION, Position, m_Token);
				AssetsManager()->SetAssetValue<vec4>(AssetsEditor()->GetEditedAssetPath(), CreatedBone, CAsset_Skeleton::BONE_COLOR, Color, m_Token);
			}
			else if(pSkeleton->IsValidBone(PickedBone))
			{
				//Create new child bone
				
					//Find a color
				vec4 Color =  pSkeleton->GetBoneColor(PickedBone) * vec4(0.8f, 0.8f, 0.8f, 1.0f);
				
					//Create
				m_Token = AssetsManager()->GenerateToken();
				CreatedBone = CAsset_Skeleton::SubPath_Bone(AssetsManager()->AddSubItem(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_Skeleton::TYPE_BONE, m_Token));
				
					//Sete default values
				AssetsManager()->SetAssetValue<CSubPath>(AssetsEditor()->GetEditedAssetPath(), CreatedBone, CAsset_Skeleton::BONE_PARENT, PickedBone, m_Token);
				AssetsManager()->SetAssetValue<vec4>(AssetsEditor()->GetEditedAssetPath(), CreatedBone, CAsset_Skeleton::BONE_COLOR, Color, m_Token);
			}
			
			if(CreatedBone.IsNotNull())
			{
				char aBuf[128];
				str_format(aBuf, sizeof(aBuf), "bone%d", CreatedBone.GetId());
				AssetsManager()->SetAssetValue<const char*>(AssetsEditor()->GetEditedAssetPath(), CreatedBone, CAsset_Skeleton::BONE_NAME, aBuf, m_Token);
				
				AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), CreatedBone, CAsset_Skeleton::BONE_ANCHOR, 1.0f, m_Token);
				
				AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), CreatedBone);
				m_DragType = DRAGTYPE_ANGLE_AND_LENGTH;

				CAssetState* pState = AssetsManager()->GetAssetState(AssetsEditor()->GetEditedAssetPath());
				pState->m_NumUpdates++;
			}
		}
		else
		{
			AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), PickedBone);
			m_DragType = DRAGTYPE_ANGLE;
		}
	}
}
	
void CCursorTool_SkeletonBoneEditor::OnViewButtonRelease(int Button)
{
	if(Button == KEY_MOUSE_1)
	{
		m_DragType = DRAGTYPE_NONE;
		m_Token = CAssetsHistory::NEW_TOKEN;
	}
}
	
void CCursorTool_SkeletonBoneEditor::OnViewMouseMove()
{
	const CAsset_Skeleton* pSkeleton = AssetsManager()->GetAsset<CAsset_Skeleton>(AssetsEditor()->GetEditedAssetPath());
	if(!pSkeleton)
		return;
	
	if(AssetsEditor()->GetFirstEditedSubPath().GetType() != CAsset_Skeleton::TYPE_BONE || !pSkeleton->IsValidBone(AssetsEditor()->GetFirstEditedSubPath()))
		return;
	
	ViewSkeleton()->RefreshSkeletonRenderer();
	
	vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	vec2 MouseBonePos = ViewSkeleton()->SkeletonRenderer()->ScreenPosToBonePos(AssetsEditor()->GetFirstEditedSubPath(), MousePos);
	
	if(m_DragType == DRAGTYPE_ANGLE_AND_LENGTH)
	{
		float Length = length(MouseBonePos);
		float Angle = pSkeleton->GetBoneAngle(AssetsEditor()->GetFirstEditedSubPath()) + angle(MouseBonePos);
		AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), AssetsEditor()->GetFirstEditedSubPath(), CAsset_Skeleton::BONE_LENGTH, Length, m_Token);
		AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), AssetsEditor()->GetFirstEditedSubPath(), CAsset_Skeleton::BONE_ANGLE, Angle, m_Token);
	}
	else if(m_DragType == DRAGTYPE_ANGLE)
	{
		float Angle = pSkeleton->GetBoneAngle(AssetsEditor()->GetFirstEditedSubPath()) + angle(MouseBonePos);
		AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), AssetsEditor()->GetFirstEditedSubPath(), CAsset_Skeleton::BONE_ANGLE, Angle, m_Token);
	}
	else if(m_DragType == DRAGTYPE_LENGTH)
	{
		float Length = length(MouseBonePos);
		AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), AssetsEditor()->GetFirstEditedSubPath(), CAsset_Skeleton::BONE_LENGTH, Length, m_Token);
	}
	else if(m_DragType == DRAGTYPE_TRANSLATION)
	{
		vec2 Position = pSkeleton->GetBoneTranslation(AssetsEditor()->GetFirstEditedSubPath()) + MouseBonePos;
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), AssetsEditor()->GetFirstEditedSubPath(), CAsset_Skeleton::BONE_TRANSLATION, Position, m_Token);
	}
}
	
void CCursorTool_SkeletonBoneEditor::RenderView()
{
	const CAsset_Skeleton* pSkeleton = AssetsManager()->GetAsset<CAsset_Skeleton>(AssetsEditor()->GetEditedAssetPath());
	if(!pSkeleton)
		return;
	
	if(AssetsEditor()->GetFirstEditedSubPath().GetType() != CAsset_Skeleton::TYPE_BONE || !pSkeleton->IsValidBone(AssetsEditor()->GetFirstEditedSubPath()))
		return;
	
	vec2 PivotPosition = ViewSkeleton()->SkeletonRenderer()->BonePosToScreenPos(AssetsEditor()->GetFirstEditedSubPath(), vec2(0.0f, 0.0f));
	float Length = pSkeleton->GetBoneLength(AssetsEditor()->GetFirstEditedSubPath());
	vec2 LengthPosition = ViewSkeleton()->SkeletonRenderer()->BonePosToScreenPos(AssetsEditor()->GetFirstEditedSubPath(), vec2(Length, 0.0f));
	float LengthAngle = angle(LengthPosition - PivotPosition);
	
	AssetsRenderer()->DrawSprite(
		AssetsEditor()->m_Path_Sprite_GizmoPivot,
		PivotPosition,
		1.0f, 0.0, 0x0, 1.0f
	);
	AssetsRenderer()->DrawSprite(
		AssetsEditor()->m_Path_Sprite_GizmoBoneLength,
		LengthPosition,
		1.0f, LengthAngle, 0x0, 1.0f
	);
}

void CCursorTool_SkeletonBoneEditor::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewSkeleton()->AssetsEditor()->SetHint(_LSTRING("Bone editor: move, rotate and scale bones"));
	
	CViewSkeleton::CCursorTool::OnMouseMove();
}
