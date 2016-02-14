//|-------------------------------------------------------------------------
//| File:        SMaterial.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <vid/SMaterial.h>
#include <scn/IMaterialsManager.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

bool SMaterial::loadFromFile(const c8 *filename)
{
	return MATERIALS_MANAGER.loadMaterialFromFile(filename, *this);
}

//---------------------------------------------------------------------------

bool SMaterial::saveToFile(const c8 *filename) const
{
	return MATERIALS_MANAGER.saveMaterialToFile(filename, *this);
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------


