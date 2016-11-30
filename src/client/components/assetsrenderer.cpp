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
	
CAssetsRenderer::CAssetsRenderer(CClientKernel* pKernel) :
	CClientKernel::CComponent(pKernel)
{
	SetName("AssetsRenderer");
}

void CAssetsRenderer::TextureSet(CAssetPath AssetPath)
{
	const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(AssetPath);
	if(pImage)
		Graphics()->TextureSet(pImage->GetTexture());
	else
		Graphics()->TextureClear();
}

bool CAssetsRenderer::GetSpriteInfo(CAssetPath SpritePath, CSpriteInfo& Info)
{
	const CAsset_Sprite* pSprite = AssetsManager()->GetAsset<CAsset_Sprite>(SpritePath);
	if(!pSprite)
		return false;
	
	int ImageWidth = 1;
	int ImageHeight = 1;
	int GridWidth = 1;
	int GridHeight = 1;
	float TexelSize = 1.0f;
	
	const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(pSprite->GetImagePath());
	if(pImage)
	{
		ImageWidth = pImage->GetDataWidth();
		ImageHeight = pImage->GetDataHeight();
		GridWidth = pImage->GetGridWidth();
		GridHeight = pImage->GetGridHeight();
		TexelSize = static_cast<float>(pImage->GetTexelSize())/1024.0f;
		Info.m_ImagePath = pSprite->GetImagePath();
	}
	
	float texStepX = 1.0f/GridWidth;
	float texStepY = 1.0f/GridHeight;
	Info.m_UMin = pSprite->GetX()*texStepX;
	Info.m_UMax = (pSprite->GetX()+pSprite->GetWidth())*texStepX;
	Info.m_VMin = pSprite->GetY()*texStepY;
	Info.m_VMax = (pSprite->GetY()+pSprite->GetHeight())*texStepY;
	Info.m_Width = TexelSize * pSprite->GetWidth() * static_cast<float>(ImageWidth) / GridWidth;
	Info.m_Height = TexelSize * pSprite->GetHeight() * static_cast<float>(ImageHeight) / GridHeight;
	
	return true;
}

void CAssetsRenderer::DrawSprite(CAssetPath SpritePath, vec2 Pos, vec2 Size, float Angle, int Flags, vec4 Color)
{
	//Get sprite
	CSpriteInfo SpriteInfo;
	if(!GetSpriteInfo(SpritePath, SpriteInfo))
		return;
	
	TextureSet(SpriteInfo.m_ImagePath);
	
	Graphics()->BlendNormal();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(Color, true);
	
	//Compute texture subquad
	float Temp = 0;
	if(Flags&CAsset_Sprite::FLAG_FLIP_Y)
	{
		Temp = SpriteInfo.m_VMin;
		SpriteInfo.m_VMin = SpriteInfo.m_VMax;
		SpriteInfo.m_VMax = Temp;
	}

	if(Flags&CAsset_Sprite::FLAG_FLIP_X)
	{
		Temp = SpriteInfo.m_UMin;
		SpriteInfo.m_UMin = SpriteInfo.m_UMax;
		SpriteInfo.m_UMax = Temp;
	}

	Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, SpriteInfo.m_VMax);
	Graphics()->QuadsSetRotation(Angle);

	//Compute sprite size
	vec2 QuadSize = vec2(SpriteInfo.m_Width, SpriteInfo.m_Height) * Size;
	
	//Draw
	CGraphics::CQuadItem QuadItem(Pos.x, Pos.y, QuadSize.x, QuadSize.y);
	Graphics()->QuadsDraw(&QuadItem, 1);
	
	Graphics()->QuadsEnd();
}

