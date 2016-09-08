//|-------------------------------------------------------------------------
//| File:        COpenGLRenderTargetTexture.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef COpenGLRenderTargetTextureHPP
#define COpenGLRenderTargetTextureHPP
//---------------------------------------------------------------------------

#include "COpenGLTexture.h"

#include "COpenGLDriver.h"

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

class COpenGLRenderTargetTexture : public COpenGLTexture
{
public:

	COpenGLRenderTargetTexture(
		const core::dimension2di &size, img::E_COLOR_FORMAT colorFormat);
    virtual ~COpenGLRenderTargetTexture();

	virtual bool isRenderTarget() { return true; }

private:
};

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif 
