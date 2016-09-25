//|-------------------------------------------------------------------------
//| File:        ODEColliders.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2016 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ODECollidersHPP
#define ODECollidersHPP
//---------------------------------------------------------------------------

#include <my_inc.h>

#include <ode/ode.h>

//---------------------------------------------------------------------------
namespace my {
namespace dyn {
namespace ode {
//---------------------------------------------------------------------------

int CollideHeightfieldRay(
	dxGeom *o1, dxGeom *rayGeom, int flags, dContactGeom *contact, int skip);

//---------------------------------------------------------------------------
} // end namespace ode
} // end namespace dyn
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef ODEHelperHPP
