//|-------------------------------------------------------------------------
//| File:        CImageLoaderDevIL.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageLoaderTGAHPP
#define CImageLoaderTGAHPP
//|-------------------------------------------------------------------------

#include "CImageLoader.h"

//|-------------------------------------------------------------------------
namespace my {
namespace img {
//|-------------------------------------------------------------------------

class CImageLoaderDevIL : public CImageLoader
{
public:

    CImageLoaderDevIL();
    virtual ~CImageLoaderDevIL();

    virtual bool isALoadableFileExtension(const c8* fileName);

    virtual bool isALoadableFileFormat(io::IReadFile* file);

    virtual IImage* loadImage(io::IReadFile* file);

private:

};

//|-------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//|-------------------------------------------------------------------------

#endif

