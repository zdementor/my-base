//|-------------------------------------------------------------------------
//| File:        core.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "coretypes.h"

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

//! some constants

//! Rounding error constant often used when comparing f32 values.

const f32 ROUNDING_ERROR_32	= 0.000001f;
const f64 ROUNDING_ERROR_64	= 0.00000001f; 

//! Constant for PI.
const f32 PI                = 3.14159f;
const f32 HALF_PI           = PI / 2.0f; 
const f32 TWO_PI            = PI * 2.0f; 

//! Constant for converting bettween degrees and radiants.
const f64 RAD2DEG       = 180.0 / 3.1415926535897932384626433832795;

//! Constant for converting bettween degrees and radiants.
const f64 DEG2RAD       = 3.1415926535897932384626433832795 / 180.0; 

//----------------------------------------------------------------------------

enum E_FUNC_TYPE
{
	EFT_SIN = 0,
	EFT_SQUARE,
	EFT_TRIANGLE,
	EFT_SAWTOOTH, 
	EFT_INVERSE_SAWTOOTH,
	EFT_NOISE,

	E_FUNC_TYPE_COUNT,

	E_FUNC_TYPE_FORCE_32BIT = 0xffffffff
};

static const c8* FuncTypeName[E_FUNC_TYPE_COUNT] = 
{
	"EFT_SIN",
	"EFT_SQUARE",
	"EFT_TRIANGLE",
	"EFT_SAWTOOTH", 
	"EFT_INVERSE_SAWTOOTH",
	"EFT_NOISE"
};

static const c8* FuncTypeReadableName[E_FUNC_TYPE_COUNT] = 
{
	"Sin",
	"Square",
	"Triangle",
	"Sawtooth", 
	"Inv.Sawtooth",
	"Noise"
};

MY_INLINE const c8* getFuncTypeName(E_FUNC_TYPE type)
{ return FuncTypeName[type]; }

MY_INLINE const c8* getFuncTypeReadableName(E_FUNC_TYPE type)
{ return FuncTypeReadableName[type]; }

__MY_CORE_LIB_API__ void dealloc(void* ptr);
__MY_CORE_LIB_API__ void* alloc(size_t size) throw (...);

//----------------------------------------------------------------------------
} // end namespace core    
} // end namespace my
//----------------------------------------------------------------------------

#include "mathfunc.h"
#include "aabbox3d.h"
#include "array.h"
#include "dimension2d.h"
#include "fast_atof.h"
#include "sort.h"
#include "line2d.h"
#include "line3d.h"
#include "list.h"
#include "matrix4.h"
#include "plane3d.h"
#include "position2d.h"
#include "rect.h"
#include "triangle3d.h"
#include "vector2d.h"
#include "vector3d.h"
#include "string.h"
#include "quaternion.h"
#include "rlepack.h"
#include "view_frustrum.h"
#include "randomizer.h"
#include "face3d.h"
#include "wave.h"
#include "queue.h"
#include "hash.h"
#include "filter.h"
#include "pair.h"
