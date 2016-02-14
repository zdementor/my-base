//|-------------------------------------------------------------------------
//| File:        MemAudit.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef MemAuditHPP
#define MemAuditHPP
//----------------------------------------------------------------------------

#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
//----------------------------------------------------------------------------

//! add IUnknown-object to list
__MY_CORE_LIB_API__ void addUnknownObjectToList(void* obj);

//----------------------------------------------------------------------------

//! remove IUnknown-object from list
__MY_CORE_LIB_API__ bool remUnknownObjectFromList(void* obj);  

//----------------------------------------------------------------------------

//! printing names of all allocated IUnknown-objects
__MY_CORE_LIB_API__ void printAllocNamesList(c8* fName); 

//----------------------------------------------------------------------------

//! printing pointerss of all allocated IUnknown-objects
__MY_CORE_LIB_API__ void printAllocPtrsList(c8* fName); 

//----------------------------------------------------------------------------

//! number of IUnknown-objects
__MY_CORE_LIB_API__ s32 getUnknownObjectsNumber();

//----------------------------------------------------------------------------
} //end namespace my
//----------------------------------------------------------------------------

#endif // MemAuditHPP


