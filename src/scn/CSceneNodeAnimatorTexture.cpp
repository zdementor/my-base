//|-------------------------------------------------------------------------
//| File:        CSceneNodeAnimatorTexture.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CSceneNodeAnimatorTexture.h"
#include <os/ITimer.h>
#include <io/ILogger.h>
#include <scn/IBillboardSceneNode.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! constructor 1
CSceneNodeAnimatorTexture::CSceneNodeAnimatorTexture(
	u32 start_time,
	const core::array<SAnimationFrame>& textures, 
	const SAnimationFrameParameters &params 
	) 
: ISceneNodeAnimator(0),
m_ActiveDirectionSetIndex(-1), m_CurrentFrameIndex(0), m_AnimationEnded(false), m_TextureAnimatorParameters(params)
{
#if MY_DEBUG_MODE 
	setClassName("CSceneNodeAnimatorTexture");        
#endif

	setFramesDirectionSet(0, textures);
	activateFramesDirectionSet(0);
}

//---------------------------------------------------------------------------

//! constructor 2
CSceneNodeAnimatorTexture::CSceneNodeAnimatorTexture(
	u32 start_time,
	const SAnimationFrameParameters &params 
	) 
: ISceneNodeAnimator(0), 
m_ActiveDirectionSetIndex(-1), m_AnimationEnded(false), m_TextureAnimatorParameters(params)
{
#if MY_DEBUG_MODE 
	setClassName("CSceneNodeAnimatorTexture");        
#endif

	activateFramesDirectionSet(0);
}

//---------------------------------------------------------------------------

//! destructor
CSceneNodeAnimatorTexture::~CSceneNodeAnimatorTexture()
{
	for (u32 ts=0; ts<m_DirectionSets.size(); ++ts)
	{
		for (u32 i=0; i<m_DirectionSets[ts].size(); ++i)
			SAFE_DROP(m_DirectionSets[ts][i].Texture);		
	}	
}

//---------------------------------------------------------------------------

//! animates a scene node
void CSceneNodeAnimatorTexture::animateNode(ISceneNode* node, u32 timeMs)
{
	if (!IsEnabled || 
		m_ActiveDirectionSetIndex >= m_DirectionSets.size() ||
		m_DirectionSets[m_ActiveDirectionSetIndex].size() <= 0)
		return;

	if (!StartTime)
		StartTime = timeMs;

	m_EndTime = StartTime + (
		m_TextureAnimatorParameters.TimePerFrame * 
		m_DirectionSets[m_ActiveDirectionSetIndex].size());

    u32 t = (timeMs-StartTime);

    m_CurrentFrameIndex = 0;

	SAnimationFrameParameters &p = m_TextureAnimatorParameters;

    if (!p.Loop && timeMs >= m_EndTime)
	{
        m_CurrentFrameIndex = m_DirectionSets[m_ActiveDirectionSetIndex].size() - 1;
		m_AnimationEnded = true;
	}
    else 
        m_CurrentFrameIndex = p.TimePerFrame ?
			(t/p.TimePerFrame) % m_DirectionSets[m_ActiveDirectionSetIndex].size() : 0;

	if (m_AnimationEnded)
		m_CurrentFrameIndex = m_DirectionSets[m_ActiveDirectionSetIndex].size() - 1;

    if (m_CurrentFrameIndex < (s32)m_DirectionSets[m_ActiveDirectionSetIndex].size())
	{
		if (node->getMaterialsCount()==1)
		{
			SAnimationFrame &fr =
				m_DirectionSets[m_ActiveDirectionSetIndex][m_CurrentFrameIndex];
			node->getMaterial(0).getPass(0).Layers[0].setTexture(fr.Texture);
			if (node->getSceneNodeType()==ESNT_BILLBOARD_SCENE_NODE)
				((IBillboardSceneNode*)node)->setTCoordsRect(fr.TCoords);
		}
	}
	else
		m_CurrentFrameIndex = 0;
}

//---------------------------------------------------------------------------

void CSceneNodeAnimatorTexture::setFramesDirectionSet(
	u32 direction_idx, const core::array<SAnimationFrame> &frames)
{
	if (direction_idx >= m_DirectionSets.size())
		for (u32 d = m_DirectionSets.size(); d <= direction_idx; d++)
			m_DirectionSets.push_back(core::array<SAnimationFrame>());
	u32 i=0;
	for (i=0; i < m_DirectionSets[direction_idx].size(); ++i)
        SAFE_DROP(m_DirectionSets[direction_idx][i].Texture);
	m_DirectionSets[direction_idx].set_used(0);
	for (i = 0; i < frames.size(); ++i)
    {
        m_DirectionSets[direction_idx].push_back(frames[i]);
		SAFE_GRAB(frames[i].Texture);
    }
}

//---------------------------------------------------------------------------

const core::array<SAnimationFrame>& CSceneNodeAnimatorTexture::getFramesDirectionSet(
	u32 direction_idx)
{
	if (direction_idx >= m_DirectionSets.size())
	// boom :), better not be here
		return *((const core::array<SAnimationFrame>*)0);
	return m_DirectionSets[direction_idx];
}

//---------------------------------------------------------------------------

bool CSceneNodeAnimatorTexture::activateFramesDirectionSet(u32 direction_idx)
{
	if (direction_idx < m_DirectionSets.size())
	{
		m_ActiveDirectionSetIndex = direction_idx;		
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

