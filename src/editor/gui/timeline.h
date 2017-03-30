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

#ifndef __CLIENT_ASSETSEDITOR_TIMELINE__
#define __CLIENT_ASSETSEDITOR_TIMELINE__

#include <client/gui/listlayout.h>

class CTimeLine : public gui::CVListLayout
{
private:
	class CGuiEditor* m_pAssetsEditor;
	CAssetPath m_EditedAssetPath;
	int m_EditedAssetVersion;
	std::vector<CSubPath> m_EditedFrames;
	bool m_NeedRefresh;
	int64 m_TimeShift;
	float m_PixelsPerMs;
	bool m_ZoomLockedToUnit;
	
public:
	CTimeLine(CGuiEditor* pAssetsEditor);
	
	class CGuiEditor* AssetsEditor() { return m_pAssetsEditor; }
	
	virtual void Update(bool ParentEnabled);
	
	CAssetPath GetAnimationPath();
	const std::vector<CSubPath> GetEditedFrames() const { return m_EditedFrames; }
	void SetEditedFrame(const CSubPath& SubPath) { m_EditedFrames.clear(); m_EditedFrames.emplace_back(SubPath); }
	void AddEditedFrame(const CSubPath& SubPath) { m_EditedFrames.emplace_back(SubPath); }
	
	inline bool RefreshNeeded() const { return m_NeedRefresh; }
	
	inline int64 GetTimeShift() const { return m_TimeShift; }
	inline void SetTimeShift(int64 Value) { m_TimeShift = Value; }
	
	inline float GetPixelsPerMs() const { return (m_ZoomLockedToUnit ? 0.25f : m_PixelsPerMs); }
	inline void SetPixelsPerMs(float Value) { m_PixelsPerMs = clamp(Value, 0.001f, 10.0f); m_ZoomLockedToUnit = false; }
	inline void SetZoomToUnit() { m_ZoomLockedToUnit = !m_ZoomLockedToUnit; }
	
	inline float TimeToScreenPos(int64 Time) const { return GetPixelsPerMs()*(Time-GetTimeShift()); }
	inline int64 ScreenPosToTime(float ScreenPos) const { return (ScreenPos/GetPixelsPerMs())+GetTimeShift(); }
	
	bool CreateBoneKeyFrame(CSubPath BonePath, int64 Time, CSubPath& AnimationSubPath, CSubPath& FramePath);
	bool CreateLayerKeyFrame(CSubPath LayerPath, int64 Time, CSubPath& AnimationSubPath, CSubPath& FramePath);
	CSubPath MoveBoneFrame(CSubPath KeyPath, int64 Time, int Token);
	CSubPath MoveLayerFrame(CSubPath KeyPath, int64 Time, int Token);
};

#endif
