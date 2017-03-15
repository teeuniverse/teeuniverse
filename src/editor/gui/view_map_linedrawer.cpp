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

#include <editor/gui/view_map_linedrawer.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/maprenderer.h>
#include <shared/geometry/geometry.h>
#include <client/gui/toggle.h>

/* CURSORTOOL VERTEX EDITOR *******************************************/

CCursorTool_MapLineDrawer::CCursorTool_MapLineDrawer(CViewMap* pViewMap) :
	CCursorTool_MapPicker(pViewMap, _LSTRING("Line Drawer"), pViewMap->AssetsEditor()->m_Path_Sprite_IconPencil)
{
	
}

void CCursorTool_MapLineDrawer::Reset()
{
	m_CurrentObject = CSubPath::Null();
	m_CurrentAssetPath = CAssetPath::Null();
}

template<typename ASSET>
void CCursorTool_MapLineDrawer::UpdateBarycenter_Objects_Impl(int Token)
{
	const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	if(pMapLayer->IsValidObject(m_CurrentObject))
	{
		const typename ASSET::CObject& Object = pMapLayer->GetObject(m_CurrentObject);
		
		vec2 Position;
		matrix2x2 Transform;
		Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
		
		vec2 Barycenter = 0.0f;
		
		for(int i=0; i<Object.GetVertexArraySize(); i++)
		{
			CSubPath VertexPath = ASSET::SubPath_ObjectVertex(m_CurrentObject.GetId(), i);
			vec2 Position0 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), VertexPath, ASSET::OBJECT_VERTEX_POSITION, 0.0f);
			Barycenter += Position0;
		}
		
		if(Object.GetVertexArraySize())
			Barycenter /= Object.GetVertexArraySize();
		
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentObject, ASSET::OBJECT_POSITION, Transform*Barycenter + Position, Token);
		
		for(int i=0; i<Object.GetVertexArraySize(); i++)
		{
			CSubPath VertexPath = ASSET::SubPath_ObjectVertex(m_CurrentObject.GetId(), i);
			vec2 Position0 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), VertexPath, ASSET::OBJECT_VERTEX_POSITION, 0.0f);
			Position0 = Position0 - Barycenter;
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), VertexPath, ASSET::OBJECT_VERTEX_POSITION, Position0, Token);
		}
	}
}

template<typename ASSET>
void CCursorTool_MapLineDrawer::OnViewButtonClick_Objects_Impl(int Button)
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
	if(!pMapLayer->IsValidObject(m_CurrentObject))
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
			UpdateBarycenter_Objects_Impl<ASSET>(Token);
		}
		
		CAssetState* pState = AssetsManager()->GetAssetState(AssetsEditor()->GetEditedAssetPath());
		pState->m_NumUpdates++;
	}

	ViewMap()->MapRenderer()->UnsetGroup();
}

void CCursorTool_MapLineDrawer::OnViewButtonClick(int Button)
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		OnViewButtonClick_Objects_Impl<CAsset_MapLayerObjects>(Button);
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		OnViewButtonClick_Objects_Impl<CAsset_MapZoneObjects>(Button);
}
	
template<typename ASSET>
void CCursorTool_MapLineDrawer::RenderView_Objects_Impl()
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

	RenderPivots();
	RenderVertices();
}
	
void CCursorTool_MapLineDrawer::RenderView()
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		RenderView_Objects_Impl<CAsset_MapLayerObjects>();
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		RenderView_Objects_Impl<CAsset_MapZoneObjects>();
}
	
void CCursorTool_MapLineDrawer::Update(bool ParentEnabled)
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

void CCursorTool_MapLineDrawer::OnInputEvent(const CInput::CEvent& Event)
{
	if(Event.m_Key == KEY_RETURN || Event.m_Key == KEY_KP_ENTER)
	{
		m_CurrentObject = CSubPath::Null();
		m_CurrentAssetPath = CAssetPath::Null();
	}
	
	CViewMap::CCursorTool::OnInputEvent(Event);
}

void CCursorTool_MapLineDrawer::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_LSTRING("Line Drawer: Create lines and polygons"));
	
	CCursorTool_MapPicker::OnMouseMove();
}
