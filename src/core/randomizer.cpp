//|-------------------------------------------------------------------------
//| File:        randomizer.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include <my_inc.h>

//-----------------------------------------------------------------------------
namespace my {
namespace core {
//-----------------------------------------------------------------------------

s32 randomizer::seed = 0x0f0f0f0f;

//-----------------------------------------------------------------------------

//! generates a pseudo random integer number
MY_FORCEINLINE s32 randomizer::random()
{   
	const s32 m = 2147483399;   // a non-Mersenne prime
    const s32 a = 40692;        // another spectral success story
    const s32 q = 52774;        // m/a
    const s32 r = 3791;         // m%a; again less than q

    seed = a * (seed%q) - r* (seed/q);
    if (seed<0) seed += m;

    return seed;
}

//-----------------------------------------------------------------------------

//! resets the randomizer
MY_FORCEINLINE void randomizer::reset()
{       
	seed = 0x0f0f0f0f;
}

//-----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//-----------------------------------------------------------------------------
