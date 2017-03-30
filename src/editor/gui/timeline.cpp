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

#include <editor/gui/timeline.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <shared/components/assetsmanager.h>
#include <client/gui/button.h>
#include <client/gui/filler.h>
#include <client/gui/float-edit.h>
#include <client/gui/duration-edit.h>
#include <client/gui/color-edit.h>

namespace
{

/* TIME EDIT **********************************************************/

class CTimeEdit : public gui::CHListLayout
{
protected:
	class CValueEdit : public gui::CAbstractDurationEdit
	{
	protected:
		CGuiEditor* m_pAssetsEditor;
		CTimeLine* m_pTimeLine;
		
		virtual int64 GetValue() const
		{
			CSubPath FramePath;
			int Member = -1;
			if(m_pTimeLine->GetEditedFrames().size())
			{
				FramePath = m_pTimeLine->GetEditedFrames()[0];
				if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_BONEANIMATION_KEYFRAME)
					Member = CAsset_SkeletonAnimation::BONEANIMATION_KEYFRAME_TIME;
				else if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_LAYERANIMATION_KEYFRAME)
					Member = CAsset_SkeletonAnimation::LAYERANIMATION_KEYFRAME_TIME;
			}
			
			if(Member >= 0)
			{
				return m_pAssetsEditor->AssetsManager()->GetAssetValue<int64>(
					m_pTimeLine->GetAnimationPath(),
					FramePath,
					Member,
					0
				);
			}
			else
				return 0;
		}
		
		virtual void SetValue(int64 Value)
		{
			CAssetPath AnimationPath = m_pTimeLine->GetAnimationPath();
			CSubPath FramePath;
			int Member = -1;
			if(m_pTimeLine->GetEditedFrames().size())
			{
				FramePath = m_pTimeLine->GetEditedFrames()[0];
				if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_BONEANIMATION_KEYFRAME)
					Member = CAsset_SkeletonAnimation::BONEANIMATION_KEYFRAME_TIME;
				else if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_LAYERANIMATION_KEYFRAME)
					Member = CAsset_SkeletonAnimation::LAYERANIMATION_KEYFRAME_TIME;
			}
			
			int64 TimeDiff = 0;
			if(Member >= 0)
			{
				TimeDiff = Value - AssetsManager()->GetAssetValue<int64>(
					AnimationPath,
					FramePath,
					Member,
					0
				);
			}
				
			int Token = AssetsManager()->GenerateToken();
			for(unsigned int i = 0; i<m_pTimeLine->GetEditedFrames().size(); i++)
			{
				CSubPath FramePath = m_pTimeLine->GetEditedFrames()[i];
				if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_BONEANIMATION_KEYFRAME)
					Member = CAsset_SkeletonAnimation::BONEANIMATION_KEYFRAME_TIME;
				else if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_LAYERANIMATION_KEYFRAME)
					Member = CAsset_SkeletonAnimation::LAYERANIMATION_KEYFRAME_TIME;
				
				int64 Time = AssetsManager()->GetAssetValue<int64>(
					AnimationPath,
					FramePath,
					Member,
					0
				);
				
				AssetsManager()->SetAssetValue<int64>(
					AnimationPath,
					FramePath,
					Member,
					Time + TimeDiff,
					Token
				);
			}
		}
		
	public:
		CValueEdit(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine) :
			gui::CAbstractDurationEdit(pAssetsEditor),
			m_pAssetsEditor(pAssetsEditor),
			m_pTimeLine(pTimeLine)
		{ }
	};
	
protected:
	CGuiEditor* m_pAssetsEditor;
	CTimeLine* m_pTimeLine;
	
public:
	CTimeEdit(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine) :
		gui::CHListLayout(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_pTimeLine(pTimeLine)
	{
		Add(new gui::CLabel(m_pAssetsEditor, m_pAssetsEditor->m_Path_Sprite_IconTime), false);
		Add(new CValueEdit(m_pAssetsEditor, m_pTimeLine), true);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(ParentEnabled)
		{
			CSubPath FramePath;
			if(m_pTimeLine->GetEditedFrames().size())
				FramePath = m_pTimeLine->GetEditedFrames()[0];
			if(FramePath.IsNull())
				Disable();
			else if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_BONEANIMATION_KEYFRAME)
				Enable();
			else if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_LAYERANIMATION_KEYFRAME)
				Enable();
			else
				Disable();
		}
		
		gui::CHListLayout::Update(ParentEnabled);
	}
};

/* COLOR EDIT *********************************************************/

class CColorEdit : public gui::CHListLayout
{
protected:
	class CValueEdit : public gui::CAbstractColorEdit
	{
	protected:
		CGuiEditor* m_pAssetsEditor;
		CTimeLine* m_pTimeLine;
		
		virtual vec4 GetValue() const
		{
			if(m_pTimeLine->GetEditedFrames().size())
			{
				CSubPath FramePath = m_pTimeLine->GetEditedFrames()[0];
				if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_LAYERANIMATION_KEYFRAME)
				{
					return m_pAssetsEditor->AssetsManager()->GetAssetValue<vec4>(
						m_pTimeLine->GetAnimationPath(),
						FramePath,
						CAsset_SkeletonAnimation::LAYERANIMATION_KEYFRAME_COLOR,
						0
					);
				}
			}
			
			return 0;
		}
		
		virtual void SetValue(vec4 Value)
		{
			CAssetPath AnimationPath = m_pTimeLine->GetAnimationPath();
				
			int Token = AssetsManager()->GenerateToken();
			for(unsigned int i = 0; i<m_pTimeLine->GetEditedFrames().size(); i++)
			{
				CSubPath FramePath = m_pTimeLine->GetEditedFrames()[i];
				if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_LAYERANIMATION_KEYFRAME)
				{
					AssetsManager()->SetAssetValue<vec4>(
						AnimationPath,
						FramePath,
						CAsset_SkeletonAnimation::LAYERANIMATION_KEYFRAME_COLOR,
						Value,
						Token
					);
				}
			}
		}
		
	public:
		CValueEdit(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine) :
			gui::CAbstractColorEdit(pAssetsEditor),
			m_pAssetsEditor(pAssetsEditor),
			m_pTimeLine(pTimeLine)
		{ }
	};
	
protected:
	CGuiEditor* m_pAssetsEditor;
	CTimeLine* m_pTimeLine;
	
public:
	CColorEdit(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine) :
		gui::CHListLayout(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_pTimeLine(pTimeLine)
	{
		Add(new gui::CLabel(m_pAssetsEditor, m_pAssetsEditor->m_Path_Sprite_IconColor), false);
		Add(new CValueEdit(m_pAssetsEditor, m_pTimeLine), true);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(ParentEnabled)
		{
			CSubPath FramePath;
			if(m_pTimeLine->GetEditedFrames().size())
				FramePath = m_pTimeLine->GetEditedFrames()[0];
			if(FramePath.IsNull())
				Disable();
			else if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_LAYERANIMATION_KEYFRAME)
				Enable();
			else
				Disable();
		}
		
		gui::CHListLayout::Update(ParentEnabled);
	}
};

