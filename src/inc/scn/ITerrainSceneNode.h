//|-------------------------------------------------------------------------
//| File:        ITerrainSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ITerrainSceneNodeHPP
#define ITerrainSceneNodeHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/ISceneNode.h>
#include <img/IImage.h>
#include <vid/ITexture.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct STileSetParams
{
	STileSetParams()
		: SplatMapFileName(NONAME_FILE_NAME),
		Tile0FileName(NONAME_FILE_NAME), Tile1FileName(NONAME_FILE_NAME), Tile2FileName(NONAME_FILE_NAME)
	{}

	const c8* getSplatMapFileName() const
	{ return SplatMapFileName.c_str(); }

	void setSplatMapFileName(const c8* name)
	{ SplatMapFileName = name; }

	const c8* getTile0FileName() const
	{ return Tile0FileName.c_str(); }

	void setTile0FileName(const c8* name)
	{ Tile0FileName = name; }

	const c8* getTile1FileName() const
	{ return Tile1FileName.c_str(); }

	void setTile1FileName(const c8* name)
	{ Tile1FileName = name; }

	const c8* getTile2FileName() const
	{ return Tile2FileName.c_str(); }

	void setTile2FileName(const c8* name)
	{ Tile2FileName = name; }
private:
	core::stringc SplatMapFileName;
	core::stringc Tile0FileName, Tile1FileName, Tile2FileName;
};

//---------------------------------------------------------------------------

struct STileSet
{
	STileSet()
		: SplatMapFileName(NONAME_FILE_NAME)
	{
		Tile0Texture = Tile1Texture = Tile2Texture = NULL;
	}

	const c8* getSplatMapFileName() const
	{ return SplatMapFileName.c_str(); }

	void setSplatMapFileName(const c8* name)
	{ SplatMapFileName = name; }

	vid::ITexture* getTile0Texture() const
	{ return Tile0Texture; }

	void setTile0Texture(vid::ITexture *tex)
	{ Tile0Texture = tex; }

	vid::ITexture* getTile1Texture() const
	{ return Tile1Texture; }

	void setTile1Texture(vid::ITexture *tex)
	{ Tile1Texture = tex; }

	vid::ITexture* getTile2Texture() const
	{ return Tile2Texture; }

	void setTile2Texture(vid::ITexture *tex)
	{ Tile2Texture = tex; }

private:
	core::stringc SplatMapFileName;
	vid::ITexture *Tile0Texture, *Tile1Texture, *Tile2Texture;
};

//---------------------------------------------------------------------------

//! A terrain scene node using simple geomipmapping.
class __MY_SCN_LIB_API__ ITerrainSceneNode : public ISceneNode
{
public:

    //! constructor
    ITerrainSceneNode(E_SCENE_NODE_TYPE type, ISceneNode* parent, s32 id)
		: ISceneNode(type, parent, id) {}

    //! create the terrain 
    //! \param heightmap, image with height values 
    //! \param gridPointSpacing, spacing between the gridpoints of the heightfield 
	virtual bool setMaps(const c8* hmap_filename, const c8* fogmap_filename,
		f32 gridPointSpacing) = 0;

	//! return file name of the height map image
    virtual const c8* getHeightMapFileName() = 0;

	//! returns geometry size beetween terrain greed cells
	virtual f32 getGridPointSpacing() = 0;

	//! Get/Set terrain height scale factor
	virtual f32 getHeightScale() = 0;
	virtual void setHeightScale(f32 scale) = 0;

	//! set terrains tile sets pool
	virtual void setTileSets(const core::array<STileSetParams> &tiles) = 0;

	//! add tile set to the terrain texturing
	virtual bool addTileSet(
		const c8 *tile0tex_file_name,
		const c8 *tile1tex_file_name,
		const c8 *tile2tex_file_name,
		const c8 *splatmap_file_name) = 0;
	//! set tile set
	virtual bool setTileSet(u32 idx,
		const c8 *tile0tex_file_name,
		const c8 *tile1tex_file_name,
		const c8 *tile2tex_file_name,
		const c8 *splatmap_file_name) = 0;
	//! removes tile set by its index
	virtual bool removeTileSet(u32 idx) = 0;
	//! return texture tile set by its index
	virtual const STileSet& getTileSet(u32 idx) = 0;
	
	//! return tile sets count
	virtual u32 getTileSetsCount() = 0;

	//! get the length of the square size of the heightfield 
    virtual u32 getHeightFieldSize() = 0;

	//! Return position/height of the cell (i, j) in the terrain
	//! The terrain local coordinate systems:
	//!
	//!             ^ j
	//!             |       (0, size - 1)
	//! (0, size-1) o-----------o
	//!             |  Terrain  |
	//!             |     o-----|-------------> X
	//!             |     |     |
	//!      (0, 0) o-----|-----o---------> i
	//!                   | (size - 1, 0)
	//!                   |
	//!                   v Y
	//!
	virtual const core::vector3df& getCellPosition(u32 i, u32 j) = 0;
	virtual f32 getCellHeight(u32 i, u32 j) = 0;

	//! calculates height at the absolute coords (x, z) in the terrain
	virtual f32 getHeight(f32 x, f32 z) = 0;

	//! calculates position at the absolute coords (x, z) in the terrain
	virtual const core::vector3df& getNormal(f32 x, f32 z) = 0;

    //! set tiles number for the detail map
    virtual void setTileRepeatNumber(u32 value) = 0;

	//! returns tiles number for the detail map
    virtual u32 getTileRepeatNumber() = 0;

	//! returns heightmap texture
	virtual vid::ITexture* getHeightMapTexture() = 0;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

