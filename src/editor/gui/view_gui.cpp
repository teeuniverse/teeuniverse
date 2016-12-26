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

#include <editor/gui/view_gui.h>
#include <editor/components/gui.h>
#include <shared/components/assetsmanager.h>
#include <client/components/assetsrenderer.h>
#include <client/components/graphics.h>

/* VIEW GUI ***********************************************************/

CViewGui::CViewGui(CGuiEditor* pAssetsEditor) :
	CViewManager::CView(pAssetsEditor)
{

}

void CViewGui::RenderView()
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_GuiRectStyle::TypeId:
		{
			ivec2 Center = ivec2(m_ViewRect.x + m_ViewRect.w/2, m_ViewRect.y + m_ViewRect.h/2);
			ivec2 Pointer = AssetsEditor()->GetMousePos();
			
			int SizeX = std::abs(Center.x - Pointer.x);
			int SizeY = std::abs(Center.y - Pointer.y);
			
			if(!m_ViewRect.IsInside(Pointer.x, Pointer.y))
			{
				SizeX = m_ViewRect.w/4;
				SizeY = m_ViewRect.h/4;
			}
				
			gui::CRect Rect;
			Rect.x = Center.x - SizeX;
			Rect.y = Center.y - SizeY;
			Rect.w = SizeX*2;
			Rect.h = SizeY*2;
			
			AssetsRenderer()->DrawGuiRect(&Rect, AssetsEditor()->GetEditedAssetPath());
			break;
		}
		case CAsset_GuiLineStyle::TypeId:
		{
			ivec2 Center = ivec2(m_ViewRect.x + m_ViewRect.w/2, m_ViewRect.y + m_ViewRect.h/2);
			ivec2 Pointer = AssetsEditor()->GetMousePos();
			
			if(!m_ViewRect.IsInside(Pointer.x, Pointer.y))
			{
				Pointer = Center;
				Center.x -= m_ViewRect.w/4;
				Pointer.x += m_ViewRect.w/4;
			}
			
			AssetsRenderer()->DrawGuiLine(Center, Pointer, AssetsEditor()->GetEditedAssetPath());
			break;
		}
	}
}
