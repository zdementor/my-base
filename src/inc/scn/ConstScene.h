//|-------------------------------------------------------------------------
//| File:        ConstScene.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_const_scnHPP
#define my_const_scnHPP

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

enum E_SCENE_RENDER_FLAG
{
	ESRF_RENDER_SHADOW_DEBUG_DATA = (1 << 0),
	ESRF_RENDER_LIGHTS            = (1 << 1),
	ESRF_RENDER_TERRAIN_NORMALS   = (1 << 2),
	ESRF_RENDER_COLLISION_POINTS  = (1 << 3),
};

//---------------------------------------------------------------------------

enum E_SHADOW_TYPE
{
	ESHT_SHADOW_FAKE = 0,
	ESHT_SHADOW_VOLUME,

	E_SHADOW_TYPE_COUNT,

	E_SHADOW_TYPE_FORCE_32BIT = 0xffffffff
};

//---------------------------------------------------------------------------------

//! shadow types as string
static const c8* ShadowTypeStr[E_SHADOW_TYPE_COUNT] = 
{ 	
	"ESHT_SHADOW_FAKE",
	"ESHT_SHADOW_VOLUME"
};

//---------------------------------------------------------------------------------

//! shadow types as string (short version)
static const c8* ShadowTypeShortStr[E_SHADOW_TYPE_COUNT] = 
{ 	
	"Shadow Fake",
	"Shadow Volume"
}; 

//---------------------------------------------------------------------------------

//! enumeration for the scene node types
enum E_SCENE_NODE_TYPE
{	
	//! this types are for scene editor
	ESNT_ANIMATED_MESH_SCENE_NODE = 0,
	ESNT_BILLBOARD_SCENE_NODE,
	ESNT_LIGHT_SCENE_NODE,
	ESNT_KTREE_SCENE_NODE,
	ESNT_PARTICLE_SYSTEM_SCENE_NODE,	
	ESNT_SKY_BOX_SCENE_NODE,
	ESNT_TEST_SCENE_NODE,
	ESNT_TERRAIN_SCENE_NODE,
	ESNT_LENS_FLARE_SCENE_NODE,
	ESNT_TEXT_SCENE_NODE,
	ESNT_DUMMY_TANSFORMATION_SCENE_NODE,
	
	//! this type not used
	ESNT_UNKNOWN_SCENE_NODE,

	//! this types only for internal use by MyEngine
	ESNT_CAMERA_SCENE_NODE,		
	ESNT_PATH_FINDER_SCENE_NODE,

	//! for counting enumeration
	E_SCENE_NODE_TYPE_COUNT,

	E_SCENE_NODE_TYPE_FORCE_32BIT = 0xffffffff
};

//---------------------------------------------------------------------------------

//! node type names as string
static const c8* SceneNodeTypeStr[E_SCENE_NODE_TYPE_COUNT] = 
{ 	
	"ESNT_ANIMATED_MESH_SCENE_NODE",
	"ESNT_BILLBOARD_SCENE_NODE",
	"ESNT_LIGHT_SCENE_NODE",
	"ESNT_KTREE_SCENE_NODE",
	"ESNT_PARTICLE_SYSTEM_SCENE_NODE",	
	"ESNT_SKY_BOX_SCENE_NODE",
	"ESNT_TEST_SCENE_NODE",
	"ESNT_TERRAIN_SCENE_NODE",
	"ESNT_LENS_FLARE_SCENE_NODE",
	"ESNT_TEXT_SCENE_NODE",
	"ESNT_DUMMY_TANSFORMATION_SCENE_NODE",	
	"ESNT_UNKNOWN_SCENE_NODE",
	"ESNT_CAMERA_SCENE_NODE",		
	"ESNT_PATH_FINDER_SCENE_NODE"
};

//---------------------------------------------------------------------------------

//! node type names as string (short version)
static const c8* SceneNodeTypeShortStr[E_SCENE_NODE_TYPE_COUNT] = 
{ 	
	"Animated Mesh",
	"Billboard",
	"Light",
	"K-Tree",
	"Particle System",
	"Sky Box",
	"Test Node",
	"Terrain",
	"Lens Flare",
	"Text",
	"Dummy",
	"Unknown Type",
	"Camera",			
	"Path Finder"
};

//---------------------------------------------------------------------------------

static MY_INLINE const c8* getSceneNodeTypeName(E_SCENE_NODE_TYPE type)
{ return SceneNodeTypeStr[type]; }

static MY_INLINE const c8* getSceneNodeTypeReadableName(E_SCENE_NODE_TYPE type)
{ return SceneNodeTypeShortStr[type]; }

//---------------------------------------------------------------------------------

//! axis aligment type
enum E_AXIS_ALIGMENT_TYPE
{
	EAAT_NONE,

	EAAT_SHARP_CAM_TGT,
	EAAT_SHARP_CAM_POS,

	EAAT_BLEND_CAM_TGT,	
	EAAT_BLEND_CAM_POS,	

	EAAT_STRETCH_CAM_TGT,
	EAAT_STRETCH_CAM_POS,

	EAAT_NORMAL_AXIS,

	E_AXIS_ALIGMENT_TYPE_COUNT,

	E_AXIS_ALIGMENT_TYPE_FORCE_32BIT = 0xffffffff
};

//---------------------------------------------------------------------------

//! axis aligment type names as string
static const c8* AxisAligmentTypeStr[E_AXIS_ALIGMENT_TYPE_COUNT] = 
{ 
	"EAAT_NONE",
	"EAAT_SHARP_CAM_TGT",
	"EAAT_SHARP_CAM_POS",
	"EAAT_BLEND_CAM_TGT",
	"EAAT_BLEND_CAM_POS",
	"EAAT_STRETCH_CAM_TGT",
	"EAAT_STRETCH_CAM_POS",
	"EAAT_NORMAL_AXIS"
};

//---------------------------------------------------------------------------

//! enumeration for camera projection types
enum E_CAMERA_PROJECTION_TYPE
{
	ECPT_PERSPECTIVE = 0,
	ECPT_ORTHOGONAL,

	E_CAMERA_PROJECTION_TYPE_COUNT,

	E_CAMERA_PROJECTION_TYPE_FORCE_32BIT = 0xffffffff
};

//---------------------------------------------------------------------------

//! camera projection types as string
static const c8* CameraProjectionTypeStr[E_CAMERA_PROJECTION_TYPE_COUNT] = 
{
	"ECPT_PERSPECTIVE",
	"ECPT_ORTHOGONAL"
};

//----------------------------------------------------------------------------

enum E_CULLING_TESTS
{
	ECT_NO_CULL                      = 0x00000000,
	ECT_SCREEN_BOUNDS_CULL           = 0x00000001,
	ECT_VIEW_FRUSTRUM_CULL           = 0x00000002,
	ECT_OCCLUDERS_FRUSTRUM_CULL      = 0x00000004,
	ECT_OCCLUDERS_BOTTOM_VOLUME_CULL = 0x00000008,
	ECT_HARDWARE_OCCLUSION_CULL      = 0x00000010,
	ECT_DYNAMIC_MANAGER_CULL         = 0x00000020,

	E_CULLING_TESTS_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------	

enum E_MESH_TYPE
{
    //! Unknown mesh type.
    EMT_UNKNOWN = 0,

	//! Static 3DS Max file (not animated)
    EMT_3DS,

    //! Quake 3 .bsp Map (not animated)
    EMT_BSP,

    //! Quake 2 MD2 model file (key frame animation)
    EMT_MD2,  

    //! Cally 3D .cfg file (skeletal animation)
    EMT_CAL3D,

	E_MESH_TYPE_COUNT,

	E_MESH_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------	

enum E_LOD_LEVEL
{
	ELL_LOD_NEAREST =0,
	ELL_LOD_NEAR,
	ELL_LOD_FAR,
	ELL_LOD_FARTHEST,

	E_LOD_LEVEL_COUNT,

	E_LOD_LEVEL_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------	

enum E_PARTICLE_AFFECTOR_TYPE
{
    EPAT_FADEOUT=0,
    EPAT_GRAVITY,

	E_PARTICLE_AFFECTOR_TYPE_COUNT,

