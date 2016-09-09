//|-------------------------------------------------------------------------
//| File:        ConstVideo.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_const_vidHPP
#define my_const_vidHPP

//----------------------------------------------------------------------------
namespace my {
namespace vid {
//----------------------------------------------------------------------------

enum
{
	MY_MATERIAL_MAX_LAYERS = 4,
	MY_MAX_COLOR_ATTACHEMENTS = 4,
};

//----------------------------------------------------------------------------

//! An enum for video driver family
enum E_DRIVER_FAMILY
{
    //! Null device driver family
    EDF_NULL =  0,     
    //! DirectX video driver family
    EDF_DIRECTX,
    //! OpenGL video driver family
    EDF_OPENGL
};

//----------------------------------------------------------------------------

#define MY_DRIVER_TYPE_NULL		0
#define MY_DRIVER_TYPE_DIRECTX9	1
#define MY_DRIVER_TYPE_OPENGL11	2
#define MY_DRIVER_TYPE_OPENGL12	3
#define MY_DRIVER_TYPE_OPENGL21	4
#define MY_DRIVER_TYPE_OPENGL32	5

//! An enum for all types of drivers the Irrlicht Engine supports.
enum E_DRIVER_TYPE
{
    //! Null device, useful for applications to run the engine without visualisation. 
    //! The null device is able to load textures, but does not render and display
    //! any graphics.
    EDT_NULL		= MY_DRIVER_TYPE_NULL,     
    //! DirectX 9 device, only available on Windows platforms.
    EDT_DIRECTX9	= MY_DRIVER_TYPE_DIRECTX9,
    //! OpenGL device, available on all Windows platforms and on Linux.
	EDT_OPENGL11	= MY_DRIVER_TYPE_OPENGL11,
	EDT_OPENGL12	= MY_DRIVER_TYPE_OPENGL12,
	EDT_OPENGL21	= MY_DRIVER_TYPE_OPENGL21,
	EDT_OPENGL32	= MY_DRIVER_TYPE_OPENGL32,

	E_DRIVER_TYPE_COUNT,

	E_DRIVER_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

static const c8* DriverTypeName[E_DRIVER_TYPE_COUNT] = 
{
	"EDT_NULL", 
	"EDT_DIRECTX9",
	"EDT_OPENGL11",
	"EDT_OPENGL12",
	"EDT_OPENGL21",
	"EDT_OPENGL32",	
};

//----------------------------------------------------------------------------

static const c8* DriverTypeReadableName[E_DRIVER_TYPE_COUNT] = 
{
	"NULL", 
	"Direct3D9.0c", 
	"OpenGL1.1",
	"OpenGL1.2",
	"OpenGL2.1",
	"OpenGL3.2",	
};

//----------------------------------------------------------------------------

MY_FORCEINLINE const c8* getDriverTypeName(E_DRIVER_TYPE type)
{ return DriverTypeName[type]; }

MY_FORCEINLINE const c8* getDriverTypeReadableName(E_DRIVER_TYPE type)
{ return DriverTypeReadableName[type]; }

//----------------------------------------------------------------------------

static MY_FORCEINLINE E_DRIVER_FAMILY getDriverFamily(E_DRIVER_TYPE type)
{
	if (type == EDT_NULL)
		return EDF_NULL;
	else if (type < EDT_OPENGL11)
		return EDF_DIRECTX;
	return EDF_OPENGL;
}

//----------------------------------------------------------------------------

//! Enumeration flags telling the video driver in which format textures
//! should be created.
enum E_TEXTURE_CREATION_FLAG
{
    //! Automaticaly creates mip map levels for the textures.
    ETCF_AUTOGEN_MIP_MAPS = 0x00000001,

	//! Automaticaly creates color key for the textures.
    ETCF_CREATE_COLOR_KEY = 0x00000002,

	//! Automaticaly creates a normal map textures.
    ETCF_CREATE_NORMAL_MAP = 0x00000004,

    //! This flag is never used, it only forces the compiler to 
    //! compile these enumeration values to 32 bit.
    E_TEXTURE_CREATION_FLAG_FORCE_32_BIT = 0xffffffff
};

//---------------------------------------------------------------------------

enum E_TEXTURE_TYPE
{
	ETT_COLOR_MAP = 0,
	ETT_ANIMATED, // consint from sequence from other color maps

	E_TEXTURE_TYPE_COUNT,

	E_TEXTURE_TYPE_FORCE_32_BIT = 0xffffffff
};

//---------------------------------------------------------------------------

enum E_TEXTURE_LAYER_TYPE
{
	ETLT_DIFFUSE_MAP = 0,
	ETLT_ATTENUATION_MAP,
	ETLT_SPECULAR_MAP,
	ETLT_LIGHT_MAP,
	ETLT_NORMAL_MAP,
	ETLT_SPLATTING_MAP,

	E_TEXTURE_LAYER_TYPE_COUNT,

	E_TEXTURE_LAYER_TYPE_FORCE_32_BIT = 0xffffffff
};

static const c8 *TextureLayerTypeName[E_TEXTURE_LAYER_TYPE_COUNT] =
{
	"ETLT_DIFFUSE_MAP",
	"ETLT_ATTENUATION_MAP",
	"ETLT_SPECULAR_MAP",
	"ETLT_LIGHT_MAP",
	"ETLT_NORMAL_MAP",
	"ETLT_SPLATTING_MAP",
};

static const c8 *TextureLayerTypeReadableName[E_TEXTURE_LAYER_TYPE_COUNT] =
{
	"Diffuse map",
	"Atten. map",
	"Specular map",
	"Light map",
	"Normal map",
	"Splatting map",
};

MY_FORCEINLINE const c8* getTextureLayerTypeName(E_TEXTURE_LAYER_TYPE type)
{ return TextureLayerTypeName[type]; }

MY_FORCEINLINE const c8* getTextureLayerTypeReadableName(E_TEXTURE_LAYER_TYPE type)
{ return TextureLayerTypeReadableName[type]; }

//---------------------------------------------------------------------------

enum E_TEXTURE_FORMAT
{
	ETF_TEXTURE_1D = 0,
	ETF_TEXTURE_2D,
	ETF_TEXTURE_3D,
	ETF_CUBE_MAP,

	E_TEXTURE_FORMAT_COUNT,

	E_TEXTURE_FORMAT_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! Enumeration for all vertex types there are.
enum E_VERTEX_TYPE
{
	//! Simple vertex type (only position data)
    EVT_SIMPLE = 0, 

	//! Simple vertex with color data
    EVT_SIMPLE_COLOURED, 

