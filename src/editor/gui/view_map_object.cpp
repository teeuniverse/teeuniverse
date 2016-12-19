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

/* CURSORTOOL OBJECT ADD VERTEX ***************************************/

CCursorTool_MapObjectAddVertex::CCursorTool_MapObjectAddVertex(CViewMap* pViewMap) :
	CViewMap::CCursorTool(pViewMap, _LSTRING("Line Drawer"), pViewMap->AssetsEditor()->m_Path_Sprite_IconPencil)
{
	
}

void CCursorTool_MapObjectAddVertex::Reset()
{
	m_CurrentObject = CSubPath::Null();
	m_CurrentAssetPath = CAssetPath::Null();
}

void CCursorTool_MapObjectAddVertex::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	vec2 Positon = ViewMap()->MapRenderer()->ScreenPosToMapPos(MousePos);
	
	int Token = AssetsManager()->GenerateToken();
	if(m_CurrentObject.IsNull())
	{
		m_CurrentObject = CAsset_MapLayerObjects::SubPath_Object(AssetsManager()->AddSubItem(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerObjects::TYPE_OBJECT, Token));			
		m_CurrentAssetPath = AssetsEditor()->GetEditedAssetPath();
		AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), m_CurrentObject);
	}
	
	if(pMapLayer->IsValidObject(m_CurrentObject))
	{
		CSubPath VertexPath;
		bool ClosePath = false;
		
		const CAsset_MapLayerObjects::CObject& Object = pMapLayer->GetObject(m_CurrentObject);
		if(Object.GetVertexArraySize())
		{
			VertexPath.SetId(0);
			vec2 StartPosition = ViewMap()->MapRenderer()->MapPosToScreenPos(Object.GetVertexPosition(VertexPath));
			if(distance(StartPosition, MousePos) < 16)
				ClosePath = true;
		}
		
		if(ClosePath)
		{
			m_CurrentObject = CSubPath::Null();
			m_CurrentAssetPath = CAssetPath::Null(); 
		}
		else
		{
			VertexPath = CAsset_MapLayerObjects::SubPath_ObjectVertex(
				m_CurrentObject.GetId(),
				AssetsManager()->AddSubItem(AssetsEditor()->GetEditedAssetPath(), m_CurrentObject, CAsset_MapLayerObjects::TYPE_OBJECT_VERTEX, Token)
			);
			AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), VertexPath, CAsset_MapLayerObjects::OBJECT_VERTEX_POSITION, Positon, Token);
			AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), VertexPath, CAsset_MapLayerObjects::OBJECT_VERTEX_WEIGHT, 1.0f, Token);
		}
		
		CAssetState* pState = AssetsManager()->GetAssetState(AssetsEditor()->GetEditedAssetPath());
		pState->m_NumUpdates++;
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
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
	
void CCursorTool_MapObjectAddVertex::RenderView()
{
	const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
		
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	float GizmoSize = 16.0f;
	
	CAsset_MapLayerObjects::CIteratorObject Iter;
	for(Iter = pMapLayer->BeginObject(); Iter != pMapLayer->EndObject(); ++Iter)
	{
		const CAsset_MapLayerObjects::CObject& Object = pMapLayer->GetObject(*Iter);
		
		for(int i=0; i<Object.GetVertexArraySize(); i++)
		{
			CSubPath VertexPath;
			VertexPath.SetId(i);
			
			vec2 Position = ViewMap()->MapRenderer()->MapPosToScreenPos(Object.GetVertexPosition(VertexPath));			
			
			AssetsRenderer()->DrawSprite(
				AssetsEditor()->m_Path_Sprite_GizmoVertexCornerBg,
				Position,
				1.0f, 0.0f, 0x0, 1.0f
			);
			AssetsRenderer()->DrawSprite(
				AssetsEditor()->m_Path_Sprite_GizmoVertexCornerFg,
				Position,
				1.0f, 0.0f, 0x0, 1.0f
			);
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapObjectAddVertex::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerObjects::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	if(m_CurrentAssetPath != AssetsEditor()->GetEditedAssetPath())
		Reset();
	
	CCursorTool::Update(ParentEnabled);
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

/* CURSORTOOL OBJECT VERTEX PICKER ************************************/

CCursorTool_MapObjectVertexPicker::CCursorTool_MapObjectVertexPicker(CViewMap* pViewMap, const CLocalizableString& LString, CAssetPath IconPath) :
	CViewMap::CCursorTool(pViewMap, LString, IconPath)
{
	
}
	
void CCursorTool_MapObjectVertexPicker::RenderPivots()
{
	const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
		
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	float GizmoSize = 16.0f;
	
	CAsset_MapLayerObjects::CIteratorObject Iter;
	for(Iter = pMapLayer->BeginObject(); Iter != pMapLayer->EndObject(); ++Iter)
	{
		const CAsset_MapLayerObjects::CObject& Object = pMapLayer->GetObject(*Iter);
		
		for(int i=0; i<Object.GetVertexArraySize(); i++)
		{
			CSubPath VertexPath;
			VertexPath.SetId(i);
			
			vec2 Position = ViewMap()->MapRenderer()->MapPosToScreenPos(Object.GetVertexPosition(VertexPath));			
			
			int VertexType = Object.GetVertexSmoothness(VertexPath);
			switch(VertexType)
			{
				case CBezierVertex::TYPE_CORNER:
					AssetsRenderer()->DrawSprite(
						AssetsEditor()->m_Path_Sprite_GizmoVertexCornerBg,
						Position,
						1.0f, 0.0f, 0x0, 1.0f
					);
					AssetsRenderer()->DrawSprite(
						AssetsEditor()->m_Path_Sprite_GizmoVertexCornerFg,
						Position,
						1.0f, 0.0f, 0x0, 1.0f
					);
					break;
				case CBezierVertex::TYPE_AUTOSMOOTH:
					AssetsRenderer()->DrawSprite(
						AssetsEditor()->m_Path_Sprite_GizmoVertexSmoothBg,
						Position,
						1.0f, 0.0f, 0x0, 1.0f
					);
					AssetsRenderer()->DrawSprite(
						AssetsEditor()->m_Path_Sprite_GizmoVertexSmoothFg,
						Position,
						1.0f, 0.0f, 0x0, 1.0f
					);
					break;
				case CBezierVertex::TYPE_CONTROLLED_FREE:
				case CBezierVertex::TYPE_CONTROLLED_SYMETRIC:
				case CBezierVertex::TYPE_CONTROLLED_ALIGNED:
					AssetsRenderer()->DrawSprite(
						AssetsEditor()->m_Path_Sprite_GizmoVertexBezierBg,
						Position,
						1.0f, 0.0f, 0x0, 1.0f
					);
					AssetsRenderer()->DrawSprite(
						AssetsEditor()->m_Path_Sprite_GizmoVertexBezierFg,
						Position,
						1.0f, 0.0f, 0x0, 1.0f
					);
					break;
			}
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapObjectVertexPicker::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerObjects::TypeId:
			Enable();
			break;
		default:
			Disable();
	}
	
	CCursorTool::Update(ParentEnabled);
}

vec2 CCursorTool_MapObjectVertexPicker::PickVertex(vec2 Position)
{
	m_CurrentVertex = CSubPath::Null();
	
	const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return 0.0f;
	
	//Find a vertex
	CAsset_MapLayerObjects::CIteratorObject Iter;
	for(Iter = pMapLayer->BeginObject(); Iter != pMapLayer->EndObject(); ++Iter)
	{
		const CAsset_MapLayerObjects::CObject& Object = pMapLayer->GetObject(*Iter);
		const array< CAsset_MapLayerObjects::CVertex, allocator_copy<CAsset_MapLayerObjects::CVertex> >& Vertices = Object.GetVertexArray();
		
		for(int i=Vertices.size()-1; i>=0; i--)
		{
			vec2 VertexPosition = ViewMap()->MapRenderer()->MapPosToScreenPos(Vertices[i].GetPosition());
			if(distance(VertexPosition, Position) < 16.0f)
			{
				m_CurrentVertex = CAsset_MapLayerObjects::SubPath_ObjectVertex((*Iter).GetId(), i);
				return VertexPosition;
			}
		}
	}
	
	return 0.0f;
}

/* CURSORTOOL OBJECT EDIT VERTEX **************************************/

CCursorTool_MapObjectEditVertex::CCursorTool_MapObjectEditVertex(CViewMap* pViewMap) :
	CCursorTool_MapObjectVertexPicker(pViewMap, _LSTRING("Vertex Editor"), pViewMap->AssetsEditor()->m_Path_Sprite_IconMoveVertex),
	m_DragType(0),
	m_Token(CAssetsHistory::NEW_TOKEN)
{
	
}
	
void CCursorTool_MapObjectEditVertex::RenderView()
{
	RenderPivots();
	
	const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer || !pMapLayer->IsValidObjectVertex(m_CurrentVertex))
		return;
		
	vec2 VertexPos = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_POSITION, 0.0f);
	vec2 VertexControlPoint0 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_CONTROLPOINT0, 0.0f);
	vec2 VertexControlPoint1 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_CONTROLPOINT1, 0.0f);
	int VertexType = AssetsManager()->GetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_SMOOTHNESS, CBezierVertex::TYPE_CORNER);
	
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
		
		VertexControlPoint0 = ViewMap()->MapRenderer()->MapPosToScreenPos(VertexPos + VertexControlPoint0);
		VertexControlPoint1 = ViewMap()->MapRenderer()->MapPosToScreenPos(VertexPos + VertexControlPoint1);
		VertexPos = ViewMap()->MapRenderer()->MapPosToScreenPos(VertexPos);
		
		float GizmoSize = 16.0f;
		
		Graphics()->TextureClear();
		Graphics()->LinesBegin();
		CGraphics::CLineItem aLineItem[] = {
			CGraphics::CLineItem(VertexControlPoint0.x, VertexControlPoint0.y, VertexPos.x, VertexPos.y),
			CGraphics::CLineItem(VertexPos.x, VertexPos.y, VertexControlPoint1.x, VertexControlPoint1.y)
		};
		Graphics()->LinesDraw(aLineItem, 2);
		Graphics()->LinesEnd();
		
		AssetsRenderer()->DrawSprite(
			AssetsEditor()->m_Path_Sprite_GizmoVertexControl,
			VertexControlPoint0,
			1.0f, angle(VertexControlPoint0 - VertexPos), 0x0, 1.0f
		);
		AssetsRenderer()->DrawSprite(
			AssetsEditor()->m_Path_Sprite_GizmoVertexControl,
			VertexControlPoint1,
			1.0f, angle(VertexControlPoint1 - VertexPos), 0x0, 1.0f
		);
	
		ViewMap()->MapRenderer()->UnsetGroup();
	}
}

void CCursorTool_MapObjectEditVertex::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(Button != KEY_MOUSE_1)
		return;
	
	const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	
	if(m_CurrentVertex.IsNotNull())
	{
		vec2 VertexPos = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_POSITION, 0.0f);
		vec2 VertexControlPoint0 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_CONTROLPOINT0, 0.0f);
		vec2 VertexControlPoint1 = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_CONTROLPOINT1, 0.0f);
		int VertexType = AssetsManager()->GetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_SMOOTHNESS, CBezierVertex::TYPE_CORNER);
		
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
			
			VertexControlPoint0 = ViewMap()->MapRenderer()->MapPosToScreenPos(VertexPos + VertexControlPoint0);
			VertexControlPoint1 = ViewMap()->MapRenderer()->MapPosToScreenPos(VertexPos + VertexControlPoint1);
			VertexPos = ViewMap()->MapRenderer()->MapPosToScreenPos(VertexPos);
			
			float GizmoSize = 4.0f;
			
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
		vec2 VertexPosition = PickVertex(MousePos);
		if(m_CurrentVertex.IsNotNull())
		{
			AssetsEditor()->SetEditedAsset(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex);
			ClickDiff = MousePos - VertexPosition;
			m_Token = AssetsManager()->GenerateToken();
			m_DragType = 1;
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapObjectEditVertex::OnViewButtonRelease(int Button)
{
	if(Button == KEY_MOUSE_1)
	{
		m_DragType = 0;
		m_Token = CAssetsHistory::NEW_TOKEN;
	}
}
	
void CCursorTool_MapObjectEditVertex::OnViewMouseMove()
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(m_DragType <= 0)
		return;
	
	const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
	if(!pMapLayer)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	vec2 MousePos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	vec2 NewPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(MousePos);
	
	if(m_DragType == 1)
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_POSITION, NewPos, m_Token);
	else if(m_DragType == 2)
	{
		vec2 VertexPos = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_POSITION, 0.0f);
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_CONTROLPOINT0, NewPos - VertexPos, m_Token);
	}
	else if(m_DragType == 3)
	{
		vec2 VertexPos = AssetsManager()->GetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_POSITION, 0.0f);
		AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), m_CurrentVertex, CAsset_MapLayerObjects::OBJECT_VERTEX_CONTROLPOINT1, NewPos - VertexPos, m_Token);
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
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
	RenderPivots();
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
