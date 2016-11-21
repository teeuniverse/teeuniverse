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

#include <client/components/assetsrenderer.h>
#include <client/components/graphics.h>
#include <client/components/input.h>

#include "text-edit.h"
	
namespace gui
{

bool LineInput(CInput::CEvent Event, char *pStr, int StrMaxSize, int StrMaxChars, int *pStrLenPtr, int *pCursorPosPtr, int *pNumCharsPtr)
{
	int NumChars = *pNumCharsPtr;
	int CursorPos = *pCursorPosPtr;
	int Len = *pStrLenPtr;
	bool Changes = false;

	if(CursorPos > Len)
		CursorPos = Len;

	if(Event.m_Flags&CInput::FLAG_TEXT)
	{
		// gather string stats
		int CharCount = 0;
		int CharSize = 0;
		while(Event.m_aText[CharSize])
		{
			int NewCharSize = str_utf8_forward(Event.m_aText, CharSize);
			if(NewCharSize != CharSize)
			{
				++CharCount;
				CharSize = NewCharSize;
			}
		}

		// add new string
		if(CharCount)
		{
			if(Len+CharSize < StrMaxSize && CursorPos+CharSize < StrMaxSize && NumChars+CharCount < StrMaxChars)
			{
				mem_move(pStr + CursorPos + CharSize, pStr + CursorPos, Len-CursorPos+1);
				for(int i = 0; i < CharSize; i++)
					pStr[CursorPos+i] = Event.m_aText[i];
				CursorPos += CharSize;
				Len += CharSize;
				NumChars += CharCount;
				Changes = true;
			}
		}
	}

	if(Event.m_Flags&CInput::FLAG_PRESS)
	{
		int Key = Event.m_Key;
		if(Key == KEY_BACKSPACE && CursorPos > 0)
		{
			int NewCursorPos = str_utf8_rewind(pStr, CursorPos);
			int CharSize = CursorPos-NewCursorPos;
			mem_move(pStr+NewCursorPos, pStr+CursorPos, Len - NewCursorPos - CharSize + 1);
			CursorPos = NewCursorPos;
			Len -= CharSize;
			if(CharSize > 0)
				--NumChars;
			Changes = true;
		}
		else if(Key == KEY_DELETE && CursorPos < Len)
		{
			int p = str_utf8_forward(pStr, CursorPos);
			int CharSize = p-CursorPos;
			mem_move(pStr + CursorPos, pStr + CursorPos + CharSize, Len - CursorPos - CharSize + 1);
			Len -= CharSize;
			if(CharSize > 0)
				--NumChars;
			Changes = true;
		}
		else if(Key == KEY_LEFT && CursorPos > 0)
			CursorPos = str_utf8_rewind(pStr, CursorPos);
		else if(Key == KEY_RIGHT && CursorPos < Len)
			CursorPos = str_utf8_forward(pStr, CursorPos);
		else if(Key == KEY_HOME)
			CursorPos = 0;
		else if(Key == KEY_END)
			CursorPos = Len;
	}

	*pNumCharsPtr = NumChars;
	*pCursorPosPtr = CursorPos;
	*pStrLenPtr = Len;

	return Changes;
}

/* ABSTRACT TEXT EDIT *************************************************/

CAbstractTextEdit::CAbstractTextEdit(CGui* pContext) :
	CAbstractLabel(pContext),
	m_Changes(false),
	m_Composing(false),
	m_MouseOver(false),
	m_SaveOnChange(false),
	m_Editable(true)
{
	m_TextCursor.m_TextIter = -1;
	m_ButtonStylePath = Context()->GetTextEntryStyle();
}

void CAbstractTextEdit::RefreshLabelStyle()
{
	const CAsset_GuiButtonStyle* pButtonStyle = AssetsManager()->GetAsset<CAsset_GuiButtonStyle>(m_ButtonStylePath);
	if(pButtonStyle)
	{
		if(!m_Editable)
			SetLabelStyle(pButtonStyle->GetReadOnlyStylePath());
		else if(m_MouseOver)
			SetLabelStyle(pButtonStyle->GetMouseOverStylePath());
		else
			SetLabelStyle(pButtonStyle->GetIdleStylePath());
	}
	else
		SetLabelStyle(CAssetPath::Null());
}

void CAbstractTextEdit::SetButtonStyle(CAssetPath StylePath)
{
	m_ButtonStylePath = StylePath;
	RefreshLabelStyle();
}

void CAbstractTextEdit::Update(bool ParentEnabled)
{
	if(ParentEnabled)
	{
		Context()->TryToGetFocus(this);
		
		if(Context()->HasFocus(this))
		{
			const char* m_pEditedText = Input()->GetEditedText();
			if(m_pEditedText && str_length(m_pEditedText))
			{
				m_ComposingTextCache.SetText(m_pEditedText);
				m_Composing = true;
			}
		}
		
		if(m_MouseOver && m_Editable)
			Context()->SetCursor(this, CInput::CURSOR_TEXT);
		
		RefreshLabelStyle();
		
		if(!m_Changes)
			CopyToTextBuffer();
	}
	
	CAbstractLabel::Update(ParentEnabled);
}
	
void CAbstractTextEdit::Render()
{
	CAbstractLabel::Render();
	
	if(m_Editable && m_Composing)
	{
		int FontSize = GetFontSize();
		vec4 FontColor = 1.0f;
		
		CRect ComposingRect = CRect(
			GetTextRect().x + GetTextRect().w,
			GetTextRect().y,
			TextRenderer()->GetTextWidth(&m_ComposingTextCache),
			GetTextRect().h
		);
		
		const CAsset_GuiLabelStyle* pLabelStyle = AssetsManager()->GetAsset<CAsset_GuiLabelStyle>(Context()->GetComposeStyle());
		if(pLabelStyle)
		{
			FontSize = Context()->ApplyGuiScale(pLabelStyle->GetFontSize());
			FontColor = pLabelStyle->GetTextColor();
			
			CRect Rect = ComposingRect;
			int Padding = Context()->ApplyGuiScale(pLabelStyle->GetPadding());
			Rect.AddMargin(Padding);
			ComposingRect.x += Padding;
			
			AssetsRenderer()->DrawGuiRect(&ComposingRect, pLabelStyle->GetRectPath());
		}
		m_ComposingTextCache.SetFontSize(GetFontSize());
		m_ComposingTextCache.SetBoxSize(ivec2(-1, ComposingRect.h));
		TextRenderer()->DrawText(&m_ComposingTextCache, ivec2(ComposingRect.x, ComposingRect.y), FontColor);
	}
	
	// render the cursor
			// cursor position
	if(m_Editable && m_TextCursor.m_TextIter >= 0)
	{
		if((2*time_get()/time_freq()) % 2)
		{
			Graphics()->TextureClear();
			Graphics()->LinesBegin();
			Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			
			float x = m_TextCursor.m_Position.x + 0.5f;
			float y0 = GetTextRect().y;
			float y1 = GetTextRect().y + GetTextRect().h;
			CGraphics::CLineItem Line(x, y0, x, y1);
			Graphics()->LinesDraw(&Line, 1);
			
			Graphics()->LinesEnd();
		}
	}
}

void CAbstractTextEdit::OnMouseMove()
{
	if(m_Editable && m_VisibilityRect.IsInside(Context()->GetMousePos()))
		m_MouseOver = true;
	else
		m_MouseOver = false;
}

void CAbstractTextEdit::OnButtonClick(int Button)
{
	if(!m_Editable)
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
	{
		if(Context()->HasFocus(this))
			m_TextCursor = TextRenderer()->GetTextCursorFromPosition(&m_TextCache, GetTextPosition(), Context()->GetMousePos());
		else
			Context()->StartFocus(this);
	}
	else
		Context()->StopFocus(this);
}

void CAbstractTextEdit::OnInputEvent(const CInput::CEvent& Event)
{
	if(!m_Editable)
		return;
	
	m_Composing = false;
	if(!Context()->HasFocus(this))
		return;
	
	if((Event.m_Key == KEY_RETURN || Event.m_Key == KEY_KP_ENTER) && (Event.m_Flags & CInput::FLAG_RELEASE))
	{
		if(m_Changes)
		{
			SaveFromTextBuffer();
			m_Changes = false;
		}
	}
	else if(m_TextCursor.m_TextIter >= 0)
	{
		int TextIter = m_TextCursor.m_TextIter;
		
		int Len = str_length(m_aText);
		int NumChars = Len;
		if(LineInput(Event, m_aText, sizeof(m_aText), sizeof(m_aText), &Len, &TextIter, &NumChars))
		{
			OnTextUpdated();
			
			if(m_SaveOnChange)
			{
				SaveFromTextBuffer();
				m_Changes = false;
			}
			else
				m_Changes = true;
		}
		
		m_TextCursor = TextRenderer()->GetTextCursorFromTextIter(&m_TextCache, GetTextPosition(), TextIter);
	}
}

void CAbstractTextEdit::RemoveChanges()
{
	m_Changes = false;
}

void CAbstractTextEdit::Clear()
{
	m_Changes = true;
	SetText("");
	if(Context()->HasFocus(this))
		m_TextCursor = TextRenderer()->GetTextCursorFromTextIter(&m_TextCache, GetTextPosition(), str_length(GetText()));
}

void CAbstractTextEdit::OnFocusStart()
{
	Input()->StartTextEditing(GetTextRect().x, GetTextRect().y, GetTextRect().w, GetTextRect().h);
	m_TextCursor = TextRenderer()->GetTextCursorFromTextIter(&m_TextCache, GetTextPosition(), str_length(GetText()));
}

void CAbstractTextEdit::OnFocusStop()
{
	if(m_Changes)
	{
		SaveFromTextBuffer();
		m_Changes = false;
	}
	
	m_TextCursor.m_TextIter = -1;
	m_Composing = false;
	m_ComposingTextCache.ResetRendering();
	
	Input()->StopTextEditing();
}

/* INTERNAL TEXT EDIT *************************************************/

CInternalTextEdit::CInternalTextEdit(CGui* pContext, const char* pDefaultText) :
	CAbstractTextEdit(pContext)
{
	SetText(pDefaultText);
}

void CInternalTextEdit::SaveFromTextBuffer()
{
	
}

void CInternalTextEdit::CopyToTextBuffer()
{
	
}

/* EXTERNAL TEXT EDIT *************************************************/

CExternalTextEdit_DynamicString::CExternalTextEdit_DynamicString(CGui* pContext, dynamic_string* pBuffer) :
	CAbstractTextEdit(pContext),
	m_pBuffer(pBuffer)
{
	
}

void CExternalTextEdit_DynamicString::SaveFromTextBuffer()
{
	m_pBuffer->copy(GetText());
}

void CExternalTextEdit_DynamicString::CopyToTextBuffer()
{
	SetText(m_pBuffer->buffer());
}

CExternalTextEdit_Char::CExternalTextEdit_Char(CGui* pContext, char* pText, int TextSize) :
	CAbstractTextEdit(pContext),
	m_pText(pText),
	m_TextSize(TextSize)
{
	
}

void CExternalTextEdit_Char::SaveFromTextBuffer()
{
	str_copy(m_pText, GetText(), m_TextSize);
}

void CExternalTextEdit_Char::CopyToTextBuffer()
{
	SetText(m_pText);
}

}
