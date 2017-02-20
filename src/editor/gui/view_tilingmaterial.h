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

#ifndef __CLIENT_ASSETSEDITOR_VIEWTILINGMATERIAL__
#define __CLIENT_ASSETSEDITOR_VIEWTILINGMATERIAL__

#include <client/gui/listlayout.h>
#include <editor/gui/view.h>
#include <shared/tl/array2d.h>
#include <generated/assets/maplayertiles.h>

class CViewTilingMaterial : public CViewManager::CView
{	
protected:
	std::unique_ptr<class CMapRenderer> m_pMapRenderer;
	array2d<CAsset_MapLayerTiles::CTile> m_MapPreview;
	CAssetPath m_MaterialApplied;
	
public:
	CViewTilingMaterial(CGuiEditor* pAssetsEditor);
	virtual void OnButtonClick(int Button);
	virtual void RenderView();
	
	inline class CMapRenderer* MapRenderer() { return m_pMapRenderer.get(); }
	
	void GenerateMapPreview();
	void RefreshMapPreview();
};

#endif
