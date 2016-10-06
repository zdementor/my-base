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

CD3D9RenderTargetTexture::CD3D9RenderTargetTexture(
	const core::dimension2di &size, img::E_COLOR_FORMAT format, u32 flags) 
	: CD3D9Texture(flags), m_D3DSurface(0)
{
#if MY_DEBUG_MODE 
    IUnknown::setClassName("CD3D9RenderTargetTexture");
#endif

	bool ret = createEmptyTexture(size, format, true);

	if (ret)
	{
		HRESULT hr = Texture->GetSurfaceLevel(0, &m_D3DSurface);
		if (FAILED(hr))
		{
			LOGGER.logErr("Can't get render texture surface!");
			ret = false;
		}
	}

	LOGGER.log((!ret) ? io::ELL_ERROR : io::ELL_INFORMATION,
		"Created render target texture ( %s, mips %s, %dx%d )%s", 
		img::getColorFormatName(m_ColorFormat),
		hasMipMaps() ? "on" : "off",
		getSize().Width, getSize().Height,
		!ret ? " with errors" : "");
}

//----------------------------------------------------------------------------

CD3D9RenderTargetTexture::~CD3D9RenderTargetTexture()
{
    if (m_D3DSurface)
        m_D3DSurface->Release();

	LOGGER.logInfo("Destroyed render target texture ( %s, mips %s, %dx%d )", 
		img::getColorFormatName(m_ColorFormat),
		hasMipMaps() ? "on" : "off",
		getSize().Width, getSize().Height);
}

//----------------------------------------------------------------------------

IDirect3DSurface9* CD3D9RenderTargetTexture::getRenderTargetSurface()
{
    return m_D3DSurface;
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

