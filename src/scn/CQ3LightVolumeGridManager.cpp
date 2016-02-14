//|-------------------------------------------------------------------------
//| File:        CQ3LightVolumeGridManager.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CQ3LightVolumeGridManager.h"

#include <scn/ISceneNode.h>
#include <scn/ISceneManager.h>
#include <scn/ILightGrid.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Constructor
CQ3LightVolumeGridManager::CQ3LightVolumeGridManager()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CQ3LightVolumeGridManager");
#endif
}

//---------------------------------------------------------------------------

//! destructor
CQ3LightVolumeGridManager::~CQ3LightVolumeGridManager()
{
}

//---------------------------------------------------------------------------

vid::SLight CQ3LightVolumeGridManager::getLightFromPos(core::vector3df pos)
{
	vid::SLight light;
	light.AmbientColor.set(0,0,0);
	light.DiffuseColor.set(0,0,0);
	light.SpecularColor.set(0,0,0);
	light.Type = vid::ELT_DIRECTIONAL;
	light.Enabled = false;

	s32 lgsize = LightGrids.size();
	for (s32 lg = 0; lg < lgsize; lg++)
	{
		if ( LightGrids[lg]->isPointInside(pos) )
		{
			light = LightGrids[lg]->getLightFromPos(pos);			
			break;
		}
	}	

	return light;
}

//---------------------------------------------------------------------------

ILightGridManager* createQ3LightVolumeGridManager()
{
	return new CQ3LightVolumeGridManager();
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
