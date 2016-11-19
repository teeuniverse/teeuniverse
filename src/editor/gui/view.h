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

#ifndef __EDITOR_VIEW__
#define __EDITOR_VIEW__

#include <client/gui/widget.h>
#include <client/gui/listlayout.h>
#include <client/gui/button.h>

class CViewManager : public gui::CWidget
{
public:
	class CView : public gui::CWidget
	{
	public:
		class CCursorTool : public gui::CButton
		{
		private:
			CView* m_pView;
			bool m_OnUse;
		
		protected:	
			virtual void MouseClickAction();
		
		public:
			CCursorTool(CView* pView, const char* pName, CAssetPath IconPath);
			
			virtual void Update(bool ParentEnabled);
			
			virtual void OnUse(bool Used);
			virtual void UpdateView() {};
			virtual void RenderView() {};
			virtual void OnViewMouseMove() {};
			virtual void OnViewButtonClick(int Button) {};
			virtual void OnViewButtonRelease(int Button) {};
		
			class CGuiEditor* AssetsEditor() { return m_pView->AssetsEditor(); }
		};
	
	protected:
		class CGuiEditor* m_pAssetsEditor;
		gui::CHListLayout* m_pToolbar;
		gui::CRect m_ViewRect;
		CCursorTool* m_pCursorTool;
	
	public:
		CView(CGuiEditor* pAssetsEditor);
		~CView();
		
		virtual void Update(bool ParentEnabled);
		virtual void UpdateBoundingSize();
		virtual void UpdatePosition(const gui::CRect& BoundingRect, const gui::CRect& VisibilityRect);
		virtual void RenderView() = 0;
		virtual void Render();
		virtual void OnMouseMove();
		virtual void OnButtonClick(int Button);
		virtual void OnButtonRelease(int Button);
		
		void SetCursorTool(CCursorTool* pCursorTool);
		CCursorTool* GetCursorTool() const { return m_pCursorTool; }
		
		inline const gui::CRect& GetViewRect() const { return m_ViewRect; }
		
		class CGuiEditor* AssetsEditor() { return m_pAssetsEditor; }
	};
	
private:
	class CGuiEditor* m_pAssetsEditor;
	CAssetPath m_BoxStylePath;
	CView* m_pCurrentView;
	CView* m_pViewImage;
	CView* m_pViewSprite;
	CView* m_pViewMap;
	CView* m_pViewGui;
	
public:
	CViewManager(CGuiEditor* pAssetsEditor);
	
	virtual void Destroy();
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const gui::CRect& BoundingRect, const gui::CRect& VisibilityRect);
	virtual void Update(bool ParentEnabled);
	virtual void Render();
	virtual void OnMouseMove();
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	virtual void OnInputEvent(const CInput::CEvent& Event);
	
	class CGuiEditor* AssetsEditor() { return m_pAssetsEditor; }
};

#endif
