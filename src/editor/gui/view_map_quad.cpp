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

#include <editor/gui/view_map_quad.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/maprenderer.h>
#include <shared/geometry/geometry.h>
#include <generated/assets/maplayerquads.h>

/* CURSORTOOL QUAD PICKER *********************************************/

CCursorTool_QuadPicker::CCursorTool_QuadPicker(CViewMap* pViewMap, const char* pName, CAssetPath IconPath) :
	CCursorTool(pViewMap, pName, IconPath)
{
	
}

CSubPath CCursorTool_QuadPicker::PickQuad(vec2 CursorPos)
{
	const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return CSubPath::Null();
	
	const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(pMapLayer->GetImagePath());
	
	CSubPath QuadFound = CSubPath::Null();
	
	float GizmoSize = 16.0f;
	{
		CAsset_MapLayerQuads::CIteratorQuad Iter;
		for(Iter = pMapLayer->BeginQuad(); Iter != pMapLayer->EndQuad(); ++Iter)
		{
			vec2 Position;
			matrix2x2 Transform;
			pMapLayer->GetQuadTransform(*Iter, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
			
			vec2 PivotPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*vec2(0.0f, 0.0f) + Position);
			if(length(CursorPos - PivotPos) < GizmoSize*(12.0f/16.0f))
				QuadFound = *Iter;
			
			if(!QuadFound.IsNull())
				break;
		}
	}
	
	{
		CAsset_MapLayerQuads::CIteratorQuad Iter;
		for(Iter = pMapLayer->ReverseBeginQuad(); Iter != pMapLayer->ReverseEndQuad(); ++Iter)
		{
			vec2 Position;
			matrix2x2 Transform;
			pMapLayer->GetQuadTransform(*Iter, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
			
			vec2 Vertices[4];
			Vertices[0] = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*pMapLayer->GetQuadVertex0(*Iter) + Position);
			Vertices[1] = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*pMapLayer->GetQuadVertex1(*Iter) + Position);
			Vertices[2] = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*pMapLayer->GetQuadVertex2(*Iter) + Position);
			Vertices[3] = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*pMapLayer->GetQuadVertex3(*Iter) + Position);
			
			if(InsideQuad(Vertices[0], Vertices[1], Vertices[2], Vertices[3], CursorPos))
			{
				if(pImage)
				{
					vec2 uv = SampleQuad(
						Vertices[0],
						Vertices[1],
						Vertices[2],
						Vertices[3],
						CursorPos,
						pMapLayer->GetQuadUV0(*Iter),
						pMapLayer->GetQuadUV1(*Iter),
						pMapLayer->GetQuadUV2(*Iter),
						pMapLayer->GetQuadUV3(*Iter)
					);
					
					vec4 Color = pImage->Sample(uv);
					int Alpha = 255.0f*Color.a;
					if(Alpha > 0)
						QuadFound = *Iter;
				}
				else
					QuadFound = *Iter;
			}
			
			if(!QuadFound.IsNull())
				break;
		}
	}
	
	return QuadFound;
}

void CCursorTool_QuadPicker::RenderPivots()
{
	const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	float GizmoSize = 16.0f;
	
	CAsset_MapLayerQuads::CIteratorQuad Iter;
	for(Iter = pMapLayer->BeginQuad(); Iter != pMapLayer->EndQuad(); ++Iter)
	{
		vec2 Position;
		matrix2x2 Transform;
		pMapLayer->GetQuadTransform(*Iter, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
				
		//Pivot
		{
			vec2 QuadPivot = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*vec2(0.0f, 0.0f) + Position);
			
			AssetsRenderer()->DrawSprite(
				AssetsEditor()->m_Path_Sprite_GizmoPivot,
				vec2(QuadPivot.x, QuadPivot.y),
				1.0f, 0.0f, 0x0, 1.0f
			);
		}
	}
}

/* CURSORTOOL QUAD TRANSFORM ******************************************/

CCursorTool_QuadTransform::CCursorTool_QuadTransform(CViewMap* pViewMap) :
	CCursorTool_QuadPicker(pViewMap, "Transform", pViewMap->AssetsEditor()->m_Path_Sprite_IconMove),
	m_Token(CAssetsHistory::NEW_TOKEN)
{
	
}

