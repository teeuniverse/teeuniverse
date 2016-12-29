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

#ifndef __CLIENT_ASSETSEDITOR_VIEWMATERIAL__
#define __CLIENT_ASSETSEDITOR_VIEWMATERIAL__

#include <client/gui/listlayout.h>
#include <editor/gui/view.h>

class CViewMaterial : public CViewManager::CView
{	
protected:
	bool m_ShowMeshes;
	int m_ObjectShape;
	
	class CMapRenderer* m_pMapRenderer;
	
public:
	CViewMaterial(CGuiEditor* pAssetsEditor);
	virtual ~CViewMaterial();
	virtual void RenderView();
	
	inline class CMapRenderer* MapRenderer() { return m_pMapRenderer; }
};

#endif
