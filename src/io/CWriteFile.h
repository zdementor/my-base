//|-------------------------------------------------------------------------
//| File:        CWriteFile.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CWriteFileHPP
#define CWriteFileHPP
//-----------------------------------------------------------------------------

#include <my_inc.h>
#include <io/IWriteFile.h>

//-----------------------------------------------------------------------------
namespace my {
namespace io {
//-----------------------------------------------------------------------------

//! Class for writing a real file to disk.
class CWriteFile : public IWriteFile
{
public:
    CWriteFile(const wchar_t* fileName, bool append);
    CWriteFile(const c8* fileName, bool append);
    virtual ~CWriteFile();

    virtual u32 write(const void* buffer, u32 sizeToWrite, bool flush);

    virtual bool seek(s32 finalPos, bool relativeMovement = false);

	virtual s32 getPos();

    virtual const c8* getFileName();

    virtual const void* getHandle()
	{ return (const FILE*)m_File; }

	bool _isOpen() { return m_File != 0; }

private:

	void _openFile(bool append);

	core::stringc	m_Filename;
	FILE			*m_File;
	s32				m_FileSize;
};

//-----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//-----------------------------------------------------------------------------

#endif

