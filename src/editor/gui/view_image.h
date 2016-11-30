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

#ifndef __EDITOR_VIEWIMAGE__
#define __EDITOR_VIEWIMAGE__

#include <client/gui/listlayout.h>
#include <editor/gui/view.h>
#include <editor/gui/image_picker.h>

class CViewImage : public CViewManager::CView
{
protected:
	CImagePicker* m_pImageWidget;
	
public:
	CViewImage(CGuiEditor* pAssetsEditor);
	virtual void RenderView();
	virtual void Destroy();
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const gui::CRect& BoundingRect, const gui::CRect& VisibilityRect);
	virtual void Update(bool ParentEnabled);
};

#endif
