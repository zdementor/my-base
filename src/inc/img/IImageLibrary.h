//|-------------------------------------------------------------------------
//| File:        IImageLibrary.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IImageLibraryHPP
#define IImageLibraryHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <ISingleton.h>
#include <img/SColor.h>

//----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! Image Library - singleton class wich provided all kinds of functions 
//! to work with software 2-d image data
class __MY_IMG_LIB_API__ IImageLibrary : public IUnknown, public ISingleton<IImageLibrary>
{
public:

	//! destructor
	virtual ~IImageLibrary() {};

    //! Creates a software image from a file. No hardware texture will
    //! be created for this image. This method is useful for example if
    //! you want to read a heightmap for a terrain renderer.
    //! \param filename: Name of the file from which the image is created.
    //! \return Returns the created image.
    //! If you no longer need the image, you should call IImage::drop().
    //! See IUnknown::drop() for more information.
	virtual IImage* createImageFromFile(const char* filename) = 0;

    //! Creates a software image from a file. No hardware texture will
    //! be created for this image. This method is useful for example if
    //! you want to read a heightmap for a terrain renderer.
    //! \param file: File from which the image is created.
    //! \return Returns the created image.
    //! If you no longer need the image, you should call IImage::drop().
    //! See IUnknown::drop() for more information.
	virtual IImage* createImageFromFile(io::IReadFile* file) = 0;

    //! Creates a software image from a byte array. No hardware texture will
    //! be created for this image. This method is useful for example if
    //! you want to read a heightmap for a terrain renderer.
    //! \param format: Desired color format of the texture
    //! \param size: Desired the size of the image
    //! \param data: a byte array with pixelcolor information
	//! \param data_size: size of the input data (can be 0 for uncompressed formats)
    //! \return Returns the created image.
    //! If you no longer need the image, you should call IImage::drop().
    //! See IUnknown::drop() for more information.
	virtual IImage* createImageFromData(
		img::E_COLOR_FORMAT format, const core::dimension2d<s32> &size,
		u32 data_size, void *data, bool own_data=true) = 0;

    //! Creates an empty image of specified size.
    //! \param size: Size of the Image.
    //! \param format: Desired color format of the Image. 
    //! \return Returns a pointer to the new created Image. 
    virtual IImage* createEmptyImage(
		const core::dimension2d<s32>& size,
        img::E_COLOR_FORMAT format = img::ECF_A1R5G5B5
		) = 0;

	//! Creating image from anothe image surface
	virtual IImage* createImageFromAnother( 
		IImage* imageToCopy, E_COLOR_FORMAT format
		) = 0;

	//! Returns a pointer to a image. Loads the image if it is not
    //! already loaded.
    //! The image can be in BMP, JPG, TGA, PCX, PSD, PNG format.    
    //! \param filename: Filename of the image to be loaded.
    //! \return Returns a pointer to the image and NULL if the image
    //! could not be loaded. 
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    virtual IImage* getImage(const c8* filename) = 0;

    //! Returns a pointer to a image. Loads the image if it is not
    //! already loaded.
    //! The image can be in BMP, JPG, TGA, PCX, PSD, PNG format.    
    //! \param file: Pointer to an already opened file.
    //! \return Returns a pointer to the image and NULL if the image
    //! could not be loaded. 
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    virtual IImage* getImage(io::IReadFile* file) = 0;

    //! Adds Image to the images cache of the engine
    //! \param name: A name for the image. Later calls of getImage() with this name
    //! will return this image
    //! \param image: Image wich we wants to add to the images cache
    virtual void addImage(const c8* name, IImage* image) = 0;

	//! Adds an empty image of specified size.
	//! \param name: A name for the image. Later calls of getImage() with this name
    //! will return this image
    //! \param size: Size of the Image.
    //! \param format: Desired color format of the Image. 
    //! \return Returns a pointer to the new created Image. 
	virtual IImage* addImage(const c8* name,
		const core::dimension2di &size, img::E_COLOR_FORMAT format) = 0;

	//! looks if the image is already loaded
    virtual IImage* findImage(const c8* filename) = 0;

	//! looks if the image is already loaded
	virtual const c8* findImageFileName(IImage* image) = 0;

    //! looks if the image is already loaded
    virtual const c8* findImageFullFileName(IImage* image) = 0;

    //! Removes an image from the images cache and deletes it. Please note that 
	//! after calling this, the pointer to this IImage
    //! may not be longer valid, if it was not grabbed before by other parts of 
    //! the engine for storing it longer.
    virtual bool removeImage(IImage* image) = 0;

    //! Removes all unused images from the images cache and deletes them, freeing lot of
    //! memory. Please note that after calling this, the pointer to all IImage
    //! may not be longer valid
    virtual void clearUnusedImageCache() = 0;

	//! Adds an external surface loader to the array of loaders. This is useful if 
    //! the MyEngine Engine should be able to load images of currently 
    //! unsupported file formats (e.g .gif). The IImageLoader only needs
    //! to be implemented for loading this file format. A pointer to
    //! the implementation can be passed to the engine using this method.
    //! \param loader: Pointer to the external loader created.
	virtual void addExternalSurfaceLoader(IImageLoader* loader) = 0;

    //! Saves a software image to a file.
    //! \param file: File to which the image is saving.
    //! \return true on success    
	virtual bool saveImageToFile(IImage* image, io::IWriteFile* file) = 0;

    //! Saves a software image to a file.
    //! \param filename: File Name of file to which the image is saving.
    //! \return true on success    
	virtual bool saveImageToFile(IImage* image, const c8* filename) = 0;

	//! Adds an external surface saver to the array of savers
	virtual void addExternalSurfaceSaver(IImageSaver* saver) = 0;

	//! Copies the one image to the another
	virtual bool copy(img::IImage *srcImage, img::IImage *dstImage) = 0;

	//!	Return singleton object
	static IImageLibrary& getSingleton();

	//! Return pointer to singleton object
	static IImageLibrary* getSingletonPtr();	
};

//----------------------------------------------------------------------------

__MY_IMG_LIB_API__ IImageLibrary* createImageLibrary();

//----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IImageLibraryHPP