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

#include <client/components/textrenderer.h>
#include <shared/components/localization.h>
#include <shared/components/storage.h>

#include <unicode/schriter.h> //Character iterator to iterate over utf16 string
#include <unicode/ubidi.h> //To apply BiDi algorithm
#include <harfbuzz/hb-icu.h> //To tell HarfBuuf to use ICU
#include <harfbuzz/hb-ft.h> //To apply text shaping

/* FONT ***************************************************************/

CTextRenderer::CFont::~CFont()
{
	FT_Done_Face(m_FtFace);
	hb_font_destroy(m_pHBFont);
}

/* GLYPH CACHE ********************************************************/

CTextRenderer::CGlyphCache::CGlyphCache(CClientKernel* pKernel) :
	CClientKernel::CGuest(pKernel),
	m_pData(NULL),
	m_Version(0)
{
	
}

CTextRenderer::CGlyphCache::~CGlyphCache()
{
	if(m_pData)
		delete[] m_pData;
	
	if(m_Texture.IsValid())
		Graphics()->UnloadTexture(&m_Texture);
}

void CTextRenderer::CGlyphCache::UpdateVersion()
{
	m_Version++;
}

void CTextRenderer::CGlyphCache::UpdateGlyph(CTextRenderer::CGlyph* pGlyph)
{
	int NumBlockX = m_Width/m_PPB;
	int NumGlyphX = m_GPB/pGlyph->m_Granularity.x;
	int BlockPosX = pGlyph->m_BlockPos % NumGlyphX;
	int BlockPosY = pGlyph->m_BlockPos / NumGlyphX;
	
	int DataX = (pGlyph->m_Block % NumBlockX)*m_PPB + (BlockPosX * pGlyph->m_Granularity.x)*m_PPG;
	int DataY = (pGlyph->m_Block / NumBlockX)*m_PPB + (BlockPosY * pGlyph->m_Granularity.y)*m_PPG;
	pGlyph->m_pData = m_pData + DataY * m_Width + DataX;
	
	float UScale = 1.0f/m_Width;
	float VScale = 1.0f/m_Height;
	
	pGlyph->m_UVMin.x = DataX * UScale;
	pGlyph->m_UVMin.y = DataY * VScale;
	pGlyph->m_UVMax.x = (DataX + pGlyph->m_Width) * UScale;
	pGlyph->m_UVMax.y = (DataY + pGlyph->m_Height) * VScale;
}

const int s_Margin = 2;

void CTextRenderer::CGlyphCache::Init(int FontSize)
{
	m_FontSize = FontSize;
	m_OffsetY = m_FontSize;
	m_PPG = nearestPowerOfTwo((FontSize+2*s_Margin)/2);
	m_GPB = 8;
	m_PPB = m_GPB * m_PPG;
	
	//Remove old data
	if(m_pData)
		delete[] m_pData;
	
	if(m_Texture.IsValid())
		Graphics()->UnloadTexture(&m_Texture);
	
	m_Blocks.clear();
	m_Glyphs.clear();
	
	m_Width = 0;
	m_Height = 0;
	
	//Init	
	UpdateVersion();
}

