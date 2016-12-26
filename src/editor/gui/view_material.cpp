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

CViewMaterial::CViewMaterial(CGuiEditor* pAssetsEditor) :
	CViewManager::CView(pAssetsEditor),
	m_ShowMeshes(false),
	m_pMapRenderer(NULL)
{	
	m_pMapRenderer = new CMapRenderer(AssetsEditor()->EditorKernel());
	
	m_pToolbar->Add(new gui::CFiller(Context()), true);
	m_pToolbar->Add(new CSimpleToggle(AssetsEditor(), &m_ShowMeshes, AssetsEditor()->m_Path_Sprite_IconBigMesh, _LSTRING("Show/hide meshes")), false);
}

CViewMaterial::~CViewMaterial()
{
	if(m_pMapRenderer)
		delete m_pMapRenderer;
}

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
	Object.SetClosedPath(true);

	float Radius = max(min(m_DrawRect.w, m_DrawRect.h)/2.0f - 128.0f, 128.0f);
	int NumSegments = 64;
	for(int i=0; i<NumSegments; i++)
	{
		float Angle = 2.0f*Pi*i/static_cast<float>(NumSegments);
		CAsset_MapLayerObjects::CVertex& Vertex = Vertices.increment();
		Vertex.SetPosition(vec2(cos(Angle), sin(Angle))*Radius);
		Vertex.SetColor(1.0f);
		Vertex.SetWeight(1.0f);
		Vertex.SetSmoothness(CBezierVertex::TYPE_CORNER);
	}
	
	MapRenderer()->RenderObject(Object, 0.0f, m_ShowMeshes);
	if(m_ShowMeshes)
		MapRenderer()->RenderObjectCurve(Object, 0.0f);
}
