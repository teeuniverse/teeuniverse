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

#ifndef __CLIENT_GUI_PANELLAYOUT__
#define __CLIENT_GUI_PANELLAYOUT__

#include <shared/tl/array.h>

#include "widget.h"
	
namespace gui
{

class CAbstractPanelLayout : public CWidget
{
protected:
	class CChild
	{
	public:
		CWidget* m_pWidget;
		bool m_Fill;
		int m_Size;
		CRect m_SliderRect;
		bool m_SliderClicked;
	};
	
protected:
	CAssetPath m_BoxStylePath;
	array<CChild> m_Childs;
	CRect m_ClipRect;
	bool m_MouseOver;
	
public:
	CAbstractPanelLayout(class CGui *pConfig);
	
	void Clear();
	void Add(CWidget* pWidget, int Size);
	
	virtual void Destroy();
	virtual void Update(bool ParentEnabled);
	virtual void Render();
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	virtual void OnInputEvent(const CInput::CEvent& Event);
	
	inline void SetBoxStyle(CAssetPath BoxStylePath) { m_BoxStylePath = BoxStylePath; }
	inline CAssetPath GetBoxStyle() const { return m_BoxStylePath; }
};

class CHPanelLayout : public CAbstractPanelLayout
{
public:
	CHPanelLayout(class CGui *pConfig);
	
	virtual void Update(bool ParentEnabled);
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	virtual void OnMouseMove();
};

class CVPanelLayout : public CAbstractPanelLayout
{
public:
	CVPanelLayout(class CGui *pConfig);
	
	virtual void Update(bool ParentEnabled);
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	virtual void OnMouseMove();
};

}

#endif
