//|-------------------------------------------------------------------------
//| File:        IMaterialsManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IMaterialsManagerHPP
#define IMaterialsManagerHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include "ISingleton.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class __MY_SCN_LIB_API__ IMaterialsManager : 
	public IUnknown, public ISingleton<IMaterialsManager>
{
public:

	//! Loading all materials from directory,
	//! (looking for *.shader and *.material files)
	virtual void loadMaterialsFromDir(
		const c8 * dir, bool load_textures = true, bool reload_if_exists = true) = 0;

	virtual bool loadMaterialFromFile(const c8 *filename, vid::SMaterial &out_material) = 0;
	virtual bool saveMaterialToFile(const c8 *filename, const vid::SMaterial &material) = 0;

	//! Returns material and vertex type, by name and light map texture
	virtual bool getMaterialAndVertexType(
		const c8 * material_name, vid::ITexture * lmap, 
		vid::SMaterial & out_material, vid::E_VERTEX_TYPE & out_vtype) = 0;

	//! Returns overal materials count, managed by materials manager
	virtual u32 getMaterialsCount() const = 0;

	//! Returns material index,
	//! On success return value >= 0, otherwise -1
	virtual s32 findMaterialByName(const c8 *name) const = 0;
	virtual s32 findMaterialByHash(u64 hash) const = 0;

	//! Add material to library
	//! On success return value >= 0, otherwise -1
	virtual s32 addMaterial(const vid::SMaterial &m, const c8 *name) = 0;

	//! Returns material
	virtual const vid::SMaterial& getMaterial(u32 idx) const = 0;
	virtual const vid::SMaterial& getMaterialByName(const c8 *name) const = 0;

	//! Updates material, return true on success, otherwise - false
	virtual bool setMaterial(u32 idx, const vid::SMaterial &other) = 0;
	virtual bool setMaterialByName(const c8 *name, const vid::SMaterial &other) = 0;

	//! Returns material name, by its index
	virtual const c8* getMaterialName(u32 idx) const = 0;

	//! Fixup material
	virtual void correctMaterial(vid::SMaterial &material) = 0;

	//!	Return singleton object
	static IMaterialsManager& getSingleton();

	//! Return pointer to singleton object
	static IMaterialsManager* getSingletonPtr();
};

//---------------------------------------------------------------------------

__MY_SCN_LIB_API__ vid::ITexture* getNormalMapTexture(const c8 *texname);

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

