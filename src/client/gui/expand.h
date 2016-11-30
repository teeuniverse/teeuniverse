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

#ifndef __CLIENT_GUI_EXPAND__
#define __CLIENT_GUI_EXPAND__

#include <shared/tl/array.h>

#include "widget.h"
	
namespace gui
{

class CExpand : public CWidget
{	
protected:
	CAssetPath m_BoxStylePath;
	CWidget* m_pTitle;
	array<CWidget*> m_Childs;
	bool m_Expanded;

	inline int GetShift() { return Context()->ApplyGuiScale(20); }

public:
	CExpand(class CGui *pConfig);
	
	void Clear();
	void Add(CWidget* pWidget);
	void SetTitle(CWidget* pWidget);
	inline void ShowContent() { m_Expanded = true; }
	inline void HideContent() { m_Expanded = false; }
	
	virtual void Destroy();
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	virtual void Update(bool ParentEnabled);
	virtual void Render();
	virtual void OnMouseMove();
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	virtual void OnInputEvent(const CInput::CEvent& Event);
	
	void SetBoxStyle(CAssetPath BoxStylePath) { m_BoxStylePath = BoxStylePath; }
	CAssetPath GetBoxStyle() const { return m_BoxStylePath; }
};

}

#endif
