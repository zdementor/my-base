//|-------------------------------------------------------------------------
//| File:        IFileSystem.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IFileSystemHPP
#define IFileSystemHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <ISingleton.h>

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

//! The FileSystem manages files and archives and provides access to them.
//!
//! It manages where files are, so that modules which
//! use the the IO do not need to know where every file is located. A file
//! could be in a .zip-Archive or as file on disk, using the IFileSystem
//! makes no difference to this.
class __MY_IO_LIB_API__ IFileSystem : public IUnknown, public ISingleton<IFileSystem>
{
public:

    //! destructor
    virtual ~IFileSystem() {};

    //! Opens a file for read access.
    //! \param filename: Name of file to open.
    //! \return Returns a pointer to the created file interface.
    //! The returned pointer should be dropped when no longer needed.
    //! See IUnknown::drop() for more information.
    virtual IReadFile* createAndOpenFile(const c8* filename) = 0;

    //! Opens a file for write access.
    //! \param filename: Name of file to open.
    //! \param append: If the file already exist, all write operations are 
    //! appended to the file.
    //! \return Returns a pointer to the created file interface. 0 is returned, if the
    //! file could not created or opened for writing.
    //! The returned pointer should be dropped when no longer needed.
    //! See IUnknown::drop() for more information.
    virtual IWriteFile* createAndWriteFile(const c8* filename, bool append=false) = 0;

	//! creating file to read from memory stream
	virtual IReadFile* createMemoryReadFile(
		void* memory, s32 size, const c8* fileName, bool deleteMemoryWhenDropped
		) = 0;

	virtual IReadFile* createLimitReadFile(
		const c8* fileName, IReadFile* alreadyOpenedFile, s32 areaSize
		) = 0;

	virtual IWriteFile* createMemoryWriteFile(const c8* fileName, core::stringc& out_buffer) = 0;
	virtual IWriteFile* createMemoryWriteFile(const c8* fileName, core::stringw& out_buffer) = 0;

    //! Adds an zip archive to the file system.
    //! \param filename: Filename of the zip archive to add to the file system.
    //! \param ignoreCase: If set to true, files in the archive can be accessed without
    //! writing all letters in the right case.
    //! \param ignorePaths: If set to true, files in the added archive can be accessed
    //! without its complete path.
    //! \return Returns true if the archive was added successful, false if not.
    virtual bool addZipFileArchive(const c8* filename, bool ignoreCase = true, bool ignorePaths = true) = 0;
    
    //! Returns the string of the current working directory.
	virtual const c8 * getWorkingDirectory() = 0;

    //! Changes the current Working Directory to the overgiven string.
    //! \param
    //! newDirectory is a string specifiing the new working directory.
    //! The string is operating system dependent. Under Windows it has
    //! the form "<drive>:\<directory>\<sudirectory>\<..>". An example would be: "C:\Windows\"
    //! \return Returns true if successful, otherwise false.
    virtual bool changeWorkingDirectoryTo(const c8* newDirectory) = 0;

    //! Returns a list of files and directories in the current working directory 
    //! and returns it.
    //! \return a Pointer to the IFileList is returned.
    virtual const IFileList* getFileList() = 0;

    //! determinates if a file exists and would be able to be opened.
    //! \param filename is the string identifying the file which should be tested for existance.
    //! \return Returns true if file exsits, and false if it does not exist or an error occured.
    virtual bool existFile(const c8* filename) = 0;
	virtual bool existFileInZip(const c8 *filename) = 0;
	virtual bool existFileOnDisk(const c8 *filename) = 0;

	//! determinates if a directory exists and would be able to be opened.
    //! \param dirname is the string identifying the dir which should be tested for existance.
    //! \return Returns true if dir exsits, and false if it does not exist or an error occured.
	virtual bool existDir(const c8* dirname) = 0;

	//! creates directory
    //! \param dirname is the string identifying the dir which should be created.
    //! \return Returns true if operation was succeded, and false if it does not or an error occured.
	virtual bool makeDir(const c8 * dirname) = 0;

    //! Creates a XML Reader from a file.
    //! \return 0, if file could not be opened, otherwise a pointer to the created
    //! IXMLReader is returned. After use, the reader
    //! has to be deleted using its IXMLReader::drop() method.
    //! See IUnknown::drop() for more information.
    virtual IXMLReader* createXMLReader(const c8* filename) = 0;

    //! Creates a XML Reader from a file.
    //! \return 0, if file could not be opened, otherwise a pointer to the created
    //! IXMLReader is returned. After use, the reader
    //! has to be deleted using its IXMLReader::drop() method.
    //! See IUnknown::drop() for more information.
    virtual IXMLReader* createXMLReader(IReadFile* file) = 0;

	//! Creates a Text Reader from a file.
	virtual ITextReader* createTextReader(const c8* filename) = 0;

	//! Creates a Text Reader from a file.
	virtual ITextReader* createTextReader(IReadFile* file) = 0;

    //! Creates a XML Writer from a file.
    //! \return 0, if file could not be opened, otherwise a pointer to the created
    //! IXMLWriter is returned. After use, the reader
    //! has to be deleted using its IXMLWriter::drop() method.
    //! See IUnknown::drop() for more information.
    virtual IXMLWriter* createXMLWriter(const c8* filename) = 0;

    //! Creates a XML Writer from a file.
    //! \return 0, if file could not be opened, otherwise a pointer to the created
    //! IXMLWriter is returned. After use, the reader
    //! has to be deleted using its IXMLWriter::drop() method.
    //! See IUnknown::drop() for more information.
    virtual IXMLWriter* createXMLWriter(IWriteFile* file) = 0;

	//! Optimizing the directory string
	//! 'c:\game\saves\..\media\textures' convert to 'c:\game\media\textures'
	virtual const c8* optimizeDirectoryString(const c8 *dir_to_optimize) = 0;

	//!	Return singleton object
	static IFileSystem& getSingleton();

	//! Return pointer to singleton object
	static IFileSystem* getSingletonPtr();
};

//----------------------------------------------------------------------------

__MY_IO_LIB_API__ IFileSystem* createFileSystem();

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

#endif

