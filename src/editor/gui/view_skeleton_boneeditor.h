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

#ifndef __CLIENT_ASSETSEDITOR_VIEWSKELETON_BONEEDITOR__
#define __CLIENT_ASSETSEDITOR_VIEWSKELETON_BONEEDITOR__

#include <editor/gui/view_skeleton.h>

class CCursorTool_SkeletonBoneEditor : public CViewSkeleton::CCursorTool
{
protected:
	enum
	{
		DRAGTYPE_NONE=0,
		DRAGTYPE_ANGLE_AND_LENGTH,
		DRAGTYPE_TRANSLATION,
		DRAGTYPE_LENGTH,
		DRAGTYPE_ANGLE,
	};
	
	int m_DragType;
	int m_Token;
	
public:
	CCursorTool_SkeletonBoneEditor(CViewSkeleton* pViewSkeleton);
	virtual void RenderView();
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void OnMouseMove();
};

#endif
