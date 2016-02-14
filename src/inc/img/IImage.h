//|-------------------------------------------------------------------------
//| File:        IImage.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IImageHPP
#define IImageHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <my_inc.h>
#include <img/SColor.h>

//----------------------------------------------------------------------------
namespace my {
namespace img {
//----------------------------------------------------------------------------

//!	Interface for software image data.
//! Image loaders create these images from files. IVideoDrivers convert
// these images into their (hardware) textures.
class __MY_IMG_LIB_API__ IImage : public IUnknown
{
public:
	
	//! Returns image binary data (always 0 level).
	virtual void* getData() = 0;
	virtual const void* getData() const = 0;

	//! Returns image data size in bytes (always 0 level).
	virtual s32 getDataSizeBytes() const = 0;

	//! get a pointer to level data (face = 0 for non-cubemap images)
	virtual void* getLevelData(u32 level, u32 face = 0) = 0;
	virtual const void* getLevelData(u32 level, u32 face = 0) const = 0;

	//! return the Size in bytes of a level of the image
	virtual u32 getLevelDataSizeBytes(u32 level = 0) const = 0;

	//! return the number of mipmap levels available for the image (1 for images without mips)
	virtual u32 getLevelCount() const = 0;

	//! return the number of cubemap faces available for the image (0 for non-cubemap images)
	virtual u32 getFaceCount() const = 0;

	//! Returns width and height of image data.
	virtual const core::dimension2di& getDimension() const = 0;

	//! return the depth of the image (0 for images with no depth)
	virtual u32 getDepth() const = 0;

	//return whether the data is a compressed format
    virtual bool isCompressed() const = 0;

    //return whether the image represents a cubemap (false for simple images)
    virtual bool isCubeMap() const = 0;

    //return whether the image represents a volume (false for simple images)
    virtual bool isVolume() const = 0;

	//! Returns bytes per pixel
	virtual s32 getBytesPerPixel() const = 0;

	//! returns a pixel
	virtual SColor getPixel(s32 x, s32 y) const = 0;

	//! set pixel color
	virtual void setPixel(s32 x, s32 y, const SColor &color) = 0;

	//! operate on image
	virtual void operate(E_IMAGE_OPERATION op, f32 arg) = 0;

	//! merge two images into one
	virtual void blendWith(const IImage *img, E_IMAGE_OPERATION op) = 0;

	//! simple smoothing filter of the image
	virtual void smoothingFilter(
		bool rmask = true, bool gmask = true, bool bmask = true, bool amask = true) = 0;

	//! stamping filter of the image
	virtual void stampingFilter() = 0;

	//! returns the color format
	virtual E_COLOR_FORMAT getColorFormat() const = 0;

	//! converting image format
    virtual bool convertTo(E_COLOR_FORMAT new_color_format) = 0;

    //! copies this surface into another
    virtual bool copyTo(IImage* target, s32 x, s32 y) = 0;

    //! copies this surface into another
    virtual bool copyTo(IImage* target, const core::position2d<s32>& pos) = 0;

    //! copies this surface into another
    virtual bool copyTo(IImage* target,
		const core::position2di &pos, const core::recti &sourceRect, const core::recti *clipRect=0) = 0;

    //! fills the surface with color
    virtual void fill(const SColor &color) = 0;

    //! fills the surface with color
    virtual void floodFill(s32 startx, s32 starty, const SColor &color, const SColor &with_color) = 0;

	//! draws a border
    virtual void drawBorder(const core::recti &r, const SColor &color) = 0;

    //! draws a border
    virtual void drawBorder(s32 x, s32 y, s32 x2, s32 y2, const SColor &color) = 0;

	//! return true if size of this image can be decreased
	//! \param newrect - new image rectangle
	virtual bool requestToDecreaseSize(core::recti *newrect_out) = 0;

	//! copies this surface into another, scaling it to fit it.
    virtual bool copyTo(IImage* target) = 0;

    //! resizes the surface to a new size
    virtual bool resizeTo(const core::dimension2di &size) = 0;
	
    //! Modifies an alpha channel of the image based of an color key.
    virtual bool makeColorKey(const img::SColor &color) = 0;

    //! Modifies an alpha channel of the image based of an color key position.
    virtual bool makeColorKey(const core::position2di &colorKeyPixelPos) = 0;

	//! Creating normal map from heightmap image
	//! \param amplitude: Constant value by which the height information is multiplied.
    virtual bool makeNormalMap(f32 amplitude=1.0f) = 0;	

	virtual const c8* getOverridedFileName() = 0;	

	virtual void setOverridedFileName(const c8 *fname) = 0;	
};

//----------------------------------------------------------------------------

static MY_INLINE s32 getBitsPerPixelFromFormat(E_COLOR_FORMAT format)
{
    switch(format)
    {
    case ECF_A1R5G5B5:
        return 16;
    case ECF_R5G6B5:
        return 16;
    case ECF_R8G8B8:
        return 24;
    case ECF_A8R8G8B8:
        return 32;
    case ECF_A8B8G8R8:
        return 32;
    case ECF_DEPTH16:
        return 16;
    case ECF_ALPHA32F:
        return 32;
    }
    return 0;
}

//----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//----------------------------------------------------------------------------

#endif

