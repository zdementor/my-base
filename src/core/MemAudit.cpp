//|-------------------------------------------------------------------------
//| File:        MemAudit.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "../inc/MemAudit.h"
#include <IUnknown.h>

#if MY_DEBUG_MODE  
	#define ENABLE_OBJECT_COUNTING
#endif

//----------------------------------------------------------------------------
namespace my {
//----------------------------------------------------------------------------

static core::array <void*> UnknownObjects; 

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ void printMemAudit(c8* op, IUnknown *obj) 
{   
#ifdef ENABLE_OBJECT_COUNTING

    FILE *memAudit=NULL;
    c8* fName = "MemAudit.txt";

    static bool first = true;
    if (first)
        memAudit=fopen(fName,"w");
    else
        memAudit=fopen(fName,"a+");
    first=false;

    if (memAudit) 
    {
        IUnknown *obj_ptr = reinterpret_cast<IUnknown *>(obj);
        const c8 *obj_name = NULL;
        
    #if MY_DEBUG_MODE  
        if (obj_ptr)
            obj_name = obj_ptr->getClassName();
        else
            obj_name = NULL;
    #endif
        
        fprintf(memAudit,"%s %p %s\n",op, obj_ptr, obj_name); 
        fclose(memAudit); 
    }

#endif
}

//----------------------------------------------------------------------------

//! find IUnknown-object in list or return NULL
__MY_CORE_LIB_API__ s32 findUnknownObjectInList(void *obj)
{   
#ifdef ENABLE_OBJECT_COUNTING

    s32 size_1 = (s32)UnknownObjects.size()-1;

    for (s32 i=size_1; i>=0; i--)
    {
        if ( obj==UnknownObjects[i] ) 
            return i;     
    }

#endif
        
    return -1;
}

//----------------------------------------------------------------------------

//! add IUnknown-object to list
__MY_CORE_LIB_API__ void addUnknownObjectToList(void *obj)
{ 
#ifdef ENABLE_OBJECT_COUNTING

    if (UnknownObjects.size()+1>UnknownObjects.allocated_size())
    {
        UnknownObjects.reallocate(UnknownObjects.size()+1000);      
    }

    UnknownObjects.push_back(obj);

#endif
} 

//----------------------------------------------------------------------------

//! remove IUnknown-object from list
__MY_CORE_LIB_API__ bool remUnknownObjectFromList(void *obj)
{ 
#ifdef ENABLE_OBJECT_COUNTING

    s32 index = findUnknownObjectInList(obj);

    if (index!=-1) 
    {       
        UnknownObjects.erase(index); 

        return true; 
    }

#endif

    return false;
} 

//----------------------------------------------------------------------------

//! printing names of all allocated IUnknown-objects
__MY_CORE_LIB_API__ void printAllocNamesList(c8* fName) 
{  
#ifdef ENABLE_OBJECT_COUNTING

    FILE *alloc=NULL;
   
    alloc=fopen(fName,"w");

    if (alloc) 
    {   
        IUnknown *obj_ptr  = NULL;
        const c8 *obj_name = NULL;
        s32 obj_ref_num = -1;

        for (s32 i=0; i<(s32)UnknownObjects.size(); i++)
        {       
            obj_ptr  = reinterpret_cast<IUnknown *>(UnknownObjects[i]);         

            if (obj_ptr)
            {
            #if MY_DEBUG_MODE  
                obj_name    = obj_ptr->getClassName();
            #endif
                obj_ref_num = obj_ptr->getReferenceCounter();
            }
            else
                obj_name = NULL;

            fprintf( alloc,"%p %s (%d)\n", obj_ptr, obj_name, obj_ref_num ); 
        }   
        
        fclose(alloc); 
    }

#endif
}

//----------------------------------------------------------------------------

//! printing pointerss of all allocated IUnknown-objects
__MY_CORE_LIB_API__ void printAllocPtrsList(c8* fName) 
{   
#ifdef ENABLE_OBJECT_COUNTING

    FILE *alloc=NULL;
   
    alloc=fopen(fName,"w");

    if (alloc) 
    {   
        IUnknown *obj_ptr  = NULL;
        const c8 *obj_name = NULL;

        for (s32 i=0; i<(s32)UnknownObjects.size(); i++)
        {                  
            obj_ptr  = reinterpret_cast<IUnknown *>(UnknownObjects[i]);
            fprintf(alloc,"%p\n", obj_ptr); 
        }        
        fclose(alloc); 
    }

#endif
}

//----------------------------------------------------------------------------

//! number of IUnknown-objects
__MY_CORE_LIB_API__ s32 getUnknownObjectsNumber()
{   
#ifdef ENABLE_OBJECT_COUNTING
    return UnknownObjects.size();
#endif
	return 0;
}

//----------------------------------------------------------------------------
} //end namespace my
//----------------------------------------------------------------------------