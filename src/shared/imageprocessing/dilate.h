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

#ifndef __SHARED_IMAGEPROCESSING_DILATE__
#define __SHARED_IMAGEPROCESSING_DILATE__

template<typename IMG>
void ImageProcessing_Dilate(IMG& Image, int NumIterations)
{
	IMG TmpImages[2];
	
	for(int i=0; i<sizeof(TmpImages)/sizeof(IMG); i++)
		TmpImages[i].copy(Image);
	
	const int sx[] = {0, -1, 1, 0};
	const int sy[] = {-1, 0, 0, 1};
	
	int AlphaDepth = Image.get_depth()-1;
	
	int CurrTmp = 0;
	int NextTmp = 1;
	for(int i=0; i<NumIterations; i++)
	{
		NextTmp = (CurrTmp+1)%(sizeof(TmpImages)/sizeof(IMG));
		
		for(int y=0; y<Image.get_height(); y++)
		{
			for(int x=0; x<Image.get_width(); x++)
			{
				for(int d=0; d<Image.get_depth(); d++)
					TmpImages[NextTmp].set_clamp(x, y, d, TmpImages[CurrTmp].get_clamp(x, y, d));
				
				if(TmpImages[CurrTmp].get_clamp(x, y, AlphaDepth))
					continue;
				
				for(int k=0; k<sizeof(sx)/sizeof(int); k++)
				{
					if(TmpImages[CurrTmp].get_clamp(x+sx[k], y+sy[k], AlphaDepth))
					{
						for(int d=0; d<Image.get_depth()-1; d++)
							TmpImages[NextTmp].set_clamp(x, y, d, TmpImages[CurrTmp].get_clamp(x+sx[k], y+sy[k], d));
						TmpImages[NextTmp].set_clamp(x, y, AlphaDepth, 255);
						break;
					}
				}
			}
		}
		
		CurrTmp = NextTmp;
	}
	
	for(int y=0; y<Image.get_height(); y++)
	{
		for(int x=0; x<Image.get_width(); x++)
		{
			for(int d=0; d<Image.get_depth()-1; d++)
				Image.set_clamp(x, y, d, TmpImages[CurrTmp].get_clamp(x, y, d));
		}
	}
}

#endif