void CAssetsRenderer::Draw_RoundRect_Background(float x, float y, float w, float h, float r, int Corners)
{
	CGraphics::CFreeformItem ArrayF[32];
	int NumItems = 0;
	int Num = 8;
	for(int i = 0; i < Num; i+=2)
	{
		float a1 = i/(float)Num * pi/2;
		float a2 = (i+1)/(float)Num * pi/2;
		float a3 = (i+2)/(float)Num * pi/2;
		float Ca1 = cosf(a1);
		float Ca2 = cosf(a2);
		float Ca3 = cosf(a3);
		float Sa1 = sinf(a1);
		float Sa2 = sinf(a2);
		float Sa3 = sinf(a3);

		if(Corners&1) // TL
			ArrayF[NumItems++] = CGraphics::CFreeformItem(
			x+r, y+r,
			x+(1-Ca1)*r, y+(1-Sa1)*r,
			x+(1-Ca3)*r, y+(1-Sa3)*r,
			x+(1-Ca2)*r, y+(1-Sa2)*r);

		if(Corners&2) // TR
		ArrayF[NumItems++] = CGraphics::CFreeformItem(
			x+w-r, y+r,
			x+w-r+Ca1*r, y+(1-Sa1)*r,
			x+w-r+Ca3*r, y+(1-Sa3)*r,
			x+w-r+Ca2*r, y+(1-Sa2)*r);

		if(Corners&4) // BL
		ArrayF[NumItems++] = CGraphics::CFreeformItem(
			x+r, y+h-r,
			x+(1-Ca1)*r, y+h-r+Sa1*r,
			x+(1-Ca3)*r, y+h-r+Sa3*r,
			x+(1-Ca2)*r, y+h-r+Sa2*r);

		if(Corners&8) // BR
		ArrayF[NumItems++] = CGraphics::CFreeformItem(
			x+w-r, y+h-r,
			x+w-r+Ca1*r, y+h-r+Sa1*r,
			x+w-r+Ca3*r, y+h-r+Sa3*r,
			x+w-r+Ca2*r, y+h-r+Sa2*r);
	}
	Graphics()->QuadsDrawFreeform(ArrayF, NumItems);

	CGraphics::CQuadItem ArrayQ[9];
	NumItems = 0;
	ArrayQ[NumItems++] = CGraphics::CQuadItem(x+r, y+r, w-r*2, h-r*2); // center
	ArrayQ[NumItems++] = CGraphics::CQuadItem(x+r, y, w-r*2, r); // top
	ArrayQ[NumItems++] = CGraphics::CQuadItem(x+r, y+h-r, w-r*2, r); // bottom
	ArrayQ[NumItems++] = CGraphics::CQuadItem(x, y+r, r, h-r*2); // left
	ArrayQ[NumItems++] = CGraphics::CQuadItem(x+w-r, y+r, r, h-r*2); // right

	if(!(Corners&1)) ArrayQ[NumItems++] = CGraphics::CQuadItem(x, y, r, r); // TL
	if(!(Corners&2)) ArrayQ[NumItems++] = CGraphics::CQuadItem(x+w, y, -r, r); // TR
	if(!(Corners&4)) ArrayQ[NumItems++] = CGraphics::CQuadItem(x, y+h, r, -r); // BL
	if(!(Corners&8)) ArrayQ[NumItems++] = CGraphics::CQuadItem(x+w, y+h, -r, -r); // BR

	Graphics()->QuadsDrawTL(ArrayQ, NumItems);
}

