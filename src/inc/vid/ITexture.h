//|-------------------------------------------------------------------------
//| File:        ITexture.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ITextureHPP
#define ITextureHPP
//---------------------------------------------------------------------------

#include <IUnknown.h>
#include <img/IImage.h>
#include <my_inc.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! Interface for a Video Driver dependent Texture.
//! An ITexture is created by an IVideoDriver by using IVideoDriver::addTexture or
//! IVideoDriver::getTexture. After this, this texture may only be used by this VideoDriver.
//! As you can imagine, textures of the DirectX and the OpenGL device will not be compatible,
//! for example.
//! An exception is the Software device and the NULL device, their textures are compatible.
//! If you try to use a texture created by one device with an other device, the device
//! will refuse to do that and write a warning or an error message to the output buffer.
class __MY_VID_LIB_API__ ITexture : public IUnknown
{
public:

	virtual void clear() = 0;

    //! Lock function. Locks the Texture and returns a pointer to access the 
    //! pixels. After lock() has been called and all operations on the pixels
    //! are done, you must call unlock().
    //! \return Returns a pointer to the pixel data. The format of the pixel can
    //! be determinated by using getColorFormat(). NULL is returned, if
    //! the texture cannot be locked.
	virtual img::IImage* lock(u32 level = 0) = 0;

    //! Unlock function. Must be called after a lock() to the texture.
    virtual void unlock(u32 level = 0) = 0;

    //! Returns original size of the texture. The texture is usually
    //! scaled, if it was created with an unoptimal size. For example if the size
    //! of the texture file it was loaded from was not a power of two. This returns
    //! the size of the texture, it had before it was scaled. Can be useful
    //! when drawing 2d images on the screen, which should have the exact size
    //! of the original texture. Use ITexture::getSize() if you want to know 
    //! the real size it has now stored in the system.
    //! \return Returns the original size of the texture.
    virtual const core::dimension2d<s32>& getOriginalSize() = 0;

    //! Returns (=size) of the texture.
    //! \return Returns the size of the texture.
    virtual const core::dimension2d<s32>& getSize() = 0;

    //! Returns the color format of texture. This format is in most cases
    //! ECF_A1R5G5B5 or ECF_A8R8G8B8.
    //! \return Returns the color format of texture.
    virtual img::E_COLOR_FORMAT getColorFormat() = 0;

    //! Returns pitch of texture (in bytes). The pitch is the amount of bytes
    //! used for a row of pixels in a texture.
    //! \return Returns pitch of texture in bytes.
    virtual s32 getPitch() = 0;

	//! frees image data of this texture
	//! Warning!!! after that methods lock() and unlock()
	//! will not be working
	virtual bool freeImageData()=0;

	//! Regenerates the mip map levels of the texture.
    //! Useful after locking and modifying the texture 
    virtual void regenerateMipMapLevels() = 0;

	//! getting texture type
    virtual E_TEXTURE_TYPE getType() = 0;

	//! getting texture format
    virtual E_TEXTURE_FORMAT getFormat() = 0;

	//! return true if this texture has mip map levels
	virtual bool hasMipMaps() = 0;

	//! return maximal amount of the texture mip levels
	virtual u32 getMaxMipMapLevels() const = 0;

	//! return pointer to hardware texture
	virtual void * getHardwareTexture() = 0;

	//! return true if this texture is compressed
	virtual bool isCompressed() = 0;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef ITextureHPP