/* BONE EDIT **********************************************************/

class CBoneFrameFloatEdit : public gui::CAbstractFloatEdit
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CTimeLine* m_pTimeLine;
	int m_Member;
	float m_Factor;
	
	virtual float GetValue() const
	{
		if(m_pTimeLine->GetEditedFrames().size())
		{
			CSubPath FramePath = m_pTimeLine->GetEditedFrames()[0];
			if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_BONEANIMATION_KEYFRAME)
			{
				return m_Factor*m_pAssetsEditor->AssetsManager()->GetAssetValue<float>(
					m_pTimeLine->GetAnimationPath(),
					FramePath,
					m_Member,
					0
				);
			}
		}
		
		return 0.0f;
	}
	
	virtual void SetValue(float Value)
	{
		CAssetPath AnimationPath = m_pTimeLine->GetAnimationPath();
		int Token = AssetsManager()->GenerateToken();
		for(unsigned int i = 0; i<m_pTimeLine->GetEditedFrames().size(); i++)
		{
			CSubPath FramePath = m_pTimeLine->GetEditedFrames()[i];
			if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_BONEANIMATION_KEYFRAME)
			{
				AssetsManager()->SetAssetValue<float>(
					AnimationPath,
					FramePath,
					m_Member,
					Value/m_Factor,
					Token
				);
			}
		}
	}
	
public:
	CBoneFrameFloatEdit(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine, int Member, float Factor = 1.0f) :
		gui::CAbstractFloatEdit(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_pTimeLine(pTimeLine),
		m_Member(Member),
		m_Factor(Factor)
	{ }
};

class CTranslateEdit : public gui::CHListLayout
{
protected:
	
protected:
	CGuiEditor* m_pAssetsEditor;
	CTimeLine* m_pTimeLine;
	
public:
	CTranslateEdit(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine) :
		gui::CHListLayout(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_pTimeLine(pTimeLine)
	{
		Add(new gui::CLabel(m_pAssetsEditor, m_pAssetsEditor->m_Path_Sprite_IconTranslate), false);
		Add(new CBoneFrameFloatEdit(m_pAssetsEditor, m_pTimeLine, CAsset_SkeletonAnimation::BONEANIMATION_KEYFRAME_TRANSLATION_X), true);
		Add(new CBoneFrameFloatEdit(m_pAssetsEditor, m_pTimeLine, CAsset_SkeletonAnimation::BONEANIMATION_KEYFRAME_TRANSLATION_Y), true);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(ParentEnabled)
		{
			CSubPath FramePath;
			if(m_pTimeLine->GetEditedFrames().size())
				FramePath = m_pTimeLine->GetEditedFrames()[0];
			if(FramePath.IsNull())
				Disable();
			else if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_BONEANIMATION_KEYFRAME)
				Enable();
			else
				Disable();
		}
		
		gui::CHListLayout::Update(ParentEnabled);
	}
};

class CScaleEdit : public gui::CHListLayout
{
protected:
	
protected:
	CGuiEditor* m_pAssetsEditor;
	CTimeLine* m_pTimeLine;
	
public:
	CScaleEdit(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine) :
		gui::CHListLayout(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_pTimeLine(pTimeLine)
	{
		Add(new gui::CLabel(m_pAssetsEditor, m_pAssetsEditor->m_Path_Sprite_IconScale), false);
		Add(new CBoneFrameFloatEdit(m_pAssetsEditor, m_pTimeLine, CAsset_SkeletonAnimation::BONEANIMATION_KEYFRAME_SCALE_X), true);
		Add(new CBoneFrameFloatEdit(m_pAssetsEditor, m_pTimeLine, CAsset_SkeletonAnimation::BONEANIMATION_KEYFRAME_SCALE_Y), true);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(ParentEnabled)
		{
			CSubPath FramePath;
			if(m_pTimeLine->GetEditedFrames().size())
				FramePath = m_pTimeLine->GetEditedFrames()[0];
			if(FramePath.IsNull())
				Disable();
			else if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_BONEANIMATION_KEYFRAME)
				Enable();
			else
				Disable();
		}
		
		gui::CHListLayout::Update(ParentEnabled);
	}
};

class CAngleEdit : public gui::CHListLayout
{
protected:
	
protected:
	CGuiEditor* m_pAssetsEditor;
	CTimeLine* m_pTimeLine;
	
public:
	CAngleEdit(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine) :
		gui::CHListLayout(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_pTimeLine(pTimeLine)
	{
		Add(new gui::CLabel(m_pAssetsEditor, m_pAssetsEditor->m_Path_Sprite_IconRotate), false);
		Add(new CBoneFrameFloatEdit(m_pAssetsEditor, m_pTimeLine, CAsset_SkeletonAnimation::BONEANIMATION_KEYFRAME_ANGLE, 180.0f/Pi), true);
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(ParentEnabled)
		{
			CSubPath FramePath;
			if(m_pTimeLine->GetEditedFrames().size())
				FramePath = m_pTimeLine->GetEditedFrames()[0];
			if(FramePath.IsNull())
				Disable();
			else if(FramePath.GetType() == CAsset_SkeletonAnimation::TYPE_BONEANIMATION_KEYFRAME)
				Enable();
			else
				Disable();
		}
		
		gui::CHListLayout::Update(ParentEnabled);
	}
};

/* ZOOM EDIT **********************************************************/

class CZoomUnitButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CTimeLine* m_pTimeLine;	
	
protected:
	virtual void MouseClickAction()
	{
		m_pTimeLine->SetZoomToUnit();
	}
	
public:
	CZoomUnitButton(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine) :
		gui::CButton(pAssetsEditor, pAssetsEditor->m_Path_Sprite_IconZoomUnit),
		m_pAssetsEditor(pAssetsEditor),
		m_pTimeLine(pTimeLine)
	{
		
	}

	virtual void OnMouseMove()
	{
		if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
			m_pAssetsEditor->SetHint(_LSTRING("Set the zoom to 100 %"));
		
		gui::CButton::OnMouseMove();
	}
};

class CZoomEdit : public gui::CFloatEdit
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CTimeLine* m_pTimeLine;

protected:
	virtual float GetValue() const
	{
		return m_pTimeLine->GetPixelsPerMs();
	}
	
	virtual void SetValue(float Value)
	{
		m_pTimeLine->SetPixelsPerMs(Value);
	}
	
public:
	CZoomEdit(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine) :
		gui::CFloatEdit(pAssetsEditor, 1.0f),
		m_pAssetsEditor(pAssetsEditor),
		m_pTimeLine(pTimeLine)
	{
		Percent();
	}
};

/* TIME BUTTON ********************************************************/

class CFirstFrameButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;

protected:
	virtual void MouseClickAction()
	{
		m_pAssetsEditor->SetTime(0.0);
	}
	
