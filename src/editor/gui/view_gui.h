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

#ifndef TU_CLIENT_ASSETSEDITOR_VIEWGUI_H
#define TU_CLIENT_ASSETSEDITOR_VIEWGUI_H

#include <editor/gui/view.h>

class CViewGui : public CViewManager::CView
{
public:
	CViewGui(CGuiEditor* pAssetsEditor);
	virtual void RenderView();
};

#endif
