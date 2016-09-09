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

	CNullRenderTarget(const core::dimension2di &size, img::E_COLOR_FORMAT colorFormat,
		E_RENDER_TARGET_CREATION_FLAG flags);
	virtual ~CNullRenderTarget();

	virtual ITexture* getColorAttachment()
	{ return m_ColorAttachment; }

	virtual ITexture* getDepthAttachment()
	{ return m_DepthAttachment; }

	virtual bool bind();
	virtual bool unbind();

protected:

	ITexture *m_ColorAttachment;
	ITexture *m_DepthAttachment;

	core::dimension2di m_Size;
	img::E_COLOR_FORMAT m_ColorFormat, m_DepthFormat, m_StencilFormat;
	E_RENDER_TARGET_CREATION_FLAG m_Flags;

	core::list<CNullRenderTarget *>::iterator m_RTEntry;

	vid::IVideoDriver &m_Driver;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif
