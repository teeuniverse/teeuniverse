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

#ifndef __EDITOR_PREFERENCES__
#define __EDITOR_PREFERENCES__

#include <client/gui/popup.h>

class CPreferences : public gui::CPopup
{
private:
	class CGuiEditor* m_pAssetsEditor;
	
public:
	CPreferences(CGuiEditor* pAssetsEditor);
	
	class CGuiEditor* AssetsEditor() { return m_pAssetsEditor; }
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

#endif
