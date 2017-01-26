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

#include <editor/gui/view_map_object.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/maprenderer.h>
#include <shared/geometry/geometry.h>
#include <generated/assets/maplayerobjects.h>

/* CURSORTOOL OBJECT VERTEX PICKER ************************************/

CCursorTool_MapObjectVertexPicker::CCursorTool_MapObjectVertexPicker(CViewMap* pViewMap, const CLocalizableString& LString, CAssetPath IconPath) :
	CViewMap::CCursorTool(pViewMap, LString, IconPath)
{
	
}
	
void CCursorTool_MapObjectVertexPicker::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerObjects::TypeId:
		case CAsset_MapZoneObjects::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	CCursorTool::Update(ParentEnabled);
}

/* CURSORTOOL OBJECT ADD VERTEX ***************************************/

CCursorTool_MapObjectAddVertex::CCursorTool_MapObjectAddVertex(CViewMap* pViewMap) :
	CCursorTool_MapObjectVertexPicker(pViewMap, _LSTRING("Line Drawer"), pViewMap->AssetsEditor()->m_Path_Sprite_IconPencil)
{
	
}

void CCursorTool_MapObjectAddVertex::Reset()
{
	m_CurrentObject = CSubPath::Null();
	m_CurrentAssetPath = CAssetPath::Null();
}

void CCursorTool_MapObjectAddVertex::UpdateBarycenter(int Token)
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
	{
		const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return;
		
		if(pMapLayer->IsValidObject(m_CurrentObject))
		{
			const CAsset_MapLayerObjects::CObject& Object = pMapLayer->GetObject(m_CurrentObject);
			
			vec2 Position;
			matrix2x2 Transform;
			Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
			
			vec2 Barycenter = 0.0f;
			
			for(int i=0; i<Object.GetVertexArraySize(); i++)
			{
				CSubPath VertexPath = CAsset_MapLayerObjects::SubPath_ObjectVertex(m_CurrentObject.GetId(), i);
				vec2 Position0 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), VertexPath, CAsset_MapLayerObjects::OBJECT_VERTEX_POSITION, 0.0f);
				Barycenter += Position0;
			}
			
			if(Object.GetVertexArraySize())
				Barycenter /= Object.GetVertexArraySize();
			
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentObject, CAsset_MapLayerObjects::OBJECT_POSITION, Transform*Barycenter + Position, Token);
			
			for(int i=0; i<Object.GetVertexArraySize(); i++)
			{
				CSubPath VertexPath = CAsset_MapLayerObjects::SubPath_ObjectVertex(m_CurrentObject.GetId(), i);
				vec2 Position0 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), VertexPath, CAsset_MapLayerObjects::OBJECT_VERTEX_POSITION, 0.0f);
				Position0 = Position0 - Barycenter;
				AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), VertexPath, CAsset_MapLayerObjects::OBJECT_VERTEX_POSITION, Position0, Token);
			}
		}
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
	{
		const CAsset_MapZoneObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapZoneObjects>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return;
		
		if(pMapLayer->IsValidObject(m_CurrentObject))
		{
			const CAsset_MapZoneObjects::CObject& Object = pMapLayer->GetObject(m_CurrentObject);
			
			vec2 Position;
			matrix2x2 Transform;
			Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
			
			vec2 Barycenter = 0.0f;
			
			for(int i=0; i<Object.GetVertexArraySize(); i++)
			{
				CSubPath VertexPath = CAsset_MapZoneObjects::SubPath_ObjectVertex(m_CurrentObject.GetId(), i);
				vec2 Position0 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), VertexPath, CAsset_MapZoneObjects::OBJECT_VERTEX_POSITION, 0.0f);
				Barycenter += Position0;
			}
			
			if(Object.GetVertexArraySize())
				Barycenter /= Object.GetVertexArraySize();
			
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentObject, CAsset_MapZoneObjects::OBJECT_POSITION, Transform*Barycenter + Position, Token);
			
			for(int i=0; i<Object.GetVertexArraySize(); i++)
			{
				CSubPath VertexPath = CAsset_MapZoneObjects::SubPath_ObjectVertex(m_CurrentObject.GetId(), i);
				vec2 Position0 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), VertexPath, CAsset_MapZoneObjects::OBJECT_VERTEX_POSITION, 0.0f);
				Position0 = Position0 - Barycenter;
				AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), VertexPath, CAsset_MapZoneObjects::OBJECT_VERTEX_POSITION, Position0, Token);
			}
		}
	}
}

