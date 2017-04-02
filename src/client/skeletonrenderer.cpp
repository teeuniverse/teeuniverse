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

#include <client/components/graphics.h>
#include <client/components/assetsrenderer.h>

#include "skeletonrenderer.h"

CSkeletonRenderer::CSkeletonRenderer(CClientKernel* pKernel) :
	CClientKernel::CGuest(pKernel)
{
	
}

vec2 CSkeletonRenderer::SkeletonPosToBonePos(CSubPath BonePath, vec2 SkeletonPos)
{
	if(BonePath.GetId() >= m_Bones.size())
		return 0.0f;
	
	return m_Bones[BonePath.GetId()].m_InvTransform * (SkeletonPos - m_Bones[BonePath.GetId()].m_StartPoint);
}

vec2 CSkeletonRenderer::BonePosToSkeletonPos(CSubPath BonePath, vec2 BonePos)
{
	if(BonePath.GetId() >= m_Bones.size())
		return 0.0f;
	
	return (m_Bones[BonePath.GetId()].m_Transform * BonePos) + m_Bones[BonePath.GetId()].m_StartPoint;
}

void CSkeletonRenderer::SetSkeleton(const CAssetPath& SkeletonPath)
{
	m_SkeletonPath = SkeletonPath;
	
	m_Bones.clear();
	m_Layers.clear();
	m_Skins.clear();
	
	const CAsset_Skeleton* pSkeleton = AssetsManager()->GetAsset<CAsset_Skeleton>(SkeletonPath);
	if(!pSkeleton)
		return;
	
	const std::vector<CAsset_Skeleton::CBone>& Bones = pSkeleton->GetBoneArray();
	m_Bones.resize(Bones.size());
	for(unsigned int i=0; i<Bones.size(); i++)
	{
		CSkeletonRenderer::CBoneState& BoneState = m_Bones[i];
		if(Bones[i].GetParent().IsNotNull())
			BoneState.m_ParentBone = Bones[i].GetParent().GetId();
		else
			BoneState.m_ParentBone = -1;
		BoneState.m_Finalized = false;
			
		BoneState.m_Length = Bones[i].GetLength();
		BoneState.m_Anchor = Bones[i].GetAnchor();
		BoneState.m_Color = Bones[i].GetColor();
		
		BoneState.m_StartPoint = Bones[i].GetTranslation();
		BoneState.m_Transform = matrix2x2::rotation(Bones[i].GetAngle());
	}
	
	const std::vector<CAsset_Skeleton::CLayer>& Layers = pSkeleton->GetLayerArray();
	m_Layers.resize(Layers.size());
	for(unsigned int i=0; i<Layers.size(); i++)
	{
		CSkeletonRenderer::CLayerState& LayerState = m_Layers[i];
		LayerState.m_Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		LayerState.m_State = CAsset_SkeletonAnimation::LAYERSTATE_VISIBLE;
	}
}

void CSkeletonRenderer::ApplyAnimation(CAssetPath AnimationPath, int64 Time)
{
	const CAsset_SkeletonAnimation* pAnimation = AssetsManager()->GetAsset<CAsset_SkeletonAnimation>(AnimationPath);
	if(!pAnimation || pAnimation->GetSkeletonPath() != m_SkeletonPath)
		return;
	
	const std::vector<CAsset_SkeletonAnimation::CBoneAnimation>& BoneAnimations = pAnimation->GetBoneAnimationArray();
	for(unsigned int i=0; i<BoneAnimations.size(); i++)
	{
		CSubPath BonePath = BoneAnimations[i].GetBonePath();
		int BoneId = BonePath.GetId();
		if(BoneId < 0 || BoneId >= (int) m_Bones.size())
			continue;
		
		CSkeletonRenderer::CBoneState& BoneState = m_Bones[BoneId];
		
		CAsset_SkeletonAnimation::CBoneAnimation::CFrame Frame;
		if(BoneAnimations[i].GetFrame(Time, Frame))
		{
			BoneState.m_StartPoint += BoneState.m_Transform * Frame.GetTranslation();
			BoneState.m_Transform = BoneState.m_Transform * matrix2x2::rotation(Frame.GetAngle()) * matrix2x2::scaling(Frame.GetScale());
			BoneState.m_Alignment = Frame.GetAlignment();
		}
	}
	
	const std::vector<CAsset_SkeletonAnimation::CLayerAnimation>& LayerAnimations = pAnimation->GetLayerAnimationArray();
	for(unsigned int i=0; i<LayerAnimations.size(); i++)
	{
		CSubPath LayerPath = LayerAnimations[i].GetLayerPath();
		int LayerId = LayerPath.GetId();
		if(LayerId < 0 || LayerId >= (int) m_Layers.size())
			continue;
		
		CSkeletonRenderer::CLayerState& LayerState = m_Layers[LayerId];
		
		CAsset_SkeletonAnimation::CLayerAnimation::CFrame Frame;
		if(LayerAnimations[i].GetFrame(Time, Frame))
		{
			LayerState.m_Color = Frame.GetColor();
			LayerState.m_State = Frame.GetState();
		}
	}
}