public:
	CFirstFrameButton(CGuiEditor* pAssetsEditor) :
		gui::CButton(pAssetsEditor, pAssetsEditor->m_Path_Sprite_IconFirstFrame),
		m_pAssetsEditor(pAssetsEditor)
	{
		
	}
};

class CPlayPauseButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;

protected:
	virtual void MouseClickAction()
	{
		m_pAssetsEditor->SetTimePaused(!m_pAssetsEditor->GetTimePaused());
	}
	
public:
	CPlayPauseButton(CGuiEditor* pAssetsEditor) :
		gui::CButton(pAssetsEditor, pAssetsEditor->m_Path_Sprite_IconPlay),
		m_pAssetsEditor(pAssetsEditor)
	{
		
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(m_pAssetsEditor->GetTimePaused())
			SetIcon(m_pAssetsEditor->m_Path_Sprite_IconPlay);
		else
			SetIcon(m_pAssetsEditor->m_Path_Sprite_IconPause);
		
		gui::CButton::Update(ParentEnabled);
	}
};

/* LAYERLIST **********************************************************/

enum
{
	ANIMMODE_BONE=0,
	ANIMMODE_LAYER,
	ANIMMODE_TIME,
};

enum
{
	DRAGTYPE_NONE,
	DRAGTYPE_TIMESCROLL,
	DRAGTYPE_TIME,
	DRAGTYPE_BONE,
	DRAGTYPE_LAYER,
};

class CFrameList : public gui::CWidget
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CTimeLine* m_pTimeLine;
	CAssetPath m_AssetPath;
	CSubPath m_AnimationSubPath;
	CSubPath m_SkeletonSubPath; //Bone path or layer path
	CSubPath m_DragItemPath;
	int m_Mode;
	int m_DragType;
	int m_ClickDiff;
	int m_Token;
	
protected:
	void SetSkeletonBonePath(const CSubPath& SubPath)
	{
		m_SkeletonSubPath = SubPath;
		
		const CAsset_SkeletonAnimation* pAnimation = AssetsManager()->GetAsset<CAsset_SkeletonAnimation>(m_AssetPath);
		if(pAnimation)
			m_AnimationSubPath = pAnimation->FindBoneAnim(m_SkeletonSubPath);
		else
			m_AnimationSubPath = CSubPath::Null();
	}
	
	void SetSkeletonLayerPath(const CSubPath& SubPath)
	{
		m_SkeletonSubPath = SubPath;
		
		const CAsset_SkeletonAnimation* pAnimation = AssetsManager()->GetAsset<CAsset_SkeletonAnimation>(m_AssetPath);
		if(pAnimation)
			m_AnimationSubPath = pAnimation->FindLayerAnim(m_SkeletonSubPath);
		else
			m_AnimationSubPath = CSubPath::Null();
	}

	CSubPath PickBoneKeyFrame(ivec2 Pos, int* pClickDiff = nullptr)
	{
		if(pClickDiff)
			*pClickDiff = 0;
		
		const CAsset_SkeletonAnimation* pAnimation = AssetsManager()->GetAsset<CAsset_SkeletonAnimation>(m_AssetPath);
		if(!pAnimation || m_AnimationSubPath.IsNull() || !pAnimation->IsValidBoneAnimation(m_AnimationSubPath))
			return CSubPath::Null();
		
		const std::vector<CAsset_SkeletonAnimation::CBoneAnimation::CKeyFrame>& Animations = pAnimation->GetBoneAnimationKeyFrameArray(m_AnimationSubPath);
		
		for(unsigned int i=0; i<Animations.size(); i++)
		{
			int FrameX = m_DrawRect.x + m_pTimeLine->TimeToScreenPos(Animations[i].GetTime());
			
			int Diff = Pos.x - FrameX;
			if(Pos.y > m_DrawRect.y && Pos.y < m_DrawRect.y + m_DrawRect.h && std::abs(Diff) < 8)
			{
				if(pClickDiff)
					*pClickDiff = Diff;
				return CAsset_SkeletonAnimation::SubPath_BoneAnimationKeyFrame(m_AnimationSubPath.GetId(), i);
			}
		}
		
		return CSubPath::Null();
	}

	CSubPath PickLayerKeyFrame(ivec2 Pos, int* pClickDiff = nullptr)
	{
		if(pClickDiff)
			*pClickDiff = 0;
		
		const CAsset_SkeletonAnimation* pAnimation = AssetsManager()->GetAsset<CAsset_SkeletonAnimation>(m_AssetPath);
		if(!pAnimation || m_AnimationSubPath.IsNull() || !pAnimation->IsValidLayerAnimation(m_AnimationSubPath))
			return CSubPath::Null();
		
		const std::vector<CAsset_SkeletonAnimation::CLayerAnimation::CKeyFrame>& Animations = pAnimation->GetLayerAnimationKeyFrameArray(m_AnimationSubPath);
		
		for(unsigned int i=0; i<Animations.size(); i++)
		{
			int FrameX = m_DrawRect.x + m_pTimeLine->TimeToScreenPos(Animations[i].GetTime());
			
			int Diff = Pos.x - FrameX;
			if(Pos.y > m_DrawRect.y && Pos.y < m_DrawRect.y + m_DrawRect.h && std::abs(Diff) < 8)
			{
				if(pClickDiff)
					*pClickDiff = Diff;
				return CAsset_SkeletonAnimation::SubPath_LayerAnimationKeyFrame(m_AnimationSubPath.GetId(), i);
			}
		}
		
		return CSubPath::Null();
	}

	void CreateBoneFrame(int64 Time)
	{
		CSubPath FramePath;
		m_pTimeLine->CreateBoneKeyFrame(m_SkeletonSubPath, Time, m_AnimationSubPath, FramePath);
		m_pTimeLine->SetEditedFrame(FramePath);
	}
	
	void CreateLayerFrame(int64 Time)
	{
		CSubPath FramePath;
		m_pTimeLine->CreateLayerKeyFrame(m_SkeletonSubPath, Time, m_AnimationSubPath, FramePath);
		m_pTimeLine->SetEditedFrame(FramePath);
	}
	
