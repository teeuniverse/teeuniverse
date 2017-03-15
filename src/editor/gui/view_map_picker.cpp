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

#include <editor/gui/view_map_picker.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/maprenderer.h>
#include <shared/geometry/geometry.h>
#include <generated/assets/maplayerquads.h>
#include <client/gui/toggle.h>

/* CURSORTOOL MAP PICKER **********************************************/

CCursorTool_MapPicker::CCursorTool_MapPicker(CViewMap* pViewMap, const CLocalizableString& LString, CAssetPath IconPath) :
	CCursorTool(pViewMap, LString, IconPath)
{
	
}

CSubPath CCursorTool_MapPicker::Pick(vec2 CursorPos)
{
	static const float GizmoSize = 16.0f;
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapEntities::TypeId)
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
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
	{
		const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return CSubPath::Null();
		
		const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(pMapLayer->GetImagePath());
		
		CSubPath QuadFound = CSubPath::Null();
		
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
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
	{
		const CAsset_MapLayerObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return CSubPath::Null();
		
		CSubPath ObjectFound = CSubPath::Null();
		
		{
			CAsset_MapLayerObjects::CIteratorObject Iter;
			for(Iter = pMapLayer->BeginObject(); Iter != pMapLayer->EndObject(); ++Iter)
			{
				vec2 Position;
				matrix2x2 Transform;
				pMapLayer->GetObjectTransform(*Iter, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
				
				vec2 PivotPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*vec2(0.0f, 0.0f) + Position);
				if(length(CursorPos - PivotPos) < GizmoSize*(12.0f/16.0f))
					ObjectFound = *Iter;
				
				if(!ObjectFound.IsNull())
					break;
			}
		}
		
		return ObjectFound;
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
	{
		const CAsset_MapZoneObjects* pMapLayer = AssetsManager()->GetAsset<CAsset_MapZoneObjects>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return CSubPath::Null();
		
		CSubPath ObjectFound = CSubPath::Null();
		
		{
			CAsset_MapZoneObjects::CIteratorObject Iter;
			for(Iter = pMapLayer->BeginObject(); Iter != pMapLayer->EndObject(); ++Iter)
			{
				vec2 Position;
				matrix2x2 Transform;
				pMapLayer->GetObjectTransform(*Iter, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
				
				vec2 PivotPos = ViewMap()->MapRenderer()->MapPosToScreenPos(Transform*vec2(0.0f, 0.0f) + Position);
				if(length(CursorPos - PivotPos) < GizmoSize*(12.0f/16.0f))
					ObjectFound = *Iter;
				
				if(!ObjectFound.IsNull())
					break;
			}
		}
		
		return ObjectFound;
	}
	else
		return CSubPath::Null();
}

template<typename ASSET>
void CCursorTool_MapPicker::RenderPivots_Objects_Impl()
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

void CCursorTool_MapPicker::RenderPivots()
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
	{
		const CAsset_MapLayerQuads* pMapLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
		if(!pMapLayer)
			return;
		
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
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		RenderPivots_Objects_Impl<CAsset_MapLayerObjects>();
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		RenderPivots_Objects_Impl<CAsset_MapZoneObjects>();
}

template<typename ASSET>
void CCursorTool_MapPicker::RenderVertices_Objects_Impl()
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

void CCursorTool_MapPicker::RenderVertices()
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
	{
		
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		RenderVertices_Objects_Impl<CAsset_MapLayerObjects>();
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		RenderVertices_Objects_Impl<CAsset_MapZoneObjects>();
}
