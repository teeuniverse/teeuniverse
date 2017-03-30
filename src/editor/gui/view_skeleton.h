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

#ifndef __CLIENT_ASSETSEDITOR_VIEWSKELETON__
#define __CLIENT_ASSETSEDITOR_VIEWSKELETON__

#include <client/gui/listlayout.h>
#include <editor/gui/view.h>

class CViewSkeleton : public CViewManager::CView
{
public:
	class CCursorTool : public CViewManager::CView::CCursorTool
	{
	protected:
		CViewSkeleton* m_pViewSkeleton;

	public:
		CCursorTool(CViewSkeleton* pViewSkeleton, const CLocalizableString& LString, CAssetPath IconPath) :
			CViewManager::CView::CCursorTool(pViewSkeleton, LString, IconPath),
			m_pViewSkeleton(pViewSkeleton)
		{
			
		}
		
		inline CViewSkeleton* ViewSkeleton() { return m_pViewSkeleton; }
	};
	
protected:
	vec2 m_CameraPosition;
	float m_CameraZoom;
	bool m_CameraDraged;
	bool m_ZoomLockedToUnit;
	
	std::unique_ptr<class CSkeletonRenderer> m_pSkeletonRenderer;
	
	CView::CCursorTool* m_pCursorTool_SkeletonBoneEditor;
	
public:
	CViewSkeleton(CGuiEditor* pAssetsEditor);
	
	virtual void RenderView();
	virtual void Update(bool ParentEnabled);
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	virtual void OnMouseMove();
	
	inline class CSkeletonRenderer* SkeletonRenderer() { return m_pSkeletonRenderer.get(); }
	void RefreshSkeletonRenderer();
	
	float GetCameraZoom();
	void SetCameraZoom(float Value);
	void SetCameraZoomToUnit();
	
	CSubPath PickBone(vec2 Position);
};

#endif