public:
	CFrameList(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine, int Mode, const CAssetPath& AssetPath, const CSubPath& SubPath) :
		gui::CWidget(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_pTimeLine(pTimeLine),
		m_AssetPath(AssetPath),
		m_Mode(Mode),
		m_DragType(DRAGTYPE_NONE),
		m_Token(CAssetsHistory::NEW_TOKEN)
	{
		if(m_Mode == ANIMMODE_BONE)
			SetSkeletonBonePath(SubPath);
		else if(m_Mode == ANIMMODE_LAYER)
			SetSkeletonLayerPath(SubPath);
	}
	
	virtual void UpdateBoundingSize()
	{
		m_BoundingSizeRect.BSNoConstraint();
		m_BoundingSizeRect.BSAddSpacing(0, 32);
	}
	
	virtual void OnButtonClick(int Button)
	{
		ivec2 MousePos = Context()->GetMousePos();
		if(m_DrawRect.IsInside(MousePos))
		{
			if(Button == KEY_MOUSE_1)
			{
				float Time = m_pTimeLine->ScreenPosToTime(MousePos.x - m_DrawRect.x);
				
				if(m_Mode == ANIMMODE_TIME)
				{
					m_pAssetsEditor->SetTime(Time);
					m_DragType = DRAGTYPE_TIME;
				}
				else if(m_Mode == ANIMMODE_BONE)
				{
					int ClickDiff = 0;
					CSubPath PickedFramePath = PickBoneKeyFrame(MousePos, &ClickDiff);
					if(PickedFramePath.IsNull())
					{
						CreateBoneFrame(Time);
					}
					else
					{
						m_pTimeLine->SetEditedFrame(PickedFramePath);
						m_DragItemPath = PickedFramePath;
						m_DragType = DRAGTYPE_BONE;
						m_Token = AssetsManager()->GenerateToken();
						
						m_ClickDiff = ClickDiff;
					}
				}
				else if(m_Mode == ANIMMODE_LAYER)
				{
					int ClickDiff = 0;
					CSubPath PickedFramePath = PickLayerKeyFrame(MousePos, &ClickDiff);
					if(PickedFramePath.IsNull())
					{
						CreateLayerFrame(Time);
					}
					else
					{
						m_pTimeLine->SetEditedFrame(PickedFramePath);
						m_DragItemPath = PickedFramePath;
						m_DragType = DRAGTYPE_LAYER;
						m_Token = AssetsManager()->GenerateToken();
						
						m_ClickDiff = ClickDiff;
					}
				}
			}
			else if(Button == KEY_MOUSE_3)
			{
				m_DragType = DRAGTYPE_TIMESCROLL;
			}
			else if(Button == KEY_MOUSE_WHEEL_UP)
			{
				float Zoom = m_pTimeLine->GetPixelsPerMs();
				Zoom *= 1.1f;
				m_pTimeLine->SetPixelsPerMs(Zoom);
			}
			else if(Button == KEY_MOUSE_WHEEL_DOWN)
			{
				float Zoom = m_pTimeLine->GetPixelsPerMs();
				Zoom /= 1.1f;
				m_pTimeLine->SetPixelsPerMs(Zoom);
			}
		}
	}
	
	virtual void OnButtonRelease(int Button)
	{
		if(Button == KEY_MOUSE_1 || Button == KEY_MOUSE_3)
		{
			m_DragType = DRAGTYPE_NONE;
			m_DragItemPath = CSubPath::Null();
			m_Token = CAssetsHistory::NEW_TOKEN;
		}
	}
	
	virtual void OnMouseMove()
	{		
		if(m_DragType == DRAGTYPE_TIMESCROLL)
		{
			
		}
		else if(m_DragType == DRAGTYPE_TIME)
		{
			ivec2 MousePos = Context()->GetMousePos();
			int64 Time = max(m_pTimeLine->ScreenPosToTime(MousePos.x - m_DrawRect.x), (int64) 0);
			
			m_pAssetsEditor->SetTime(Time);
		}
		else if(m_DragType == DRAGTYPE_BONE)
		{
			ivec2 MousePos = Context()->GetMousePos();
			int64 Time = max(m_pTimeLine->ScreenPosToTime(MousePos.x - m_DrawRect.x - m_ClickDiff), (int64) 0);
			
			m_DragItemPath = m_pTimeLine->MoveBoneFrame(m_DragItemPath, Time, m_Token);
		}
		else if(m_DragType == DRAGTYPE_LAYER)
		{
			ivec2 MousePos = Context()->GetMousePos();
			int64 Time = max(m_pTimeLine->ScreenPosToTime(MousePos.x - m_DrawRect.x - m_ClickDiff), (int64) 0);
			
			m_DragItemPath = m_pTimeLine->MoveLayerFrame(m_DragItemPath, Time, m_Token);
		}
	}
	
	void RenderInterpolationCurve(int Type, float X0, float X1, float Y0, float Y1)
	{
		Graphics()->TextureClear();
		Graphics()->LinesBegin();
		
		Graphics()->SetColor(1.0f, true);
		
		float dt = (X1-X0);
		
		switch(Type)
		{
			case CAsset_SkeletonAnimation::GRAPHTYPE_STEPSTART:
			{
				CGraphics::CLineItem aLines[] = {
						CGraphics::CLineItem(X0, Y0, X0, Y1),
						CGraphics::CLineItem(X0, Y1, X1, Y1)
				};
				Graphics()->LinesDraw(aLines, sizeof(aLines)/sizeof(aLines[0]));
				break;
			}
			case CAsset_SkeletonAnimation::GRAPHTYPE_STEPEND:
			{
				CGraphics::CLineItem aLines[] = {
						CGraphics::CLineItem(X0, Y0, X1, Y0),
						CGraphics::CLineItem(X1, Y0, X1, Y1)
				};
				Graphics()->LinesDraw(aLines, sizeof(aLines)/sizeof(aLines[0]));
				break;
			}
			case CAsset_SkeletonAnimation::GRAPHTYPE_STEPMIDDLE:
			{
				CGraphics::CLineItem aLines[] = {
						CGraphics::CLineItem(X0, Y0, (X0+X1)/2.0f, Y0),
						CGraphics::CLineItem((X0+X1)/2.0f, Y0, (X0+X1)/2.0f, Y1),
						CGraphics::CLineItem((X0+X1)/2.0f, Y1, X1, Y1)
				};
				Graphics()->LinesDraw(aLines, sizeof(aLines)/sizeof(aLines[0]));
				break;
			}
			case CAsset_SkeletonAnimation::GRAPHTYPE_LINEAR:
			{
				CGraphics::CLineItem aLines[] = {
						CGraphics::CLineItem(X0, Y0, X1, Y1)
				};
				Graphics()->LinesDraw(aLines, sizeof(aLines)/sizeof(aLines[0]));
				break;
			}
			case CAsset_SkeletonAnimation::GRAPHTYPE_ACCELERATION:
			{
				CGraphics::CLineItem aLines[10];
				for(int i=0; i<10; i++)
				{
					float a0 = (i/10.0f);
					float a1 = ((i+1)/10.0f);
					float b0 = a0*a0*a0;
					float b1 = a1*a1*a1;
					aLines[i] = CGraphics::CLineItem(X0+a0*dt, Y0+b0*(Y1-Y0), X0+a1*dt, Y0+b1*(Y1-Y0));
				}
				Graphics()->LinesDraw(aLines, sizeof(aLines)/sizeof(aLines[0]));
				break;
			}
			case CAsset_SkeletonAnimation::GRAPHTYPE_DECELERATION:
			{
				CGraphics::CLineItem aLines[10];
				for(int i=0; i<10; i++)
				{
					float a0 = (i/10.0f);
					float a1 = ((i+1)/10.0f);
					float b0 = a0*(3.0f-a0*(3.0f-a0));
					float b1 = a1*(3.0f-a1*(3.0f-a1));
					aLines[i] = CGraphics::CLineItem(X0+a0*dt, Y0+b0*(Y1-Y0), X0+a1*dt, Y0+b1*(Y1-Y0));
				}
				Graphics()->LinesDraw(aLines, sizeof(aLines)/sizeof(aLines[0]));
				break;
			}
			case CAsset_SkeletonAnimation::GRAPHTYPE_SMOOTH:
			{
				CGraphics::CLineItem aLines[10];
				for(int i=0; i<10; i++)
				{
					float a0 = (i/10.0f);
					float a1 = ((i+1)/10.0f);
					float b0 = -2.0f*a0*a0*a0 + 3*a0*a0;
					float b1 = -2.0f*a1*a1*a1 + 3.0f*a1*a1;
					aLines[i] = CGraphics::CLineItem(X0+a0*dt, Y0+b0*(Y1-Y0), X0+a1*dt, Y0+b1*(Y1-Y0));
				}
				Graphics()->LinesDraw(aLines, sizeof(aLines)/sizeof(aLines[0]));
				break;
			}
		}
		
		Graphics()->LinesEnd();
	}
	
	virtual void Render()
	{
		gui::CRect TimelineRect = m_DrawRect;
		
		//Draw box
		const CAsset_GuiBoxStyle* pStyle = AssetsManager()->GetAsset<CAsset_GuiBoxStyle>(m_pAssetsEditor->m_Path_Box_FrameList);
		if(pStyle)
		{
			TimelineRect.RemoveMargin(Context()->ApplyGuiScale(pStyle->GetMargin()));
			AssetsRenderer()->DrawGuiRect(&TimelineRect, pStyle->GetRectPath());
			TimelineRect.RemoveMargin(Context()->ApplyGuiScale(pStyle->GetPadding()));
		}
		
		Graphics()->ClipPush(TimelineRect.x, TimelineRect.y, TimelineRect.w, TimelineRect.h);
		
		//Draw rules
		int64 TimeMin = m_pTimeLine->ScreenPosToTime(0);
		int64 TimeMax = m_pTimeLine->ScreenPosToTime(TimelineRect.w);
		int64 TimeLength = TimeMax - TimeMin;
		int64 TimeSegment;
		static int64 s_aPossibleSegmentLength[] = {20, 100, 1000, 5*1000, 10*1000, 60*1000, 5*60*1000, 15*60*1000, 60*60*1000, 6*60*60*1000, 24*60*60*1000};
		for(unsigned int i=0; i<sizeof(s_aPossibleSegmentLength)/sizeof(s_aPossibleSegmentLength[0]); i++)
		{
			TimeSegment = s_aPossibleSegmentLength[i];
			if(TimeLength/s_aPossibleSegmentLength[i] < 16)
				break;
		}
		
		int64 TimeIter = TimeMin;
		while(TimeIter < TimeMax)
		{
			int XMin = TimelineRect.x + m_pTimeLine->TimeToScreenPos(TimeIter);
			
			//Quad
			if((TimeIter/TimeSegment)%2)
			{
				int XMax = TimelineRect.x + m_pTimeLine->TimeToScreenPos(TimeIter+TimeSegment);
					
				Graphics()->TextureClear();
				Graphics()->QuadsBegin();
				
				Graphics()->SetColor(vec4(0.0f, 0.0f, 0.0f, 0.25f), true);
				CGraphics::CQuadItem QuadItem(
					(XMin+XMax)/2.0f,
					TimelineRect.y+TimelineRect.h/2,
					XMax-XMin,
					TimelineRect.h
				);
				Graphics()->QuadsDraw(&QuadItem, 1);
				
				Graphics()->QuadsEnd();
			}
			
			//Text
			if(m_Mode == ANIMMODE_TIME)
			{
				dynamic_string Buffer;
				Localization()->FormatDouble(Buffer, NULL, TimeIter/1000.0);
				
				CTextRenderer::CTextCache TextCache;
				TextCache.SetText(Buffer.buffer());
				TextCache.SetBoxSize(ivec2(0, TimelineRect.h));
				TextCache.SetFontSize(12.0f);
				float TextWidth = TextRenderer()->GetTextWidth(&TextCache);
				TextRenderer()->DrawText(&TextCache, ivec2(XMin - TextWidth/2.0f, TimelineRect.y), 1.0f);
			}
			
			TimeIter += TimeSegment;
		}
		
		//Draw frames
		ivec2 MousePos = Context()->GetMousePos();
		
		float TimePos = TimelineRect.x + m_pTimeLine->TimeToScreenPos(m_pAssetsEditor->GetTime());
		{
			Graphics()->TextureClear();
			Graphics()->LinesBegin();
			
			Graphics()->SetColor(1.0f, true);
			CGraphics::CLineItem Line(TimePos+0.5f, TimelineRect.y, TimePos+0.5f, TimelineRect.y + TimelineRect.h);
			Graphics()->LinesDraw(&Line, 1);
			
			Graphics()->LinesEnd();
		}
		
		bool CanAddNewFrame = true;
		
		const CAsset_SkeletonAnimation* pAnimation = AssetsManager()->GetAsset<CAsset_SkeletonAnimation>(m_AssetPath);
		if(pAnimation)
		{
			if(m_Mode == ANIMMODE_BONE)
			{
				if(m_AnimationSubPath.IsNotNull())
				{
					const std::vector<CAsset_SkeletonAnimation::CBoneAnimation::CKeyFrame>& Animations = pAnimation->GetBoneAnimationKeyFrameArray(m_AnimationSubPath);
					
					CSubPath PickedFramePath = PickBoneKeyFrame(MousePos);
					
					for(unsigned int i=0; i<Animations.size(); i++)
					{
						CSubPath FramePath = CAsset_SkeletonAnimation::SubPath_BoneAnimationKeyFrame(m_AnimationSubPath.GetId(), i);
						
						int FrameX = TimelineRect.x + m_pTimeLine->TimeToScreenPos(Animations[i].GetTime());
						
						//Draw interpolation curve
						if(i+1 < Animations.size())
						{
							int FrameXNext = TimelineRect.x + m_pTimeLine->TimeToScreenPos(Animations[i+1].GetTime());
							if(FrameXNext - FrameX > 100)
							{
								RenderInterpolationCurve(
									Animations[i+1].GetGraphType(),
									FrameX+0.5f+16, FrameXNext+0.5f-16, TimelineRect.y + TimelineRect.h, TimelineRect.y
								);
							}
						}
						
						//Draw frame
						bool ShowHighlight = false;
						for(unsigned j=0; j<m_pTimeLine->GetEditedFrames().size(); j++)
						{
							if(m_pTimeLine->GetEditedFrames()[j] == FramePath)
							{
								ShowHighlight = true;
								break;
							}
						}
						
						AssetsRenderer()->DrawSprite(
							m_pAssetsEditor->m_Path_Sprite_GizmoFrame,
							vec2(FrameX, TimelineRect.y + TimelineRect.h/2),
							1.0f, 0.0f, 0x0, 1.0f
						);
						
						if(ShowHighlight || (PickedFramePath.IsNotNull() && PickedFramePath.GetId2() == i))
						{
							CanAddNewFrame = false;
							AssetsRenderer()->DrawSprite(
								m_pAssetsEditor->m_Path_Sprite_GizmoFrameCursor,
								vec2(FrameX, TimelineRect.y + TimelineRect.h/2),
								1.0f, 0.0f, 0x0, vec4(1.0f, 1.0f, 0.0f, 1.0f)
							);
						}
					}
				}
			}
			else if(m_Mode == ANIMMODE_LAYER)
			{
				if(m_AnimationSubPath.IsNotNull())
				{
					const std::vector<CAsset_SkeletonAnimation::CLayerAnimation::CKeyFrame>& Animations = pAnimation->GetLayerAnimationKeyFrameArray(m_AnimationSubPath);
					
					CSubPath PickedFramePath = PickLayerKeyFrame(MousePos);
					
					for(unsigned int i=0; i<Animations.size(); i++)
					{
						int FrameX = TimelineRect.x + m_pTimeLine->TimeToScreenPos(Animations[i].GetTime());
						
						//Draw interpolation curve
						if(i+1 < Animations.size())
						{
							int FrameXNext = TimelineRect.x + m_pTimeLine->TimeToScreenPos(Animations[i+1].GetTime());
							if(FrameXNext - FrameX > 100)
							{
								RenderInterpolationCurve(
									Animations[i+1].GetGraphType(),
									FrameX+0.5f+16, FrameXNext+0.5f-16, TimelineRect.y + TimelineRect.h, TimelineRect.y
								);
							}
						}
						
						//Draw frame
						vec4 Color = Animations[i].GetColor();
						
						AssetsRenderer()->DrawSprite(
							m_pAssetsEditor->m_Path_Sprite_GizmoFrameLayerBg,
							vec2(FrameX, TimelineRect.y + TimelineRect.h/2),
							1.0f, 0.0f, 0x0, 1.0f
						);
						AssetsRenderer()->DrawSprite(
							m_pAssetsEditor->m_Path_Sprite_GizmoFrameLayerColor,
							vec2(FrameX, TimelineRect.y + TimelineRect.h/2),
							1.0f, 0.0f, 0x0, vec4(Color.r, Color.g, Color.b, 1.0f)
						);
						AssetsRenderer()->DrawSprite(
							m_pAssetsEditor->m_Path_Sprite_GizmoFrameLayerAlpha,
							vec2(FrameX, TimelineRect.y + TimelineRect.h/2),
							1.0f, 0.0f, 0x0, Color
						);
						
						if(PickedFramePath.IsNotNull() && PickedFramePath.GetId2() == i)
						{
							CanAddNewFrame = false;
							AssetsRenderer()->DrawSprite(
								m_pAssetsEditor->m_Path_Sprite_GizmoFrameCursor,
								vec2(FrameX, TimelineRect.y + TimelineRect.h/2),
								1.0f, 0.0f, 0x0, vec4(1.0f, 1.0f, 0.0f, 1.0f)
							);
						}
					}
				}
			}
		}
		
		if(m_Mode != ANIMMODE_TIME && CanAddNewFrame)
		{
			if(TimelineRect.IsInside(MousePos))
			{
				int64 MouseTime = m_pTimeLine->ScreenPosToTime(MousePos.x - TimelineRect.x);
				int FramePos = m_pTimeLine->TimeToScreenPos(MouseTime) + TimelineRect.x;
				AssetsRenderer()->DrawSprite(
					m_pAssetsEditor->m_Path_Sprite_GizmoFrameCursor,
					vec2(FramePos, TimelineRect.y + TimelineRect.h/2),
					1.0f, 0.0f, 0x0, vec4(0.0f, 0.8f, 0.0f, 1.0f)
				);
			}
		}
		
		Graphics()->ClipPop();
	}
};

