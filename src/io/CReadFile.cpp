//|-------------------------------------------------------------------------
//| File:        CReadFile.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CReadFile.h"
#include <io/ILogger.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
namespace my {
namespace io {
//-----------------------------------------------------------------------------

CReadFile::CReadFile(const c8* fileName) : FileSize(0)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CReadFile");
#endif

    Filename = fileName;
	FilePath = core::extractFilePath(Filename);

    openFile();
}

//-----------------------------------------------------------------------------

CReadFile::~CReadFile()
{   
	if (File) fclose(File);
}

//-----------------------------------------------------------------------------

inline bool CReadFile::isOpen()
{   
	return File != 0;
}

//-----------------------------------------------------------------------------

s32 CReadFile::read(void* buffer, s32 sizeToRead)
{   
	if (!isOpen())
        return 0;

    return (s32)fread(buffer, 1, sizeToRead, File);
}

//-----------------------------------------------------------------------------

bool CReadFile::seek(s32 finalPos, bool relativeMovement)
{
    if (!isOpen())
        return false;

    return fseek(File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}

//-----------------------------------------------------------------------------

s32 CReadFile::getSize()
{
    return FileSize;
}

//-----------------------------------------------------------------------------

s32 CReadFile::getPos()
{
    return ftell(File);
}

//-----------------------------------------------------------------------------

void CReadFile::openFile()
{
    if (Filename.size() == 0)
    {   
		File = 0;
        return;
    }

    File = fopen(Filename.c_str(), "rb");

    if (File)
    {   
        fseek(File, 0, SEEK_END);
        FileSize = ftell(File);
        fseek(File, 0, SEEK_SET);
    }
}

//-----------------------------------------------------------------------------

const c8* CReadFile::getFileName()
{   
	return Filename.c_str();
}

//-----------------------------------------------------------------------------

const c8* CReadFile::getFilePath()
{   
	return FilePath.c_str();
}

//-----------------------------------------------------------------------------

IReadFile* createReadFile(const c8* fileName)
{
    CReadFile *file = new CReadFile(fileName);
    if (file->isOpen())
        return file;
	LOGGER.logWarn("Can't open file %s for read", fileName);
    file->drop();
    return 0;
}
//-----------------------------------------------------------------------------
} // end namespace io
} // end namespace irr
//-----------------------------------------------------------------------------


