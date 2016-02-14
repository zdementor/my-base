//|-------------------------------------------------------------------------
//| File:        IRunnable.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IRunnableHPP
#define IRunnableHPP
//---------------------------------------------------------------------------

#include <IUnknown.h>

//---------------------------------------------------------------------------
namespace my {
//---------------------------------------------------------------------------

class __MY_SCN_LIB_API__ IRunnable : public IUnknown
{
public:

    //! destructor
    virtual ~IRunnable() {}

	//! callback function
	virtual void run(void* user_data=NULL) = 0;

};

//---------------------------------------------------------------------------
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef IRunnableHPP