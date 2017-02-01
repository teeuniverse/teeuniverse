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

#ifndef __CLIENT_GUI__
#define __CLIENT_GUI__

#include <shared/components/localization.h>
#include <client/kernel.h>
#include <shared/assets/assetpath.h>
#include <client/gui/rect.h>

#define GUI_DEFAULT_SCALE 128

namespace gui
{
	class CWidget;
	class CPopup;
}

class CGui : public CClientKernel::CComponent, CLocalization::IListener
{
public:
	class CGuest
	{
	private:
		CGui* m_pContext;
		
	public:
		CGuest(CGui* pContext) : m_pContext(pContext) { }
		
		inline CClientKernel* ClientKernel() { return m_pContext->ClientKernel(); }
		inline const CClientKernel* ClientKernel() const { return m_pContext->ClientKernel(); }
		
		inline CStorage* Storage() { return m_pContext->Storage(); }
		inline CLocalization* Localization() { return m_pContext->Localization(); }
		inline CCommandLineInterpreter* CLI() { return m_pContext->CLI(); }
		inline CAssetsManager* AssetsManager() { return m_pContext->AssetsManager(); }
		
		inline CGraphics* Graphics() { return m_pContext->Graphics(); }
		inline CInput* Input() { return m_pContext->Input(); }
		inline CTextRenderer* TextRenderer() { return m_pContext->TextRenderer(); }
		inline CAssetsRenderer* AssetsRenderer() { return m_pContext->AssetsRenderer(); }
		inline CBindsManager* BindsManager() { return m_pContext->BindsManager(); }
		
		inline CGui* Context() { return m_pContext; }
	};

public:
	enum
	{
		BLOCKEDINPUT_NONE=0x0,
		BLOCKEDINPUT_KEY=0x1,
		BLOCKEDINPUT_MOUSEMOVE=0x2,
		BLOCKEDINPUT_TEXT=0x4,
		BLOCKEDINPUT_ALL = BLOCKEDINPUT_KEY | BLOCKEDINPUT_MOUSEMOVE | BLOCKEDINPUT_TEXT,
	};
	
	enum
	{
		ALIGNMENT_LEFT = 0,
		ALIGNMENT_CENTER,
		ALIGNMENT_RIGHT,
	};
	
	enum
	{
		TRYTOGETFOCUS_NO=0,
		TRYTOGETFOCUS_SEARCHPREV,
		TRYTOGETFOCUS_SEARCHNEXT,
		TRYTOGETFOCUS_TAKENEXT,
	};

public:
	int m_Cfg_Scale;
	int m_Cfg_ColorChooserTab;
	
protected:
	gui::CRect m_DrawRect;
	bool m_LocalizationUpdated;
	
	//Widget
	std::vector<gui::CPopup*> m_pPopups;
	std::vector<gui::CWidget*> m_DestructionList;
	gui::CWidget* m_pMainWidget;
	gui::CWidget* m_pFocusedWidget;
	gui::CWidget* m_pFocusCandidate;
	bool m_DestroyFocusedWidget;
	int m_FocusIteratorState;
	
	//Input
	ivec2 m_MouseDelta;
	ivec2 m_MousePos;
	int m_BlockedInput;
	bool m_Quit;
	bool m_ShowLoadingCursor;
	
	//Style
	CAssetPath m_LabelStyle;
	CAssetPath m_LabelHeaderStyle;
	CAssetPath m_ButtonStyle;
	CAssetPath m_IntEditStyle;
	CAssetPath m_ColorEditStyle;
	CAssetPath m_ToggleStyle;
	CAssetPath m_TextEntryStyle;
	CAssetPath m_NumericEntryStyle;
	CAssetPath m_SliderStyle;
	CAssetPath m_ScrollbarStyle;
	CAssetPath m_TabsStyle;
	CAssetPath m_PopupStyle;
	CAssetPath m_ComposeStyle;
	CAssetPath m_TextSelectionStyle;
	CAssetPath m_ComboBoxStyle;
	CAssetPath m_SeparatorStyle;
	
public:
	CGui(CClientKernel* pKernel);
	virtual ~CGui();
	
	virtual bool InitConfig(int argc, const char** argv);
	virtual void SaveConfig(class CCLI_Output* pOutput);
	virtual bool Init();
	virtual bool PreUpdate();
	virtual void Render();
	
	inline void Quit() { m_Quit = true; }
	
	virtual void LoadAssets() = 0;
	virtual gui::CWidget* CreateMainWidget() = 0;
	
	virtual void OnLocalizationModified();
	inline bool LocalizationUpdated() const { return m_LocalizationUpdated; }
	
	//Widget management
	void DestroyWidget(gui::CWidget* pWidget);
	
	//Popup
	void DisplayPopup(gui::CPopup* pPopup);
	
	//Focus
	inline bool HasFocus(gui::CWidget* pWidget) const { return (pWidget == m_pFocusedWidget); }
	void StartFocus(gui::CWidget* pWidget);
	void StopFocus(gui::CWidget* pWidget);
	void TryToGetFocus(gui::CWidget* pWidget);
	
	//Input
	const ivec2& GetMousePos() { return m_MousePos; }
	const ivec2& GetMouseRelPos() { return m_MouseDelta; }
	
	//Style
	inline CAssetPath GetLabelStyle() const { return m_LabelStyle; }
	inline CAssetPath GetLabelHeaderStyle() const { return m_LabelHeaderStyle; }
	inline CAssetPath GetButtonStyle() const { return m_ButtonStyle; }
	inline CAssetPath GetIntEditStyle() const { return m_IntEditStyle; }
	inline CAssetPath GetColorEditStyle() const { return m_ColorEditStyle; }
	inline CAssetPath GetToggleStyle() const { return m_ToggleStyle; }
	inline CAssetPath GetTextEntryStyle() const { return m_TextEntryStyle; }
	inline CAssetPath GetNumericEntryStyle() const { return m_NumericEntryStyle; }
	inline CAssetPath GetSliderStyle() const { return m_SliderStyle; }
	inline CAssetPath GetScrollbarStyle() const { return m_ScrollbarStyle; }
	inline CAssetPath GetTabsStyle() const { return m_TabsStyle; }
	inline CAssetPath GetPopupStyle() const { return m_PopupStyle; }
	inline CAssetPath GetComposeStyle() const { return m_ComposeStyle; }
	inline CAssetPath GetTextSelectionStyle() const { return m_TextSelectionStyle; }
	inline CAssetPath GetComboBoxStyle() const { return m_ComboBoxStyle; }
	inline CAssetPath GetSeparatorStyle() const { return m_SeparatorStyle; }
	
	void SetCursor(gui::CWidget* pWidget, int CursorType);
	void ShowLoadingCursor();
	
	//Context
	inline float GetGuiScale() const { return static_cast<float>(m_Cfg_Scale)/GUI_DEFAULT_SCALE; }
	inline void SetGuiScale(float Value) { m_Cfg_Scale = Value*GUI_DEFAULT_SCALE; }
	inline int ApplyGuiScale(int Value) const { return static_cast<int>(Value*(static_cast<float>(m_Cfg_Scale)/GUI_DEFAULT_SCALE)); }
	
	inline gui::CRect GetDrawRect() const { return m_DrawRect; }
};

#endif
