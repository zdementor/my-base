//|-------------------------------------------------------------------------
//| File:        CTerrainLibMiniSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CTerrainLibMiniSceneNodeHPP 
#define CTerrainLibMiniSceneNodeHPP
//-----------------------------------------------------------------------

#include "CTerrainSceneNode.h"

#include <ministub.hpp>

//-----------------------------------------------------------------------
namespace my { 
namespace scn { 
//-----------------------------------------------------------------------

using namespace core; 
using namespace vid; 
using namespace img; 

//-----------------------------------------------------------------------

class CTerrainLibMiniSceneNode : public CTerrainSceneNode 
{ 
public: 
    
    CTerrainLibMiniSceneNode(ISceneNode* parent, s32 id); 
    virtual ~CTerrainLibMiniSceneNode(); 
        
    virtual bool setMaps(const c8* hmap_filename, const c8* fogmap_filename,
        f32 gridPointSpacing);

	virtual void setHeightScale(f32 scale);

	virtual f32 getCellHeight(u32 cell_x, u32 cell_y);

	virtual void OnCheckCulling();
    virtual void OnPreRender(u32 timeMs); 

    virtual void OnPostRender(u32 timeMs); 

	core::vector3df getNormal(float x,float z); 

    f32 getHeight(float x,float z); 

private: 

    ////////////////////////////////////////////////////////////////// 
    // globals for libMini callbacks 
    ////////////////////////////////////////////////////////////////// 

    // prepares a buffer if the buffer is nonempty draws and then empties the buffer 
    // this function needs to be called after m_Stub->draw one more time 
    static void _BeginFan(); 

    // fills the buffer with terrain vertices 
    static void _FanVertex(float i,float y,float j); 

    // fills the buffer with fog vertices
    static void _PrismEdge(float x,float y,float yf,float z);

	// self pointer for callback 
    static CTerrainLibMiniSceneNode* ms_Self; 

    ////////////////////////////////////////////////////////////////// 

	virtual u32 getCompatibleHeightFieldSize(u32 origSize);

    ministub *m_Stub; 

	s32 m_FanStartIdx, m_FanVertsCnt;

	core::array<S3DVertex2TCoords> m_RenderVertices;
	core::array<u16> m_RenderIndices;

	u32 m_IndicesToRender, m_VerticesToRender;
	   
    void BeginFan(); 
    void FanVertex(float i,float y,float j); 
    void PrismEdge(float x,float y,float yf,float z); 

    void getHeightColor(f32 y, s32 &R, s32 &G, s32 &B);
   
    // resolution 
    float m_Resolution; 
    // field of view 
    float fovy; 
    // aspect ratio 
    float aspect; 
    // near value 
    float nearp; 
    // far value 
    float farp; 	

    // current camera pos in heightfield coordinate 
    vector3df cam_pos; 
    // current camera target in heightfield coordinate 
    vector3df cam_tgt; 
    // current camera up in heightfield coordinate 
    vector3df cam_up; 
    // current camera ciew direction
    vector3df cam_dir; 

    f32 m_TerrainWidth,  m_TerrainLength,  m_TerrainHeight;

	s32 *ColorArrayPtr;

    E_TERRAIN_COLOR_SCHEMA ColorScheme;
    s32 SampleColorsNum;

    f32 detail_div_size_1;
    s32 size_minus_1, size_minus_2;
	s32 size_plus_1;
    f32 detail_minus_2;
    f32 dX;
    f32 dZ;
    f32 deltaCoeff;

	f32 cam_height_over_terrain;
	f32 fog_height_above_camera;
	f32 fog_height_at_camera_pos; 
}; 

//-----------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//-----------------------------------------------------------------------
#endif 

