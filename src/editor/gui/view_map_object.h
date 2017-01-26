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

#ifndef __CLIENT_ASSETSEDITOR_VIEWMAP_OBJECT__
#define __CLIENT_ASSETSEDITOR_VIEWMAP_OBJECT__

#include <client/components/assetsrenderer.h>
#include <editor/components/gui.h>
#include <editor/gui/view_map.h>
#include <generated/assets/maplayerobjects.h>
#include <generated/assets/mapzoneobjects.h>
#include <client/maprenderer.h>

class CCursorTool_MapObjectVertexPicker : public CViewMap::CCursorTool
{
protected:
	CSubPath m_CurrentVertex;
	
protected:
	template<typename ASSET>
	vec2 PickVertex(vec2 PickPosition)
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
			const array< typename ASSET::CVertex, allocator_copy<typename ASSET::CVertex> >& Vertices = Object.GetVertexArray();
			vec2 Position;
			matrix2x2 Transform;
			Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
			
			for(int i=Vertices.size()-1; i>=0; i--)
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
	bool PickNewVertex(vec2 PickPosition, vec2& VertexPosition, CSubPath& PreviousVertex, CSubPath& NextVertex)
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
	void RenderPivots()
	{
		const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return;
			
		ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
		
		typename ASSET::CIteratorObject Iter;
		
		Graphics()->TextureClear();
		Graphics()->LinesBegin();
		
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
				
				vec2 VertexPosition0 = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*Object.GetVertexPosition(Vertex0Path) + Position);
				vec2 VertexPosition1 = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*Object.GetVertexPosition(Vertex1Path) + Position);
				
				CGraphics::CLineItem LineItem(VertexPosition0.x, VertexPosition0.y, VertexPosition1.x, VertexPosition1.y);
				Graphics()->LinesDraw(&LineItem, 1);
			}
		}
		
		Graphics()->LinesEnd();
		
		for(Iter = pMapLayer->BeginObject(); Iter != pMapLayer->EndObject(); ++Iter)
		{
			const typename ASSET::CObject& Object = pMapLayer->GetObject(*Iter);
			vec2 Position;
			matrix2x2 Transform;
			Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
			
			AssetsRenderer()->DrawSprite(
				AssetsEditor()->m_Path_Sprite_GizmoPivot,
				ViewMap()->MapRenderer()->MapPosToScreenPos(Position),
				1.0f, 0.0f, 0x0, 1.0f
			);
		}
		
		ViewMap()->MapRenderer()->UnsetGroup();
	}
	
	template<typename ASSET>
	void RenderVertices()
	{
		const ASSET* pMapLayer = AssetsManager()->template GetAsset<ASSET>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return;
			
		ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
		
		typename ASSET::CIteratorObject Iter;
		for(Iter = pMapLayer->BeginObject(); Iter != pMapLayer->EndObject(); ++Iter)
		{
			const typename ASSET::CObject& Object = pMapLayer->GetObject(*Iter);
			vec2 Position;
			matrix2x2 Transform;
			Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
			
			for(int i=0; i<Object.GetVertexArraySize(); i++)
			{
				CSubPath VertexPath;
				VertexPath.SetId(i);
				
				vec2 VertexPosition = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*Object.GetVertexPosition(VertexPath)+Position);			
				
				int VertexType = Object.GetVertexSmoothness(VertexPath);
				switch(VertexType)
				{
					case CBezierVertex::TYPE_CORNER:
						AssetsRenderer()->DrawSprite(
							AssetsEditor()->m_Path_Sprite_GizmoVertexCornerBg,
							VertexPosition,
							1.0f, 0.0f, 0x0, 1.0f
						);
						AssetsRenderer()->DrawSprite(
							AssetsEditor()->m_Path_Sprite_GizmoVertexCornerFg,
							VertexPosition,
							1.0f, 0.0f, 0x0, 1.0f
						);
						break;
					case CBezierVertex::TYPE_AUTOSMOOTH:
						AssetsRenderer()->DrawSprite(
							AssetsEditor()->m_Path_Sprite_GizmoVertexSmoothBg,
							VertexPosition,
							1.0f, 0.0f, 0x0, 1.0f
						);
						AssetsRenderer()->DrawSprite(
							AssetsEditor()->m_Path_Sprite_GizmoVertexSmoothFg,
							VertexPosition,
							1.0f, 0.0f, 0x0, 1.0f
						);
						break;
					case CBezierVertex::TYPE_CONTROLLED_FREE:
					case CBezierVertex::TYPE_CONTROLLED_SYMETRIC:
					case CBezierVertex::TYPE_CONTROLLED_ALIGNED:
						AssetsRenderer()->DrawSprite(
							AssetsEditor()->m_Path_Sprite_GizmoVertexBezierBg,
							VertexPosition,
							1.0f, 0.0f, 0x0, 1.0f
						);
						AssetsRenderer()->DrawSprite(
							AssetsEditor()->m_Path_Sprite_GizmoVertexBezierFg,
							VertexPosition,
							1.0f, 0.0f, 0x0, 1.0f
						);
						break;
				}
			}
		}
		
		ViewMap()->MapRenderer()->UnsetGroup();
	}
	
public:
	CCursorTool_MapObjectVertexPicker(CViewMap* pViewMap, const CLocalizableString& LString, CAssetPath IconPath);
	virtual void Update(bool ParentEnabled);
};

class CCursorTool_MapObjectAddVertex : public CCursorTool_MapObjectVertexPicker
{
protected:
	CSubPath m_CurrentObject;
	CAssetPath m_CurrentAssetPath;

protected:
	void UpdateBarycenter(int Token);
	
	template<typename ASSET> void OnViewButtonClick_Impl(int Button);
	template<typename ASSET> void RenderView_Impl();

public:
	CCursorTool_MapObjectAddVertex(CViewMap* pViewMap);
	virtual void Reset();
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void RenderView();
	virtual void OnMouseMove();
	virtual void OnInputEvent(const CInput::CEvent& Event);
};

class CCursorTool_MapObjectEditVertex : public CCursorTool_MapObjectVertexPicker
{
protected:
	vec2 ClickDiff;
	int m_DragType;
	int m_Token;
	
	template<typename ASSET> void OnViewButtonClick_Impl(int Button);
	template<typename ASSET> void OnViewMouseMove_Impl();
	template<typename ASSET> void RenderView_Impl();
	
public:
	CCursorTool_MapObjectEditVertex(CViewMap* pViewMap);
	virtual void RenderView();
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void OnMouseMove();
};

class CCursorTool_MapObjectWeightVertex : public CCursorTool_MapObjectVertexPicker
{
protected:
	vec2 ClickPos;
	bool m_Drag;
	int m_Token;
	
public:
	CCursorTool_MapObjectWeightVertex(CViewMap* pViewMap);
	virtual void RenderView();
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void OnMouseMove();
};

#endif
