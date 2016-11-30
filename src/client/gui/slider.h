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

#ifndef __CLIENT_GUI_SLIDER__
#define __CLIENT_GUI_SLIDER__

#include "widget.h"
	
namespace gui
{

class CAbstractSlider : public CWidget
{
protected:	
	bool m_Clicked;
	int m_ClickShift;
	CAssetPath m_SliderStylePath;
	CRect m_ClipRect;
	CRect m_CursorRect;
	
	int m_Min;
	int m_Max;

	virtual int GetValue() const = 0;
	virtual void SetValue(int Value) = 0;
	
public:
	CAbstractSlider(class CGui *pContext, int Min, int Max);
	
	virtual void UpdateBoundingSize();
	
	virtual void OnButtonRelease(int Button);
	
	int GetContentPos() const;
	
	void SetSliderStyle(CAssetPath StylePath) { m_SliderStylePath = StylePath; }
	CAssetPath GetSliderStyle() const { return m_SliderStylePath; }
};

class CHSlider : public CAbstractSlider
{
public:
	CHSlider(class CGui *pContext, int Min, int Max);
	
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	virtual void Render();
	virtual void OnButtonClick(int Button);
	virtual void OnMouseMove();
};

}

#endif