    //! Standard vertex type with normal and single texture layer
    EVT_1TCOORDS, 

    //! Standard vertex type with color data (for per vertex color effects)
    EVT_1TCOORDS_COLOURED, 

    //! Vertex with two texture coordinates. 
	//! Usually used for geometry with lightmaps or other special materials.
    EVT_2TCOORDS,

    //! Vertex with two texture coordinates with color data (for per vertex color effects)
	//! Usually used for geometry with lightmaps or other special materials.
    EVT_2TCOORDS_COLOURED,

	//! Vertex with a tangent and binormal vector. 
	//! Usually used for tangent space normal mapping.
	EVT_1TCOORDS_TBN,

	//! Vertex with a tangent and binormal vector with color data. 
	//! Usually used for tangent space normal mapping.
	EVT_1TCOORDS_TBN_COLOURED,
	
	//! Vertex with two texture coordinates, with a tangent and binormal vector also. 
	//! Usually used for tangent space normal mapping.
	EVT_2TCOORDS_TBN,

	//! Vertex with two texture coordinates, with color data, with a tangent and binormal vector also. 
	//! Usually used for tangent space normal mapping.
	EVT_2TCOORDS_TBN_COLOURED,

	//! Vertex with a W-component, single tex coord and with color data. 
	//! Usialy used for GUI 2d drawing.
	EVT_1TCOORDS_RHW_COLOURED,

	E_VERTEX_TYPE_COUNT,

	E_VERTEX_TYPE_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! vertex types as string
static const c8* VertexTypeName[E_VERTEX_TYPE_COUNT] = 
{
	"EVT_SIMPLE",
	"EVT_SIMPLE_COLOURED",
	"EVT_1TCOORDS", 
    "EVT_1TCOORDS_COLOURED", 
    "EVT_2TCOORDS",
    "EVT_2TCOORDS_COLOURED",
	"EVT_1TCOORDS_TBN",
	"EVT_1TCOORDS_TBN_COLOURED",
	"EVT_2TCOORDS_TBN",
	"EVT_2TCOORDS_TBN_COLOURED",
	"EVT_1TCOORDS_RHW_COLOURED"
};

//----------------------------------------------------------------------------

MY_FORCEINLINE const c8* getVertexTypeName(E_VERTEX_TYPE vtype)
{ return VertexTypeName[vtype]; }

//----------------------------------------------------------------------------

enum E_VERTEX_COMPONENTS
{
	EVC_POS = 1,
	EVC_COL = 2,
	EVC_TC0 = 4,
	EVC_TC1 = 8,
	EVC_NRM = 16,
	EVC_TBN = 32,
	EVC_RHW = 64
};

//----------------------------------------------------------------------------

//! Enumeration for draw primitive types
enum E_DRAW_PRIMITIVE_TYPE
{
	EDPT_TRIANGLE_LIST = 0,
	EDPT_TRIANGLE_STRIP,
	EDPT_TRIANGLE_FAN,
	EDPT_POINT_LIST,
    EDPT_LINE_LIST,
    EDPT_LINE_STRIP,

	E_DRAW_PRIMITIVE_TYPE_COUNT,

	E_DRAW_PRIMITIVE_TYPE_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! enumeration for texture filters
enum E_TEXTURE_FILTER
{
	//! Without filter
	ETF_NONE,

	//! Bilinear filter
	ETF_BILINEAR,

	//! Trilinear filter
	ETF_TRILINEAR,

	//! Anisotropic filter
	ETF_ANISOTROPIC,
	
	//! Not used
	E_TEXTURE_FILTER_COUNT,

	E_TEXTURE_FILTER_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! texture filters as string
static const c8* TextureFilterName[E_TEXTURE_FILTER_COUNT] = 
{
	"ETF_NONE", 
	"ETF_BILINEAR", 
	"ETF_TRILINEAR" ,
	"ETF_ANISOTROPIC"
};

//----------------------------------------------------------------------------

//! texture filters as string
static const c8* TextureFilterReadableName[E_TEXTURE_FILTER_COUNT] = 
{
	"None", 
	"Bilinear", 
	"Trilinear" ,
	"Anisotropic"
};

//---------------------------------------------------------------------------

enum E_MATERIAL_RENDERER_TYPE
{	
	EMRT_STANDARD_RENDERER=0,
	
	E_MATERIAL_RENDERER_TYPE_COUNT,

	//! This value is not used. It only forces this enumeration to compile in 32 bit. 
    E_MATERIAL_RENDERER_TYPE_FORCE_32BIT = 0x7fffffff    
};

//---------------------------------------------------------------------------

//! Material flags
enum E_MATERIAL_FLAG
{
	//! Flat or Gouraud shading? Default: true
	EMF_GOURAUD_SHADING = 0,

	//! May be written to the zbuffer or is it readonly. Default: true
	//! This flag is ignored, if the material type is a transparent type.
	EMF_ZWRITE_ENABLE,

	//! Is backfaceculling enabled? Default: true
	EMF_BACK_FACE_CULLING,

	//! Frontface is in counterclockwise order? Default: true
	EMF_FRONT_FACE_CCW,

	//! Is fog enabled? Default: false
	EMF_FOG_ENABLE,

	//! Is off/on mip maps? Default: false
	EMF_MIP_MAP_OFF,

	//! Is blending enabled? Default: false
	EMF_BLENDING,

	//! This is not a flag, but a value indicating how much flags there are.
	E_MATERIAL_FLAG_COUNT,	

	E_MATERIAL_FLAG_FORCE_32_BIT = 0xffffffff
};

static const c8* MaterialFlagName[E_MATERIAL_FLAG_COUNT] = 
{            
	"EMF_GOURAUD_SHADING", 
    "EMF_ZWRITE_ENABLE", 
	"EMF_BACK_FACE_CULLING",
	"EMF_FRONT_FACE_CCW",
	"EMF_FOG_ENABLE",
	"EMF_MIP_MAP_OFF",
	"EMF_BLENDING",
};

static const c8* MaterialFlagReadableName[E_MATERIAL_FLAG_COUNT] = 
{            
	"gouraud shading", 
    "Z-write enable", 
	"backface cull", 
	"frontface CCW", 
	"fog enable",
	"mip map off",
	"blending",
};

MY_FORCEINLINE const c8* getMaterialFlagName(E_MATERIAL_FLAG flag)
{ return MaterialFlagName[flag]; }

MY_FORCEINLINE const c8* getMaterialFlagReadableName(E_MATERIAL_FLAG flag)
{ return MaterialFlagReadableName[flag]; }

//---------------------------------------------------------------------------

//! Material flags bits, that stored flags values
static const u16 MaterialFlagBit[E_MATERIAL_FLAG_COUNT] =
{
	u16(0xffff & (1 << 0)),
	u16(0xffff & (1 << 1)),
	u16(0xffff & (1 << 2)),
	u16(0xffff & (1 << 3)),
	u16(0xffff & (1 << 4)),
	u16(0xffff & (1 << 5)),
	u16(0xffff & (1 << 6)),
};

//---------------------------------------------------------------------------

//! Blending factors for source objects with the scene.

enum E_SRC_BLEND_FUNCTION
{
	ESBF_ONE = 0,
	ESBF_ZERO,
	ESBF_DST_COLOR,
	ESBF_ONE_MINUS_DST_COLOR,
	ESBF_SRC_ALPHA,
	ESBF_ONE_MINUS_SRC_ALPHA,
	ESBF_DST_ALPHA,
	ESBF_ONE_MINUS_DST_ALPHA,
	ESBF_SRC_ALPHA_SATURATE,

	E_SRC_BLEND_FUNCTION_COUNT,

	E_SRC_BLEND_FUNCTION_FORCE_32_BIT = 0xffffffff
};

static const c8* SrcBlendFunctionName[E_SRC_BLEND_FUNCTION_COUNT] = 
{
	"ESBF_ONE",
	"ESBF_ZERO",
	"ESBF_DST_COLOR",
	"ESBF_ONE_MINUS_DST_COLOR",
	"ESBF_SRC_ALPHA",
	"ESBF_ONE_MINUS_SRC_ALPHA",
	"ESBF_DST_ALPHA",
	"ESBF_ONE_MINUS_DST_ALPHA",
	"ESBF_SRC_ALPHA_SATURATE"	
};

static const c8* SrcBlendFunctionReadableName[E_SRC_BLEND_FUNCTION_COUNT] = 
{
	"One",
	"Zero",
	"Dst.Color",
	"1 - Dst.Color",
	"Src.Alpha",
	"1 - Src.Alpha",
	"Dst.Alpha",
	"1 - Dst.Alpha",
	"Src.Alpha sat."	
};

MY_FORCEINLINE const c8* getSrcBlendFunctionName(E_SRC_BLEND_FUNCTION func)
{ return SrcBlendFunctionName[func]; }

MY_FORCEINLINE const c8* getSrcBlendFunctionReadableName(E_SRC_BLEND_FUNCTION func)
{ return SrcBlendFunctionReadableName[func]; }

//---------------------------------------------------------------------------

//! Blending factors for destination objects with the scene.

enum E_DST_BLEND_FUNCTION
{
	EDBF_ONE = 0,
	EDBF_ZERO,
	EDBF_SRC_ALPHA,
	EDBF_ONE_MINUS_SRC_ALPHA,
	EDBF_DST_ALPHA,
	EDBF_ONE_MINUS_DST_ALPHA,
	EDBF_SRC_COLOR,
	EDBF_ONE_MINUS_SRC_COLOR,

	E_DST_BLEND_FUNCTION_COUNT,

	E_DST_BLEND_FUNCTION_FORCE_32_BIT = 0xffffffff
};

static const c8* DstBlendFunctionName[E_DST_BLEND_FUNCTION_COUNT] = 
{
	"EDBF_ONE",
	"EDBF_ZERO",
	"EDBF_SRC_ALPHA",
	"EDBF_ONE_MINUS_SRC_ALPHA",
	"EDBF_DST_ALPHA",
	"EDBF_ONE_MINUS_DST_ALPHA",
	"EDBF_SRC_COLOR",
	"EDBF_ONE_MINUS_SRC_COLOR"	
};

static const c8* DstBlendFunctionReadableName[E_DST_BLEND_FUNCTION_COUNT] = 
{
	"One",
	"Zero",
	"Src.Alpha",
	"1 - Src.Alpha",
	"Dst.Alpha",
	"1 - Dst.Alpha",
	"Src.Color",
	"1 - Src.Color"	
};

MY_FORCEINLINE const c8* getDstBlendFunctionName(E_DST_BLEND_FUNCTION func)
{ return DstBlendFunctionName[func]; }

MY_FORCEINLINE const c8* getDstBlendFunctionReadableName(E_DST_BLEND_FUNCTION func)
{ return DstBlendFunctionReadableName[func]; }

//---------------------------------------------------------------------------

//! Blending equations for source objects with the scene.

enum E_BLEND_EQUATION
{
	EBE_ADD = 0,
	EBE_REVERSE_SUBTRACT,
	EBE_SUBTRACT,
	EBE_MIN,
	EBE_MAX,

	E_BLEND_EQUATION_COUNT,
	E_BLEND_EQUATION_FORCE_32_BIT = 0xffffffff
};

static const c8* BlendEquationName[E_BLEND_EQUATION_COUNT] = 
{
	"EBE_ADD",
	"EBE_SUBTRACT",
	"EBE_REVERSE_SUBTRACT",
	"EBE_MIN",
	"EBE_MAX"
};

static const c8* BlendEquationReadableName[E_BLEND_EQUATION_COUNT] = 
{
	"Add",
	"Subtract",
	"Rev.Subtract",
	"Min",
	"Max"
};

MY_FORCEINLINE const c8* getBlendEquationName(E_BLEND_EQUATION eq)
{ return BlendEquationName[eq]; }

MY_FORCEINLINE const c8* getBlendEquationReadableName(E_BLEND_EQUATION eq)
{ return BlendEquationReadableName[eq]; }

//---------------------------------------------------------------------------

enum E_TEXCOORD_GEN_TYPE
{
	ETCGT_MANUAL_MAPPING = 0,
	ETCGT_ENVIRONMENT_MAPPING, 
	ETCGT_PROJECTED_MAPPING, 

	E_TEXCOORD_GEN_TYPE_COUNT,
	E_TEXCOORD_GEN_TYPE_FORCE_32_BIT = 0xffffffff
};

static const c8* TexCoordGenTypeName[E_TEXCOORD_GEN_TYPE_COUNT] =
{
	"ETCGT_MANUAL_MAPPING",
	"ETCGT_ENVIRONMENT_MAPPING",
	"ETCGT_PROJECTED_MAPPING"
};

static const c8* TexCoordGenTypeReadableName[E_TEXCOORD_GEN_TYPE_COUNT] =
{
	"Manual",
	"Environment",
	"Projected"
};

MY_FORCEINLINE const c8* getTexCoordGenTypeName(E_TEXCOORD_GEN_TYPE type)
{ return TexCoordGenTypeName[type]; }

MY_FORCEINLINE const c8* getTexCoordGenTypeReadableName(E_TEXCOORD_GEN_TYPE type)
{ return TexCoordGenTypeReadableName[type]; }

//---------------------------------------------------------------------------

enum E_TEXCOORD_MOD_TYPE
{
	ETCMT_TRANSFORM,
	ETCMT_TURBULENT,
	ETCMT_SCROLL,
	ETCMT_SCALE,
	ETCMT_ROTATE,

	E_TEXCOORD_MOD_TYPE_COUNT,

	E_TEXCOORD_MOD_TYPE_FORCE_32_BIT = 0xffffffff
};

static const c8* TexCoordModTypeName[E_TEXCOORD_MOD_TYPE_COUNT] =
{
	"ETCMT_TRANSFORM",
	"ETCMT_TURBULENT",
	"ETCMT_SCROLL",
	"ETCMT_SCALE",
	"ETCMT_ROTATE"
};

static const c8* TexCoordModTypeReadableName[E_TEXCOORD_MOD_TYPE_COUNT] =
{
	"Transform",
	"Turbulent",
	"Scroll",
	"Scale",
	"Rotate"
};

MY_FORCEINLINE const c8* getTexCoordModTypeName(E_TEXCOORD_MOD_TYPE type)
{ return TexCoordModTypeName[type]; }

MY_FORCEINLINE const c8* getTexCoordModTypeReadableName(E_TEXCOORD_MOD_TYPE type)
{ return TexCoordModTypeReadableName[type]; }

//---------------------------------------------------------------------------

enum E_COLOR_GEN_TYPE
{
	ECGT_VERTEX=0,	// taken from vertex color
	ECGT_FUNCTION,	// functionaly generated
	ECGT_CUSTOM,	// custom color value

	E_COLOR_GEN_TYPE_COUNT,

	E_COLOR_GEN_TYPE_FORCE_32_BIT = 0xffffffff
};

static const c8* ColorGenTypeName[E_COLOR_GEN_TYPE_COUNT] =
{
	"ECGT_VERTEX",
	"ECGT_FUNCTION",
	"ECGT_CUSTOM"
};

static const c8* ColorGenTypeReadableName[E_COLOR_GEN_TYPE_COUNT] =
{
	"Vertex",
	"Function",
	"Custom"
};

MY_FORCEINLINE const c8* getColorGenTypeName(E_COLOR_GEN_TYPE type)
{ return ColorGenTypeName[type]; }

MY_FORCEINLINE const c8* getColorGenTypeReadableName(E_COLOR_GEN_TYPE type)
{ return ColorGenTypeReadableName[type]; }

//---------------------------------------------------------------------------

enum E_ALPHA_GEN_TYPE
{
	EAGT_VERTEX = 0,	// taken from vertex alpha
	EAGT_FUNCTION,		// functionaly generated
	EAGT_CUSTOM,		// custom alpha value

	E_ALPHA_GEN_TYPE_COUNT,

	E_ALPHA_GEN_TYPE_FORCE_32_BIT = 0xffffffff
};

static const c8* AlphaGenTypeName[E_ALPHA_GEN_TYPE_COUNT] =
{
	"EAGT_VERTEX",
	"EAGT_FUNCTION",
	"EAGT_CUSTOM"
};

static const c8* AlphaGenTypeReadableName[E_ALPHA_GEN_TYPE_COUNT] =
{
	"Vertex",
	"Function",
	"Custom"
};

MY_FORCEINLINE const c8* getAlphaGenTypeName(E_ALPHA_GEN_TYPE type)
{ return AlphaGenTypeName[type]; }

MY_FORCEINLINE const c8* getAlphaGenTypeReadableName(E_ALPHA_GEN_TYPE type)
{ return AlphaGenTypeReadableName[type]; }

//---------------------------------------------------------------------------

//! Texture addressing modes - default is TAM_REPEAT.
enum E_TEXCOORD_ADDRESS_MODE
{
	// Texture wraps at values over 1.0
    ETCAM_WRAP = 0,    
	// Texture mirrors (flips) at joins over 1.0
    ETCAM_MIRROR,
    // Texture clamps at 1.0
    ETCAM_CLAMP,
    // Texture coordinates outside the range [0.0, 1.0] are set to the border colour
    ETCAM_BORDER,

	E_TEXCOORD_ADDRESS_MODE_COUNT,

	E_TEXCOORD_ADDRESS_MODE_FORCE_32_BIT = 0xffffffff
};

static const c8* TexCoordAddrModeName[E_TEXCOORD_ADDRESS_MODE_COUNT] =
{
	"ETCAM_WRAP",    
	"ETCAM_MIRROR",
	"ETCAM_CLAMP",
    "ETCAM_BORDER"
};

static const c8* TexCoordAddrModeReadableName[E_TEXCOORD_ADDRESS_MODE_COUNT] =
{
	"Wrap",
	"Mirror",
	"Clamp",
    "Border"
};

MY_FORCEINLINE const c8* getTexCoordAddrModeName(E_TEXCOORD_ADDRESS_MODE mode)
{ return TexCoordAddrModeName[mode]; }

MY_FORCEINLINE const c8* getTexCoordAddrModeReadableName(E_TEXCOORD_ADDRESS_MODE mode)
{ return TexCoordAddrModeReadableName[mode]; }

//----------------------------------------------------------------------------

//! Enumeration for different types of lights
enum E_LIGHT_TYPE
{
    //! point light, it has a position in space and radiates light in all directions
    ELT_POINT = 0,

    //! directional light, coming from a direction from an infinite distance
    ELT_DIRECTIONAL,

	E_LIGHT_TYPE_COUNT,

	E_LIGHT_TYPE_FORCE_32_BIT = 0xffffffff
};

static const c8* LightTypeName[E_LIGHT_TYPE_COUNT] =
{
	"ELT_POINT", 	
	"ELT_DIRECTIONAL"
};

static const c8* LightTypeReadableName[E_LIGHT_TYPE_COUNT] =
{
	"Point",
	"Directional"
};

MY_FORCEINLINE const c8* getLightTypeName(E_LIGHT_TYPE type)
{ return LightTypeName[type]; }

MY_FORCEINLINE const c8* getLightTypeReadableName(E_LIGHT_TYPE type)
{ return LightTypeReadableName[type]; }

//----------------------------------------------------------------------------

//! enumeration for querying features of the video driver.
enum E_VIDEO_DRIVER_FEATURE
{
	//! Is driver able to render to textures (ore more) at one render pass ?
	EVDF_MULITEXTURE = 0,

    //! Is driver able to render to a surface?
    EVDF_RENDER_TO_TARGET,  

    //! Is driver able to render with a bilinear filter applied?
    EVDF_BILINEAR_FILTER, 

	//! Is driver able to render with a trilinear filter applied?
	EVDF_TRILINEAR_FILTER,

	//! Is driver able to render with a anisotropic filter applied?
	EVDF_ANISOTROPIC_FILTER,

    //! Can the driver handle mip maps?
    EVDF_MIP_MAP,       

    //! Are stencil buffers switched on and does the device support stencil buffers?
    EVDF_STENCIL_BUFFER,

	//! Is GLSL/HLSL supported?
	EVDF_SHADER_LANGUAGE,
	
	//! Is Occlusion Querry  supported?
	EVDF_OCCLUSION_QUERY,

	//! Is Compressed textures (DXT1/DXT3/DXT5) supported by video hardware?
	EVDF_COMPRESSED_TEXTURES,

	E_VIDEO_DRIVER_FEATURE_COUNT,

	E_VIDEO_DRIVER_FEATURE_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! enumeration for geometry transformation states
enum E_TRANSFORMATION_STATE
{
    //! View transformation
    ETS_VIEW = 0,
    //! World transformation
    ETS_MODEL,
    //! Projection transformation
    ETS_PROJ,

    //! Not used
    E_TRANSFORMATION_STATE_COUNT,

	E_TRANSFORMATION_STATE_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_FOG_TYPE
{
	EFT_LINEAR,
	EFT_EXP,
	EFT_EXP2,

	//! For counting enum
	E_FOG_TYPE_COUNT,

	E_FOG_TYPE_FORCE_32_BIT = 0xffffffff
};

static const c8* FogTypeName[E_FOG_TYPE_COUNT] = 
{
	"EFT_LINEAR", 
	"EFT_EXP", 
	"EFT_EXP2"
};

static const c8* FogTypeReadableName[E_FOG_TYPE_COUNT] = 
{
	"Linear",
	"Exp",
	"Exp^2"
};

MY_FORCEINLINE const c8* getFogTypeName(E_FOG_TYPE type)
{ return FogTypeName[type]; }

MY_FORCEINLINE const c8* getFogTypeReadableName(E_FOG_TYPE type)
{ return FogTypeReadableName[type]; }

//----------------------------------------------------------------------------

enum E_UNIFORM_TYPE
{
	EUT_NONE = 0,
	EUT_MODEL_VIEW_PROJ_MATRIX,
	EUT_MODEL_VIEW_MATRIX,
	EUT_MODEL_MATRIX,
	EUT_NORMAL_MATRIX,
	EUT_GLOBAL_AMBIENT_COLOR,
	EUT_MATERIAL_COLORS,
	EUT_MATERIAL_SHININESS,
	EUT_LIGHTING,
	EUT_EYE_POS,
	EUT_TEXTURE0,
	EUT_TEXTURE1,
	EUT_TEXTURE2,
	EUT_TEXTURE3,
	EUT_TEXTURE_MATRIX0,
	EUT_TEXTURE_MATRIX1,
	EUT_TEXTURE_MATRIX2,
	EUT_TEXTURE_MATRIX3,
	EUT_FOG_PARAMS,
	EUT_FOG_COLOR,

