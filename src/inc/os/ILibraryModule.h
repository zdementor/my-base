//|-------------------------------------------------------------------------
//| File:        ILibraryModule.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ILibraryModuleHPP
#define ILibraryModuleHPP
//---------------------------------------------------------------------------

#include <IUnknown.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

//! Interface for working with Library Module class
class __MY_OS_LIB_API__ ILibraryModule : public IUnknown
{
public:	

	//! load dynamic library, attached to this object
	virtual bool load() = 0;

	//! unload dynamic library, attached to this object
	virtual bool unload() = 0;

	//! return address from dynamic labrary module
	virtual void* getProcAddress(c8 *proc_name) = 0;	
};

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef ILibraryModuleHPP