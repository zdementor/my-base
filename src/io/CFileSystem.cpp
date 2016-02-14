//|-------------------------------------------------------------------------
//| File:        CFileSystem.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CFileSystem.h"
#include "CZipReader.h"
#include "CFileList.h"
#include "CXMLReader.h"
#include "CXMLWriter.h"
#include "CTextReader.h"
#include <res/IResourceManager.h>
#include <io/ILogger.h>
#include <io/IReadFile.h>
#include <io/IWriteFile.h>

//---------------------------------------------------------------------------

#ifdef WIN32
#include <direct.h> // for _chdir
#endif
#ifdef LINUX
#include <unistd.h>
#endif

//---------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

IReadFile* createReadFile(const c8* fileName);
IWriteFile* createWriteFile(const c8* fileName, bool append);
IReadFile* createMemReadFile(
	void* memory, s32 size, const c8* fileName, bool deleteMemoryWhenDropped
	);
IReadFile* createLimitedReadFile(
	const c8* fileName, IReadFile* alreadyOpenedFile, s32 areaSize
	);
IWriteFile* createMemWriteFile(const c8* fileName, core::stringc& out_buffer);
IWriteFile* createMemWriteFile(const c8* fileName, core::stringw& out_buffer);

//---------------------------------------------------------------------------

CFileSystem::CFileSystem()
	: m_FileList(NULL)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CFileSystem");
#endif
	_updateWorkingDirectory();
}

//---------------------------------------------------------------------------

CFileSystem::~CFileSystem()
{
    for (u32 i=0; i<m_ZipFileSystems.size(); ++i)
        m_ZipFileSystems[i]->drop();
	if (m_FileList)
	{
		m_FileList->unlockObject();
		m_FileList->drop();
		m_FileList = NULL;
	}
}

//---------------------------------------------------------------------------

IReadFile* CFileSystem::createAndOpenFile(const c8* filename)
{
    IReadFile* file = 0;

    for (u32 i=0; i<m_ZipFileSystems.size(); ++i)
    {
        file = m_ZipFileSystems[i]->openFile(filename);
        if (file)
            return file;
    }

    file = createReadFile(filename);
    return file;
}

//---------------------------------------------------------------------------

IWriteFile* CFileSystem::createAndWriteFile(const c8* filename, bool append)
{ return createWriteFile(filename, append); }

//---------------------------------------------------------------------------

IReadFile* CFileSystem::createMemoryReadFile(
	void* memory, s32 size, const c8* fileName, bool deleteMemoryWhenDropped)
{ return createMemReadFile(memory, size, fileName, deleteMemoryWhenDropped); }

//---------------------------------------------------------------------------

IReadFile* CFileSystem::createLimitReadFile(
	const c8* fileName, IReadFile* alreadyOpenedFile, s32 areaSize)
{ return createLimitedReadFile(fileName, alreadyOpenedFile, areaSize); }

//---------------------------------------------------------------------------

IWriteFile* CFileSystem::createMemoryWriteFile(
	const c8* fileName, core::stringc& out_buffer)
{ return createMemWriteFile(fileName, out_buffer); }

//---------------------------------------------------------------------------

IWriteFile* CFileSystem::createMemoryWriteFile(
	const c8* fileName, core::stringw& out_buffer)
{ return createMemWriteFile(fileName, out_buffer); }

//---------------------------------------------------------------------------

bool CFileSystem::addZipFileArchive(
	const c8* filename, bool ignoreCase, bool ignorePaths)
{
    IReadFile* file = createReadFile(filename);

    if (file)
    {
        CZipReader* zr = new CZipReader(file, ignoreCase, ignorePaths);

        if (zr)
            m_ZipFileSystems.push_back(zr);
        file->drop();

		if (zr)
			LOGGER.logInfo ("Package file mounted %s", filename);			
		else
			LOGGER.logErr("Could not mount package file %s", filename);			

        return zr != 0;
    }

    LOGGER.logErr("Could not open file. Package file not added %s", filename);
    
	return false;
}

//---------------------------------------------------------------------------

const c8* CFileSystem::getWorkingDirectory()
{
    return m_WorkingDirectory;
}

//---------------------------------------------------------------------------

bool CFileSystem::changeWorkingDirectoryTo(const c8* newDirectory)
{
	bool ret = false;
#if MY_PLATFORM == MY_PLATFORM_WIN32
    ret = (chdir(newDirectory) == 0);
#elif MY_PLATFORM == MY_PLATFORM_LINUX
    ret = !(chdir(newDirectory) == 0);
#endif
	_updateWorkingDirectory();
	if (m_FileList)
		((CFileList*)m_FileList)->onChangeWorkingDirectory();
    return ret;
}

