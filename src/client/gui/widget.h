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

#ifndef __CLIENT_GUI_WIDGET__
#define __CLIENT_GUI_WIDGET__

#include "rect.h"

#include <client/components/gui.h>
#include <client/components/input.h>
#include <client/keys.h>

enum
{
	TU_INPUT_CTRL=0x1,
	TU_INPUT_SHIFT=0x2,
	TU_INPUT_ALT=0x4,
};
	
namespace gui
{

class CWidget : public CGui::CGuest
{
public:
	CRect m_BoundingSizeRect;
	CRect m_VisibilityRect;
	CRect m_DrawRect;
	bool m_Disabled;

protected:
	CWidget(class CGui *pContext);
	
public:
	virtual void Destroy();

	virtual void UpdateBoundingSize() = 0;
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	virtual void Update(bool ParentEnabled) {}
	virtual void Render() {}
	
	virtual void OnMouseMove() {}
	virtual void OnButtonClick(int Button) {}
	virtual void OnButtonRelease(int Button) {}
	virtual void OnInputEvent(const CInput::CEvent& Event) {}
	
	virtual void OnFocusStart() {}
	virtual void OnFocusStop() {}
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_NONE; }
	
	virtual void OnEnableDisable() {};
		
	inline const CRect& GetBS() const { return m_BoundingSizeRect; }
	
	inline void Disable() { m_Disabled = true; OnEnableDisable(); }
	inline void Enable() { m_Disabled = false; OnEnableDisable(); }
	inline bool IsDisabled() const { return m_Disabled; }
	inline bool IsEnabled() const { return !m_Disabled; }
};

}

#endif