template<typename ASSET>
void CCursorTool_MapObjectAddVertex::OnViewButtonClick_Impl(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button == KEY_MOUSE_2)
	{
		m_CurrentObject = CSubPath::Null();
		m_CurrentAssetPath = CAssetPath::Null();
		return;
	}
	
	if(Button != KEY_MOUSE_1)
		return;

	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	
	int Token = AssetsManager()->GenerateToken();
	if(m_CurrentObject.IsNull())
	{
		m_CurrentObject = ASSET::SubPath_Object(AssetsManager()->AddSubItem(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), ASSET::TYPE_OBJECT, Token));			
		m_CurrentAssetPath = AssetsEditor()->GetEditedAssetPath();
		AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), m_CurrentObject);
	}
	
	if(pMapLayer->IsValidObject(m_CurrentObject))
	{
		vec2 Position;
		matrix2x2 Transform;
		pMapLayer->GetObjectTransform(m_CurrentObject, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
		matrix2x2 TransformInv = matrix2x2::inverse(Transform);
		
		CSubPath VertexPath;
		bool ClosePath = false;
		
		const typename ASSET::CObject& Object = pMapLayer->GetObject(m_CurrentObject);
		if(Object.GetVertexArraySize())
		{
			VertexPath.SetId(0);
			vec2 StartPosition = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*Object.GetVertexPosition(VertexPath) + Position);
			if(distance(StartPosition, MousePos) < 16)
				ClosePath = true;
		}
		
		if(ClosePath)
		{
			AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), m_CurrentObject, ASSET::OBJECT_PATHTYPE, ASSET::PATHTYPE_CLOSED, Token);
			m_CurrentObject = CSubPath::Null();
			m_CurrentAssetPath = CAssetPath::Null(); 
		}
		else
		{
			VertexPath = ASSET::SubPath_ObjectVertex(
				m_CurrentObject.GetId(),
				AssetsManager()->AddSubItem(AssetsEditor()->GetEditedAssetPath(), m_CurrentObject, ASSET::TYPE_OBJECT_VERTEX, Token)
			);
			
			vec2 VertexPositon = TransformInv*(ViewMap()->MapRenderer()->ScreenPosToMapPos(MousePos) - Position);
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), VertexPath, ASSET::OBJECT_VERTEX_POSITION, VertexPositon, Token);
			UpdateBarycenter(Token);
		}
		
		CAssetState* pState = AssetsManager()->GetAssetState(AssetsEditor()->GetEditedAssetPath());
		pState->m_NumUpdates++;
	}

	ViewMap()->MapRenderer()->UnsetGroup();
}

void CCursorTool_MapObjectAddVertex::OnViewButtonClick(int Button)
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		OnViewButtonClick_Impl<CAsset_MapLayerObjects>(Button);
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		OnViewButtonClick_Impl<CAsset_MapZoneObjects>(Button);
}
	
void CCursorTool_MapObjectAddVertex::OnViewButtonRelease(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
}
	
void CCursorTool_MapObjectAddVertex::OnViewMouseMove()
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
}

