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

#include "CD3D9RenderTarget.h"
#include "CD3D9Driver.h"
#include "CD3D9RenderTargetTexture.h"

//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

CD3D9RenderTarget::CD3D9RenderTarget(const core::dimension2di &size,
	img::E_COLOR_FORMAT colorFormat, E_RENDER_TARGET_DEPTH_FORMAT depthFormat)
	: CNullRenderTarget(size, colorFormat, depthFormat),
m_D3DDriver((CD3D9Driver*)VIDEO_DRIVER_PTR),
m_D3DRenderTargetSurface(0), m_D3DDepthStencilSurface(0)
{
	_rebuild();
}

//----------------------------------------------------------------------------

CD3D9RenderTarget::~CD3D9RenderTarget()
{
	SAFE_RELEASE(m_D3DRenderTargetSurface);
	SAFE_RELEASE(m_D3DDepthStencilSurface);
}

//----------------------------------------------------------------------------

bool CD3D9RenderTarget::_rebuild()
{
	m_OK = true;

	IDirect3DDevice9 *pd3dDevice = m_D3DDriver->_getDirect3DDevice();

	SAFE_RELEASE(m_D3DRenderTargetSurface);
	SAFE_RELEASE(m_D3DDepthStencilSurface);

	if (m_ColorTexture->isRenderTarget())
	{
		m_D3DRenderTargetSurface =
			((CD3D9RenderTargetTexture *)m_ColorTexture)->getRenderTargetSurface();
		if (m_D3DRenderTargetSurface)
			m_D3DRenderTargetSurface->AddRef();
		else
			m_OK = false;
	}

	D3DFORMAT format = D3DFMT_UNKNOWN;
	switch (m_DepthFormat)
	{
	case ERTDF_DEPTH16:
		format = D3DFMT_D16;
		break;
	case ERTDF_DEPTH24:
		format = D3DFMT_D24X8;
		break;
	case ERTDF_DEPTH32:
		format = D3DFMT_D32;
		break;
	case ERTDF_DEPTH24_STENCIL8:
		format = D3DFMT_D24S8;
		break;
	default:
		break;
	}
	if (m_OK && format != D3DFMT_UNKNOWN)
	{
		HRESULT hr = pd3dDevice->CreateDepthStencilSurface(
			m_Size.Width, m_Size.Height,
			format,
			D3DMULTISAMPLE_NONE,
			0,
			TRUE,
			&m_D3DDepthStencilSurface,
			NULL);
		m_OK = SUCCEEDED(hr);
	}

	return m_OK;
}

//----------------------------------------------------------------------------

bool CD3D9RenderTarget::bind()
{
	if (!m_OK)
		return false;

	IDirect3DDevice9 *pd3dDevice = m_D3DDriver->_getDirect3DDevice();
	bool ret = true;
	HRESULT hr;

	hr = pd3dDevice->SetRenderTarget(0, m_D3DRenderTargetSurface);
	if (FAILED(hr))
		LOGGER.logWarn("Can not bind Render Target surface!");
	else
	{
		hr = pd3dDevice->SetDepthStencilSurface(m_D3DDepthStencilSurface);
		if (FAILED(hr))
			LOGGER.logWarn("Can not bind Depth Stencil surface!");
	}
	m_OK = SUCCEEDED(hr);

	if (!m_OK)
	{
		LOGGER.logErr("Failed to bind Render Target (%dx%d, %s/%s)!",
			m_Size.Width, m_Size.Height, img::getColorFormatName(m_ColorFormat),
			getRenderTargetDepthFormatName(m_DepthFormat));

		_bindMainRT();
		return false;
	}
	return CNullRenderTarget::bind();
}

//----------------------------------------------------------------------------

void CD3D9RenderTarget::_bindMainRT()
{
	IDirect3DDevice9 *pd3dDevice = m_D3DDriver->_getDirect3DDevice();
	IDirect3DSurface9 *mainRTSurf = m_D3DDriver->_getMainRenderTargetSurface();
	IDirect3DSurface9 *mainDSSurf = m_D3DDriver->_getMainDepthStencilSurface();

	if (mainRTSurf)
		pd3dDevice->SetRenderTarget( 0, mainRTSurf);
	if (mainDSSurf)
		pd3dDevice->SetDepthStencilSurface(mainDSSurf);
}

//----------------------------------------------------------------------------

bool CD3D9RenderTarget::unbind()
{
	if (!m_OK)
		return false;

	_bindMainRT();

	return CNullRenderTarget::unbind();
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
  