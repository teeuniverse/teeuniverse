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

#include <editor/gui/view_map_vertexeditor.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/maprenderer.h>
#include <shared/geometry/geometry.h>
#include <generated/assets/maplayerquads.h>
#include <client/gui/toggle.h>

/* CURSORTOOL VERTEX EDITOR *******************************************/

CCursorTool_MapVertexEditor::CCursorTool_MapVertexEditor(CViewMap* pViewMap) :
	CCursorTool_MapPicker(pViewMap, _LSTRING("Vertex Editor"), pViewMap->AssetsEditor()->m_Path_Sprite_IconMoveVertex),
	m_DragType(0),
	m_Vertex(CAsset_MapLayerQuads::VERTEX_NONE),
	m_Token(CAssetsHistory::NEW_TOKEN)
{
	
}

template<typename ASSET>
vec2 CCursorTool_MapVertexEditor::PickVertex_Objects_Impl(vec2 PickPosition)
{
	m_CurrentVertex = CSubPath::Null();
	
	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return 0.0f;
	
	//Find a vertex
	typename ASSET::CIteratorObject Iter;
	for(Iter = pMapLayer->BeginObject(); Iter != pMapLayer->EndObject(); ++Iter)
	{
		const typename ASSET::CObject& Object = pMapLayer->GetObject(*Iter);
		const std::vector<typename ASSET::CVertex>& Vertices = Object.GetVertexArray();
		vec2 Position;
		matrix2x2 Transform;
		Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
		
		for(int i=static_cast<int>(Vertices.size())-1; i>=0; i--)
		{
			vec2 VertexPosition = Transform*Vertices[i].GetPosition()+Position;
			vec2 VertexScreenPosition = ViewMap()->MapRenderer()->MapPosToScreenPos(VertexPosition);
			if(distance(VertexScreenPosition, PickPosition) < 16.0f)
			{
				m_CurrentVertex = ASSET::SubPath_ObjectVertex((*Iter).GetId(), i);
				return VertexPosition;
			}
		}
	}
	
	return 0.0f;
}

template<typename ASSET>
bool CCursorTool_MapVertexEditor::PickNewVertex_Objects_Impl(vec2 PickPosition, vec2& VertexPosition, CSubPath& PreviousVertex, CSubPath& NextVertex)
{
	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return false;
	
	vec2 MouseMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(PickPosition);
	
	bool Found = false;
	vec2 NearestPosition = MouseMapPos + vec2(64.0f, 64.0f);
	CSubPath NearestSubPath0;
	CSubPath NearestSubPath1;
	
	//Iterator over all polygons
	typename ASSET::CIteratorObject Iter;
	for(Iter = pMapLayer->BeginObject(); Iter != pMapLayer->EndObject(); ++Iter)
	{
		const typename ASSET::CObject& Object = pMapLayer->GetObject(*Iter);
		vec2 Position;
		matrix2x2 Transform;
		Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
		
		//Iterate over all edges
		int FirstVertex = (Object.GetPathType() == ASSET::PATHTYPE_CLOSED) ? 0 : 1;
		for(int i=FirstVertex; i<Object.GetVertexArraySize(); i++)
		{
			CSubPath Vertex0Path;
			CSubPath Vertex1Path;
			Vertex0Path.SetId((i+Object.GetVertexArraySize()-1)%Object.GetVertexArraySize());
			Vertex1Path.SetId(i);
			
			
			vec2 Position0 = Transform*Object.GetVertexPosition(Vertex0Path)+Position;
			vec2 Position1 = Transform*Object.GetVertexPosition(Vertex1Path)+Position;
			
			vec2 ScreenPos;
			
			ScreenPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Position0);
			if(distance(PickPosition, ScreenPos) < 16.0f)
				continue;
				
			ScreenPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Position1);
			if(distance(PickPosition, ScreenPos) < 16.0f)
				continue;
			
			vec2 Dir = normalize(Position1 - Position0);
			float PosOnLine = dot(MouseMapPos - Position0, Dir);
			
			if(PosOnLine < 0.0f)
				continue;
			if(PosOnLine > length(Position1 - Position0))
				continue;
			
			vec2 MouseProj = Position0 + Dir * PosOnLine;
			if(distance(MouseProj, MouseMapPos) < distance(NearestPosition, MouseMapPos))
			{
				ScreenPos = ViewMap()->MapRenderer()->MapPosToScreenPos(MouseProj);
				if(distance(ScreenPos, PickPosition) < 32.0f)
				{
					NearestPosition = MouseProj;
					NearestSubPath0 = ASSET::SubPath_ObjectVertex((*Iter).GetId(), Vertex0Path.GetId());
					NearestSubPath1 = ASSET::SubPath_ObjectVertex((*Iter).GetId(), Vertex1Path.GetId());
					Found = true;
				}
			}
		}
	}
	
	if(Found)
	{
		VertexPosition = NearestPosition;
		PreviousVertex = NearestSubPath0;
		NextVertex = NearestSubPath1;
		return true;
	}
	else
		return false;
}

