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

#include <generated/assets/skeletonanimation.h>
#include <shared/math/math.h>

float CAsset_SkeletonAnimation::CBoneAnimation::GetDuration() const
{
	if(m_KeyFrame.size())
		return m_KeyFrame[m_KeyFrame.size()-1].GetTime()/static_cast<float>(CAsset_SkeletonAnimation::TIMESTEP);
	else
		return 0.0f;
}

inline int CAsset_SkeletonAnimation::CBoneAnimation::IntTimeToKeyFrame(int IntTime) const
{
	if(m_KeyFrame.size() == 0)
		return 0;
	
	int i;
	for(i=0; i<static_cast<int>(m_KeyFrame.size()); i++)
	{
		if(m_KeyFrame[i].GetTime() > IntTime)
			break;
	}
	
	return i;
}

int CAsset_SkeletonAnimation::CBoneAnimation::TimeToKeyFrame(float Time) const
{
	return IntTimeToKeyFrame(Time*CAsset_SkeletonAnimation::TIMESTEP);
}

bool CAsset_SkeletonAnimation::CBoneAnimation::GetFrame(float Time, CBoneAnimation::CFrame& Frame) const
{
	float CycleTime = Time;
	if(m_CycleType == CYCLETYPE_LOOP)
		CycleTime = fmod(Time, GetDuration());
	
	int i = TimeToKeyFrame(CycleTime);
	
	if(i == static_cast<int>(m_KeyFrame.size()))
	{
		if(i == 0)
			return false;
		else
			Frame = m_KeyFrame[m_KeyFrame.size()-1];
	}
	else if(i == 0)
	{
		Frame = m_KeyFrame[0];
	}
	else
	{
		float alpha = (CAsset_SkeletonAnimation::TIMESTEP*CycleTime - m_KeyFrame[i-1].GetTime()) / (m_KeyFrame[i].GetTime() - m_KeyFrame[i-1].GetTime());
		Frame.SetTranslation(mix(m_KeyFrame[i-1].GetTranslation(), m_KeyFrame[i].GetTranslation(), alpha));
		Frame.SetScale(mix(m_KeyFrame[i-1].GetScale(), m_KeyFrame[i].GetScale(), alpha));
		Frame.SetAngle(mix(m_KeyFrame[i-1].GetAngle(), m_KeyFrame[i].GetAngle(), alpha)); //Need better interpolation
		Frame.SetAlignment(m_KeyFrame[i-1].GetAlignment());
	}
	
	return true;
}

bool CAsset_SkeletonAnimation::GetLocalBoneAnimFrame(int Id, float Time, CBoneAnimation::CFrame& Frame) const
{
	if(Id < 0 || Id >= static_cast<int>(m_LocalBoneAnim.size()))
	{
		Frame.SetTranslation(vec2(0.0f, 0.0f));
		Frame.SetScale(vec2(1.0f, 1.0f));
		Frame.SetAngle(0.0f);
		Frame.SetAlignment(BONEALIGN_PARENTBONE);
		return false;
	}
	
	return m_LocalBoneAnim[Id].GetFrame(Time, Frame);
}

float CAsset_SkeletonAnimation::CLayerAnimation::GetDuration() const
{
	if(m_KeyFrame.size())
		return m_KeyFrame[m_KeyFrame.size()-1].GetTime()/static_cast<float>(CAsset_SkeletonAnimation::TIMESTEP);
	else
		return 0.0f;
}

int CAsset_SkeletonAnimation::CLayerAnimation::IntTimeToKeyFrame(int IntTime) const
{
	if(m_KeyFrame.size() == 0)
		return 0;
	
	int i;
	for(i=0; i<static_cast<int>(m_KeyFrame.size()); i++)
	{
		if(m_KeyFrame[i].GetTime() > IntTime)
			break;
	}
	
	return i;
}

int CAsset_SkeletonAnimation::CLayerAnimation::TimeToKeyFrame(float Time) const
{
	return IntTimeToKeyFrame(Time*CAsset_SkeletonAnimation::TIMESTEP);
}

bool CAsset_SkeletonAnimation::CLayerAnimation::GetFrame(float Time, CLayerAnimation::CFrame& Frame) const
{
	float CycleTime = Time;
	if(m_CycleType == CYCLETYPE_LOOP)
		CycleTime = fmod(Time, GetDuration());
	
	int i = TimeToKeyFrame(CycleTime);
	
	if(i == static_cast<int>(m_KeyFrame.size()))
	{
		if(i == 0)
			return false;
		else
			Frame = m_KeyFrame[m_KeyFrame.size()-1];
	}
	else if(i == 0)
	{
		Frame = m_KeyFrame[0];
	}
	else
	{
		float alpha = (CAsset_SkeletonAnimation::TIMESTEP*CycleTime - m_KeyFrame[i-1].GetTime()) / (m_KeyFrame[i].GetTime() - m_KeyFrame[i-1].GetTime());
		Frame.SetColor(mix(m_KeyFrame[i-1].GetColor(), m_KeyFrame[i].GetColor(), alpha));
		Frame.SetState(m_KeyFrame[i-1].GetState());
	}
	
	return true;
}

bool CAsset_SkeletonAnimation::GetLayerAnimFrame(int Id, float Time, CLayerAnimation::CFrame& Frame) const
{
	if(Id < 0 || Id >= static_cast<int>(m_LayerAnimation.size()))
	{
		Frame.SetColor(1.0f);
		Frame.SetState(LAYERSTATE_VISIBLE);
		return false;
	}
	
	return m_LayerAnimation[Id].GetFrame(Time, Frame);
}