void CCursorTool_QuadTransform::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(pMapLayer->GetImagePath());
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	//Find gizmo
	float GizmoSize = 16.0f;
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	
	CSubPath SelectedQuad = AssetsEditor()->GetEditedSubPath();
	if(SelectedQuad.GetType() == CAsset_MapLayerQuads::TYPE_QUAD && pMapLayer->IsValidQuad(SelectedQuad))
	{
		vec2 Position;
		matrix2x2 Transform;
		pMapLayer->GetQuadTransform(SelectedQuad, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
		
		vec2 Vertex = pMapLayer->GetQuadVertex0(SelectedQuad);
		
		vec2 BBMin = Vertex;
		vec2 BBMax = BBMin;
		
		Vertex = pMapLayer->GetQuadVertex1(SelectedQuad);
		UpdateBoundingBox(Vertex, &BBMin, &BBMax);
		Vertex = pMapLayer->GetQuadVertex2(SelectedQuad);
		UpdateBoundingBox(Vertex, &BBMin, &BBMax);
		Vertex = pMapLayer->GetQuadVertex3(SelectedQuad);
		UpdateBoundingBox(Vertex, &BBMin, &BBMax);
		
		vec2 GizmosPos[4];
		GizmosPos[0] = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*vec2(BBMin.x, BBMin.y) + Position);
		GizmosPos[1] = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*vec2(BBMax.x, BBMin.y) + Position);
		GizmosPos[2] = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*vec2(BBMin.x, BBMax.y) + Position);
		GizmosPos[3] = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*vec2(BBMax.x, BBMax.y) + Position);
		
		for(int i=0; i<4; i++)
		{
			if(length(CursorPos - GizmosPos[i]) < GizmoSize*(12.0f/16.0f))
			{
				m_Token = AssetsManager()->GenerateToken();
				m_ClickDiff = CursorPos - GizmosPos[i];
				m_SelectedGizmo = i;
				m_DragType = DRAGTYPE_GIZMO;
		
				ViewMap()->MapRenderer()->UnsetGroup();
				return;
			}
		}
	}
	
	CSubPath QuadFound = PickQuad(CursorPos);
	
	if(!QuadFound.IsNull())
	{
		if(SelectedQuad != QuadFound)
		{
			SelectedQuad = QuadFound;
			AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), SelectedQuad);
			
			m_GizmoType = GIZMOTYPE_ROTATION;
			m_Transformed = true;
		}
		else
			m_Transformed = false;
		
		m_Token = AssetsManager()->GenerateToken();
		m_DragType = DRAGTYPE_TRANSLATION;
		
		AssetsEditor()->SetEditedAsset(
			AssetsEditor()->GetEditedAssetPath(),
			SelectedQuad
		);
	}
	else
		AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null());
		
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_QuadTransform::OnViewButtonRelease(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	m_SelectedGizmo = -1;
	m_DragType = DRAGTYPE_NONE;
	m_Token = CAssetsHistory::NEW_TOKEN;
	
	if(!m_Transformed)
	{
		if(m_GizmoType == GIZMOTYPE_ROTATION)
			m_GizmoType = GIZMOTYPE_SCALE;
		else
			m_GizmoType = GIZMOTYPE_ROTATION;
	}
}
	
