//|-------------------------------------------------------------------------
//| File:        CSoftwareTexture.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CSoftwareTextureHPP
#define CSoftwareTextureHPP
//---------------------------------------------------------------------------

#include <vid/ITexture.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! interface for a Video Driver dependent Texture.
class CSoftwareTexture : public ITexture
{
public:

	CSoftwareTexture(core::dimension2di &size, img::E_COLOR_FORMAT format);
    CSoftwareTexture(img::IImage* surface);
    virtual ~CSoftwareTexture();

	virtual void clear();

	virtual img::IImage* lock(u32 level);
    virtual void unlock(u32 level);

    virtual const core::dimension2d<s32>& getOriginalSize();

    virtual const core::dimension2d<s32>& getSize();

    virtual img::E_COLOR_FORMAT getColorFormat();

    virtual s32 getPitch();

	virtual bool isLightmap() { return IsLightmap; }

	virtual void setIsLightmap(bool l) { IsLightmap = l; }

	virtual bool freeImageData() { return false; }

    img::IImage* getImage();

    img::IImage* getTexture();

    virtual void regenerateMipMapLevels();

    virtual E_TEXTURE_TYPE getType() { return ETT_COLOR_MAP; }
	virtual E_TEXTURE_FORMAT getFormat() { return ETF_TEXTURE_2D; }

	virtual void* getHardwareTexture() { return NULL; }

	virtual bool hasMipMaps() { return false; }

	virtual u32 getMaxMipMapLevels() const { return 1; }

	virtual bool isCompressed() { return false; }

private:

    //! returns the size of a texture which would be the optimize size for rendering it
    inline s32 getTextureSizeFromSurfaceSize(s32 size);

    img::IImage* Image;
    img::IImage* Texture;
    core::dimension2d<s32> OrigSize;

	bool IsLightmap;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif

