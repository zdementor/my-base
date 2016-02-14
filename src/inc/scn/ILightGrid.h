//|-------------------------------------------------------------------------
//| File:      ILightGrid.h
//|
//| Descr:     This file is a part of the 'My Engine' v.0.1
//| Author(s): Zhuck 'zdementor' Dmitri (aka ZDimitor)
//| Email:     zdimitor@pochta.ru, sibergames@nm.ru
//|
//| Copyright (c) 2004-2009 by SiberGames, Krasnoyarsk - Moscow. All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ILightGridHPP
#define ILightGridHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/ISceneNode.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct SLightVolume
{
	u8 ambient[3];     // This is the ambient color
    u8 directional[3]; // This is the directional color in RGB
    u8 direction[2];   // The direction of the light: [phi,theta] 
}; 

//---------------------------------------------------------------------------

struct SLightGridData
{
	SLightGridData() : lightGridData(0)
	{
		lightGridSize[0] = 64;
		lightGridSize[1] = 64;
		lightGridSize[2] = 128;

		lightGridBounds[0] = 0;
		lightGridBounds[1] = 0;
		lightGridBounds[2] = 0;

		lightGridOrigin[0] = 0;
		lightGridOrigin[1] = 0;
		lightGridOrigin[2] = 0;
	}

	SLightVolume *lightGridData;

	f32 lightGridSize[3]; 
	s32 lightGridBounds[3];
	f32 lightGridOrigin[3];
};

//---------------------------------------------------------------------------

class __MY_SCN_LIB_API__ ILightGrid
{
public:

	 virtual vid::SLight getLightFromPos(core::vector3df pos) = 0;

	 virtual bool isPointInside(core::vector3df point) = 0;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

