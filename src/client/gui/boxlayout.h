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

#ifndef __CLIENT_GUI_BOXLAYOUT__
#define __CLIENT_GUI_BOXLAYOUT__

#include <shared/tl/array.h>

#include "widget.h"
#include "scrollbar.h"
	
namespace gui
{

class CBoxLayout : public CWidget
{
protected:	
	class CChild
	{
	public:
		CWidget* m_pWidget;
	};
	
protected:
	CAssetPath m_BoxStylePath;
	array<CChild> m_Childs;
	CRect m_ClipRect;
	bool m_ShowScrollBar;
	CVScrollBar* m_pScrollBar;
	int m_ContentHeight;

public:
	CBoxLayout(class CGui *pConfig);
	
	virtual void Destroy();
	virtual void Update(bool ParentEnabled);
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	virtual void Render();
	virtual void OnMouseMove();
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	virtual void OnInputEvent(const CInput::CEvent& Event);
	
	void Clear();
	void Add(CWidget* pWidget);
	
	void SetBoxStyle(CAssetPath BoxStylePath) { m_BoxStylePath = BoxStylePath; }
	CAssetPath GetBoxStyle() const { return m_BoxStylePath; }
};

}

#endif
