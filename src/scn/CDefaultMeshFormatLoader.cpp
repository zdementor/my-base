//|-------------------------------------------------------------------------
//| File:        CDefaultMeshFormatLoader.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CDefaultMeshFormatLoader.h"

#include "CAnimatedMeshMD2.h"
#include "CAnimatedMeshCal3d.h"
#include "CQ3LevelMesh.h"
#include "C3DSMesh.h"

#include <scn/ISceneManager.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! Constructor
CDefaultMeshFormatLoader::CDefaultMeshFormatLoader()
{
#if MY_DEBUG_MODE 
	setClassName("CDefaultMeshFormatLoader");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CDefaultMeshFormatLoader::~CDefaultMeshFormatLoader()
{
}

//----------------------------------------------------------------------------

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool CDefaultMeshFormatLoader::isALoadableFileExtension(const c8* filename)
{
    return (
		strstr(filename, ".md2") || strstr(filename, ".MD2") ||
		strstr(filename, ".bsp") || strstr(filename, ".BSP") ||
		strstr(filename, ".cfg") || strstr(filename, ".CFG") ||
		strstr(filename, ".3ds") || strstr(filename, ".3DS"));
}

//----------------------------------------------------------------------------

//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
IAnimatedMesh* CDefaultMeshFormatLoader::createMesh(
	io::IReadFile* file, E_LOD_LEVEL lodlevel)
{
    IAnimatedMesh* msh = 0;

    // This method loads a mesh if it cans.
    // Someday I will have to refactor this, and split the DefaultMeshFormatloader
    // into one loader for every format.

    bool success = false;

    // load quake 2 md2 model
    if (strstr(file->getFileName(), ".md2") || strstr(file->getFileName(), ".MD2"))
    {
        file->seek(0);
            msh = new CAnimatedMeshMD2();
        success = ((CAnimatedMeshMD2*)msh)->loadFromFile(file);
        if (success)
            return msh;
        msh->drop();  
    } 

    // load quake 3 bsp
    if (strstr(file->getFileName(), ".bsp") || strstr(file->getFileName(), ".BSP"))
    {
        file->seek(0);
        msh = new CQ3LevelMesh();
        success = ((CQ3LevelMesh*)msh)->loadFromFile(file);
        if (success)
            return msh;
        msh->drop();
    }

	// load cal3d mesh
    if (strstr(file->getFileName(), ".cfg") || strstr(file->getFileName(), ".CFG"))
    {
		file->seek(0);
    	IAnimatedMesh* msh = new CAnimatedMeshCal3d();
		success = ((CAnimatedMeshCal3d*)msh)->loadFromFile(file);
   		if (success)
			return msh;
		msh->drop();  
    }

	// load 3ds mesh
    if (strstr(file->getFileName(), ".3ds") || strstr(file->getFileName(), ".3DS"))
    {
		file->seek(0);
		IAnimatedMesh* msh = new C3DSMesh();
		success = ((C3DSMesh*)msh)->loadFromFile(file);
    	if (success)
			return msh;
		msh->drop();  
    }

    return NULL;
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------


