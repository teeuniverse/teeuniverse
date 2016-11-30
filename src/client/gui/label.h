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

#ifndef __CLIENT_GUI_LABEL__
#define __CLIENT_GUI_LABEL__

#include "widget.h"

#include <client/components/textrenderer.h>
#include <shared/components/localization.h>

#define _GUI(TEXT) (CLocalizableString(TEXT))

namespace gui
{

class CAbstractLabel : public CWidget
{
private:
	CAssetPath m_LabelStylePath;
	CAssetPath m_IconPath;
	gui::CWidget* m_pIconWidget;
	int m_FontSize;
	float m_IconScaling;
	CRect m_IconRect;
	CRect m_TextRect;
	bool m_ClipText;

protected:
	bool m_Localize;
	CLocalizableString m_LString;
	dynamic_string m_Text;
	CTextRenderer::CTextCache m_TextCache;

protected:
	void OnTextUpdated();
	void ApplyLocalization();

public:
	CAbstractLabel(CGui *pContext);
	
	virtual void Destroy();
	virtual void Update(bool ParentEnabled);
	virtual void UpdateBoundingSize();
	virtual void UpdatePosition(const CRect& BoundingRect, const CRect& VisibilityRect);
	virtual void Render();
	
	inline void SetIcon(CAssetPath IconPath) { m_IconPath = IconPath; }
	inline CAssetPath GetIcon() const { return m_IconPath; }
	
	inline void SetIconWidget(CWidget* pIconWidget) { m_pIconWidget = pIconWidget; }
	inline CWidget* GetIconWidget() const { return m_pIconWidget; }
	
	void SetLabelStyle(CAssetPath Path);
	inline CAssetPath GetLabelStyle() const { return m_LabelStylePath; }
	
	inline ivec2 GetTextPosition() const { return ivec2(m_TextRect.x, m_TextRect.y); }
	inline float GetFontSize() const { return m_FontSize; }
	
	void SetText(const char* pText);
	void SetText(const CLocalizableString& Text);
	inline const char* GetText() const { return m_Text.buffer(); }
	
	inline const CRect& GetTextRect() const { return m_TextRect; }
	
	inline void NoTextClipping() { m_ClipText = false; }
	
};

class CLabel : public CAbstractLabel
{
public:
	CLabel(class CGui *pConfig, const char* pText, CAssetPath IconPath = CAssetPath::Null());
	CLabel(class CGui *pConfig, const CLocalizableString& LocalizableString, CAssetPath IconPath = CAssetPath::Null());
	CLabel(class CGui *pConfig, const CLocalizableString& LocalizableString, gui::CWidget* pIconWidget);
};

class CLabelHeader : public CAbstractLabel
{
public:
	CLabelHeader(class CGui *pConfig, const char* pText, CAssetPath IconPath = CAssetPath::Null());
	CLabelHeader(class CGui *pConfig, const CLocalizableString& LocalizableString, CAssetPath IconPath = CAssetPath::Null());
};

}

#endif
