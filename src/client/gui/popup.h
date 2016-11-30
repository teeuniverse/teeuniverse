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

#ifndef __CLIENT_GUI_POPUP__
#define __CLIENT_GUI_POPUP__

#include <shared/tl/array.h>

#include "widget.h"
#include "listlayout.h"
#include "button.h"
	
namespace gui
{

class CPopup : public CWidget
{
public:
	enum
	{
		ALIGNMENT_BOTTOM,
		ALIGNMENT_SIDE,
		ALIGNMENT_INNER,
	};
	
	enum
	{
		LEVEL_LOWEST=0,
		LEVEL_HIGHEST,
		NUM_LEVELS,
	};
	
protected:
	int m_Level;
	int m_ChildWidth;
	int m_ChildHeight;
	CWidget* m_pChild;
	bool m_IsClosed;
	CRect m_CreatorRect;
	CAssetPath m_BoxStylePath;
	int m_Width;
	int m_Height;
	int m_Alignment;

	CPopup(class CGui *pConfig, const CRect& CreatorRect, int Width, int Height, int Alignment);

public:
	virtual void Destroy();	
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	virtual void Update(bool ParentEnabled);
	virtual void Render();
	virtual void OnMouseMove();
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	virtual void OnInputEvent(const CInput::CEvent& Event);
	
	virtual void Add(CWidget* pWidget);
	virtual void Clear();
	
	virtual void Close();
	virtual bool IsClosed();
	
	inline void SetLevel(int Level) { m_Level = Level; }
	inline int GetLevel() const { return m_Level; }
	
	inline void SetBoxStyle(CAssetPath BoxStylePath) { m_BoxStylePath = BoxStylePath; }
	inline CAssetPath GetBoxStyle() const { return m_BoxStylePath; }
};

}

#endif