void CTextRenderer::CGlyphCache::IncreaseCache()
{
	if(m_Width == 0 || m_Height == 0)
	{
		m_Width = m_PPB;
		m_Height = m_PPB;
		m_pData = new char[m_Width*m_Height];
		
		mem_zero(m_pData, m_Width*m_Height);
		m_Texture = Graphics()->LoadTextureRaw(m_Width, m_Height, 1, 1, CImageInfo::FORMAT_ALPHA, m_pData, CImageInfo::FORMAT_ALPHA, CGraphics::TEXLOAD_NOMIPMAPS);
		
		m_MemoryUsage = m_Width*m_Height;
	}
	else
	{
		//Compute new sizes
		int NewWidth = m_Width;
		int NewHeight = m_Height;
		
		if(NewWidth > NewHeight)
			NewHeight <<= 1;
		else
			NewWidth <<= 1;
		
		//Allocate a new buffer and copy the old inside
		char* pNewData = new char[NewWidth*NewHeight];
		mem_zero(pNewData, NewWidth*NewHeight);
			
			//Block are arranged differently to be still accessible iterativly
		if(m_pData)
		{
			int OldNumBlockX = m_Width/m_PPB;
			int NewNumBlockX = NewWidth/m_PPB;
			
			for(int b=0; b<m_Blocks.size(); b++)
			{
				for(int j=0; j<m_PPB; j++)
				{
					int OldY = (b / OldNumBlockX)*m_PPB + j;
					int NewY = (b / NewNumBlockX)*m_PPB + j;
					
					for(int i=0; i<m_PPB; i++)
					{
						int OldX = (b % OldNumBlockX)*m_PPB + i;
						int NewX = (b % NewNumBlockX)*m_PPB + i;
						
						pNewData[NewY*NewWidth+NewX] = m_pData[OldY*m_Width+OldX];
					}
				}
			}
			
			delete[] m_pData;
			
			m_MemoryUsage -= m_Width*m_Height;
			
			if(m_Texture.IsValid())
				Graphics()->UnloadTexture(&m_Texture);
		}
		m_pData = pNewData;
		m_Width = NewWidth;
		m_Height = NewHeight;
		
		//Update Glyphs
		for(int i=0; i<m_Glyphs.size(); i++)
		{
			UpdateGlyph(&m_Glyphs[i]);
		}
		
		//Update texture
		
		m_Texture = Graphics()->LoadTextureRaw(m_Width, m_Height, 1, 1, CImageInfo::FORMAT_ALPHA, m_pData, CImageInfo::FORMAT_ALPHA, CGraphics::TEXLOAD_NOMIPMAPS);
		m_MemoryUsage += m_Width*m_Height;
	}
	
	UpdateVersion();
}

int CTextRenderer::CGlyphCache::NewBlock(ivec2 Granularity)
{
	int MaxNumBlock = (m_Width * m_Height) / (m_PPB * m_PPB);
	
	if(MaxNumBlock <= m_Blocks.size())
		IncreaseCache();
	
	int BlockId = m_Blocks.size();
	CBlock& pBlock = m_Blocks.increment();
	
	pBlock.m_Granularity = Granularity;
	pBlock.m_Size = 0;
	
	int NumGlyphX = m_GPB/Granularity.x;
	int NumGlyphY = m_GPB/Granularity.y;
	pBlock.m_MaxSize = NumGlyphX * NumGlyphY;
	
	return BlockId;
}

CTextRenderer::CGlyph* CTextRenderer::CGlyphCache::NewGlyph(CGlyphId GlyphId, int Width, int Height)
{	
	ivec2 Granularity;
	Granularity.x = (Width%m_PPG == 0 ? (Width / m_PPG) : (Width / m_PPG) + 1);
	Granularity.y = (Height%m_PPG == 0 ? (Height / m_PPG) : (Height / m_PPG) + 1);
	if(Granularity.x < 1)
		Granularity.x = 1;
	if(Granularity.y < 1)
		Granularity.y = 1;
	
	//First pass: find a free slot in blocks
		//We use backward iteration because non-full blockes are at the end
	for(int i=m_Blocks.size()-1; i>=0; i--)
	{
		if(m_Blocks[i].m_Granularity == Granularity)
		{
			if(!m_Blocks[i].IsFull())
			{
				CGlyph& Glyph = m_Glyphs.add_by_copy(CGlyph(GlyphId));
				
				Glyph.m_Width = Width;
				Glyph.m_Height = Height;
				Glyph.m_Granularity = Granularity;
				Glyph.m_Block = i;
				Glyph.m_BlockPos = m_Blocks[i].m_Size;
				UpdateGlyph(&Glyph);
		
				m_Blocks[Glyph.m_Block].m_Size++;
				
				return &Glyph;
			}
			else
				break; //Only the last block of this granularity can be non-full
		}
	}
	
	//Second pass: find the oldest glyph with the same granularity
	int OldestTick = m_RenderTick;
	int OldestGlyph = -1;
	for(int i=0; i<m_Glyphs.size(); i++)
	{
		if(m_Glyphs[i].m_Granularity == Granularity && m_Glyphs[i].m_RenderTick < OldestTick)
		{
			OldestTick = m_Glyphs[i].m_RenderTick;
			OldestGlyph = i;
		}
	}
	
	if(OldestGlyph >= 0) //Replace the glyph
	{
		CGlyph Glyph = CGlyph(GlyphId);
		Glyph.m_Block = m_Glyphs[OldestGlyph].m_Block;
		Glyph.m_BlockPos = m_Glyphs[OldestGlyph].m_BlockPos;
		
		m_Glyphs.remove_index(OldestGlyph);
		
		CGlyph& NewGlyph = m_Glyphs.add_by_copy(Glyph);
		
		NewGlyph.m_Width = Width;
		NewGlyph.m_Height = Height;
		NewGlyph.m_Granularity = Granularity;
		UpdateGlyph(&NewGlyph);
		UpdateVersion();
		
		return &NewGlyph;
	}
	else //Add a new block
	{
		int BlockId = NewBlock(Granularity);

		CGlyph& NewGlyph = m_Glyphs.increment();
		
		NewGlyph.m_Width = Width;
		NewGlyph.m_Height = Height;
		NewGlyph.m_Granularity = Granularity;
		NewGlyph.m_Block = BlockId;
		NewGlyph.m_BlockPos = 0;
		
		UpdateGlyph(&NewGlyph);
		
		m_Blocks[NewGlyph.m_Block].m_Size++;
		
		return &NewGlyph;
	}
}

CTextRenderer::CGlyph* CTextRenderer::CGlyphCache::FindGlyph(CGlyphId GlyphId)
{
	if(m_Glyphs.size() == 0)
		return 0;
	
	int iMin = 0;
	int iMax = m_Glyphs.size()-1;
	
	if(m_Glyphs[iMax] < GlyphId)
		return 0;

	while((iMax-iMin) > 1)
	{
		int Pivot = iMin + (iMax-iMin-1)/2;
		if(m_Glyphs[Pivot] < GlyphId)
			iMin = Pivot+1;
		else
			iMax = Pivot+1;
	}
	
	if(m_Glyphs[iMin] == GlyphId)
		return &m_Glyphs[iMin];
	else
		return 0;
}

void CTextRenderer::CGlyphCache::UpdateTexture(CTextRenderer::CGlyph* pGlyph, const char* pData)
{
	int DataX = (pGlyph->m_pData - m_pData) % m_Width;
	int DataY = (pGlyph->m_pData - m_pData) / m_Width;
	
	Graphics()->LoadTextureRawSub(
		m_Texture,
		DataX, DataY,
		pGlyph->m_Granularity.x * m_PPG,
		pGlyph->m_Granularity.y * m_PPG,
		CImageInfo::FORMAT_ALPHA,
		pData
	);
}

/* TEXT CACHE *********************************************************/

CTextRenderer::CTextCache::CTextCache() :
	m_GlyphCacheId(-1),
	m_Rendered(false)
{
	
}

void CTextRenderer::CTextCache::SetText(const char* pText)
{
	if(pText == 0)
	{
		m_Rendered = false;
		m_Text.clear();
	}
	else if(m_Text.empty() || m_Text != pText)
	{
		m_Text.copy(pText);
		m_Rendered = false;
	}
}

void CTextRenderer::CTextCache::SetBoxSize(ivec2 BoxSize)
{
	if(m_BoxSize != BoxSize)
	{
		m_BoxSize = BoxSize;
		m_Rendered = false;
	}
}

void CTextRenderer::CTextCache::SetFontSize(float FontSize)
{
	if(m_FontSize != FontSize)
	{
		m_FontSize = FontSize;
		m_Rendered = false;
	}
}


void CTextRenderer::CTextCache::ResetRendering()
{
	m_GlyphCacheId = -1;
	m_GlyphCacheVersion = -1;
	m_Quads.clear();
}

/* TEXT RENDERER ******************************************************/

CTextRenderer::CTextRenderer(CClientKernel* pKernel) :
	CClientKernel::CComponent(pKernel)
{
	SetName("TextRenderer");
}

CTextRenderer::~CTextRenderer()
{	
	for(int i=0; i<m_Fonts.size(); i++)
		delete m_Fonts[i];
	
	for(int i=0; i<m_GlyphCaches.size(); i++)
		delete m_GlyphCaches[i];
		
	m_Fonts.clear();
	m_GlyphCaches.clear();
}

static int s_aFontSizes[] = {8,9,10,11,12,13,14,15,16,17,18,19,20,36,64};
//~ static int s_aFontSizes[] = {30};
#define NUM_FONT_SIZES (sizeof(s_aFontSizes)/sizeof(int))

bool CTextRenderer::Init()
{	
	//Cleaning
	for(int i=0; i<m_Fonts.size(); i++)
		delete m_Fonts[i];
	
	for(int i=0; i<m_GlyphCaches.size(); i++)
		delete m_GlyphCaches[i];
	
	m_Fonts.clear();
	m_GlyphCaches.clear();
	
	//Init caches
	m_GlyphCaches.resize(NUM_FONT_SIZES);
	for(int i=0; i<m_GlyphCaches.size(); i++)
	{
		m_GlyphCaches[i] = new CGlyphCache(ClientKernel());
		m_GlyphCaches[i]->Init(s_aFontSizes[i]);
	}
	
	//Load FreeType
	if(FT_Init_FreeType(&m_FTLibrary) != FT_Err_Ok)
		return false;
	
	//Load Font
	if(!LoadFont("fonts/DejaVuSans.ttf"))
		return false;
	LoadFont("fonts/NotoSansCJKjp-Medium.ttf");
	
	return true;
}

bool CTextRenderer::LoadFont(const char* pFilename)
{
	dynamic_string FullPath;
	IOHANDLE File = Storage()->OpenFile(pFilename, IOFLAG_READ, CStorage::TYPE_ALL, FullPath);
	if(!File)
	{
		dbg_msg("TextRenderer", "Can't open %s", pFilename);
		return false;
	}
	
	io_close(File);
	
	CFont* pFont = new CFont();
	pFont->m_Filename.copy(FullPath);
	
	if(FT_New_Face(m_FTLibrary, pFont->m_Filename.buffer(), 0, &pFont->m_FtFace) != FT_Err_Ok)
	{
		dbg_msg("TextRenderer", "Can't create a regular font from %s", pFilename);
		delete pFont;
		return false;
	}
	
	pFont->m_pHBFont = hb_ft_font_create(pFont->m_FtFace, 0);
	
	m_Fonts.add_by_copy(pFont);
	
	return true;
}

char s_aGlyphBuffer[128*128];

CTextRenderer::CGlyph* CTextRenderer::LoadGlyph(CGlyphCache* pCache, CGlyphId GlyphId)
{
	if(m_Fonts.size() == 0)
	{
		dbg_msg("TextRenderer", "No font loaded");
		return 0;
	}
	
	CFont* pFont = m_Fonts[GlyphId.m_FontId];
	if(FT_Set_Pixel_Sizes(pFont->m_FtFace, 0, pCache->m_FontSize) != FT_Err_Ok)
	{
		dbg_msg("TextRenderer", "Can't set pixel size %d", pCache->m_FontSize);
		return 0;
	}
	if(FT_Load_Glyph(pFont->m_FtFace, GlyphId.m_GlyphCode, FT_LOAD_RENDER|FT_LOAD_NO_BITMAP) != FT_Err_Ok)
	{
		dbg_msg("TextRenderer", "Can't load glyph %d", GlyphId.m_GlyphCode);
		return 0;
	}
	
	FT_Bitmap* pBitmap = &pFont->m_FtFace->glyph->bitmap;
	int BitmapWidth = pBitmap->width;
	int BitmapHeight = pBitmap->rows;
	int BBWidth = BitmapWidth+2*s_Margin;
	int BBHeight = BitmapHeight+2*s_Margin;
	
	CTextRenderer::CGlyph* pGlyph = pCache->NewGlyph(GlyphId, BBWidth, BBHeight);
	
	pGlyph->m_GlyphId = GlyphId;
	pGlyph->m_AdvanceX = (pFont->m_FtFace->glyph->advance.x >> 6);
	pGlyph->m_OffsetX = pFont->m_FtFace->glyph->bitmap_left - s_Margin;
	pGlyph->m_OffsetY = pFont->m_FtFace->glyph->bitmap_top + s_Margin;
	
	for(int j=0; j<pGlyph->m_Granularity.y * pCache->m_PPG; j++)
	{
		for(int i=0; i<pGlyph->m_Granularity.x * pCache->m_PPG; i++)
		{
			pGlyph->m_pData[j*pCache->m_Width + i] = 0;
		}
	}
	
	if(pBitmap->pixel_mode == FT_PIXEL_MODE_GRAY)
	{
		for(int j = 0; j < BitmapHeight; j++)
		{
			for(int i = 0; i < BitmapWidth; i++)
			{
				int CacheDataIndex = (j+s_Margin)*pCache->m_Width + (i+s_Margin);
				pGlyph->m_pData[CacheDataIndex] = pBitmap->buffer[j*pBitmap->pitch+i];
			}
		}
	}
	else if(pBitmap->pixel_mode == FT_PIXEL_MODE_MONO)
	{
		for(int j = 0; j < BitmapHeight; j++)
		{
			for(int i = 0; i < BitmapWidth; i++)
			{
				int CacheDataIndex = (j+s_Margin)*pCache->m_Width + (i+s_Margin);
				if(pBitmap->buffer[j*pBitmap->pitch+i/8]&(1<<(7-(i%8))))
					pGlyph->m_pData[CacheDataIndex] = 255;
				else
					pGlyph->m_pData[CacheDataIndex] = 0;
			}
		}
	}
	
	mem_zero(s_aGlyphBuffer, (pGlyph->m_Granularity.x * pCache->m_PPG) * (pGlyph->m_Granularity.y * pCache->m_PPG));
	
	for(int j = s_Margin; j < pGlyph->m_Height-s_Margin; j++)
	{
		for(int i = s_Margin; i < pGlyph->m_Width-s_Margin; i++)
		{
			s_aGlyphBuffer[j*(pGlyph->m_Granularity.x * pCache->m_PPG) + i] = pGlyph->m_pData[j*pCache->m_Width + i];
		}
	}
	
	pCache->UpdateTexture(pGlyph, s_aGlyphBuffer);
	
	return pGlyph;
}

CTextRenderer::CGlyph* CTextRenderer::FindGlyph(CGlyphCache* pCache, CGlyphId GlyphId)
{
	return pCache->FindGlyph(GlyphId);
}


CTextRenderer::CGlyph* CTextRenderer::GetGlyph(CGlyphCache* pCache, CGlyphId GlyphId)
{
	CTextRenderer::CGlyph* pGlyph = FindGlyph(pCache, GlyphId);
	
	//Load Glyph
	if(!pGlyph)
		pGlyph = LoadGlyph(pCache, GlyphId);
	
	//Update timer
	if(pGlyph)
		pGlyph->m_RenderTick = m_RenderTick;
	
	return pGlyph;
}

void CTextRenderer::UpdateTextCache_HarfBuzz(array<CHarfbuzzGlyph>* pGlyphChain, const UChar* pTextUTF16, int Start, int Length, bool IsRTL, int FontId)
{
	//Use harfbuzz to shape the text (arabic, tamil, ...)
	hb_buffer_t* m_pHBBuffer = hb_buffer_create();
	hb_buffer_set_unicode_funcs(m_pHBBuffer, hb_icu_get_unicode_funcs());
	hb_buffer_set_direction(m_pHBBuffer, (IsRTL ? HB_DIRECTION_RTL : HB_DIRECTION_LTR));
	hb_buffer_set_script(m_pHBBuffer, HB_SCRIPT_ARABIC);
	hb_buffer_set_language(m_pHBBuffer, hb_language_from_string("ar", 2));
	
	hb_buffer_add_utf16(m_pHBBuffer, pTextUTF16, Length, Start, Length);
	hb_shape(m_Fonts[FontId]->m_pHBFont, m_pHBBuffer, 0, 0);
	
	unsigned int GlyphCount;
	hb_glyph_info_t* GlyphInfo = hb_buffer_get_glyph_infos(m_pHBBuffer, &GlyphCount);
	hb_glyph_position_t* GlyphPos = hb_buffer_get_glyph_positions(m_pHBBuffer, &GlyphCount);

	for(int i=0; i<GlyphCount; i++)
	{
		CHarfbuzzGlyph& Glyph = pGlyphChain->increment();
		Glyph.m_GlyphId = CGlyphId(FontId, GlyphInfo[i].codepoint);
		Glyph.m_CharPos = GlyphInfo[i].cluster;
	}
		
	hb_buffer_destroy(m_pHBBuffer);
}

void CTextRenderer::UpdateTextCache_Font(array<CHarfbuzzGlyph>* pGlyphChain, const UChar* pTextUTF16, int Start, int Length, bool IsRTL)
{
	int FontId = 0;
	
	int SubStart = Start;
	int SubLength = 0;
	int LastIndex = 0;
	UChar Char;
	UCharCharacterIterator Iter(pTextUTF16+Start, Length);
	while(Iter.hasNext())
	{
		Char = Iter.next32PostInc();
		int Index = Iter.getIndex();
		int Increment = Index - LastIndex;
		LastIndex = Index;
		
		//Always try to get back to the main font.
		//This is important because sometimes, CFK fonts embded latin fonts as well.
		if(FontId > 0 && FT_Get_Char_Index(m_Fonts[0]->m_FtFace, Char) > 0)
		{
			if(SubLength > Increment)
			{
				UpdateTextCache_HarfBuzz(pGlyphChain, pTextUTF16, SubStart, SubLength, IsRTL, FontId);
				
				SubStart = Start + Index - Increment;
				SubLength = 0;
			}
			FontId = 0;
		}
		//If the glyph is not in the current font
		if(FT_Get_Char_Index(m_Fonts[FontId]->m_FtFace, Char) == 0)
		{
			int FontIdFound = -1;
			int FontIdIter = (FontId+1)%m_Fonts.size();
			for(int i=0; i<m_Fonts.size(); i++)
			{
				//It's possible that using FT_Get_Char_Index one time is more time consuming than testing this condition on all fonts
				//But I'm not sure...
				if(i == FontId)
					continue;
				
				if(FT_Get_Char_Index(m_Fonts[FontIdIter]->m_FtFace, Char) > 0)
				{
					FontIdFound = FontIdIter;
					break;
				}
			}
			
			//Send the first part to harfbuzz, then continue with the new font.
			//If no glyph has been found, use the "noglyph" symbol of the main font
			if(SubLength > Increment)
			{
				UpdateTextCache_HarfBuzz(pGlyphChain, pTextUTF16, SubStart, SubLength, IsRTL, FontId);
				
				SubStart = Start + Index - Increment;
				SubLength = 0;
			}
			FontId = (FontIdFound >= 0 ? FontIdFound : 0);
		}
		
		SubLength += Increment;
	}
	
	if(SubLength > 0)
		UpdateTextCache_HarfBuzz(pGlyphChain, pTextUTF16, SubStart, SubLength, IsRTL, FontId);
}

void CTextRenderer::UpdateTextCache_BiDi(array<CHarfbuzzGlyph>* pGlyphChain, const char* pText)
{
	//Use ICU for bidirectional text
	//note: bidirectional texts appear for example when a latin username is displayed in a arabic text
	UErrorCode ICUError = U_ZERO_ERROR;
	UnicodeString UTF16Text = icu::UnicodeString::fromUTF8(pText);
	UBiDi* pICUBiDi = ubidi_openSized(UTF16Text.length(), 0, &ICUError);
	
	//Perform the BiDi algorithm
	//TODO: change UBIDI_DEFAULT_LTR by some variable dependend of the user config
	ubidi_setPara(pICUBiDi, UTF16Text.getBuffer(), UTF16Text.length(), (Localization()->GetWritingDirection() == CLocalization::DIRECTION_RTL ? UBIDI_DEFAULT_RTL : UBIDI_DEFAULT_LTR), 0, &ICUError);
	
	if(U_SUCCESS(ICUError))
	{
		UBiDiLevel ICULevel = 1&ubidi_getParaLevel(pICUBiDi);
		UBiDiDirection Direction = ubidi_getDirection(pICUBiDi);

		if(Direction != UBIDI_MIXED)
		{
			UpdateTextCache_Font(pGlyphChain, UTF16Text.getBuffer(), 0, UTF16Text.length(), (Direction == UBIDI_RTL));
		}
		else
		{
			int CharStart = 0;
			UBiDiLevel level;
			int NumberOfParts = ubidi_countRuns(pICUBiDi, &ICUError);
			if(U_SUCCESS(ICUError))
			{
				for(int i=0; i<NumberOfParts; i++)
				{
					int Start;
					int SubLength;
					Direction = ubidi_getVisualRun(pICUBiDi, i, &Start, &SubLength);

					UpdateTextCache_Font(pGlyphChain, UTF16Text.getBuffer(), Start, SubLength, (Direction == UBIDI_RTL));
				}
			}
			else
			{
				dbg_msg("TextRenderer", "BiDi algorithm failed (ubidi_countRuns): %s", u_errorName(ICUError));
				return;
			}
		}
    }
    else
    {
		dbg_msg("TextRenderer", "BiDi algorithm failed: %s", u_errorName(ICUError));
		return;
	}
}

void CTextRenderer::UpdateTextCache(CTextCache* pTextCache)
{
	bool NeedUpdate = false;
	
	if(!pTextCache->m_Rendered)
		NeedUpdate = true;
	if(pTextCache->m_GlyphCacheId < 0 || pTextCache->m_GlyphCacheId >= m_GlyphCaches.size())
		NeedUpdate = true;
	else if(pTextCache->m_GlyphCacheVersion != m_GlyphCaches[pTextCache->m_GlyphCacheId]->m_Version)
		NeedUpdate = true;
	
	if(!NeedUpdate)
		return;
	
	pTextCache->ResetRendering();
	
	//Search the appropriate cached font size to render the text
	CGlyphCache* pGlyphCache = 0;
	for(int i=0; i<m_GlyphCaches.size(); i++)
	{
		pGlyphCache = m_GlyphCaches[i];
		pTextCache->m_GlyphCacheId = i;
		
		if(m_GlyphCaches[i]->m_FontSize >= pTextCache->m_FontSize)
			break;
	}
	if(!pGlyphCache)
		return;
	
	float RelativeSize = pTextCache->m_FontSize / pGlyphCache->m_FontSize;
	array<CHarfbuzzGlyph> GlyphChain;
	
	//The text is separated in 3 levels:
		//Level 1: the complete string the the ICU will process (UpdateTextCache_BiDi)
		//Level 2: a substring that use exclusivly the LTR or RTL writing direction (UpdateTextCache_Font)
		//Level 3: a substring that use only one font, because HarfBuzz need this condition (UpdateTextCache_HarfBuzz)
	UpdateTextCache_BiDi(&GlyphChain, pTextCache->m_Text.buffer());
	
	//Here, we use two passes in order to make sure that the glyph cache is in the same state for all letters
		//First pass: Update the glyph cache
	for(int i=0; i<GlyphChain.size(); i++)
		GetGlyph(pGlyphCache, GlyphChain[i].m_GlyphId);
	
		//Second pass: Generate the list of quads
	float PosY = pTextCache->m_BoxSize.y/2 + static_cast<int>(pTextCache->m_FontSize*0.4f); //TODO: find a way to not hard code the generic glyph height.
	float PosX = 0.0f;
	for(int i=0; i<GlyphChain.size(); i++)
	{
		CGlyph* pGlyph = GetGlyph(pGlyphCache, GlyphChain[i].m_GlyphId);
		if(pGlyph)
		{
			CTextCache::CQuad& Quad = pTextCache->m_Quads.increment();
			Quad.m_AdvancePos = vec2(PosX, PosY);
			Quad.m_QuadPos = vec2(PosX + pGlyph->m_OffsetX*RelativeSize, PosY - pGlyph->m_OffsetY*RelativeSize);
			Quad.m_Size = vec2(pGlyph->m_Width*RelativeSize, pGlyph->m_Height*RelativeSize);
			Quad.m_UVMin = pGlyph->m_UVMin;
			Quad.m_UVMax = pGlyph->m_UVMax;
			Quad.m_CharPos = GlyphChain[i].m_CharPos;
			
			PosX += pGlyph->m_AdvanceX*RelativeSize;
		}
	}
	
	pTextCache->m_TextWidth = PosX;
	pTextCache->m_GlyphCacheVersion = pGlyphCache->m_Version;
	pTextCache->m_Rendered = true;
}

CTextRenderer::CTextCursor CTextRenderer::GetTextCursorFromPosition(CTextCache* pTextCache, ivec2 TextPosition, ivec2 MousePosition)
{
	UpdateTextCache(pTextCache);
	
	CTextCursor Cursor;
	Cursor.m_TextIter = 0;
	Cursor.m_Position.x = TextPosition.x;
	Cursor.m_Position.y = TextPosition.y;
	
	if(pTextCache->m_Quads.size() <= 0)
		return Cursor;
	
	int NearestQuad = 0;
	float CharDistance = 999999.0f;
	vec2 TestPos = vec2(MousePosition.x - TextPosition.x, MousePosition.y - TextPosition.y);
	for(int i=0; i<pTextCache->m_Quads.size(); i++)
	{
		float Dist = fabs(pTextCache->m_Quads[i].m_QuadPos.x + pTextCache->m_Quads[i].m_Size.x/2.0f - TestPos.x);
		if(Dist < CharDistance)
		{
			NearestQuad = i;
			CharDistance = Dist;
		}
	}
	
	int CharPos = pTextCache->m_Quads[NearestQuad].m_CharPos;
	Cursor.m_Position.x = TextPosition.x + pTextCache->m_Quads[NearestQuad].m_AdvancePos.x;
	if(TestPos.x > pTextCache->m_Quads[NearestQuad].m_QuadPos.x + pTextCache->m_Quads[NearestQuad].m_Size.x/2.0f)
	{
		if(NearestQuad == pTextCache->m_Quads.size()-1)
		{
			CharPos++;
			Cursor.m_Position.x = TextPosition.x + pTextCache->m_TextWidth;
		}
		else
		{
			CharPos = pTextCache->m_Quads[NearestQuad+1].m_CharPos;
			Cursor.m_Position.x += pTextCache->m_Quads[NearestQuad].m_Size.x;
		}
	}
	
	//Find the position of the character in the string
	const char* pText = pTextCache->m_Text.buffer();
	int Iter = 0;
	int CharIter = 0;
	while(pText[Iter])
	{
		if(CharIter >= CharPos)
			break;
		CharIter++;
		Iter = str_utf8_forward(pText, Iter);
	}
	
	Cursor.m_TextIter = Iter;
	
	return Cursor;
}

CTextRenderer::CTextCursor CTextRenderer::GetTextCursorFromTextIter(CTextCache* pTextCache, ivec2 TextPosition, int TextIter)
{
	UpdateTextCache(pTextCache);
	
	CTextCursor Cursor;
	Cursor.m_TextIter = TextIter;
	Cursor.m_Position.x = TextPosition.x + pTextCache->m_TextWidth;
	Cursor.m_Position.y = TextPosition.y;
	
	//Find the position of the character in the string
	int CharPos = 0;
	const char* pText = pTextCache->m_Text.buffer();
	int Iter = 0;
	while(pText[Iter])
	{
		if(Iter >= TextIter)
			break;
		CharPos++;
		Iter = str_utf8_forward(pText, Iter);
	}
	
	for(int i=0; i<pTextCache->m_Quads.size(); i++)
	{
		if(pTextCache->m_Quads[i].m_CharPos == CharPos)
		{
			Cursor.m_Position.x = TextPosition.x + pTextCache->m_Quads[i].m_AdvancePos.x;
			break;
		}
	}
	
	return Cursor;
}

float CTextRenderer::GetTextWidth(CTextCache* pTextCache)
{
	UpdateTextCache(pTextCache);
	
	return pTextCache->m_TextWidth;
}

void CTextRenderer::DrawText(CTextCache* pTextCache, ivec2 Position, vec4 Color)
{
	UpdateTextCache(pTextCache);
	
	Graphics()->TextureSet(m_GlyphCaches[pTextCache->m_GlyphCacheId]->m_Texture);
	Graphics()->QuadsBegin();
	for(int p=0; p<2; p++)
	{
		vec2 Offset = 0.0f;
		if(p==0)
		{
			Graphics()->SetColor(0.0f, 0.0f, 0.0f, 0.5f*Color.a);
			Offset = vec2(0.0f, 1.0f);
		}
		else
			Graphics()->SetColor(Color, true);
		
		for(int i=0; i<pTextCache->m_Quads.size(); i++)
		{
			Graphics()->QuadsSetSubset(
				pTextCache->m_Quads[i].m_UVMin.x,
				pTextCache->m_Quads[i].m_UVMin.y,
				pTextCache->m_Quads[i].m_UVMax.x,
				pTextCache->m_Quads[i].m_UVMax.y
			);
			CGraphics::CQuadItem QuadItem(
				Position.x + pTextCache->m_Quads[i].m_QuadPos.x + Offset.x,
				Position.y + pTextCache->m_Quads[i].m_QuadPos.y + Offset.y,
				pTextCache->m_Quads[i].m_Size.x,
				pTextCache->m_Quads[i].m_Size.y
			);
			Graphics()->QuadsDrawTL(&QuadItem, 1);
		}
	}
	Graphics()->QuadsEnd();
}

bool CTextRenderer::PreUpdate()
{
	m_RenderTick++;
	
	for(int i=0; i<m_GlyphCaches.size(); i++)
		m_GlyphCaches[i]->m_RenderTick = m_RenderTick;
	
	return true;
}
