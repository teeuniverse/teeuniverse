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

#ifndef __CLIENT_ASSETSEDITOR_VIEWMAP_LINEDRAWER__
#define __CLIENT_ASSETSEDITOR_VIEWMAP_LINEDRAWER__

#include <editor/gui/view_map.h>
#include <editor/gui/view_map_picker.h>
#include <shared/components/assetsmanager.h>
#include <client/components/assetsrenderer.h>
#include <editor/components/gui.h>
#include <client/maprenderer.h>

class CCursorTool_MapLineDrawer : public CCursorTool_MapPicker
{
protected:
	CSubPath m_CurrentObject;
	CAssetPath m_CurrentAssetPath;

protected:
	template<typename ASSET> void UpdateBarycenter_Objects_Impl(int Token);
	template<typename ASSET> void OnViewButtonClick_Objects_Impl(int Button);
	template<typename ASSET> void RenderView_Objects_Impl();
	
	void Reset();
	
public:
	CCursorTool_MapLineDrawer(CViewMap* pViewMap);
	virtual void OnViewButtonClick(int Button);
	virtual void RenderView();
	virtual void Update(bool ParentEnabled);
	virtual void OnMouseMove();
	virtual void OnInputEvent(const CInput::CEvent& Event);
};

#endif
