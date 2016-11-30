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

#include <editor/gui/view_map_transform.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/maprenderer.h>
#include <shared/geometry/geometry.h>
#include <generated/assets/maplayerquads.h>

/* CURSORTOOL QUAD PICKER *********************************************/

CCursorTool_MapPicker::CCursorTool_MapPicker(CViewMap* pViewMap, const CLocalizableString& LString, CAssetPath IconPath) :
	CCursorTool(pViewMap, LString, IconPath)
{
	
}

CSubPath CCursorTool_MapPicker::PickEntity(vec2 CursorPos)
{
	const CAsset_MapEntities* pEntities = AssetsManager()->GetAsset<CAsset_MapEntities>(AssetsEditor()->GetEditedAssetPath());
	if(pEntities)
	{
		float GizmoSize = 24.0f;
		
		CAsset_MapEntities::CIteratorEntity Iter;
		for(Iter = pEntities->ReverseBeginEntity(); Iter != pEntities->ReverseEndEntity(); ++Iter)
		{
			vec2 PivotPos = ViewMap()->MapRenderer()->MapPosToScreenPos(pEntities->GetEntityPosition(*Iter));
			if(length(CursorPos - PivotPos) < GizmoSize)
				return *Iter;
		}
	}
	
	return CSubPath::Null();
}

CSubPath CCursorTool_MapPicker::PickQuad(vec2 CursorPos)
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

void CCursorTool_MapPicker::RenderPivots()
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

CCursorTool_MapTransform::CCursorTool_MapTransform(CViewMap* pViewMap) :
	CCursorTool_MapPicker(pViewMap, _GUI("Transform"), pViewMap->AssetsEditor()->m_Path_Sprite_IconMove),
	m_Token(CAssetsHistory::NEW_TOKEN)
{
	
}

void CCursorTool_MapTransform::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
	if(pMapLayer)
	{
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
		return;
	}
	
	const CAsset_MapEntities* pEntities = AssetsManager()->GetAsset<CAsset_MapEntities>(AssetsEditor()->GetEditedAssetPath());
	if(pEntities)
	{
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		CSubPath EntityFound = PickEntity(CursorPos);
		
		if(!EntityFound.IsNull())
		{
			m_Token = AssetsManager()->GenerateToken();
			m_DragType = DRAGTYPE_TRANSLATION;
			
			AssetsEditor()->SetEditedAsset(
				AssetsEditor()->GetEditedAssetPath(),
				EntityFound
			);
		}
		else
			AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null());
		
		return;
	}
	
	const CAsset_MapGroup* pMapGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(AssetsEditor()->GetEditedAssetPath());
	if(pMapGroup)
	{
		m_Token = AssetsManager()->GenerateToken();
		m_DragType = DRAGTYPE_TRANSLATION;
		
		return;
	}
}
	
void CCursorTool_MapTransform::OnViewButtonRelease(int Button)
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
	
void CCursorTool_MapTransform::OnViewMouseMove()
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
	if(pMapLayer)
	{
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
			if(RelX != 0 || RelY != 0)
			{
				vec2 PivotScreenPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Position);
				vec2 Diff = ViewMap()->MapRenderer()->ScreenPosToMapPos(PivotScreenPos + vec2(RelX, RelY)) - Position;
				vec2 NewPivotPos = pMapLayer->GetQuadPivot(SelectedQuad) + Diff;
				AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedQuad, CAsset_MapLayerQuads::QUAD_PIVOT, NewPivotPos, m_Token);
				m_Transformed = true;
			}
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
		
		return;
	}
	
	const CAsset_MapEntities* pEntities = AssetsManager()->GetAsset<CAsset_MapEntities>(AssetsEditor()->GetEditedAssetPath());
	if(pEntities)
	{
		CSubPath SelectedEntity = AssetsEditor()->GetEditedSubPath();
		if(SelectedEntity.GetType() != CAsset_MapEntities::TYPE_ENTITY || !pEntities->IsValidEntity(SelectedEntity))
			return;
		
		ViewMap()->MapRenderer()->UnsetGroup();
		
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		int RelX = Context()->GetMouseRelPos().x;
		int RelY = Context()->GetMouseRelPos().y;
		
		if(m_DragType == DRAGTYPE_TRANSLATION)
		{
			vec2 Position = pEntities->GetEntityPosition(SelectedEntity);
			vec2 ScreenPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Position);
			vec2 NewPosition = ViewMap()->MapRenderer()->ScreenPosToMapPos(ScreenPos + vec2(RelX, RelY));
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedEntity, CAsset_MapEntities::ENTITY_POSITION, NewPosition, m_Token);
			m_Transformed = true;
		}
	}
	
	const CAsset_MapGroup* pMapGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(AssetsEditor()->GetEditedAssetPath());
	if(pMapGroup && m_DragType == DRAGTYPE_TRANSLATION)
	{
		ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
		
		vec2 CursorRelPos = vec2(Context()->GetMouseRelPos().x, Context()->GetMouseRelPos().y);
		vec2 ScreenPos = ViewMap()->MapRenderer()->MapPosToScreenPos(pMapGroup->GetPosition());
		vec2 NewPosition = ViewMap()->MapRenderer()->ScreenPosToMapPos(ScreenPos - CursorRelPos);
		
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapGroup::POSITION, NewPosition, m_Token);
		
		ViewMap()->MapRenderer()->UnsetGroup();
	}
}
	
