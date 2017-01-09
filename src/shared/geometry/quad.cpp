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

#include <shared/geometry/quad.h>

void RotateQuadTexture(CTexturedQuad& Quad)
{
	vec2 Tmp = Quad.m_Texture[0];
	Quad.m_Texture[0] = Quad.m_Texture[1];
	Quad.m_Texture[1] = Quad.m_Texture[3];
	Quad.m_Texture[3] = Quad.m_Texture[2];
	Quad.m_Texture[2] = Tmp;
}

void TesselateQuad(const CTexturedQuad& Input, array<CTexturedQuad>& Output, int TesselationX, int TesselationY)
{
	for(int j=0; j<TesselationY; j++)
	{
		float y0 = static_cast<float>(j)/TesselationY;
		float y1 = static_cast<float>(j+1)/TesselationY;
		
		for(int i=0; i<TesselationX; i++)
		{
			float x0 = static_cast<float>(i)/TesselationX;
			float x1 = static_cast<float>(i+1)/TesselationX;
			
			CTexturedQuad& Quad = Output.increment();
			Quad.m_ImagePath = Input.m_ImagePath;
			
			Quad.m_Position[0] = mix(mix(Input.m_Position[0], Input.m_Position[1], x0), mix(Input.m_Position[2], Input.m_Position[3], x0), y0);
			Quad.m_Position[1] = mix(mix(Input.m_Position[0], Input.m_Position[1], x1), mix(Input.m_Position[2], Input.m_Position[3], x1), y0);
			Quad.m_Position[2] = mix(mix(Input.m_Position[0], Input.m_Position[1], x0), mix(Input.m_Position[2], Input.m_Position[3], x0), y1);
			Quad.m_Position[3] = mix(mix(Input.m_Position[0], Input.m_Position[1], x1), mix(Input.m_Position[2], Input.m_Position[3], x1), y1);
			
			Quad.m_Texture[0] = mix(mix(Input.m_Texture[0], Input.m_Texture[1], x0), mix(Input.m_Texture[2], Input.m_Texture[3], x0), y0);
			Quad.m_Texture[1] = mix(mix(Input.m_Texture[0], Input.m_Texture[1], x1), mix(Input.m_Texture[2], Input.m_Texture[3], x1), y0);
			Quad.m_Texture[2] = mix(mix(Input.m_Texture[0], Input.m_Texture[1], x0), mix(Input.m_Texture[2], Input.m_Texture[3], x0), y1);
			Quad.m_Texture[3] = mix(mix(Input.m_Texture[0], Input.m_Texture[1], x1), mix(Input.m_Texture[2], Input.m_Texture[3], x1), y1);
			
			Quad.m_Color[0] = mix(mix(Input.m_Color[0], Input.m_Color[1], x0), mix(Input.m_Color[2], Input.m_Color[3], x0), y0);
			Quad.m_Color[1] = mix(mix(Input.m_Color[0], Input.m_Color[1], x1), mix(Input.m_Color[2], Input.m_Color[3], x1), y0);
			Quad.m_Color[2] = mix(mix(Input.m_Color[0], Input.m_Color[1], x0), mix(Input.m_Color[2], Input.m_Color[3], x0), y1);
			Quad.m_Color[3] = mix(mix(Input.m_Color[0], Input.m_Color[1], x1), mix(Input.m_Color[2], Input.m_Color[3], x1), y1);
		}
	}
}
