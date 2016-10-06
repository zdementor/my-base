//|-------------------------------------------------------------------------
//| File:        CNullHardwareTexture.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CNullHardwareTextureHPP
#define CNullHardwareTextureHPP
//----------------------------------------------------------------------------

#include <vid/ITexture.h> 
#include <img/IImage.h>
#include "CNullDriver.h"

//----------------------------------------------------------------------------
namespace my { 
namespace vid {
//----------------------------------------------------------------------------

#define MY_TEXTURE_MAX_MIP_LEVELS 16

//! interface for a Video Driver dependent Texture.
class CNullHardwareTexture : public ITexture
{
public:

    CNullHardwareTexture(u32 flags);
    virtual ~CNullHardwareTexture();

	virtual void clear();

	virtual const core::dimension2d<s32>& getOriginalSize() { return m_ImageSize; }

	virtual const core::dimension2d<s32>& getSize() { return m_TextureSize; }

	virtual img::E_COLOR_FORMAT getColorFormat() { return m_ColorFormat; }

	virtual s32 getPitch() { return m_Pitch; }

    virtual E_TEXTURE_TYPE getType() { return m_Type; }
	virtual E_TEXTURE_FORMAT getFormat() { return m_Format; }

	virtual bool hasMipMaps() { return m_MaxMipMapLevels > 1 || m_AutogenMipMaps; }

	virtual u32 getMaxMipMapLevels() const { return m_MaxMipMapLevels; }

	virtual bool isCompressed()
	{ return m_IsCompressed; }

	virtual bool isRenderTarget() { return false; }

protected:

	virtual bool createHardwareTexture(bool renderTarget) { return false; }

	bool createTextureFrom(img::IImage* image);
	bool createEmptyTexture(
		const core::dimension2di &size, img::E_COLOR_FORMAT format, bool renderTarget = false);

	virtual bool createTextureLevel(u32 level, void *data, u32 dataSize, img::E_COLOR_FORMAT format);

	//! copies the image to the texture
	virtual bool copyTextureFrom(img::IImage* image);	

    core::dimension2d<s32> m_TextureSize;
    core::dimension2d<s32> m_ImageSize;
    s32 m_Pitch;
    img::E_COLOR_FORMAT m_ColorFormat;

	u32 m_CreationFlags;

	CNullDriver *m_Driver;

	u32 m_MaxMipMapLevels, m_BytesPerPixel;

	img::IImage	*m_LockImage[MY_TEXTURE_MAX_MIP_LEVELS];
	u32	m_ImageDataSizeBytes[MY_TEXTURE_MAX_MIP_LEVELS];

	bool m_AutogenMipMaps, m_IsCompressed;

	E_TEXTURE_TYPE m_Type;
	E_TEXTURE_FORMAT m_Format;
};

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif 

