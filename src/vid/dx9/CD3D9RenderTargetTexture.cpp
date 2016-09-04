//|-------------------------------------------------------------------------
//| File:        CD3D9Texture.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CD3D9RenderTargetTexture.h"
#include "CD3D9Driver.h"

#include <io/ILogger.h>
#include <vid/IVideoDriver.h>
#include <img/IImageLibrary.h>
#include <io/IFileSystem.h>

#include <d3d/d3dx9tex.h>

//----------------------------------------------------------------------------
namespace my { 
namespace vid {
//----------------------------------------------------------------------------

CD3D9RenderTargetTexture::CD3D9RenderTargetTexture(const core::dimension2di &size) 
	: CD3D9Texture()
{
#if MY_DEBUG_MODE 
    IUnknown::setClassName("CD3D9RenderTargetTexture");
#endif

	m_ImageSize = size;
	m_TextureSize.Width  = core::math::GetNearestPowerOfTwo(size.Width);
	m_TextureSize.Height = core::math::GetNearestPowerOfTwo(size.Height);

    // get backbuffer format to create the render target in the 
    // same format

    IDirect3DSurface9* bb;
    D3DFORMAT d3DFormat;

    if (!FAILED(Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb)))
    {
        D3DSURFACE_DESC desc;
        bb->GetDesc(&desc);
        d3DFormat = desc.Format;
        
        if (d3DFormat == D3DFMT_X8R8G8B8)
            d3DFormat = D3DFMT_A8R8G8B8;

        bb->Release();
    }
    else
    {
        LOGGER.logWarn("Could not create RenderTarget texture: could not get BackBuffer.");
        return;
    }

    // create texture
    HRESULT hr;

    hr = Device->CreateTexture(
        m_TextureSize.Width,
        m_TextureSize.Height,
        1, // mip map level count, we don't want mipmaps here
        D3DUSAGE_RENDERTARGET,
        d3DFormat,
        D3DPOOL_DEFAULT,
        &Texture,
        NULL);

    switch(d3DFormat)
    {
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
        m_ColorFormat = img::ECF_A1R5G5B5;
		m_Pitch = m_TextureSize.Width*2;
        break;
    case D3DFMT_A8B8G8R8:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
        m_ColorFormat = img::ECF_A8R8G8B8;
		m_Pitch = m_TextureSize.Width*4;
        break;
    case D3DFMT_R5G6B5:
        m_ColorFormat = img::ECF_R5G6B5;
		m_Pitch = m_TextureSize.Width*2;
        break;
    default:
        m_ColorFormat = (img::E_COLOR_FORMAT)-1;
		m_Pitch = 0;
    };

    if (FAILED(hr))
	{
        LOGGER.log("Could not create render target texture");
		return;
	}

	LOGGER.logInfo("Created render target texture ( %s, mips %s, %dx%d )", 
		img::ColorFormatStr[m_ColorFormat],
		hasMipMaps() ? "on" : "off",
		getSize().Width, getSize().Height);
}

//----------------------------------------------------------------------------

CD3D9RenderTargetTexture::~CD3D9RenderTargetTexture()
{
}

//----------------------------------------------------------------------------

IDirect3DSurface9* CD3D9RenderTargetTexture::getRenderTargetSurface()
{
    IDirect3DSurface9 *pRTTSurface = 0;
    if (Texture)
        Texture->GetSurfaceLevel(0, &pRTTSurface);

    if (pRTTSurface)
        pRTTSurface->Release();

    return pRTTSurface;
}

//----------------------------------------------------------------------------

img::IImage* CD3D9RenderTargetTexture::lock(u32 level)
{
	if (!Texture || level > 0 || m_LockImage[level])
	{
		LOGGER.logErr("Could not lock, D3D Render Target Texture (level %d)",
			level);
		return NULL;
	}

	HRESULT hr = 0;

	IDirect3DSurface9 * pSurface, * pTargetSurface;
	ID3DXBuffer * pBuffer;

	hr = Texture->GetSurfaceLevel(0, &pSurface);
	if (SUCCEEDED(hr))
		hr  = Device->CreateOffscreenPlainSurface(getSize().Width, getSize().Height,
			D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTargetSurface, NULL);

	if (SUCCEEDED(hr))
		hr = Device->GetRenderTargetData(pSurface, pTargetSurface);
	SAFE_RELEASE(pSurface);

	if (SUCCEEDED(hr))
		hr = D3DXSaveSurfaceToFileInMemory(
			&pBuffer, D3DXIFF_BMP, pTargetSurface, NULL, NULL);
	SAFE_RELEASE(pTargetSurface);

	if (SUCCEEDED(hr))
	{
		io::IReadFile * file = FILE_SYSTEM.createMemoryReadFile(
			pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), "#MemoryFile.bmp", false);
		if (file)
			m_LockImage[level] = IMAGE_LIBRARY.createImageFromFile(file);
		SAFE_DROP(file);
	}
	SAFE_RELEASE(pBuffer);

	if (FAILED(hr) || !m_LockImage[level])
	{
		LOGGER.logErr("Could not lock D3D Render Target Texture.");
 		return NULL;
	}

	return m_LockImage[level];
}

//----------------------------------------------------------------------------

void CD3D9RenderTargetTexture::unlock(u32 level)
{
	if (!Texture || level > 0 ||!m_LockImage[level])
	{
		LOGGER.logErr("Could not unlock D3D Render Target Texture (level %d)", level);
		return;
	}

	SAFE_DROP(m_LockImage[level]);
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

