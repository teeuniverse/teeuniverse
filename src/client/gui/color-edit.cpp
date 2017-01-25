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

#include <client/components/graphics.h>
#include <shared/components/assetsmanager.h>
#include <shared/math/color.h>

#include "color-edit.h"
#include "popup.h"
#include "tabs.h"
#include "float-edit.h"
#include "integer-edit.h"
	
namespace gui
{

/* ICON ***************************************************************/

class CColorIcon : public CWidget
{
protected:
	CAbstractColorEdit* m_pColorEdit;
	
public:
	CColorIcon(CGui *pContext, CAbstractColorEdit* pColorEdit) :
		CWidget(pContext),
		m_pColorEdit(pColorEdit)
	{
		
	}

	virtual void UpdateBoundingSize()
	{
		m_BoundingSizeRect.BSNoConstraint();
	}

	virtual void Render()
	{
		Graphics()->TextureClear();
		Graphics()->QuadsBegin();
		
		CRect Rect = m_DrawRect;
		Rect.w /= 2;
		
		vec4 Color = m_pColorEdit->GetValue();
		Graphics()->SetColor(Color, true);
		CGraphics::CQuadItem QuadItem1(Rect.x+Rect.w/2, Rect.y+Rect.h/2, Rect.w, Rect.h);
		Graphics()->QuadsDraw(&QuadItem1, 1);
		
		Rect.x += Rect.w;
		Rect.w = m_DrawRect.w - Rect.w;
		
		Color.a = 1.0f;
		Graphics()->SetColor(Color, true);
		CGraphics::CQuadItem QuadItem2(Rect.x+Rect.w/2, Rect.y+Rect.h/2, Rect.w, Rect.h);
		Graphics()->QuadsDraw(&QuadItem2, 1);
		
		Graphics()->QuadsEnd();
	}
};

/* SLIDERS ************************************************************/

class CColorSliderEdit : public gui::CWidget
{
protected:
	CAbstractColorEdit* m_pColorEdit;
	bool m_Clicked;
	
public:
	CColorSliderEdit(CAbstractColorEdit* pColorEdit) :
		gui::CWidget(pColorEdit->Context()),
		m_pColorEdit(pColorEdit),
		m_Clicked(false)
	{
		
	}
	
	virtual void UpdateBoundingSize()
	{
		m_BoundingSizeRect.BSNoConstraint();
		m_BoundingSizeRect.BSAddSpacing(30, 30);
	}

	virtual vec4 GetSliderValue(float Value) = 0;

	void RenderLines(float Cursor)
	{
		Graphics()->LinesBegin();
		Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		
		CGraphics::CLineItem Lines[4] = {
				CGraphics::CLineItem(m_DrawRect.x, m_DrawRect.y, m_DrawRect.x+m_DrawRect.w, m_DrawRect.y),
				CGraphics::CLineItem(m_DrawRect.x+m_DrawRect.w, m_DrawRect.y, m_DrawRect.x+m_DrawRect.w, m_DrawRect.y+m_DrawRect.h),
				CGraphics::CLineItem(m_DrawRect.x+m_DrawRect.w, m_DrawRect.y+m_DrawRect.h, m_DrawRect.x, m_DrawRect.y+m_DrawRect.h),
				CGraphics::CLineItem(m_DrawRect.x, m_DrawRect.y+m_DrawRect.h, m_DrawRect.x, m_DrawRect.y)
		};
		Graphics()->LinesDraw(Lines, 4);
		
		CGraphics::CLineItem CursorLine(m_DrawRect.x+m_DrawRect.w*Cursor, m_DrawRect.y, m_DrawRect.x+m_DrawRect.w*Cursor, m_DrawRect.y+m_DrawRect.h);
		Graphics()->LinesDraw(&CursorLine, 1);
		Graphics()->LinesEnd();
	}
	
	virtual void OnMouseMove()
	{
		ivec2 MousePos = Context()->GetMousePos();
		if(m_Clicked)
		{
			float Value = clamp(static_cast<float>(MousePos.x - m_DrawRect.x)/static_cast<float>(m_DrawRect.w), 0.0f, 1.0f);
			m_pColorEdit->SetValue(GetSliderValue(Value));
		}
	}

	virtual void OnButtonClick(int Button)
	{
		if(Button != KEY_MOUSE_1)
			return;
		
		if(m_DrawRect.IsInside(Context()->GetMousePos()))
		{
			m_Clicked = true;
		}
	}

	virtual void OnButtonRelease(int Button)
	{
		if(Button != KEY_MOUSE_1)
			return;
		
		m_Clicked = false;
	}
};

class CRGBSliderEdit : public CColorSliderEdit
{
protected:
	int m_C;
	
public:
	CRGBSliderEdit(CAbstractColorEdit* pColorEdit, int C) :
		CColorSliderEdit(pColorEdit),
		m_C(C)
	{
		
	}
	
	virtual void Render()
	{
		Graphics()->TextureClear();
		Graphics()->QuadsBegin();
		
		vec4 Color = m_pColorEdit->GetValue();
		vec4 minColor = Color;
		*(static_cast<float*>(&minColor.r)+m_C) = 0.0f;
		vec4 maxColor = Color;
		*(static_cast<float*>(&maxColor.r)+m_C) = 1.0f;
					
		CGraphics::CColorVertex ColorArray[4];
		ColorArray[0] = CGraphics::CColorVertex(0, minColor.r, minColor.g, minColor.b, 1.0f);
		ColorArray[1] = CGraphics::CColorVertex(1, maxColor.r, maxColor.g, maxColor.b, 1.0f);
		ColorArray[2] = CGraphics::CColorVertex(2, maxColor.r, maxColor.g, maxColor.b, 1.0f);
		ColorArray[3] = CGraphics::CColorVertex(3, minColor.r, minColor.g, minColor.b, 1.0f);
		Graphics()->SetColorVertex(ColorArray, 4);
	
		CGraphics::CQuadItem QuadItem(m_DrawRect.x, m_DrawRect.y, m_DrawRect.w, m_DrawRect.h);
		Graphics()->QuadsDrawTL(&QuadItem, 1);
		Graphics()->QuadsEnd();
		
		float Cursor = *(static_cast<float*>(&Color.r)+m_C);
		RenderLines(Cursor);
	}

	virtual vec4 GetSliderValue(float Value)
	{
		vec4 Color = m_pColorEdit->GetValue();
		*(static_cast<float*>(&Color.r)+m_C) = Value;
		return Color;
	}
};
	
class CAlphaSliderEdit : public CRGBSliderEdit
{		
public:
	CAlphaSliderEdit(CAbstractColorEdit* pColorEdit) :
		CRGBSliderEdit(pColorEdit, 3)
	{
		
	}
	
public:
	virtual void Render()
	{
		Graphics()->TextureClear();
		Graphics()->QuadsBegin();
		
		float SquareSize = m_DrawRect.h/3.0f;
		int nbColumns = m_DrawRect.w / SquareSize;
		
		for(int j=0; j<3; j++)
		{
			for(int i=0; i<nbColumns; i++)
			{
				if(i%2==j%2)
					Graphics()->SetColor(0.4f, 0.4f, 0.4f, 1.0f);
				else
					Graphics()->SetColor(0.6f, 0.6f, 0.6f, 1.0f);
				CGraphics::CQuadItem QuadItem(m_DrawRect.x + i*SquareSize, m_DrawRect.y + j*SquareSize, SquareSize, SquareSize);
				Graphics()->QuadsDrawTL(&QuadItem, 1);
			}
		}
		
		vec4 Color = m_pColorEdit->GetValue();
		
		CGraphics::CColorVertex ColorArray[4];
		ColorArray[0] = CGraphics::CColorVertex(0, 0.0f, 0.0f, 0.0f, 0.0f);
		ColorArray[1] = CGraphics::CColorVertex(1, Color.r, Color.g, Color.b, 1.0f);
		ColorArray[2] = CGraphics::CColorVertex(2, Color.r, Color.g, Color.b, 1.0f);
		ColorArray[3] = CGraphics::CColorVertex(3, 0.0f, 0.0f, 0.0f, 0.0f);
		Graphics()->SetColorVertex(ColorArray, 4);
	
		CGraphics::CQuadItem QuadItem(m_DrawRect.x, m_DrawRect.y, m_DrawRect.w, m_DrawRect.h);
		Graphics()->QuadsDrawTL(&QuadItem, 1);
		
		Graphics()->QuadsEnd();
		
		float Cursor = Color.a;
		RenderLines(Cursor);
	}
};

class CHSVSliderEdit : public CColorSliderEdit
{
protected:
	int m_C;
	
public:
	CHSVSliderEdit(CAbstractColorEdit* pColorEdit, int C) :
		CColorSliderEdit(pColorEdit),
		m_C(C)
	{
		
	}
	
	virtual void Render()
	{
		Graphics()->TextureClear();
		Graphics()->QuadsBegin();

		vec4 Color = m_pColorEdit->GetValue();
		vec3 ColorHSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
		
		int nbCells = 6;
		float Offset = m_DrawRect.w/static_cast<float>(nbCells);
		for(int i=0; i<nbCells; i++)
		{
			float Value0 = i*1.0f/nbCells;
			float Value1 = (i+1)*1.0f/nbCells;
			vec3 ColorHSV0 = ColorHSV;
			*(static_cast<float*>(&ColorHSV0.r)+m_C) = Value0;
			vec3 ColorHSV1 = ColorHSV;		
			*(static_cast<float*>(&ColorHSV1.r)+m_C) = Value1;
			vec3 ColorRGB0 = HsvToRgb(vec3(ColorHSV0.r, ColorHSV0.g, ColorHSV0.b));
			vec3 ColorRGB1 = HsvToRgb(vec3(ColorHSV1.r, ColorHSV1.g, ColorHSV1.b));

			CGraphics::CColorVertex ColorArray[4];
			ColorArray[0] = CGraphics::CColorVertex(0, ColorRGB0.r, ColorRGB0.g, ColorRGB0.b, 1.0f);
			ColorArray[1] = CGraphics::CColorVertex(1, ColorRGB1.r, ColorRGB1.g, ColorRGB1.b, 1.0f);
			ColorArray[2] = CGraphics::CColorVertex(2, ColorRGB1.r, ColorRGB1.g, ColorRGB1.b, 1.0f);
			ColorArray[3] = CGraphics::CColorVertex(3, ColorRGB0.r, ColorRGB0.g, ColorRGB0.b, 1.0f);
			Graphics()->SetColorVertex(ColorArray, 4);
			CGraphics::CQuadItem QuadItem(m_DrawRect.x+i*Offset, m_DrawRect.y, Offset, m_DrawRect.h);
			Graphics()->QuadsDrawTL(&QuadItem, 1);
		}
	
		Graphics()->QuadsEnd();

		float Cursor = *(static_cast<float*>(&ColorHSV.r)+m_C);
		RenderLines(Cursor);
	}

	virtual vec4 GetSliderValue(float Value)
	{
		vec4 Color = m_pColorEdit->GetValue();
		vec3 HSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
		*(static_cast<float*>(&HSV.r)+m_C) = Value;
		vec3 RGB = HsvToRgb(vec3(HSV.r, HSV.g, HSV.b));
		return vec4(RGB.r, RGB.g, RGB.b, Color.a);
	}
};
	
class CHueVSliderEdit : public gui::CWidget
{	
protected:
	CAbstractColorEdit* m_pColorEdit;	
	bool m_Clicked;
	
public:
	CHueVSliderEdit(CAbstractColorEdit* pColorEdit) :
		gui::CWidget(pColorEdit->Context()),
		m_pColorEdit(pColorEdit),
		m_Clicked(false)
	{
		
	}
	
	virtual void UpdateBoundingSize()
	{
		m_BoundingSizeRect.BSNoConstraint();
		m_BoundingSizeRect.BSAddSpacing(30, 30);
	}

	void RenderLines(float Cursor)
	{
		Graphics()->LinesBegin();
		Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		
		CGraphics::CLineItem Lines[4] = {
				CGraphics::CLineItem(m_DrawRect.x, m_DrawRect.y, m_DrawRect.x+m_DrawRect.w, m_DrawRect.y),
				CGraphics::CLineItem(m_DrawRect.x+m_DrawRect.w, m_DrawRect.y, m_DrawRect.x+m_DrawRect.w, m_DrawRect.y+m_DrawRect.h),
				CGraphics::CLineItem(m_DrawRect.x+m_DrawRect.w, m_DrawRect.y+m_DrawRect.h, m_DrawRect.x, m_DrawRect.y+m_DrawRect.h),
				CGraphics::CLineItem(m_DrawRect.x, m_DrawRect.y+m_DrawRect.h, m_DrawRect.x, m_DrawRect.y)
		};
		Graphics()->LinesDraw(Lines, 4);
		
		CGraphics::CLineItem CursorLine(m_DrawRect.x, m_DrawRect.y+m_DrawRect.h*Cursor, m_DrawRect.x+m_DrawRect.w, m_DrawRect.y+m_DrawRect.h*Cursor);
		Graphics()->LinesDraw(&CursorLine, 1);
		Graphics()->LinesEnd();
	}
	
	virtual void Render()
	{
		Graphics()->TextureClear();
		Graphics()->QuadsBegin();
		
		int nbCells = 6;
		float Offset = m_DrawRect.h/static_cast<float>(nbCells);
		for(int i=0; i<nbCells; i++)
		{
			float Value0 = i*1.0f/nbCells;
			float Value1 = (i+1)*1.0f/nbCells;
			vec3 ColorRGB0 = HsvToRgb(vec3(Value0, 1.0f, 1.0f));
			vec3 ColorRGB1 = HsvToRgb(vec3(Value1, 1.0f, 1.0f));

			CGraphics::CColorVertex ColorArray[4];
			ColorArray[0] = CGraphics::CColorVertex(0, ColorRGB0.r, ColorRGB0.g, ColorRGB0.b, 1.0f);
			ColorArray[1] = CGraphics::CColorVertex(1, ColorRGB0.r, ColorRGB0.g, ColorRGB0.b, 1.0f);
			ColorArray[2] = CGraphics::CColorVertex(2, ColorRGB1.r, ColorRGB1.g, ColorRGB1.b, 1.0f);
			ColorArray[3] = CGraphics::CColorVertex(3, ColorRGB1.r, ColorRGB1.g, ColorRGB1.b, 1.0f);
			Graphics()->SetColorVertex(ColorArray, 4);
			CGraphics::CQuadItem QuadItem(m_DrawRect.x, m_DrawRect.y+i*Offset, m_DrawRect.w, Offset);
			Graphics()->QuadsDrawTL(&QuadItem, 1);
		}
	
		Graphics()->QuadsEnd();

		vec4 Color = m_pColorEdit->GetValue();
		vec3 ColorHSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
		RenderLines(ColorHSV.r);
	}

	virtual vec4 GetSliderValue(float Value)
	{
		vec4 Color = m_pColorEdit->GetValue();
		vec3 HSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
		HSV.r = Value;
		vec3 RGB = HsvToRgb(vec3(HSV.r, HSV.g, HSV.b));
		return vec4(RGB.r, RGB.g, RGB.b, Color.a);
	}
	
	virtual void OnMouseMove()
	{
		ivec2 MousePos = Context()->GetMousePos();
		if(m_Clicked)
		{
			float Value = clamp(static_cast<float>(MousePos.y - m_DrawRect.y)/static_cast<float>(m_DrawRect.h), 0.0f, 1.0f);
			m_pColorEdit->SetValue(GetSliderValue(Value));
		}
	}

	virtual void OnButtonClick(int Button)
	{
		if(Button != KEY_MOUSE_1)
			return;
		
		if(m_DrawRect.IsInside(Context()->GetMousePos()))
		{
			m_Clicked = true;
		}
	}

	virtual void OnButtonRelease(int Button)
	{
		if(Button != KEY_MOUSE_1)
			return;
		
		m_Clicked = false;
	}
};

/* PICKERS ************************************************************/

class CSquarePicker : public CWidget
{	
protected:
	CAbstractColorEdit* m_pColorEdit;	
	bool m_Clicked;
	
public:
	CSquarePicker(CAbstractColorEdit* pColorEdit) :
		CWidget(pColorEdit->Context()),
		m_pColorEdit(pColorEdit),
		m_Clicked(false)
	{
		
	}
	
	virtual void UpdateBoundingSize()
	{
		m_BoundingSizeRect.BSNoConstraint();
		m_BoundingSizeRect.BSAddSpacing(255, 255);
	}

	virtual void Render()
	{
		Graphics()->TextureClear();
		Graphics()->QuadsBegin();

		vec4 Color = m_pColorEdit->GetValue();
		vec3 ColorHSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
		vec3 ColorRGB0 = HsvToRgb(vec3(ColorHSV.x, 0.0f, 1.0f));
		vec3 ColorRGB1 = HsvToRgb(vec3(ColorHSV.x, 1.0f, 1.0f));

		{
			CGraphics::CColorVertex ColorArray[4];
			ColorArray[0] = CGraphics::CColorVertex(0, ColorRGB0.r, ColorRGB0.g, ColorRGB0.b, 1.0f);
			ColorArray[1] = CGraphics::CColorVertex(1, ColorRGB1.r, ColorRGB1.g, ColorRGB1.b, 1.0f);
			ColorArray[2] = CGraphics::CColorVertex(2, ColorRGB1.r, ColorRGB1.g, ColorRGB1.b, 1.0f);
			ColorArray[3] = CGraphics::CColorVertex(3, ColorRGB0.r, ColorRGB0.g, ColorRGB0.b, 1.0f);
			Graphics()->SetColorVertex(ColorArray, 4);
			CGraphics::CQuadItem QuadItem(m_DrawRect.x, m_DrawRect.y, m_DrawRect.w, m_DrawRect.h);
			Graphics()->QuadsDrawTL(&QuadItem, 1);
		}
		
		{
			CGraphics::CColorVertex ColorArray[4];
			ColorArray[0] = CGraphics::CColorVertex(0, 0.0f, 0.0f, 0.0f, 0.0f);
			ColorArray[1] = CGraphics::CColorVertex(1, 0.0f, 0.0f, 0.0f, 0.0f);
			ColorArray[2] = CGraphics::CColorVertex(2, 0.0f, 0.0f, 0.0f, 1.0f);
			ColorArray[3] = CGraphics::CColorVertex(3, 0.0f, 0.0f, 0.0f, 1.0f);
			Graphics()->SetColorVertex(ColorArray, 4);
			CGraphics::CQuadItem QuadItem(m_DrawRect.x, m_DrawRect.y, m_DrawRect.w, m_DrawRect.h);
			Graphics()->QuadsDrawTL(&QuadItem, 1);
		}
	
		Graphics()->QuadsEnd();

		Graphics()->LinesBegin();
		Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
				
		vec2 CursorPos = vec2(m_DrawRect.x + m_DrawRect.w*ColorHSV.g, m_DrawRect.y + m_DrawRect.h*(1.0f-ColorHSV.b));
		CGraphics::CLineItem CursorLines[4] = {
				CGraphics::CLineItem(CursorPos.x - 2.0f, CursorPos.y - 2.0f, CursorPos.x + 2.0f, CursorPos.y - 2.0f),
				CGraphics::CLineItem(CursorPos.x + 2.0f, CursorPos.y - 2.0f, CursorPos.x + 2.0f, CursorPos.y + 2.0f),
				CGraphics::CLineItem(CursorPos.x + 2.0f, CursorPos.y + 2.0f, CursorPos.x - 2.0f, CursorPos.y + 2.0f),
				CGraphics::CLineItem(CursorPos.x - 2.0f, CursorPos.y + 2.0f, CursorPos.x - 2.0f, CursorPos.y - 2.0f),
		};
		
		Graphics()->LinesDraw(CursorLines, 4);
		Graphics()->LinesEnd();
	}
	
	virtual void OnMouseMove()
	{
		if(m_Clicked)
		{
			ivec2 MousePos = Context()->GetMousePos();
			
			float Value0 = clamp(static_cast<float>(MousePos.x - m_DrawRect.x)/static_cast<float>(m_DrawRect.w), 0.0f, 1.0f);
			float Value1 = 1.0f - clamp(static_cast<float>(MousePos.y - m_DrawRect.y)/static_cast<float>(m_DrawRect.h), 0.0f, 1.0f);
			
			vec4 Color = m_pColorEdit->GetValue();
			vec3 HSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
			HSV.g = Value0;
			HSV.b = Value1;
			vec3 RGB = HsvToRgb(vec3(HSV.r, HSV.g, HSV.b));

			m_pColorEdit->SetValue(vec4(RGB.r, RGB.g, RGB.b, Color.a));
		}
	}

	virtual void OnButtonClick(int Button)
	{
		if(Button != KEY_MOUSE_1)
			return;
		
		if(m_DrawRect.IsInside(Context()->GetMousePos()))
		{
			m_Clicked = true;
		}
	}

	virtual void OnButtonRelease(int Button)
	{
		if(Button != KEY_MOUSE_1)
			return;
		
		m_Clicked = false;
	}
};

class CWheelPicker : public gui::CWidget
{	
protected:
	CAbstractColorEdit* m_pColorEdit;	
	int m_Clicked;
	float m_LastHue;
	
public:
	CWheelPicker(CAbstractColorEdit* pColorEdit) :
		CWidget(pColorEdit->Context()),
		m_pColorEdit(pColorEdit),
		m_Clicked(false)
	{
		
	}
	
	virtual void UpdateBoundingSize()
	{
		m_BoundingSizeRect.BSNoConstraint();
		m_BoundingSizeRect.BSAddSpacing(30, 30);
	}
		
	virtual void Render()
	{
		Graphics()->TextureClear();

		vec2 Center(m_DrawRect.x + m_DrawRect.w/2, m_DrawRect.y + m_DrawRect.h/2);
		float Radius1 = min(m_DrawRect.w, m_DrawRect.h)/2.0f - 1.0f;
		float Radius0 = Radius1*0.8f;

		vec4 Color = m_pColorEdit->GetValue();
		vec3 ColorHSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
		
		float CurrentHue = (m_Clicked == 2 ? m_LastHue : ColorHSV.r);
		float CursorAngle = CurrentHue*2.0f*Pi;
		
		const int nbSegments = 64;
		const float deltaAngle = Pi*2.0f/nbSegments;
		
		Graphics()->QuadsBegin();
		
		for(int i=0; i<nbSegments; i++)
		{
			float Hue0 = i*1.0f/nbSegments;
			float Hue1 = (i+1)*1.0f/nbSegments;
			vec3 ColorRGB0 = HsvToRgb(vec3(Hue0, 1.0f, 1.0f));
			vec3 ColorRGB1 = HsvToRgb(vec3(Hue1, 1.0f, 1.0f));

			CGraphics::CColorVertex ColorArray[4];
			ColorArray[0] = CGraphics::CColorVertex(0, ColorRGB0.r, ColorRGB0.g, ColorRGB0.b, 1.0f);
			ColorArray[1] = CGraphics::CColorVertex(1, ColorRGB0.r, ColorRGB0.g, ColorRGB0.b, 1.0f);
			ColorArray[2] = CGraphics::CColorVertex(2, ColorRGB1.r, ColorRGB1.g, ColorRGB1.b, 1.0f);
			ColorArray[3] = CGraphics::CColorVertex(3, ColorRGB1.r, ColorRGB1.g, ColorRGB1.b, 1.0f);
			Graphics()->SetColorVertex(ColorArray, 4);
			
			CGraphics::CFreeformItem Freeform(
				Center.x + Radius1*cos(i*deltaAngle), Center.y + Radius1*sin(i*deltaAngle),
				Center.x + Radius0*cos(i*deltaAngle), Center.y + Radius0*sin(i*deltaAngle),
				Center.x + Radius1*cos((i+1)*deltaAngle), Center.y + Radius1*sin((i+1)*deltaAngle),
				Center.x + Radius0*cos((i+1)*deltaAngle), Center.y + Radius0*sin((i+1)*deltaAngle));
			Graphics()->QuadsDrawFreeform(&Freeform, 1);
		}
		
		{
			CGraphics::CColorVertex ColorArray[4];
			vec3 Hue = HsvToRgb(vec3(CurrentHue, 1.0f, 1.0f));
			vec3 Saturation = HsvToRgb(vec3(CurrentHue, 1.0f, 0.0f));
			vec3 Value = HsvToRgb(vec3(CurrentHue, 0.0f, 1.0f));
			
			ColorArray[0] = CGraphics::CColorVertex(0, Saturation.r, Saturation.g, Saturation.b, 1.0f);
			ColorArray[1] = CGraphics::CColorVertex(1, Saturation.r, Saturation.g, Saturation.b, 1.0f);
			ColorArray[2] = CGraphics::CColorVertex(2, Value.r, Value.g, Value.b, 1.0f);
			ColorArray[3] = CGraphics::CColorVertex(3, Hue.r, Hue.g, Hue.b, 1.0f);
			Graphics()->SetColorVertex(ColorArray, 4);
			
			CGraphics::CFreeformItem Freeform(
				Center.x + Radius0*cos(CursorAngle-2.0f*Pi/3.0f), Center.y + Radius0*sin(CursorAngle-2.0f*Pi/3.0f),
				Center.x + Radius0*cos(CursorAngle-2.0f*Pi/3.0f), Center.y + Radius0*sin(CursorAngle-2.0f*Pi/3.0f),
				Center.x + Radius0*cos(CursorAngle+2.0f*Pi/3.0f), Center.y + Radius0*sin(CursorAngle+2.0f*Pi/3.0f),
				Center.x + Radius0*cos(CursorAngle), Center.y + Radius0*sin(CursorAngle));
			Graphics()->QuadsDrawFreeform(&Freeform, 1);
		}
		
		Graphics()->QuadsEnd();
		
		Graphics()->LinesBegin();
		Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		
		for(int i=0; i<nbSegments; i++)
		{
			CGraphics::CLineItem Line1(Center.x + Radius1*cos(i*deltaAngle), Center.y + Radius1*sin(i*deltaAngle), Center.x + Radius1*cos((i+1)*deltaAngle), Center.y + Radius1*sin((i+1)*deltaAngle));
			Graphics()->LinesDraw(&Line1, 1);
			CGraphics::CLineItem Line2(Center.x + Radius0*cos(i*deltaAngle), Center.y + Radius0*sin(i*deltaAngle), Center.x + Radius0*cos((i+1)*deltaAngle), Center.y + Radius0*sin((i+1)*deltaAngle));
			Graphics()->LinesDraw(&Line2, 1);
		}
		{
			vec2 HuePos(Center.x + Radius0*cos(CursorAngle), Center.y + Radius0*sin(CursorAngle));
			vec2 SaturationPos(Center.x + Radius0*cos(CursorAngle+2.0f*Pi/3.0f), Center.y + Radius0*sin(CursorAngle+2.0f*Pi/3.0f));
			vec2 ValuePos(Center.x + Radius0*cos(CursorAngle-2.0f*Pi/3.0f), Center.y + Radius0*sin(CursorAngle-2.0f*Pi/3.0f));
			vec2 Origin = (SaturationPos + HuePos)/2.0f;
			
			CGraphics::CLineItem TriangleLines[3] = {
				CGraphics::CLineItem(HuePos.x, HuePos.y, SaturationPos.x, SaturationPos.y),
				CGraphics::CLineItem(SaturationPos.x, SaturationPos.y, ValuePos.x, ValuePos.y),
				CGraphics::CLineItem(ValuePos.x, ValuePos.y, HuePos.x, HuePos.y),
			};
			Graphics()->LinesDraw(TriangleLines, 3);
			
			vec2 CursorPos = Origin + (HuePos - Origin)*2.0f*(ColorHSV.g-0.5f)*ColorHSV.b + (ValuePos - Origin)*(1.0f-ColorHSV.b);
			CGraphics::CLineItem CursorLines[4] = {
					CGraphics::CLineItem(CursorPos.x - 2.0f, CursorPos.y - 2.0f, CursorPos.x + 2.0f, CursorPos.y - 2.0f),
					CGraphics::CLineItem(CursorPos.x + 2.0f, CursorPos.y - 2.0f, CursorPos.x + 2.0f, CursorPos.y + 2.0f),
					CGraphics::CLineItem(CursorPos.x + 2.0f, CursorPos.y + 2.0f, CursorPos.x - 2.0f, CursorPos.y + 2.0f),
					CGraphics::CLineItem(CursorPos.x - 2.0f, CursorPos.y + 2.0f, CursorPos.x - 2.0f, CursorPos.y - 2.0f),
			};
			Graphics()->LinesDraw(CursorLines, 4);
		}
		
		Graphics()->LinesEnd();
	}
	
	virtual void OnMouseMove()
	{
		vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		if(m_Clicked == 1)
		{
			vec2 Center(m_DrawRect.x + m_DrawRect.w/2, m_DrawRect.y + m_DrawRect.h/2);
			float Hue = angle(MousePos-Center)/(2.0f*Pi);
			if(Hue < 0.0f) Hue += 1.0f;
			
			vec4 Color = m_pColorEdit->GetValue();
			vec3 HSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
			vec3 RGB = HsvToRgb(vec3(Hue, HSV.g, HSV.b));

			m_pColorEdit->SetValue(vec4(RGB.r, RGB.g, RGB.b, Color.a));
		}
		else if(m_Clicked == 2)
		{
			vec4 Color = m_pColorEdit->GetValue();
			float CursorAngle = m_LastHue*2.0f*Pi;
			
			float Radius1 = min(m_DrawRect.w, m_DrawRect.h)/2.0f - 1.0f;
			float Radius0 = Radius1*0.8f;
			
			vec2 Center(m_DrawRect.x + m_DrawRect.w/2, m_DrawRect.y + m_DrawRect.h/2);
			vec2 HuePos(Center.x + Radius0*cos(CursorAngle), Center.y + Radius0*sin(CursorAngle));
			vec2 SaturationPos(Center.x + Radius0*cos(CursorAngle+2.0f*Pi/3.0f), Center.y + Radius0*sin(CursorAngle+2.0f*Pi/3.0f));
			vec2 ValuePos(Center.x + Radius0*cos(CursorAngle-2.0f*Pi/3.0f), Center.y + Radius0*sin(CursorAngle-2.0f*Pi/3.0f));
			vec2 Origin = (SaturationPos + HuePos)/2.0f;
			float Size = length(SaturationPos - HuePos);
			float Size2 = length(ValuePos - Origin);
			
			//Find the closest point to the triangle
			{
				vec2 Vertices[3];
				Vertices[0] = HuePos - MousePos;
				Vertices[1] = SaturationPos - MousePos;
				Vertices[2] = ValuePos - MousePos;
				
				for(int e=0; e<3; e++)
				{
					vec2 Edge = Vertices[(e+1)%3] - Vertices[e];
					if(Edge.x * Vertices[e].y - Edge.y * Vertices[e].x > 0.0f)
					{
						vec2 Dir = normalize(Edge);
						MousePos += Vertices[e] + Dir * clamp(dot(Dir, -Vertices[e]), 0.0f, Size);
						break;
					}
				}
			}
			
			float Value = clamp(1.0f-dot(normalize(ValuePos - Origin), MousePos - Origin)/Size2, 0.0f, 1.0f);
			vec2 ValueOrigin = Origin + (ValuePos - Origin)*(1.0f-Value) - (HuePos - SaturationPos)*Value/2.0f;
			float Saturation = clamp(length(MousePos - ValueOrigin)/(Size*Value), 0.0f, 1.0f);
			
			vec3 RGB = HsvToRgb(vec3(m_LastHue, Saturation, Value));

			m_pColorEdit->SetValue(vec4(RGB.r, RGB.g, RGB.b, Color.a));
		}
	}

	virtual void OnButtonClick(int Button)
	{
		if(Button != KEY_MOUSE_1)
			return;
		
		vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		
		vec4 Color = m_pColorEdit->GetValue();
		vec3 ColorHSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
		float CursorAngle = ColorHSV.r*2.0f*Pi;
		
		float Radius1 = min(m_DrawRect.w, m_DrawRect.h)/2.0f - 1.0f;
		float Radius0 = Radius1*0.8f;
		
		vec2 Center(m_DrawRect.x + m_DrawRect.w/2, m_DrawRect.y + m_DrawRect.h/2);
		vec2 HuePos(Center.x + Radius0*cos(CursorAngle), Center.y + Radius0*sin(CursorAngle));
		vec2 SaturationPos(Center.x + Radius0*cos(CursorAngle+2.0f*Pi/3.0f), Center.y + Radius0*sin(CursorAngle+2.0f*Pi/3.0f));
		vec2 ValuePos(Center.x + Radius0*cos(CursorAngle-2.0f*Pi/3.0f), Center.y + Radius0*sin(CursorAngle-2.0f*Pi/3.0f));
		
		float d = distance(Center, MousePos);
		if(Radius0 < d && d < Radius1)
		{
			m_Clicked = 1;
		}
		else
		{
			vec2 Vertices[3];
			Vertices[0] = HuePos - MousePos;
			Vertices[1] = SaturationPos - MousePos;
			Vertices[2] = ValuePos - MousePos;
			
			bool isInside = true;
			for(int i=0; i<3; i++)
			{
				vec2 Edge = Vertices[(i+1)%3] - Vertices[i];
				if(Edge.x * Vertices[i].y - Edge.y * Vertices[i].x > 0.0f)
				{
					isInside = false;
					break;
				}
			}
			
			if(isInside)
			{
				m_Clicked = 2;
				m_LastHue = ColorHSV.r;
			}
			else m_Clicked = 0;
		}
	}

	virtual void OnButtonRelease(int Button)
	{
		if(Button != KEY_MOUSE_1)
			return;
		
		m_Clicked = 0;
	}
};

/* EDITS **************************************************************/

class CRGBIntegerEdit : public gui::CAbstractFloatEdit
{
protected:
	CAbstractColorEdit* m_pColorEdit;
	int m_C;
	
protected:
	virtual void SetValue(float Value)
	{
		vec4 Color = m_pColorEdit->GetValue();
		*(static_cast<float*>(&Color.r)+m_C) = Value/255.0f;
		m_pColorEdit->SetValue(Color);
	}
	
	virtual float GetValue() const
	{
		vec4 Color = m_pColorEdit->GetValue();
		float Value = *(static_cast<float*>(&Color.r)+m_C);
		return Value*255.0f;
	}
	
public:
	CRGBIntegerEdit(CAbstractColorEdit* pColorEdit, int C) :
		gui::CAbstractFloatEdit(pColorEdit->Context()),
		m_pColorEdit(pColorEdit),
		m_C(C)
	{
		
	}
};

class CRGBFloatEdit : public gui::CAbstractFloatEdit
{
protected:
	CAbstractColorEdit* m_pColorEdit;
	int m_C;

protected:
	virtual void SetValue(float Value)
	{
		vec4 Color = m_pColorEdit->GetValue();
		*(static_cast<float*>(&Color.r)+m_C) = Value;
		m_pColorEdit->SetValue(Color);
	}
	
	virtual float GetValue() const
	{
		vec4 Color = m_pColorEdit->GetValue();
		float Value = *(static_cast<float*>(&Color.r)+m_C);
		return Value;
	}
	
public:
	CRGBFloatEdit(CAbstractColorEdit* pColorEdit, int C) :
		gui::CAbstractFloatEdit(pColorEdit->Context()),
		m_pColorEdit(pColorEdit),
		m_C(C)
	{
		
	}
};

class CHSVIntegerEdit : public gui::CAbstractFloatEdit
{
protected:
	CAbstractColorEdit* m_pColorEdit;
	int m_C;
	
	virtual void SetValue(float Value)
	{
		vec4 Color = m_pColorEdit->GetValue();
		vec3 ColorHSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
		*(static_cast<float*>(&ColorHSV.r)+m_C) = Value/255.0f;
		vec3 ColorRGB = HsvToRgb(vec3(ColorHSV.r, ColorHSV.g, ColorHSV.b));
		m_pColorEdit->SetValue(vec4(ColorRGB.r, ColorRGB.g, ColorRGB.b, Color.a));
	}
	
	virtual float GetValue() const
	{
		vec4 Color = m_pColorEdit->GetValue();
		vec3 ColorHSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
		float Value = *(static_cast<float*>(&ColorHSV.r)+m_C);
		return Value*255.0f;
	}
	
public:
	CHSVIntegerEdit(CAbstractColorEdit* pColorEdit, int C) :
		gui::CAbstractFloatEdit(pColorEdit->Context()),
		m_pColorEdit(pColorEdit),
		m_C(C)
	{
		
	}
};

class CHSVFloatEdit : public gui::CAbstractFloatEdit
{
protected:
	CAbstractColorEdit* m_pColorEdit;
	int m_C;
	
	virtual void SetValue(float Value)
	{
		vec4 Color = m_pColorEdit->GetValue();
		vec3 ColorHSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
		*(static_cast<float*>(&ColorHSV.r)+m_C) = Value;
		vec3 ColorRGB = HsvToRgb(vec3(ColorHSV.r, ColorHSV.g, ColorHSV.b));
		m_pColorEdit->SetValue(vec4(ColorRGB.r, ColorRGB.g, ColorRGB.b, Color.a));
	}
	
	virtual float GetValue() const
	{
		vec4 Color = m_pColorEdit->GetValue();
		vec3 ColorHSV = RgbToHsv(vec3(Color.r, Color.g, Color.b));
		float Value = *(static_cast<float*>(&ColorHSV.r)+m_C);
		return Value;
	}
	
public:
	CHSVFloatEdit(CAbstractColorEdit* pColorEdit, int C) :
		gui::CAbstractFloatEdit(pColorEdit->Context()),
		m_pColorEdit(pColorEdit),
		m_C(C)
	{
		
	}
};

class CHexaEdit : public gui::CAbstractTextEdit
{
protected:
	CAbstractColorEdit* m_pColorEdit;
	
protected:
	virtual void SaveFromTextBuffer()
	{
		const char* pText = GetText();
		pText = str_skip_whitespaces((char*) pText);
		if(pText && *pText == '#')
			pText++;
		pText = str_skip_whitespaces((char*) pText);
		
		uint32 Value = str_to_int_base(pText, 16);
		m_pColorEdit->SetValue(vec4(
			static_cast<float>((Value>>24) & 255)/255.0f,
			static_cast<float>((Value>>16) & 255)/255.0f,
			static_cast<float>((Value>>8) & 255)/255.0f,
			static_cast<float>(Value & 255)/255.0f
		));
	}
	
	virtual void CopyToTextBuffer()
	{
		vec4 Color = m_pColorEdit->GetValue();
		uint32 Value = static_cast<uint32>(Color.a * 255.0f);
		Value += (static_cast<uint32>(Color.b * 255.0f)<<8);
		Value += (static_cast<uint32>(Color.g * 255.0f)<<16);
		Value += (static_cast<uint32>(Color.r * 255.0f)<<24);
		
		char aBuf[16];
		str_format(aBuf, sizeof(aBuf), "#%08X", Value);
		SetText(aBuf);
	}
	
public:
	CHexaEdit(CAbstractColorEdit* pColorEdit) :
		gui::CAbstractTextEdit(pColorEdit->Context()),
		m_pColorEdit(pColorEdit)
	{
		
	}
};

/* POPUP **************************************************************/

class CColorPopup : public CPopup
{
protected:
	CAbstractColorEdit* m_pColorEdit;
	
public:
	CColorPopup(CGui *pContext, CAbstractColorEdit* pColorEdit, CRect CreatorRect) :
		CPopup(pContext, CreatorRect, 400, 400, gui::CPopup::ALIGNMENT_SIDE),
		m_pColorEdit(pColorEdit)
	{
		CAssetPath RGBIcon;
		CAssetPath HSVIcon;
		CAssetPath SquareIcon;
		CAssetPath WheelIcon;
		
		const CAsset_GuiColorEditStyle* pStyle = AssetsManager()->GetAsset<CAsset_GuiColorEditStyle>(m_pColorEdit->GetColorEditStyle());
		if(pStyle)
		{
			SetBoxStyle(pStyle->GetPopupStylePath());
			RGBIcon = pStyle->GetRGBIconPath();
			HSVIcon = pStyle->GetHSVIconPath();
			SquareIcon = pStyle->GetSquareIconPath();
			WheelIcon = pStyle->GetWheelIconPath();
		}
		
		CExternalTabs* pTab = new CExternalTabs(Context(), &(Context()->m_Cfg_ColorChooserTab));
		
		//RGB
		{
			gui::CVListLayout* pVLayout = new gui::CVListLayout(Context());
			pTab->AddTab(pVLayout, "RGB", RGBIcon);
			
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Red:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CRGBFloatEdit(m_pColorEdit, 0), true);
				pLayout->Add(new CRGBIntegerEdit(m_pColorEdit, 0), true);
				
				pVLayout->Add(new CRGBSliderEdit(m_pColorEdit, 0), false);
			}
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Green:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CRGBFloatEdit(m_pColorEdit, 1), true);
				pLayout->Add(new CRGBIntegerEdit(m_pColorEdit, 1), true);
				