void CCursorTool_QuadTransform::OnViewMouseMove()
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
		
	CSubPath SelectedQuad = AssetsEditor()->GetEditedSubPath();
	if(SelectedQuad.GetType() != CAsset_MapLayerQuads::TYPE_QUAD || !pMapLayer->IsValidQuad(SelectedQuad))
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	vec2 Position;
	matrix2x2 Transform;
	pMapLayer->GetQuadTransform(SelectedQuad, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
	
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	int RelX = Context()->GetMouseRelPos().x;
	int RelY = Context()->GetMouseRelPos().y;
	
	if(m_DragType == DRAGTYPE_TRANSLATION)
	{
		vec2 PivotScreenPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Position);
		vec2 Diff = ViewMap()->MapRenderer()->ScreenPosToMapPos(PivotScreenPos + vec2(RelX, RelY)) - Position;
		vec2 NewPivotPos = pMapLayer->GetQuadPivot(SelectedQuad) + Diff;
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedQuad, CAsset_MapLayerQuads::QUAD_PIVOT, NewPivotPos, m_Token);
		m_Transformed = true;
	}
	else if(m_DragType == DRAGTYPE_GIZMO)
	{
		if(m_SelectedGizmo < 0)
		{
			ViewMap()->MapRenderer()->UnsetGroup();
			return;
		}
		
		vec2 Vertex = pMapLayer->GetQuadVertex0(SelectedQuad);
		
		vec2 BBMin = Vertex;
		vec2 BBMax = BBMin;
		
		Vertex = pMapLayer->GetQuadVertex1(SelectedQuad);
		UpdateBoundingBox(Vertex, &BBMin, &BBMax);
		Vertex = pMapLayer->GetQuadVertex2(SelectedQuad);
		UpdateBoundingBox(Vertex, &BBMin, &BBMax);
		Vertex = pMapLayer->GetQuadVertex3(SelectedQuad);
		UpdateBoundingBox(Vertex, &BBMin, &BBMax);
		
		vec2 GizmosPos;
		switch(m_SelectedGizmo)
		{
			case 0:
				GizmosPos = Transform * vec2(BBMin.x, BBMin.y) + Position;
				break;
			case 1:
				GizmosPos = Transform * vec2(BBMax.x, BBMin.y) + Position;
				break;
			case 2:
				GizmosPos = Transform * vec2(BBMin.x, BBMax.y) + Position;
				break;
			case 3:
				GizmosPos = Transform * vec2(BBMax.x, BBMax.y) + Position;
				break;
		}
		
		vec2 MousePos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos - m_ClickDiff);
		vec2 PivotPos = Position;
		
		if(m_GizmoType == GIZMOTYPE_ROTATION)
		{
			float Angle = pMapLayer->GetQuadAngle(SelectedQuad);
			Angle -= angle(normalize(MousePos - PivotPos), normalize(GizmosPos - PivotPos));
			AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), SelectedQuad, CAsset_MapLayerQuads::QUAD_ANGLE, Angle, m_Token);
		}
		else
		{
			vec2 Size = pMapLayer->GetQuadSize(SelectedQuad);
			
			float Ratio = length(MousePos - PivotPos)/length(GizmosPos - PivotPos);
			Size *= Ratio;
			
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedQuad, CAsset_MapLayerQuads::QUAD_SIZE, Size, m_Token);
		}
		
		m_Transformed = true;
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_QuadTransform::RenderView()
{
	const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
		
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	RenderPivots();
	
	CSubPath SelectedQuad = AssetsEditor()->GetEditedSubPath();
	if(SelectedQuad.GetType() == CAsset_MapLayerQuads::TYPE_QUAD && pMapLayer->IsValidQuad(SelectedQuad))
	{
		ViewMap()->MapRenderer()->RenderQuads_Mesh(&pMapLayer->GetQuad(SelectedQuad), 1);

		float GizmoSize = 16.0f;
	
		vec2 Position;
		matrix2x2 Transform;
		pMapLayer->GetQuadTransform(SelectedQuad, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
		
		CAssetPath GizmoPath;
		if(m_GizmoType == GIZMOTYPE_ROTATION)
			GizmoPath = AssetsEditor()->m_Path_Sprite_GizmoRotate;
		else if(m_GizmoType == GIZMOTYPE_SCALE)
			GizmoPath = AssetsEditor()->m_Path_Sprite_GizmoScale;
		
		vec2 Vertex = pMapLayer->GetQuadVertex0(SelectedQuad);
		
		vec2 BBMin = Vertex;
		vec2 BBMax = BBMin;
		
		Vertex = pMapLayer->GetQuadVertex1(SelectedQuad);
		UpdateBoundingBox(Vertex, &BBMin, &BBMax);
		Vertex = pMapLayer->GetQuadVertex2(SelectedQuad);
		UpdateBoundingBox(Vertex, &BBMin, &BBMax);
		Vertex = pMapLayer->GetQuadVertex3(SelectedQuad);
		UpdateBoundingBox(Vertex, &BBMin, &BBMax);
		
		vec2 Vertex0Pos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform * vec2(BBMin.x, BBMin.y) + Position);
		vec2 Vertex1Pos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform * vec2(BBMax.x, BBMin.y) + Position);
		vec2 Vertex2Pos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform * vec2(BBMin.x, BBMax.y) + Position);
		vec2 Vertex3Pos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform * vec2(BBMax.x, BBMax.y) + Position);
		
		float GizmoAngle = angle(vec2(1.0f, 0.0f), Transform * vec2(1.0f, 0.0f));
		
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(Vertex0Pos.x, Vertex0Pos.y), 1.0f, GizmoAngle, 0x0, 1.0f);
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(Vertex1Pos.x, Vertex1Pos.y), 1.0f, GizmoAngle+pi/2.0f, 0x0, 1.0f);
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(Vertex2Pos.x, Vertex2Pos.y), 1.0f, GizmoAngle-pi/2.0f, 0x0, 1.0f);
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(Vertex3Pos.x, Vertex3Pos.y), 1.0f, GizmoAngle+pi, 0x0, 1.0f);
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_QuadTransform::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerQuads::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	CCursorTool::Update(ParentEnabled);
}

/* CURSORTOOL QUAD EDIT ***********************************************/

CCursorTool_QuadEdit::CCursorTool_QuadEdit(CViewMap* pViewMap) :
	CCursorTool_QuadPicker(pViewMap, "Edit", pViewMap->AssetsEditor()->m_Path_Sprite_IconQuad),
	m_Token(CAssetsHistory::NEW_TOKEN),
	m_Vertex(VERTEX_NONE)
{
	
}

void CCursorTool_QuadEdit::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	//Find gizmo
	float GizmoSize = 16.0f;
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);

	CSubPath SelectedQuad = AssetsEditor()->GetEditedSubPath();
	if(SelectedQuad.GetType() == CAsset_MapLayerQuads::TYPE_QUAD && pMapLayer->IsValidQuad(SelectedQuad))
	{
		vec2 Position;
		matrix2x2 Transform;
		pMapLayer->GetQuadTransform(SelectedQuad, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
		
		vec2 PivotPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*vec2(0.0f, 0.0f) + Position);
		if(length(CursorPos - PivotPos) < GizmoSize*(12.0f/16.0f))
		{
			m_ClickDiff = CursorPos - PivotPos;
			m_Vertex = VERTEX_PIVOT;
			
			m_Token = AssetsManager()->GenerateToken();
			
			return;
		}
		else
		{
			for(int p=3; p>=0; p--)
			{
				vec2 VertexPos;
				int VertexId = p;
				
				switch(p)
				{
					case 0:
						VertexPos = pMapLayer->GetQuadVertex0(SelectedQuad);
						break;
					case 1:
						VertexPos = pMapLayer->GetQuadVertex1(SelectedQuad);
						break;
					case 2:
						VertexPos = pMapLayer->GetQuadVertex2(SelectedQuad);
						break;
					case 3:
						VertexPos = pMapLayer->GetQuadVertex3(SelectedQuad);
						break;
				}
				
				vec2 VertexScreenPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*VertexPos + Position);
				if(length(CursorPos - VertexScreenPos) < GizmoSize*(12.0f/16.0f))
				{
					m_ClickDiff = CursorPos - VertexScreenPos;
					m_Vertex = VertexId;
						
					m_Token = AssetsManager()->GenerateToken();
					
					return;
				}
			}
		}
	}
	
	CSubPath QuadFound = PickQuad(CursorPos);
		
	if(!QuadFound.IsNull())
	{
		if(SelectedQuad != QuadFound)
		{
			SelectedQuad = QuadFound;
			m_Token = AssetsManager()->GenerateToken();
		}
		
		AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), SelectedQuad);
	}
	else
	{
		AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null());
	}
	
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_QuadEdit::OnViewButtonRelease(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	m_Vertex = VERTEX_NONE;
	m_Token = CAssetsHistory::NEW_TOKEN;
}
	
