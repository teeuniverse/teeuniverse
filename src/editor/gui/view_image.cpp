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
#include <editor/gui/view_image_spritecreator.h>
#include <editor/gui/view_image_eraser.h>

class CImagePicker_ImageView : public CImagePicker
{
protected:
	CViewImage* m_pImageView;
	
protected:
	void OnImagePicked(int MinX, int MinY, int MaxX, int MaxY)
	{
		if(m_pImageView->GetCursorTool() == m_pImageView->m_pCursorTool_SpriteCreator)
			m_pImageView->m_pCursorTool_SpriteCreator->OnImagePicked(MinX, MinY, MaxX, MaxY);
		else if(m_pImageView->GetCursorTool() == m_pImageView->m_pCursorTool_Eraser)
			m_pImageView->m_pCursorTool_Eraser->OnImagePicked(MinX, MinY, MaxX, MaxY);
	}
	
public:
	CImagePicker_ImageView(CGuiEditor* pAssetsEditor, CViewImage* pImageView, CAssetPath ImagePath) :
		CImagePicker(pAssetsEditor, ImagePath),
		m_pImageView(pImageView)
	{
		EnableSelection();
	}
};

/* VIEW IMAGE *********************************************************/

CViewImage::CViewImage(CGuiEditor* pAssetsEditor) :
	CViewManager::CView(pAssetsEditor),
	m_pImageWidget(NULL),
	m_pCursorTool_SpriteCreator(NULL),
	m_pCursorTool_Eraser(NULL)
{
	m_pImageWidget = new CImagePicker_ImageView(AssetsEditor(), this, AssetsEditor()->GetEditedAssetPath());
	
	m_pCursorTool_SpriteCreator = new CCursorTool_ImageSpriteCreator(this);
	m_pToolbar->Add(m_pCursorTool_SpriteCreator);
	
	m_pCursorTool_Eraser = new CCursorTool_Eraser(this);
	m_pToolbar->Add(m_pCursorTool_Eraser);
	
	m_pCursorTool_SpriteCreator->UpdateToolbar();
	m_pCursorTool_Eraser->UpdateToolbar();
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
		m_pImageWidget->UpdatePosition(m_ViewRect, m_VisibilityRect);
}

void CViewImage::Update(bool ParentEnabled)
{
	if(m_pImageWidget)
	{
		m_pImageWidget->SetImage(AssetsEditor()->GetEditedAssetPath());
		m_pImageWidget->Update(ParentEnabled);
	}
	
	if(!GetCursorTool() || GetCursorTool()->IsDisabled())
	{
		switch(AssetsEditor()->GetEditedAssetPath().GetType())
		{
			case CAsset_Image::TypeId:
				SetCursorTool(m_pCursorTool_SpriteCreator);
				break;
		}
	}
	
	CViewManager::CView::Update(ParentEnabled);
}

void CViewImage::RenderView()
{
	if(m_pImageWidget)
		m_pImageWidget->Render();
}

void CViewImage::OnButtonClick(int Button)
{
	if(m_pImageWidget && m_ViewRect.IsInside(Context()->GetMousePos()))
		m_pImageWidget->OnButtonClick(Button);
	
	CViewManager::CView::OnButtonClick(Button);
}

void CViewImage::OnButtonRelease(int Button)
{
	if(m_pImageWidget)
		m_pImageWidget->OnButtonRelease(Button);
	
	CViewManager::CView::OnButtonRelease(Button);
}

void CViewImage::OnMouseMove()
{
	if(m_pImageWidget)
		m_pImageWidget->OnMouseMove();
	
	CViewManager::CView::OnMouseMove();
}

