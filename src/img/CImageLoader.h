//|-------------------------------------------------------------------------
//| File:        CImageLoader.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageLoaderHPP
#define CImageLoaderHPP
//-----------------------------------------------------------------------------
#include "../io/CReadFile.h"
#include <img/IImageLoader.h>
#include <img/IImage.h>
//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------
//! Базовый не абстрактный файл для загрузки изображений из файла                                 
class CImageLoader : public IImageLoader
{
public:
    //! конструктор
    CImageLoader() 
    {
	#if MY_DEBUG_MODE  
		IUnknown::setClassName("CImageLoader");
	#endif
    }

    //! деструктор
    virtual ~CImageLoader() {};

    //! returns true if the file maybe is able to be loaded by this class
    //! based on the file extension (e.g. ".tga")
    virtual bool isALoadableFileExtension(const c8* fileName) {return 0;}

    //! returns true if the file maybe is able to be loaded by this class
    virtual bool isALoadableFileFormat(io::IReadFile* file) {return 0;}

    //! creates a surface from the file
    virtual IImage* loadImage(io::IReadFile* file) {return 0;}

    //! загружает изображение из файла
    virtual IImage* loadImage(const c8* fileName);

};
//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------
#endif