template<typename ASSET>
void CCursorTool_MapVertexEditor::OnViewButtonClick_Objects_Impl(int Button)
{
	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	
	if(pMapLayer->IsValidObject(m_CurrentVertex))
	{
		const typename ASSET::CObject& Object = pMapLayer->GetObject(ASSET::SubPath_Object(m_CurrentVertex.GetId()));
		vec2 Position;
		matrix2x2 Transform;
		Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
		
		vec2 VertexPos = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_POSITION, 0.0f);
		vec2 VertexControlPoint0 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_CONTROLPOINT0, 0.0f);
		vec2 VertexControlPoint1 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_CONTROLPOINT1, 0.0f);
		int VertexType = AssetsManager()->GetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_SMOOTHNESS, CBezierVertex::TYPE_CORNER);
		
		if(VertexType != CBezierVertex::TYPE_CORNER && VertexType != CBezierVertex::TYPE_AUTOSMOOTH)
		{
			ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
			
			if(VertexType == CBezierVertex::TYPE_CONTROLLED_SYMETRIC)
			{
				VertexControlPoint0 = (VertexControlPoint0 - VertexControlPoint1)/2.0f;
				VertexControlPoint1 = -VertexControlPoint0;
			}
			else if(VertexType == CBezierVertex::TYPE_CONTROLLED_ALIGNED)
			{
				float Length0 = length(VertexControlPoint0);
				float Length1 = length(VertexControlPoint1);
				vec2 Dir = normalize(VertexControlPoint0 - VertexControlPoint1);
				VertexControlPoint0 = Dir * Length0;
				VertexControlPoint1 = -Dir * Length1;
			}
			
			VertexControlPoint0 = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*(VertexControlPoint0+VertexPos) + Position);
			VertexControlPoint1 = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*(VertexControlPoint1+VertexPos) + Position);
			VertexPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*VertexPos + Position);
			
			float GizmoSize = 8.0f;
			
			if(distance(MousePos, VertexControlPoint0) <= GizmoSize)
			{
				m_Token = AssetsManager()->GenerateToken();
				m_DragType = 2;
			}
			else if(distance(MousePos, VertexControlPoint1) <= GizmoSize)
			{
				m_Token = AssetsManager()->GenerateToken();
				m_DragType = 3;
			}
			else
				m_DragType = 0;
		}
	}
	
	if(m_DragType == 0)
	{
		vec2 VertexPosition = PickVertex_Objects_Impl<ASSET>(MousePos);
		if(m_CurrentVertex.IsNotNull())
		{
			m_Token = AssetsManager()->GenerateToken();
			AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex);
			m_ClickDiff = MousePos - ViewMap()->MapRenderer()->MapPosToScreenPos(VertexPosition);
			m_DragType = 1;
		}
	}
	
	if(m_DragType == 0)
	{
		vec2 NewVertexPos;
		CSubPath PrevVertexPath;
		CSubPath NextVertexPath;
		
		if(PickNewVertex_Objects_Impl<ASSET>(MousePos, NewVertexPos, PrevVertexPath, NextVertexPath))
		{
			const typename ASSET::CObject& Object = pMapLayer->GetObject(ASSET::SubPath_Object(PrevVertexPath.GetId()));
			vec2 Position;
			matrix2x2 Transform;
			Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
			matrix2x2 InvTransform = matrix2x2::inverse(Transform);
			
			NewVertexPos = InvTransform*(NewVertexPos - Position);
			
			m_Token = AssetsManager()->GenerateToken();
			
			CSubPath ObjectPath = ASSET::SubPath_Object(NextVertexPath.GetId());
			int Index = NextVertexPath.GetId2();
			AssetsManager()->AddSubItemAt(AssetsEditor()->GetEditedAssetPath(), ObjectPath, ASSET::TYPE_OBJECT_VERTEX, Index, m_Token);
			m_CurrentVertex = ASSET::SubPath_ObjectVertex(NextVertexPath.GetId(), Index);
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_POSITION, NewVertexPos, m_Token);
			
			CAssetState* pState = AssetsManager()->GetAssetState(AssetsEditor()->GetEditedAssetPath());
			pState->m_NumUpdates++;
			
			AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex);
			m_ClickDiff = 0.0f;
			m_DragType = 1;
		}
	}
}

