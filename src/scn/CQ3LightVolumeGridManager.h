//|-------------------------------------------------------------------------
//| File:        CQ3LightVolumeGridManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CQ3LightVolumeGridManagerHPP
#define CQ3LightVolumeGridManagerHPP
//---------------------------------------------------------------------------

#include <scn/ILightGridManager.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Meshloader capable of loading 3ds meshes.
class CQ3LightVolumeGridManager : public ILightGridManager
{

public:

    //! Constructor
    CQ3LightVolumeGridManager();

    //! destructor
    virtual ~CQ3LightVolumeGridManager();  

	virtual vid::SLight getLightFromPos(core::vector3df pos);

	virtual void addLightGrid(ILightGrid* lgrid)
	{
		if (lgrid)
		{
			LightGrids.push_back(lgrid);
		}
	}

	virtual void removeLightGrid(ILightGrid* lgrid)
	{
		for (u32 lg=0; lg<LightGrids.size(); lg++)
		{
			if (LightGrids[lg] == lgrid)
			{
				LightGrids.erase(lg);
				break;
			}
		}
	}	

private:	

	core::array<ILightGrid*> LightGrids;	     
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

