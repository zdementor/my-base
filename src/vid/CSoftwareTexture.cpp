//|-------------------------------------------------------------------------
//| File:        CSoftwareTexture.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CSoftwareTexture.h"
#include <io/ILogger.h>
#include <img/IImageLibrary.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

CSoftwareTexture::CSoftwareTexture(core::dimension2di &size, img::E_COLOR_FORMAT format)
	: Texture(0), IsLightmap(false)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CSoftwareTexture");
#endif

	OrigSize = size;

    core::dimension2di optSize(
		getTextureSizeFromSurfaceSize(OrigSize.Width),
		getTextureSizeFromSurfaceSize(OrigSize.Height));

    Image = IMAGE_LIBRARY.createEmptyImage(size, format);

    if (optSize == OrigSize)
    {
        Texture = Image;
        Texture->grab();
    }
    else
    {
        Texture = IMAGE_LIBRARY.createEmptyImage(optSize, format);
        Image->copyTo(Texture);          
    }
}

//---------------------------------------------------------------------------

CSoftwareTexture::CSoftwareTexture(img::IImage* image)
	: Texture(0), IsLightmap(false)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CSoftwareTexture");
#endif

    if (image)
    {
		OrigSize = image->getDimension();

		core::dimension2di optSize(
			getTextureSizeFromSurfaceSize(OrigSize.Width),
			getTextureSizeFromSurfaceSize(OrigSize.Height));

        Image = IMAGE_LIBRARY.createImageFromAnother(image, img::ECF_A1R5G5B5);

        if (optSize == OrigSize)
        {
            Texture = Image;
            Texture->grab();
        }
        else
        {
            Texture = IMAGE_LIBRARY.createEmptyImage(optSize, img::ECF_A1R5G5B5);
            Image->copyTo(Texture);          
        }
    }
}

//---------------------------------------------------------------------------

CSoftwareTexture::~CSoftwareTexture()
{
    clear();
}

//---------------------------------------------------------------------------

void CSoftwareTexture::clear()
{
    SAFE_DROP(Image);
	SAFE_DROP(Texture);
}

//---------------------------------------------------------------------------

img::IImage* CSoftwareTexture::lock(u32 level)
{
    return Texture;
}

//---------------------------------------------------------------------------

//! unlock function
void CSoftwareTexture::unlock(u32 level)
{
    if (Image != Texture)
    {
        LOGGER.logWarn("Performance warning, slow unlock of non power of 2 texture.");
        Image->copyTo(Texture);
    }
}

//---------------------------------------------------------------------------

//! Returns original size of the texture.
const core::dimension2d<s32>& CSoftwareTexture::getOriginalSize()
{
    return OrigSize;
}

//---------------------------------------------------------------------------

//! Returns (=size) of the texture.
const core::dimension2d<s32>& CSoftwareTexture::getSize()
{
    return Image->getDimension();
}

//---------------------------------------------------------------------------

//! returns unoptimized surface
img::IImage* CSoftwareTexture::getImage()
{
    return Image;
}

//---------------------------------------------------------------------------

//! returns texture surface
img::IImage* CSoftwareTexture::getTexture()
{
    return Texture;
}

//---------------------------------------------------------------------------

//! returns the size of a texture which would be the optimize size for rendering it
inline s32 CSoftwareTexture::getTextureSizeFromSurfaceSize(s32 size)
{
    s32 ts = 0x01;
    while(ts < size)
        ts <<= 1;

    return ts;
}

//---------------------------------------------------------------------------

//! returns color format of texture
img::E_COLOR_FORMAT CSoftwareTexture::getColorFormat()
{
    return img::ECF_A1R5G5B5;
}

//---------------------------------------------------------------------------

//! returns pitch of texture (in bytes)
s32 CSoftwareTexture::getPitch()
{
    return Image->getDimension().Width * 2;
}

//---------------------------------------------------------------------------

//! Regenerates the mip map levels of the texture. Useful after locking and 
//! modifying the texture
void CSoftwareTexture::regenerateMipMapLevels()
{
    // our software textures don't have mip maps
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

