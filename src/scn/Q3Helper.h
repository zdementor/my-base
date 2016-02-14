//|-------------------------------------------------------------------------
//| File:        Q3Helper.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef Q3HelperHPP
#define Q3HelperHPP

//---------------------------------------------------------------------------

#include <my_inc.h>
#include <vid/SMaterial.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
namespace q3 {
//---------------------------------------------------------------------------

#define qboolean bool
#define qfalse   false
#define qtrue    true

#define	MAX_QPATH				64		// max length of a quake game pathname
#define	MAX_TOKEN_CHARS			1024	// max length of an individual token
#define	MAX_IMAGE_ANIMATIONS	8
#define NUM_TEXTURE_BUNDLES		2
#define	MAX_SHADER_DEFORMS		3
#define MAX_SHADER_STAGES		8
#define MAX_STATES_PER_SHADER	32

#define GLS_SRCBLEND_ZERO					0x00000001
#define GLS_SRCBLEND_ONE					0x00000002
#define GLS_SRCBLEND_DST_COLOR				0x00000003
#define GLS_SRCBLEND_ONE_MINUS_DST_COLOR	0x00000004
#define GLS_SRCBLEND_SRC_ALPHA				0x00000005
#define GLS_SRCBLEND_ONE_MINUS_SRC_ALPHA	0x00000006
#define GLS_SRCBLEND_DST_ALPHA				0x00000007
#define GLS_SRCBLEND_ONE_MINUS_DST_ALPHA	0x00000008
#define GLS_SRCBLEND_ALPHA_SATURATE			0x00000009
#define	GLS_SRCBLEND_BITS					0x0000000f

#define GLS_DSTBLEND_ZERO					0x00000010
#define GLS_DSTBLEND_ONE					0x00000020
#define GLS_DSTBLEND_SRC_COLOR				0x00000030
#define GLS_DSTBLEND_ONE_MINUS_SRC_COLOR	0x00000040
#define GLS_DSTBLEND_SRC_ALPHA				0x00000050
#define GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA	0x00000060
#define GLS_DSTBLEND_DST_ALPHA				0x00000070
#define GLS_DSTBLEND_ONE_MINUS_DST_ALPHA	0x00000080
#define	GLS_DSTBLEND_BITS					0x000000f0

#define GLS_DEPTHMASK_TRUE					0x00000100

#define GLS_POLYMODE_LINE					0x00001000

#define GLS_DEPTHTEST_DISABLE				0x00010000
#define GLS_DEPTHFUNC_EQUAL					0x00020000

#define GLS_ATEST_GT_0						0x10000000
#define GLS_ATEST_LT_80						0x20000000
#define GLS_ATEST_GE_80						0x40000000
#define	GLS_ATEST_BITS						0x70000000

#define GLS_DEFAULT			GLS_DEPTHMASK_TRUE

#define TR_MAX_TEXMODS 4

#define	VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))

#ifndef M_PI
#define M_PI		3.14159265358979323846f	// matches value in gcc v2 math.h
#endif

#define	CONTENTS_SOLID			1		// an eye is never valid in a solid
#define	CONTENTS_LAVA			8
#define	CONTENTS_SLIME			16
#define	CONTENTS_WATER			32
#define	CONTENTS_FOG			64

#define CONTENTS_NOTTEAM1		0x0080
#define CONTENTS_NOTTEAM2		0x0100
#define CONTENTS_NOBOTCLIP		0x0200

#define	CONTENTS_AREAPORTAL		0x8000

#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000
//bot specific contents types
#define	CONTENTS_TELEPORTER		0x40000
#define	CONTENTS_JUMPPAD		0x80000
#define CONTENTS_CLUSTERPORTAL	0x100000
#define CONTENTS_DONOTENTER		0x200000
#define CONTENTS_BOTCLIP		0x400000
#define CONTENTS_MOVER			0x800000

#define	CONTENTS_ORIGIN			0x1000000	// removed before bsping an entity

#define	CONTENTS_BODY			0x2000000	// should never be on a brush, only in game
#define	CONTENTS_CORPSE			0x4000000
#define	CONTENTS_DETAIL			0x8000000	// brushes not used for the bsp
#define	CONTENTS_STRUCTURAL		0x10000000	// brushes used for the bsp
#define	CONTENTS_TRANSLUCENT	0x20000000	// don't consume surface fragments inside
#define	CONTENTS_TRIGGER		0x40000000
#define	CONTENTS_NODROP			0x80000000	// don't leave bodies or items (death fog, lava)

#define	SURF_NODAMAGE			0x1		// never give falling damage
#define	SURF_SLICK				0x2		// effects game physics
#define	SURF_SKY				0x4		// lighting from environment map
#define	SURF_LADDER				0x8
#define	SURF_NOIMPACT			0x10	// don't make missile explosions
#define	SURF_NOMARKS			0x20	// don't leave missile marks
#define	SURF_FLESH				0x40	// make flesh sounds and effects
#define	SURF_NODRAW				0x80	// don't generate a drawsurface at all
#define	SURF_HINT				0x100	// make a primary bsp splitter
#define	SURF_SKIP				0x200	// completely ignore, allowing non-closed brushes
#define	SURF_NOLIGHTMAP			0x400	// surface doesn't need a lightmap
#define	SURF_POINTLIGHT			0x800	// generate lighting info at vertexes
#define	SURF_METALSTEPS			0x1000	// clanking footsteps
#define	SURF_NOSTEPS			0x2000	// no footstep sounds
#define	SURF_NONSOLID			0x4000	// don't collide against curves with this set
#define	SURF_LIGHTFILTER		0x8000	// act as a light filter during q3map -light
#define	SURF_ALPHASHADOW		0x10000	// do per-pixel light shadow casting in q3map
#define	SURF_NODLIGHT			0x20000	// don't dlight even if solid (solid lava, skies)
#define SURF_DUST				0x40000 // leave a dust trail when walking on this surface

#define LIGHTMAP_2D			-4		// shader is for 2D rendering
#define LIGHTMAP_BY_VERTEX	-3		// pre-lit triangle models
#define LIGHTMAP_WHITEIMAGE	-2
#define	LIGHTMAP_NONE		-1

//---------------------------------------------------------------------------

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

typedef enum {
	SS_BAD,
	SS_PORTAL,			// mirrors, portals, viewscreens
	SS_ENVIRONMENT,		// sky box
	SS_OPAQUE,			// opaque

	SS_DECAL,			// scorch marks, etc.
	SS_SEE_THROUGH,		// ladders, grates, grills that may have small blended edges
						// in addition to alpha test
	SS_BANNER,

	SS_FOG,

	SS_UNDERWATER,		// for items that should be drawn in front of the water plane

	SS_BLEND0,			// regular transparency and filters
	SS_BLEND1,			// generally only used for additive type effects
	SS_BLEND2,
	SS_BLEND3,

	SS_BLEND6,
	SS_STENCIL_SHADOW,
	SS_ALMOST_NEAREST,	// gun smoke puffs

	SS_NEAREST			// blood blobs
} shaderSort_t;

typedef enum {
	TCGEN_BAD,
	TCGEN_IDENTITY,			// clear to 0,0
	TCGEN_LIGHTMAP,
	TCGEN_TEXTURE,
	TCGEN_ENVIRONMENT_MAPPED,
	TCGEN_FOG,
	TCGEN_VECTOR			// S and T from world coordinates
} texCoordGen_t;

typedef enum {
	TMOD_NONE,
	TMOD_TRANSFORM,
	TMOD_TURBULENT,
	TMOD_SCROLL,
	TMOD_SCALE,
	TMOD_STRETCH,
	TMOD_ROTATE,
	TMOD_ENTITY_TRANSLATE
} texMod_t;

typedef enum {
	ACFF_NONE,
	ACFF_MODULATE_RGB,
	ACFF_MODULATE_RGBA,
	ACFF_MODULATE_ALPHA
} acff_t;

typedef enum {
	GF_NONE,

	GF_SIN,
	GF_SQUARE,
	GF_TRIANGLE,
	GF_SAWTOOTH, 
	GF_INVERSE_SAWTOOTH, 

	GF_NOISE

} genFunc_t;

typedef enum {
	CGEN_BAD,
	CGEN_IDENTITY_LIGHTING,	// tr.identityLight
	CGEN_IDENTITY,			// always (1,1,1,1)
	CGEN_ENTITY,			// grabbed from entity's modulate field
	CGEN_ONE_MINUS_ENTITY,	// grabbed from 1 - entity.modulate
	CGEN_EXACT_VERTEX,		// tess.vertexColors
	CGEN_VERTEX,			// tess.vertexColors * tr.identityLight
	CGEN_ONE_MINUS_VERTEX,
	CGEN_WAVEFORM,			// programmatically generated
	CGEN_LIGHTING_DIFFUSE,
	CGEN_FOG,				// standard fog
	CGEN_CONST				// fixed color
} colorGen_t;

typedef enum {
	AGEN_IDENTITY,
	AGEN_SKIP,
	AGEN_ENTITY,
	AGEN_ONE_MINUS_ENTITY,
	AGEN_VERTEX,
	AGEN_ONE_MINUS_VERTEX,
	AGEN_LIGHTING_SPECULAR,
	AGEN_WAVEFORM,
	AGEN_PORTAL,
	AGEN_CONST
} alphaGen_t;

typedef enum {
	CT_FRONT_SIDED,
	CT_BACK_SIDED,
	CT_TWO_SIDED
} cullType_t;

typedef enum {
	DEFORM_NONE,
	DEFORM_WAVE,
	DEFORM_NORMALS,
	DEFORM_BULGE,
	DEFORM_MOVE,
	DEFORM_PROJECTION_SHADOW,
	DEFORM_AUTOSPRITE,
	DEFORM_AUTOSPRITE2,
	DEFORM_TEXT0,
	DEFORM_TEXT1,
	DEFORM_TEXT2,
	DEFORM_TEXT3,
	DEFORM_TEXT4,
	DEFORM_TEXT5,
	DEFORM_TEXT6,
	DEFORM_TEXT7
} deform_t;

typedef enum {
	FP_NONE,		// surface is translucent and will just be adjusted properly
	FP_EQUAL,		// surface is opaque but possibly alpha tested
	FP_LE			// surface is trnaslucent, but still needs a fog pass (fog surface)
} fogPass_t;

typedef struct {
	genFunc_t	func;

	float base;
	float amplitude;
	float phase;
	float frequency;
} waveForm_t;

typedef struct {
	deform_t	deformation;			// vertex coordinate modification type

	vec3_t		moveVector;
	waveForm_t	deformationWave;
	float		deformationSpread;

	float		bulgeWidth;
	float		bulgeHeight;
	float		bulgeSpeed;
} deformStage_t;

typedef struct {
	texMod_t		type;

	// used for TMOD_TURBULENT and TMOD_STRETCH
	waveForm_t		wave;

	// used for TMOD_TRANSFORM
	float			matrix[2][2];		// s' = s * m[0][0] + t * m[1][0] + trans[0]
	float			translate[2];		// t' = s * m[0][1] + t * m[0][1] + trans[1]

	// used for TMOD_SCALE
	float			scale[2];			// s *= scale[0]
	                                    // t *= scale[1]

	// used for TMOD_SCROLL
	float			scroll[2];			// s' = s + scroll[0] * time
										// t' = t + scroll[1] * time

	// + = clockwise
	// - = counterclockwise
	float			rotateSpeed;

} texModInfo_t;

typedef class vid::ITexture image_t;

struct textureBundle_t
{
	image_t			*image[MAX_IMAGE_ANIMATIONS];
	int				numImageAnimations;
	float			imageAnimationSpeed;

	texCoordGen_t	tcGen;
	vec3_t			tcGenVectors[2];

	int				numTexMods;
	texModInfo_t	*texMods;

	int				videoMapHandle;
	qboolean		isLightmap;
	qboolean		vertexLightmap;
	qboolean		isVideoMap;	

	bool            isClampMap;
};

typedef struct {
	qboolean		active;
	
	textureBundle_t	bundle[NUM_TEXTURE_BUNDLES];

	waveForm_t		rgbWave;
	colorGen_t		rgbGen;

	waveForm_t		alphaWave;
	alphaGen_t		alphaGen;

	byte			constantColor[4];			// for CGEN_CONST and AGEN_CONST

	unsigned		stateBits;					// GLS_xxxx mask

	acff_t			adjustColorsForFog;

	qboolean		isDetail;
} shaderStage_t;

typedef struct {
	float		cloudHeight;
	image_t		*outerbox[6], *innerbox[6];
} skyParms_t;

typedef struct {
	vec3_t	color;
	float	depthForOpaque;
} fogParms_t;

struct shader_t 
{
	shader_t()
	{
		for (s32 st=0; st<MAX_SHADER_STAGES; st++)
		{
			stages[st]=0;
		}
	}

	void free()
	{
		for (s32 st=0; st<MAX_SHADER_STAGES; st++)
		{
			if (stages[st])
			{
				for (s32 tb=0; tb<NUM_TEXTURE_BUNDLES; tb++)
				{
					SAFE_DELETE_ARRAY(stages[st]->bundle[tb].texMods);
				}
			}
			SAFE_DELETE(stages[st]);
		}
	}

