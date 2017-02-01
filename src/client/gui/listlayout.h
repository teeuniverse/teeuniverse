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

#ifndef __CLIENT_GUI_LISTLAYOUT__
#define __CLIENT_GUI_LISTLAYOUT__

#include "widget.h"
#include "scrollbar.h"
	
namespace gui
{

enum
{
	LAYOUTFILLING_NONE=0,
	LAYOUTFILLING_FIRST,
	LAYOUTFILLING_LAST,
	LAYOUTFILLING_ALL,
};

class CAbstractListLayout : public CWidget
{
protected:	
	class CChild
	{
	public:
		CWidget* m_pWidget;
		bool m_Fill;
		int m_MinSize;
	};
	
protected:
	CAssetPath m_BoxStylePath;
	std::vector<CChild> m_Childs;
	CRect m_ClipRect;
	bool m_ClipEnabled;

public:
	CAbstractListLayout(class CGui *pConfig);
	
	virtual void Destroy();
	virtual void Update(bool ParentEnabled);
	virtual void Render();
	virtual void OnMouseMove();
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	virtual void OnInputEvent(const CInput::CEvent& Event);
	
	void Clear();
	void Add(CWidget* pWidget, bool Fill = false, int MinSize = 0);
	
	void SetBoxStyle(CAssetPath BoxStylePath) { m_BoxStylePath = BoxStylePath; }
	CAssetPath GetBoxStyle() const { return m_BoxStylePath; }
};

class CHListLayout : public CAbstractListLayout
{
protected:
	class CSeparator : public CWidget
	{
	public:
		CSeparator(CGui* pConfig);
		virtual void UpdateBoundingSize();
		virtual void Render();
	};
	
public:
	CHListLayout(CGui *pConfig);
	
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	
	void AddSeparator();
};

class CVListLayout : public CAbstractListLayout
{
protected:	
	class CSeparator : public CWidget
	{
	public:
		CSeparator(CGui* pConfig);
		virtual void UpdateBoundingSize();
		virtual void Render();
	};
	
protected:
	int m_ChildrenLength;
	
public:
	CVListLayout(class CGui *pConfig);
	
	virtual void AddSeparator();
	
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
};

class CVScrollLayout : public CVListLayout
{
protected:
	bool m_ShowScrollBar;
	CVScrollBar* m_pScrollBar;
	
public:
	CVScrollLayout(class CGui *pConfig);
	
	virtual void Destroy();
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	virtual void Update(bool ParentEnabled);
	virtual void Render();
	virtual void OnMouseMove();
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	virtual void OnInputEvent(const CInput::CEvent& Event);
};

}

#endif
