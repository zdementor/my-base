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
	CNullRenderTarget();
	CNullRenderTarget(const core::dimension2di &size,
		img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat);
	CNullRenderTarget(ITexture *colorTexture, ITexture *depthTexture);
	virtual ~CNullRenderTarget();

	virtual bool bindColorTexture(ITexture *colorTexture, bool doRebuild)
	{ return bindColorTexture(0, colorTexture, doRebuild); }

	virtual bool bindColorTexture(u32 no, ITexture *colorTexture, bool doRebuild);

	virtual bool bindDepthTexture(ITexture *depthTexture, bool doRebuild);

	virtual bool rebuild() { return _rebuild(); }

	virtual ITexture* getColorTexture(u32 no)
	{
		if (no >= MY_MAX_COLOR_ATTACHMENTS)
			return NULL;
		return m_ColorTexture[no];
	}

	virtual ITexture* getDepthTexture()
	{ return m_DepthTexture; }

	virtual bool bind();
	virtual bool unbind();

	bool isOK() { return m_OK; }

protected:

	virtual bool _rebuild() { return false; }

	bool m_OK;

	vid::IVideoDriver &m_Driver;

	core::list<CNullRenderTarget *>::iterator m_RTEntry;

	ITexture *m_ColorTexture[MY_MAX_COLOR_ATTACHMENTS];
	ITexture *m_DepthTexture;

	core::dimension2di m_Size[MY_MAX_COLOR_ATTACHMENTS];

	img::E_COLOR_FORMAT m_ColorFormat[MY_MAX_COLOR_ATTACHMENTS];
	img::E_COLOR_FORMAT m_DepthFormat;

private:

	void _defaultInit();
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif
