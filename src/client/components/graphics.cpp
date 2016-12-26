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

/*
 * Some parts of this file comes from other projects.
 * These parts are itendified in this file by the following block:
 * 
 * FOREIGN CODE BEGIN: ProjectName *************************************
 * 
 * FOREIGN CODE END: ProjectName ***************************************
 * 
 * If ProjectName is "TeeWorlds", then this part of the code follows the
 * TeeWorlds licence:
 *      (c) Magnus Auvinen. See LICENSE_TEEWORLDS in the root of the
 *      distribution for more information. If you are missing that file,
 *      acquire a complete release at teeworlds.com.
 */

#include <shared/system/memory.h>
#include <shared/system/fs.h>
#include <shared/components/storage.h>
#include <shared/components/cli.h>
#include <shared/components/assetsmanager.h>
#include "graphics_backend_sdl.h"

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

#include "graphics.h"

static CVideoMode g_aFakeModes[] = {
	{320,200,8,8,8}, {320,240,8,8,8}, {400,300,8,8,8},
	{512,384,8,8,8}, {640,400,8,8,8}, {640,480,8,8,8},
	{720,400,8,8,8}, {768,576,8,8,8}, {800,600,8,8,8},
	{1024,600,8,8,8}, {1024,768,8,8,8}, {1152,864,8,8,8},
	{1280,600,8,8,8}, {1280,720,8,8,8}, {1280,768,8,8,8},
	{1280,800,8,8,8}, {1280,960,8,8,8}, {1280,1024,8,8,8},
	{1360,768,8,8,8}, {1366,768,8,8,8}, {1368,768,8,8,8},
	{1400,1050,8,8,8}, {1440,900,8,8,8}, {1440,1050,8,8,8},
	{1600,900,8,8,8}, {1600,1000,8,8,8}, {1600,1200,8,8,8},
	{1680,1050,8,8,8}, {1792,1344,8,8,8}, {1800,1440,8,8,8},
	{1856,1392,8,8,8}, {1920,1080,8,8,8}, {1920,1200,8,8,8},
	{1920,1440,8,8,8}, {1920,2400,8,8,8}, {2048,1536,8,8,8}
};

void CGraphics::FlushVertices()
{
	if(m_NumVertices == 0)
		return;

	int NumVerts = m_NumVertices;
	m_NumVertices = 0;

	CCommandBuffer::SCommand_Render Cmd;
	Cmd.m_State = m_State;

	if(m_Drawing == DRAWING_QUADS)
	{
		Cmd.m_PrimType = CCommandBuffer::PRIMTYPE_QUADS;
		Cmd.m_PrimCount = NumVerts/4;
	}
	else if(m_Drawing == DRAWING_LINES)
	{
		Cmd.m_PrimType = CCommandBuffer::PRIMTYPE_LINES;
		Cmd.m_PrimCount = NumVerts/2;
	}
	else
		return;

	Cmd.m_pVertices = (CCommandBuffer::SVertex *)m_pCommandBuffer->AllocData(sizeof(CCommandBuffer::SVertex)*NumVerts);
	if(Cmd.m_pVertices == 0x0)
	{
		// kick command buffer and try again
		KickCommandBuffer();

		Cmd.m_pVertices = (CCommandBuffer::SVertex *)m_pCommandBuffer->AllocData(sizeof(CCommandBuffer::SVertex)*NumVerts);
		if(Cmd.m_pVertices == 0x0)
		{
			dbg_msg("graphics", "failed to allocate data for vertices");
			return;
		}
	}

	// check if we have enough free memory in the commandbuffer
	if(!m_pCommandBuffer->AddCommand(Cmd))
	{
		// kick command buffer and try again
		KickCommandBuffer();
		
		Cmd.m_pVertices = (CCommandBuffer::SVertex *)m_pCommandBuffer->AllocData(sizeof(CCommandBuffer::SVertex)*NumVerts);
		if(Cmd.m_pVertices == 0x0)
		{
			dbg_msg("graphics", "failed to allocate data for vertices");
			return;
		}

		if(!m_pCommandBuffer->AddCommand(Cmd))
		{
			dbg_msg("graphics", "failed to allocate memory for render command");
			return;
		}
	}

	mem_copy(Cmd.m_pVertices, m_aVertices, sizeof(CCommandBuffer::SVertex)*NumVerts);
}

void CGraphics::AddVertices(int Count)
{
	m_NumVertices += Count;
	if((m_NumVertices + Count) >= MAX_VERTICES)
		FlushVertices();
}

void CGraphics::Rotate4(const CCommandBuffer::SPoint &rCenter, CCommandBuffer::SVertex *pPoints)
{
	float c = cosf(m_Rotation);
	float s = sinf(m_Rotation);
	float x, y;
	int i;

	for(i = 0; i < 4; i++)
	{
		x = pPoints[i].m_Pos.x - rCenter.x;
		y = pPoints[i].m_Pos.y - rCenter.y;
		pPoints[i].m_Pos.x = x * c - y * s + rCenter.x;
		pPoints[i].m_Pos.y = x * s + y * c + rCenter.y;
	}
}

CGraphics::CGraphics(CClientKernel* pKernel) :
	CClientKernel::CComponent(pKernel)
{
	SetName("Graphics");
	
	m_State.m_ScreenTL.x = 0;
	m_State.m_ScreenTL.y = 0;
	m_State.m_ScreenBR.x = 0;
	m_State.m_ScreenBR.y = 0;
	m_State.m_ClipEnable = false;
	m_State.m_ClipX = 0;
	m_State.m_ClipY = 0;
	m_State.m_ClipW = 0;
	m_State.m_ClipH = 0;
	m_State.m_Texture = -1;
	m_State.m_BlendMode = CCommandBuffer::BLEND_NONE;
	m_State.m_WrapModeU = WRAP_REPEAT;
	m_State.m_WrapModeV = WRAP_REPEAT;

	m_CurrentCommandBuffer = 0;
	m_pCommandBuffer = 0x0;
	m_apCommandBuffers[0] = 0x0;
	m_apCommandBuffers[1] = 0x0;

	m_NumVertices = 0;

	m_ScreenWidth = -1;
	m_ScreenHeight = -1;

	m_Rotation = 0;
	m_Drawing = 0;

	m_TextureMemoryUsage = 0;

	m_RenderEnable = true;
	m_DoScreenshot = false;
}

void CGraphics::ClipEnable(int x, int y, int w, int h)
{
	if(x < 0)
		w += x;
	if(y < 0)
		h += y;

	x = clamp(x, 0, ScreenWidth());
	y = clamp(y, 0, ScreenHeight());
	w = clamp(w, 0, ScreenWidth()-x);
	h = clamp(h, 0, ScreenHeight()-y);

	m_State.m_ClipEnable = true;
	m_State.m_ClipX = x;
	m_State.m_ClipY = ScreenHeight()-(y+h);
	m_State.m_ClipW = w;
	m_State.m_ClipH = h;
}

void CGraphics::ClipDisable()
{
	m_State.m_ClipEnable = false;
}

void CGraphics::BlendNone()
{
	m_State.m_BlendMode = CCommandBuffer::BLEND_NONE;
}

void CGraphics::BlendNormal()
{
	m_State.m_BlendMode = CCommandBuffer::BLEND_ALPHA;
}

void CGraphics::BlendAdditive()
{
	m_State.m_BlendMode = CCommandBuffer::BLEND_ADDITIVE;
}

void CGraphics::WrapNormal()
{
	m_State.m_WrapModeU = CGraphics::WRAP_REPEAT;
	m_State.m_WrapModeV = CGraphics::WRAP_REPEAT;
}

void CGraphics::WrapClamp()
{
	m_State.m_WrapModeU = WRAP_CLAMP;
	m_State.m_WrapModeV = WRAP_CLAMP;
}

void CGraphics::WrapMode(int WrapU, int WrapV)
{
	m_State.m_WrapModeU = WrapU;
	m_State.m_WrapModeV = WrapV;
}

int CGraphics::MemoryUsage() const
{
	return m_pBackend->MemoryUsage();
}

void CGraphics::MapScreen(float TopLeftX, float TopLeftY, float BottomRightX, float BottomRightY)
{
	m_State.m_ScreenTL.x = TopLeftX;
	m_State.m_ScreenTL.y = TopLeftY;
	m_State.m_ScreenBR.x = BottomRightX;
	m_State.m_ScreenBR.y = BottomRightY;
}

void CGraphics::GetScreen(float *pTopLeftX, float *pTopLeftY, float *pBottomRightX, float *pBottomRightY)
{
	*pTopLeftX = m_State.m_ScreenTL.x;
	*pTopLeftY = m_State.m_ScreenTL.y;
	*pBottomRightX = m_State.m_ScreenBR.x;
	*pBottomRightY = m_State.m_ScreenBR.y;
}

void CGraphics::LinesBegin()
{
	assert(m_Drawing == 0);
	m_Drawing = DRAWING_LINES;
	SetColor(1,1,1,1);
}

void CGraphics::LinesEnd()
{
	assert(m_Drawing == DRAWING_LINES);
	FlushVertices();
	m_Drawing = 0;
}

void CGraphics::LinesDraw(const CLineItem *pArray, int Num)
{
	assert(m_Drawing == DRAWING_LINES);

	for(int i = 0; i < Num; ++i)
	{
		m_aVertices[m_NumVertices + 2*i].m_Pos.x = pArray[i].m_X0;
		m_aVertices[m_NumVertices + 2*i].m_Pos.y = pArray[i].m_Y0;
		m_aVertices[m_NumVertices + 2*i].m_Tex = m_aTexture[0];
		m_aVertices[m_NumVertices + 2*i].m_Color = m_aColor[0];

		m_aVertices[m_NumVertices + 2*i + 1].m_Pos.x = pArray[i].m_X1;
		m_aVertices[m_NumVertices + 2*i + 1].m_Pos.y = pArray[i].m_Y1;
		m_aVertices[m_NumVertices + 2*i + 1].m_Tex = m_aTexture[1];
		m_aVertices[m_NumVertices + 2*i + 1].m_Color = m_aColor[1];
	}

	AddVertices(2*Num);
}

int CGraphics::UnloadTexture(CTextureHandle *Index)
{
	if(Index->Id() == m_InvalidTexture.Id())
		return 0;

	if(!Index->IsValid())
		return 0;

	CCommandBuffer::SCommand_Texture_Destroy Cmd;
	Cmd.m_Slot = Index->Id();
	m_pCommandBuffer->AddCommand(Cmd);

	m_aTextureIndices[Index->Id()] = m_FirstFreeTexture;
	m_FirstFreeTexture = Index->Id();

	Index->Invalidate();
	return 0;
}

static int ImageFormatToTexFormat(int Format)
{
	if(Format == CImageInfo::FORMAT_RGB) return CCommandBuffer::TEXFORMAT_RGB;
	if(Format == CImageInfo::FORMAT_RGBA) return CCommandBuffer::TEXFORMAT_RGBA;
	if(Format == CImageInfo::FORMAT_ALPHA) return CCommandBuffer::TEXFORMAT_ALPHA;
	return CCommandBuffer::TEXFORMAT_RGBA;
}

static int ImageFormatToPixelSize(int Format)
{
	switch(Format)
	{
	case CImageInfo::FORMAT_RGB: return 3;
	case CImageInfo::FORMAT_ALPHA: return 1;
	default: return 4;
	}
}


int CGraphics::LoadTextureRawSub(CTextureHandle TextureID, int x, int y, int Width, int Height, int Format, const void *pData)
{
	CCommandBuffer::SCommand_Texture_Update Cmd;
	Cmd.m_Slot = TextureID.Id();
	Cmd.m_X = x;
	Cmd.m_Y = y;
	Cmd.m_Width = Width;
	Cmd.m_Height = Height;
	Cmd.m_Format = ImageFormatToTexFormat(Format);

	// calculate memory usage
	int MemSize = Width*Height*ImageFormatToPixelSize(Format);

	// copy texture data
	unsigned char* pTmpData = new unsigned char[MemSize];
	mem_copy(pTmpData, pData, MemSize);
	Cmd.m_pData = pTmpData;

	//
	m_pCommandBuffer->AddCommand(Cmd);
	return 0;
}

CTextureHandle CGraphics::LoadTextureRaw(int Width, int Height, int GridWidth, int GridHeight, int Format, const void *pData, int StoreFormat, int Flags)
{
	// grab texture
	int Tex = m_FirstFreeTexture;
	m_FirstFreeTexture = m_aTextureIndices[Tex];
	m_aTextureIndices[Tex] = -1;

	CCommandBuffer::SCommand_Texture_Create Cmd;
	Cmd.m_Slot = Tex;
	Cmd.m_Width = Width;
	Cmd.m_Height = Height;
	Cmd.m_GridWidth = GridWidth;
	Cmd.m_GridHeight = GridHeight;
	Cmd.m_PixelSize = ImageFormatToPixelSize(Format);
	Cmd.m_Format = ImageFormatToTexFormat(Format);
	Cmd.m_StoreFormat = ImageFormatToTexFormat(StoreFormat);


	// flags
	Cmd.m_Flags = CCommandBuffer::TEXFLAG_TEXTURE2D;
	if(Flags&CGraphics::TEXLOAD_NOMIPMAPS)
		Cmd.m_Flags |= CCommandBuffer::TEXFLAG_NOMIPMAPS;
	if(m_Cfg_TextureCompression)
		Cmd.m_Flags |= CCommandBuffer::TEXFLAG_COMPRESSED;
	if(m_Cfg_TextureQuality || Flags&TEXLOAD_NORESAMPLE)
		Cmd.m_Flags |= CCommandBuffer::TEXFLAG_QUALITY;
	if(Flags&CGraphics::TEXLOAD_ARRAY_256)
	{
		Cmd.m_Flags |= CCommandBuffer::TEXFLAG_TEXTURE3D;
		Cmd.m_Flags &= ~CCommandBuffer::TEXFLAG_TEXTURE2D;
	}
	if(Flags&CGraphics::TEXLOAD_MULTI_DIMENSION)
		Cmd.m_Flags |= CCommandBuffer::TEXFLAG_TEXTURE3D;

	
	// copy texture data
	int MemSize = Width*Height*Cmd.m_PixelSize;
	unsigned char* pTmpData = new unsigned char[MemSize];
	mem_copy(pTmpData, pData, MemSize);
	Cmd.m_pData = pTmpData;
	

	//
	m_pCommandBuffer->AddCommand(Cmd);

	return CreateTextureHandle(Tex);
}

/*
// simple uncompressed RGBA loaders
CTextureHandle CGraphics::LoadTexture(const char *pFilename, int StorageType, int StoreFormat, int Flags)
{
	int l = str_length(pFilename);
	CTextureHandle ID;
	CImageInfo Img;

	if(l < 3)
		return CTextureHandle();
	if(LoadPNG(&Img, pFilename, StorageType))
	{
		if (StoreFormat == CImageInfo::FORMAT_AUTO)
			StoreFormat = Img.m_Format;

		int GridWidth = 1;
		int GridHeight = 1;
		if(Flags&CGraphics::TEXLOAD_MULTI_DIMENSION || Flags&CGraphics::TEXLOAD_ARRAY_256)
		{
			GridWidth = 16;
			GridHeight = 16;
		}

		ID = LoadTextureRaw(Img.m_Width, Img.m_Height, GridWidth, GridHeight, Img.m_Format, Img.m_pData, StoreFormat, Flags);
		delete[] Img.m_pData;
		
		return ID;
	}

	return m_InvalidTexture;
}

int CGraphics::LoadPNG(CImageInfo *pImg, const char *pFilename, int StorageType)
{
	char aCompleteFilename[512];
	unsigned char *pBuffer;
	png_t Png; // ignore_convention

	// open file for reading
	png_init(0,0); // ignore_convention

	IOHANDLE File = m_pStorage->OpenFile(pFilename, IOFLAG_READ, StorageType, aCompleteFilename, sizeof(aCompleteFilename));
	if(File)
		io_close(File);
	else
	{
		dbg_msg("game/png", "failed to open file. filename='%s'", pFilename);
		return 0;
	}

	int Error = png_open_file(&Png, aCompleteFilename); // ignore_convention
	if(Error != PNG_NO_ERROR)
	{
		dbg_msg("game/png", "failed to open file. filename='%s'", aCompleteFilename);
		if(Error != PNG_FILE_ERROR)
			png_close_file(&Png); // ignore_convention
		return 0;
	}

	if(Png.depth != 8 || (Png.color_type != PNG_TRUECOLOR && Png.color_type != PNG_TRUECOLOR_ALPHA) || Png.width > (2<<12) || Png.height > (2<<12)) // ignore_convention
	{
		dbg_msg("game/png", "invalid format. filename='%s'", aCompleteFilename);
		png_close_file(&Png); // ignore_convention
		return 0;
	}

	pBuffer = (unsigned char *)mem_alloc(Png.width * Png.height * Png.bpp, 1); // ignore_convention
	png_get_data(&Png, pBuffer); // ignore_convention
	png_close_file(&Png); // ignore_convention

	pImg->m_Width = Png.width; // ignore_convention
	pImg->m_Height = Png.height; // ignore_convention
	if(Png.color_type == PNG_TRUECOLOR) // ignore_convention
		pImg->m_Format = CImageInfo::FORMAT_RGB;
	else if(Png.color_type == PNG_TRUECOLOR_ALPHA) // ignore_convention
		pImg->m_Format = CImageInfo::FORMAT_RGBA;
	pImg->m_pData = pBuffer;
	return 1;
}
*/

void CGraphics::KickCommandBuffer()
{
	m_pBackend->RunBuffer(m_pCommandBuffer);

	// swap buffer
	m_CurrentCommandBuffer ^= 1;
	m_pCommandBuffer = m_apCommandBuffers[m_CurrentCommandBuffer];
	m_pCommandBuffer->Reset();
}

void CGraphics::ScreenshotDirect(const char *pFilename)
{
	// add swap command
	CImageInfo Image;
	mem_zero(&Image, sizeof(Image));

	CCommandBuffer::SCommand_Screenshot Cmd;
	Cmd.m_pImage = &Image;
	Cmd.m_X = 0; Cmd.m_Y = 0;
	Cmd.m_W = -1; Cmd.m_H = -1;
	m_pCommandBuffer->AddCommand(Cmd);

	// kick the buffer and wait for the result
	KickCommandBuffer();
	WaitForIdle();

	if(Image.m_pData)
	{
		// find filename
		dynamic_string FullPath;
		png_t Png; // ignore_convention

		IOHANDLE File = m_pStorage->OpenFile(pFilename, IOFLAG_WRITE, CStorage::TYPE_SAVE, FullPath);
		if(File)
			io_close(File);

		// save png
		dbg_msg("Graphics", "saved screenshot to '%s'", FullPath.buffer());
		
		png_open_file_write(&Png, FullPath.buffer()); // ignore_convention
		png_set_data(&Png, Image.m_Width, Image.m_Height, 8, PNG_TRUECOLOR, (unsigned char *)Image.m_pData); // ignore_convention
		png_close_file(&Png); // ignore_convention

		delete[] Image.m_pData;
	}
}

void CGraphics::TextureSet(CTextureHandle TextureID)
{
	assert(m_Drawing == 0);
	m_State.m_Texture = TextureID.Id();
	m_State.m_Dimension = 2;
}

void CGraphics::Clear(float r, float g, float b)
{
	CCommandBuffer::SCommand_Clear Cmd;
	Cmd.m_Color.r = r;
	Cmd.m_Color.g = g;
	Cmd.m_Color.b = b;
	Cmd.m_Color.a = 0;
	m_pCommandBuffer->AddCommand(Cmd);
}

void CGraphics::QuadsBegin()
{
	assert(m_Drawing == 0);
	m_Drawing = DRAWING_QUADS;

	QuadsSetSubset(0,0,1,1,-1);
	QuadsSetRotation(0);
	SetColor(1,1,1,1);
}

void CGraphics::QuadsEnd()
{
	assert(m_Drawing == DRAWING_QUADS);
	FlushVertices();
	m_Drawing = 0;
}

void CGraphics::QuadsSetRotation(float Angle)
{
	assert(m_Drawing == DRAWING_QUADS);
	m_Rotation = Angle;
}

void CGraphics::SetColorVertex(const CColorVertex *pArray, int Num)
{
	assert(m_Drawing != 0);

	for(int i = 0; i < Num; ++i)
	{
		m_aColor[pArray[i].m_Index].r = pArray[i].m_R;
		m_aColor[pArray[i].m_Index].g = pArray[i].m_G;
		m_aColor[pArray[i].m_Index].b = pArray[i].m_B;
		m_aColor[pArray[i].m_Index].a = pArray[i].m_A;
	}
}

void CGraphics::SetColor(float r, float g, float b, float a)
{
	assert(m_Drawing != 0);
	CColorVertex Array[4] = {
		CColorVertex(0, r, g, b, a),
		CColorVertex(1, r, g, b, a),
		CColorVertex(2, r, g, b, a),
		CColorVertex(3, r, g, b, a)};
	SetColorVertex(Array, 4);
}

void CGraphics::SetColor(vec4 rgba, bool AlphaBlend)
{
	assert(m_Drawing != 0);
	if(AlphaBlend)
	{
		CColorVertex Array[4] = {
			CColorVertex(0, rgba.r*rgba.a, rgba.g*rgba.a, rgba.b*rgba.a, rgba.a),
			CColorVertex(1, rgba.r*rgba.a, rgba.g*rgba.a, rgba.b*rgba.a, rgba.a),
			CColorVertex(2, rgba.r*rgba.a, rgba.g*rgba.a, rgba.b*rgba.a, rgba.a),
			CColorVertex(3, rgba.r*rgba.a, rgba.g*rgba.a, rgba.b*rgba.a, rgba.a)};
		SetColorVertex(Array, 4);
	}
	else
	{
		CColorVertex Array[4] = {
			CColorVertex(0, rgba.r, rgba.g, rgba.b, rgba.a),
			CColorVertex(1, rgba.r, rgba.g, rgba.b, rgba.a),
			CColorVertex(2, rgba.r, rgba.g, rgba.b, rgba.a),
			CColorVertex(3, rgba.r, rgba.g, rgba.b, rgba.a)};
		SetColorVertex(Array, 4);
	}
}

void CGraphics::SetColor2(vec4 Color0, vec4 Color1, bool AlphaBlend)
{
	assert(m_Drawing != 0);
	if(AlphaBlend)
	{
		CColorVertex Array[2] = {
			CColorVertex(0, Color0.r*Color0.a, Color0.g*Color0.a, Color0.b*Color0.a, Color0.a),
			CColorVertex(1, Color1.r*Color1.a, Color1.g*Color1.a, Color1.b*Color1.a, Color1.a)
		};
		SetColorVertex(Array, 2);
	}
	else
	{
		CColorVertex Array[2] = {
			CColorVertex(0, Color0.r, Color0.g, Color0.b, Color0.a),
			CColorVertex(1, Color1.r, Color1.g, Color1.b, Color1.a)
		};
		SetColorVertex(Array, 2);
	}
}

void CGraphics::SetColor4(vec4 C0, vec4 C1, vec4 C2, vec4 C3, bool AlphaBlend)
{
	assert(m_Drawing != 0);
	if(AlphaBlend)
	{
		CColorVertex Array[4] = {
			CColorVertex(0, C0.r*C0.a, C0.g*C0.a, C0.b*C0.a, C0.a),
			CColorVertex(1, C1.r*C1.a, C1.g*C1.a, C1.b*C1.a, C1.a),
			CColorVertex(2, C2.r*C2.a, C2.g*C2.a, C2.b*C2.a, C2.a),
			CColorVertex(3, C3.r*C3.a, C3.g*C3.a, C3.b*C3.a, C3.a)};
		SetColorVertex(Array, 4);
	}
	else
	{
		CColorVertex Array[4] = {
			CColorVertex(0, C0.r, C0.g, C0.b, C0.a),
			CColorVertex(1, C1.r, C1.g, C1.b, C1.a),
			CColorVertex(2, C2.r, C2.g, C2.b, C2.a),
			CColorVertex(3, C3.r, C3.g, C3.b, C3.a)};
		SetColorVertex(Array, 4);
	}
}

void CGraphics::SetColor4(vec4 C0, vec4 C1, vec4 C2, vec4 C3)
{
	assert(m_Drawing != 0);
	CColorVertex Array[4] = {
		CColorVertex(0, C0.r, C0.g, C0.b, C0.a),
		CColorVertex(1, C1.r, C1.g, C1.b, C1.a),
		CColorVertex(2, C2.r, C2.g, C2.b, C2.a),
		CColorVertex(3, C3.r, C3.g, C3.b, C3.a)};
	SetColorVertex(Array, 4);
}

void CGraphics::QuadsSetSubset(float TlU, float TlV, float BrU, float BrV, int TextureIndex)
{
	assert(m_Drawing == DRAWING_QUADS);

	m_aTexture[0].u = TlU;	m_aTexture[1].u = BrU;
	m_aTexture[0].v = TlV;	m_aTexture[1].v = TlV;

	m_aTexture[3].u = TlU;	m_aTexture[2].u = BrU;
	m_aTexture[3].v = BrV;	m_aTexture[2].v = BrV;

	m_aTexture[0].i = m_aTexture[1].i = m_aTexture[2].i = m_aTexture[3].i = (0.5f + TextureIndex) / 256.0f;
	m_State.m_Dimension = (TextureIndex < 0) ? 2 : 3;
}

void CGraphics::QuadsSetSubsetFree(
	float x0, float y0, float x1, float y1,
	float x2, float y2, float x3, float y3, int TextureIndex)
{
	m_aTexture[0].u = x0; m_aTexture[0].v = y0;
	m_aTexture[1].u = x1; m_aTexture[1].v = y1;
	m_aTexture[2].u = x2; m_aTexture[2].v = y2;
	m_aTexture[3].u = x3; m_aTexture[3].v = y3;

	m_aTexture[0].i = m_aTexture[1].i = m_aTexture[2].i = m_aTexture[3].i = (0.5f + TextureIndex) / 256.0f;
	m_State.m_Dimension = (TextureIndex < 0) ? 2 : 3;
}

void CGraphics::QuadsDraw(CQuadItem *pArray, int Num)
{
	for(int i = 0; i < Num; ++i)
	{
		pArray[i].m_X -= pArray[i].m_Width/2;
		pArray[i].m_Y -= pArray[i].m_Height/2;
	}

	QuadsDrawTL(pArray, Num);
}

void CGraphics::QuadsDrawTL(const CQuadItem *pArray, int Num)
{
	CCommandBuffer::SPoint Center;
	Center.z = 0;

	assert(m_Drawing == DRAWING_QUADS);

	for(int i = 0; i < Num; ++i)
	{
		m_aVertices[m_NumVertices + 4*i].m_Pos.x = pArray[i].m_X;
		m_aVertices[m_NumVertices + 4*i].m_Pos.y = pArray[i].m_Y;
		m_aVertices[m_NumVertices + 4*i].m_Tex = m_aTexture[0];
		m_aVertices[m_NumVertices + 4*i].m_Color = m_aColor[0];

		m_aVertices[m_NumVertices + 4*i + 1].m_Pos.x = pArray[i].m_X + pArray[i].m_Width;
		m_aVertices[m_NumVertices + 4*i + 1].m_Pos.y = pArray[i].m_Y;
		m_aVertices[m_NumVertices + 4*i + 1].m_Tex = m_aTexture[1];
		m_aVertices[m_NumVertices + 4*i + 1].m_Color = m_aColor[1];

		m_aVertices[m_NumVertices + 4*i + 2].m_Pos.x = pArray[i].m_X + pArray[i].m_Width;
		m_aVertices[m_NumVertices + 4*i + 2].m_Pos.y = pArray[i].m_Y + pArray[i].m_Height;
		m_aVertices[m_NumVertices + 4*i + 2].m_Tex = m_aTexture[2];
		m_aVertices[m_NumVertices + 4*i + 2].m_Color = m_aColor[2];

		m_aVertices[m_NumVertices + 4*i + 3].m_Pos.x = pArray[i].m_X;
		m_aVertices[m_NumVertices + 4*i + 3].m_Pos.y = pArray[i].m_Y + pArray[i].m_Height;
		m_aVertices[m_NumVertices + 4*i + 3].m_Tex = m_aTexture[3];
		m_aVertices[m_NumVertices + 4*i + 3].m_Color = m_aColor[3];

		if(m_Rotation != 0)
		{
			Center.x = pArray[i].m_X + pArray[i].m_Width/2;
			Center.y = pArray[i].m_Y + pArray[i].m_Height/2;

			Rotate4(Center, &m_aVertices[m_NumVertices + 4*i]);
		}
	}

	AddVertices(4*Num);
}

void CGraphics::QuadsDrawFreeform(const CFreeformItem *pArray, int Num)
{
	assert(m_Drawing == DRAWING_QUADS);

	for(int i = 0; i < Num; ++i)
	{
		m_aVertices[m_NumVertices + 4*i].m_Pos.x = pArray[i].m_X0;
		m_aVertices[m_NumVertices + 4*i].m_Pos.y = pArray[i].m_Y0;
		m_aVertices[m_NumVertices + 4*i].m_Tex = m_aTexture[0];
		m_aVertices[m_NumVertices + 4*i].m_Color = m_aColor[0];

		m_aVertices[m_NumVertices + 4*i + 1].m_Pos.x = pArray[i].m_X1;
		m_aVertices[m_NumVertices + 4*i + 1].m_Pos.y = pArray[i].m_Y1;
		m_aVertices[m_NumVertices + 4*i + 1].m_Tex = m_aTexture[1];
		m_aVertices[m_NumVertices + 4*i + 1].m_Color = m_aColor[1];

		m_aVertices[m_NumVertices + 4*i + 2].m_Pos.x = pArray[i].m_X3;
		m_aVertices[m_NumVertices + 4*i + 2].m_Pos.y = pArray[i].m_Y3;
		m_aVertices[m_NumVertices + 4*i + 2].m_Tex = m_aTexture[3];
		m_aVertices[m_NumVertices + 4*i + 2].m_Color = m_aColor[3];

		m_aVertices[m_NumVertices + 4*i + 3].m_Pos.x = pArray[i].m_X2;
		m_aVertices[m_NumVertices + 4*i + 3].m_Pos.y = pArray[i].m_Y2;
		m_aVertices[m_NumVertices + 4*i + 3].m_Tex = m_aTexture[2];
		m_aVertices[m_NumVertices + 4*i + 3].m_Color = m_aColor[2];
	}

	AddVertices(4*Num);
}

void CGraphics::QuadsText(float x, float y, float Size, const char *pText)
{
	float StartX = x;

	while(*pText)
	{
		char c = *pText;
		pText++;

		if(c == '\n')
		{
			x = StartX;
			y += Size;
		}
		else
		{
			QuadsSetSubset(
				(c%16)/16.0f,
				(c/16)/16.0f,
				(c%16)/16.0f+1.0f/16.0f,
				(c/16)/16.0f+1.0f/16.0f);

			CQuadItem QuadItem(x, y, Size, Size);
			QuadsDrawTL(&QuadItem, 1);
			x += Size/2;
		}
	}
}

int CGraphics::IssueInit()
{
	int Flags = 0x0;
	if(m_Cfg_Borderless) Flags |= CGraphicsBackend_SDL::INITFLAG_BORDERLESS;
	if(m_Cfg_Fullscreen) Flags |= CGraphicsBackend_SDL::INITFLAG_FULLSCREEN;
	if(m_Cfg_Vsync) Flags |= CGraphicsBackend_SDL::INITFLAG_VSYNC;
	if(m_Cfg_Resizable) Flags |= CGraphicsBackend_SDL::INITFLAG_RESIZABLE;

	return m_pBackend->Init(
		"TeeUniverse",
		&m_Cfg_Screen,
		&m_Cfg_ScreenWidth,
		&m_Cfg_ScreenHeight,
		m_Cfg_FsaaSamples,
		Flags,
		&m_DesktopWidth,
		&m_DesktopHeight
	);
}

int CGraphics::InitWindow()
{
	if(IssueInit() == 0)
		return 0;

	// try disabling fsaa
	while(m_Cfg_FsaaSamples)
	{
		m_Cfg_FsaaSamples--;

		if(m_Cfg_FsaaSamples)
			dbg_msg("gfx", "lowering FSAA to %d and trying again", m_Cfg_FsaaSamples);
		else
			dbg_msg("gfx", "disabling FSAA and trying again");

		if(IssueInit() == 0)
			return 0;
	}

	// try lowering the resolution
	if(m_Cfg_ScreenWidth != 640 || m_Cfg_ScreenHeight != 480)
	{
		dbg_msg("gfx", "setting resolution to 640x480 and trying again");
		m_Cfg_ScreenWidth = 640;
		m_Cfg_ScreenHeight = 480;

		if(IssueInit() == 0)
			return 0;
	}

	dbg_msg("gfx", "out of ideas. failed to init graphics");

	return -1;
}

bool CGraphics::InitConfig(int argc, const char** argv)
{
	//Register commands
	m_Cfg_Screen = 0;
	m_Cfg_ScreenWidth = 0;
	m_Cfg_ScreenHeight = 0;
	m_Cfg_Borderless = 0;
	m_Cfg_Fullscreen = 1;
	m_Cfg_Alphabits = 0;
	m_Cfg_Clear = 1;
	m_Cfg_Vsync = 1;
	m_Cfg_Resizable = 1;
	m_Cfg_DisplayAllModes = 0;
	m_Cfg_TextureCompression = 0;
	m_Cfg_HighDetail = 1;
	m_Cfg_TextureQuality = 1;
	m_Cfg_FsaaSamples = 0;
	m_Cfg_RefreshRate = 0;
	m_Cfg_Finish = 1;
	m_Cfg_AsyncRender = 0;
	m_Cfg_DbgTexture = -1;
	
	CLI()->RegisterConfigInteger("gfx_screen", "Screen index", &m_Cfg_Screen, 0, 4);
	CLI()->RegisterConfigInteger("gfx_screen_width", "Screen resolution width", &m_Cfg_ScreenWidth, 0, 4096*4);
	CLI()->RegisterConfigInteger("gfx_screen_height", "Screen resolution height", &m_Cfg_ScreenHeight, 0, 4096*4);
	CLI()->RegisterConfigInteger("gfx_borderless", "Borderless window (not to be used with fullscreen)", &m_Cfg_Borderless, 0, 1);
	CLI()->RegisterConfigInteger("gfx_fullscreen", "Fullscreen", &m_Cfg_Fullscreen, 0, 1);
	CLI()->RegisterConfigInteger("gfx_alphabits", "Alpha bits for framebuffer (fullscreen only)", &m_Cfg_Alphabits, 0, 1);
	CLI()->RegisterConfigInteger("gfx_clear", "Clear screen before rendering", &m_Cfg_Clear, 0, 1);
	CLI()->RegisterConfigInteger("gfx_vsync", "Vertical sync", &m_Cfg_Vsync, 0, 1);
	CLI()->RegisterConfigInteger("gfx_display_all_modes", "", &m_Cfg_DisplayAllModes, 0, 1);
	CLI()->RegisterConfigInteger("gfx_texture_compression", "Use texture compression", &m_Cfg_TextureCompression, 0, 1);
	CLI()->RegisterConfigInteger("gfx_high_detail", "High detail", &m_Cfg_HighDetail, 0, 1);
	CLI()->RegisterConfigInteger("gfx_texture_quality", "", &m_Cfg_TextureQuality, 0, 1);
	CLI()->RegisterConfigInteger("gfx_fsaa_samples", "FSAA Samples", &m_Cfg_FsaaSamples, 0, 16);
	CLI()->RegisterConfigInteger("gfx_refresh_rate", "Screen refresh rate", &m_Cfg_RefreshRate, 0, 999999);
	CLI()->RegisterConfigInteger("gfx_finish", "", &m_Cfg_Finish, 0, 1);
	CLI()->RegisterConfigInteger("gfx_asyncrender", "Do rendering async from the the update", &m_Cfg_AsyncRender, 0, 1);
	CLI()->RegisterConfigInteger("gfx_dbg_texture", "Show debug information for a given texture", &m_Cfg_DbgTexture, -1, 9999);
	
	return true;
}

void CGraphics::SaveConfig(class CCLI_Output* pOutput)
{
	fixed_string128 Buffer;
	
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_screen %d", m_Cfg_Screen); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_screen_width %d", m_Cfg_ScreenWidth); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_screen_height %d", m_Cfg_ScreenHeight); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_borderless %d", m_Cfg_Borderless); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_fullscreen %d", m_Cfg_Fullscreen); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_alphabits %d", m_Cfg_Alphabits); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_clear %d", m_Cfg_Clear); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_vsync %d", m_Cfg_Vsync); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_display_all_modes %d", m_Cfg_DisplayAllModes); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_texture_compression %d", m_Cfg_TextureCompression); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_high_detail %d", m_Cfg_HighDetail); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_texture_quality %d", m_Cfg_TextureQuality); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_fsaa_samples %d", m_Cfg_FsaaSamples); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_refresh_rate %d", m_Cfg_RefreshRate); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_finish %d", m_Cfg_Finish); pOutput->Print(Buffer.buffer());
	str_format(Buffer.buffer(), Buffer.maxsize(), "gfx_asyncrender %d", m_Cfg_AsyncRender); pOutput->Print(Buffer.buffer());
}

bool CGraphics::Init()
{	
	m_RenderFrameTime = 0.0001f;
	m_RenderFrameTimeLow = 1.0f;
	m_RenderFrameTimeHigh = 0.0f;
	m_RenderFrames = 0;
	m_LastRenderTime = time_get();
	
	// Set all z to -5.0f
	for(int i = 0; i < MAX_VERTICES; i++)
		m_aVertices[i].m_Pos.z = -5.0f;

	// init textures
	m_FirstFreeTexture = 0;
	for(int i = 0; i < MAX_TEXTURES-1; i++)
		m_aTextureIndices[i] = i+1;
	m_aTextureIndices[MAX_TEXTURES-1] = -1;

	m_pBackend = new CGraphicsBackend_SDL();
	if(InitWindow() != 0)
		return false;

	// fetch final resolusion
	m_ScreenWidth = m_Cfg_ScreenWidth;
	m_ScreenHeight = m_Cfg_ScreenHeight;

	// create command buffers
	for(int i = 0; i < NUM_CMDBUFFERS; i++)
		m_apCommandBuffers[i] = new CCommandBuffer(128*1024, 2*1024*1024);
	m_pCommandBuffer = m_apCommandBuffers[0];

	// create null texture, will get id=0
	unsigned char aNullTextureData[4*32*32];
	for(int x = 0; x < 32; ++x)
		for(int y = 0; y < 32; ++y)
		{
			if(x < 16)
			{
				if(y < 16)
				{
					aNullTextureData[4*(y*32+x)+0] = y*8+x*8+15;
					aNullTextureData[4*(y*32+x)+1] = 0;
					aNullTextureData[4*(y*32+x)+2] = 0;
				}
				else
				{
					aNullTextureData[4*(y*32+x)+0] = 0;
					aNullTextureData[4*(y*32+x)+1] = y*8+x*8-113;
					aNullTextureData[4*(y*32+x)+2] = 0;
				}
			}
			else
			{
				if(y < 16)
				{
					aNullTextureData[4*(y*32+x)+0] = 0;
					aNullTextureData[4*(y*32+x)+1] = 0;
					aNullTextureData[4*(y*32+x)+2] = y*8+x*8-113;
				}
				else
				{
					aNullTextureData[4*(y*32+x)+0] = y*8+x*8-496;
					aNullTextureData[4*(y*32+x)+1] = y*8+x*8-496;
					aNullTextureData[4*(y*32+x)+2] = 0;
				}
			}
			aNullTextureData[4*(y*32+x)+3] = 255;
		}

	m_InvalidTexture = LoadTextureRaw(32,32,1,1,CImageInfo::FORMAT_RGBA,aNullTextureData,CImageInfo::FORMAT_RGBA,TEXLOAD_NORESAMPLE|TEXLOAD_MULTI_DIMENSION);
	return true;
}

bool CGraphics::PreUpdate()
{
	//Update images
	const array<CAssetsManager::CAssetUpdateDesc>& ImagesToUpdate = AssetsManager()->GetImagesToUpdate();
	for(int i=0; i<ImagesToUpdate.size(); i++)
	{
		if(ImagesToUpdate[i].m_Updated)
			continue;
		
		CAsset_Image* pImage = AssetsManager()->GetAsset_Hard<CAsset_Image>(ImagesToUpdate[i].m_Path);
		if(!pImage)
			continue;
		
		CTextureHandle Texture = pImage->GetTexture();
		
		if(Texture.IsValid())
			UnloadTexture(&Texture);
		
		int Flags = 0x0;
		if(pImage->GetTilingEnabled() && pImage->GetGridWidth() == 16 && pImage->GetGridHeight() == 16)
			Flags = CGraphics::TEXLOAD_MULTI_DIMENSION;
		
		const array2d<uint8>& Data = pImage->GetDataArray();
		
		int Format = CImageInfo::FORMAT_AUTO;
		if(Data.get_depth() == 1)
			Format = CImageInfo::FORMAT_ALPHA;
		else if(Data.get_depth() == 3)
			Format = CImageInfo::FORMAT_RGB;
		else if(Data.get_depth() == 4)
			Format = CImageInfo::FORMAT_RGBA;
		
		Texture = LoadTextureRaw(
			Data.get_width(),
			Data.get_height(),
			pImage->GetGridWidth(),
			pImage->GetGridHeight(),
			Format,
			Data.base_ptr(),
			CImageInfo::FORMAT_AUTO,
			Flags
		);
		
		pImage->SetTexture(Texture);

		AssetsManager()->MarkUpdated(i);
	}
	
	m_IsReadyToRender = false;
	if(!m_Cfg_AsyncRender || IsIdle())
	{		
		m_RenderFrames++;

		// update frametime
		int64 Now = time_get();
		m_RenderFrameTime = (Now - m_LastRenderTime) / (float)time_freq();
		
		if(m_RenderFrameTime < m_RenderFrameTimeLow)
			m_RenderFrameTimeLow = m_RenderFrameTime;
			
		if(m_RenderFrameTime > m_RenderFrameTimeHigh)
			m_RenderFrameTimeHigh = m_RenderFrameTime;

		m_LastRenderTime = Now;
		
		if(m_Cfg_Clear)
			Graphics()->Clear(0.5f, 0.5f, 0.5f);
		
		m_IsReadyToRender = true;
	}
	
	return true;
}

