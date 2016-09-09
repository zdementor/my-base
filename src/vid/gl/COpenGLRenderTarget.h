 //|-------------------------------------------------------------------------
//| File:        COpenGLRenderTarget.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2013 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef COpenGLRenderTargetHPP
#define COpenGLRenderTargetHPP
//---------------------------------------------------------------------------

#include "../CNullRenderTarget.h"
#include "COpenGLDriver.h"

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

class __MY_VID_LIB_API__ COpenGLRenderTarget : public CNullRenderTarget
{
public:
	COpenGLRenderTarget(const core::dimension2di &size, img::E_COLOR_FORMAT colorFormat,
		E_RENDER_TARGET_CREATION_FLAG flags);
	COpenGLRenderTarget(ITexture *colorRenderTarget, E_RENDER_TARGET_CREATION_FLAG flags);
	virtual ~COpenGLRenderTarget();

	virtual bool bind();
	virtual bool unbind();
private:
	GLuint m_FBO;
	bool _rebuild();

	bool m_OK;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif
 