class CAnimationItem : public gui::CHListLayout
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CTimeLine* m_pTimeLine;

public:
	CAnimationItem(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine, int Mode, const CAssetPath& AssetPath, const CSubPath& SubPath, const char* pText, CAssetPath IconPath) :
		gui::CHListLayout(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_pTimeLine(pTimeLine)
	{
		gui::CLabel* pLabel = new gui::CLabel(m_pAssetsEditor, pText, IconPath);
		Add(pLabel, false, 150);
		Add(new CFrameList(m_pAssetsEditor, m_pTimeLine, Mode, AssetPath, SubPath), true);
	}
};

class CAnimationList : public gui::CVScrollLayout
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CTimeLine* m_pTimeLine;

protected:
	void Refresh()
	{
		Clear();
		
		//Skeleton animations
		CAssetPath AnimationPath = m_pTimeLine->GetAnimationPath();
		const CAsset_SkeletonAnimation* pSkeletonAnimation = AssetsManager()->GetAsset<CAsset_SkeletonAnimation>(AnimationPath);
		if(!pSkeletonAnimation)
			return;
		
		if(pSkeletonAnimation->GetSkeletonPath().IsNull())
		{
			//Animation without a skeleton -> Add global position and global color
			Add(new CAnimationItem(m_pAssetsEditor, m_pTimeLine, ANIMMODE_BONE, AnimationPath, CSubPath::Null(), "Global Transform", m_pAssetsEditor->m_Path_Sprite_IconBone));
			Add(new CAnimationItem(m_pAssetsEditor, m_pTimeLine, ANIMMODE_LAYER, AnimationPath, CSubPath::Null(), "Global Color", m_pAssetsEditor->m_Path_Sprite_IconLayer));
		}
		else
		{
			const CAsset_Skeleton* pSkeleton = AssetsManager()->GetAsset<CAsset_Skeleton>(pSkeletonAnimation->GetSkeletonPath());
			if(!pSkeleton)
				return;
			
			//Animation with a skeleton -> Add global position, each bone, global color and each layer
			Add(new CAnimationItem(m_pAssetsEditor, m_pTimeLine, ANIMMODE_BONE, AnimationPath, CSubPath::Null(), "Global Transform", m_pAssetsEditor->m_Path_Sprite_IconBone));
			
			{
				CAsset_Skeleton::CIteratorBone Iter;
				for(Iter = pSkeleton->BeginBone(); Iter != pSkeleton->EndBone(); ++Iter)
				{
					Add(new CAnimationItem(m_pAssetsEditor, m_pTimeLine, ANIMMODE_BONE, AnimationPath, *Iter, pSkeleton->GetBoneName(*Iter), m_pAssetsEditor->m_Path_Sprite_IconBone));
				}
			}
			
			Add(new CAnimationItem(m_pAssetsEditor, m_pTimeLine, ANIMMODE_LAYER, AnimationPath, CSubPath::Null(), "Global Color", m_pAssetsEditor->m_Path_Sprite_IconLayer));
			
			{
				CAsset_Skeleton::CIteratorLayer Iter;
				for(Iter = pSkeleton->BeginLayer(); Iter != pSkeleton->EndLayer(); ++Iter)
				{
					Add(new CAnimationItem(m_pAssetsEditor, m_pTimeLine, ANIMMODE_LAYER, AnimationPath, *Iter, pSkeleton->GetLayerName(*Iter), m_pAssetsEditor->m_Path_Sprite_IconLayer));
				}
			}
		}
	}

