//|-------------------------------------------------------------------------
//| File:        CSceneNodeAnimatorTexture.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CSceneNodeAnimatorTextureHPP
#define CSceneNodeAnimatorTextureHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/ISceneNode.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class CSceneNodeAnimatorTexture : public ISceneNodeAnimator
{
public:

	CSceneNodeAnimatorTexture(u32 start_time, 
		const core::array<SAnimationFrame>& textures, 
		const SAnimationFrameParameters &params);

    CSceneNodeAnimatorTexture(u32 start_time, 		
		const SAnimationFrameParameters &params);

    virtual ~CSceneNodeAnimatorTexture();

    virtual void animateNode(ISceneNode* node, u32 timeMs);

	virtual E_SCENE_NODE_ANIMATOR_TYPE getAnimatorType()
	{ return ESNAT_TEXTURE_ANIMATOR; }

	void setFramesDirectionSet(u32 direction_idx,
		const core::array <SAnimationFrame> &frames);

	const core::array <SAnimationFrame>& getFramesDirectionSet(u32 direction_idx);

	u32 getDirectionSetsCount()
	{ return m_DirectionSets.size(); }

	u32 getDirectionSetFramesCount(u32 direction_idx)
	{ return m_DirectionSets[direction_idx].size(); }

	bool activateFramesDirectionSet(u32 direction_idx);

	u32 getActiveFramesDirectionSetIndex()
	{ return m_ActiveDirectionSetIndex; }

	u32 getCurrentFrameIndex()
	{ return m_CurrentFrameIndex; }

	u32 getCurrentFramesCount()
	{ return m_DirectionSets[m_ActiveDirectionSetIndex].size(); }

	const SAnimationFrameParameters& getTextureAnimatorParameters()
	{ return m_TextureAnimatorParameters; }

	const SAnimationFrame& getCurrentFrame()
	{ return m_DirectionSets[m_ActiveDirectionSetIndex][m_CurrentFrameIndex]; }

private:

	SAnimationFrameParameters m_TextureAnimatorParameters; 

    core::array < core::array <SAnimationFrame> > m_DirectionSets;
    u32 m_EndTime;

	u32 m_ActiveDirectionSetIndex, m_CurrentFrameIndex;

	bool m_AnimationEnded;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

