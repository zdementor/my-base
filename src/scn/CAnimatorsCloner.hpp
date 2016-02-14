//|-------------------------------------------------------------------------
//| File:        CAnimatorsCloner.hpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class CAnimatorsCloner
{
private:	

public:

	static scn::ISceneNodeAnimator* clone(scn::ISceneNodeAnimator* ani_to_clone)
	{
		if (!ani_to_clone)
			return NULL;

		scn::ISceneNodeAnimator *cloned_ani = NULL;
		scn::ISceneManager &smgr = SCENE_MANAGER;

		if (ani_to_clone->getAnimatorType() == ESNAT_FLY_CIRCLE_ANIMATOR)
		{
			SFlyCircleParameters fly_params = 
				((CSceneNodeAnimatorFlyCircle*)ani_to_clone)->getFlyCircleParameters();

			cloned_ani = smgr.createFlyCircleAnimator(
				fly_params.Center, fly_params.Radius, fly_params.Speed);
		}
		else if (ani_to_clone->getAnimatorType() == ESNAT_FLY_STRAIGHT_ANIMATOR)
		{
			SFlyStraightParameters fly_params = 
				((CSceneNodeAnimatorFlyStraight*)ani_to_clone)->getFlyStraightParameters();

			cloned_ani = smgr.createFlyStraightAnimator(
				fly_params.Start, fly_params.End, fly_params.TimeForWay, fly_params.Loop);
		}
		else if (ani_to_clone->getAnimatorType() == ESNAT_FOLLOW_SPLINE_ANIMATOR)
		{
			SFollowSplineParameters follow_params = 
				((CSceneNodeAnimatorFollowSpline*)ani_to_clone)->getFollowSplineParameters();

			cloned_ani = smgr.createFollowSplineAnimator(
				0, follow_params.Points, follow_params.Speed, follow_params.Tightness);
		}
		else if (ani_to_clone->getAnimatorType() == ESNAT_ROTATION_ANIMATOR)
		{
			cloned_ani = smgr.createRotationAnimator(
				((CSceneNodeAnimatorRotation*)ani_to_clone)->getRotationPerSec());
		}
		else if (ani_to_clone->getAnimatorType() == ESNAT_TEXTURE_ANIMATOR)
		{
			const SAnimationFrameParameters &tex_ani_params =
				((CSceneNodeAnimatorTexture*)ani_to_clone)->getTextureAnimatorParameters();

			const core::array<SAnimationFrame> &frames =
				((CSceneNodeAnimatorTexture*)ani_to_clone)->getFramesDirectionSet(0);	

			cloned_ani = smgr.createTextureAnimator(
				frames, tex_ani_params.TimePerFrame,tex_ani_params.Loop);

			s32 directions_count = 
				((CSceneNodeAnimatorTexture*)ani_to_clone)->getDirectionSetsCount();

			for (s32 d=1; d<directions_count; d++)
			{
				const core::array<SAnimationFrame> &frames =
					((CSceneNodeAnimatorTexture*)ani_to_clone)->getFramesDirectionSet(d);	

				((CSceneNodeAnimatorTexture*)cloned_ani)->setFramesDirectionSet(d, frames);
			}			
		}
		if (cloned_ani)
			cloned_ani->setAnimationStartTime(ani_to_clone->getAnimationStartTime());

		return cloned_ani;
	}		
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------