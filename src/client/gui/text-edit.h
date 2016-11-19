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

#ifndef __CLIENT_GUI_TEXTEDIT__
#define __CLIENT_GUI_TEXTEDIT__

#include "label.h"
	
namespace gui
{

class CAbstractTextEdit : public CAbstractLabel
{
protected:
	bool m_MouseOver;
	bool m_Changes;
	bool m_SaveOnChange;
	CTextRenderer::CTextCache m_ComposingTextCache;
	bool m_Composing;
	CTextRenderer::CTextCursor m_TextCursor;
	CAssetPath m_ButtonStylePath;
	bool m_Editable;

protected:
	virtual void SaveFromTextBuffer() = 0;
	virtual void CopyToTextBuffer() = 0;

	void RefreshLabelStyle();

public:
	CAbstractTextEdit(class CGui *pConfig);
	
	virtual void Update(bool ParentEnabled);
	virtual void Render();
	
	virtual void OnMouseMove();
	virtual void OnButtonClick(int Button);
	virtual void OnInputEvent(const CInput::CEvent& Event);
	
	void RemoveChanges();
	void Clear();
	
	virtual void OnFocusStart();
	virtual void OnFocusStop();
	
	inline CAssetPath GetButtonStyle() const { return m_ButtonStylePath; }
	void SetButtonStyle(CAssetPath Path);
	
	inline void SaveOnChange() { m_SaveOnChange = true; }
	
	inline bool IsEditable() const { return m_Editable; }
	inline void Editable(bool Value) { m_Editable = Value; }
};

class CInternalTextEdit : public CAbstractTextEdit
{
protected:	
	virtual void SaveFromTextBuffer();
	virtual void CopyToTextBuffer();

public:
	CInternalTextEdit(class CGui *pConfig, const char* pDefaultText);
};

class CExternalTextEdit_DynamicString : public CAbstractTextEdit
{
protected:
	char* m_pText;
	dynamic_string* m_pBuffer;
	
	virtual void SaveFromTextBuffer();
	virtual void CopyToTextBuffer();

public:
	CExternalTextEdit_DynamicString(class CGui *pConfig, dynamic_string* pBuffer);
};

class CExternalTextEdit_Char : public CAbstractTextEdit
{
protected:
	char* m_pText;
	int m_TextSize;
	
	virtual void SaveFromTextBuffer();
	virtual void CopyToTextBuffer();

public:
	CExternalTextEdit_Char(class CGui *pConfig, char* pText, int TextSize);
};

}

#endif
