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

#ifndef __EDITOR_IMAGEPICKER__
#define __EDITOR_IMAGEPICKER__

#include <client/gui/widget.h>
#include <generated/assets/image.h>

class CImagePicker : public gui::CWidget
{
public:
	enum
	{
		SELECTTYPE_NONE=0,
		SELECTTYPE_TILE,
		SELECTTYPE_AREA,
	};
	
protected:
	class CGuiEditor* m_pAssetsEditor;
	CAssetPath m_ImagePath;
	int m_SelectionType;
	ivec2 m_Pivot;
	bool m_DragSelection;

protected:
	void ComputeImageSizes(const CAsset_Image* pImage, float& x0, float& y0, float& xStep, float& yStep);
	virtual void OnImagePicked(int MinX, int MinY, int MaxX, int MaxY) {};

public:
	CImagePicker(CGuiEditor* pAssetsEditor, CAssetPath ImagePath);
	
	virtual void UpdateBoundingSize();
	virtual void Render();
	
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	
	inline void SetImage(const CAssetPath& ImagePath) { m_ImagePath = ImagePath; }
	inline void SetSelectionType(int Type) { m_SelectionType = Type; }
};

#endif
