//|-------------------------------------------------------------------------
//| File:        IBSPLevelMesh.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IBSPLevelMeshHPP
#define IBSPLevelMeshHPP
//---------------------------------------------------------------------------

#include <scn/IAnimatedMesh.h>   
#include "IBSPEntity.h"
#include <scn/mesh_buffer.h>
#include <vid/S3DVertex.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Interface for a Mesh wich can be loaded directly from a Quake3 .bsp-file.
//! The Mesh tries to load all textures of the map. 
class __MY_SCN_LIB_API__ IBSPLevelMesh : public IAnimatedMesh
{
public:

    //! destructor
    virtual ~IBSPLevelMesh() {}

	//! return Entities count	
	virtual s32 getEntitiesCount(E_BSP_ENTITY_TYPE type) = 0;

	//! return Entity by its type and index
	virtual IBSPEntity& getEntity(E_BSP_ENTITY_TYPE type, s32 idx) = 0;

    //! loads a level from a .bsp-File. 
	//! Also tries to load all needed textures and all other .bsp data. 
	//! Returns true if successful.
    //! \param file: File of the mesh to load.
    virtual bool loadFromFile(io::IReadFile* file) = 0;

	virtual void freeUnusedData() = 0;

    //! saving a level to a .bsp-File
    virtual bool saveToFile(const c8* filename) = 0;

	virtual vid::SLight getLightFromPos(core::vector3df _pos) = 0;

	virtual bool isLightGridOK() = 0;

	virtual void removePatches() = 0;

	virtual void collapseVertices() = 0;

	virtual void generateMaxImportScript(const c8* filename, f32 light_intens_mul, f32 light_radius_mul) = 0;

	virtual void rewriteLightMapsFrom(const c8* lightmaps_path) = 0;	
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

