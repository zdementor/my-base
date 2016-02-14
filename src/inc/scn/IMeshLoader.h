//|-------------------------------------------------------------------------
//| File:        IMeshLoader.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IMeshLoaderHPP
#define IMeshLoaderHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <io/IReadFile.h>
#include <scn/IAnimatedMesh.h>
#include <scn/ISceneNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! Class which is able to load an animated mesh from a file.
class __MY_SCN_LIB_API__ IMeshLoader : public IUnknown
{
public:

    //! destructor
    virtual ~IMeshLoader() {};

    //! returns true if the file maybe is able to be loaded by this class
    //! based on the file extension (e.g. ".bsp")
    virtual bool isALoadableFileExtension(const c8* fileName) = 0;

    //! creates/loads an animated mesh from the file.
    //! \param file: File of the mesh to load.
    //! \param lodlevel: detail level of creating mesh
    //! \return Pointer to the created mesh. Returns 0 if loading failed.
    //! If you no longer need the mesh, you should call IAnimatedMesh::drop().
    //! See IUnknown::drop() for more information.
    virtual IAnimatedMesh* createMesh(
		io::IReadFile* file, E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST) = 0;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IMeshLoaderHPP

