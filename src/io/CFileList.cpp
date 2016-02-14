//|-------------------------------------------------------------------------
//| File:        CFileList.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CFileList.h"

#ifdef LINUX
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#ifdef WIN32
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#endif

#include <io/ILogger.h>
#include <io/IFileSystem.h>

//-------------------------------------------------------------------------
namespace my {
namespace io {
//-------------------------------------------------------------------------

CFileList::CFileList()
	: m_NeedToUpdate(true)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CFileList");
#endif
	onChangeWorkingDirectory();
}

//-------------------------------------------------------------------------

CFileList::~CFileList()
{
}

//-------------------------------------------------------------------------

s32 CFileList::getFileCount() const
{
	if (m_NeedToUpdate)
		update();
    return m_Files.size();
}

//-------------------------------------------------------------------------

const c8* CFileList::getFileExt(s32 index) const
{
	if (m_NeedToUpdate)
		update();
    if (index < 0 || index > (s32)m_Files.size())
        return 0;
    return m_Files[index].Ext.c_str();
}

//-------------------------------------------------------------------------

const c8* CFileList::getFileName(s32 index) const
{
	if (m_NeedToUpdate)
		update();
    if (index < 0 || index > (s32)m_Files.size())
        return 0;
    return m_Files[index].Name.c_str();
}

//-------------------------------------------------------------------------

const c8* CFileList::getFullFileName(s32 index) const
{
	if (m_NeedToUpdate)
		update();
    if (index < 0 || index > (s32)m_Files.size())
        return 0;
    if (m_Files[index].FullName.size() < m_Files[index].Name.size())
    {
        // create full name
        m_Files[index].FullName = m_Path;
        if (m_Path.size() > 3)
            m_Files[index].FullName.append("/");
        m_Files[index].FullName.append(m_Files[index].Name);
    }
    return m_Files[index].FullName.c_str();
}

//-------------------------------------------------------------------------

bool CFileList::isDirectory(s32 index) const
{
	if (m_NeedToUpdate)
		update();
    if (index < 0 || index > (s32)m_Files.size())
        return false;
    return m_Files[index].isDirectory;
}

//-------------------------------------------------------------------------

s32 CFileList::getFileSizeInBytes(s32 index) const
{
	if (m_NeedToUpdate)
		update();
	if (index < 0 || index > (s32)m_Files.size() || isDirectory(index))
        return 0;
    return m_Files[index].Size;
}

//-------------------------------------------------------------------------

const c8* CFileList::getWorkingDirectory() const
{
	return m_Path.c_str();
}

//-------------------------------------------------------------------------

void CFileList::update() const
{
	m_Files.set_used(0);
#if MY_PLATFORM == MY_PLATFORM_WIN32

	struct _finddata_t c_file;
    s32 hFile;
    FileEntry entry;

    if( (hFile = _findfirst( "*", &c_file )) != -1L )
    {
        do
        {
			entry.Name = c_file.name;
			entry.Name.replace_char('\\', '/');
			entry.Ext = core::extractFileExt(entry.Name);
            entry.Size = c_file.size;
            entry.isDirectory = (_A_SUBDIR & c_file.attrib) != 0;
            m_Files.push_back(entry);
        }
        while( _findnext( hFile, &c_file ) == 0 );

        _findclose( hFile );
    }
    //TODO add drives
    //entry.Name = "E:\\";
    //entry.isDirectory = true;
    //m_Files.push_back(entry);
#elif MY_PLAFORM == MY_PLATFORM_LINUX

    struct dirent **namelist;
    FileEntry entry;

    s32 n = scandir(".", &namelist, 0, alphasort);
    if (n >= 0)
    {
        while(n--)
        {
            entry.Name = namelist[n]->d_name;
            entry.Size = 0;
            entry.isDirectory = namelist[n]->d_type == DT_DIR;
            m_Files.push_back(entry);
            free(namelist[n]);
        }
        free(namelist);
    }
#endif
	m_Files.sort();
	m_NeedToUpdate = false;
}

//-------------------------------------------------------------------------

void CFileList::onChangeWorkingDirectory()
{
	m_NewWorkingDir = FILE_SYSTEM.getWorkingDirectory();
	m_NewWorkingDir.replace_char('\\', '/');
	if (m_Path != m_NewWorkingDir)
	{
		m_Path = m_NewWorkingDir;
		m_NeedToUpdate = true;
	}
}

//-------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//-------------------------------------------------------------------------

