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

#include <generated/assets/maplayerquads.h>
#include <generated/assets/skeletonanimation.h>
#include <shared/components/assetsmanager.h>

void CAsset_MapLayerQuads::CQuad::GetTransform(CAssetsManager* pAssetsManager, float Time, matrix2x2* pMatrix, vec2* pPosition) const
{
	*pPosition = m_Pivot;
	
	float Angle = m_Angle;
	vec2 Scale = m_Size;
	
	if(m_AnimationPath.IsNotNull())
	{
		const CAsset_SkeletonAnimation* pAnimation = pAssetsManager->GetAsset<CAsset_SkeletonAnimation>(m_AnimationPath);
		if(pAnimation)
		{
			CAsset_SkeletonAnimation::CBoneAnimation::CFrame Frame;
			if(pAnimation->GetLocalBoneAnimFrame(0, Time, Frame))
			{
				*pPosition += Frame.GetTranslation();
				Angle += Frame.GetAngle();
				Scale *= Frame.GetScale();
			}
		}
	}
	
	*pMatrix = matrix2x2::rotation(Angle)*matrix2x2::scaling(Scale);
}

void CAsset_MapLayerQuads::CQuad::GetDrawState(CAssetsManager* pAssetsManager, float Time, vec4* pColor, int* pState) const
{
	*pColor = m_Color;
	*pState = CAsset_SkeletonAnimation::LAYERSTATE_VISIBLE;
	
	if(!m_AnimationPath.IsNull())
	{
		const CAsset_SkeletonAnimation* pAnimation = pAssetsManager->GetAsset<CAsset_SkeletonAnimation>(m_AnimationPath);
		if(pAnimation)
		{
			CAsset_SkeletonAnimation::CLayerAnimation::CFrame Frame;
			if(pAnimation->GetLayerAnimFrame(0, Time, Frame))
			{
				*pColor *= Frame.GetColor();
				*pState = Frame.GetState();
			}
		}
	}
}

void CAsset_MapLayerQuads::GetQuadTransform(const CSubPath& SubPath, float Time, matrix2x2* pMatrix, vec2* pPosition) const
{
	m_Quad[SubPath.GetId()].GetTransform(AssetsManager(), Time, pMatrix, pPosition);
}

void CAsset_MapLayerQuads::GetQuadDrawState(const CSubPath& SubPath, float Time, vec4* pColor, int* pState) const
{
	m_Quad[SubPath.GetId()].GetDrawState(AssetsManager(), Time, pColor, pState);
}