void CSkeletonRenderer::FinalizeBone(int b)
{
	CSkeletonRenderer::CBoneState& BoneState = m_Bones[b];
	if(BoneState.m_Finalized)
		return;
	
	if(BoneState.m_ParentBone >= 0)
	{
		FinalizeBone(BoneState.m_ParentBone);
		
		CSkeletonRenderer::CBoneState& ParentBoneState = m_Bones[BoneState.m_ParentBone];
		BoneState.m_Transform = ParentBoneState.m_Transform * BoneState.m_Transform;
		BoneState.m_StartPoint = ParentBoneState.m_StartPoint + (ParentBoneState.m_EndPoint - ParentBoneState.m_StartPoint)*BoneState.m_Anchor + ParentBoneState.m_Transform * BoneState.m_StartPoint;
	}
	
	switch(BoneState.m_Alignment)
	{
		case CAsset_SkeletonAnimation::BONEALIGN_WORLD:
		{
			vec2 Orientation = BoneState.m_Transform * vec2(1.0f, 0.0f);
			BoneState.m_Transform = BoneState.m_Transform * matrix2x2::rotation(angle(Orientation, vec2(1.0f, 0.0f)));
			break;
		}
	}
	
	BoneState.m_InvTransform = matrix2x2::inverse(BoneState.m_Transform);
	BoneState.m_EndPoint = BoneState.m_StartPoint + BoneState.m_Transform * vec2(BoneState.m_Length, 0.0f);
	BoneState.m_Finalized = true;
}

void CSkeletonRenderer::Finalize()
{
	for(unsigned int b=0; b<m_Bones.size(); b++)
		FinalizeBone(b);
}

void CSkeletonRenderer::RenderSkins(vec2 Position, float Size)
{
	for(unsigned int l=0; l<m_Layers.size(); l++)
	{
		for(unsigned int s=0; s<m_Skins.size(); s++)
		{
			const CAsset_SkeletonSkin* pSkin = AssetsManager()->GetAsset<CAsset_SkeletonSkin>(m_Skins[s]);
			if(!pSkin)
				continue;
			
			CAsset_SkeletonSkin::CIteratorSprite SpriteIter = pSkin->BeginSprite();
			while(SpriteIter != pSkin->EndSprite())
			{
				if(pSkin->GetSpriteLayerPath(*SpriteIter) == CAsset_Skeleton::SubPath_Layer(l))
				{
					vec2 Pos = pSkin->GetSpriteTranslation(*SpriteIter);
					vec2 X = rotate(vec2(pSkin->GetSpriteScale(*SpriteIter).x/2.0f, 0.0f), pSkin->GetSpriteAngle(*SpriteIter));
					vec2 Y = rotate(vec2(0.0f, pSkin->GetSpriteScale(*SpriteIter).y/2.0f), pSkin->GetSpriteAngle(*SpriteIter));
					
					vec2 SPos = BonePosToScreenPos(pSkin->GetSpriteBonePath(*SpriteIter), Pos);
					vec2 SX = BonePosToScreenPos(pSkin->GetSpriteBonePath(*SpriteIter), Pos + X) - SPos;
					vec2 SY = BonePosToScreenPos(pSkin->GetSpriteBonePath(*SpriteIter), Pos + Y) - SPos;
					
					AssetsRenderer()->DrawFreeSprite(
						pSkin->GetSpriteSpritePath(*SpriteIter),
						SPos, SX, SY,
						0x0,
						pSkin->GetSpriteColor(*SpriteIter)*m_Layers[l].m_Color
					);
				}
				
				++SpriteIter;
			}
		}
	}
}

void CSkeletonRenderer::RenderBones(vec2 Position, float Size)
{
	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	
	for(unsigned int b=0; b<m_Bones.size(); b++)
	{
		CSkeletonRenderer::CBoneState& BoneState = m_Bones[b];
		
		Graphics()->SetColor(BoneState.m_Color.r*BoneState.m_Color.a, BoneState.m_Color.g*BoneState.m_Color.a, BoneState.m_Color.b*BoneState.m_Color.a, BoneState.m_Color.a);
			
		vec2 StartPoint = Position + BoneState.m_StartPoint*Size;
		vec2 EndPoint = Position + BoneState.m_EndPoint*Size;
		vec2 Dir = normalize(EndPoint - StartPoint);
		
		float StartPointLength = min(Size*4.0f, distance(StartPoint, EndPoint)/2.0f);
		vec2 TopPoint = StartPoint + vec2(Dir.x - Dir.y, Dir.x + Dir.y)*StartPointLength/sqrt(2.0f);
		vec2 BottomPoint = StartPoint + vec2(Dir.x + Dir.y, - Dir.x + Dir.y)*StartPointLength/sqrt(2.0f);
		
		//Apply transform
		StartPoint = SkeletonPosToScreenPos(StartPoint);
		BottomPoint = SkeletonPosToScreenPos(BottomPoint);
		TopPoint = SkeletonPosToScreenPos(TopPoint);
		EndPoint = SkeletonPosToScreenPos(EndPoint);
		
		CGraphics::CFreeformItem Freeform(
			StartPoint.x, StartPoint.y,
			BottomPoint.x, BottomPoint.y,
			TopPoint.x, TopPoint.y,
			EndPoint.x, EndPoint.y);
		Graphics()->QuadsDrawFreeform(&Freeform, 1);
	}
	
	Graphics()->QuadsEnd();
}

CSubPath CSkeletonRenderer::BonePicking(vec2 Position, float Size, vec2 Point)
{
	vec2 Vertices[4];
	
	for(unsigned int b=0; b<m_Bones.size(); b++)
	{
		CSkeletonRenderer::CBoneState& BoneState = m_Bones[b];
		
		vec2 StartPoint = Position + BoneState.m_StartPoint*Size;
		vec2 EndPoint = Position + BoneState.m_EndPoint*Size;
		vec2 Dir = normalize(EndPoint - StartPoint);
		
		float StartPointLength = min(Size*4.0f, distance(StartPoint, EndPoint)/2.0f);
		vec2 TopPoint = StartPoint + vec2(Dir.x - Dir.y, Dir.x + Dir.y)*StartPointLength/sqrt(2.0f);
		vec2 BottomPoint = StartPoint + vec2(Dir.x + Dir.y, - Dir.x + Dir.y)*StartPointLength/sqrt(2.0f);
		
		//Apply transform
		StartPoint = SkeletonPosToScreenPos(StartPoint);
		BottomPoint = SkeletonPosToScreenPos(BottomPoint);
		TopPoint = SkeletonPosToScreenPos(TopPoint);
		EndPoint = SkeletonPosToScreenPos(EndPoint);
		
		Vertices[0] = StartPoint - Point;
		Vertices[1] = BottomPoint - Point;
		Vertices[2] = EndPoint - Point;
		Vertices[3] = TopPoint - Point;
		
		bool IsInside = true;
		for(int i=0; i<4; i++)
		{
			vec2 Edge = Vertices[(i+1)%4] - Vertices[i];
			if(Edge.x * Vertices[i].y - Edge.y * Vertices[i].x > 0.0f)
			{
				IsInside = false;
				break;
			}
		}
		
		if(IsInside)
		{
			return CAsset_Skeleton::SubPath_Bone(b);
		}
	}
	
	return CSubPath::Null();
}