void CAssetsRenderer::Draw_RoundRect_Border(float x, float y, float w, float h, float r, int Borders, int Corners)
{
	CGraphics::CLineItem ArrayL[64+12];
	int NumItems = 0;
	int Num = 8;
	for(int i = 0; i < Num; i+=2)
	{
		float a1 = i/(float)Num * pi/2;
		float a2 = (i+1)/(float)Num * pi/2;
		float a3 = (i+2)/(float)Num * pi/2;
		float Ca1 = cosf(a1);
		float Ca2 = cosf(a2);
		float Ca3 = cosf(a3);
		float Sa1 = sinf(a1);
		float Sa2 = sinf(a2);
		float Sa3 = sinf(a3);

		if((Corners&1) && (Borders&(1|4))) // TL
		{
			ArrayL[NumItems++] = CGraphics::CLineItem(x+(1-Ca1)*r, y+(1-Sa1)*r, x+(1-Ca2)*r, y+(1-Sa2)*r);
			ArrayL[NumItems++] = CGraphics::CLineItem(x+(1-Ca2)*r, y+(1-Sa2)*r, x+(1-Ca3)*r, y+(1-Sa3)*r);
		}
		
		if((Corners&2) && (Borders&(1|8))) // TR
		{
			ArrayL[NumItems++] = CGraphics::CLineItem(x+w-r+Ca1*r, y+(1-Sa1)*r, x+w-r+Ca2*r, y+(1-Sa2)*r);
			ArrayL[NumItems++] = CGraphics::CLineItem(x+w-r+Ca2*r, y+(1-Sa2)*r, x+w-r+Ca3*r, y+(1-Sa3)*r);
		}
		
		if((Corners&4) && (Borders&(2|4))) // BL
		{
			ArrayL[NumItems++] = CGraphics::CLineItem(x+(1-Ca1)*r, y+h-r+Sa1*r, x+(1-Ca2)*r, y+h-r+Sa2*r);
			ArrayL[NumItems++] = CGraphics::CLineItem(x+(1-Ca2)*r, y+h-r+Sa2*r, x+(1-Ca3)*r, y+h-r+Sa3*r);
		}
		
		if((Corners&8) && (Borders&(2|8))) // BR
		{
			ArrayL[NumItems++] = CGraphics::CLineItem(x+w-r+Ca1*r, y+h-r+Sa1*r, x+w-r+Ca2*r, y+h-r+Sa2*r);
			ArrayL[NumItems++] = CGraphics::CLineItem(x+w-r+Ca2*r, y+h-r+Sa2*r, x+w-r+Ca3*r, y+h-r+Sa3*r);
		}
	}

	if(Borders&1)
		ArrayL[NumItems++] = CGraphics::CLineItem(x+r, y, x+w-r, y); // top
	if(Borders&2)
		ArrayL[NumItems++] = CGraphics::CLineItem(x+r, y+h, x+w-r, y+h); // bottom
	if(Borders&4)
		ArrayL[NumItems++] = CGraphics::CLineItem(x, y+r, x, y+h-r); // left
	if(Borders&8)
		ArrayL[NumItems++] = CGraphics::CLineItem(x+w, y+r, x+w, y+h-r); // right

	if(!(Corners&1)) // TL
	{
		if(Borders&1)
			ArrayL[NumItems++] = CGraphics::CLineItem(x, y, x+r, y);
		if(Borders&4)
			ArrayL[NumItems++] = CGraphics::CLineItem(x, y, x, y+r);
	}
	if(!(Corners&2)) // TR
	{
		if(Borders&1)
			ArrayL[NumItems++] = CGraphics::CLineItem(x+w, y, x+w-r, y);
		if(Borders&8)
			ArrayL[NumItems++] = CGraphics::CLineItem(x+w, y, x+w, y+r);
	}
	if(!(Corners&4)) // BL
	{
		if(Borders&2)
			ArrayL[NumItems++] = CGraphics::CLineItem(x, y+h, x+r, y+h);
		if(Borders&4)
			ArrayL[NumItems++] = CGraphics::CLineItem(x, y+h, x, y+h-r);
	}
	if(!(Corners&8)) // BR
	{
		if(Borders&2)
			ArrayL[NumItems++] = CGraphics::CLineItem(x+w, y+h, x+w-r, y+h);
		if(Borders&8)
			ArrayL[NumItems++] = CGraphics::CLineItem(x+w, y+h, x+w, y+h-r);
	}

	Graphics()->LinesDraw(ArrayL, NumItems);
}

void CAssetsRenderer::DrawGuiRect(const gui::CRect* pRect, CAssetPath StylePath)
{
	const CAsset_GuiRectStyle* pStyle = AssetsManager()->GetAsset<CAsset_GuiRectStyle>(StylePath);
	if(!pStyle)
		return;
	
	Graphics()->BlendNormal();
	Graphics()->TextureClear();
	
	int BorderEnabled = 0;
	if(pStyle->GetBorderFlags()&CAsset_GuiRectStyle::BORDERFLAG_T)
		BorderEnabled |= 1;
	if(pStyle->GetBorderFlags()&CAsset_GuiRectStyle::BORDERFLAG_B)
		BorderEnabled |= 2;
	if(pStyle->GetBorderFlags()&CAsset_GuiRectStyle::BORDERFLAG_L)
		BorderEnabled |= 4;
	if(pStyle->GetBorderFlags()&CAsset_GuiRectStyle::BORDERFLAG_R)
		BorderEnabled |= 8;
	
	int CornerEnabled = 0;
	if(pStyle->GetCornerFlags()&CAsset_GuiRectStyle::CORNERFLAG_TL)
		CornerEnabled |= 1;
	if(pStyle->GetCornerFlags()&CAsset_GuiRectStyle::CORNERFLAG_TR)
		CornerEnabled |= 2;
	if(pStyle->GetCornerFlags()&CAsset_GuiRectStyle::CORNERFLAG_BL)
		CornerEnabled |= 4;
	if(pStyle->GetCornerFlags()&CAsset_GuiRectStyle::CORNERFLAG_BR)
		CornerEnabled |= 8;
	
	//Background
	if(pStyle->GetFlags()&CAsset_GuiRectStyle::FLAG_BACKGROUND)
	{
		Graphics()->QuadsBegin();
		
		Graphics()->SetColor(pStyle->GetBackgroundColor(), true);
		
		gui::CRect BGRect;
		BGRect.x = pRect->x + pStyle->GetBackgroundPadding();
		BGRect.y = pRect->y + pStyle->GetBackgroundPadding();
		BGRect.w = pRect->w - 2.0f*pStyle->GetBackgroundPadding();
		BGRect.h = pRect->h - 2.0f*pStyle->GetBackgroundPadding();
		if(pStyle->GetFlags()&CAsset_GuiRectStyle::FLAG_ROUNDCORNER)
		{
			Draw_RoundRect_Background(BGRect.x, BGRect.y, BGRect.w, BGRect.h, pStyle->GetCornerRadius(), CornerEnabled);
		}
		else
		{
			CGraphics::CQuadItem QuadItem(BGRect.x+BGRect.w/2, BGRect.y+BGRect.h/2, BGRect.w, BGRect.h);
			Graphics()->QuadsDraw(&QuadItem, 1);
		}
		
		Graphics()->QuadsEnd();
	}
	
	//Image
	if(pStyle->GetFlags()&CAsset_GuiRectStyle::FLAG_IMAGE)
	{
		gui::CRect Rect;
		Rect.x = pRect->x + pStyle->GetImagePadding();
		Rect.y = pRect->y + pStyle->GetImagePadding();
		Rect.w = pRect->w - 2*pStyle->GetImagePadding();
		Rect.h = pRect->h - 2*pStyle->GetImagePadding();
		
		CSpriteInfo SpriteInfo;
		
		//TL
		if(GetSpriteInfo(pStyle->GetImageTLPath(), SpriteInfo))
		{
			TextureSet(SpriteInfo.m_ImagePath);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, true);
			Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, SpriteInfo.m_VMax);
			CGraphics::CQuadItem QuadItem(
				Rect.x-SpriteInfo.m_Width/2,
				Rect.y-SpriteInfo.m_Height/2,
				SpriteInfo.m_Width,
				SpriteInfo.m_Height
			);
			Graphics()->QuadsDraw(&QuadItem, 1);
			Graphics()->QuadsEnd();
		}
		//TR
		if(GetSpriteInfo(pStyle->GetImageTRPath(), SpriteInfo))
		{
			TextureSet(SpriteInfo.m_ImagePath);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, true);
			Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, SpriteInfo.m_VMax);
			CGraphics::CQuadItem QuadItem(
				Rect.x+Rect.w+SpriteInfo.m_Width/2,
				Rect.y-SpriteInfo.m_Height/2,
				SpriteInfo.m_Width,
				SpriteInfo.m_Height
			);
			Graphics()->QuadsDraw(&QuadItem, 1);
			Graphics()->QuadsEnd();
		}
		//BL
		if(GetSpriteInfo(pStyle->GetImageBLPath(), SpriteInfo))
		{
			TextureSet(SpriteInfo.m_ImagePath);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, true);
			Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, SpriteInfo.m_VMax);
			CGraphics::CQuadItem QuadItem(
				Rect.x-SpriteInfo.m_Width/2,
				Rect.y+Rect.h+SpriteInfo.m_Height/2,
				SpriteInfo.m_Width,
				SpriteInfo.m_Height
			);
			Graphics()->QuadsDraw(&QuadItem, 1);
			Graphics()->QuadsEnd();
		}
		//BR
		if(GetSpriteInfo(pStyle->GetImageBRPath(), SpriteInfo))
		{
			TextureSet(SpriteInfo.m_ImagePath);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, true);
			Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, SpriteInfo.m_VMax);
			CGraphics::CQuadItem QuadItem(
				Rect.x+Rect.w+SpriteInfo.m_Width/2,
				Rect.y+Rect.h+SpriteInfo.m_Height/2,
				SpriteInfo.m_Width,
				SpriteInfo.m_Height
			);
			Graphics()->QuadsDraw(&QuadItem, 1);
			Graphics()->QuadsEnd();
		}
		
		//T
		if(GetSpriteInfo(pStyle->GetImageTPath(), SpriteInfo))
		{
			TextureSet(SpriteInfo.m_ImagePath);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, true);
			Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, SpriteInfo.m_VMax);
			int LengthIter = 0;
			while(LengthIter < Rect.w)
			{
				float Width = SpriteInfo.m_Width;
				if(Rect.w - LengthIter < SpriteInfo.m_Width)
				{
					Width = Rect.w - LengthIter;
					float NewUMax = SpriteInfo.m_UMin + (SpriteInfo.m_UMax - SpriteInfo.m_UMin) * static_cast<float>(Width)/SpriteInfo.m_Width;
					Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, NewUMax, SpriteInfo.m_VMax);
				}
				CGraphics::CQuadItem QuadItem(
					Rect.x+LengthIter+Width/2,
					Rect.y-SpriteInfo.m_Height/2,
					Width,
					SpriteInfo.m_Height
				);
				Graphics()->QuadsDraw(&QuadItem, 1);
				LengthIter += Width;
			}
			Graphics()->QuadsEnd();
		}
		//B
		if(GetSpriteInfo(pStyle->GetImageBPath(), SpriteInfo))
		{
			TextureSet(SpriteInfo.m_ImagePath);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, true);
			Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, SpriteInfo.m_VMax);
			int LengthIter = 0;
			while(LengthIter < Rect.w)
			{
				float Width = SpriteInfo.m_Width;
				if(Rect.w - LengthIter < SpriteInfo.m_Width)
				{
					Width = Rect.w - LengthIter;
					float NewUMax = SpriteInfo.m_UMin + (SpriteInfo.m_UMax - SpriteInfo.m_UMin) * static_cast<float>(Width)/SpriteInfo.m_Width;
					Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, NewUMax, SpriteInfo.m_VMax);
				}
				CGraphics::CQuadItem QuadItem(
					Rect.x+LengthIter+Width/2,
					Rect.y+Rect.h+SpriteInfo.m_Height/2,
					Width,
					SpriteInfo.m_Height
				);
				Graphics()->QuadsDraw(&QuadItem, 1);
				LengthIter += Width;
			}
			Graphics()->QuadsEnd();
		}
		//L
		if(GetSpriteInfo(pStyle->GetImageLPath(), SpriteInfo))
		{
			TextureSet(SpriteInfo.m_ImagePath);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, true);
			Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, SpriteInfo.m_VMax);
			int LengthIter = 0;
			while(LengthIter < Rect.h)
			{
				float Height = SpriteInfo.m_Height;
				if(Rect.h - LengthIter < SpriteInfo.m_Height)
				{
					Height = Rect.h - LengthIter;
					float NewVMax = SpriteInfo.m_VMin + (SpriteInfo.m_VMax - SpriteInfo.m_VMin) * static_cast<float>(Height)/SpriteInfo.m_Height;
					Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, NewVMax);
				}
				CGraphics::CQuadItem QuadItem(
					Rect.x-SpriteInfo.m_Width/2,
					Rect.y+LengthIter+Height/2,
					SpriteInfo.m_Width,
					Height
				);
				Graphics()->QuadsDraw(&QuadItem, 1);
				LengthIter += Height;
			}
			Graphics()->QuadsEnd();
		}
		//R
		if(GetSpriteInfo(pStyle->GetImageRPath(), SpriteInfo))
		{
			TextureSet(SpriteInfo.m_ImagePath);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, true);
			Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, SpriteInfo.m_VMax);
			int LengthIter = 0;
			while(LengthIter < Rect.h)
			{
				float Height = SpriteInfo.m_Height;
				if(Rect.h - LengthIter < SpriteInfo.m_Height)
				{
					Height = Rect.h - LengthIter;
					float NewVMax = SpriteInfo.m_VMin + (SpriteInfo.m_VMax - SpriteInfo.m_VMin) * static_cast<float>(Height)/SpriteInfo.m_Height;
					Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, NewVMax);
				}
				CGraphics::CQuadItem QuadItem(
					Rect.x+Rect.w+SpriteInfo.m_Width/2,
					Rect.y+LengthIter+Height/2,
					SpriteInfo.m_Width,
					Height
				);
				Graphics()->QuadsDraw(&QuadItem, 1);
				LengthIter += Height;
			}
			Graphics()->QuadsEnd();
		}
	}
	
	//Border
	if(pStyle->GetFlags()&CAsset_GuiRectStyle::FLAG_BORDER)
	{
		Graphics()->LinesBegin();
		
		Graphics()->SetColor(pStyle->GetBorderColor(), true);
		
		float xMin = pRect->x-0.5f;
		float xMax = pRect->x+pRect->w;
		float yMin = pRect->y-0.5f;
		float yMax = pRect->y+pRect->h;
		
		if(pStyle->GetFlags()&CAsset_GuiRectStyle::FLAG_ROUNDCORNER)
		{
			Draw_RoundRect_Border(xMin, yMin, xMax-xMin, yMax-yMin, pStyle->GetCornerRadius()-0.5f, BorderEnabled, CornerEnabled);
		}
		else
		{
			CGraphics::CLineItem Lines[4];
			int NumLines = 0;
			
			if(BorderEnabled&1)
				Lines[NumLines++] = CGraphics::CLineItem(xMin, yMin, xMax, yMin);
			if(BorderEnabled&2)
				Lines[NumLines++] = CGraphics::CLineItem(xMin, yMax, xMax, yMax);
			if(BorderEnabled&4)
				Lines[NumLines++] = CGraphics::CLineItem(xMin, yMin, xMin, yMax);
			if(BorderEnabled&8)
				Lines[NumLines++] = CGraphics::CLineItem(xMax, yMin, xMax, yMax);
		
			Graphics()->LinesDraw(Lines, NumLines);
		}
		Graphics()->LinesEnd();
	}
}

void CAssetsRenderer::DrawGuiLine(ivec2 Point0, ivec2 Point1, CAssetPath StylePath)
{
	const CAsset_GuiLineStyle* pStyle = AssetsManager()->GetAsset<CAsset_GuiLineStyle>(StylePath);
	if(!pStyle)
		return;
	
	Graphics()->BlendNormal();
	Graphics()->TextureClear();
	
	vec2 p0(Point0.x, Point0.y);
	vec2 p1(Point1.x, Point1.y);
	vec2 Dir = normalize(p1 - p0);
	float LineLength = length(p1-p0);
	float LineAngle = angle(p1-p0);
	
	//Image
	if(pStyle->GetFlags()&CAsset_GuiLineStyle::FLAG_IMAGE)
	{
		CSpriteInfo SpriteInfo;
		
		//L
		float LWidth = 0.0f;
		if(GetSpriteInfo(pStyle->GetImageLPath(), SpriteInfo))
		{
			LWidth = SpriteInfo.m_Width;
			vec2 Shift = Dir * SpriteInfo.m_Width/2.0f;
			TextureSet(SpriteInfo.m_ImagePath);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, true);
			Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, SpriteInfo.m_VMax);
			Graphics()->QuadsSetRotation(LineAngle);
			CGraphics::CQuadItem QuadItem(
				p0.x-Shift.x,
				p0.y-Shift.y,
				SpriteInfo.m_Width,
				SpriteInfo.m_Height
			);
			Graphics()->QuadsDraw(&QuadItem, 1);
			Graphics()->QuadsEnd();
		}
		//R
		float RWidth = 0.0f;
		if(GetSpriteInfo(pStyle->GetImageRPath(), SpriteInfo))
		{
			RWidth = SpriteInfo.m_Width;
			vec2 Shift = Dir * SpriteInfo.m_Width/2.0f;
			TextureSet(SpriteInfo.m_ImagePath);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, true);
			Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, SpriteInfo.m_VMax);
			Graphics()->QuadsSetRotation(LineAngle);
			CGraphics::CQuadItem QuadItem(
				p1.x+Shift.x,
				p1.y+Shift.y,
				SpriteInfo.m_Width,
				SpriteInfo.m_Height
			);
			Graphics()->QuadsDraw(&QuadItem, 1);
			Graphics()->QuadsEnd();
		}
		//M
		if(GetSpriteInfo(pStyle->GetImageMPath(), SpriteInfo))
		{
			TextureSet(SpriteInfo.m_ImagePath);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, true);
			Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, SpriteInfo.m_UMax, SpriteInfo.m_VMax);
			Graphics()->QuadsSetRotation(LineAngle);
			int LengthIter = 0.0f;
			float IterMax = LineLength;
			while(LengthIter < IterMax)
			{
				float Width = SpriteInfo.m_Width;
				if(IterMax - LengthIter < SpriteInfo.m_Width)
				{
					Width = IterMax - LengthIter;
					float NewUMax = SpriteInfo.m_UMin + (SpriteInfo.m_UMax - SpriteInfo.m_UMin) * static_cast<float>(Width)/SpriteInfo.m_Width;
					Graphics()->QuadsSetSubset(SpriteInfo.m_UMin, SpriteInfo.m_VMin, NewUMax, SpriteInfo.m_VMax);
				}
				CGraphics::CQuadItem QuadItem(
					p0.x+Dir.x * (LengthIter + Width/2),
					p0.y+Dir.y * (LengthIter + Width/2),
					Width,
					SpriteInfo.m_Height
				);
				Graphics()->QuadsDraw(&QuadItem, 1);
				LengthIter += SpriteInfo.m_Width;
			}
			Graphics()->QuadsEnd();
		}
	}
	
	//Border
	if(pStyle->GetFlags()&CAsset_GuiLineStyle::FLAG_BORDER)
	{
		Graphics()->LinesBegin();
		Graphics()->SetColor(pStyle->GetBorderColor(), true);
		
		CGraphics::CLineItem Line(p0.x, p0.y, p1.x, p1.y);
		
		Graphics()->LinesDraw(&Line, 1);
		Graphics()->LinesEnd();
	}
}
