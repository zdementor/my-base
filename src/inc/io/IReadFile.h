//|-------------------------------------------------------------------------
//| File:        IReadFile.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IReadFileHPP
#define IReadFileHPP
//-----------------------------------------------------------------------------

#include <IUnknown.h>

//-----------------------------------------------------------------------------
namespace my {
namespace io {
//-----------------------------------------------------------------------------

//! Interface providing read acess to a file.
class __MY_IO_LIB_API__ IReadFile : public IUnknown
{
public:

    virtual ~IReadFile() {};

	//! Reads an amount of bytes from the file.
	//! \param buffer: Pointer to buffer where to read bytes will be written to.
	//! \param sizeToRead: Amount of bytes to read from the file.
	//! \return Returns how much bytes were read.
	virtual s32 read(void* buffer, s32 sizeToRead)  = 0;

    //! читаем строчку из файла (пока не встретим 0)
    //! returns how much was read
    virtual s32 readString(void* buffer, s32 maxBuffSize) {return 0;}

	//! Changes position in file, returns true if successful.
	//! \param finalPos: Destination position in the file.
	//! \param relativeMovement: If set to true, the position in the file is
	//! changed relative to current position. Otherwise the position is changed
	//! from begin of file.
	//! \return Returns true if successful, otherwise false.
	virtual bool seek(s32 finalPos, bool relativeMovement = false)  = 0;

	//! Returns size of file.
	//! \return Returns the size of the file in bytes.
	virtual s32 getSize()  = 0;

	//! Returns the current position in the file.
	//! \return Returns the current position in the file in bytes.
	virtual s32 getPos()  = 0;

	//! Returns name of file.
	//! \return Returns the file name as zero terminated character string.
	virtual const c8* getFileName()  = 0; 

	//! Returns file path.
	//! \return Returns the file path as zero terminated character string.
	virtual const c8* getFilePath()  = 0; 

	//! Returns file internal handle
	virtual const void* getHandle() = 0;
};

//-----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//-----------------------------------------------------------------------------

#endif