template<typename ASSET>
void CCursorTool_MapObjectAddVertex::RenderView_Impl()
{
	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(m_CurrentObject.IsNotNull() && pMapLayer->IsValidObject(m_CurrentObject))
	{
		vec2 Position;
		matrix2x2 Transform;
		pMapLayer->GetObjectTransform(m_CurrentObject, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
		
		CSubPath Vertex0Path = ASSET::SubPath_ObjectVertex(m_CurrentObject.GetId(), pMapLayer->GetObjectVertexArraySize(m_CurrentObject)-1);
		
		vec2 VertexPosition = pMapLayer->GetObjectVertexPosition(Vertex0Path);
		VertexPosition = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*VertexPosition+Position);
		vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		
		Graphics()->TextureClear();
		Graphics()->LinesBegin();
	
		CGraphics::CLineItem LineItem(VertexPosition.x, VertexPosition.y, MousePos.x, MousePos.y);
		Graphics()->LinesDraw(&LineItem, 1);
		
		Graphics()->LinesEnd();
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();

	RenderPivots<ASSET>();
	RenderVertices<ASSET>();
}

void CCursorTool_MapObjectAddVertex::RenderView()
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		RenderView_Impl<CAsset_MapLayerObjects>();
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		RenderView_Impl<CAsset_MapZoneObjects>();
}

void CCursorTool_MapObjectAddVertex::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_LSTRING("Line Drawer: Create lines and polygons"));
	
	CViewMap::CCursorTool::OnMouseMove();
}

void CCursorTool_MapObjectAddVertex::OnInputEvent(const CInput::CEvent& Event)
{
	if(Event.m_Key == KEY_RETURN || Event.m_Key == KEY_KP_ENTER)
	{
		m_CurrentObject = CSubPath::Null();
		m_CurrentAssetPath = CAssetPath::Null();
	}
	
	CViewMap::CCursorTool::OnInputEvent(Event);
}

/* CURSORTOOL OBJECT EDIT VERTEX **************************************/

CCursorTool_MapObjectEditVertex::CCursorTool_MapObjectEditVertex(CViewMap* pViewMap) :
	CCursorTool_MapObjectVertexPicker(pViewMap, _LSTRING("Vertex Editor"), pViewMap->AssetsEditor()->m_Path_Sprite_IconMoveVertex),
	m_DragType(0),
	m_Token(CAssetsHistory::NEW_TOKEN)
{
	
}

template<typename ASSET>
void CCursorTool_MapObjectEditVertex::RenderView_Impl()
{	
	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer || !pMapLayer->IsValidObjectVertex(m_CurrentVertex))
		return;
		
	const typename ASSET::CObject& Object = pMapLayer->GetObject(ASSET::SubPath_Object(m_CurrentVertex.GetId()));
	vec2 Position;
	matrix2x2 Transform;
	Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	RenderPivots<ASSET>();
	RenderVertices<ASSET>();
	
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
	
	//Render new vertex ghost
	if(m_DragType <= 0)
	{
		vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		vec2 NewVertexPos;
		CSubPath PrevVertexPath;
		CSubPath NextVertexPath;
		
		if(PickNewVertex<ASSET>(MousePos, NewVertexPos, PrevVertexPath, NextVertexPath))
		{
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
	
	ViewMap()->MapRenderer()->UnsetGroup();
}

void CCursorTool_MapObjectEditVertex::RenderView()
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		RenderView_Impl<CAsset_MapLayerObjects>();
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		RenderView_Impl<CAsset_MapZoneObjects>();
}

template<typename ASSET>
void CCursorTool_MapObjectEditVertex::OnViewButtonClick_Impl(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	
	if(m_CurrentVertex.IsNotNull())
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
				m_DragType = 2;
			else if(distance(MousePos, VertexControlPoint1) <= GizmoSize)
				m_DragType = 3;
			else
				m_DragType = 0;
		}
	}
	
	if(m_DragType == 0)
	{
		vec2 VertexPosition = PickVertex<ASSET>(MousePos);
		if(m_CurrentVertex.IsNotNull())
		{
			m_Token = AssetsManager()->GenerateToken();
			AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex);
			ClickDiff = MousePos - ViewMap()->MapRenderer()->MapPosToScreenPos(VertexPosition);
			m_DragType = 1;
		}
	}
	
	if(m_DragType == 0)
	{
		vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
		vec2 NewVertexPos;
		CSubPath PrevVertexPath;
		CSubPath NextVertexPath;
		
		if(PickNewVertex<ASSET>(MousePos, NewVertexPos, PrevVertexPath, NextVertexPath))
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
			
			AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex);
			ClickDiff = 0.0f;
			m_DragType = 1;
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}