				pVLayout->Add(new CRGBSliderEdit(m_pColorEdit, 1), false);
			}
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Blue:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CRGBFloatEdit(m_pColorEdit, 2), true);
				pLayout->Add(new CRGBIntegerEdit(m_pColorEdit, 2), true);
				
				pVLayout->Add(new CRGBSliderEdit(m_pColorEdit, 2), false);
			}
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Alpha:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CRGBFloatEdit(m_pColorEdit, 3), true);
				pLayout->Add(new CRGBIntegerEdit(m_pColorEdit, 3), true);
				
				pVLayout->Add(new CAlphaSliderEdit(m_pColorEdit), false);
			}
			
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Code:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CHexaEdit(m_pColorEdit), true);
			}
		}
		
		//HSV
		{
			gui::CVListLayout* pVLayout = new gui::CVListLayout(Context());
			pTab->AddTab(pVLayout, "HSV", HSVIcon);
			
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Hue:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CHSVFloatEdit(m_pColorEdit, 0), true);
				pLayout->Add(new CHSVIntegerEdit(m_pColorEdit, 0), true);
				
				pVLayout->Add(new CHSVSliderEdit(m_pColorEdit, 0), false);
			}
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Saturation:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CHSVFloatEdit(m_pColorEdit, 1), true);
				pLayout->Add(new CHSVIntegerEdit(m_pColorEdit, 1), true);
				
				pVLayout->Add(new CHSVSliderEdit(m_pColorEdit, 1), false);
			}
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Value:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CHSVFloatEdit(m_pColorEdit, 2), true);
				pLayout->Add(new CHSVIntegerEdit(m_pColorEdit, 2), true);
				
				pVLayout->Add(new CHSVSliderEdit(m_pColorEdit, 2), false);
			}
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Alpha:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CRGBFloatEdit(m_pColorEdit, 3), true);
				pLayout->Add(new CRGBIntegerEdit(m_pColorEdit, 3), true);
				
				pVLayout->Add(new CAlphaSliderEdit(m_pColorEdit), false);
			}
			
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Code:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CHexaEdit(m_pColorEdit), true);
			}
		}
		
		//Square
		{
			gui::CVListLayout* pVLayout = new gui::CVListLayout(Context());
			pTab->AddTab(pVLayout, "Square", SquareIcon);
			
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				pLayout->Add(new CSquarePicker(m_pColorEdit), true);
				pLayout->Add(new CHueVSliderEdit(m_pColorEdit), false);
			}
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Alpha:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CRGBFloatEdit(m_pColorEdit, 3), true);
				pLayout->Add(new CRGBIntegerEdit(m_pColorEdit, 3), true);
				
				pVLayout->Add(new CAlphaSliderEdit(m_pColorEdit), false);
			}
			
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Code:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CHexaEdit(m_pColorEdit), true);
			}
		}
		
		//Wheel
		{
			gui::CVListLayout* pVLayout = new gui::CVListLayout(Context());
			pTab->AddTab(pVLayout, "Wheel", WheelIcon);
			
			pVLayout->Add(new CWheelPicker(m_pColorEdit), true);
			
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Alpha:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CRGBFloatEdit(m_pColorEdit, 3), true);
				pLayout->Add(new CRGBIntegerEdit(m_pColorEdit, 3), true);
				
				pVLayout->Add(new CAlphaSliderEdit(m_pColorEdit), false);
			}
			
			{
				gui::CHListLayout* pLayout = new gui::CHListLayout(Context());
				pVLayout->Add(pLayout, false);
				
				gui::CLabel* pLabel = new gui::CLabel(Context(), "Code:");
				pLayout->Add(pLabel, true);
				pLayout->Add(new CHexaEdit(m_pColorEdit), true);
			}
		}
		
		Add(pTab);
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

/* COLOR EDIT *********************************************************/

CAbstractColorEdit::CAbstractColorEdit(CGui *pContext) :
	CButton(pContext, "")
{
	SetColorEditStyle(Context()->GetColorEditStyle());
	
	SetIconWidget(new CColorIcon(Context(), this));
}

void CAbstractColorEdit::SetColorEditStyle(CAssetPath Path)
{
	m_ColorEditStyePath = Path;
	
	const CAsset_GuiColorEditStyle* pStyle = AssetsManager()->GetAsset<CAsset_GuiColorEditStyle>(m_ColorEditStyePath);
	if(pStyle)
		SetButtonStyle(pStyle->GetButtonStylePath());
}

void CAbstractColorEdit::MouseClickAction()
{
	Context()->DisplayPopup(new CColorPopup(Context(), this, m_DrawRect));
}

}
