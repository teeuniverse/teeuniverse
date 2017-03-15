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

#include <editor/gui/view_map_transform.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/maprenderer.h>
#include <shared/geometry/geometry.h>
#include <generated/assets/maplayerquads.h>
#include <client/gui/toggle.h>

/* CURSORTOOL MAP TRANSFORM *******************************************/

CCursorTool_MapTransform::CCursorTool_MapTransform(CViewMap* pViewMap) :
	CCursorTool_MapPicker(pViewMap, _LSTRING("Transform"), pViewMap->AssetsEditor()->m_Path_Sprite_IconMove),
	m_Token(CAssetsHistory::NEW_TOKEN)
{
	
}
	
template<typename ASSET>
void CCursorTool_MapTransform::OnViewButtonClick_Objects_Impl(int Button)
{
	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
		
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	//Find gizmo
	float GizmoSize = 16.0f;
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	
	CSubPath SelectedObject = AssetsEditor()->GetFirstEditedSubPath();
	if(SelectedObject.GetType() == ASSET::TYPE_OBJECT && pMapLayer->IsValidObject(SelectedObject))
	{
		if(pMapLayer->GetObjectVertexArraySize(SelectedObject) > 0)
		{
			const typename ASSET::CObject& Object = pMapLayer->GetObject(SelectedObject);
			const std::vector<typename ASSET::CVertex>& Vertices = Object.GetVertexArray();
			vec2 Position;
			matrix2x2 Transform;
			Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
			
			vec2 BBMin = Vertices[0].GetPosition();
			vec2 BBMax = BBMin;
			
			for(unsigned int i=1; i<Vertices.size(); i++)
			{
				vec2 VertexPosition = Vertices[i].GetPosition();
				UpdateBoundingBox(VertexPosition, &BBMin, &BBMax);
			}
			
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
	}
		
	CSubPath ObjectFound = Pick(CursorPos);
	
	if(!ObjectFound.IsNull())
	{
		if(SelectedObject != ObjectFound)
		{
			SelectedObject = ObjectFound;
			AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), SelectedObject);
			
			m_GizmoType = GIZMOTYPE_ROTATION;
			m_Transformed = true;
		}
		else
			m_Transformed = false;
		
		m_Token = AssetsManager()->GenerateToken();
		m_DragType = DRAGTYPE_TRANSLATION;
		
		vec2 PivotMapPos = pMapLayer->GetObjectPosition(SelectedObject);
		vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(vec2(Context()->GetMousePos().x, Context()->GetMousePos().y));
		m_ClickDiff = CursorMapPos - PivotMapPos;
		
		AssetsEditor()->SetEditedAsset(
			AssetsEditor()->GetEditedAssetPath(),
			SelectedObject
		);
	}
	else
		AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null());
		
	ViewMap()->MapRenderer()->UnsetGroup();
}

void CCursorTool_MapTransform::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
	{
		const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return;
		
		ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
		
		//Find gizmo
		float GizmoSize = 16.0f;
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		
		CSubPath SelectedQuad = AssetsEditor()->GetFirstEditedSubPath();
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
		
		CSubPath QuadFound = Pick(CursorPos);
		
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
			
			vec2 PivotMapPos = pMapLayer->GetQuadPivot(SelectedQuad);
			vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(vec2(Context()->GetMousePos().x, Context()->GetMousePos().y));
			m_ClickDiff = CursorMapPos - PivotMapPos;
			
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
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		OnViewButtonClick_Objects_Impl<CAsset_MapZoneObjects>(Button);
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		OnViewButtonClick_Objects_Impl<CAsset_MapLayerObjects>(Button);
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapEntities::TypeId)
	{
		const CAsset_MapEntities* pEntities = AssetsManager()->GetAsset<CAsset_MapEntities>(AssetsEditor()->GetEditedAssetPath());
		if(!pEntities)
			return;
		
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		CSubPath EntityFound = Pick(CursorPos);
		
		if(!EntityFound.IsNull())
		{
			m_Token = AssetsManager()->GenerateToken();
			m_DragType = DRAGTYPE_TRANSLATION;
			
			vec2 PivotMapPos = pEntities->GetEntityPosition(EntityFound);
			vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(vec2(Context()->GetMousePos().x, Context()->GetMousePos().y));
			m_ClickDiff = CursorMapPos - PivotMapPos;
			
			AssetsEditor()->SetEditedAsset(
				AssetsEditor()->GetEditedAssetPath(),
				EntityFound
			);
		}
		else
			AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null());
		
		return;
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapGroup::TypeId)
	{
		const CAsset_MapGroup* pMapGroup = AssetsManager()->GetAsset<CAsset_MapGroup>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapGroup)
			return;
		
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
	
