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

#ifndef __CLIENT_GUI_SCROLLBAR__
#define __CLIENT_GUI_SCROLLBAR__

#include "widget.h"
	
namespace gui
{

class CAbstractScrollBar : public CWidget
{
protected:
	int m_ViewSize;
	int m_ContentSize;
	float m_ContentPos;
	
	bool m_Clicked;
	int m_ClickShift;
	CAssetPath m_ScrollbarStylePath;
	CRect m_SliderRect;
	CRect m_ClipRect;

public:
	CAbstractScrollBar(class CGui *pContext);
	
	virtual void UpdateBoundingSize();
	virtual void SetContentInformations(int ContentSize, int ViewSize);
	
	virtual void OnButtonRelease(int Button);
	
	void OnWheelUp();
	void OnWheelDown();
	int GetContentPos() const;
	
	void SetScrollbarStyle(CAssetPath ScrollbarStylePath) { m_ScrollbarStylePath = ScrollbarStylePath; }
	CAssetPath GetScrollbarStyle() const { return m_ScrollbarStylePath; }
};

class CHScrollBar : public CAbstractScrollBar
{
public:
	CHScrollBar(class CGui *pContext);
	
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	virtual void Render();
	virtual void OnButtonClick(int Button);
	virtual void OnMouseMove();
};

class CVScrollBar : public CAbstractScrollBar
{
public:
	CVScrollBar(class CGui *pContext);
	
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	virtual void Render();
	virtual void OnButtonClick(int Button);
	virtual void OnMouseMove();
};

}

#endif
