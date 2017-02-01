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

#ifndef __CLIENT_ASSETSRENDERER__
#define __CLIENT_ASSETSRENDERER__

#include <shared/components/assetsmanager.h>
#include <client/kernel.h>
#include <client/gui/rect.h>

class CAssetsRenderer : public CClientKernel::CComponent
{
public:
	struct CSpriteInfo
	{
		CAssetPath m_ImagePath;
		float m_Width;
		float m_Height;
		float m_UMin;
		float m_UMax;
		float m_VMin;
		float m_VMax;
	};
	
protected:
	void Draw_RoundRect_Background(float x, float y, float w, float h, float r, int Corners);
	void Draw_RoundRect_Border(float x, float y, float w, float h, float r, int Borders, int Corners);
	
public:
	CAssetsRenderer(CClientKernel* pKernel);
	
	void TextureSet(CAssetPath AssetPath);
	
	bool GetSpriteInfo(CAssetPath SpritePath, CSpriteInfo& Info);
	
	void DrawSprite(CAssetPath AssetPath, vec2 Pos, vec2 Size, float Angle, int FlipFlag, vec4 Color);
	void DrawGuiRect(const gui::CRect* pRect, CAssetPath StylePath);
	void DrawGuiLine(ivec2 Point0, ivec2 Point1, CAssetPath StylePath);
};

#endif
