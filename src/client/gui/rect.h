/*
 * Copyright (C) 2016 necropotame (necropotame@gmail.com)
 * 
 * This file is part of TeeUniverses.
 * 
 * TeeUniverses is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * TeeUniverses is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with TeeUniverses.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CLIENT_GUI_RECT__
#define __CLIENT_GUI_RECT__

#include <shared/math/vector.h>
	
namespace gui
{

class CRect
{
public:
	union { int x,minx,minw,x0; };
	union { int y,miny,minh,y0; };
	union { int w,maxx,maxw,x1; };
	union { int h,maxy,maxh,y1; };
	
public:
	CRect();
	CRect(int X, int Y, int W, int H);
	
	void CenterIn(const CRect& Rect);
	CRect Intersection(const CRect& Rect);
	bool IsInside(int X, int Y) const;
	inline bool IsInside(const ivec2& Pos) const { return IsInside(Pos.x, Pos.y); }
	
	void RemoveMargin(int Padding);
	void AddMargin(int Padding);
	
	void BSNoConstraint();
	void BSMinimum(int W, int H);
	void BSFixed(int W, int H);
	void BSInfiniteWidth();
	void BSInfiniteHeight();
	void BSAddSpacing(int X, int Y);
	void BSAddMargin(int M);
	void BSHorizontalAdd(const CRect& Rect);
	void BSVerticalAdd(const CRect& Rect);
	void BSInnerAdd(const CRect& Rect);
	
	void DRUpdatePosition(const CRect& BoundingRect, const CRect BoundingSize);
};

}

#endif
