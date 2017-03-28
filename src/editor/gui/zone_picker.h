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

#ifndef __EDITOR_ZONEPICKER__
#define __EDITOR_ZONEPICKER__

#include <client/gui/listlayout.h>
#include <client/gui/label.h>

class CZonePicker : public gui::CVListLayout
{	
protected:
	class CGuiEditor* m_pAssetsEditor;
	CAssetPath m_ZoneTypePath;
	gui::CLabel* m_pZoneTitle;
	gui::CLabel* m_pZoneDescription;
	std::vector<int> m_DataInt;

public:
	CZonePicker(CGuiEditor* pAssetsEditor, CAssetPath ZoneTypePath, const std::vector<int>* pDataInt);
	
	inline gui::CLabel* GetTitleLabel() { return m_pZoneTitle; }
	inline gui::CLabel* GetDescriptionLabel() { return m_pZoneDescription; }
	
	virtual void OnZonePicked(CSubPath IndexPath) {}
};

#endif
