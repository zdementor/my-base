//|-------------------------------------------------------------------------
//| File:        IWriteFile.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IWriteFileHPP
#define IWriteFileHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

//! Interface providing write acess to a file.
class __MY_IO_LIB_API__ IWriteFile : public IUnknown
{	
public:

	virtual ~IWriteFile() {};

	//! Reads an amount of bytes from the file.
	//! \param buffer: Pointer to buffer of bytes to write.
	//! \param sizeToWrite: Amount of bytes to wrtie to the file.
	//! \return Returns how much bytes were written.
	virtual u32 write(const void* buffer, u32 sizeToWrite, bool flush = false) = 0;

	//! Changes position in file, returns true if successful.
	//! \param finalPos: Destination position in the file.
	//! \param relativeMovement: If set to true, the position in the file is
	//! changed relative to current position. Otherwise the position is changed
	//! from begin of file.
	//! \return Returns true if successful, otherwise false.
	virtual bool seek(s32 finalPos, bool relativeMovement = false) = 0;

	//! Returns the current position in the file.
	//! \return Returns the current position in the file in bytes.
	virtual s32 getPos() = 0;

	//! Returns name of file.
	//! \return Returns the file name as zero terminated character string.
	virtual const c8* getFileName() = 0;

	//! Returns file internal handle
	virtual const void* getHandle() = 0;
};

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

#endif

