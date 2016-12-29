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

#include <editor/gui/view_material.h>

#include <editor/components/gui.h>
#include <client/maprenderer.h>
#include <client/gui/toggle.h>
#include <client/gui/filler.h>
#include <client/gui/combobox.h>

/* VIEW MATERIAL ******************************************************/

class CSimpleToggle : public gui::CToggle
{
protected:
	CGuiEditor* m_pAssetsEditor;
	bool* m_pBoolean;
	CLocalizableString m_LHint;
	
protected:
	virtual bool GetValue()
	{
		return *m_pBoolean;
	}

	virtual void SetValue(bool Value)
	{
		*m_pBoolean = Value;
	}
	
public:
	CSimpleToggle(CGuiEditor* pAssetsEditor, bool* pBoolean, CAssetPath IconPath, const CLocalizableString& LHint) :
		gui::CToggle(pAssetsEditor, IconPath),
		m_pAssetsEditor(pAssetsEditor),
		m_pBoolean(pBoolean)
	{
		SetToggleStyle(pAssetsEditor->m_Path_Toggle_Toolbar);
		m_LHint.copy(LHint);
	}

	virtual void OnMouseMove()
	{
		if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
			m_pAssetsEditor->SetHint(m_LHint);
		
		gui::CToggle::OnMouseMove();
	}
};

class CSimpleComboBox : public gui::CComboBox
{
protected:
	int* m_pInteger;

protected:
	virtual int GetValue() const
	{
		return *m_pInteger;
	}
	
	virtual void SetValue(int Value)
	{
		*m_pInteger = Value;
	}

public:
	CSimpleComboBox(CGuiEditor* pAssetsEditor, int* pInteger) :
		gui::CComboBox(pAssetsEditor),
		m_pInteger(pInteger)
	{
		NoTextClipping();
	}
};

enum
{
	SHAPE_CIRCLE=0,
	SHAPE_STAR,
	SHAPE_HEXAGON,
	SHAPE_HEXAGON2,
	SHAPE_PENTAGON,
	SHAPE_SQUARE,
	SHAPE_TRIANGLE,
	SHAPE_BOW,
	SHAPE_ARC,
	SHAPE_ARC_WEIGHT,
};

CViewMaterial::CViewMaterial(CGuiEditor* pAssetsEditor) :
	CViewManager::CView(pAssetsEditor),
	m_ShowMeshes(false),
	m_ObjectShape(0),
	m_pMapRenderer(NULL)
{	
	m_pMapRenderer = new CMapRenderer(AssetsEditor()->EditorKernel());
	
	m_pToolbar->Add(new gui::CFiller(Context()), true);
	
	{
		CSimpleComboBox* pComboBox = new CSimpleComboBox(AssetsEditor(), &m_ObjectShape);
		
		pComboBox->Add(_LSTRING("Circle"), AssetsEditor()->m_Path_Sprite_IconShapeCircle);
		pComboBox->Add(_LSTRING("Star"), AssetsEditor()->m_Path_Sprite_IconShapeStar);
		pComboBox->Add(_LSTRING("Hexagon"), AssetsEditor()->m_Path_Sprite_IconShapeHexagon);
		pComboBox->Add(_LSTRING("Hexagon"), AssetsEditor()->m_Path_Sprite_IconShapeHexagon2);
		pComboBox->Add(_LSTRING("Pentagon"), AssetsEditor()->m_Path_Sprite_IconShapePentagon);
		pComboBox->Add(_LSTRING("Square"), AssetsEditor()->m_Path_Sprite_IconShapeSquare);
		pComboBox->Add(_LSTRING("Triangle"), AssetsEditor()->m_Path_Sprite_IconShapeTriangle);
		pComboBox->Add(_LSTRING("Bow"), AssetsEditor()->m_Path_Sprite_IconShapeBow);
		pComboBox->Add(_LSTRING("Arc"), AssetsEditor()->m_Path_Sprite_IconShapeArc);
		pComboBox->Add(_LSTRING("Weighted Arc"), AssetsEditor()->m_Path_Sprite_IconShapeArcWeight);
		m_pToolbar->Add(pComboBox, false);
	}
	
	m_pToolbar->Add(new CSimpleToggle(AssetsEditor(), &m_ShowMeshes, AssetsEditor()->m_Path_Sprite_IconBigMesh, _LSTRING("Show/hide meshes")), false);
}

CViewMaterial::~CViewMaterial()
{
	if(m_pMapRenderer)
		delete m_pMapRenderer;
}

enum
{
	VIEWSHAPE_CIRCLE=0,
	VIEWSHAPE_BOW,
};

