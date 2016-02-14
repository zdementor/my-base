//|-------------------------------------------------------------------------
//| File:        CFileList.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CFileListHPP
#define CFileListHPP
//-------------------------------------------------------------------------

#include <my_inc.h>
#include <io/IFileList.h>

//-------------------------------------------------------------------------
namespace my {
namespace io {
//-------------------------------------------------------------------------

class CFileList : public IFileList
{
public:

    CFileList();
    virtual ~CFileList();

    virtual s32 getFileCount() const;

    virtual const c8* getFileExt(s32 index) const;

    virtual const c8* getFileName(s32 index) const;

    virtual const c8* getFullFileName(s32 index) const;

    virtual bool isDirectory(s32 index) const;

	virtual s32 getFileSizeInBytes(s32 index) const;

	virtual const c8* getWorkingDirectory() const;

	virtual bool isNeedToUpdate() const
	{ return m_NeedToUpdate; }

	virtual void update() const;

	void onChangeWorkingDirectory();

private:

    struct FileEntry
    {
		core::stringc Ext;
        core::stringc Name;
        core::stringc FullName;
        s32 Size;
        bool isDirectory;

		bool operator < (const FileEntry &other)
		{
			if (isDirectory != other.isDirectory)
				return isDirectory && !other.isDirectory;
			return Name < other.Name;
		}
    };

    mutable core::stringc m_Path;
    mutable core::array <FileEntry> m_Files;
	core::stringc m_NewWorkingDir;
	mutable bool m_NeedToUpdate;
};

//-------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//-------------------------------------------------------------------------

#endif

