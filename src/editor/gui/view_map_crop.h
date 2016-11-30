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

#ifndef __CLIENT_ASSETSEDITOR_VIEWMAP_CROP__
#define __CLIENT_ASSETSEDITOR_VIEWMAP_CROP__

#include <editor/gui/view_map.h>

class CCursorTool_MapCrop : public CViewMap::CCursorTool
{
protected:
	enum
	{
		DRAGTYPE_NONE=0,
		DRAGTYPE_TRANSLATION,
		DRAGTYPE_LEFT,
		DRAGTYPE_RIGHT,
		DRAGTYPE_TOP,
		DRAGTYPE_BOTTOM,
		DRAGTYPE_TL,
		DRAGTYPE_TR,
		DRAGTYPE_BL,
		DRAGTYPE_BR,
	};
	
protected:
	int m_Token;
	int m_DragType;
	vec2 m_CursorGizmoDiff;
	
public:
	CCursorTool_MapCrop(CViewMap* pViewMap);
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void RenderView();
	virtual void Update(bool ParentEnabled);
	virtual void OnMouseMove();
};

#endif