template<typename ASSET>
void CCursorTool_MapTransform::OnViewMouseMove_Objects_Impl()
{
	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	CSubPath SelectedObject = AssetsEditor()->GetFirstEditedSubPath();
	if(SelectedObject.GetType() != ASSET::TYPE_OBJECT || !pMapLayer->IsValidObject(SelectedObject))
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	const typename ASSET::CObject& Object = pMapLayer->GetObject(SelectedObject);
	vec2 Position;
	matrix2x2 Transform;
	Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
	
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	
	if(m_DragType == DRAGTYPE_TRANSLATION)
	{
		ivec2 RelPos = Context()->GetMouseRelPos();
		if(RelPos.x != 0 || RelPos.y != 0)
		{
			vec2 NewPivotPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos) - m_ClickDiff;
		
			if(ViewMap()->GetGridAlign())
			{
				NewPivotPos = ViewMap()->MapRenderer()->MapPosToTilePos(NewPivotPos);
				NewPivotPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(NewPivotPos.x), floor(NewPivotPos.y))) + vec2(16.0f, 16.0f);
			}
			
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedObject, ASSET::OBJECT_POSITION, NewPivotPos, m_Token);
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
		
		const std::vector<typename ASSET::CVertex>& Vertices = Object.GetVertexArray();
		
		vec2 BBMin = Vertices[0].GetPosition();
		vec2 BBMax = BBMin;
		
		for(unsigned int i=1; i<Vertices.size(); i++)
		{
			vec2 VertexPosition = Vertices[i].GetPosition();
			UpdateBoundingBox(VertexPosition, &BBMin, &BBMax);
		}
		
		vec2 GizmosPos = Position;
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
			float Angle = pMapLayer->GetObjectAngle(SelectedObject);
			Angle -= angle(normalize(MousePos - PivotPos), normalize(GizmosPos - PivotPos));
			AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), SelectedObject, ASSET::OBJECT_ANGLE, Angle, m_Token);
		}
		else
		{
			vec2 Size = pMapLayer->GetObjectSize(SelectedObject);
			
			float Ratio = length(MousePos - PivotPos)/length(GizmosPos - PivotPos);
			Size *= Ratio;
			
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedObject, ASSET::OBJECT_SIZE, Size, m_Token);
		}
		
		m_Transformed = true;
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
	
	return;
}
	
void CCursorTool_MapTransform::OnViewMouseMove()
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
		
		vec2 Position;
		matrix2x2 Transform;
		pMapLayer->GetQuadTransform(SelectedQuad, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
		
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		
		if(m_DragType == DRAGTYPE_TRANSLATION)
		{
			ivec2 RelPos = Context()->GetMouseRelPos();
			if(RelPos.x != 0 || RelPos.y != 0)
			{
				vec2 NewPivotPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos) - m_ClickDiff;
			
				if(ViewMap()->GetGridAlign())
				{
					NewPivotPos = ViewMap()->MapRenderer()->MapPosToTilePos(NewPivotPos);
					NewPivotPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(NewPivotPos.x), floor(NewPivotPos.y))) + vec2(16.0f, 16.0f);
				}
				
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
			
			vec2 GizmosPos = Position;
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
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		OnViewMouseMove_Objects_Impl<CAsset_MapZoneObjects>();
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		OnViewMouseMove_Objects_Impl<CAsset_MapLayerObjects>();
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapEntities::TypeId)
	{
		const CAsset_MapEntities* pEntities = AssetsManager()->GetAsset<CAsset_MapEntities>(AssetsEditor()->GetEditedAssetPath());
		if(!pEntities)
			return;
		
		CSubPath SelectedEntity = AssetsEditor()->GetFirstEditedSubPath();
		if(SelectedEntity.GetType() != CAsset_MapEntities::TYPE_ENTITY || !pEntities->IsValidEntity(SelectedEntity))
			return;
		
		ViewMap()->MapRenderer()->UnsetGroup();
		
		vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		
		if(m_DragType == DRAGTYPE_TRANSLATION)
		{
			vec2 NewPosition = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos) - m_ClickDiff;
		
			if(ViewMap()->GetGridAlign())
			{
				NewPosition = ViewMap()->MapRenderer()->MapPosToTilePos(NewPosition);
				NewPosition = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(NewPosition.x), floor(NewPosition.y))) + vec2(16.0f, 16.0f);
			}
			
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), SelectedEntity, CAsset_MapEntities::ENTITY_POSITION, NewPosition, m_Token);
			m_Transformed = true;
		}
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapGroup::TypeId)
	{
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
}
	
