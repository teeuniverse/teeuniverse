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

#ifndef __CLIENT_GUI_DURATIONEDIT__
#define __CLIENT_GUI_DURATIONEDIT__

#include "text-edit.h"
	
namespace gui
{

class CAbstractDurationEdit : public CAbstractTextEdit
{
protected:
	int64 m_Value;
	bool m_Percent;
	
	virtual void SaveFromTextBuffer();
	virtual void CopyToTextBuffer();
	
	virtual int64 GetValue() const = 0;
	virtual void SetValue(int64 Value) = 0;
	
public:
	CAbstractDurationEdit(class CGui *pConfig);
};

}

#endif
