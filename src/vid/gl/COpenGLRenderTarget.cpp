//|-------------------------------------------------------------------------
//| File:        COpenGLRenderTarget.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2013 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "COpenGLRenderTarget.h"
#include "COpenGLDriver.h"

//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

COpenGLRenderTarget::COpenGLRenderTarget(const core::dimension2di &size)
	: CNullRenderTarget()
{
}

//----------------------------------------------------------------------------

COpenGLRenderTarget::~COpenGLRenderTarget()
{
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
 