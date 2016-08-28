//|-------------------------------------------------------------------------
//| File:        ISingleton.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ISingletonHPP
#define ISingletonHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <cassert>
#include <typeinfo>

//---------------------------------------------------------------------------
namespace my {
//---------------------------------------------------------------------------

//! Singleton template class
template <typename T> class ISingleton
{
	friend class CSingletonsTableInitializator; 

private:
	static T** ms_Singleton; // this pointer must be initialized

public:		
	ISingleton()
    {
#if MY_DEBUG_MODE
#if MY_COMPILER == MY_COMPILER_MSVC && !MY_COMP_ARCH_64
		if (!ms_Singleton || (*ms_Singleton))
			_asm int 3
#else
		if (!ms_Singleton || (*ms_Singleton))
			assert(!"!ms_Singleton || (*ms_Singleton)");
#endif
#endif
		*(ms_Singleton) = static_cast <T*>(this);
    }
	virtual ~ISingleton()
    {  	
#if MY_DEBUG_MODE
#if MY_COMPILER == MY_COMPILER_MSVC && !MY_COMP_ARCH_64
		if (!ms_Singleton || !(*ms_Singleton))
			_asm int 3
#else
		if (!ms_Singleton || !(*ms_Singleton))
			assert(!"!ms_Singleton || !(*ms_Singleton)");
#endif
#endif
		*(ms_Singleton) = 0; 
	}
    static T& getSingleton()
    {  
#if MY_DEBUG_MODE
#if MY_COMPILER == MY_COMPILER_MSVC && !MY_COMP_ARCH_64
		if (!ms_Singleton || !(*ms_Singleton))
			_asm int 3
#else
		if (!ms_Singleton || !(*ms_Singleton))
			assert(!"!ms_Singleton || !(*ms_Singleton)");
#endif
#endif
		return (**ms_Singleton);
	}
    static T* getSingletonPtr()
    {  
#if MY_DEBUG_MODE
#if MY_COMPILER == MY_COMPILER_MSVC && !MY_COMP_ARCH_64
		if (!ms_Singleton)
			_asm int 3
#else
		if (!ms_Singleton)
			assert(!"!ms_Singleton");
#endif
#endif
		return (*ms_Singleton);
	}
};

//---------------------------------------------------------------------------
} // end namespace my
//---------------------------------------------------------------------------

#endif	// #ifndef ISingletonHPP
