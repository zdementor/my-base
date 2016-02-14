//|-------------------------------------------------------------------------
//| File:        CDefaultMeshFormatLoader.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CDefaultMeshFormatLoaderHPP
#define CDefaultMeshFormatLoaderHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/IMeshLoader.h>
#include <io/IFileSystem.h>
#include <vid/IVideoDriver.h>    

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! Meshloader capable of loading all Irrlicht default build in formats.
//! Which are: Quake 3 Bsp level, Quake 2 MD2 model, Maya .obj mesh, 
//! Milkshape .ms3d model. 
class CDefaultMeshFormatLoader : public IMeshLoader
{
public:

    //! Constructor
    CDefaultMeshFormatLoader();

    //! destructor
    virtual ~CDefaultMeshFormatLoader();

    //! returns true if the file maybe is able to be loaded by this class
    //! based on the file extension (e.g. ".bsp")
    virtual bool isALoadableFileExtension(const c8* fileName);

    //! creates/loads an animated mesh from the file.
    //! \param filename: Filename of the mesh to load.
    //! \param  detailLevel: detail level of creating mesh
	//! \param  ChildNodes: array of nodes creating while loading mesh
    //! \return Pointer to the created mesh. Returns 0 if loading failed.
    //! If you no longer need the mesh, you should call IAnimatedMesh::drop().
    //! See IUnknown::drop() for more information.
    virtual IAnimatedMesh* createMesh(
		io::IReadFile* file, E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST);
private:

};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif

