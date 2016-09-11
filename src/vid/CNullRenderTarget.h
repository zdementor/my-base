//|-------------------------------------------------------------------------
//| File:        CNullRenderTarget.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2013 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CNullRenderTargetHPP
#define CNullRenderTargetHPP
//---------------------------------------------------------------------------

#include <vid/IVideoDriver.h>
#include <vid/IRenderTarget.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

class __MY_VID_LIB_API__ CNullRenderTarget : public IRenderTarget
{
	friend class CNullDriver;
public:

	CNullRenderTarget(const core::dimension2di &size,
		img::E_COLOR_FORMAT colorFormat, E_RENDER_TARGET_DEPTH_FORMAT depthFormat);
	virtual ~CNullRenderTarget();

	virtual ITexture* getColorTexture()
	{ return m_ColorTexture; }

	virtual bool bind();
	virtual bool unbind();

protected:

	bool m_OK;

	vid::IVideoDriver &m_Driver;

	core::list<CNullRenderTarget *>::iterator m_RTEntry;

	ITexture *m_ColorTexture;

	core::dimension2di m_Size;
	img::E_COLOR_FORMAT m_ColorFormat;
	E_RENDER_TARGET_DEPTH_FORMAT m_DepthFormat;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif
