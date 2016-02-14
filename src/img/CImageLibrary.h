//|-------------------------------------------------------------------------
//| File:        CImageLibrary.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageLibraryHPP
#define CImageLibraryHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <img/IImageLibrary.h>
#include <img/ISurfaceLoader.h>
#include <img/ISurfaceSaver.h>

//----------------------------------------------------------------------------
namespace my {
namespace img {
//----------------------------------------------------------------------------

//! Image Library - singleton class eich provade all kinds of functions 
//! to work with 2-d image data
class __MY_SCN_LIB_API__ CImageLibrary : public IImageLibrary
{
public:

	CImageLibrary();
	virtual ~CImageLibrary();

	virtual IImage* createImageFromFile(const char* filename);

	virtual IImage* createImageFromFile(io::IReadFile* file);

	virtual IImage* createImageFromData(
		img::E_COLOR_FORMAT format, const core::dimension2d<s32> &size,
		u32 data_size, void *data, bool own_data);

    virtual IImage* createEmptyImage(
		const core::dimension2d<s32>& size,
        img::E_COLOR_FORMAT format = img::ECF_A1R5G5B5);

	virtual IImage* createImageFromAnother( 
		IImage* imageToCopy, E_COLOR_FORMAT format);

    virtual IImage* getImage(const c8* filename);

    virtual IImage* getImage(io::IReadFile* file);

    virtual void addImage(const c8* name, IImage* image);

    virtual IImage* findImage(const c8* filename);

	virtual core::stringc findImageFileName(IImage* img_ptr);

    virtual core::stringc findImageFullFileName(IImage* img_ptr);

    virtual bool removeImage(IImage* image);

    virtual void clearImageCache();

    virtual void clearUnusedImageCache();

	virtual void addExternalSurfaceLoader(IImageLoader* loader);

	virtual bool saveImageToFile(IImage* image, io::IWriteFile* file);

	virtual bool saveImageToFile(IImage* image, const c8* filename);

	virtual void addExternalSurfaceSaver(IImageSaver* saver);

	virtual bool copy(img::IImage *srcImage, img::IImage *dstImage);

private:

	struct SImageSurface
    {
        core::stringc  Filename;
        IImage*        ImageSurface;		
        bool operator < (const SImageSurface& other) const
        { return Filename<other.Filename; }
    };

	core::array <SImageSurface> Images;

	ISurfaceSaver   *SurfaceSaver;
	ISurfaceLoader  *SurfaceLoader;
	io::IFileSystem *FileSystem;
	io::ILogger     *Logger;
};

//----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CImageLibraryHPP