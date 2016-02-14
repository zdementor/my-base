//|-------------------------------------------------------------------------
//| File:        CSceneNodeAnimatorFollowSpline.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CSceneNodeAnimatorFollowSpline.h"
#include <io/ILogger.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! constructor
CSceneNodeAnimatorFollowSpline::CSceneNodeAnimatorFollowSpline(
    u32 time, const SFollowSplineParameters& params
    ) 
: ISceneNodeAnimator(time), FollowSplineParameters(params)
{
#if MY_DEBUG_MODE 
	setClassName("CSceneNodeAnimatorFollowSpline");
#endif

    NumPoints = FollowSplineParameters.Points.size();
	NumPoints_1 = NumPoints - 1;    
}

//---------------------------------------------------------------------------

//! destructor
CSceneNodeAnimatorFollowSpline::~CSceneNodeAnimatorFollowSpline()
{
}

//---------------------------------------------------------------------------

inline s32 CSceneNodeAnimatorFollowSpline::clamp(s32 idx, s32 size)
{
	CHECK_RANGE(idx, 0, (s32)NumPoints_1);

    return idx;
}

//---------------------------------------------------------------------------

//! animates a scene node
void CSceneNodeAnimatorFollowSpline::animateNode(ISceneNode* node, u32 timeMs)
{
	if (!IsEnabled)
		return;

    core::vector3df p, p0, p1, p2, p3;
    core::vector3df t1, t2;

    f32 dt = ( (timeMs-StartTime) * FollowSplineParameters.Speed );

    f32 dT = 0.001f * dt;

    s32 idx = static_cast< s32 >( dT ) % NumPoints;    
        
    if ( (dT>=(f32)(NumPoints_1) || idx >= (s32)NumPoints_1) && EndAnimationCallback)
    {
		// setting new node position
		node->setPosition( FollowSplineParameters.Points[NumPoints_1] );

        EndAnimationCallback(this, node);
    }
	else
	{
		f32 u = 0.001f * fmodf( dt, 1000.0f );
    
		p0 = FollowSplineParameters.Points[ clamp( idx - 1, NumPoints ) ];
		p1 = FollowSplineParameters.Points[ clamp( idx,     NumPoints ) ];
		p2 = FollowSplineParameters.Points[ clamp( idx + 1, NumPoints ) ];
		p3 = FollowSplineParameters.Points[ clamp( idx + 2, NumPoints ) ];

		// hermite polynomials
		f32 h1 = 2.0f * u * u * u - 3.0f * u * u + 1.0f;
		f32 h2 = -2.0f * u * u * u + 3.0f * u * u;
		f32 h3 = u * u * u - 2.0f * u * u + u;
		f32 h4 = u * u * u - u * u;

		// tangents
		t1 = ( p2 - p0 ) * FollowSplineParameters.Tightness;
		t2 = ( p3 - p1 ) * FollowSplineParameters.Tightness;

		// interpolated point
		p = p1 * h1 + p2 * h2 + t1 * h3 + t2 * h4;

		// setting new node position
		node->setPosition(p);
	}
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