	char		name[MAX_QPATH];		// game path, including extension
	int			lightmapIndex;			// for a shader to match, both name and lightmapIndex must match

	int			index;					// this shader == tr.shaders[index]
	int			sortedIndex;			// this shader == tr.sortedShaders[sortedIndex]

	float		sort;					// lower numbered shaders draw before higher numbered

	qboolean	defaultShader;			// we want to return index 0 if the shader failed to
										// load for some reason, but R_FindShader should
										// still keep a name allocated for it, so if
										// something calls RE_RegisterShader again with
										// the same name, we don't try looking for it again

	qboolean	explicitlyDefined;		// found in a .shader file

	int			surfaceFlags;			// if explicitlyDefined, this will have SURF_* flags
	int			contentFlags;

	qboolean	entityMergable;			// merge across entites optimizable (smoke, blood)

	qboolean	isSky;
	skyParms_t	sky;
	fogParms_t	fogParms;

	float		portalRange;			// distance to fog out at

	int			multitextureEnv;		// 0, GL_MODULATE, GL_ADD (FIXME: put in stage)

	cullType_t	cullType;				// CT_FRONT_SIDED, CT_BACK_SIDED, or CT_TWO_SIDED
	qboolean	polygonOffset;			// set for decals and other items that must be offset 
	qboolean	noMipMaps;				// for console fonts, 2D elements, etc.
	qboolean	noPicMip;				// for images that must always be full resolution

