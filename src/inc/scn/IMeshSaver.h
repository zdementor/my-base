//|-------------------------------------------------------------------------
//| File:        IMeshSaver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IMeshSaverHPP
#define IMeshSaverHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <io/IReadFile.h>
#include <scn/IAnimatedMesh.h>
#include <scn/ISceneNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! Class which is able to save an animated mesh in file.
class __MY_SCN_LIB_API__ IMeshSaver : public IUnknown
{
public:

    //! destructor
    virtual ~IMeshSaver() {};

    //! returns true if the file maybe is able to be loaded by this class
    //! based on the file extension (e.g. ".bsp")
    virtual bool isALoadableFileExtension(const c8* fileName) = 0;

    //! saves animated mesh to the file.
    //! \param amesh: Animated mesh, wich we wanted to save
	//! \param filename: Filename of the mesh to load.
	//! \param meshtype: Type of mesh we wanted save to
    virtual void saveMesh(
		IAnimatedMesh* amesh,
		c8 *filename, EANIMATED_MESH_TYPE meshtype		
		) = 0;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IMeshSaverHPP
 
