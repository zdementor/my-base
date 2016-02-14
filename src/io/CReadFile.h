//|-------------------------------------------------------------------------
//| File:        CReadFile.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CReadFileHPP
#define CReadFileHPP
//-----------------------------------------------------------------------------

#include <stdio.h>

#include <my_inc.h>
#include <io/IReadFile.h>

//-----------------------------------------------------------------------------
namespace my {
namespace io {
//-----------------------------------------------------------------------------

class CReadFile : public IReadFile
{
public:
    CReadFile(const wchar_t* fileName);
    CReadFile(const c8* fileName);
    virtual ~CReadFile();

    virtual s32 read(void* buffer, s32 sizeToRead);
    virtual bool seek(s32 finalPos, bool relativeMovement = false);

    virtual s32 getSize();

    bool isOpen();

    virtual s32 getPos();

    virtual const c8* getFileName();
    virtual const c8* getFilePath();

    virtual const void* getHandle() { return (const void*)File; }

private:

    //! opens the file
    void openFile();

    core::stringc Filename;
	core::stringc FilePath;

    FILE* File;
    s32 FileSize;
};
//-----------------------------------------------------------------------------
} // end namespace io
} // end namespace irr
//-----------------------------------------------------------------------------
#endif

