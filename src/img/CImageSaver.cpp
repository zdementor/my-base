//|-------------------------------------------------------------------------
//| File:        CImageSaver.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CImageSaver.h"
#include <io/IFileSystem.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! write image to file
bool CImageSaver::saveImage(IImage *image, const c8* fileName)
{   
	io::IWriteFile *file = 
		FILE_SYSTEM.createAndWriteFile(fileName,false);

    bool res = saveImage(image, file);

    SAFE_DROP(file);  

    return res;
}

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------
