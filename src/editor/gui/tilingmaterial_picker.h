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

#ifndef __EDITOR_TILINGMATERIALPICKER__
#define __EDITOR_TILINGMATERIALPICKER__

#include <client/gui/listlayout.h>
#include <client/gui/label.h>

class CTilingMaterialPicker : public gui::CVListLayout
{	
protected:
	class CGuiEditor* m_pAssetsEditor;
	CAssetPath m_MaterialPath;
	gui::CLabel* m_pIndexTitle;

public:
	CTilingMaterialPicker(CGuiEditor* pAssetsEditor, CAssetPath MaterialPath);
	
	inline gui::CLabel* GetTitleLabel() { return m_pIndexTitle; }
	
	virtual void OnBrushPicked(CSubPath IndexPath) {}
};

#endif
