//|-------------------------------------------------------------------------
//| File:        CLibraryModule.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CLibraryModuleHPP
#define CLibraryModuleHPP
//---------------------------------------------------------------------------

#include <os/ILibraryModule.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

//! Interface for working with Event class
class __MY_SCN_LIB_API__ CLibraryModule : public ILibraryModule
{
public:	

	//! constructor
	CLibraryModule(c8 *module_name);

	//! destructor
	virtual ~CLibraryModule();

	//! load dynamic library, attached to this object
	virtual bool load();

	//! unload dynamic library, attached to this object
	virtual bool unload();

	//! return address from dynamic labrary module
	virtual void* getProcAddress(c8 *proc_name);

private:

	HINSTANCE m_LibraryInstance;
	core::stringc m_LibraryName;

	bool m_Loaded;

};

//---------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef CLibraryModuleHPP