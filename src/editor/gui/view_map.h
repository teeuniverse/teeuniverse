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

#ifndef __CLIENT_ASSETSEDITOR_VIEWMAP__
#define __CLIENT_ASSETSEDITOR_VIEWMAP__

#include <client/gui/listlayout.h>
#include <editor/gui/view.h>

class CViewMap : public CViewManager::CView
{
public:
	class CCursorTool : public CViewManager::CView::CCursorTool
	{
	protected:
		CViewMap* m_pViewMap;

	public:
		CCursorTool(CViewMap* pViewMap, const char* pName, CAssetPath IconPath) :
			CViewManager::CView::CCursorTool(pViewMap, pName, IconPath),
			m_pViewMap(pViewMap)
		{
			
		}
		
		inline CViewMap* ViewMap() { return m_pViewMap; }
	};
	
protected:
	float m_CameraZoom;
	vec2 m_CameraPos;
	bool m_CameraDraged;
	float m_ZoneOpacity;
	
	class CMapRenderer* m_pMapRenderer;
	
	CView::CCursorTool* m_pCursorTool_MapStamp;
	CView::CCursorTool* m_pCursorTool_MapTransform;
	CView::CCursorTool* m_pCursorTool_MapEdit;
	CView::CCursorTool* m_pCursorTool_MapCrop;

public:
	CViewMap(CGuiEditor* pAssetsEditor);
	~CViewMap();
	virtual void Update(bool ParentEnabled);
	virtual void RenderView();
	virtual void OnMouseMove();
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	
	inline class CMapRenderer* MapRenderer() { return m_pMapRenderer; }
	
	CAssetPath GetMapPath();
	CAssetPath GetMapGroupPath();
	
	inline float GetZoneOpacity() const { return m_ZoneOpacity; }
	inline void SetZoneOpacity(float Value) { m_ZoneOpacity = Value; }
};

#endif
