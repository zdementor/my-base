//|-------------------------------------------------------------------------
//| File:        face3d.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef face3dHPP
#define face3dHPP

//----------------------------------------------------------------------------

#include "triangle3d.h"

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------   

//! 3d triangle template class for doing collision detection and other things.
template <class T>
class face3d
{
public:

	//! constructor
	face3d() {}

	//! constructor
	face3d(const T& a, const T& b, const T& c)
	{
		set(a, b, c);
	}

	//------------------------------------------------------------------------

    inline void set(const T& a, const T& b, const T& c)
    {
        A = a;
        B = b;
        C = c;
    }

	//------------------------------------------------------------------------

	inline bool isValide()
	{	
		return triangle3df(A.Pos, B.Pos, C.Pos).isValide();
	}

	//------------------------------------------------------------------------

	inline triangle3df getTriangle()
	{	
		return triangle3df(A.Pos, B.Pos, C.Pos);
	}

	//------------------------------------------------------------------------

    //! the three vertices of the triangle
    T A; 
    T B; 
    T C; 
};

//----------------------------------------------------------------------------
} // end namespace core
} // end namespac my
//----------------------------------------------------------------------------

#endif

