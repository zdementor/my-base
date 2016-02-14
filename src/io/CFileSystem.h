//|-------------------------------------------------------------------------
//| File:        CFileSystem.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CFileSystemHPP
#define CFileSystemHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include "../CompileConf.h"
#include <io/IFileSystem.h>

//---------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

class CZipReader;
const s32 FILE_SYSTEM_MAX_PATH = 1024;

//---------------------------------------------------------------------------

//! FileSystem which uses normal files and one zipfile
class CFileSystem : public IFileSystem
{
public:

    CFileSystem();
    virtual ~CFileSystem();

    virtual IReadFile* createAndOpenFile(const c8* filename);

    virtual IWriteFile* createAndWriteFile(const c8* filename, bool append=false);

	virtual IReadFile* createMemoryReadFile(
		void* memory, s32 size, const c8* fileName, bool deleteMemoryWhenDropped);

	virtual IReadFile* createLimitReadFile(
		const c8* fileName, IReadFile* alreadyOpenedFile, s32 areaSize);

	virtual IWriteFile* createMemoryWriteFile(const c8* fileName, core::stringc& out_buffer);
	virtual IWriteFile* createMemoryWriteFile(const c8* fileName, core::stringw& out_buffer);

    virtual bool addZipFileArchive(
		const c8* filename, bool ignoreCase = true, bool ignorePaths = true);

    virtual const c8* getWorkingDirectory();

    virtual bool changeWorkingDirectoryTo(const c8* newDirectory);

    virtual const IFileList* getFileList();

    virtual bool existFile(const c8 *filename);
	virtual bool existFileInZip(const c8 *filename);
	virtual bool existFileOnDisk(const c8 *filename);

	virtual bool existDir(const c8* dirname);

	virtual bool makeDir(const c8 * dirname);

    virtual IXMLReader* createXMLReader(const c8* filename);

    virtual IXMLReader* createXMLReader(IReadFile* file);

	ITextReader* createTextReader(const c8* filename);

	ITextReader* createTextReader(IReadFile* file);

    virtual IXMLWriter* createXMLWriter(const c8* filename);

    virtual IXMLWriter* createXMLWriter(IWriteFile* file);

	virtual const c8* optimizeDirectoryString(const c8 *dir_to_optimize);

private:

	void _updateWorkingDirectory();

    core::array<CZipReader*> m_ZipFileSystems;
    c8 m_WorkingDirectory[FILE_SYSTEM_MAX_PATH];
	core::stringc m_OptimizedDir;
	IFileList *m_FileList;
};
//---------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//---------------------------------------------------------------------------
#endif

