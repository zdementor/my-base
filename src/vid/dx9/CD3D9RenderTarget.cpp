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

CD3D9RenderTarget::CD3D9RenderTarget()
	: CNullRenderTarget(),
m_D3DDriver((CD3D9Driver*)VIDEO_DRIVER_PTR),
m_D3DDepthStencilSurface(0)
{
	memset(m_D3DRenderTargetSurface, 0, sizeof(m_D3DRenderTargetSurface));

	m_OK = true;
}

//----------------------------------------------------------------------------

CD3D9RenderTarget::CD3D9RenderTarget(const core::dimension2di &size,
	img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat)
	: CNullRenderTarget(size, colorFormat, depthFormat),
m_D3DDriver((CD3D9Driver*)VIDEO_DRIVER_PTR),
m_D3DDepthStencilSurface(0)
{
	memset(m_D3DRenderTargetSurface, 0, sizeof(m_D3DRenderTargetSurface));

	_rebuild();
}

//----------------------------------------------------------------------------

CD3D9RenderTarget::CD3D9RenderTarget(
	ITexture *colorTexture, ITexture *depthTexture)
	: CNullRenderTarget(colorTexture, depthTexture),
m_D3DDriver((CD3D9Driver*)VIDEO_DRIVER_PTR),
m_D3DDepthStencilSurface(0)
{
	memset(m_D3DRenderTargetSurface, 0, sizeof(m_D3DRenderTargetSurface));

	if (colorTexture)
		_rebuild();
	else
		m_OK = true;
}

//----------------------------------------------------------------------------

CD3D9RenderTarget::~CD3D9RenderTarget()
{
	for (u32 no = 0; no < MY_MAX_COLOR_ATTACHMENTS; no++)
		SAFE_RELEASE(m_D3DRenderTargetSurface[no]);
	SAFE_RELEASE(m_D3DDepthStencilSurface);
}

//----------------------------------------------------------------------------

bool CD3D9RenderTarget::_rebuild()
{
	m_OK = true;

	IDirect3DDevice9 *pd3dDevice = m_D3DDriver->_getDirect3DDevice();

	for (u32 no = 0; no < MY_MAX_COLOR_ATTACHMENTS; no++)
		SAFE_RELEASE(m_D3DRenderTargetSurface[no]);
	SAFE_RELEASE(m_D3DDepthStencilSurface);

	for (u32 no = 0; no < MY_MAX_COLOR_ATTACHMENTS; no++)
	{
		ITexture *colTex = m_ColorTexture[no];
		if (!colTex)
			break;
		if (colTex->isRenderTarget())
		{
			m_D3DRenderTargetSurface[no] =
				((CD3D9RenderTargetTexture *)colTex)->getRenderTargetSurface();
			if (m_D3DRenderTargetSurface[no])
				m_D3DRenderTargetSurface[no]->AddRef();
			else
				m_OK = false;
		}
		else
			m_OK = false;
	}

	if (m_OK)
	{
		if (m_DepthTexture && m_DepthTexture->isRenderTarget())
		{
			m_D3DDepthStencilSurface =
				((CD3D9RenderTargetTexture *)m_DepthTexture)->getRenderTargetSurface();
			if (m_D3DDepthStencilSurface)
				m_D3DDepthStencilSurface->AddRef();
		}
		else
		{
			D3DFORMAT format = D3DFMT_UNKNOWN;
			switch (m_DepthFormat)
			{
			case img::ECF_DEPTH24_STENCIL8:
				format = D3DFMT_D24S8;
				break;
			default:
				break;
			}
			if (format != D3DFMT_UNKNOWN)
			{
				HRESULT hr = pd3dDevice->CreateDepthStencilSurface(
					m_Size[0].Width, m_Size[0].Height,
					format,
					D3DMULTISAMPLE_NONE,
					0,
					TRUE,
					&m_D3DDepthStencilSurface,
					NULL);
				m_OK = SUCCEEDED(hr);
			}
		}
	}

	if (!m_OK)
		LOGGER.logErr("Can not create D3D9 render target");

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

	for (u32 no = 0; m_OK && no < MY_MAX_COLOR_ATTACHMENTS; no++)
	{
		if (! m_D3DRenderTargetSurface[no])
			break;
		hr = pd3dDevice->SetRenderTarget(no, m_D3DRenderTargetSurface[no]);
		if (FAILED(hr))
			LOGGER.logWarn("Can not bind Render Target surface %d (%dx%d, %s)!",
				no, m_Size[no].Width, m_Size[no].Height,
				img::getColorFormatName(m_ColorFormat[no]));
		m_OK = SUCCEEDED(hr);
	}
	if (m_OK)
	{
		hr = pd3dDevice->SetDepthStencilSurface(m_D3DDepthStencilSurface);
		if (FAILED(hr))
			LOGGER.logWarn("Can not bind Depth Stencil surface!");
		m_OK = SUCCEEDED(hr);
	}

	if (!m_OK)
	{
		LOGGER.logErr("Failed to bind Render Target (%dx%d, %s/%s)!",
			m_Size[0].Width, m_Size[0].Height, img::getColorFormatName(m_ColorFormat[0]),
			img::getColorFormatName(m_DepthFormat));

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

	for (u32 no = 0; no < MY_MAX_COLOR_ATTACHMENTS; no++)
		pd3dDevice->SetRenderTarget(no, no == 0 ? mainRTSurf : NULL);
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
  