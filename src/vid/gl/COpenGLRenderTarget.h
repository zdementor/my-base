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
	COpenGLRenderTarget(const core::dimension2di &size,
		img::E_COLOR_FORMAT colorFormat, E_RENDER_TARGET_DEPTH_FORMAT depthFormat);
	virtual ~COpenGLRenderTarget();

	virtual bool bind();
	virtual bool unbind();
private:
	bool _rebuild();

	GLuint m_FBO, m_DepthStencilBuffer;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif
 