//|-------------------------------------------------------------------------
//| File:        ILightGridManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ILightGridManagerHPP
#define ILightGridManagerHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <ISingleton.h>
#include <vid/SLight.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class __MY_SCN_LIB_API__ ILightGridManager : 
	public IUnknown, public ISingleton<ILightGridManager>
{
public:

	virtual vid::SLight getLightFromPos(core::vector3df pos) = 0;

	virtual void addLightGrid(ILightGrid* lgrid) = 0;

	virtual void removeLightGrid(ILightGrid* lgrid) = 0;

	//!	Return singleton object
	static ILightGridManager& getSingleton();

	//! Return pointer to singleton object
	static ILightGridManager* getSingletonPtr();    
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

