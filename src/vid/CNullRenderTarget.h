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

#include <vid/IRenderTarget.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

class __MY_VID_LIB_API__ CNullRenderTarget : public IRenderTarget
{
public:

	CNullRenderTarget();
	virtual ~CNullRenderTarget();

	virtual bool attachColorTexture(
		u32 attachIndex, ITexture *colorTex)
	{
		return false;
	}

	virtual bool attachColorTexture(
		u32 attachIndex, core::dimension2di &size, img::E_COLOR_FORMAT format)
	{
		return false;
	}

	virtual ITexture* getColorAttachement(u32 attachIndex)
	{
		if (attachIndex >= m_ColorAttachementsCount)
			return NULL;
		return m_ColorAttachements[attachIndex];
	}

	virtual u32 getColorAttachementsCount()
	{ return m_ColorAttachementsCount; }

	virtual bool attachDepthTexture(
		ITexture *dephTex)
	{
		return false;
	}

	virtual bool attachDepthTexture(
		core::dimension2di &size, img::E_COLOR_FORMAT format)
	{
		return false;
	}

	virtual ITexture* getDepthAttachement()
	{ return m_DepthAttachement; }

private:

	ITexture *m_ColorAttachements[MY_MAX_COLOR_ATTACHEMENTS];
	ITexture *m_DepthAttachement;

	u32 m_ColorAttachementsCount;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif
