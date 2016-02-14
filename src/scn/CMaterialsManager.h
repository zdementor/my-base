//|-------------------------------------------------------------------------
//| File:        CMaterialsManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CMaterialsManagerHPP
#define CMaterialsManagerHPP
//---------------------------------------------------------------------------

#include <scn/IMaterialsManager.h>
#include <vid/SMaterial.h>
#include <vid/S3DVertex.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct SMeshBufferMaterial
{
	vid::SMaterial      Material;
	vid::E_VERTEX_TYPE  VertexType;
	core::stringc		Name;

	SMeshBufferMaterial()
		: VertexType((vid::E_VERTEX_TYPE)-1), Name("") {}
	SMeshBufferMaterial(const vid::SMaterial &m,
		vid::E_VERTEX_TYPE vtype, const c8 *name)
		: Material(m), VertexType(vtype), Name(name) {}

	bool operator < (const SMeshBufferMaterial& other) const
    { return Name < other.Name; }
};

//---------------------------------------------------------------------------

struct SMeshBufferMaterialHashEntry
{
	SMeshBufferMaterial *Material;
	s32 Index;

	SMeshBufferMaterialHashEntry() : Material(NULL), Index(-1) {}
	SMeshBufferMaterialHashEntry(SMeshBufferMaterial *m, u32 idx)
		: Material(m), Index(idx) {}

	bool operator == (u64 hash) const
	{ return Material->Material.getHash() == hash; }
};

//---------------------------------------------------------------------------

//! Meshloader capable of loading 3ds meshes.
class CMaterialsManager : public IMaterialsManager
{
public:

    CMaterialsManager();
    virtual ~CMaterialsManager();  

	virtual void loadMaterialsFromDir(const c8 * dir, bool load_textures, bool reload_if_exists);

	virtual bool loadMaterialFromFile(const c8 *filename, vid::SMaterial &out_material);
	virtual bool saveMaterialToFile(const c8 *filename, const vid::SMaterial &material);

	virtual bool getMaterialAndVertexType(
		const c8 * material_name, vid::ITexture * lmap, 
		vid::SMaterial & out_material, vid::E_VERTEX_TYPE & out_vtype);

	virtual u32 getMaterialsCount() const;

	virtual s32 findMaterialByName(const c8 *name) const;
	virtual s32 findMaterialByHash(u64 hash) const;

	virtual s32 addMaterial(const vid::SMaterial &m, const c8 *name);

	virtual const vid::SMaterial& getMaterial(u32 idx) const;
	virtual const vid::SMaterial& getMaterialByName(const c8 *name) const;

	virtual bool setMaterial(u32 idx, const vid::SMaterial &other);
	virtual bool setMaterialByName(const c8 *name, const vid::SMaterial &other);

	virtual const c8* getMaterialName(u32 idx) const;

	virtual void correctMaterial(vid::SMaterial &material);

private:

	// updating materials hash
	void _updateMaterialsHash();

	s32 _findMaterial(const c8* material_name) const;

	mutable core::array <SMeshBufferMaterial> Materials;

	mutable core::hash_array <SMeshBufferMaterialHashEntry> m_MaterialsHash;

	vid::SMaterial m_StubMaterial;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

