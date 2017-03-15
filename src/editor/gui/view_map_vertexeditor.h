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

#ifndef __CLIENT_ASSETSEDITOR_VIEWMAP_VERTEXEDITOR__
#define __CLIENT_ASSETSEDITOR_VIEWMAP_VERTEXEDITOR__

#include <editor/gui/view_map.h>
#include <editor/gui/view_map_picker.h>
#include <shared/components/assetsmanager.h>
#include <client/components/assetsrenderer.h>
#include <editor/components/gui.h>
#include <client/maprenderer.h>

class CCursorTool_MapVertexEditor : public CCursorTool_MapPicker
{
protected:
	CSubPath m_CurrentVertex;
	vec2 m_ClickDiff;
	int m_DragType;
	int m_Vertex;
	int m_Token;

protected:	
	template<typename ASSET> vec2 PickVertex_Objects_Impl(vec2 PickPosition);
	template<typename ASSET> bool PickNewVertex_Objects_Impl(vec2 PickPosition, vec2& VertexPosition, CSubPath& PreviousVertex, CSubPath& NextVertex);	
	template<typename ASSET> void RenderView_Objects_Impl();
	template<typename ASSET> void OnViewMouseMove_Objects_Impl();
	template<typename ASSET> void OnViewButtonClick_Objects_Impl(int Button);
	
public:
	CCursorTool_MapVertexEditor(CViewMap* pViewMap);
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void RenderView();
	virtual void Update(bool ParentEnabled);
	virtual void OnMouseMove();
};

#endif