void CCursorTool_MapVertexEditor::OnViewButtonClick_Quads_Impl(int Button)
{
	const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	//Find gizmo
	float GizmoSize = 16.0f;
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);

	CSubPath SelectedQuad = AssetsEditor()->GetFirstEditedSubPath();
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
	
	CSubPath QuadFound = Pick(CursorPos);
		
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
}

void CCursorTool_MapVertexEditor::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());

	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
		OnViewButtonClick_Quads_Impl(Button);
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		OnViewButtonClick_Objects_Impl<CAsset_MapLayerObjects>(Button);
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		OnViewButtonClick_Objects_Impl<CAsset_MapZoneObjects>(Button);

	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapVertexEditor::OnViewButtonRelease(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button == KEY_MOUSE_1)
	{
		m_DragType = 0;
		m_Vertex = CAsset_MapLayerQuads::VERTEX_NONE;
		m_Token = CAssetsHistory::NEW_TOKEN;
	}
}

template<typename ASSET>
void CCursorTool_MapVertexEditor::OnViewMouseMove_Objects_Impl()
{
	if(m_DragType <= 0)
		return;
	
	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer || !pMapLayer->IsValidObjectVertex(m_CurrentVertex))
		return;
	
	const typename ASSET::CObject& Object = pMapLayer->GetObject(ASSET::SubPath_Object(m_CurrentVertex.GetId()));
	vec2 Position;
	matrix2x2 Transform;
	Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
	matrix2x2 InvTransform = matrix2x2::inverse(Transform);
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos - m_ClickDiff);
	if(ViewMap()->GetGridAlign())
	{
		CursorMapPos = ViewMap()->MapRenderer()->RoundMapPosToTile(CursorMapPos);
	}
	vec2 NewPos = InvTransform*(CursorMapPos - Position);
	
	if(m_DragType == 1)
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_POSITION, NewPos, m_Token);
	else if(m_DragType == 2)
	{
		vec2 VertexPos = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_POSITION, 0.0f);
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_CONTROLPOINT0, NewPos - VertexPos, m_Token);
	}
	else if(m_DragType == 3)
	{
		vec2 VertexPos = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_POSITION, 0.0f);
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_CONTROLPOINT1, NewPos - VertexPos, m_Token);
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}

void CCursorTool_MapVertexEditor::OnViewMouseMove()
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
	{
		const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return;
		
		CSubPath SelectedQuad = AssetsEditor()->GetFirstEditedSubPath();
		if(SelectedQuad.GetType() != CAsset_MapLayerQuads::TYPE_QUAD || !pMapLayer->IsValidQuad(SelectedQuad))
			return;
		
		ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
		
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos - m_ClickDiff);
		
		if(ViewMap()->GetGridAlign())
		{
			CursorMapPos = ViewMap()->MapRenderer()->RoundMapPosToTile(CursorMapPos);
		}
		
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
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		OnViewMouseMove_Objects_Impl<CAsset_MapLayerObjects>();
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		OnViewMouseMove_Objects_Impl<CAsset_MapZoneObjects>();
}
	
template<typename ASSET>
void CCursorTool_MapVertexEditor::RenderView_Objects_Impl()
{
	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	if(pMapLayer->IsValidObjectVertex(m_CurrentVertex))
	{
		const typename ASSET::CObject& Object = pMapLayer->GetObject(ASSET::SubPath_Object(m_CurrentVertex.GetId()));
		vec2 Position;
		matrix2x2 Transform;
		Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
		
		ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
		
		RenderPivots();
		RenderVertices();
		
		//Render control points
		vec2 VertexPos = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_POSITION, 0.0f);
		vec2 VertexControlPoint0 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_CONTROLPOINT0, 0.0f);
		vec2 VertexControlPoint1 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_CONTROLPOINT1, 0.0f);
		int VertexType = AssetsManager()->GetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, ASSET::OBJECT_VERTEX_SMOOTHNESS, CBezierVertex::TYPE_CORNER);
			
		if(VertexType != CBezierVertex::TYPE_CORNER && VertexType != CBezierVertex::TYPE_AUTOSMOOTH)
		{
			if(VertexType == CBezierVertex::TYPE_CONTROLLED_SYMETRIC)
			{
				VertexControlPoint0 = (VertexControlPoint0 - VertexControlPoint1)/2.0f;
				VertexControlPoint1 = -VertexControlPoint0;
			}
			else if(VertexType == CBezierVertex::TYPE_CONTROLLED_ALIGNED)
			{
				float Length0 = length(VertexControlPoint0);
				float Length1 = length(VertexControlPoint1);
				vec2 Dir = normalize(VertexControlPoint0 - VertexControlPoint1);
				VertexControlPoint0 = Dir * Length0;
				VertexControlPoint1 = -Dir * Length1;
			}
			
			VertexControlPoint0 = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*(VertexControlPoint0 + VertexPos) + Position);
			VertexControlPoint1 = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*(VertexControlPoint1 + VertexPos) + Position);
			VertexPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*VertexPos + Position);
			
			if(Object.GetPathType() == ASSET::PATHTYPE_CLOSED || m_CurrentVertex.GetId2() > 0)
			{
				Graphics()->TextureClear();
				Graphics()->LinesBegin();
				CGraphics::CLineItem LineItem(VertexControlPoint0.x, VertexControlPoint0.y, VertexPos.x, VertexPos.y);
				Graphics()->LinesDraw(&LineItem, 1);
				Graphics()->LinesEnd();
				
				AssetsRenderer()->DrawSprite(
					AssetsEditor()->m_Path_Sprite_GizmoVertexControl,
					VertexControlPoint0,
					1.0f, angle(VertexControlPoint0 - VertexPos), 0x0, 1.0f
				);
			}
			
			if(Object.GetPathType() == ASSET::PATHTYPE_CLOSED || m_CurrentVertex.GetId2() < Object.GetVertexArraySize()-1)
			{
				Graphics()->TextureClear();
				Graphics()->LinesBegin();
				CGraphics::CLineItem LineItem(VertexPos.x, VertexPos.y, VertexControlPoint1.x, VertexControlPoint1.y);
				Graphics()->LinesDraw(&LineItem, 1);
				Graphics()->LinesEnd();
				
				AssetsRenderer()->DrawSprite(
					AssetsEditor()->m_Path_Sprite_GizmoVertexControl,
					VertexControlPoint1,
					1.0f, angle(VertexControlPoint1 - VertexPos), 0x0, 1.0f
				);
			}
		}
	}
	else
	{
		RenderPivots();
		RenderVertices();
	}
	
	//Render new vertex ghost
	if(m_DragType <= 0)
	{
		vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		vec2 NewVertexPos;
		CSubPath PrevVertexPath;
		CSubPath NextVertexPath;
		
		if(PickNewVertex_Objects_Impl<ASSET>(MousePos, NewVertexPos, PrevVertexPath, NextVertexPath))
		{
			const typename ASSET::CObject& Object = pMapLayer->GetObject(ASSET::SubPath_Object(NextVertexPath.GetId()));
			vec2 Position;
			matrix2x2 Transform;
			Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
			
			vec2 Vertex0Pos = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), PrevVertexPath, ASSET::OBJECT_VERTEX_POSITION, 0.0f);
			vec2 Vertex1Pos = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), NextVertexPath, ASSET::OBJECT_VERTEX_POSITION, 0.0f);
			
			Vertex0Pos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*Vertex0Pos + Position);
			Vertex1Pos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*Vertex1Pos + Position);
			
			AssetsRenderer()->DrawSprite(
				AssetsEditor()->m_Path_Sprite_GizmoVertexSmoothBg,
				ViewMap()->MapRenderer()->MapPosToScreenPos(NewVertexPos),
				1.0f, 0.0f, 0x0, 1.0f
			);
		}
	}
}

void CCursorTool_MapVertexEditor::RenderView()
{
	CViewMap::ScopedGroupSetter GroupSetter(ViewMap());
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
	{
		const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return;
		
		RenderPivots();
		
		CSubPath SelectedQuad = AssetsEditor()->GetFirstEditedSubPath();
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
				AssetsEditor()->m_Path_Sprite_GizmoVertexCornerBg,
				vec2(VertexScreenPos.x, VertexScreenPos.y),
				1.0f, 0.0, 0x0, 1.0f
			);
			
			VertexColor.a = 1.0f;
			
			AssetsRenderer()->DrawSprite(
				AssetsEditor()->m_Path_Sprite_GizmoVertexCornerFg,
				vec2(VertexScreenPos.x, VertexScreenPos.y),
				1.0f, 0.0, 0x0, VertexColor
			);
		}
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		RenderView_Objects_Impl<CAsset_MapLayerObjects>();
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		RenderView_Objects_Impl<CAsset_MapZoneObjects>();
}
	
void CCursorTool_MapVertexEditor::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerQuads::TypeId:
		case CAsset_MapLayerObjects::TypeId:
		case CAsset_MapZoneObjects::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	if(ParentEnabled && IsEnabled() && IsEditable())
	{
		if(AssetsEditor()->m_BindCall_ToolVertex > 0)
		{
			AssetsEditor()->m_BindCall_ToolVertex = 0;
			ViewMap()->SetCursorTool(this);
		}
	}
	
	CCursorTool::Update(ParentEnabled);
}

void CCursorTool_MapVertexEditor::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_LSTRING("Vertex Editor: Change an object’s shape by moving vertices"));
	
	CCursorTool_MapPicker::OnMouseMove();
}
