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

#ifndef TU_CLIENT_ASSETSEDITOR_VIEWMAP_QUADTRANSFORM_H
#define TU_CLIENT_ASSETSEDITOR_VIEWMAP_QUADTRANSFORM_H

#include <editor/gui/view_map.h>

class CCursorTool_QuadTransform : public CViewMap::CCursorTool
{
public:
	enum
	{
		GIZMOTYPE_NONE=0,
		GIZMOTYPE_ROTATION,
		GIZMOTYPE_SCALE,
	};
	
	enum
	{
		DRAGTYPE_NONE=0,
		DRAGTYPE_GIZMO,
		DRAGTYPE_TRANSLATION,
	};
	
protected:
	CSubPath m_SelectedQuad;
	int m_SelectedGizmo;
	int m_GizmoType;
	int m_DragType;
	vec2 m_ClickDiff;
	bool m_Transformed;
	bool m_FrameTransform;
	int m_Token;

protected:
	CSubPath PickQuad(vec2 CursorPos);
	void RenderPivots();
	
public:
	CCursorTool_QuadTransform(CViewMap* pViewMap);
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void RenderView();
	virtual void Update(bool ParentEnabled);
};

#endif
