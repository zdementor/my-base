//|-------------------------------------------------------------------------
//| File:        CMemoryWriteFile.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CMemoryWriteFileHPP
#define CMemoryWriteFileHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <io/IWriteFile.h>

//---------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

//! Class for reading from memory.
class CMemoryWriteFile : public IWriteFile
{
public:
    
	CMemoryWriteFile(const c8* fileName, core::stringc& out_buffer);
	CMemoryWriteFile(const c8* fileName, core::stringw& out_buffer);
    virtual ~CMemoryWriteFile();

	virtual u32 write(const void* buffer, u32 sizeToWrite, bool flush);

	virtual bool seek(s32 finalPos, bool relativeMovement = false);

	virtual s32 getPos();

	virtual const c8* getFileName() { return Filename.c_str(); }

	virtual const void* getHandle() { return 0; }

private:

    core::stringc Filename, FilePath;
	core::stringw *BufferWPtr;
	core::stringc *BufferCPtr;
    u32 Pos;
};

//---------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//---------------------------------------------------------------------------
#endif