void CCursorTool_QuadEdit::OnViewMouseMove()
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	CSubPath SelectedQuad = AssetsEditor()->GetEditedSubPath();
	if(SelectedQuad.GetType() != CAsset_MapLayerQuads::TYPE_QUAD || !pMapLayer->IsValidQuad(SelectedQuad))
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos - m_ClickDiff);
	
	vec2 Position;
	matrix2x2 Transform;
	pMapLayer->GetQuadTransform(SelectedQuad, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
	matrix2x2 InvTransform = matrix2x2::inverse(Transform);
	
	int Members[] = {
		CAsset_MapLayerQuads::QUAD_VERTEX0,
		CAsset_MapLayerQuads::QUAD_VERTEX1,
		CAsset_MapLayerQuads::QUAD_VERTEX2,
		CAsset_MapLayerQuads::QUAD_VERTEX3
	};
	
	if(m_Vertex >= VERTEX0 && m_Vertex <= VERTEX3)
	{
		vec2 NewPos = InvTransform*(CursorMapPos - Position);
		
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedQuad, Members[m_Vertex], NewPos, m_Token);
	}
	else if(m_Vertex == VERTEX_PIVOT)
	{
		vec2 Diff = InvTransform*(CursorMapPos - Position);
		vec2 NewPivot = Diff + pMapLayer->GetQuadPivot(SelectedQuad);
		
		for(int p=0; p<4; p++)
		{
			vec2 OldVertex;
			OldVertex = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedQuad, Members[p], 0.0f);
			
			vec2 NewVertex = OldVertex - Diff;
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedQuad, Members[p], NewVertex, m_Token);
		}
		
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedQuad, CAsset_MapLayerQuads::QUAD_PIVOT, NewPivot, m_Token);
	}
	
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_QuadEdit::RenderView()
{
	const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
		
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	RenderPivots();
	
	float GizmoSize = 16.0f;
	
	CSubPath SelectedQuad = AssetsEditor()->GetEditedSubPath();
	if(SelectedQuad.GetType() != CAsset_MapLayerQuads::TYPE_QUAD || !pMapLayer->IsValidQuad(SelectedQuad))
		return;
	
	ViewMap()->MapRenderer()->RenderQuads_Mesh(&pMapLayer->GetQuad(SelectedQuad), 1);
	
	vec2 Position;
	matrix2x2 Transform;
	pMapLayer->GetQuadTransform(SelectedQuad, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
	
	for(int p=0; p<4; p++)
	{
		vec2 VertexPos;
		vec4 VertexColor;
		switch(p)
		{
			case 0:
				VertexPos = pMapLayer->GetQuadVertex0(SelectedQuad);
				VertexColor = pMapLayer->GetQuadColor0(SelectedQuad);
				break;
			case 1:
				VertexPos = pMapLayer->GetQuadVertex1(SelectedQuad);
				VertexColor = pMapLayer->GetQuadColor1(SelectedQuad);
				break;
			case 2:
				VertexPos = pMapLayer->GetQuadVertex2(SelectedQuad);
				VertexColor = pMapLayer->GetQuadColor2(SelectedQuad);
				break;
			case 3:
				VertexPos = pMapLayer->GetQuadVertex3(SelectedQuad);
				VertexColor = pMapLayer->GetQuadColor3(SelectedQuad);
				break;
		}
		vec2 VertexScreenPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*VertexPos + Position);
		
		AssetsRenderer()->DrawSprite(
			AssetsEditor()->m_Path_Sprite_GizmoVertexBg,
			vec2(VertexScreenPos.x, VertexScreenPos.y),
			1.0f, 0.0, 0x0, 1.0f
		);
		
		VertexColor.a = 1.0f;
		
		AssetsRenderer()->DrawSprite(
			AssetsEditor()->m_Path_Sprite_GizmoVertexFg,
			vec2(VertexScreenPos.x, VertexScreenPos.y),
			1.0f, 0.0, 0x0, VertexColor
		);
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_QuadEdit::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerQuads::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	CCursorTool::Update(ParentEnabled);
}
