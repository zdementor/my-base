//|-------------------------------------------------------------------------
//| File:        CImageLoader.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CImageLoader.h"
#include <io/IFileSystem.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! загружает изображение из файла
IImage* CImageLoader::loadImage(const c8* fileName)
{   
	io::IReadFile *file = 
		FILE_SYSTEM.createAndOpenFile(fileName);

    IImage* image = loadImage(file);

    SAFE_DROP(file);

    return image;
}

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------