	fogPass_t	fogPass;				// draw a blended pass, possibly with depth test equals

	qboolean	needsNormal;			// not all shaders will need all data to be gathered
	qboolean	needsST1;
	qboolean	needsST2;
	qboolean	needsColor;

	int			numDeforms;
	deformStage_t	deforms[MAX_SHADER_DEFORMS];

	int			numUnfoggedPasses;
	shaderStage_t	*stages[MAX_SHADER_STAGES];		

	void		(*optimalStageiteratorFunc)( void );

	float clampTime;                                  // time this shader is clamped to
	float timeOffset;                                 // current time offset for this shader

	int numStates;                                    // if non-zero this is a state shader
	struct shader_s *currentShader;                   // current state if this is a state shader
	struct shader_s *parentShader;                    // current state if this is a state shader
	int currentState;                                 // current state index for cycle purposes
	long expireTime;                                  // time in milliseconds this expires

	struct shader_s *remappedShader;                  // current shader this one is remapped too

	int shaderStates[MAX_STATES_PER_SHADER];          // index to valid shader states

	struct	shader_s	*next;
};

typedef struct {
	
	// ...

	vec3_t					sunLight;			// from the sky shader for this level
	vec3_t					sunDirection;

	// ...

} trGlobals_t;

static trGlobals_t tr;

//---------------------------------------------------------------------------

int Q_stricmpn (const char *s1, const char *s2, int n);
int Q_stricmp  (const char *s1, const char *s2);

char *COM_ParseExt( char **data_p, qboolean allowLineBreaks );
int   COM_Compress( char *data_p );
void  COM_StripExtension( const char *in, char *out );

void SkipBracedSection (char **program);
void SkipRestOfLine ( char **data );

vec_t VectorNormalize( vec3_t v );

unsigned NameToAFunc( const char *funcname );

int NameToSrcBlendMode( const char *name );
vid::E_SRC_BLEND_FUNCTION Q3ToMySrcBlendMode(int mode);

int NameToDstBlendMode( const char *name );
vid::E_DST_BLEND_FUNCTION Q3ToMyDstBlendMode(int mode);

genFunc_t NameToGenFunc( const char *funcname );

//---------------------------------------------------------------------------
} // end namespace q3
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // Q3HelperHPP