	E_PARTICLE_AFFECTOR_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! particle emitter type as string
static const c8* ParticleAffectorTypeStr[E_PARTICLE_AFFECTOR_TYPE_COUNT] = 
{  
    "EPAT_FADEOUT",
	"EPAT_GRAVITY"
};

//----------------------------------------------------------------------------

//! particle emitter type as string
static const c8* ParticleAffectorTypeShortStr[E_PARTICLE_AFFECTOR_TYPE_COUNT] = 
{  
    "Fadeout",
	"Gravity"
};

//----------------------------------------------------------------------------

MY_INLINE const c8* getParticleAffectorTypeName(E_PARTICLE_AFFECTOR_TYPE type)
{ return ParticleAffectorTypeStr[type]; }

MY_INLINE const c8* getParticleAffectorTypeReadableName(E_PARTICLE_AFFECTOR_TYPE type)
{ return ParticleAffectorTypeShortStr[type]; }

//----------------------------------------------------------------------------

enum E_PARTICLE_EMITTER_TYPE 
{
    EPET_POINT=0,
    EPET_BOX,

	E_PARTICLE_EMITTER_TYPE_COUNT,

	E_PARTICLE_EMITTER_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! particle emitter type as string
static const c8* ParticleEmitterTypeStr[E_PARTICLE_EMITTER_TYPE_COUNT] = 
{  
    "EPET_POINT",
	"EPET_BOX"
};

//----------------------------------------------------------------------------

//! particle emitter type as string
static const c8* ParticleEmitterTypeShortStr[E_PARTICLE_EMITTER_TYPE_COUNT] = 
{  
    "Point",
	"Box"
};

//----------------------------------------------------------------------------

MY_INLINE const c8* getParticleEmitterTypeName(E_PARTICLE_EMITTER_TYPE type)
{ return ParticleEmitterTypeStr[type]; }

MY_INLINE const c8* getParticleEmitterTypeReadableName(E_PARTICLE_EMITTER_TYPE type)
{ return ParticleEmitterTypeShortStr[type]; }

//----------------------------------------------------------------------------

enum E_PATH_FINDER_TYPE
{
	//! a-star pathfinder type
	EPFT_ASTAR,

	//! for counting enumeration
	E_PATH_FINDER_TYPE_COUNT,

	E_PATH_FINDER_TYPE_FORCE_32BIT = 0xffffffff
};

//---------------------------------------------------------------------------

enum E_BSP_ENTITY_TYPE
{
	EBSPET_WORLDSPAWN = 0,
	EBSPET_INFO_PLAYER_START,
	EBSPET_INFO_NULL,
	EBSPET_FUNC_DOOR,
	EBSPET_FUNC_BUTTON,
	EBSPET_FUNC_TRANSPARENT,
	EBSPET_LIGHT,
	
	EBSPET_UNKNOWN,

	E_BSP_ENTITY_TYPE_COUNT,

	E_BSP_ENTITY_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_GEOM_PRIMITIVE_TYPE 
{
	//! simple box
	EGPT_BOX,

	//! simple sphere
	EGPT_SPHERE, 

	//! cylinder along y-axis
	EGPT_CYLINDER, 

	//! capped cylinder along y-axis
	//! with cape radius same as main cylinder radius 
	EGPT_CAPPED_CYLINDER,

	E_GEOM_PRIMITIVE_TYPE_COUNT,

	E_GEOM_PRIMITIVE_TYPE_FORCE_32BIT = 0xffffffff
}; 

//---------------------------------------------------------------------------------

//! node type names as string
static const c8* GeomPrimitiveTypeStr[E_GEOM_PRIMITIVE_TYPE_COUNT] = 
{ 	
	"EGPT_BOX",
	"EGPT_SPHERE",
	"EGPT_CYLINDER",
	"EGPT_CAPPED_CYLINDER"
};

//---------------------------------------------------------------------------------

//! node type names as string (short version)
static const c8* GeomPrimitiveTypeShortStr[E_GEOM_PRIMITIVE_TYPE_COUNT] = 
{ 	
	"Box",
	"Sphere",
	"Cylinder",
	"Capsula"
};
 
//----------------------------------------------------------------------------

//! enumeration for terrain fog rendering mode
enum E_TERRAIN_FOG_RENDERING_MODE
{
	//! Stencil algorithm
	ETFRM_STENCIL = 0, 

	//! Maximal Intensity Projection algorithm
	ETFRM_MIP,
		
	//! Projected Convex Polyhedra algorithm
	ETFRM_PCP, 

