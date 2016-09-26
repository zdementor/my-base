//|-------------------------------------------------------------------------
//| File:        MyCEGUITexture.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <CEGUISystem.h>
#include <CEGUIDefaultResourceProvider.h>
#include "MyCEGUIRenderer.h"

#include "MyCEGUITexture.h"
#include "MyEngineResourceProvider.h"

#include <img/IImage.h>
#include <vid/IVideoDriver.h>
#include <io/ILogger.h>

//------------------------------------------------------------------------------------
namespace CEGUI { // Start of CEGUI namespace section
//------------------------------------------------------------------------------------

// counter for unique texture names
static int _iTextureNumber = 0;
static my::core::stringc _UniqueName = "";

//------------------------------------------------------------------------------------

static const c8* _GetUniqueName(void)
{
	_UniqueName.sprintf("CEGUI-Texture %d", _iTextureNumber++);
	return _UniqueName.c_str();
}
//------------------------------------------------------------------------------------

MyCEGUITexture::MyCEGUITexture(MyCEGUIRenderer &r)
	: m_Texture(0), m_Renderer(r),
	m_Size(0, 0), m_DataSize(0, 0), m_TexelScaling(0, 0),
	m_Driver(VIDEO_DRIVER), m_UseAlphaBlending(true)
{
}

//------------------------------------------------------------------------------------

MyCEGUITexture::MyCEGUITexture(MyCEGUIRenderer &r, const Size& tex_sz)
	: m_Texture(0), m_Renderer(r),
	m_Size(tex_sz), m_DataSize(tex_sz), m_TexelScaling(0, 0),
	m_Driver(VIDEO_DRIVER)
{
	const core::dimension2di size(
		static_cast<u32>(tex_sz.d_width),
		static_cast<u32>(tex_sz.d_height));

	const my::c8 *texname = _GetUniqueName();

	m_Texture = m_Driver.addTexture(
		texname, size, img::ECF_A8R8G8B8);
	m_Texture->grab();

	m_Size = CEGUI::Size(
		static_cast<f32>(m_Texture->getSize().Width),
		static_cast<f32>(m_Texture->getSize().Height));
	m_DataSize = CEGUI::Size(
		static_cast<f32>(m_Texture->getOriginalSize().Width),
		static_cast<f32>(m_Texture->getOriginalSize().Height));
	_updateCachedScaleValues();
}

//------------------------------------------------------------------------------------

MyCEGUITexture::~MyCEGUITexture()
{
	_freeTexture();
}

//------------------------------------------------------------------------------------

my::vid::ITexture* MyCEGUITexture::getTexture()
{
	return m_Texture;
}

//------------------------------------------------------------------------------------

void MyCEGUITexture::setTexture(vid::ITexture *mytex, bool useAlphaBlending)
{
	m_UseAlphaBlending = useAlphaBlending;

	if (m_Texture == mytex)
		return;

	_freeTexture(true);

	m_Texture = mytex;

	if (m_Texture)
	{
		m_Texture->grab();

		m_Size = CEGUI::Size(
			static_cast<float>(m_Texture->getSize().Width),
			static_cast<float>(m_Texture->getSize().Height));
		m_DataSize = CEGUI::Size(
			static_cast<float>(m_Texture->getOriginalSize().Width),
			static_cast<float>(m_Texture->getOriginalSize().Height));
		_updateCachedScaleValues();
	}
}

//------------------------------------------------------------------------------------

void MyCEGUITexture::_freeTexture(bool remove)
{
	if(m_Texture) 
	{
		m_Texture->drop();
		if (remove && m_Texture->getReferenceCounter()==1)
			m_Driver.removeTexture(m_Texture);
	}
	m_Texture=0;
}

//------------------------------------------------------------------------------------

const Size& MyCEGUITexture::getSize() const
{
	return m_Size;
}

//------------------------------------------------------------------------------------

const Size& MyCEGUITexture::getOriginalDataSize() const
{
    return m_DataSize;
}

//------------------------------------------------------------------------------------

const Vector2& MyCEGUITexture::getTexelScaling() const
{
    return m_TexelScaling;
}

//------------------------------------------------------------------------------------

void MyCEGUITexture::loadFromFile(const String& filename, const String& resourceGroup)
{
	const c8 *filename_c = filename.c_str();
	const c8 *resourceGroup_c = resourceGroup.c_str();

	_freeTexture(true);

	bool mipfl = m_Driver.getTextureCreationFlag(my::vid::ETCF_AUTOGEN_MIP_MAPS);
	
	m_Driver.setTextureCreationFlag(my::vid::ETCF_AUTOGEN_MIP_MAPS,false);
  
	MyEngineResourceProvider* rp = 
        static_cast<MyEngineResourceProvider*>(System::getSingleton().getResourceProvider());

	String final_filename;
	if (resourceGroup.size() > 0)
		final_filename = rp->getFinalFilename(filename, resourceGroup);
	else
		final_filename = filename;
		
	m_Texture = m_Driver.getTexture(final_filename.c_str());

	m_Driver.setTextureCreationFlag(my::vid::ETCF_AUTOGEN_MIP_MAPS,mipfl);

	if (m_Texture)
	{
		m_Texture->grab();

		m_Size = CEGUI::Size(
			static_cast<float>(m_Texture->getSize().Width),
			static_cast<float>(m_Texture->getSize().Height));
		m_DataSize = CEGUI::Size(
			static_cast<float>(m_Texture->getOriginalSize().Width),
			static_cast<float>(m_Texture->getOriginalSize().Height));
		_updateCachedScaleValues();
	}
	else
	{
		LOGGER.logWarn("Can't load CEGUI texture from file '%s' !",
			final_filename.c_str());
	}
}

//------------------------------------------------------------------------------------

void MyCEGUITexture::loadFromMemory(
	const void* buffPtr, const Size& buffer_size, PixelFormat pf)
{
	my::core::dimension2di dim(buffer_size.d_width, buffer_size.d_height);

	if (m_Texture && (
			(pf == CEGUI::Texture::PF_RGBA && m_Texture->getColorFormat() != my::img::ECF_A8R8G8B8) ||
			(pf == CEGUI::Texture::PF_RGB && m_Texture->getColorFormat() != my::img::ECF_R8G8B8) ||
			dim != m_Texture->getSize()))
		_freeTexture(true);

	bool mipfl = m_Driver.getTextureCreationFlag(my::vid::ETCF_AUTOGEN_MIP_MAPS);

	m_Driver.setTextureCreationFlag(my::vid::ETCF_AUTOGEN_MIP_MAPS,false);
	
	const my::c8 *texname = _GetUniqueName();
	
	if (!m_Texture)
	{
		m_Texture = m_Driver.addTexture(texname, dim,
			(pf == CEGUI::Texture::PF_RGBA) ? my::img::ECF_A8R8G8B8 : my::img::ECF_R8G8B8);
		m_Texture->grab();
	}
		
	if (pf == CEGUI::Texture::PF_RGBA || pf == CEGUI::Texture::PF_RGB)
	{
		my::img::IImage *tex_img = m_Texture->lock();
		if (tex_img )
			memcpy(tex_img->getData(), buffPtr, tex_img->getDataSizeBytes());
		m_Texture->unlock();
	}
	
	m_Driver.setTextureCreationFlag(my::vid::ETCF_AUTOGEN_MIP_MAPS, mipfl);

	m_Size = m_DataSize = CEGUI::Size(
		static_cast<float>(m_Texture->getSize().Width),
		static_cast<float>(m_Texture->getSize().Height));
	m_DataSize = buffer_size;
	_updateCachedScaleValues();
}

//------------------------------------------------------------------------------------

void MyCEGUITexture::saveToMemory(void* buffer)
{
    if (!m_Texture)
        return;

    const size_t sz = static_cast<size_t>(m_Size.d_width * m_Size.d_height) * 4;
    memcpy(buffer, m_Texture->lock(), sz);
    m_Texture->unlock();
}

//------------------------------------------------------------------------------------

void MyCEGUITexture::_updateCachedScaleValues()
{
    //
    // calculate what to use for x scale
    //
    const float orgW = m_DataSize.d_width;
    const float texW = m_Size.d_width;

    // if texture and original data width are the same, scale is based
    // on the original size.
    // if texture is wider (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    m_TexelScaling.d_x = 1.0f / ((orgW == texW) ? orgW : texW);

    //
    // calculate what to use for y scale
    //
    const float orgH = m_DataSize.d_height;
    const float texH = m_Size.d_height;

    // if texture and original data height are the same, scale is based
    // on the original size.
    // if texture is taller (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    m_TexelScaling.d_y = 1.0f / ((orgH == texH) ? orgH : texH);
}

//------------------------------------------------------------------------------------
} // End of  CEGUI namespace section
//------------------------------------------------------------------------------------
