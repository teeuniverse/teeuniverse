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
#include <shared/system/debug.h>

namespace
{

float FloatInterpolation(int Type, int64 Time, int64 Time0, int64 Time1)
{
	double Alpha = (double)(Time - Time0) / (double)(Time1 - Time0);
	
	switch(Type)
	{
		case CAsset_SkeletonAnimation::GRAPHTYPE_STEPSTART:
			return 1.0f;
		case CAsset_SkeletonAnimation::GRAPHTYPE_STEPMIDDLE:
			return (Alpha < 0.5f ? 0.0f : 1.0f);
		case CAsset_SkeletonAnimation::GRAPHTYPE_LINEAR:
			return Alpha;
		case CAsset_SkeletonAnimation::GRAPHTYPE_ACCELERATION:
			return Alpha*Alpha*Alpha;
		case CAsset_SkeletonAnimation::GRAPHTYPE_DECELERATION:
			return Alpha*(3.0f-Alpha*(3.0f-Alpha));
		case CAsset_SkeletonAnimation::GRAPHTYPE_SMOOTH:
			return -2.0f*Alpha*Alpha*Alpha + 3*Alpha*Alpha;
		case CAsset_SkeletonAnimation::GRAPHTYPE_STEPEND:
		default:
			return 0.0f;
	}
}

}

/* BONE KEY FRAME *****************************************************/

CAsset_SkeletonAnimation::CBoneAnimation::CKeyFrame& CAsset_SkeletonAnimation::CBoneAnimation::CKeyFrame::operator=(const CAsset_SkeletonAnimation::CBoneAnimation::CFrame& Frame)
{
	SetTranslation(Frame.GetTranslation());
	SetScale(Frame.GetScale());
	SetAngle(Frame.GetAngle());
	SetAlignment(Frame.GetAlignment());
	return *this;
}

/* BONE ***************************************************************/

int64 CAsset_SkeletonAnimation::CBoneAnimation::GetDuration() const
{
	if(m_KeyFrame.size())
		return m_KeyFrame[m_KeyFrame.size()-1].GetTime();
	else
		return 0.0;
}

int CAsset_SkeletonAnimation::CBoneAnimation::TimeToKeyFrame(int64 Time) const
{
	if(m_KeyFrame.size() == 0)
		return 0;
	
	int i;
	for(i=0; i<static_cast<int>(m_KeyFrame.size()); i++)
	{
		if(m_KeyFrame[i].GetTime() > Time)
			break;
	}
	
	return i;
}

bool CAsset_SkeletonAnimation::CBoneAnimation::GetFrame(int64 Time, CBoneAnimation::CFrame& Frame) const
{
	if(!m_KeyFrame.size())
		return false;
	
	int64 Duration = GetDuration();
	int64 CycleTime = Time;
	if(m_CycleType == CYCLETYPE_LOOP)
		CycleTime = Time % GetDuration();
	
	int i1 = TimeToKeyFrame(CycleTime);
	int i0 = i1-1;
	
	if(m_CycleType == CYCLETYPE_LOOP)
	{
		i0 = (i0 + m_KeyFrame.size())%m_KeyFrame.size();
		i1 = (i1 + m_KeyFrame.size())%m_KeyFrame.size();
	}
	else
	{
		i0 = clamp(i0, 0, (int)m_KeyFrame.size()-1);
		i1 = clamp(i1, 0, (int)m_KeyFrame.size()-1);
	}
	
	if(i0 == i1)
		Frame = m_KeyFrame[i0];
	else
	{
		int64 Time0 = m_KeyFrame[i0].GetTime();
		int64 Time1 = m_KeyFrame[i1].GetTime();
		if(Time0 > CycleTime)
			Time0 -= Duration;
		if(Time1 < CycleTime)
			Time1 += Duration;
		
		double Alpha = FloatInterpolation(m_KeyFrame[i0].GetGraphType(), CycleTime, Time0, Time1);
		
		Frame.SetTranslation(mix(m_KeyFrame[i0].GetTranslation(), m_KeyFrame[i1].GetTranslation(), Alpha));
		Frame.SetScale(mix(m_KeyFrame[i0].GetScale(), m_KeyFrame[i1].GetScale(), Alpha));
		Frame.SetAngle(mix(m_KeyFrame[i0].GetAngle(), m_KeyFrame[i1].GetAngle(), Alpha)); //Need better interpolation
		Frame.SetAlignment(m_KeyFrame[i0].GetAlignment());
	}
	
	return true;
}

bool CAsset_SkeletonAnimation::GetBoneAnimFrame(const CSubPath& SubPath, int64 Time, CBoneAnimation::CFrame& Frame) const
{
	int Id = SubPath.GetId();
	if(Id < 0 || Id >= static_cast<int>(m_BoneAnimation.size()))
	{
		Frame.SetTranslation(vec2(0.0f, 0.0f));
		Frame.SetScale(vec2(1.0f, 1.0f));
		Frame.SetAngle(0.0f);
		Frame.SetAlignment(BONEALIGN_PARENTBONE);
		return false;
	}
	
	return m_BoneAnimation[Id].GetFrame(Time, Frame);
}

CSubPath CAsset_SkeletonAnimation::FindBoneAnim(const CSubPath& BonePath) const
{
	for(unsigned int i=0; i<m_BoneAnimation.size(); i++)
	{
		if(m_BoneAnimation[i].GetBonePath() == BonePath)
		{
			return CAsset_SkeletonAnimation::SubPath_BoneAnimation(i);
		}
	}
	
	return CSubPath::Null();
}

/* LAYER KEY FRAME ****************************************************/

CAsset_SkeletonAnimation::CLayerAnimation::CKeyFrame& CAsset_SkeletonAnimation::CLayerAnimation::CKeyFrame::operator=(const CAsset_SkeletonAnimation::CLayerAnimation::CFrame& Frame)
{
	SetColor(Frame.GetColor());
	SetState(Frame.GetState());
	return *this;
}

/* LAYER **************************************************************/

int64 CAsset_SkeletonAnimation::CLayerAnimation::GetDuration() const
{
	if(m_KeyFrame.size())
		return m_KeyFrame[m_KeyFrame.size()-1].GetTime();
	else
		return 0;
}

int CAsset_SkeletonAnimation::CLayerAnimation::TimeToKeyFrame(int64 Time) const
{
	if(m_KeyFrame.size() == 0)
		return 0;
	
	int i;
	for(i=0; i<static_cast<int>(m_KeyFrame.size()); i++)
	{
		if(m_KeyFrame[i].GetTime() > Time)
			break;
	}
	
	return i;
}

bool CAsset_SkeletonAnimation::CLayerAnimation::GetFrame(int64 Time, CLayerAnimation::CFrame& Frame) const
{
	if(!m_KeyFrame.size())
		return false;
	
	int64 Duration = GetDuration();
	int64 CycleTime = Time;
	if(m_CycleType == CYCLETYPE_LOOP)
		CycleTime = Time % GetDuration();
	
	int i1 = TimeToKeyFrame(CycleTime);
	int i0 = i1-1;
	
	if(m_CycleType == CYCLETYPE_LOOP)
	{
		i0 = (i0 + m_KeyFrame.size())%m_KeyFrame.size();
		i1 = (i1 + m_KeyFrame.size())%m_KeyFrame.size();
	}
	else
	{
		i0 = clamp(i0, 0, (int)m_KeyFrame.size()-1);
		i1 = clamp(i1, 0, (int)m_KeyFrame.size()-1);
	}
	
	if(i0 == i1)
		Frame = m_KeyFrame[i0];
	else
	{
		int64 Time0 = m_KeyFrame[i0].GetTime();
		int64 Time1 = m_KeyFrame[i1].GetTime();
		if(Time0 > CycleTime)
			Time0 -= Duration;
		if(Time1 < CycleTime)
			Time1 += Duration;
		
		double Alpha = FloatInterpolation(m_KeyFrame[i0].GetGraphType(), CycleTime, Time0, Time1);
		
		Frame.SetColor(mix(m_KeyFrame[i0].GetColor(), m_KeyFrame[i1].GetColor(), Alpha));
		Frame.SetState(m_KeyFrame[i0].GetState());
	}
	
	return true;
}

bool CAsset_SkeletonAnimation::GetLayerAnimFrame(const CSubPath& SubPath, int64 Time, CLayerAnimation::CFrame& Frame) const
{
	int Id = SubPath.GetId();
	if(Id < 0 || Id >= static_cast<int>(m_LayerAnimation.size()))
	{
		Frame.SetColor(1.0f);
		Frame.SetState(LAYERSTATE_VISIBLE);
		return false;
	}
	
	return m_LayerAnimation[Id].GetFrame(Time, Frame);
}

CSubPath CAsset_SkeletonAnimation::FindLayerAnim(const CSubPath& LayerPath) const
{
	for(unsigned int i=0; i<m_LayerAnimation.size(); i++)
	{
		if(m_LayerAnimation[i].GetLayerPath() == LayerPath)
		{
			return CAsset_SkeletonAnimation::SubPath_LayerAnimation(i);
		}
	}
	
	return CSubPath::Null();
}
