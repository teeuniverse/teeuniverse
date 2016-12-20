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

#ifndef __CLIENT_ASSETSEDITOR_VIEWMAP_OBJECT__
#define __CLIENT_ASSETSEDITOR_VIEWMAP_OBJECT__

#include <editor/gui/view_map.h>

class CCursorTool_MapObjectVertexPicker : public CViewMap::CCursorTool
{
protected:
	CSubPath m_CurrentVertex;
	
protected:
	vec2 PickVertex(vec2 Position);
	bool PickNewVertex(vec2 Position, vec2& VertexPosition, CSubPath& PreviousVertex, CSubPath& NextVertex);
	
public:
	CCursorTool_MapObjectVertexPicker(CViewMap* pViewMap, const CLocalizableString& LString, CAssetPath IconPath);
	virtual void RenderPivots();
	virtual void Update(bool ParentEnabled);
};

class CCursorTool_MapObjectAddVertex : public CCursorTool_MapObjectVertexPicker
{
protected:
	CSubPath m_CurrentObject;
	CAssetPath m_CurrentAssetPath;
	
public:
	CCursorTool_MapObjectAddVertex(CViewMap* pViewMap);
	virtual void Reset();
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void RenderView();
	virtual void Update(bool ParentEnabled);
	virtual void OnMouseMove();
	virtual void OnInputEvent(const CInput::CEvent& Event);
};

class CCursorTool_MapObjectEditVertex : public CCursorTool_MapObjectVertexPicker
{
protected:
	vec2 ClickDiff;
	int m_DragType;
	int m_Token;
	
public:
	CCursorTool_MapObjectEditVertex(CViewMap* pViewMap);
	virtual void RenderView();
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void OnMouseMove();
};

class CCursorTool_MapObjectWeightVertex : public CCursorTool_MapObjectVertexPicker
{
protected:
	vec2 ClickPos;
	bool m_Drag;
	int m_Token;
	
public:
	CCursorTool_MapObjectWeightVertex(CViewMap* pViewMap);
	virtual void RenderView();
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void OnMouseMove();
};

#endif
