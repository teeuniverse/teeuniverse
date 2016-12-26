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

#include <client/components/assetsrenderer.h>
#include <client/components/graphics.h>
#include <client/components/input.h>

#include "text-edit.h"
	
namespace gui
{

/* ABSTRACT TEXT EDIT *************************************************/

CAbstractTextEdit::CAbstractTextEdit(CGui* pContext) :
	CAbstractLabel(pContext),
	m_MouseOver(false),
	m_Changes(false),
	m_SaveOnChange(false),
	m_Composing(false),
	m_Editable(true)
{
	m_ButtonStylePath = Context()->GetTextEntryStyle();
	EnableSelection();
	m_TextCursor.m_TextIter = -1;
}

bool CAbstractTextEdit::LineInput(CInput::CEvent Event, dynamic_string& String, int& CursorPos)
{
	bool Changes = false;
	bool ResetSelection = false;
	
	int Length = String.length();
	
	if(Event.m_Flags&CInput::FLAG_TEXT)
	{
		if(m_TextSelection0.m_TextIter >= 0 && m_TextSelection1.m_TextIter >= 0 && m_TextSelection0.m_TextIter != m_TextSelection1.m_TextIter)
		{
			int StartCharPos = min(m_TextSelection0.m_TextIter, m_TextSelection1.m_TextIter);
			int EndCharPos = max(m_TextSelection0.m_TextIter, m_TextSelection1.m_TextIter);
			int CharSize = EndCharPos-StartCharPos;
			if(CharSize > 0)
			{
				for(int c=StartCharPos; String.buffer()[c]; c++)
					String.buffer()[c] = String.buffer()[c+CharSize];
				String.buffer()[Length-CharSize] = 0;
				
				if(CursorPos > EndCharPos)
					CursorPos -= CharSize;
				else if(CursorPos > StartCharPos)
					CursorPos = StartCharPos;
			}
		}
		
		String.insert_at(CursorPos, Event.m_aText);
		CursorPos += str_length(Event.m_aText);
		
		Changes = true;
		ResetSelection = true;
		
		Length = String.length();
	}
	
	if(Event.m_Flags&CInput::FLAG_PRESS)
	{
		int Key = Event.m_Key;
		if(Key == KEY_BACKSPACE)
		{
			int StartCharPos = 0;
			int EndCharPos = 0;
			
			if(m_TextSelection0.m_TextIter >= 0 && m_TextSelection1.m_TextIter >= 0 && m_TextSelection0.m_TextIter != m_TextSelection1.m_TextIter)
			{
				StartCharPos = min(m_TextSelection0.m_TextIter, m_TextSelection1.m_TextIter);
				EndCharPos = max(m_TextSelection0.m_TextIter, m_TextSelection1.m_TextIter);
			}
			else if(CursorPos > 0)
			{
				StartCharPos = str_utf8_rewind(String.buffer(), CursorPos);
				EndCharPos = CursorPos;
			}
			
			int CharSize = EndCharPos-StartCharPos;
			if(CharSize > 0)
			{
				for(int c=EndCharPos; String.buffer()[c]; c++)
					String.buffer()[c-CharSize] = String.buffer()[c];
				String.buffer()[Length-CharSize] = 0;
			}
			
			CursorPos = StartCharPos;
			
			Changes = true;
			ResetSelection = true;
		}
		else if(Key == KEY_DELETE)
		{
			int StartCharPos = 0;
			int EndCharPos = 0;
			
			if(m_TextSelection0.m_TextIter >= 0 && m_TextSelection1.m_TextIter >= 0 && m_TextSelection0.m_TextIter != m_TextSelection1.m_TextIter)
			{
				StartCharPos = min(m_TextSelection0.m_TextIter, m_TextSelection1.m_TextIter);
				EndCharPos = max(m_TextSelection0.m_TextIter, m_TextSelection1.m_TextIter);
			}
			else if(CursorPos < Length)
			{
				StartCharPos = CursorPos;
				EndCharPos = str_utf8_forward(String.buffer(), CursorPos);
			}
			
			int CharSize = EndCharPos-StartCharPos;
			if(CharSize > 0)
			{
				for(int c=EndCharPos; String.buffer()[c]; c++)
					String.buffer()[c-CharSize] = String.buffer()[c];
				String.buffer()[Length-CharSize] = 0;
			}
			
			CursorPos = StartCharPos;
			
			Changes = true;
			ResetSelection = true;
		}
		else if(Key == KEY_LEFT)
		{
			if(m_TextSelection0.m_TextIter >= 0 && m_TextSelection1.m_TextIter >= 0 && m_TextSelection0.m_TextIter != m_TextSelection1.m_TextIter)
				CursorPos = ((m_TextSelection0.m_Position < m_TextSelection1.m_Position) ? m_TextSelection0.m_TextIter : m_TextSelection1.m_TextIter);
			else if(CursorPos > 0)
				CursorPos = str_utf8_rewind(String.buffer(), CursorPos);
				
			ResetSelection = true;
		}
		else if(Key == KEY_RIGHT)
		{
			if(m_TextSelection0.m_TextIter >= 0 && m_TextSelection1.m_TextIter >= 0 && m_TextSelection0.m_TextIter != m_TextSelection1.m_TextIter)
				CursorPos = ((m_TextSelection0.m_Position < m_TextSelection1.m_Position) ? m_TextSelection1.m_TextIter : m_TextSelection0.m_TextIter);
			else if(CursorPos < Length)
				CursorPos = str_utf8_forward(String.buffer(), CursorPos);
				
			ResetSelection = true;
		}
		else if(Key == KEY_HOME)
		{
			CursorPos = 0;
			ResetSelection = true;
		}
		else if(Key == KEY_END)
		{
			CursorPos = Length;
			ResetSelection = true;
		}
		else if(Key == KEY_V && Input()->KeyIsPressed(KEY_LCTRL))
		{
			const char* pClipboardText = Input()->GetClipboardText();
			if(pClipboardText)
			{
				if(m_TextSelection0.m_TextIter >= 0 && m_TextSelection1.m_TextIter >= 0 && m_TextSelection0.m_TextIter != m_TextSelection1.m_TextIter)
				{
					int StartCharPos = min(m_TextSelection0.m_TextIter, m_TextSelection1.m_TextIter);
					int EndCharPos = max(m_TextSelection0.m_TextIter, m_TextSelection1.m_TextIter);
					int CharSize = EndCharPos-StartCharPos;
					if(CharSize > 0)
					{
						for(int c=EndCharPos; String.buffer()[c]; c++)
							String.buffer()[c-CharSize] = String.buffer()[c];
						String.buffer()[Length-CharSize] = 0;
					}
				}
				
				String.insert_at(CursorPos, pClipboardText);
				CursorPos += str_length(pClipboardText);
				
				Changes = true;
				ResetSelection = true;
				
				Length = String.length();
			}
		}
	}
	
	if(ResetSelection)
	{
		m_TextSelection0.m_TextIter = -1;
		m_TextSelection1.m_TextIter = -1;
		m_DragSelection = false;
	}
	
	return Changes;
}

void CAbstractTextEdit::RefreshLabelStyle()
{
	const CAsset_GuiButtonStyle* pButtonStyle = AssetsManager()->GetAsset<CAsset_GuiButtonStyle>(m_ButtonStylePath);
	if(pButtonStyle)
	{
		if(!m_Editable)
			SetLabelStyle(pButtonStyle->GetReadOnlyStylePath());
		else if(Context()->HasFocus(this))
			SetLabelStyle(pButtonStyle->GetFocusStylePath());
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
		
		RefreshLabelStyle();
		
		if(!m_Changes)
			CopyToTextBuffer();
	}
	
	CAbstractLabel::Update(ParentEnabled);
}
	
void CAbstractTextEdit::Render()
{
	CAbstractLabel::Render();
	
	float CursorHeight = GetTextRect().h;
	
	// render composing text
	if(m_Editable && m_Composing)
	{
		int FontSize = GetFontSize();
		vec4 FontColor = 1.0f;
		
		const CAsset_GuiLabelStyle* pLabelStyle = AssetsManager()->GetAsset<CAsset_GuiLabelStyle>(Context()->GetComposeStyle());
		if(pLabelStyle)
		{
			FontSize = Context()->ApplyGuiScale(pLabelStyle->GetFontSize());
			FontColor = pLabelStyle->GetTextColor();
		}
		
		CRect ComposingRect = CRect(
			0,
			0,
			TextRenderer()->GetTextWidth(&m_ComposingTextCache),
			FontSize
		);
		
		if(pLabelStyle)
		{
			ComposingRect.AddMargin(Context()->ApplyGuiScale(pLabelStyle->GetPadding()));
			ComposingRect.AddMargin(Context()->ApplyGuiScale(pLabelStyle->GetMargin()));
		}
		
		ComposingRect.x = GetTextPosition().x + m_TextCursor.m_Position.x - ComposingRect.w/2;
		ComposingRect.y = GetTextPosition().y - ComposingRect.h;
		
		ivec2 ComposeTextPos;
		ComposeTextPos.x = ComposingRect.x + ComposingRect.w/2 - TextRenderer()->GetTextWidth(&m_ComposingTextCache)/2;
		ComposeTextPos.y = ComposingRect.y;
		
		if(pLabelStyle)
		{
			CRect Rect = ComposingRect;
			Rect.RemoveMargin(Context()->ApplyGuiScale(pLabelStyle->GetMargin()));
			AssetsRenderer()->DrawGuiRect(&Rect, pLabelStyle->GetRectPath());
		}
		
		m_ComposingTextCache.SetFontSize(GetFontSize());
		m_ComposingTextCache.SetBoxSize(ivec2(-1, ComposingRect.h));
		TextRenderer()->DrawText(&m_ComposingTextCache, ivec2(ComposeTextPos.x, ComposeTextPos.y), FontColor);
	}
	
	// render the cursor
	if(m_Editable && m_TextCursor.m_TextIter >= 0 && !(m_TextSelection0.m_TextIter >= 0 && m_TextSelection1.m_TextIter >= 0 && m_TextSelection0.m_TextIter != m_TextSelection1.m_TextIter))
	{
		Graphics()->TextureClear();
		Graphics()->LinesBegin();
		double Time = fmod((double)time_get()/(double)time_freq(), 1.0f);
		Graphics()->SetColor(vec4(1.0f, 1.0f, 1.0f, 0.5f+cos(2.0f*Pi*Time)/2.0f), true);		
		
		float x = GetTextPosition().x + m_TextCursor.m_Position.x + 0.5f;
		float y0 = GetTextRect().y;
		float y1 = GetTextRect().y + CursorHeight;
		CGraphics::CLineItem Line(x, y0, x, y1);
		Graphics()->LinesDraw(&Line, 1);
		
		Graphics()->LinesEnd();
	}
}

void CAbstractTextEdit::OnMouseMove()
{
	if(m_Editable && m_VisibilityRect.IsInside(Context()->GetMousePos()))
		m_MouseOver = true;
	else
		m_MouseOver = false;
	
	CAbstractLabel::OnMouseMove();
}

void CAbstractTextEdit::OnButtonClick(int Button)
{
	if(m_Editable && Button == KEY_MOUSE_1)
	{
		if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		{
			if(!Context()->HasFocus(this))
				Context()->StartFocus(this);
			
			m_TextCursor = TextRenderer()->GetTextCursorFromPosition(&m_TextCache, GetTextPosition(), Context()->GetMousePos());
			
			if(m_TextCursor.m_TextIter >= 0 && !(m_TextSelection0.m_TextIter >= 0 && m_TextSelection1.m_TextIter >= 0 && m_TextSelection0.m_TextIter != m_TextSelection1.m_TextIter))
				TextShiftFromCursor(ivec2(GetTextPosition().x + m_TextCursor.m_Position.x, GetTextPosition().y));
		}
		else
			Context()->StopFocus(this);
	}
	
	CAbstractLabel::OnButtonClick(Button);
}

void CAbstractTextEdit::OnInputEvent(const CInput::CEvent& Event)
{
	if(m_Editable)
	{
		m_Composing = false;
		if(Context()->HasFocus(this))
		{		
			if((Event.m_Key == KEY_RETURN || Event.m_Key == KEY_KP_ENTER) && (Event.m_Flags & CInput::FLAG_RELEASE))
			{
				if(m_Changes)
				{
					SaveFromTextBuffer();
					m_Changes = false;
					m_TextCursor.m_TextIter = -1;
					m_TextShift = 0;
				}
			}
			else if(m_TextCursor.m_TextIter >= 0)
			{
				int TextIter = m_TextCursor.m_TextIter;
				
				if(LineInput(Event, m_Text, TextIter))
				{
					m_Localize = false;
					OnTextUpdated();
					
					if(m_SaveOnChange)
					{
						SaveFromTextBuffer();
						m_Changes = false;
					}
					else
						m_Changes = true;
					
					if(m_TextCursor.m_TextIter >= 0 && !(m_TextSelection0.m_TextIter >= 0 && m_TextSelection1.m_TextIter >= 0 && m_TextSelection0.m_TextIter != m_TextSelection1.m_TextIter))
						TextShiftFromCursor(ivec2(GetTextPosition().x + m_TextCursor.m_Position.x, GetTextPosition().y));
				}
				
				m_TextCursor = TextRenderer()->GetTextCursorFromTextIter(&m_TextCache, GetTextPosition(), TextIter);
			}
		}
	}
	
	CAbstractLabel::OnInputEvent(Event);
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
	{
		m_TextCursor.m_TextIter = -1;
		m_TextSelection0.m_TextIter = -1;
		m_TextSelection1.m_TextIter = -1;
	}
}

void CAbstractTextEdit::OnFocusStart()
{
	m_Localize = false;
	Input()->StartTextEditing(GetClipRect().x, GetClipRect().y, GetClipRect().w, GetClipRect().h);
	m_TextCursor = TextRenderer()->GetTextCursorFromTextIter(&m_TextCache, GetTextPosition(), str_length(GetText()));
}

void CAbstractTextEdit::OnFocusStop()
{
	if(m_Changes)
	{
		SaveFromTextBuffer();
		m_Changes = false;
	}
	
	m_TextShift = 0;
	m_TextCursor.m_TextIter = -1;
	m_TextSelection0.m_TextIter = -1;
	m_TextSelection1.m_TextIter = -1;
	m_DragSelection = false;
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
