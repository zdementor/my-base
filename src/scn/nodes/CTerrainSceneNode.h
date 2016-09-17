//|-------------------------------------------------------------------------
//| File:        CTerrainSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CTerrainSceneNodeHPP
#define CTerrainSceneNodeHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/ITerrainSceneNode.h>
#include <vid/SMaterial.h>
#include <scn/SMesh.h>
#include <scn/mesh_buffer.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class CTerrainSceneNode : public ITerrainSceneNode
{
public:

	CTerrainSceneNode(ISceneNode* parent, s32 id); 
	virtual ~CTerrainSceneNode();

	//! ITerrainSceneNode interface

	virtual const c8* getHeightMapFileName();
	virtual f32 getGridPointSpacing();

	virtual f32 getHeightScale();
	virtual void setHeightScale(f32 scale);

	virtual void setTileSets(const core::array<STileSetParams> &tiles);

	virtual bool addTileSet(
		const c8 *tile0tex_file_name,
		const c8 *tile1tex_file_name,
		const c8 *tile2tex_file_name,
		const c8 *splatmap_file_name);
	virtual bool setTileSet(u32 idx,
		const c8 *tile0tex_file_name,
		const c8 *tile1tex_file_name,
		const c8 *tile2tex_file_name,
		const c8 *splatmap_file_name);
	virtual bool removeTileSet(u32 idx);
	virtual const STileSet& getTileSet(u32 idx);
	virtual u32 getTileSetsCount();

    virtual u32 getHeightFieldSize();

	virtual const core::vector3df& getCellPosition(u32 cell_x, u32 cell_y);

    virtual void setTileRepeatNumber(u32 value);
    virtual u32 getTileRepeatNumber();

	virtual vid::ITexture* getHeightMapTexture();

	//! ISceneNode interface
	virtual const c8* getSceneCorePropertiesXMLString();

    virtual vid::SMaterial& getMaterial(s32 i)
	{ return m_Material; }

	virtual s32 getMaterialsCount()
	{ return 1; }

	const core::aabbox3df& getBoundingBox() const 
	{ return m_BoundingBox; } 

protected:

	bool _setMaps(const c8* hmap_filename, const c8* fogmap_filename,
        f32 gridPointSpacing, img::IImage **heightImageOut);

	virtual u32 getCompatibleHeightFieldSize(u32 origSize) = 0;
	void _clearMaps();
	void _clearTiles();

	void _resetMaterial();

	bool _addTileSet(
		const c8 *tile0tex_file_name,
		const c8 *tile1tex_file_name,
		const c8 *tile2tex_file_name,
		const c8 *splatmap_file_name);
	bool _setTileSet(u32 idx,
		const c8 *tile0tex_file_name,
		const c8 *tile1tex_file_name,
		const c8 *tile2tex_file_name,
		const c8 *splatmap_file_name);

    s32 m_HeightFieldSize, m_HeightFieldSizeValue; 
    f32 m_HeightScale, m_HeightScaleData; 

	vid::SMaterial m_Material; 

	core::stringc m_HeightMapFileName, m_FogMapFileName, m_SplatMapFileName;

	core::stringc m_HeightMapTextureName;
	
	f32 m_TileRepeatNumber;

    f32 m_GridPointSpacing, m_GridPointSpacingValue; 

	core::matrix4 m_InvAbsoluteTransform;

	core::vector3df	OldCameraPosition;
	core::vector3df	OldCameraRotation;
	f32 CameraMovementDelta;
	f32 CameraRotationDelta;

    mutable core::aabbox3df m_BoundingBox; 

	vid::IVideoDriver &m_VideoDriver;

	u32 m_EnabledLights;

	core::array<STileSet> m_Tiles;

	typedef core::array<vid::ITexture*> TTexArr;
	core::array< TTexArr > m_TilesTextures;

	f32 m_HeightMin, m_HeightMax;

	f32 m_SizeDiv2Dim;

	bool m_Reset;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef CTerrainIrrlichtSceneNodeHPP