void CCursorTool_MapTransform::RenderView()
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
	
void CCursorTool_MapTransform::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapGroup::TypeId:
		case CAsset_MapLayerQuads::TypeId:
		case CAsset_MapEntities::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	CCursorTool::Update(ParentEnabled);
}

void CCursorTool_MapTransform::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_GUI("Transform Tool: Move, rotate and scale objects"));
	
	CCursorTool_MapPicker::OnMouseMove();
}

/* CURSORTOOL QUAD EDIT ***********************************************/

CCursorTool_MapEdit::CCursorTool_MapEdit(CViewMap* pViewMap) :
	CCursorTool_MapPicker(pViewMap, _GUI("Edit"), pViewMap->AssetsEditor()->m_Path_Sprite_IconQuad),
	m_Token(CAssetsHistory::NEW_TOKEN),
	m_Vertex(CAsset_MapLayerQuads::VERTEX_NONE)
{
	
}

void CCursorTool_MapEdit::OnViewButtonClick(int Button)
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
			m_Vertex = CAsset_MapLayerQuads::VERTEX_PIVOT;
			
			m_Token = AssetsManager()->GenerateToken();
			
			return;
		}
		else
		{
			for(int p=3; p>=0; p--)
			{
				vec2 VertexPos;
				int VertexId = CAsset_MapLayerQuads::VERTEX0+p;
				
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
					
					SelectedQuad.SetId2(m_Vertex);
					AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), SelectedQuad);
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
	
void CCursorTool_MapEdit::OnViewButtonRelease(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	m_Vertex = CAsset_MapLayerQuads::VERTEX_NONE;
	m_Token = CAssetsHistory::NEW_TOKEN;
}
	
void CCursorTool_MapEdit::OnViewMouseMove()
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
	
	if(m_Vertex >= CAsset_MapLayerQuads::VERTEX0 && m_Vertex <= CAsset_MapLayerQuads::VERTEX3)
	{
		vec2 NewPos = InvTransform*(CursorMapPos - Position);
		
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedQuad, Members[m_Vertex-CAsset_MapLayerQuads::VERTEX0], NewPos, m_Token);
	}
	else if(m_Vertex == CAsset_MapLayerQuads::VERTEX_PIVOT)
	{
		vec2 Diff = InvTransform*(CursorMapPos - Position);
		vec2 NewPivot = pMapLayer->GetQuadPivot(SelectedQuad) + CursorMapPos - Position;
		
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedQuad, CAsset_MapLayerQuads::QUAD_PIVOT, NewPivot, m_Token);
		
		for(int p=0; p<4; p++)
		{
			vec2 OldVertex;
			OldVertex = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedQuad, Members[p], 0.0f);
			
			vec2 NewVertex = OldVertex - Diff;
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedQuad, Members[p], NewVertex, m_Token);
		}
		
	}
	
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapEdit::RenderView()
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
	
void CCursorTool_MapEdit::Update(bool ParentEnabled)
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

void CCursorTool_MapEdit::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_GUI("Edit Tool: Change the shape of object by moving verticies."));
	
	CCursorTool_MapPicker::OnMouseMove();
}

/* CURSORTOOL QUAD EDIT ***********************************************/

CCursorTool_MapEraser::CCursorTool_MapEraser(CViewMap* pViewMap) :
	CCursorTool_MapPicker(pViewMap, _GUI("Eraser"), pViewMap->AssetsEditor()->m_Path_Sprite_IconErase)
{
	
}

void CCursorTool_MapEraser::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
	{
		const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return;
		
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		CSubPath QuadFound = PickQuad(CursorPos);
			
		if(!QuadFound.IsNull())
		{
			AssetsManager()->DeleteSubItem(AssetsEditor()->GetEditedAssetPath(), QuadFound);
			AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null());
		}
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapEntities::TypeId)
	{
		const CAsset_MapEntities* pMapLayer = AssetsManager()->GetAsset<CAsset_MapEntities>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return;
		
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		CSubPath EntitiyFound = PickEntity(CursorPos);
			
		if(!EntitiyFound.IsNull())
		{
			AssetsManager()->DeleteSubItem(AssetsEditor()->GetEditedAssetPath(), EntitiyFound);
			AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null());
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapEraser::RenderView()
{
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
		RenderPivots();
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapEraser::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerQuads::TypeId:
		case CAsset_MapEntities::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	CCursorTool::Update(ParentEnabled);
}

void CCursorTool_MapEraser::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_GUI("Eraser Tool: Delete objects in the map."));
	
	CCursorTool_MapPicker::OnMouseMove();
}