//---------------------------------------------------------------------------

void CFileSystem::_updateWorkingDirectory()
{
#if MY_PLATFORM == MY_PLATFORM_WIN32
    _getcwd(m_WorkingDirectory, FILE_SYSTEM_MAX_PATH);
#elif MY_PLATFORM == MY_PLATFORM_LINUX
	#error "CFileSystem::_updateWorkingDirectory Not implemented"
#endif
}

//---------------------------------------------------------------------------

const IFileList* CFileSystem::getFileList()
{
	if (!m_FileList)
	{
		m_FileList = new CFileList();
		m_FileList->lockObject();
	}
	return m_FileList;
}

//---------------------------------------------------------------------------

bool CFileSystem::existFile(const c8 *filename)
{
    if (existFileInZip(filename))
		return true;
    return existFileOnDisk(filename);
}

//---------------------------------------------------------------------------

bool CFileSystem::existFileInZip(const c8 *filename)
{
	u32 zfcnt = m_ZipFileSystems.size();
    for (u32 i = 0; i < zfcnt; ++i)
        if (m_ZipFileSystems[i]->findFile(filename) != -1)
            return true;
	return false;
}

//---------------------------------------------------------------------------

bool CFileSystem::existFileOnDisk(const c8 *filename)
{
    FILE *f = fopen(filename, "rb");
    if (f) 
    {
        fclose(f);
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------

bool CFileSystem::existDir(const c8* dirname)
{
	core::stringc curdir = getWorkingDirectory();
    bool res = changeWorkingDirectoryTo(dirname);
	changeWorkingDirectoryTo(curdir.c_str());
	return res;
}

//---------------------------------------------------------------------------

bool CFileSystem::makeDir(const c8 * dirname)
{
#if MY_PLATFORM == MY_PLATFORM_WIN32
    return (_mkdir(dirname)==0);
#elif MY_PLATFORM == MY_PLATFORM_LINUX
    return (_mkdir(dirname)==0);
#else
	#error Unsupported platform! Abort! Abort!
#endif
    return false;
}

//---------------------------------------------------------------------------

IXMLReader* CFileSystem::createXMLReader(const c8* filename)
{
    IReadFile* file = createAndOpenFile(filename);
    if (!file)
        return 0;

    IXMLReader* reader = createXMLReader(file);
    file->drop();
    return reader;
}

//---------------------------------------------------------------------------

IXMLReader* CFileSystem::createXMLReader(IReadFile* file)
{
    if (!file)
        return 0;

    CXMLReader* xmlreader = new CXMLReader(file);

    return xmlreader;
}

//---------------------------------------------------------------------------

ITextReader* CFileSystem::createTextReader(const c8* filename)
{
    IReadFile* file = createAndOpenFile(filename);
    if (!file)
        return 0;

    ITextReader* reader = createTextReader(file);
    file->drop();
    return reader;
}

//---------------------------------------------------------------------------

ITextReader* CFileSystem::createTextReader(IReadFile* file)
{
    if (!file)
        return 0;

    ITextReader* txtreader = new CTextReader(file);

    return txtreader;
}

//---------------------------------------------------------------------------

IXMLWriter* CFileSystem::createXMLWriter(const c8* filename)
{
    IWriteFile *file = createAndWriteFile(filename);
	IXMLWriter *writer = NULL;
	if (file)
	{
		writer = createXMLWriter(file);
		file->drop();
	}
    return writer;
}

//---------------------------------------------------------------------------

IXMLWriter* CFileSystem::createXMLWriter(IWriteFile* file)
{
    return new CXMLWriter(file);
}

//---------------------------------------------------------------------------

const c8* CFileSystem::optimizeDirectoryString(const c8 *dir_to_optimize)
{
	// remember init working directory
	core::stringc workingDirectory = getWorkingDirectory();

	// change wworking dir
	changeWorkingDirectoryTo(dir_to_optimize);

	// getting optimized dir
	m_OptimizedDir = getWorkingDirectory();
	m_OptimizedDir.append("/");
	u32 strsz = m_OptimizedDir.size();
	for (u32 i = 0; i < strsz; i++)
		if (m_OptimizedDir[i] == '\\' )
			m_OptimizedDir[i] = '/';

	// restore initial working directory
	changeWorkingDirectoryTo(workingDirectory.c_str());

	return m_OptimizedDir.c_str();
}

//---------------------------------------------------------------------------

__MY_EXPORT__ IFileSystem* createFileSystem()
{
    return new CFileSystem();
}

//---------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//---------------------------------------------------------------------------

