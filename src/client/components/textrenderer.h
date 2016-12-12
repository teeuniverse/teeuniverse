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

#ifndef __CLIENT_TEXTRENDERER__
#define __CLIENT_TEXTRENDERER__

#include <client/kernel.h>
#include <client/components/graphics.h>
#include <client/gui/rect.h>
#include <shared/math/vector.h>
#include <shared/tl/array.h>
#include <shared/tl/sorted_array.h>

//FreeType to load font and rasterization
#include <ft2build.h>
#include FT_FREETYPE_H

//ICU to apply BiDi algorithm
#include <unicode/unistr.h> //To convert utf8 to utf16

//HarfBuff to shape the text (arabic, ...)
#ifdef HARFBUZZ_ENABLED
#include <harfbuzz/hb.h>
#endif
	
class CTextRenderer : public CClientKernel::CComponent
{
protected:
	class CFont
	{
	public:
		dynamic_string m_Filename;
		FT_Face m_FtFace;
		
#ifdef HARFBUZZ_ENABLED
		hb_font_t* m_pHBFont;
#endif
		
		~CFont();
	};
	
	struct CBlock
	{
	public:
		ivec2 m_Granularity;
		int m_Size;
		int m_MaxSize;
		
		inline bool IsFull() const { return (m_Size >= m_MaxSize); }
	};
	
	struct CGlyphId
	{
		int m_FontId;
		int m_GlyphCode;
		
		CGlyphId() : m_FontId(0), m_GlyphCode(0) {}
		CGlyphId(int GlyphCode) : m_FontId(0), m_GlyphCode(GlyphCode) {}
		CGlyphId(int FontId, int GlyphCode) : m_FontId(FontId), m_GlyphCode(GlyphCode) {}
		
		inline bool operator==(const CGlyphId& GlyphId) const { return ((m_FontId == GlyphId.m_FontId) && (m_GlyphCode == GlyphId.m_GlyphCode)); }
		inline bool operator<(const CGlyphId& GlyphId) const { return ((m_FontId == GlyphId.m_FontId) ? (m_GlyphCode < GlyphId.m_GlyphCode) : (m_FontId < GlyphId.m_FontId)); }
	};
	
	struct CShaperGlyph
	{
		CGlyphId m_GlyphId;
		int m_CharPos;
	};
	
	struct CGlyph
	{
		CGlyphId m_GlyphId;
		int m_RenderTick;
		int m_Width;
		int m_Height;
		ivec2 m_Granularity;
		int m_Block;
		int m_BlockPos;
		char* m_pData;
		vec2 m_UVMin;
		vec2 m_UVMax;
		
		float m_AdvanceX;
		float m_OffsetX;
		float m_OffsetY;
		
		//For sorting: this speed up *a lot* the time needed to search a glyph
		CGlyph() {}
		CGlyph(const CGlyphId& GlyphId) : m_GlyphId(GlyphId) {}
		inline bool operator==(const CGlyphId& GlyphId) const { return (m_GlyphId == GlyphId); }
		inline bool operator<(const CGlyphId& GlyphId) const { return (m_GlyphId < GlyphId); }
		inline bool operator<(const CGlyph& Glyph) const { return (m_GlyphId < Glyph.m_GlyphId); }
	};
	
	class CGlyphCache : public CClientKernel::CGuest
	{
	public:
		int m_FontSize;
		int m_PPG;
		int m_GPB;
		int m_PPB;
		int m_OffsetY;
		int m_RenderTick;
		int m_Width;
		int m_Height;
		char* m_pData;
		CTextureHandle m_Texture;
		int m_MemoryUsage;
		int m_Version;
		
		array<CBlock> m_Blocks;
		sorted_array<CGlyph> m_Glyphs;
		
	private:
		void IncreaseCache();
		int NewBlock(ivec2 Granularity);
		void UpdateGlyph(CGlyph* pGlyph);
		
	public:
		CGlyphCache(CClientKernel* pKernel);
		~CGlyphCache();
		void Init(int FontSize);
		CGlyph* NewGlyph(CGlyphId GlyphId, int Width, int Height);
		CGlyph* FindGlyph(CGlyphId GlyphId);
		void UpdateTexture(CGlyph* pGlyph, const char* pData);
		void UpdateVersion();
	};
	
public:
	//Warning: the cache assume that the text is still the same.
	class CTextCache
	{
	public:
		struct CQuad
		{
			int m_CharPos;			//Character in the original string that has created this quad
			vec2 m_AdvancePos;
			vec2 m_QuadPos;
			vec2 m_Size;
			vec2 m_UVMin;
			vec2 m_UVMax;
		};
		
	public:
		bool m_Rendered;
		
		dynamic_string m_Text;				//Store the text to detect if the cache must be updated
		ivec2 m_BoxSize;			//To check if the context is different
		float m_FontSize;
		
		int m_GlyphCacheId;			//Any modification in the GlyphCache can make obsolete the TextCache
		int m_GlyphCacheVersion;
					
		array<CQuad> m_Quads;		//Keep quads to redraw quickly
		float m_TextWidth;
		
	public:
		CTextCache();
		void ResetRendering();
		void SetText(const char* pText);
		void SetBoxSize(ivec2 BoxSize);
		void SetFontSize(float FontSize);
	};
	
	struct CTextCursor
	{
		int m_TextIter;
		vec2 m_Position;
	};
	
private:
	FT_Library m_FTLibrary;
	array<CFont*> m_Fonts;
	array<CGlyphCache*> m_GlyphCaches;
	int m_RenderTick;

private:
	CGlyph* LoadGlyph(CGlyphCache* pCache, CGlyphId GlyphId);
	CGlyph* FindGlyph(CGlyphCache* pCache, CGlyphId GlyphId);
	CGlyph* GetGlyph(CGlyphCache* pCache, CGlyphId GlyphId);
	
	void UpdateTextCache_Shaper(array<CShaperGlyph>* pGlyphChain, const UChar* pTextUTF16, int Start, int Length, bool IsRTL, int FontId);
	void UpdateTextCache_Font(array<CShaperGlyph>* pGlyphChain, const UChar* pTextUTF16, int Start, int Length, bool IsRTL);
	void UpdateTextCache_BiDi(array<CShaperGlyph>* pGlyphChain, const char* pText);

public:
	CTextRenderer(CClientKernel* pKernel);
	virtual ~CTextRenderer();
	
	virtual bool Init();
	virtual bool PreUpdate();
	
	bool LoadFont(const char* pFilename);
	
	void UpdateTextCache(CTextCache* pTextCache);
	float GetTextWidth(CTextCache* pTextCache);
	CTextCursor GetTextCursorFromPosition(CTextCache* pTextCache, ivec2 TextPosition, ivec2 MousePosition);
	CTextCursor GetTextCursorFromTextIter(CTextCache* pTextCache, ivec2 TextPosition, int TextIter);
	void DrawText(CTextCache* pTextCache, ivec2 Position, vec4 Color);
};

#endif