template<typename ASSET>
void CCursorTool_MapTransform::RenderView_Objects_Impl()
{
	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	CSubPath SelectedObject = AssetsEditor()->GetFirstEditedSubPath();
	if(SelectedObject.GetType() == ASSET::TYPE_OBJECT && pMapLayer->IsValidObject(SelectedObject) && pMapLayer->GetObjectVertexArraySize(SelectedObject) > 0)
	{
		const typename ASSET::CObject& Object = pMapLayer->GetObject(SelectedObject);
		const std::vector<typename ASSET::CVertex>& Vertices = Object.GetVertexArray();
		vec2 Position;
		matrix2x2 Transform;
		Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
		
		vec2 BBMin = Vertices[0].GetPosition();
		vec2 BBMax = BBMin;
		
		for(unsigned int i=1; i<Vertices.size(); i++)
		{
			vec2 VertexPosition = Vertices[i].GetPosition();
			UpdateBoundingBox(VertexPosition, &BBMin, &BBMax);
		}
		
		vec2 Vertex0Pos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform * vec2(BBMin.x, BBMin.y) + Position);
		vec2 Vertex1Pos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform * vec2(BBMax.x, BBMin.y) + Position);
		vec2 Vertex2Pos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform * vec2(BBMin.x, BBMax.y) + Position);
		vec2 Vertex3Pos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform * vec2(BBMax.x, BBMax.y) + Position);
		
		float GizmoAngle = angle(vec2(1.0f, 0.0f), Transform * vec2(1.0f, 0.0f));
		
		CAssetPath GizmoPath;
		if(m_GizmoType == GIZMOTYPE_ROTATION)
			GizmoPath = AssetsEditor()->m_Path_Sprite_GizmoRotate;
		else if(m_GizmoType == GIZMOTYPE_SCALE)
			GizmoPath = AssetsEditor()->m_Path_Sprite_GizmoScale;
		
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(Vertex0Pos.x, Vertex0Pos.y), 1.0f, GizmoAngle, 0x0, 1.0f);
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(Vertex1Pos.x, Vertex1Pos.y), 1.0f, GizmoAngle+Pi/2.0f, 0x0, 1.0f);
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(Vertex2Pos.x, Vertex2Pos.y), 1.0f, GizmoAngle-Pi/2.0f, 0x0, 1.0f);
		AssetsRenderer()->DrawSprite(GizmoPath, vec2(Vertex3Pos.x, Vertex3Pos.y), 1.0f, GizmoAngle+Pi, 0x0, 1.0f);
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapTransform::RenderView()
{
	RenderPivots();
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
	{
		const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return;
			
		ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
		
		CSubPath SelectedQuad = AssetsEditor()->GetFirstEditedSubPath();
		if(SelectedQuad.GetType() == CAsset_MapLayerQuads::TYPE_QUAD && pMapLayer->IsValidQuad(SelectedQuad))
		{
			ViewMap()->MapRenderer()->RenderQuads_Mesh(&pMapLayer->GetQuad(SelectedQuad), 1);

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
			AssetsRenderer()->DrawSprite(GizmoPath, vec2(Vertex1Pos.x, Vertex1Pos.y), 1.0f, GizmoAngle+Pi/2.0f, 0x0, 1.0f);
			AssetsRenderer()->DrawSprite(GizmoPath, vec2(Vertex2Pos.x, Vertex2Pos.y), 1.0f, GizmoAngle-Pi/2.0f, 0x0, 1.0f);
			AssetsRenderer()->DrawSprite(GizmoPath, vec2(Vertex3Pos.x, Vertex3Pos.y), 1.0f, GizmoAngle+Pi, 0x0, 1.0f);
		}
		
		ViewMap()->MapRenderer()->UnsetGroup();
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		RenderView_Objects_Impl<CAsset_MapLayerObjects>();
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		RenderView_Objects_Impl<CAsset_MapZoneObjects>();
}
	
void CCursorTool_MapTransform::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapGroup::TypeId:
		case CAsset_MapZoneObjects::TypeId:
		case CAsset_MapLayerObjects::TypeId:
		case CAsset_MapLayerQuads::TypeId:
		case CAsset_MapEntities::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	if(ParentEnabled && IsEnabled() && IsEditable())
	{
		if(AssetsEditor()->m_BindCall_ToolTransform > 0)
		{
			AssetsEditor()->m_BindCall_ToolTransform = 0;
			ViewMap()->SetCursorTool(this);
		}
	}
	
	CCursorTool::Update(ParentEnabled);
}

void CCursorTool_MapTransform::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_LSTRING("Transform Tool: Move, rotate and scale objects"));
	
	CCursorTool_MapPicker::OnMouseMove();
}
