//|-------------------------------------------------------------------------
//| File:        coretypes.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef coretypesHPP
#define coretypesHPP

//----------------------------------------------------------------------------
namespace my {
//----------------------------------------------------------------------------

//! 8 bit unsigned variable.
//! This is a typedef for unsigned char, it ensures portability of the engine.
typedef unsigned char       u8;

//----------------------------------------------------------------------------

//! 8 bit signed variable.
//! This is a typedef for signed char, it ensures portability of the engine.
typedef signed char         s8;

//----------------------------------------------------------------------------

//! 8 bit character variable.
//! This is a typedef for char, it ensures portability of the engine.
typedef char                c8;

//----------------------------------------------------------------------------

//! 16 bit unsigned variable.
//! This is a typedef for unsigned short, it ensures portability of the engine. 
typedef unsigned short      u16;

//----------------------------------------------------------------------------

//! 16 bit signed variable.
//! This is a typedef for signed short, it ensures portability of the engine. 
typedef signed short        s16;

//----------------------------------------------------------------------------

//! 32 bit unsigned variable.
//! This is a typedef for unsigned int, it ensures portability of the engine.
typedef unsigned int        u32;

//----------------------------------------------------------------------------

//! 32 bit signed variable.
//! This is a typedef for signed int, it ensures portability of the engine. 
typedef signed int          s32;

//----------------------------------------------------------------------------

//! 64 bit signed variable.
//! This is a typedef for __int64, it ensures portability of the engine.
typedef __int64             s64;

//----------------------------------------------------------------------------

//! 64 bit unsigned variable.
//! This is a typedef for __int64, it ensures portability of the engine.
typedef __int64             u64;

//----------------------------------------------------------------------------

//! 32 bit floating point variable.
//! This is a typedef for float, it ensures portability of the engine.
typedef float               f32;

//----------------------------------------------------------------------------

//! 64 bit floating point variable.
//! This is a typedef for double, it ensures portability of the engine.

typedef double              f64;

//----------------------------------------------------------------------------
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef coretypesHPP