bool CGraphics::PostUpdate()
{
	if(ReadyToRender())
	{
		if(m_Cfg_DbgTexture >= 0)
		{
			CTextureHandle Texture;
			Texture.SetId(m_Cfg_DbgTexture);
			TextureSet(Texture);
			BlendNormal();
			QuadsBegin();
			SetColor(vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
			CQuadItem QuadItem(ScreenWidth()/2, ScreenHeight()/2, 512, 512);
			QuadsDraw(&QuadItem, 1);
			QuadsEnd();
		}
		
		Swap();
		m_IsReadyToRender = false;
	}
	
	return true;
}

void CGraphics::Shutdown()
{
	// shutdown the backend
	m_pBackend->Shutdown();
	delete m_pBackend;
	m_pBackend = 0x0;

	// delete the command buffers
	for(int i = 0; i < NUM_CMDBUFFERS; i++)
		delete m_apCommandBuffers[i];
}

int CGraphics::GetNumScreens() const
{
	return m_pBackend->GetNumScreens();
}

void CGraphics::Minimize()
{
	m_pBackend->Minimize();
}

void CGraphics::Maximize()
{
	m_pBackend->Maximize();
}

bool CGraphics::Fullscreen(bool State)
{
	if(m_pBackend->Fullscreen(State))
	{
		SetScreenSize(m_DesktopWidth, m_DesktopHeight);
		return true;
	}
	else
		return false;
}

void CGraphics::SetWindowBordered(bool State)
{
	m_pBackend->SetWindowBordered(State);
}

int CGraphics::SetWindowScreen(int Index)
{
	return m_pBackend->SetWindowScreen(Index);
}

int CGraphics::GetWindowScreen()
{
	return m_pBackend->GetWindowScreen();
}

int CGraphics::WindowActive()
{
	return m_pBackend->WindowActive();
}

int CGraphics::WindowOpen()
{
	return m_pBackend->WindowOpen();

}

void CGraphics::ReadBackbuffer(unsigned char **ppPixels, int x, int y, int w, int h)
{
	if(!ppPixels)
		return;

	// add swap command
	CImageInfo Image;
	mem_zero(&Image, sizeof(Image));

	CCommandBuffer::SCommand_Screenshot Cmd;
	Cmd.m_pImage = &Image;
	Cmd.m_X = x; Cmd.m_Y = y;
	Cmd.m_W = w; Cmd.m_H = h;
	m_pCommandBuffer->AddCommand(Cmd);

	// kick the buffer and wait for the result
	KickCommandBuffer();
	WaitForIdle();

	*ppPixels = (unsigned char *)Image.m_pData; // take ownership!
}

void CGraphics::TakeScreenshot(const char *pFilename)
{
	// TODO: screenshot support
	char aDate[20];
	str_timestamp(aDate, sizeof(aDate));
	str_format(m_aScreenshotName, sizeof(m_aScreenshotName), "screenshots/%s_%s.png", pFilename?pFilename:"screenshot", aDate);
	m_DoScreenshot = true;
}

void CGraphics::Swap()
{
	// TODO: screenshot support
	if(m_DoScreenshot)
	{
		if(WindowActive())
			ScreenshotDirect(m_aScreenshotName);
		m_DoScreenshot = false;
	}

	// add swap command
	CCommandBuffer::SCommand_Swap Cmd;
	Cmd.m_Finish = m_Cfg_Finish;
	m_pCommandBuffer->AddCommand(Cmd);

	// kick the command buffer
	KickCommandBuffer();
}

bool CGraphics::SetVSync(bool State)
{
	// add vsnc command
	bool RetOk = 0;
	CCommandBuffer::SCommand_VSync Cmd;
	Cmd.m_VSync = State ? 1 : 0;
	Cmd.m_pRetOk = &RetOk;
	m_pCommandBuffer->AddCommand(Cmd);

	// kick the command buffer
	KickCommandBuffer();
	WaitForIdle();
	return RetOk;
}

// syncronization
void CGraphics::InsertSignal(semaphore *pSemaphore)
{
	CCommandBuffer::SCommand_Signal Cmd;
	Cmd.m_pSemaphore = pSemaphore;
	m_pCommandBuffer->AddCommand(Cmd);
}

bool CGraphics::IsIdle() const
{
	return m_pBackend->IsIdle();
}

void CGraphics::WaitForIdle()
{
	m_pBackend->WaitForIdle();
}

int CGraphics::GetVideoModes(CVideoMode *pModes, int MaxModes, int Screen)
{
	if(m_Cfg_DisplayAllModes)
	{
		int Count = sizeof(g_aFakeModes)/sizeof(CVideoMode);
		mem_copy(pModes, g_aFakeModes, sizeof(g_aFakeModes));
		if(MaxModes < Count)
			Count = MaxModes;
		return Count;
	}

	// add videomodes command
	CImageInfo Image;
	mem_zero(&Image, sizeof(Image));

	int NumModes = 0;
	CCommandBuffer::SCommand_VideoModes Cmd;
	Cmd.m_pModes = pModes;
	Cmd.m_MaxModes = MaxModes;
	Cmd.m_pNumModes = &NumModes;
	Cmd.m_Screen = Screen;
	m_pCommandBuffer->AddCommand(Cmd);

	// kick the buffer and wait for the result and return it
	KickCommandBuffer();
	WaitForIdle();
	return NumModes;
}

/* FOREIGN CODE END: TeeWorlds ****************************************/

void CGraphics::ClipPush(int x, int y, int w, int h)
{
	CCommandBuffer::SClip& ClipRect = m_ClipStack.increment();
	if(m_ClipStack.size() > 1)
		ClipRect = m_ClipStack[m_ClipStack.size()-2];
	else
	{
		ClipRect.x = 0;
		ClipRect.y = 0;
		ClipRect.w = ScreenWidth();
		ClipRect.h = ScreenHeight();
	}
	
	int X = clamp(x, ClipRect.x, ClipRect.x + ClipRect.w);
	int Y = clamp(y, ClipRect.y, ClipRect.y + ClipRect.h);
	int W = clamp(x+w, X, ClipRect.x + ClipRect.w) - X;
	int H = clamp(y+h, Y, ClipRect.y + ClipRect.h) - Y;
	
	m_State.m_ClipEnable = true;
	m_State.m_ClipX = X;
	m_State.m_ClipY = ScreenHeight()-(Y+H);
	m_State.m_ClipW = W;
	m_State.m_ClipH = H;
	
	ClipRect.x = X;
	ClipRect.y = Y;
	ClipRect.w = W;
	ClipRect.h = H;
}

void CGraphics::ClipPop()
{
	if(m_ClipStack.size())
		m_ClipStack.remove_index(m_ClipStack.size()-1);
	
	if(m_ClipStack.size())
	{
		CCommandBuffer::SClip ClipRect = m_ClipStack[m_ClipStack.size()-1];
		
		int x = ClipRect.x;
		int y = ClipRect.y;
		int w = ClipRect.w;
		int h = ClipRect.h;
		
		m_State.m_ClipEnable = true;
		m_State.m_ClipX = x;
		m_State.m_ClipY = ScreenHeight()-(y+h);
		m_State.m_ClipW = w;
		m_State.m_ClipH = h;
	}
	else
	{
		m_State.m_ClipEnable = false;
	}
}
	
void CGraphics::SetScreenSize(int Width, int Height)
{
	m_ScreenWidth = Width;
	m_ScreenHeight = Height;
	
	CCommandBuffer::SCommand_SetSize Cmd;
	Cmd.m_Width = Width;
	Cmd.m_Height = Height;
	m_pCommandBuffer->AddCommand(Cmd);

	// kick the buffer and wait for the result
	KickCommandBuffer();
	WaitForIdle();
}
