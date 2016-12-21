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

#include <generated/assets/maplayerobjects.h>
#include <generated/assets/skeletonanimation.h>
#include <shared/components/assetsmanager.h>

void CAsset_MapLayerObjects::CObject::GetTransform(CAssetsManager* pAssetsManager, float Time, matrix2x2* pMatrix, vec2* pPosition) const
{
	*pPosition = m_Position;
	
	float Angle = m_Angle;
	vec2 Scale = m_Size;
	
	*pMatrix = matrix2x2::rotation(Angle)*matrix2x2::scaling(Scale);
}

void CAsset_MapLayerObjects::CObject::GetDrawState(CAssetsManager* pAssetsManager, float Time, vec4* pColor, int* pState) const
{
	*pColor = 1.0f;
	*pState = CAsset_SkeletonAnimation::LAYERSTATE_VISIBLE;
}

void CAsset_MapLayerObjects::GetObjectTransform(const CSubPath& SubPath, float Time, matrix2x2* pMatrix, vec2* pPosition) const
{
	m_Object[SubPath.GetId()].GetTransform(AssetsManager(), Time, pMatrix, pPosition);
}

void CAsset_MapLayerObjects::GetObjectDrawState(const CSubPath& SubPath, float Time, vec4* pColor, int* pState) const
{
	m_Object[SubPath.GetId()].GetDrawState(AssetsManager(), Time, pColor, pState);
}
