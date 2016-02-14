//|-------------------------------------------------------------------------
//| File:        IImageLoader.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IImageLoaderHPP
#define IImageLoaderHPP
//-----------------------------------------------------------------------------

#include <IUnknown.h>
#include <io/IReadFile.h>
#include <img/IImage.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! Class which is able to create a image from a file.
//! If you want the Irrlicht Engine be able to load textures of
//! currently unsupported file formats (e.g .gif), then implement
//! this and add your new Surface loader with
//! IVideoDriver::addExternalImageLoader() to the engine. 
class __MY_IMG_LIB_API__ IImageLoader : public IUnknown
{
public:

    //! destructor
    virtual ~IImageLoader() {};

    //! returns true if the file maybe is able to be loaded by this class
    //! based on the file extension (e.g. ".tga")
    virtual bool isALoadableFileExtension(const c8* fileName) = 0;

    //! returns true if the file maybe is able to be loaded by this class
    virtual bool isALoadableFileFormat(io::IReadFile* file) = 0;

    //! creates a surface from the file
    virtual IImage* loadImage(io::IReadFile* file) = 0;

    //! загрузчик изображения из файла
    virtual IImage* loadImage(const c8* fileName) = 0;
};

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------

#endif