public:
	CAnimationList(CGuiEditor* pAssetsEditor, CTimeLine* pTimeLine) :
		gui::CVScrollLayout(pAssetsEditor),
		m_pAssetsEditor(pAssetsEditor),
		m_pTimeLine(pTimeLine)
	{
		
	}
	
	virtual void Update(bool ParentEnabled)
	{
		if(m_pTimeLine->RefreshNeeded())
			Refresh();
		
		gui::CVScrollLayout::Update(ParentEnabled);
	}
};

}

/* TIMELINE ***********************************************************/

CTimeLine::CTimeLine(CGuiEditor* pAssetsEditor) :
	gui::CVListLayout(pAssetsEditor),
	m_pAssetsEditor(pAssetsEditor),
	m_EditedAssetVersion(-1),
	m_TimeShift(0),
	m_PixelsPerMs(0.25f),
	m_ZoomLockedToUnit(false)
{
	gui::CHListLayout* pToolbar = new gui::CHListLayout(Context());
	Add(pToolbar, false);
	pToolbar->SetBoxStyle(AssetsEditor()->m_Path_Box_Panel);
	
	pToolbar->Add(new CFirstFrameButton(AssetsEditor()), false);
	pToolbar->Add(new CPlayPauseButton(AssetsEditor()), false);
	pToolbar->AddSeparator();
	pToolbar->Add(new CTimeEdit(AssetsEditor(), this), false, 150);
	pToolbar->Add(new CTranslateEdit(AssetsEditor(), this), false, 225);
	pToolbar->Add(new CAngleEdit(AssetsEditor(), this), false, 150);
	pToolbar->Add(new CScaleEdit(AssetsEditor(), this), false, 225);
	pToolbar->Add(new CColorEdit(AssetsEditor(), this), false, 150);
	pToolbar->Add(new gui::CFiller(Context()), true);
	pToolbar->Add(new CZoomUnitButton(AssetsEditor(), this), false);
	pToolbar->Add(new CZoomEdit(AssetsEditor(), this), false, 75);
	
	gui::CVListLayout* pList = new gui::CVListLayout(Context());
	Add(pList, true);
	pList->SetBoxStyle(m_pAssetsEditor->m_Path_Box_Panel);
	pList->Add(new CAnimationItem(AssetsEditor(), this, ANIMMODE_TIME, CAssetPath::Null(), CSubPath::Null(), "Time", CAssetPath::Null()), false);
	pList->AddSeparator();
	pList->Add(new CAnimationList(AssetsEditor(), this), true);
}

CAssetPath CTimeLine::GetAnimationPath()
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_SkeletonAnimation::TypeId)
	{
		return AssetsEditor()->GetEditedAssetPath();
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
	{
		const CAsset_MapLayerQuads* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
		CSubPath QuadPath = AssetsEditor()->GetFirstEditedSubPath();
		if(pLayer && QuadPath.GetType() == CAsset_MapLayerQuads::TYPE_QUAD && pLayer->IsValidQuad(QuadPath))
			return pLayer->GetQuadAnimationPath(QuadPath);
	}
	
	return CAssetPath::Null();
}

void CTimeLine::Update(bool ParentEnabled)
{
	m_NeedRefresh = false;
	
	if(ParentEnabled)
	{
		switch(AssetsEditor()->GetEditedAssetPath().GetType())
		{
			case CAsset_Map::TypeId:
			case CAsset_MapGroup::TypeId:
			case CAsset_MapLayerTiles::TypeId:
			case CAsset_MapLayerQuads::TypeId:
			case CAsset_MapLayerObjects::TypeId:
			case CAsset_MapZoneTiles::TypeId:
			case CAsset_MapZoneObjects::TypeId:
			case CAsset_SkeletonAnimation::TypeId:
				Enable();
				break;
			default:
				Disable();
		}
	}
	
	if(ParentEnabled && IsEnabled())
	{
		if(m_EditedAssetPath != GetAnimationPath())
		{
			m_NeedRefresh = true;
		}
		else
		{
			CAssetState* pState = AssetsManager()->GetAssetState(m_EditedAssetPath);
			
			if(pState && pState->m_NumUpdates != m_EditedAssetVersion)
				m_NeedRefresh = true;
		}

		if(m_NeedRefresh)
		{
			m_EditedFrames.clear();
			
			CAssetState* pState = AssetsManager()->GetAssetState(m_EditedAssetPath);
			m_EditedAssetPath = GetAnimationPath();
			m_EditedAssetVersion = (pState ? pState->m_NumUpdates : 0);
		}
	}
	
	gui::CVListLayout::Update(ParentEnabled);
}

bool CTimeLine::CreateBoneKeyFrame(CSubPath BonePath, int64 Time, CSubPath& AnimationSubPath, CSubPath& FramePath)
{
	CAssetPath AnimationPath = GetAnimationPath();
	CAsset_SkeletonAnimation* pAnimation = AssetsManager()->GetAsset_Hard<CAsset_SkeletonAnimation>(AnimationPath);
	if(!pAnimation)
		return false;

	int Token = AssetsManager()->GenerateToken();
	AssetsManager()->SaveAssetInHistory(AnimationPath, Token);

	//Check if a new animation is needed
	AnimationSubPath = pAnimation->FindBoneAnim(BonePath);
	if(AnimationSubPath.IsNull() || !pAnimation->IsValidBoneAnimation(AnimationSubPath))
	{
		AnimationSubPath = CAsset_SkeletonAnimation::SubPath_BoneAnimation(pAnimation->GetBoneAnimationArray().size());
		pAnimation->GetBoneAnimationArray().emplace_back();
		pAnimation->GetBoneAnimationArray().back().SetBonePath(BonePath);
	}
	
	//Get the animation
	CAsset_SkeletonAnimation::CBoneAnimation& BoneAnimation = pAnimation->GetBoneAnimationArray()[AnimationSubPath.GetId()];
	
	//Get the frame
	CAsset_SkeletonAnimation::CBoneAnimation::CFrame Frame;
	BoneAnimation.GetFrame(Time, Frame);
	
	//Insert the frame
	int KeyFrameId = BoneAnimation.TimeToKeyFrame(Time);
	{
		FramePath = CAsset_SkeletonAnimation::SubPath_BoneAnimationKeyFrame(AnimationSubPath.GetId(), KeyFrameId);
		auto InsertPos = BoneAnimation.GetKeyFrameArray().begin() + KeyFrameId;
		auto Iter = BoneAnimation.GetKeyFrameArray().insert(InsertPos, CAsset_SkeletonAnimation::CBoneAnimation::CKeyFrame());
		*Iter = Frame;
		Iter->SetTime(Time);
	}
	
	return true;
}

