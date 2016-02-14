//|-------------------------------------------------------------------------
//| File:        randomizer.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef __randomizerHPP__
#define __randomizerHPP__

//-----------------------------------------------------------------------------
namespace my {
namespace core {
//-----------------------------------------------------------------------------

class __MY_CORE_LIB_API__ randomizer
{
public:

    //! resets the randomizer
    static MY_FORCEINLINE void reset();

    //! generates a pseudo random integer number
    static MY_FORCEINLINE s32 random();

private:

    static s32 seed;
};

//-----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//-----------------------------------------------------------------------------

#endif // #ifndef __randomizerHPP__