void CViewMaterial::RenderView()
{
	MapRenderer()->SetTime(0.0f);
	MapRenderer()->SetLocalTime((double)time_get()/(double)time_freq());
	MapRenderer()->SetCanvas(m_ViewRect, vec2(m_ViewRect.x + m_ViewRect.w/2, m_ViewRect.y + m_ViewRect.h/2));
	MapRenderer()->SetCamera(vec2(0.0f, 0.0f), 1.0f);
	
	CAsset_MapLayerObjects::CObject Object;
	array< CAsset_MapLayerObjects::CVertex, allocator_copy<CAsset_MapLayerObjects::CVertex> >& Vertices = Object.GetVertexArray();
	
	Object.SetPosition(0.0f);
	Object.SetSize(1.0f);
	Object.SetAngle(0.0f);
	Object.SetStylePath(AssetsEditor()->GetEditedAssetPath());
	
	float Radius = max(min(m_DrawRect.w, m_DrawRect.h)/2.0f - 128.0f, 128.0f);
	int NumSegments;
	switch(m_ObjectShape)
	{
		case SHAPE_CIRCLE:
			Object.SetClosedPath(true);
			NumSegments = 64;
			for(int i=0; i<NumSegments; i++)
			{
				float Angle = 2.0f*Pi*i/static_cast<float>(NumSegments);
				CAsset_MapLayerObjects::CVertex& Vertex = Vertices.increment();
				Vertex.SetPosition(vec2(cos(Angle), sin(Angle))*Radius);
				Vertex.SetColor(1.0f);
				Vertex.SetWeight(1.0f);
				Vertex.SetSmoothness(CBezierVertex::TYPE_CORNER);
			}
			break;
		case SHAPE_HEXAGON:
			Object.SetClosedPath(true);
			NumSegments = 6;
			for(int i=0; i<NumSegments; i++)
			{
				float Angle = 2.0f*Pi*i/static_cast<float>(NumSegments);
				CAsset_MapLayerObjects::CVertex& Vertex = Vertices.increment();
				Vertex.SetPosition(vec2(cos(Angle), sin(Angle))*Radius);
				Vertex.SetColor(1.0f);
				Vertex.SetWeight(1.0f);
				Vertex.SetSmoothness(CBezierVertex::TYPE_CORNER);
			}
			break;
		case SHAPE_PENTAGON:
			Object.SetClosedPath(true);
			NumSegments = 5;
			for(int i=0; i<NumSegments; i++)
			{
				float Angle = 2.0f*Pi*i/static_cast<float>(NumSegments);
				CAsset_MapLayerObjects::CVertex& Vertex = Vertices.increment();
				Vertex.SetPosition(vec2(cos(Angle), sin(Angle))*Radius);
				Vertex.SetColor(1.0f);
				Vertex.SetWeight(1.0f);
				Vertex.SetSmoothness(CBezierVertex::TYPE_CORNER);
			}
			break;
		case SHAPE_SQUARE:
			Object.SetClosedPath(true);
			NumSegments = 4;
			for(int i=0; i<NumSegments; i++)
			{
				float Angle = 2.0f*Pi*i/static_cast<float>(NumSegments);
				CAsset_MapLayerObjects::CVertex& Vertex = Vertices.increment();
				Vertex.SetPosition(vec2(cos(Angle), sin(Angle))*Radius);
				Vertex.SetColor(1.0f);
				Vertex.SetWeight(1.0f);
				Vertex.SetSmoothness(CBezierVertex::TYPE_CORNER);
			}
			break;
		case SHAPE_TRIANGLE:
			Object.SetClosedPath(true);
			NumSegments = 3;
			for(int i=0; i<NumSegments; i++)
			{
				float Angle = 2.0f*Pi*i/static_cast<float>(NumSegments);
				CAsset_MapLayerObjects::CVertex& Vertex = Vertices.increment();
				Vertex.SetPosition(vec2(cos(Angle), sin(Angle))*Radius);
				Vertex.SetColor(1.0f);
				Vertex.SetWeight(1.0f);
				Vertex.SetSmoothness(CBezierVertex::TYPE_CORNER);
			}
			break;
		case SHAPE_BOW:
			Object.SetClosedPath(true);
			NumSegments = 32;
			for(int i=0; i<=NumSegments; i++)
			{
				float Angle = Pi*i/static_cast<float>(NumSegments);
				CAsset_MapLayerObjects::CVertex& Vertex = Vertices.increment();
				Vertex.SetPosition(vec2(-cos(Angle), 0.5f-sin(Angle))*Radius);
				Vertex.SetColor(1.0f);
				Vertex.SetWeight(1.0f);
				Vertex.SetSmoothness(CBezierVertex::TYPE_CORNER);
			}
			break;
		case SHAPE_ARC:
			NumSegments = 32;
			for(int i=0; i<=NumSegments; i++)
			{
				float Angle = Pi*i/static_cast<float>(NumSegments);
				CAsset_MapLayerObjects::CVertex& Vertex = Vertices.increment();
				Vertex.SetPosition(vec2(-cos(Angle), 0.5f-sin(Angle))*Radius);
				Vertex.SetColor(1.0f);
				Vertex.SetWeight(1.0f);
				Vertex.SetSmoothness(CBezierVertex::TYPE_CORNER);
			}
			break;
		case SHAPE_ARC_WEIGHT:
			NumSegments = 32;
			for(int i=0; i<=NumSegments; i++)
			{
				float Angle = Pi*i/static_cast<float>(NumSegments);
				CAsset_MapLayerObjects::CVertex& Vertex = Vertices.increment();
				Vertex.SetPosition(vec2(-cos(Angle), 0.5f-sin(Angle))*Radius);
				Vertex.SetColor(1.0f);
				Vertex.SetWeight(0.1f + 1.9f*(i/static_cast<float>(NumSegments)));
				Vertex.SetSmoothness(CBezierVertex::TYPE_CORNER);
			}
			break;
	}
	
	MapRenderer()->RenderObject(Object, 0.0f, m_ShowMeshes);
	if(m_ShowMeshes)
		MapRenderer()->RenderObjectCurve(Object, 0.0f);
}