	//! Never used, value specifing how much parameters there are.
	E_TERRAIN_FOG_RENDERING_MODE_COUNT,

	E_TERRAIN_FOG_RENDERING_MODE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! material type names  as string
static const c8* TerrainFogRenderingModeString[E_TERRAIN_FOG_RENDERING_MODE_COUNT] = 
{  
	"Stencil", 
	"MIP", 
	"PCP"
};

//----------------------------------------------------------------------------

//! scene render modes
enum E_SCENE_RENDERING_MODE
{
	ESRM_INDOOR=0,
	ESRM_OUTDOOR,

	E_SCENE_RENDERING_MODE_COUNT,

	E_SCENE_RENDERING_MODE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! scene render modes as string
static const c8* SceneRenderingModeString[E_SCENE_RENDERING_MODE_COUNT] = 
{  
	"Indoor", 
	"Outdoor"
};

//----------------------------------------------------------------------------

enum E_SCENE_NODE_ANIMATOR_TYPE
{
	ESNAT_FLY_CIRCLE_ANIMATOR = 0,
	ESNAT_FLY_STRAIGHT_ANIMATOR,
	ESNAT_FOLLOW_SPLINE_ANIMATOR,
	ESNAT_ROTATION_ANIMATOR,
	ESNAT_TEXTURE_ANIMATOR,
	ESNAT_UNKNOWN_ANIMATOR,

	E_SCENE_NODE_ANIMATOR_TYPE_COUNT,

	E_SCENE_NODE_ANIMATOR_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! scene render modes as string
static const c8* SceneRenderingAnimatorTypeString[E_SCENE_NODE_ANIMATOR_TYPE_COUNT] = 
{  
	"ESNAT_FLY_CIRCLE_ANIMATOR", 
	"ESNAT_FLY_STRAIGHT_ANIMATOR",
	"ESNAT_FOLLOW_SPLINE_ANIMATOR",
	"ESNAT_ROTATION_ANIMATOR",
	"ESNAT_TEXTURE_ANIMATOR",
	"ESNAT_UNKNOWN_ANIMATOR"
};

//----------------------------------------------------------------------------

//! enumeration for terrain color schemas
enum E_TERRAIN_COLOR_SCHEMA
{   
    ETCS_WATER_GREENLANDS=0, 
    ETCS_WOODLANDS_MOUNTAINS, 
    ETCS_DESERT, 
    ETCS_NONE,

	E_TERRAIN_COLOR_SCHEMA_COUNT,

	E_TERRAIN_COLOR_SCHEMA_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//!  terrain color schemas as string
static const c8* TerrainColorSchema[E_TERRAIN_COLOR_SCHEMA_COUNT] = 
{
	"ETCS_WATER_GREENLANDS", 
	"ETCS_WOODLANDS_MOUNTAINS", 
	"ETCS_DESERT",	
	"ETCS_NONE"
};

//----------------------------------------------------------------------------

//! enumeration for patch sizes specifying the size of patches in the TerrainSceneNode
enum E_TERRAIN_PATCH_SIZE
{
	//! patch size of 9, at most, use 4 levels of detail with this patch size.
	ETPS_9 = 9,

	//! patch size of 17, at most, use 5 levels of detail with this patch size.
	ETPS_17 = 17,

	//! patch size of 33, at most, use 6 levels of detail with this patch size.
	ETPS_33 = 33,

	//! patch size of 65, at most, use 7 levels of detail with this patch size.
	ETPS_65 = 65,

	//! patch size of 129, at most, use 8 levels of detail with this patch size.
	ETPS_129 = 129,

	E_TERRAIN_PATCH_SIZE_COUNT = 6,

	E_TERRAIN_PATCH_SIZE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//!  terrain color schemas as string
static const c8* TerrainPatchSizeString[E_TERRAIN_PATCH_SIZE_COUNT] = 
{
	"ETPS_9", 
	"ETPS_17", 
	"ETPS_33",	
	"ETPS_65",
	"ETPS_129"
};

//----------------------------------------------------------------------------

//! 
static const c8* SceneNodeCorePropertiesTagValue	= "SceneNodeCoreProperties";

MY_INLINE const c8* getSceneNodeCorePropertiesTagValue()
{ return SceneNodeCorePropertiesTagValue; }

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef my_const_scnHPP