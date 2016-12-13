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
public:
	class CCursorTool : public CViewManager::CView::CCursorTool
	{
	protected:
		CViewImage* m_pViewImage;

	public:
		CCursorTool(CViewImage* pViewImage, const CLocalizableString& LString, CAssetPath IconPath) :
			CViewManager::CView::CCursorTool(pViewImage, LString, IconPath),
			m_pViewImage(pViewImage)
		{
			
		}
		
		inline CViewImage* ViewImage() { return m_pViewImage; }
	};
	
protected:
	CImagePicker* m_pImageWidget;
	
	CView::CCursorTool* m_pCursorTool_SpriteCreator;
	
public:
	CViewImage(CGuiEditor* pAssetsEditor);
	virtual void RenderView();
	virtual void Destroy();
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const gui::CRect& BoundingRect, const gui::CRect& VisibilityRect);
	virtual void Update(bool ParentEnabled);
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	virtual void OnMouseMove();
};

#endif
