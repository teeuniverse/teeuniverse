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
#include <editor/gui/view_skeleton.h>
#include <editor/gui/view_skeleton_boneeditor.h>
#include <client/skeletonrenderer.h>
#include <client/gui/float-edit.h>
#include <client/gui/filler.h>

class CViewSkeleton_ZoomUnitButton : public gui::CButton
{
protected:
	CViewSkeleton* m_pViewSkeleton;	
	
protected:
	virtual void MouseClickAction()
	{
		m_pViewSkeleton->SetCameraZoomToUnit();
	}
	
public:
	CViewSkeleton_ZoomUnitButton(CViewSkeleton* pViewSkeleton) :
		gui::CButton(pViewSkeleton->Context(), pViewSkeleton->AssetsEditor()->m_Path_Sprite_IconZoomUnit),
		m_pViewSkeleton(pViewSkeleton)
	{
		
	}

	virtual void OnMouseMove()
	{
		if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
			m_pViewSkeleton->AssetsEditor()->SetHint(_LSTRING("Set the zoom to 100 %"));
		
		gui::CButton::OnMouseMove();
	}
};

class CViewSkeleton_ZoomEdit : public gui::CFloatEdit
{
protected:
	CViewSkeleton* m_pViewSkeleton;	

protected:
	virtual float GetValue() const
	{
		return m_pViewSkeleton->GetCameraZoom();
	}
	
	virtual void SetValue(float Value)
	{
		m_pViewSkeleton->SetCameraZoom(Value);
	}
	
public:
	CViewSkeleton_ZoomEdit(CViewSkeleton* pViewSkeleton) :
		gui::CFloatEdit(pViewSkeleton->Context(), 1.0f),
		m_pViewSkeleton(pViewSkeleton)
	{
		Percent();
	}
};

/* VIEW SKELETON ******************************************************/

CViewSkeleton::CViewSkeleton(CGuiEditor* pAssetsEditor) :
	CViewManager::CView(pAssetsEditor),
	m_CameraPosition(0.0f),
	m_CameraZoom(1.0f),
	m_CameraDraged(false),
	m_ZoomLockedToUnit(false),
	m_pCursorTool_SkeletonBoneEditor(nullptr)
{
	m_pSkeletonRenderer.reset(new CSkeletonRenderer(AssetsEditor()->EditorKernel()));
	
	m_pCursorTool_SkeletonBoneEditor = new CCursorTool_SkeletonBoneEditor(this);
	m_pToolbar->Add(m_pCursorTool_SkeletonBoneEditor);
	
	m_pCursorTool_SkeletonBoneEditor->UpdateToolbar();
	
	m_pToolbar->Add(new gui::CFiller(Context()), true);
	m_pToolbar->Add(new CViewSkeleton_ZoomUnitButton(this), false);
	m_pToolbar->Add(new CViewSkeleton_ZoomEdit(this), false, 75);
}

void CViewSkeleton::Update(bool ParentEnabled)
{
	CView::Update(ParentEnabled);
	
	if(!GetCursorTool() || GetCursorTool()->IsDisabled())
	{
		switch(AssetsEditor()->GetEditedAssetPath().GetType())
		{
			case CAsset_Skeleton::TypeId:
				SetCursorTool(m_pCursorTool_SkeletonBoneEditor);
				break;
		}
	}
	
	if(ParentEnabled && IsEnabled())
	{
		while(AssetsEditor()->m_BindCall_ZoomIn > 0)
		{
			AssetsEditor()->m_BindCall_ZoomIn--;
			SetCameraZoom(GetCameraZoom()*1.1f);
		}
		while(AssetsEditor()->m_BindCall_ZoomOut > 0)
		{
			AssetsEditor()->m_BindCall_ZoomOut--;
			SetCameraZoom(GetCameraZoom()/1.1f);
		}
		while(AssetsEditor()->m_BindCall_Zoom11 > 0)
		{
			AssetsEditor()->m_BindCall_Zoom11--;
			SetCameraZoomToUnit();
		}
	}
}

