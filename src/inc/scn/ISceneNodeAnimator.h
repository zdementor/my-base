//|-------------------------------------------------------------------------
//| File:        ISceneNodeAnimator.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ISceneNodeAnimatorHPP
#define ISceneNodeAnimatorHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct SAnimationFrameParameters
{	 
	s32 TimePerFrame; 
	bool Loop;
};

//----------------------------------------------------------------------------

struct SAnimationFrame
{	 
	SAnimationFrame()
		: Texture(0), TCoords(0.0f, 0.0f, 1.0f, 1.0f),
		TOffset(0.0f, 0.0f) {}
	SAnimationFrame(vid::ITexture *t)
		: Texture(t), TCoords(0.0f, 0.0f, 1.0f, 1.0f),
		TOffset(0.0f, 0.0f) {}
	SAnimationFrame(vid::ITexture *t,
		const core::rectf &tc, const core::vector2df & toffs)
		: Texture(t), TCoords(tc), TOffset(toffs) {}

	void set(const SAnimationFrame &other)
	{ *this = other; }

	vid::ITexture   *Texture; 
	core::rectf     TCoords;
	core::vector2df TOffset;
};

//----------------------------------------------------------------------------

//! Animates a scene node. Can animate position, rotation, material, and so on.
//! A scene node animator is able to animate a scene node in a very simple way. It may
//! change its position, rotation, scale and/or material. There are lots of animators
//! to choose from. You can create scene node animators with the ISceneManager interface.
class __MY_SCN_LIB_API__ ISceneNodeAnimator : public IUnknown
{
public:

	//! destructor
    ISceneNodeAnimator(u32 start_time) 
		: EndAnimationCallback(NULL), AnimatedNode(NULL), IsEnabled(true), StartTime(start_time) {}

	//! destructor
    virtual ~ISceneNodeAnimator() {}

    //! Animates a scene node.
    //! \param node: Node to animate.
    //! \param timeMs: Current time in milli seconds.
    virtual void animateNode(ISceneNode* node, u32 timeMs) = 0;

	//! setting end animation callback
	virtual void setEndAnimationCallback(
		void (*callback)(ISceneNodeAnimator *animator, ISceneNode* anode)
		)
	{
		EndAnimationCallback = callback;
	}

	//! return scene node animator type
	virtual E_SCENE_NODE_ANIMATOR_TYPE getAnimatorType()
	{
		return ESNAT_UNKNOWN_ANIMATOR;
	}

	virtual bool isEnabled()
	{
		return IsEnabled;
	}

	virtual void setEnabled(bool enabled)
	{
		IsEnabled = enabled;
	}

	virtual void setAnimationStartTime(u32 start)
	{
		StartTime = start;
	}

	virtual u32 getAnimationStartTime()
	{
		return StartTime;
	}

protected:

	ISceneNode* AnimatedNode;

	void (*EndAnimationCallback)(ISceneNodeAnimator *animator, ISceneNode* anode);

	bool IsEnabled;

	u32 StartTime;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif

