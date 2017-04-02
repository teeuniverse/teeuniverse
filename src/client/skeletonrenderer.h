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

#ifndef __CLIENT_SKELETONRENDERER__
#define __CLIENT_SKELETONRENDERER__

#include <shared/components/assetsmanager.h>
#include <client/kernel.h>

class CSkeletonRenderer : public CClientKernel::CGuest
{
protected:	
	class CBoneState
	{
	public:
		int m_ParentBone;
		float m_Length;
		float m_Anchor;
		matrix2x2 m_Transform;
		matrix2x2 m_InvTransform;
		vec2 m_StartPoint;
		vec2 m_EndPoint;
		vec4 m_Color;
		int m_Alignment;
		
		bool m_Finalized;
	};
	
	class CLayerState
	{
	public:
		int m_State;
		vec4 m_Color;
	};
	
	CAssetPath m_SkeletonPath;
	std::vector<CBoneState> m_Bones;
	std::vector<CLayerState> m_Layers;
	std::vector<CAssetPath> m_Skins;
	
	matrix2x2 m_WorldTransform;
	matrix2x2 m_InvWorldTransform;
	vec2 m_WorldPosition;
	vec2 m_ScreenPosition;

protected:
	void FinalizeBone(int BoneId);
	
public:	
	CSkeletonRenderer(CClientKernel* pKernel);
	
	inline void SetWorldTransform(const matrix2x2& Transform, const vec2& WorldPosition, const vec2 ScreenPosition)
	{
		m_WorldTransform = Transform;
		m_InvWorldTransform = matrix2x2::inverse(Transform);
		m_WorldPosition = WorldPosition;
		m_ScreenPosition = ScreenPosition;
	}
	
	void SetSkeleton(const CAssetPath& SkeletonPath);
	void ApplyAnimation(CAssetPath AnimationPath, int64 Time);
	void AddSkin(const CAssetPath& SkinPath)
	{
		m_Skins.emplace_back(SkinPath);
	}
	void Finalize();
	
	void RenderSkins(vec2 Position, float Size);
	
	void RenderBones(vec2 Position, float Size);
	
	CSubPath BonePicking(vec2 Position, float Size, vec2 Point);
	
	inline vec2 SkeletonPosToScreenPos(vec2 SkeletonPos)
	{
		return m_ScreenPosition + m_WorldTransform * (SkeletonPos + m_WorldPosition);
	}
	
	inline vec2 ScreenPosToSkeletonPos(vec2 WorldPos)
	{
		return (m_InvWorldTransform * (WorldPos - m_ScreenPosition)) - m_WorldPosition;
	}

	inline vec2 ScreenPosToBonePos(CSubPath BonePath, vec2 WorldPos)
	{
		return SkeletonPosToBonePos(BonePath, ScreenPosToSkeletonPos(WorldPos));
	}

	inline vec2 BonePosToScreenPos(CSubPath BonePath, vec2 BonePos)
	{
		return SkeletonPosToScreenPos(BonePosToSkeletonPos(BonePath, BonePos));
	}
	
	vec2 SkeletonPosToBonePos(CSubPath BonePath, vec2 WorldPos);
	vec2 BonePosToSkeletonPos(CSubPath BonePath, vec2 BonePos);
};

#endif