void CViewSkeleton::RefreshSkeletonRenderer()
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_Skeleton::TypeId)
		SkeletonRenderer()->SetSkeleton(AssetsEditor()->GetEditedAssetPath());
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_SkeletonSkin::TypeId)
	{
		const CAsset_SkeletonSkin* pSkeletonSkin = AssetsManager()->GetAsset<CAsset_SkeletonSkin>(AssetsEditor()->GetEditedAssetPath());
		if(pSkeletonSkin)
		{
			SkeletonRenderer()->SetSkeleton(pSkeletonSkin->GetSkeletonPath());
			SkeletonRenderer()->AddSkin(AssetsEditor()->GetEditedAssetPath());
		}
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_SkeletonAnimation::TypeId)
	{
		const CAsset_SkeletonAnimation* pSkeletonAnimation = AssetsManager()->GetAsset<CAsset_SkeletonAnimation>(AssetsEditor()->GetEditedAssetPath());
		if(pSkeletonAnimation)
		{
			SkeletonRenderer()->SetSkeleton(pSkeletonAnimation->GetSkeletonPath());
			SkeletonRenderer()->ApplyAnimation(AssetsEditor()->GetEditedAssetPath(), AssetsEditor()->GetTime());
		}
	}
	else
		return;
	
	SkeletonRenderer()->Finalize();
	SkeletonRenderer()->SetWorldTransform(matrix2x2::scaling(GetCameraZoom()), m_CameraPosition, vec2(m_ViewRect.x + m_ViewRect.w/2, m_ViewRect.y + m_ViewRect.h/2));
}

void CViewSkeleton::RenderView()
{
	RefreshSkeletonRenderer();
	
	SkeletonRenderer()->RenderBones(0.0f, 1.0f);
	SkeletonRenderer()->RenderSkins(0.0f, 1.0f);
}

void CViewSkeleton::OnButtonClick(int Button)
{
	float CameraZoom = GetCameraZoom();
	
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
	{
		if(Button == KEY_MOUSE_WHEEL_UP)
		{
			CameraZoom *= 1.1f;
			SetCameraZoom(CameraZoom);
		}
		else if(Button == KEY_MOUSE_WHEEL_DOWN)
		{
			CameraZoom /= 1.1f;
			SetCameraZoom(CameraZoom);
		}
		else if(Button == KEY_MOUSE_3 || (Button == KEY_MOUSE_1 && Input()->KeyIsPressed(KEY_LCTRL)))
		{
			m_CameraDraged = true;
			return;
		}
	}
	
	CView::OnButtonClick(Button);
}

void CViewSkeleton::OnButtonRelease(int Button)
{
	if(m_CameraDraged && (Button == KEY_MOUSE_1 || Button == KEY_MOUSE_3))
	{
		m_CameraDraged = false;
	}
	
	CView::OnButtonRelease(Button);
}

void CViewSkeleton::OnMouseMove()
{
	vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	matrix2x2 InverseTransform = matrix2x2::inverse(matrix2x2::scaling(GetCameraZoom()));
	vec2 Pos = (InverseTransform * MousePos) - m_CameraPosition;
	
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
	{
		CLocalizableString LString(_("Position: ({float:XCoord}   {float:YCoord})"));
		LString.AddDouble("XCoord", Pos.x);
		LString.AddDouble("YCoord", Pos.y);
		AssetsEditor()->SetCoordinates(LString);
	}
	
	if(m_CameraDraged)
	{
		vec2 PrevMousePos = MousePos - vec2(Context()->GetMouseRelPos().x, Context()->GetMouseRelPos().y);
		vec2 PrevPos = (InverseTransform * PrevMousePos) - m_CameraPosition;
		
		m_CameraPosition += Pos - PrevPos;
	}
	
	CView::OnMouseMove();
}

float CViewSkeleton::GetCameraZoom()
{
	return (m_ZoomLockedToUnit ? 1.0f : m_CameraZoom);
}

void CViewSkeleton::SetCameraZoom(float Value)
{
	Value = clamp(Value, 0.01f, 100.0f);
	m_ZoomLockedToUnit = false;
	m_CameraZoom = Value;
}

void CViewSkeleton::SetCameraZoomToUnit()
{
	m_ZoomLockedToUnit = !m_ZoomLockedToUnit;
}

CSubPath CViewSkeleton::PickBone(vec2 Position)
{
	return SkeletonRenderer()->BonePicking(0.0f, 1.0f, Position);
}