void CCursorTool_MapObjectEditVertex::OnViewButtonClick(int Button)
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		OnViewButtonClick_Impl<CAsset_MapLayerObjects>(Button);
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		OnViewButtonClick_Impl<CAsset_MapZoneObjects>(Button);
}
	
void CCursorTool_MapObjectEditVertex::OnViewButtonRelease(int Button)
{
	if(Button == KEY_MOUSE_1)
	{
		m_DragType = 0;
		m_Token = CAssetsHistory::NEW_TOKEN;
	}
}

template<typename ASSET>
void CCursorTool_MapObjectEditVertex::OnViewMouseMove_Impl()
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
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
	
	vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	vec2 MapMousePos = ViewMap()->MapRenderer()->ScreenPosToMapPos(MousePos - ClickDiff);
	if(ViewMap()->GetGridAlign())
	{
		MapMousePos = ViewMap()->MapRenderer()->MapPosToTilePos(MapMousePos);
		MapMousePos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(MapMousePos.x), floor(MapMousePos.y)));
	}
	vec2 NewPos = InvTransform*(MapMousePos - Position);
	
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

void CCursorTool_MapObjectEditVertex::OnViewMouseMove()
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		OnViewMouseMove_Impl<CAsset_MapLayerObjects>();
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		OnViewMouseMove_Impl<CAsset_MapZoneObjects>();
}

void CCursorTool_MapObjectEditVertex::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_LSTRING("Vertex Editor: Move and shape vertices"));
	
	CViewMap::CCursorTool::OnMouseMove();
}

/* CURSORTOOL OBJECT WEIGHT VERTEX ************************************/

CCursorTool_MapObjectWeightVertex::CCursorTool_MapObjectWeightVertex(CViewMap* pViewMap) :
	CCursorTool_MapObjectVertexPicker(pViewMap, _LSTRING("Weight Editor"), pViewMap->AssetsEditor()->m_Path_Sprite_IconWeightVertex),
	m_Drag(false),
	m_Token(CAssetsHistory::NEW_TOKEN)
{
	
}
	
void CCursorTool_MapObjectWeightVertex::RenderView()
{
	RenderPivots<CAsset_MapLayerObjects>();
	RenderVertices<CAsset_MapLayerObjects>();
}

void CCursorTool_MapObjectWeightVertex::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(m_CurrentVertex.IsNotNull())
	{
		AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex);
		m_Token = AssetsManager()->GenerateToken();
		m_Drag = true;
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapObjectWeightVertex::OnViewButtonRelease(int Button)
{
	if(Button == KEY_MOUSE_1)
	{
		m_Drag = false;
		m_Token = CAssetsHistory::NEW_TOKEN;
	}
}
	
void CCursorTool_MapObjectWeightVertex::OnViewMouseMove()
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(!m_Drag)
		return;
	
	const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	float Weight = AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_WEIGHT, 1.0f);
	
	vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	float Diff = MousePos.x - ClickPos.x;
	float NewWeight = max(Weight + Diff/30.0f, 0.01f);
	
	AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_WEIGHT, NewWeight, m_Token);
	
	ViewMap()->MapRenderer()->UnsetGroup();
}

void CCursorTool_MapObjectWeightVertex::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_LSTRING("Weight Editor: Change the thickness of vertices"));
	
	CViewMap::CCursorTool::OnMouseMove();
}
