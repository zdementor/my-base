//|-------------------------------------------------------------------------
//| File:        IFileList.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IFileListHPP
#define IFileListHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

//! The Filelist lists all files in a directory.
class __MY_IO_LIB_API__ IFileList : public IUnknown
{
public:

    //! Returns the amount of files in the filelist.
    //! \return
    //! Returns the amount of files and directories in the file list.
    virtual s32 getFileCount() const = 0;

	//! Gets the externsion of a file in the list, based on an index.
	virtual const c8* getFileExt(s32 index) const = 0;

    //! Gets the name of a file in the list, based on an index.
    //! The path is not included in this name. Use getFullFileName for this.
    //! \param index is the zero based index of the file which name should
    //!   be returned. The index has to be smaller than the amount getFileCount() returns.
    //! \return
    //! Returns the file name of the file. Returns 0, if an error occured.
	virtual const c8* getFileName(s32 index) const = 0;

    //! Gets the full name of a file in the list, path included, based on an index.
    //! \param index is the zero based index of the file which name should
    //!   be returned. The index has to be smaller than the amount getFileCount() returns.
    //! \return
    //! Returns the file name of the file. Returns 0, if an error occured.
	virtual const c8* getFullFileName(s32 index) const = 0;

    //! Returns of the file is a directory
    //! \param
    //! index is the zero based index of the file which name should
    //!   be returned. The index has to be smaller than the amount getFileCount() returns.
    //! \return
    //! Returns true, if the file is a directory, and false, if it is not.
    //!  If an error occurs, the result is undefined.
    virtual bool isDirectory(s32 index) const = 0;

	//! return file size in bytes
	virtual s32 getFileSizeInBytes(s32 index) const = 0;

	//! return current active path
	virtual const c8* getWorkingDirectory() const = 0;

	//! return true id file list need to be updates
	virtual bool isNeedToUpdate() const = 0;

	//! update directories list for current working directory
	virtual void update() const = 0;
};

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

#endif