	E_UNIFORM_TYPE_COUNT,
	E_UNIFORM_TYPE_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_UNIFORM_FLAG
{
	EUF_NONE						= 0,
	EUF_MODEL_VIEW_PROJ_MATRIX		= 1 << 0,
	EUF_MODEL_VIEW_MATRIX			= 1 << 1,
	EUF_MODEL_MATRIX				= 1 << 2,
	EUF_NORMAL_MATRIX				= 1 << 3,
	EUF_GLOBAL_AMBIENT_COLOR		= 1 << 4,
	EUF_MATERIAL_COLORS				= 1 << 5,
	EUF_MATERIAL_SHININESS			= 1 << 6,
	EUF_LIGHTING					= 1 << 7,
	EUF_EYE_POS						= 1 << 8,
	EUF_TEXTURE0					= 1 << 9,
	EUF_TEXTURE1					= 1 << 10,
	EUF_TEXTURE2					= 1 << 11,
	EUF_TEXTURE3					= 1 << 12,
	EUF_TEXTURE_MATRIX0				= 1 << 13,
	EUF_TEXTURE_MATRIX1				= 1 << 14,
	EUF_TEXTURE_MATRIX2				= 1 << 15,
	EUF_TEXTURE_MATRIX3				= 1 << 16,
	EUF_FOG_PARAMS                  = 1 << 17,
	EUF_FOG_COLOR                   = 1 << 18,

	E_UNIFORM_FLAG_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

static u32 UniformTypeBits[E_UNIFORM_TYPE_COUNT] =
{
	EUF_NONE,
	EUF_MODEL_VIEW_PROJ_MATRIX,
	EUF_MODEL_VIEW_MATRIX,
	EUF_MODEL_MATRIX,
	EUF_NORMAL_MATRIX,
	EUF_GLOBAL_AMBIENT_COLOR,
	EUF_MATERIAL_COLORS,
	EUF_MATERIAL_SHININESS,
	EUF_LIGHTING,
	EUF_EYE_POS,
	EUF_TEXTURE0,
	EUF_TEXTURE1,
	EUF_TEXTURE2,
	EUF_TEXTURE3,
	EUF_TEXTURE_MATRIX0,
	EUF_TEXTURE_MATRIX1,
	EUF_TEXTURE_MATRIX2,
	EUF_TEXTURE_MATRIX3,
	EUF_FOG_PARAMS,
	EUF_FOG_COLOR,
};

//----------------------------------------------------------------------------

static const c8* UniformTypeNames[E_UNIFORM_TYPE_COUNT] =
{
	"EUT_NONE",
	"EUT_MODEL_VIEW_PROJ_MATRIX",
	"EUT_MODEL_VIEW_MATRIX",
	"EUT_MODEL_MATRIX",
	"EUT_NORMAL_MATRIX",
	"EUT_GLOBAL_AMBIENT_COLOR",
	"EUT_MATERIAL_COLORS",
	"EUT_MATERIAL_SHININESS",
	"EUT_LIGHTING",
	"EUT_EYE_POS",
	"EUT_TEXTURE0",
	"EUT_TEXTURE1",
	"EUT_TEXTURE2",
	"EUT_TEXTURE3",
	"EUT_TEXTURE_MATRIX0",
	"EUT_TEXTURE_MATRIX1",
	"EUT_TEXTURE_MATRIX2",
	"EUT_TEXTURE_MATRIX3",
	"EUT_FOG_PARAMS",
	"EUT_FOG_COLOR",
};

static const c8* UniformTypeReadableNames[E_UNIFORM_TYPE_COUNT] =
{
	"NoName",
	"ModelViewProjMatrix",
    "ModelViewMatrix",
    "ModelMatrix",
	"NormalMatrix",
	"GlobalAmbientColor",
	"MaterialColors",
	"MaterialShininess",
	"Lighting",
	"EyePos",
	"Texture0",
	"Texture1",
	"Texture2",
	"Texture3",
	"TextureMatrix0",
	"TextureMatrix1",
	"TextureMatrix2",
	"TextureMatrix3",
	"FogParams",
	"FogColor",
};

MY_FORCEINLINE E_UNIFORM_FLAG getUniformFlag(E_UNIFORM_TYPE type)
{ return (E_UNIFORM_FLAG)UniformTypeBits[type]; }

MY_FORCEINLINE const c8 *getUniformName(E_UNIFORM_TYPE type)
{ return UniformTypeNames[type]; }

MY_FORCEINLINE const c8 *getUniformReadableName(E_UNIFORM_TYPE type)
{ return UniformTypeReadableNames[type]; }

//----------------------------------------------------------------------------

enum E_POLYGON_FILL_MODE
{
	EPFM_SOLID = 0,
	EPFM_WIREFRAME,
	EPFM_POINT,    

	E_POLYGON_FILL_MODE_COUNT,

	E_POLYGON_FILL_MODE_FORCE_32_BIT = 0xffffffff
};

static const c8* PolygonFillModeNames[E_POLYGON_FILL_MODE_COUNT] =
{
	"EPFM_SOLID",
	"EPFM_WIREFRAME",
    "EPFM_POINT",
};

static const c8* PolygonFillModeReadableNames[E_POLYGON_FILL_MODE_COUNT] =
{
	"Solid",
	"Wireframe",
    "Point",
};

MY_FORCEINLINE const c8 *getPolygonFillModeName(E_POLYGON_FILL_MODE type)
{ return PolygonFillModeNames[type]; }

MY_FORCEINLINE const c8 *getPolygonFillModeReadableName(E_POLYGON_FILL_MODE type)
{ return PolygonFillModeReadableNames[type]; }

//----------------------------------------------------------------------------

enum E_RENDER_BUFFER_TYPE
{
	ERBT_DYNAMIC = 0,
    ERBT_STATIC,

	E_RENDER_BUFFER_TYPE_COUNT,

	E_RENDER_BUFFER_TYPE_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_RENDER_BUFFER_LOCK_MODE
{
	ERBLM_WRITE_ONLY,
	ERBLM_READ_ONLY,
	ERBLM_READ_WRITE,

	E_RENDER_BUFFER_LOCK_MODE_COUNT,

	E_RENDER_BUFFER_LOCK_MODE_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! Comparison tests used for the depth/stencil buffer operations and 
//! others. 
enum E_COMPARE_TEST
{
    ECT_ALWAYS = 0,
	ECT_NEVER,
    ECT_LESS,
    ECT_LEQUAL,
    ECT_EQUAL,
    ECT_NOTEQUAL,
    ECT_GREATER,
    ECT_GEQUAL,

	//! For counting enum
	E_COMPARE_TEST_COUNT,

	E_COMPARE_TEST_FORCE_32_BIT = 0xffffffff
};

static const c8* CompareTestName[E_COMPARE_TEST_COUNT] =
{
	"ECT_ALWAYS",
	"ECT_NEVER",
	"ECT_LESS",
	"ECT_LEQUAL",
	"ECT_EQUAL",
	"ECT_NOTEQUAL",
	"ECT_GREATER",
	"ECT_GEQUAL"
};

static const c8* CompareTestReadableName[E_COMPARE_TEST_COUNT] =
{
	"Always",
	"Never",
	"Less",
	"LEqual",
	"Equal",
	"NotEqual",
	"Greate",
	"GEqual"
};

MY_FORCEINLINE const c8* getCompareTestName(E_COMPARE_TEST test)
{ return CompareTestName[test]; }

MY_FORCEINLINE const c8* getCompareTestReadableName(E_COMPARE_TEST test)
{ return CompareTestReadableName[test]; }

//----------------------------------------------------------------------------

enum E_STENCIL_OPERATION
{
	ESO_KEEP = 0,
    ESO_ZERO,
    ESO_REPLACE,
    ESO_INCR_WRAP,
    ESO_DECR_WRAP,
    ESO_INVERT,
    ESO_INCR,
    ESO_DECR,

	E_STENCIL_OPERATION_COUNT,

	E_STENCIL_OPERATION_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_RENDER_PASS
{
	ERP_3D_SKY_PASS = 0,
	ERP_3D_SOLID_PASS,
	ERP_3D_LIGHTING_PASS,
	ERP_3D_DIRT_PASS,
	ERP_3D_TRANSP_1ST_PASS,
	ERP_3D_TRANSP_2ND_PASS,
	ERP_3D_TRANSP_3RD_PASS,	
	ERP_2D_PASS,
	ERP_GUI_3D_SOLID_PASS,
	ERP_GUI_3D_TRANSP_PASS,
	ERP_GUI_2D_PASS,

	E_RENDER_PASS_COUNT,

	E_RENDER_PASS_FORCE_32BIT = 0xFFFFFFFF
};

static const c8* RenderPassName[E_RENDER_PASS_COUNT] =
{
	"ERP_3D_SKY_PASS",
	"ERP_3D_SOLID_PASS",
	"ERP_3D_LIGHTING_PASS",
	"ERP_3D_DIRT_PASS",
	"ERP_3D_TRANSP_1ST_PASS",
	"ERP_3D_TRANSP_2ND_PASS",
	"ERP_3D_TRANSP_3RD_PASS",
	"ERP_2D_PASS",
	"ERP_GUI_3D_SOLID_PASS",
	"ERP_GUI_3D_TRANSP_PASS",
	"ERP_GUI_2D_PASS",
};

static const c8* RenderPassReadableName[E_RENDER_PASS_COUNT] =
{
	"3D Sky pass",
	"3D Solid pass",
	"3D Lighting pass",
	"3D Dirt pass",
	"3D Transp.pass 1st",
	"3D Transp.pass 2nd",
	"3D Transp.pass 3rd",
	"2D pass",
	"GUI 3D Solid pass",
	"GUI 3D Transp.pass",
	"GUI 2D pass",
};

MY_FORCEINLINE const c8* getRenderPassName(E_RENDER_PASS pass)
{ return RenderPassName[pass]; }

MY_FORCEINLINE const c8* getRenderPassReadableName(E_RENDER_PASS pass)
{ return RenderPassReadableName[pass]; }

//--------------------------------------------------------------------------

enum E_RENDER_MODE
{
	ERM_RENDER_STANDARD = 0,
	ERM_RENDER_INTO_STENCIL,
	ERM_RENDER_INTO_STENCIL_SHADOW_VOLUME_ZPASS,
	ERM_RENDER_INTO_STENCIL_SHADOW_VOLUME_ZFAIL,
	ERM_RENDER_WITH_STENCIL_TEST_SHADOW_GROUND_COLOR,
	ERM_RENDER_WITH_STENCIL_TEST_SHADOW_GROUND_ALPHA,
	ERM_RENDER_WITH_STENCIL_TEST_LIGHTING,

	E_RENDER_MODE_COUNT,

	E_RENDER_MODE_FORCE_32_BIT = 0xffffffff
};

static const c8* RenderModeName[E_RENDER_MODE_COUNT] =
{
	"ERM_RENDER_STANDARD",
	"ERM_RENDER_INTO_STENCIL",
	"ERM_RENDER_INTO_STENCIL_SHADOW_VOLUME_ZPASS",
	"ERM_RENDER_INTO_STENCIL_SHADOW_VOLUME_ZFAIL",
	"ERM_RENDER_WITH_STENCIL_TEST_SHADOW_GROUND_COLOR",
	"ERM_RENDER_WITH_STENCIL_TEST_SHADOW_GROUND_ALPHA",
	"ERM_RENDER_WITH_STENCIL_TEST_LIGHTING",
};

static const c8* RenderModeReadableName[E_RENDER_MODE_COUNT] =
{
	"Standard",
	"Into stencil",
	"Into stencil SHADOW VOLUME zpass",
	"Into stencil SHADOW VOLUME zfail",
	"With stencil test SHADOW GROUND color",
	"With stencil test SHADOW GROUND alpha",
	"With stencil test LIGHTING",
};

MY_FORCEINLINE const c8* getRenderModeName(E_RENDER_MODE mode)
{ return RenderModeName[mode]; }

MY_FORCEINLINE const c8* getRenderPassReadableName(E_RENDER_MODE mode)
{ return RenderModeReadableName[mode]; }

//----------------------------------------------------------------------------

enum E_LIGHTING_MODE
{
	ELM_NONE = 0,
	ELM_8_LIGHTS,

	E_LIGHTING_MODE_COUNT,

	E_LIGHTING_MODE_FORCE_32_BIT = 0xffffffff
};

//---------------------------------------------------------------------------

//! Alpha test mode
enum E_ALPHA_TEST
{	
    EAT_ALWAYS = 0, 
	EAT_GT_0,
	EAT_LT_80,
	EAT_GE_80,		

	E_ALPHA_TEST_COUNT,

	E_ALPHA_TEST_FORCE_32_BIT = 0xffffffff
};

static const c8* AlphaTestName[E_ALPHA_TEST_COUNT] =
{
	"EAT_ALWAYS",
	"EAT_GT_0",
	"EAT_LT_80",
	"EAT_GE_80"
};

static const c8* AlphaTestReadableName[E_ALPHA_TEST_COUNT] =
{
	"Always",
	"> 0",
	"< 128",
	">= 128"
};

MY_FORCEINLINE const c8* getAlphaTestName(E_ALPHA_TEST test)
{ return AlphaTestName[test]; }

MY_FORCEINLINE const c8* getAlphaTestReadableName(E_ALPHA_TEST test)
{ return AlphaTestReadableName[test]; }

//---------------------------------------------------------------------------

struct SAlphaTestSetup
{
	E_COMPARE_TEST	func;
	f32				ref;
};

static SAlphaTestSetup AlphaTestSetups[E_ALPHA_TEST_COUNT] =
{
	{ECT_ALWAYS,	0.0f},
	{ECT_GREATER,	0.0f},
	{ECT_LESS,		0.5f},
	{ECT_GEQUAL,	0.5f},
};

//---------------------------------------------------------------------------

enum E_DEFAULT_TEXTURE
{
	EDT_LIGHT_SPHERE,
	EDT_LIGHT_GRADIENT,
	EDT_LIGHT_SPHERE_WHITE,
	EDT_LIGHT_WHITE_GRADIENT,
	EDT_WHITE_WITH_BORDER,
	EDT_ENVIRONMENT_MAP,
	EDT_DEFAULT_TEXTURE,

	E_DEFAULT_TEXTURE_COUNT,

	E_DEFAULT_TEXTURE_FORCE_32_BIT = 0xffffffff
};

static const c8* DefaultTextureName[E_DEFAULT_TEXTURE_COUNT] =
{
	"EDT_LIGHT_SPHERE",
	"EDT_LIGHT_GRADIENT",
	"EDT_LIGHT_SPHERE_WHITE",
	"EDT_LIGHT_WHITE_GRADIENT",
	"EDT_WHITE_WITH_BORDER",
	"EDT_ENVIRONMENT_MAP",
	"EDT_DEFAULT_TEXTURE",
};

static const c8* DefaultTextureReadableName[E_DEFAULT_TEXTURE_COUNT] =
{
	"#DefaultLightSphere",
	"#DefaultLightGradient",
	"#DefaultLightSphereWhite",
	"#DefaultLightWhiteGradient",
	"#DefaultWhiteWithBorder",
	"#DefaultEnvironmentMap",
	"#DefaultTexture",
};

MY_FORCEINLINE const c8* getDefaultTextureName(E_DEFAULT_TEXTURE tex)
{ return DefaultTextureName[tex]; }

MY_FORCEINLINE const c8* getDefaultTextureReadableName(E_DEFAULT_TEXTURE tex)
{ return DefaultTextureReadableName[tex]; }

//---------------------------------------------------------------------------

//! Vertex shader version enums
enum E_VERTEX_SHADER_VERSION
{
	EVSV_HLSL_1_1 = 0,
	EVSV_HLSL_2_0,
	EVSV_HLSL_2_a,
	EVSV_HLSL_3_0,
	EVSV_HLSL_4_0,
	EVSV_HLSL_4_1,
	EVSV_HLSL_5_0,
	EVSV_GLSL_1_0,
	EVSV_GLSL_1_1,
	EVSV_GLSL_1_2,
	EVSV_GLSL_1_3,
	EVSV_GLSL_1_4,
	EVSV_GLSL_1_5,
	EVSV_GLSL_3_3,
	EVSV_GLSL_4_0,
	EVSV_GLSL_4_1,

	E_VERTEX_SHADER_VERSION_COUNT,
	E_VERTEX_SHADER_VERSION_FORCE_32_BIT = 0xffffffff
};

static const c8 *VertexShaderVersionName[E_VERTEX_SHADER_VERSION_COUNT] =
{
	"EVSV_HLSL_1_1",
	"EVSV_HLSL_2_0",
	"EVSV_HLSL_2_a",
	"EVSV_HLSL_3_0",
	"EVSV_HLSL_4_0",
	"EVSV_HLSL_4_1",
	"EVSV_HLSL_5_0",
	"EVSV_GLSL_1_0",
	"EVSV_GLSL_1_1",
	"EVSV_GLSL_1_2",
	"EVSV_GLSL_1_3",
	"EVSV_GLSL_1_4",
	"EVSV_GLSL_1_5",
	"EVSV_GLSL_3_3",
	"EVSV_GLSL_4_0",
	"EVSV_GLSL_4_1",
};

static const c8 *VertexShaderVersionReadableName[E_VERTEX_SHADER_VERSION_COUNT] =
{
	"HLSL 1.1",
	"HLSL 2.0",
	"HLSL 2.0a",
	"HLSL 3.0",
	"HLSL 4.0",
	"HLSL 4.1",
	"HLSL 5.0",
	"GLSL 1.0",
	"GLSL 1.1",
	"GLSL 1.2",
	"GLSL 1.3",
	"GLSL 1.4",
	"GLSL 1.5",
	"GLSL 3.3",
	"GLSL 4.0",
	"GLSL 4.1",
};

static const c8* VertexShaderVersionTargetName[E_VERTEX_SHADER_VERSION_COUNT] =
{
	"vs_1_1",
	"vs_2_0",
	"vs_2_a",
	"vs_3_0",
	"vs_4_0",
	"vs_4_1",
	"vs_5_0",
	"100",
	"110",
	"120",
	"130",
	"140",
	"150",
	"330"
	"400",
	"410",
};

MY_FORCEINLINE const c8* getVertexShaderVersionName(E_VERTEX_SHADER_VERSION ver)
{ return VertexShaderVersionName[ver]; }

MY_FORCEINLINE const c8* getVertexShaderVersionReadableName(E_VERTEX_SHADER_VERSION ver)
{ return VertexShaderVersionReadableName[ver]; }

//---------------------------------------------------------------------------

//! Pixel shader version enums
enum E_PIXEL_SHADER_VERSION
{
	EPSV_HLSL_1_1 = 0,
	EPSV_HLSL_1_2,
	EPSV_HLSL_1_3,
	EPSV_HLSL_1_4,
	EPSV_HLSL_2_0,
	EPSV_HLSL_2_a,
	EPSV_HLSL_2_b,
	EPSV_HLSL_3_0,
	EPSV_HLSL_4_0,
	EPSV_HLSL_4_1,
	EPSV_HLSL_5_0,
	EPSV_GLSL_1_0,
	EPSV_GLSL_1_1,
	EPSV_GLSL_1_2,
	EPSV_GLSL_1_3,
	EPSV_GLSL_1_4,
	EPSV_GLSL_1_5,
	EPSV_GLSL_3_3,
	EPSV_GLSL_4_0,
	EPSV_GLSL_4_1,

	E_PIXEL_SHADER_VERSION_COUNT,
	E_PIXEL_SHADER_VERSION_FORCE_32_BIT = 0xffffffff
};

static const c8 *PixelShaderVersionName[E_PIXEL_SHADER_VERSION_COUNT] =
{
	"EPSV_HLSL_1_1",
	"EPSV_HLSL_1_2",
	"EPSV_HLSL_1_3",
	"EPSV_HLSL_1_4",
	"EPSV_HLSL_2_0",
	"EPSV_HLSL_2_a",
	"EPSV_HLSL_2_b",
	"EPSV_HLSL_3_0",
	"EPSV_HLSL_4_0",
	"EPSV_HLSL_4_1",
	"EPSV_HLSL_5_0",
	"EPSV_GLSL_1_0",
	"EPSV_GLSL_1_1",
	"EPSV_GLSL_1_2",
	"EPSV_GLSL_1_3",
	"EPSV_GLSL_1_4",
	"EPSV_GLSL_1_5",
	"EPSV_GLSL_3_3",
	"EPSV_GLSL_4_0",
	"EPSV_GLSL_4_1",
};

static const c8 *PixelShaderVersionReadableName[E_PIXEL_SHADER_VERSION_COUNT] =
{
	"HLSL 1.1",
	"HLSL 1.2",
	"HLSL 1.3",
	"HLSL 1.4",
	"HLSL 2.0",
	"HLSL 2.0a",
	"HLSL 2.0b",
	"HLSL 3.0",
	"HLSL 4.0",
	"HLSL 4.1",
	"HLSL 5.0",
	"GLSL 1.0",
	"GLSL 1.1",
	"GLSL 1.2",
	"GLSL 1.3",
	"GLSL 1.4",
	"GLSL 1.5",
	"GLSL 3.3",
	"GLSL 4.0",
	"GLSL 4.1",
};

static const c8* PixelShaderVersionTargetName[E_PIXEL_SHADER_VERSION_COUNT] =
{
	"ps_1_1",
	"ps_1_2",
	"ps_1_3",
	"ps_1_4",
	"ps_2_0",
	"ps_2_a",
	"ps_2_b",
	"ps_3_0",
	"ps_4_0",
	"ps_4_1",
	"ps_5_0",
	"100",
	"110",
	"120",
	"130",
	"140",
	"150",
	"330"
	"400",
	"410",
};

MY_FORCEINLINE const c8* getPixelShaderVersionName(E_PIXEL_SHADER_VERSION ver)
{ return PixelShaderVersionName[ver]; }

MY_FORCEINLINE const c8* getPixelShaderVersionReadableName(E_PIXEL_SHADER_VERSION ver)
{ return PixelShaderVersionReadableName[ver]; }

//----------------------------------------------------------------------------

enum E_RENDER_TARGET_CREATION_FLAG
{
	ERTCF_DEPTH16  = (1)<<0,  // 16-bit depth buffer
	ERTCF_DEPTH24  = (1)<<1,  // 24-bit depth buffer
	ERTCF_DEPTH32  = (1)<<2,  // 32-bit depth buffer

	ERTCF_STENCIL8 = (1)<<3,  // 8-bit stencil buffer

	E_RENDER_TARGET_CREATION_FLAG_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef my_const_vidHPP