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

#ifndef __CLIENT_ASSETSEDITOR_VIEWMAP_PICKER__
#define __CLIENT_ASSETSEDITOR_VIEWMAP_PICKER__

#include <editor/gui/view_map.h>

class CCursorTool_MapPicker : public CViewMap::CCursorTool
{
protected:
	CSubPath Pick(vec2 CursorPos);
	
	void RenderPivots();
	template<typename ASSET> void RenderPivots_Objects_Impl();
	
	void RenderVertices();
	template<typename ASSET> void RenderVertices_Objects_Impl();
	
public:
	CCursorTool_MapPicker(CViewMap* pViewMap, const CLocalizableString& LString, CAssetPath IconPath);
};

#endif
