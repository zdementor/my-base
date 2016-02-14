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
		if (!ms_Singleton || (*ms_Singleton))
			_asm int 3
#endif
		*(ms_Singleton) = static_cast <T*>(this);
    }
	virtual ~ISingleton()
    {  	
#if MY_DEBUG_MODE
		if (!ms_Singleton || !(*ms_Singleton))
			_asm int 3
#endif
		*(ms_Singleton) = 0; 
	}
    static T& getSingleton()
    {  
#if MY_DEBUG_MODE
		if (!ms_Singleton || !(*ms_Singleton))
			_asm int 3
#endif
		return (**ms_Singleton);
	}
    static T* getSingletonPtr()
    {  
#if MY_DEBUG_MODE
		if (!ms_Singleton)
			_asm int 3
#endif
		return (*ms_Singleton);
	}
};

//---------------------------------------------------------------------------
} // end namespace my
//---------------------------------------------------------------------------

#endif	// #ifndef ISingletonHPP
