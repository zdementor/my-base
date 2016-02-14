//|-------------------------------------------------------------------------
//| File:        SParticle.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SParticleHPP
#define SParticleHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <img/SColor.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! Struct for holding particle data
struct __MY_SCN_LIB_API__ SParticle
{
    //! Position of the particle
    core::vector3df pos;

    //! Direction and speed of the particle
    core::vector3df vector;

    //! Start life time of the particle
    u32 startTime;

    //! End life time of the particle
    u32 endTime;

    //! Current color of the particle
    img::SColor color;

    //! Original color of the particle. That's the color
    //! of the particle it had when it was emitted.
    img::SColor startColor;

    //! Original direction and speed of the particle, 
    //! the direction and speed the particle had when
    //! it was emitted.
    core::vector3df startVector;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif

