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

#ifndef __CLIENT_ASSETSEDITOR_VIEWMAP_QUAD__
#define __CLIENT_ASSETSEDITOR_VIEWMAP_QUAD__

#include <editor/gui/view_map.h>

class CCursorTool_QuadPicker : public CViewMap::CCursorTool
{
protected:
	CSubPath PickQuad(vec2 CursorPos);
	CSubPath PickEntity(vec2 CursorPos);
	void RenderPivots();
	
public:
	CCursorTool_QuadPicker(CViewMap* pViewMap, const char* pName, CAssetPath IconPath);
};

class CCursorTool_QuadTransform : public CCursorTool_QuadPicker
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
	int m_SelectedGizmo;
	int m_GizmoType;
	int m_DragType;
	vec2 m_ClickDiff;
	bool m_Transformed;
	bool m_FrameTransform;
	int m_Token;
	
public:
	CCursorTool_QuadTransform(CViewMap* pViewMap);
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void RenderView();
	virtual void Update(bool ParentEnabled);
};

class CCursorTool_QuadEdit : public CCursorTool_QuadPicker
{
public:
	enum
	{
		VERTEX0=0,
		VERTEX1,
		VERTEX2,
		VERTEX3,
		VERTEX_PIVOT,
		VERTEX_NONE,
	};
	
protected:
	vec2 m_ClickDiff;
	int m_Vertex;
	int m_Token;
	
public:
	CCursorTool_QuadEdit(CViewMap* pViewMap);
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void RenderView();
	virtual void Update(bool ParentEnabled);
};

#endif
