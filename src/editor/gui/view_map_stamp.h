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

#ifndef TU_CLIENT_ASSETSEDITOR_VIEWMAP_STAMP_H
#define TU_CLIENT_ASSETSEDITOR_VIEWMAP_STAMP_H

#include <editor/gui/view_map.h>
#include <generated/assets/maplayertiles.h>

class CCursorTool_TileStamp : public CViewMap::CCursorTool
{
protected:
	bool m_DragEnabled;
	bool m_SelectionEnabled;
	array2d< CAsset_MapLayerTiles::CTile, allocator_copy<CAsset_MapLayerTiles::CTile> > m_Selection;
	vec2 m_Pivot;
	int m_Token;
	
public:
	CCursorTool_TileStamp(CViewMap* pViewMap);
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void RenderView();
	virtual void Update(bool ParentEnabled);
	void PaletteCallback_SelectZoneType(CAssetPath ZoneTypePath, CSubPath Index);
};

#endif
