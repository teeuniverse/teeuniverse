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

#ifndef __SHARED_IMAGEPROCESSING_ADDBORDER__
#define __SHARED_IMAGEPROCESSING_ADDBORDER__

template<typename IMG>
void ImageProcessing_AddBorder(IMG& Image, int GridWidth, int GridHeight, int BorderSize)
{
	IMG TmpImage;
	TmpImage = Image;
	
	int CellWidth = Image.get_width()/GridWidth;
	int CellHeight = Image.get_height()/GridHeight;
	
	Image.init(
		(CellWidth+2*BorderSize)*GridWidth,
		(CellHeight+2*BorderSize)*GridHeight,
		TmpImage.get_depth()
	);
	
	for(int gy=0; gy<GridHeight; gy++)
	{
		for(int gx=0; gx<GridWidth; gx++)
		{
			for(int y=0; y<CellWidth; y++)
			{
				for(int x=0; x<CellWidth; x++)
				{
					int ox = gx*(CellWidth) + x;
					int oy = gy*(CellHeight) + y;
					int nx = gx*(CellWidth+2*BorderSize) + x + BorderSize;
					int ny = gy*(CellHeight+2*BorderSize) + y + BorderSize;
					
					for(int d=0; d<TmpImage.get_depth(); d++)
						Image.set_clamp(nx, ny, d, TmpImage.get_clamp(ox, oy, d));
				}
			}
			
			//Replicate corners
			for(int j=0; j<BorderSize; j++)
			{
				for(int i=0; i<BorderSize; i++)
				{
					for(int d=0; d<TmpImage.get_depth(); d++)
					{
						Image.set_clamp(
							gx*(CellWidth+2*BorderSize) + i,
							gy*(CellHeight+2*BorderSize) + j,
							d,
							TmpImage.get_clamp(gx*(CellWidth), gy*(CellHeight), d)
						);
						Image.set_clamp(
							(gx+1)*(CellWidth+2*BorderSize) - 1 - i,
							gy*(CellHeight+2*BorderSize) + j,
							d,
							TmpImage.get_clamp((gx+1)*(CellWidth)-1, gy*(CellHeight), d)
						);
						Image.set_clamp(
							gx*(CellWidth+2*BorderSize) + i,
							(gy+1)*(CellHeight+2*BorderSize) - 1 - j,
							d,
							TmpImage.get_clamp(gx*(CellWidth), (gy+1)*(CellHeight)-1, d)
						);
						Image.set_clamp(
							(gx+1)*(CellWidth+2*BorderSize) - 1 - i,
							(gy+1)*(CellHeight+2*BorderSize) - 1 - j,
							d,
							TmpImage.get_clamp((gx+1)*(CellWidth)-1, (gy+1)*(CellHeight)-1, d)
						);
					}
				}
			}
			
			//Replicate top/bottom
			for(int j=0; j<BorderSize; j++)
			{
				for(int i=0; i<CellWidth; i++)
				{
					for(int d=0; d<TmpImage.get_depth(); d++)
					{
						Image.set_clamp(
							gx*(CellWidth+2*BorderSize) + BorderSize + i,
							gy*(CellHeight+2*BorderSize) + j,
							d,
							TmpImage.get_clamp(gx*(CellWidth) + i, gy*(CellHeight), d)
						);
						Image.set_clamp(
							gx*(CellWidth+2*BorderSize) + BorderSize + i,
							(gy+1)*(CellHeight+2*BorderSize) - 1 - j,
							d,
							TmpImage.get_clamp(gx*(CellWidth) + i, (gy+1)*(CellHeight)-1, d)
						);
					}
				}
			}
			
			//Replicate left/right
			for(int j=0; j<CellHeight; j++)
			{
				for(int i=0; i<BorderSize; i++)
				{
					for(int d=0; d<TmpImage.get_depth(); d++)
					{
						Image.set_clamp(
							gx*(CellWidth+2*BorderSize) + i,
							gy*(CellHeight+2*BorderSize) + BorderSize + j,
							d,
							TmpImage.get_clamp(gx*(CellWidth), gy*(CellHeight) + j, d)
						);
						Image.set_clamp(
							(gx+1)*(CellWidth+2*BorderSize) - 1 - i,
							gy*(CellHeight+2*BorderSize) + BorderSize + j,
							d,
							TmpImage.get_clamp((gx+1)*(CellWidth)-1, gy*(CellHeight) + j, d)
						);
					}
				}
			}
		}
	}
}

#endif