bool CTimeLine::CreateLayerKeyFrame(CSubPath LayerPath, int64 Time, CSubPath& AnimationSubPath, CSubPath& FramePath)
{
	CAssetPath AnimationPath = GetAnimationPath();
	CAsset_SkeletonAnimation* pAnimation = AssetsManager()->GetAsset_Hard<CAsset_SkeletonAnimation>(AnimationPath);
	if(!pAnimation)
		return false;

	int Token = AssetsManager()->GenerateToken();
	AssetsManager()->SaveAssetInHistory(AnimationPath, Token);

	//Check if a new animation is needed
	AnimationSubPath = pAnimation->FindLayerAnim(LayerPath);
	if(AnimationSubPath.IsNull() || !pAnimation->IsValidLayerAnimation(AnimationSubPath))
	{
		AnimationSubPath = CAsset_SkeletonAnimation::SubPath_LayerAnimation(pAnimation->GetLayerAnimationArray().size());
		pAnimation->GetLayerAnimationArray().emplace_back();
		pAnimation->GetLayerAnimationArray().back().SetLayerPath(LayerPath);
	}
	
	//Get the animation
	CAsset_SkeletonAnimation::CLayerAnimation& LayerAnimation = pAnimation->GetLayerAnimationArray()[AnimationSubPath.GetId()];
	
	//Get the frame
	CAsset_SkeletonAnimation::CLayerAnimation::CFrame Frame;
	LayerAnimation.GetFrame(Time, Frame);
	
	//Insert the frame
	int KeyFrameId = LayerAnimation.TimeToKeyFrame(Time);
	{
		FramePath = CAsset_SkeletonAnimation::SubPath_LayerAnimationKeyFrame(AnimationSubPath.GetId(), KeyFrameId);
		auto InsertPos = LayerAnimation.GetKeyFrameArray().begin() + KeyFrameId;
		auto Iter = LayerAnimation.GetKeyFrameArray().insert(InsertPos, CAsset_SkeletonAnimation::CLayerAnimation::CKeyFrame());
		*Iter = Frame;
		Iter->SetTime(Time);
	}
	
	return true;
}

CSubPath CTimeLine::MoveBoneFrame(CSubPath FramePath, int64 Time, int Token)
{
	CSubPath OldPath = FramePath;
	
	CAssetPath AnimationPath = GetAnimationPath();
	CAsset_SkeletonAnimation* pAnimation = AssetsManager()->GetAsset_Hard<CAsset_SkeletonAnimation>(AnimationPath);
	if(!pAnimation)
		return FramePath;
	
	CSubPath AnimationSubPath = CAsset_SkeletonAnimation::SubPath_BoneAnimation(FramePath.GetId());
	
	if(AnimationSubPath.IsNull() || !pAnimation->IsValidBoneAnimation(AnimationSubPath))
		return FramePath;
	
	CAsset_SkeletonAnimation::CBoneAnimation& BoneAnimation = pAnimation->GetBoneAnimationArray()[AnimationSubPath.GetId()];
	
	if(FramePath.IsNull() || !pAnimation->IsValidBoneAnimationKeyFrame(FramePath))
		return FramePath;
	
	AssetsManager()->SaveAssetInHistory(AnimationPath, Token);
	
	//Copy the frame
	CAsset_SkeletonAnimation::CBoneAnimation::CKeyFrame KeyFrame = pAnimation->GetBoneAnimationKeyFrame(FramePath);
	
	//Remove the frame
	BoneAnimation.GetKeyFrameArray().erase(BoneAnimation.GetKeyFrameArray().begin() + FramePath.GetId2());
	
	//Insert the frame
	int KeyFrameId = BoneAnimation.TimeToKeyFrame(Time);
	{
		FramePath = CAsset_SkeletonAnimation::SubPath_BoneAnimationKeyFrame(AnimationSubPath.GetId(), KeyFrameId);
		auto InsertPos = BoneAnimation.GetKeyFrameArray().begin() + KeyFrameId;
		auto Iter = BoneAnimation.GetKeyFrameArray().insert(InsertPos, CAsset_SkeletonAnimation::CBoneAnimation::CKeyFrame());
		*Iter = KeyFrame;
		Iter->SetTime(Time);
	}
	
	for(unsigned int i=0; i<m_EditedFrames.size(); i++)
	{
		if(m_EditedFrames[i] == OldPath)
			m_EditedFrames[i] = FramePath;
	}
	
	return FramePath;
}

CSubPath CTimeLine::MoveLayerFrame(CSubPath FramePath, int64 Time, int Token)
{
	CSubPath OldPath = FramePath;
	
	CAssetPath AnimationPath = GetAnimationPath();
	CAsset_SkeletonAnimation* pAnimation = AssetsManager()->GetAsset_Hard<CAsset_SkeletonAnimation>(AnimationPath);
	if(!pAnimation)
		return FramePath;
	
	CSubPath AnimationSubPath = CAsset_SkeletonAnimation::SubPath_LayerAnimation(FramePath.GetId());
	
	if(AnimationSubPath.IsNull() || !pAnimation->IsValidLayerAnimation(AnimationSubPath))
		return FramePath;
	
	CAsset_SkeletonAnimation::CLayerAnimation& LayerAnimation = pAnimation->GetLayerAnimationArray()[AnimationSubPath.GetId()];
	
	if(FramePath.IsNull() || !pAnimation->IsValidLayerAnimationKeyFrame(FramePath))
		return FramePath;
	
	AssetsManager()->SaveAssetInHistory(AnimationPath, Token);
	
	//Copy the frame
	CAsset_SkeletonAnimation::CLayerAnimation::CKeyFrame KeyFrame = pAnimation->GetLayerAnimationKeyFrame(FramePath);
	
	//Remove the frame
	LayerAnimation.GetKeyFrameArray().erase(LayerAnimation.GetKeyFrameArray().begin() + FramePath.GetId2());
	
	//Insert the frame
	int KeyFrameId = LayerAnimation.TimeToKeyFrame(Time);
	{
		FramePath = CAsset_SkeletonAnimation::SubPath_LayerAnimationKeyFrame(AnimationSubPath.GetId(), KeyFrameId);
		auto InsertPos = LayerAnimation.GetKeyFrameArray().begin() + KeyFrameId;
		auto Iter = LayerAnimation.GetKeyFrameArray().insert(InsertPos, CAsset_SkeletonAnimation::CLayerAnimation::CKeyFrame());
		*Iter = KeyFrame;
		Iter->SetTime(Time);
	}
	
	for(unsigned int i=0; i<m_EditedFrames.size(); i++)
	{
		if(m_EditedFrames[i] == OldPath)
			m_EditedFrames[i] = FramePath;
	}
	
	return FramePath;
}
