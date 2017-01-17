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

#ifndef __SHARED_TL_ARRAY2D__
#define __SHARED_TL_ARRAY2D__

#include "allocator.h"
#include <shared/math/math.h>

template <class T, class ALLOCATOR = allocator_default<T> >
class array2d : private ALLOCATOR
{
private:
	int m_Width;
	int m_Height;
	int m_Depth;
	T* m_pData;

private:
	 //throw a compilation error if the object is copied
	array2d(const array2d&);
	array2d& operator=(const array2d&);

public:
	array2d() :
		m_Width(0),
		m_Height(0),
		m_Depth(1),
		m_pData(NULL)
	{
		
	}
	
	~array2d()
	{
		if(m_pData)
			ALLOCATOR::free_array(m_pData);
	}
	
	void copy(const array2d& Item)
	{
		if(m_pData)
			ALLOCATOR::free_array(m_pData);
		
		m_Width = Item.m_Width;
		m_Height = Item.m_Height;
		m_Depth = Item.m_Depth;
		
		int Size = m_Width * m_Height * m_Depth;
		m_pData = ALLOCATOR::alloc_array(Size);
		for(int i=0; i<Size; i++)
			ALLOCATOR::copy(m_pData[i], Item.m_pData[i]);
	}
	
	void transfert(array2d& Item)
	{
		if(m_pData)
			ALLOCATOR::free_array(m_pData);
		
		m_Width = Item.m_Width;
		m_Height = Item.m_Height;
		m_Depth = Item.m_Depth;
		m_pData = Item.m_pData;
		Item.m_pData = NULL;
	}
	
	inline int get_width() const { return m_Width; }
	inline int get_height() const { return m_Height; }
	inline int get_depth() const { return m_Depth; }
	inline int get_linear_size() const { return m_Width*m_Height*m_Depth; }
	
	void fill(const T& Element)
	{
		if(!m_pData)
			return;
		
		int Size = m_Width * m_Height * m_Depth;
		m_pData = ALLOCATOR::alloc_array(Size);
		for(int i=0; i<Size; i++)
			ALLOCATOR::copy(m_pData[i], Element);
	}
	
	const T& linear_get_clamp(int i) const
	{
		int d = i%m_Depth;
		i = i/m_Depth;
		int x = i%m_Width;
		i = i/m_Width;
		int y = i;
		return get_clamp(x, y, d);
	}
	
	T& linear_get_clamp(int i)
	{
		int d = i%m_Depth;
		i = i/m_Depth;
		int x = i%m_Width;
		i = i/m_Width;
		int y = i;
		return get_clamp(x, y, d);
	}
	
	void linear_set_clamp(int i, const T& Element)
	{
		set_clamp(i%m_Width, i/m_Width, Element);
	}
	
	const T& get_clamp(int x, int y, int d=0) const
	{
		x = clamp(x, 0, m_Width-1);
		y = clamp(y, 0, m_Height-1);
		d = clamp(d, 0, m_Depth-1);
		
		return m_pData[(y*m_Width+x)*m_Depth+d];
	}
	
	T& get_clamp(int x, int y, int d=0)
	{
		x = clamp(x, 0, m_Width-1);
		y = clamp(y, 0, m_Height-1);
		d = clamp(d, 0, m_Depth-1);
		
		return m_pData[(y*m_Width+x)*m_Depth+d];
	}
	
	void set_clamp(int x, int y, const T& Element)
	{
		x = clamp(x, 0, m_Width-1);
		y = clamp(y, 0, m_Height-1);
		
		ALLOCATOR::copy(m_pData[(y*m_Width+x)*m_Depth], Element);
	}
	
	void set_clamp(int x, int y, int d, const T& Element)
	{
		x = clamp(x, 0, m_Width-1);
		y = clamp(y, 0, m_Height-1);
		d = clamp(d, 0, m_Depth-1);
		
		ALLOCATOR::copy(m_pData[(y*m_Width+x)*m_Depth+d], Element);
	}
	
	void apply_shift(int sx, int sy)
	{
		if(sx > 0)
		{
			if(sy > 0)
			{
				for(int j=m_Height-1; j>=0; j--)
				{
					for(int i=m_Width-1; i>=0; i--)
					{
						for(int d=0; d<m_Depth; d++)
						{
							if(i-sx < 0 || i-sx >= m_Width || j-sy < 0 || j-sy >= m_Height)
								m_pData[(j*m_Width+i)*m_Depth+d] = T();
							else
								ALLOCATOR::transfert(m_pData[(j*m_Width+i)*m_Depth+d], m_pData[((j-sy)*m_Width+(i-sx))*m_Depth+d]);
						}
					}
				}
			}
			else
			{
				for(int j=0; j<m_Height; j++)
				{
					for(int i=m_Width-1; i>=0; i--)
					{
						for(int d=0; d<m_Depth; d++)
						{
							if(i-sx < 0 || i-sx >= m_Width || j-sy < 0 || j-sy >= m_Height)
								m_pData[(j*m_Width+i)*m_Depth+d] = T();
							else
								ALLOCATOR::transfert(m_pData[(j*m_Width+i)*m_Depth+d], m_pData[((j-sy)*m_Width+(i-sx))*m_Depth+d]);
						}
					}
				}
			}
		}
		else
		{
			if(sy > 0)
			{
				for(int j=m_Height-1; j>=0; j--)
				{
					for(int i=0; i<m_Width; i++)
					{
						for(int d=0; d<m_Depth; d++)
						{
							if(i-sx < 0 || i-sx >= m_Width || j-sy < 0 || j-sy >= m_Height)
								m_pData[(j*m_Width+i)*m_Depth+d] = T();
							else
								ALLOCATOR::transfert(m_pData[(j*m_Width+i)*m_Depth+d], m_pData[((j-sy)*m_Width+(i-sx))*m_Depth+d]);
						}
					}
				}
			}
			else
			{
				for(int j=0; j<m_Height; j++)
				{
					for(int i=0; i<m_Width; i++)
					{
						for(int d=0; d<m_Depth; d++)
						{
							if(i-sx < 0 || i-sx >= m_Width || j-sy < 0 || j-sy >= m_Height)
								m_pData[(j*m_Width+i)*m_Depth+d] = T();
							else
								ALLOCATOR::transfert(m_pData[(j*m_Width+i)*m_Depth+d], m_pData[((j-sy)*m_Width+(i-sx))*m_Depth+d]);
						}
					}
				}
			}
		}
	}
	
	void init(int Width, int Height, int Depth=1)
	{
		if(m_pData)
			ALLOCATOR::free_array(m_pData);
		
		m_Width = Width;
		m_Height = Height;
		m_Depth = Depth;
		if(m_Width == 0 || m_Height == 0 || m_Depth == 0)
			m_pData = 0;
		else
			m_pData = ALLOCATOR::alloc_array(m_Width * m_Height * m_Depth);
	}
	
	void resize(int Width, int Height, int Depth=1)
	{
		if(!m_pData)
			init(Width, Height, Depth);
		else if(Width == 0 || Height == 0 || Depth == 0)
		{
			m_Width = Width;
			m_Height = Height;
			m_Depth = Depth;
			ALLOCATOR::free_array(m_pData);
			m_pData = 0;
		}
		else
		{
			T* pData = ALLOCATOR::alloc_array(Width * Height * Depth);
			if(Depth == m_Depth)
			{
				int SharedW = min(m_Width, Width);
				int SharedH = min(m_Height, Height);
				for(int j=0; j<SharedH; j++)
				{
					for(int i=0; i<SharedW; i++)
					{
						for(int d=0; d<m_Depth; d++)
							ALLOCATOR::transfert(pData[(j*Width+i)*m_Depth+d], m_pData[(j*m_Width+i)*m_Depth+d]);
					}
				}
			}
			ALLOCATOR::free_array(m_pData);
			m_Width = Width;
			m_Height = Height;
			m_Depth = Depth;
			m_pData = pData;
		}
	}
	
	void resize_width(int Width)
	{
		resize(Width, m_Height, m_Depth);
	}
	
	void resize_height(int Height)
	{
		resize(m_Width, Height, m_Depth);
	}
	
	void resize_depth(int Depth)
	{
		resize(m_Width, m_Height, Depth);
	}
	
	const T* base_ptr() const
	{
		return m_pData;
	}
};

#endif
