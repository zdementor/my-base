//|-------------------------------------------------------------------------
//| File:        CWriteFile.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CWriteFile.h"
#include <io/ILogger.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
namespace my {
namespace io {
//-----------------------------------------------------------------------------

CWriteFile::CWriteFile(const c8* fileName, bool append) : m_FileSize(0)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CWriteFile");
#endif

    m_Filename = fileName;
    _openFile(append);
}

//-----------------------------------------------------------------------------

CWriteFile::~CWriteFile()
{   
	if (m_File) fclose(m_File);
}

//-----------------------------------------------------------------------------

u32 CWriteFile::write(const void* buffer, u32 sizeToWrite, bool flush)
{
    if (!m_File)
        return 0;
	u32 r = (u32)fwrite(buffer, 1, sizeToWrite, m_File);
	if (flush)
		fflush(m_File);
    return r;
}

//-----------------------------------------------------------------------------

bool CWriteFile::seek(s32 finalPos, bool relativeMovement)
{   
	if (!m_File)
        return false;
    return fseek(m_File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}

//-----------------------------------------------------------------------------

s32 CWriteFile::getPos()
{   
	return ftell(m_File);
}

//-----------------------------------------------------------------------------

void CWriteFile::_openFile(bool append)
{   
	if (m_Filename.size() == 0)
    {
		m_File = 0;
        return;
    }

    m_File = fopen(m_Filename.c_str(), append ? "ab" : "wb");
    if (m_File)
    {   // get m_FileSize
        fseek(m_File, 0, SEEK_END);
        m_FileSize = ftell(m_File);
        fseek(m_File, 0, SEEK_SET);
    }
}

//-----------------------------------------------------------------------------

const c8* CWriteFile::getFileName()
{   
	return m_Filename.c_str();
}

//-----------------------------------------------------------------------------

IWriteFile* createWriteFile(const c8* fileName, bool append)
{
	CWriteFile *file = new CWriteFile(fileName, append);
    if (file->_isOpen())
        return file;
	LOGGER.logWarn("Can't open file %s for write", fileName);
    file->drop();
    return 0;
}

//-----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//-----------------------------------------------------------------------------
