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

#include <editor/gui/view_image.h>
#include <editor/components/gui.h>
#include <shared/components/assetsmanager.h>
#include <client/components/assetsrenderer.h>
#include <client/components/graphics.h>

/* VIEW IMAGE *********************************************************/

CViewImage::CViewImage(CGuiEditor* pAssetsEditor) :
	CViewManager::CView(pAssetsEditor),
	m_pImageWidget(NULL)
{
	m_pImageWidget = new CImagePicker(AssetsEditor(), AssetsEditor()->GetEditedAssetPath());
}

void CViewImage::Destroy()
{
	if(m_pImageWidget)
		m_pImageWidget->Destroy();
	
	CViewManager::CView::Destroy();
}

void CViewImage::UpdateBoundingSize()
{
	if(m_pImageWidget)
		m_pImageWidget->UpdateBoundingSize();
	
	CViewManager::CView::UpdateBoundingSize();
}

void CViewImage::UpdatePosition(const gui::CRect& BoundingRect, const gui::CRect& VisibilityRect)
{
	CViewManager::CView::UpdatePosition(BoundingRect, VisibilityRect);
	
	if(m_pImageWidget)
		m_pImageWidget->UpdatePosition(BoundingRect, m_VisibilityRect);
}

void CViewImage::Update(bool ParentEnabled)
{
	if(m_pImageWidget)
	{
		m_pImageWidget->SetImage(AssetsEditor()->GetEditedAssetPath());
		m_pImageWidget->Update(ParentEnabled);
	}
	
	CViewManager::CView::Update(ParentEnabled);
}

void CViewImage::RenderView()
{
	if(m_pImageWidget)
		m_pImageWidget->Render();
}
