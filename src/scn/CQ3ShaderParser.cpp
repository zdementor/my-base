//|-------------------------------------------------------------------------
//| File:        CQ3ShaderParser.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CQ3ShaderParser.h"

#include <io/ILogger.h>
#include <io/IFileSystem.h>
#include <io/IFileList.h>
#include <vid/IVideoDriver.h>
#include <vid/ITextureAnim.h>

#include "CMaterialsManager.h"
#include "CQ3LevelMesh.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
namespace q3 {
//---------------------------------------------------------------------------

static c8 ch[255];

//---------------------------------------------------------------------------

// the shader is parsed into these global variables
static	shaderStage_t	stages[MAX_SHADER_STAGES];		
static	shader_t		shader;
static	texModInfo_t	texMods[MAX_SHADER_STAGES][TR_MAX_TEXMODS];

//---------------------------------------------------------------------------

typedef struct 
{
	char	*name;
	int		clearSolid, surfaceFlags, contents;
} 
infoParm_t;

//---------------------------------------------------------------------------

infoParm_t	infoParms[] = {
	// server relevant contents
	{"water",		1,	0,	CONTENTS_WATER },
	{"slime",		1,	0,	CONTENTS_SLIME },		// mildly damaging
	{"lava",		1,	0,	CONTENTS_LAVA },		// very damaging
	{"playerclip",	1,	0,	CONTENTS_PLAYERCLIP },
	{"monsterclip",	1,	0,	CONTENTS_MONSTERCLIP },
	{"nodrop",		1,	0,	CONTENTS_NODROP },		// don't drop items or leave bodies (death fog, lava, etc)
	{"nonsolid",	1,	SURF_NONSOLID,	0},						// clears the solid flag

	// utility relevant attributes
	{"origin",		1,	0,	CONTENTS_ORIGIN },		// center of rotating brushes
	{"trans",		0,	0,	CONTENTS_TRANSLUCENT },	// don't eat contained surfaces
	{"detail",		0,	0,	CONTENTS_DETAIL },		// don't include in structural bsp
	{"structural",	0,	0,	CONTENTS_STRUCTURAL },	// force into structural bsp even if trnas
	{"areaportal",	1,	0,	CONTENTS_AREAPORTAL },	// divides areas
	{"clusterportal", 1,0,  CONTENTS_CLUSTERPORTAL },	// for bots
	{"donotenter",  1,  0,  CONTENTS_DONOTENTER },		// for bots

	{"fog",			1,	0,	CONTENTS_FOG},			// carves surfaces entering
	{"sky",			0,	SURF_SKY,		0 },		// emit light from an environment map
	{"lightfilter",	0,	SURF_LIGHTFILTER, 0 },		// filter light going through it
	{"alphashadow",	0,	SURF_ALPHASHADOW, 0 },		// test light on a per-pixel basis
	{"hint",		0,	SURF_HINT,		0 },		// use as a primary splitter

	// server attributes
	{"slick",		0,	SURF_SLICK,		0 },
	{"noimpact",	0,	SURF_NOIMPACT,	0 },		// don't make impact explosions or marks
	{"nomarks",		0,	SURF_NOMARKS,	0 },		// don't make impact marks, but still explode
	{"ladder",		0,	SURF_LADDER,	0 },
	{"nodamage",	0,	SURF_NODAMAGE,	0 },
	{"metalsteps",	0,	SURF_METALSTEPS,0 },
	{"flesh",		0,	SURF_FLESH,		0 },
	{"nosteps",		0,	SURF_NOSTEPS,	0 },

	// drawsurf attributes
	{"nodraw",		0,	SURF_NODRAW,	0 },	// don't generate a drawsurface (or a lightmap)
	{"pointlight",	0,	SURF_POINTLIGHT, 0 },	// sample lighting at vertexes
	{"nolightmap",	0,	SURF_NOLIGHTMAP,0 },	// don't generate a lightmap
	{"nodlight",	0,	SURF_NODLIGHT, 0 },		// don't ever add dynamic lights
	{"dust",		0,	SURF_DUST, 0}			// leave a dust trail when walking on this surface
};

//---------------------------------------------------------------------------

typedef struct 
{
	int		blendA;
	int		blendB;

	int		multitextureEnv;
	int		multitextureBlend;
} collapse_t;

//---------------------------------------------------------------------------

enum
{
_GL_MODULATE=0,
_GL_ADD,
_GL_DECAL
};

static collapse_t	collapse[] = 
{
	{ 0, 0,	_GL_MODULATE, 0 },

	{ 0, GLS_DSTBLEND_SRC_COLOR | GLS_SRCBLEND_ZERO,	
		_GL_MODULATE, 0 },

	{ 0, GLS_DSTBLEND_ZERO | GLS_SRCBLEND_DST_COLOR,
		_GL_MODULATE, 0 },

	{ GLS_SRCBLEND_ONE_MINUS_SRC_ALPHA | GLS_DSTBLEND_SRC_ALPHA, GLS_SRCBLEND_ONE_MINUS_SRC_ALPHA | GLS_DSTBLEND_SRC_ALPHA,
		_GL_MODULATE, GLS_SRCBLEND_ONE_MINUS_SRC_ALPHA | GLS_DSTBLEND_SRC_ALPHA },

	{ GLS_DSTBLEND_ZERO | GLS_SRCBLEND_DST_COLOR, GLS_DSTBLEND_ZERO | GLS_SRCBLEND_DST_COLOR,
		_GL_MODULATE, GLS_DSTBLEND_ZERO | GLS_SRCBLEND_DST_COLOR },

	{ GLS_DSTBLEND_SRC_COLOR | GLS_SRCBLEND_ZERO, GLS_DSTBLEND_ZERO | GLS_SRCBLEND_DST_COLOR,
		_GL_MODULATE, GLS_DSTBLEND_ZERO | GLS_SRCBLEND_DST_COLOR },

	{ GLS_DSTBLEND_ZERO | GLS_SRCBLEND_DST_COLOR, GLS_DSTBLEND_SRC_COLOR | GLS_SRCBLEND_ZERO,
		_GL_MODULATE, GLS_DSTBLEND_ZERO | GLS_SRCBLEND_DST_COLOR },

	{ GLS_DSTBLEND_SRC_COLOR | GLS_SRCBLEND_ZERO, GLS_DSTBLEND_SRC_COLOR | GLS_SRCBLEND_ZERO,
		_GL_MODULATE, GLS_DSTBLEND_ZERO | GLS_SRCBLEND_DST_COLOR },

	{ 0, GLS_DSTBLEND_ONE | GLS_SRCBLEND_ONE,
		_GL_ADD, 0 },

	{ GLS_DSTBLEND_ONE | GLS_SRCBLEND_ONE, GLS_DSTBLEND_ONE | GLS_SRCBLEND_ONE,
		_GL_ADD, GLS_DSTBLEND_ONE | GLS_SRCBLEND_ONE },

	{ GLS_SRCBLEND_SRC_ALPHA|GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA, GLS_SRCBLEND_SRC_ALPHA|GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA,
		_GL_ADD, GLS_SRCBLEND_SRC_ALPHA|GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA },

#if 0
	{ 0, GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA | GLS_SRCBLEND_SRC_ALPHA,
		GL_DECAL, 0 },
#endif
	{ -1 }
};

//--------------------------------------------------------

void ParseTexMod(char *_text, shaderStage_t *stage )
{
	const char *token;
	char **text = &_text;
	texModInfo_t *tmi;

	if ( stage->bundle[0].numTexMods == TR_MAX_TEXMODS ) 
	{
		sprintf(ch, "too many tcMod stages in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_ERROR);

		return;
	}

	tmi = &stage->bundle[0].texMods[stage->bundle[0].numTexMods];
	stage->bundle[0].numTexMods++;

	token = COM_ParseExt( text, qfalse );

	//
	// turb
	//
	if ( !Q_stricmp( token, "turb" ) )
	{
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing tcMod turb parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->wave.base = atof( token );
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing tcMod turb in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->wave.amplitude = atof( token );
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing tcMod turb in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->wave.phase = atof( token );
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing tcMod turb in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->wave.frequency = atof( token );

		tmi->type = TMOD_TURBULENT;
	}
	//
	// scale
	//
	else if ( !Q_stricmp( token, "scale" ) )
	{
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing scale parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->scale[0] = atof( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing scale parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->scale[1] = atof( token );
		tmi->type = TMOD_SCALE;
	}
	//
	// scroll
	//
	else if ( !Q_stricmp( token, "scroll" ) )
	{
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing scale scroll parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->scroll[0] = atof( token );
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing scale scroll parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->scroll[1] = atof( token );
		tmi->type = TMOD_SCROLL;
	}
	//
	// stretch
	//
	else if ( !Q_stricmp( token, "stretch" ) )
	{
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing stretch parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->wave.func = NameToGenFunc( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing stretch parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->wave.base = atof( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing stretch parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->wave.amplitude = atof( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing stretch parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->wave.phase = atof( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing stretch parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->wave.frequency = atof( token );
		
		tmi->type = TMOD_STRETCH;
	}
	//
	// transform
	//
	else if ( !Q_stricmp( token, "transform" ) )
	{
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing transform parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->matrix[0][0] = atof( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing transform parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->matrix[0][1] = atof( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing transform parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->matrix[1][0] = atof( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing transform parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->matrix[1][1] = atof( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing transform parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->translate[0] = atof( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing transform parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->translate[1] = atof( token );

		tmi->type = TMOD_TRANSFORM;
	}
	//
	// rotate
	//
	else if ( !Q_stricmp( token, "rotate" ) )
	{
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing tcMod rotate parms in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		tmi->rotateSpeed = atof( token );
		tmi->type = TMOD_ROTATE;
	}
	//
	// entityTranslate
	//
	else if ( !Q_stricmp( token, "entityTranslate" ) )
	{
		tmi->type = TMOD_ENTITY_TRANSLATE;
	}
	else
	{
		sprintf(ch, "unknown tcMod '%s' in shader '%s'", token, shader.name );
		LOGGER.log(ch, io::ELL_WARNING);
	}
}

//---------------------------------------------------------------------------

qboolean ParseVector(char **text, int count, float *v ) 
{
	char	*token;
	int		i;

	// FIXME: spaces are currently required after parens, should change parseext...
	token = COM_ParseExt( text, qfalse );
	if ( strcmp( token, "(" ) ) 
	{
		sprintf(ch, "missing parenthesis in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return qfalse;
	}

	for ( i = 0 ; i < count ; i++ ) 
	{
		token = COM_ParseExt( text, qfalse );
		if ( !token[0] ) 
		{
			sprintf(ch, "missing vector element in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return qfalse;
		}
		v[i] = atof( token );
	}

	token = COM_ParseExt( text, qfalse );
	if ( strcmp( token, ")" ) ) 
	{
		sprintf(ch, "missing parenthesis in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return qfalse;
	}

	return qtrue;
}

//---------------------------------------------------------------------------

void ParseWaveForm(char **text, waveForm_t *wave )
{
	char *token;

	token = COM_ParseExt( text, qfalse );
	if ( token[0] == 0 )
	{
		sprintf(ch, "missing waveform parm in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return;
	}
	wave->func = NameToGenFunc( token );

	// BASE, AMP, PHASE, FREQ
	token = COM_ParseExt( text, qfalse );
	if ( token[0] == 0 )
	{
		sprintf(ch, "missing waveform parm in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return;
	}
	wave->base = atof( token );

	token = COM_ParseExt( text, qfalse );
	if ( token[0] == 0 )
	{
		sprintf(ch, "missing waveform parm in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return;
	}
	wave->amplitude = atof( token );

	token = COM_ParseExt( text, qfalse );
	if ( token[0] == 0 )
	{
		sprintf(ch, "missing waveform parm in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return;
	}
	wave->phase = atof( token );

	token = COM_ParseExt( text, qfalse );
	if ( token[0] == 0 )
	{
		sprintf(ch, "missing waveform parm in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return;
	}
	wave->frequency = atof( token );
}

//---------------------------------------------------------------------------

static qboolean ParseStage( 
	shaderStage_t *stage, char **text, bool loadtex
	)
{
	char *token;
	int depthMaskBits = GLS_DEPTHMASK_TRUE, blendSrcBits = 0, blendDstBits = 0, atestBits = 0, depthFuncBits = 0;
	qboolean depthMaskExplicit = qfalse;

	stage->active = qtrue;

	while ( 1 )
	{
		token = COM_ParseExt( text, qtrue );
		if ( !token[0] )
		{
			LOGGER.log("no matching '}' found", io::ELL_WARNING);
			return qfalse;
		}

		if ( token[0] == '}' )
		{
			break;
		}

		//
		// map <name>
		//
		if ( !Q_stricmp( token, "map" ) )
		{
			token = COM_ParseExt( text, qfalse );
			if ( !token[0] )
			{
				sprintf(ch, "missing parameter for 'map' keyword in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				return qfalse;
			}

			if ( !Q_stricmp( token, "$whiteimage" ) )
			{
				stage->bundle[0].image[0] = CQ3LevelMesh::getQ3WhiteImage();

				continue;
			}
			else if ( !Q_stricmp( token, "$lightmap" ) )
			{
				stage->bundle[0].image[0] = CQ3LevelMesh::getQ3WhiteImage();
				stage->bundle[0].isLightmap = qtrue;			

				continue;
			}
			else if (!CQ3LevelMesh::isRadiantHintTexture(token))
			{
				// loading texture here					

				if (loadtex)
				{
					bool mipsflag = 
						VIDEO_DRIVER.getTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS);

					VIDEO_DRIVER.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS, !q3::shader.noMipMaps);

					vid::ITexture* tex = CQ3LevelMesh::getQ3TextureWithExt(token);
					
					VIDEO_DRIVER.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS, mipsflag);

					stage->bundle[0].image[0] = tex;

					if ( !stage->bundle[0].image[0] )
					{
						sprintf(ch, "could not find '%s' in shader '%s'", token, shader.name );
						LOGGER.log(ch, io::ELL_WARNING);

						return qfalse;
					}
				}
			}
		}
		//
		// clampmap <name>
		//
		else if ( !Q_stricmp( token, "clampmap" ) )
		{
			token = COM_ParseExt( text, qfalse );
			if ( !token[0] )
			{
				sprintf(ch, "missing parameter for 'clampmap' keyword in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				return qfalse;
			}

			// loading texture here


			stage->bundle[0].isClampMap = true;

			if (loadtex)
			{
				bool mipsflag = 
					VIDEO_DRIVER.getTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS);

				VIDEO_DRIVER.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS, !q3::shader.noMipMaps);

				vid::ITexture* tex = CQ3LevelMesh::getQ3TextureWithExt(token);

				VIDEO_DRIVER.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS, mipsflag);
				
				stage->bundle[0].image[0]   = tex;				

				if ( !stage->bundle[0].image[0] )
				{
					sprintf(ch, "could not find '%s' in shader '%s'", token, shader.name );
					LOGGER.log(ch, io::ELL_WARNING);

					return qfalse;
				}
			}
		}
		//
		// animMap <frequency> <image1> .... <imageN>
		//
		else if ( !Q_stricmp( token, "animMap" ) )
		{
			token = COM_ParseExt( text, qfalse );
			if ( !token[0] )
			{
				sprintf(ch, "missing parameter for 'animMmap' keyword in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				return qfalse;
			}
			stage->bundle[0].imageAnimationSpeed = atof( token );

			// parse up to MAX_IMAGE_ANIMATIONS animations
			while ( 1 )
			{
				int		num;

				token = COM_ParseExt( text, qfalse );
				if ( !token[0] ) 
				{
					break;
				}
				num = stage->bundle[0].numImageAnimations;
				if ( num < MAX_IMAGE_ANIMATIONS ) 
				{
					if (loadtex)
					{
						bool mipsflag = 
							VIDEO_DRIVER.getTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS);

						VIDEO_DRIVER.setTextureCreationFlag(
							vid::ETCF_AUTOGEN_MIP_MAPS, !q3::shader.noMipMaps);

						vid::ITexture* tex = CQ3LevelMesh::getQ3TextureWithExt(token);

						VIDEO_DRIVER.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS, mipsflag);

						stage->bundle[0].image[num] = tex;
						
						if ( !stage->bundle[0].image[num] )
						{
							sprintf(ch,  "could not find '%s' in shader '%s'", token, shader.name );
							LOGGER.log(ch, io::ELL_WARNING);

							return qfalse;
						}
					}
					stage->bundle[0].numImageAnimations++;
				}
			}
		}
		else if ( !Q_stricmp( token, "videoMap" ) )
		{
			token = COM_ParseExt( text, qfalse );
			if ( !token[0] )
			{
				sprintf(ch,   "missing parameter for 'videoMmap' keyword in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				return qfalse;
			}
			stage->bundle[0].videoMapHandle = 0;
		}
		//
		// alphafunc <func>
		//
		else if ( !Q_stricmp( token, "alphaFunc" ) )
		{
			token = COM_ParseExt( text, qfalse );
			if ( !token[0] )
			{
				sprintf(ch, "missing parameter for 'alphaFunc' keyword in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				return qfalse;
			}

			atestBits = NameToAFunc( token );
		}
		//
		// depthFunc <func>
		//
		else if ( !Q_stricmp( token, "depthfunc" ) )
		{
			token = COM_ParseExt( text, qfalse );

			if ( !token[0] )
			{
				sprintf(ch, "missing parameter for 'depthfunc' keyword in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				return qfalse;
			}

			if ( !Q_stricmp( token, "lequal" ) )
			{
				depthFuncBits = 0;
			}
			else if ( !Q_stricmp( token, "equal" ) )
			{
				depthFuncBits = GLS_DEPTHFUNC_EQUAL;
			}
			else
			{
				sprintf(ch, "unknown depthfunc '%s' in shader '%s'", token, shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				continue;
			}
		}
		//
		// detail
		//
		else if ( !Q_stricmp( token, "detail" ) )
		{
			stage->isDetail = qtrue;
		}
		//
		// blendfunc <srcFactor> <dstFactor>
		// or blendfunc <add|filter|blend>
		//
		else if ( !Q_stricmp( token, "blendfunc" ) )
		{
			token = COM_ParseExt( text, qfalse );
			if ( token[0] == 0 )
			{
				sprintf(ch, "missing parm for blendFunc in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				continue;
			}
			// check for "simple" blends first
			if ( !Q_stricmp( token, "add" ) ) 
			{
				blendSrcBits = GLS_SRCBLEND_ONE;
				blendDstBits = GLS_DSTBLEND_ONE;
			} 
			else if ( !Q_stricmp( token, "filter" ) ) 
			{
				blendSrcBits = GLS_SRCBLEND_DST_COLOR;
				blendDstBits = GLS_DSTBLEND_ZERO;
			} 
			else if ( !Q_stricmp( token, "blend" ) ) 
			{
				blendSrcBits = GLS_SRCBLEND_SRC_ALPHA;
				blendDstBits = GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA;
			} 
			else 
			{
				// complex double blends
				blendSrcBits = NameToSrcBlendMode( token );

				token = COM_ParseExt( text, qfalse );
				if ( token[0] == 0 )
				{
					sprintf(ch, "missing parm for blendFunc in shader '%s'", shader.name );
					LOGGER.log(ch, io::ELL_WARNING);

					continue;
				}
				blendDstBits = NameToDstBlendMode( token );
			}

			// clear depth mask for blended surfaces
			if ( !depthMaskExplicit )
			{
				depthMaskBits = 0;
			}
		}
		//
		// rgbGen
		//
		else if ( !Q_stricmp( token, "rgbGen" ) )
		{
			token = COM_ParseExt( text, qfalse );
			if ( token[0] == 0 )
			{
				sprintf(ch, "missing parameters for rgbGen in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				continue;
			}

			if ( !Q_stricmp( token, "wave" ) )
			{
				ParseWaveForm( text, &stage->rgbWave );
				stage->rgbGen = CGEN_WAVEFORM;
			}
			else if ( !Q_stricmp( token, "const" ) )
			{
				vec3_t	color;

				ParseVector( text, 3, color );
				stage->constantColor[0] = 255 * color[0];
				stage->constantColor[1] = 255 * color[1];
				stage->constantColor[2] = 255 * color[2];

				stage->rgbGen = CGEN_CONST;
			}
			else if ( !Q_stricmp( token, "identity" ) )
			{
				stage->rgbGen = CGEN_IDENTITY;
			}
			else if ( !Q_stricmp( token, "identityLighting" ) )
			{
				stage->rgbGen = CGEN_IDENTITY_LIGHTING;
			}
			else if ( !Q_stricmp( token, "entity" ) )
			{
				stage->rgbGen = CGEN_ENTITY;
			}
			else if ( !Q_stricmp( token, "oneMinusEntity" ) )
			{
				stage->rgbGen = CGEN_ONE_MINUS_ENTITY;
			}
			else if ( !Q_stricmp( token, "vertex" ) )
			{
				stage->rgbGen = CGEN_VERTEX;
				if ( stage->alphaGen == 0 ) {
					stage->alphaGen = AGEN_VERTEX;
				}
			}
			else if ( !Q_stricmp( token, "exactVertex" ) )
			{
				stage->rgbGen = CGEN_EXACT_VERTEX;
			}
			else if ( !Q_stricmp( token, "lightingDiffuse" ) )
			{
				stage->rgbGen = CGEN_LIGHTING_DIFFUSE;
			}
			else if ( !Q_stricmp( token, "oneMinusVertex" ) )
			{
				stage->rgbGen = CGEN_ONE_MINUS_VERTEX;
			}
			else
			{
				sprintf(ch, "unknown rgbGen parameter '%s' in shader '%s'", token, shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				continue;
			}
		}
		//
		// alphaGen 
		//
		else if ( !Q_stricmp( token, "alphaGen" ) )
		{
			token = COM_ParseExt( text, qfalse );
			if ( token[0] == 0 )
			{
				sprintf(ch, "missing parameters for alphaGen in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				continue;
			}

			if ( !Q_stricmp( token, "wave" ) )
			{
				ParseWaveForm( text, &stage->alphaWave );
				stage->alphaGen = AGEN_WAVEFORM;
			}
			else if ( !Q_stricmp( token, "const" ) )
			{
				token = COM_ParseExt( text, qfalse );
				stage->constantColor[3] = 255 * atof( token );
				stage->alphaGen = AGEN_CONST;
			}
			else if ( !Q_stricmp( token, "identity" ) )
			{
				stage->alphaGen = AGEN_IDENTITY;
			}
			else if ( !Q_stricmp( token, "entity" ) )
			{
				stage->alphaGen = AGEN_ENTITY;
			}
			else if ( !Q_stricmp( token, "oneMinusEntity" ) )
			{
				stage->alphaGen = AGEN_ONE_MINUS_ENTITY;
			}
			else if ( !Q_stricmp( token, "vertex" ) )
			{
				stage->alphaGen = AGEN_VERTEX;
			}
			else if ( !Q_stricmp( token, "lightingSpecular" ) )
			{
				stage->alphaGen = AGEN_LIGHTING_SPECULAR;
			}
			else if ( !Q_stricmp( token, "oneMinusVertex" ) )
			{
				stage->alphaGen = AGEN_ONE_MINUS_VERTEX;
			}
			else if ( !Q_stricmp( token, "portal" ) )
			{
				stage->alphaGen = AGEN_PORTAL;
				token = COM_ParseExt( text, qfalse );
				if ( token[0] == 0 )
				{
					sprintf(ch, "missing range parameter for alphaGen portal in shader '%s', defaulting to 256", shader.name );
					LOGGER.log(ch, io::ELL_WARNING);
				}
			}
			else
			{
				sprintf(ch, "unknown alphaGen parameter '%s' in shader '%s'", token, shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				continue;
			}
		}
		//
		// tcGen <function>
		//
		else if ( !Q_stricmp(token, "texgen") || !Q_stricmp( token, "tcGen" ) ) 
		{
			token = COM_ParseExt( text, qfalse );
			if ( token[0] == 0 )
			{
				sprintf(ch, "missing texgen parm in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				continue;
			}

			if ( !Q_stricmp( token, "environment" ) )
			{
				stage->bundle[0].tcGen = TCGEN_ENVIRONMENT_MAPPED;
			}
			else if ( !Q_stricmp( token, "lightmap" ) )
			{
				stage->bundle[0].tcGen = TCGEN_LIGHTMAP;
			}
			else if ( !Q_stricmp( token, "texture" ) || !Q_stricmp( token, "base" ) )
			{
				stage->bundle[0].tcGen = TCGEN_TEXTURE;
			}
			else if ( !Q_stricmp( token, "vector" ) )
			{
				ParseVector( text, 3, stage->bundle[0].tcGenVectors[0] );
				ParseVector( text, 3, stage->bundle[0].tcGenVectors[1] );

				stage->bundle[0].tcGen = TCGEN_VECTOR;
			}
			else 
			{
				sprintf(ch, "unknown texgen parm in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);
			}
		}
		//
		// tcMod <type> <...>
		//
		else if ( !Q_stricmp( token, "tcMod" ) )
		{
			char buffer[1024] = "";

			while ( 1 )
			{
				token = COM_ParseExt( text, qfalse );
				if ( token[0] == 0 )
					break;
				strcat( buffer, token );
				strcat( buffer, " " );
			}

			ParseTexMod( buffer, stage );

			continue;
		}
		//
		// depthmask
		//
		else if ( !Q_stricmp( token, "depthwrite" ) )
		{
			depthMaskBits = GLS_DEPTHMASK_TRUE;
			depthMaskExplicit = qtrue;

			continue;
		}
		else
		{
			sprintf(ch, "unknown parameter '%s' in shader '%s'", token, shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return qfalse;
		}
	}

	//
	// if cgen isn't explicitly specified, use either identity or identitylighting
	//
	if ( stage->rgbGen == CGEN_BAD ) 
	{
		if (blendSrcBits == 0 ||
			blendSrcBits == GLS_SRCBLEND_ONE || 
			blendSrcBits == GLS_SRCBLEND_SRC_ALPHA 
			) 
		{
			stage->rgbGen = CGEN_IDENTITY_LIGHTING;
		} 
		else 
		{
			stage->rgbGen = CGEN_IDENTITY;
		}
	}

	//
	// implicitly assume that a GL_ONE GL_ZERO blend mask disables blending
	//
	if ( ( blendSrcBits == GLS_SRCBLEND_ONE ) && 
		 ( blendDstBits == GLS_DSTBLEND_ZERO ) 
		 )
	{
		blendDstBits = blendSrcBits = 0;
		depthMaskBits = GLS_DEPTHMASK_TRUE;
	}

	// decide which agens we can skip
	if ( stage->alphaGen == CGEN_IDENTITY ) 
	{
		if ( stage->rgbGen == CGEN_IDENTITY
			|| stage->rgbGen == CGEN_LIGHTING_DIFFUSE 
			) 
		{
			stage->alphaGen = AGEN_SKIP;
		}
	}

	//
	// compute state bits
	//
	stage->stateBits = depthMaskBits | 
		               blendSrcBits | blendDstBits | 
					   atestBits | 
					   depthFuncBits;

	return qtrue;
}

//--------------------------------------------------------

void ParseDeform( char **text ) 
{
	char	*token;
	deformStage_t	*ds;

	token = COM_ParseExt( text, qfalse );
	if ( token[0] == 0 )
	{
		sprintf(ch, "missing deform parm in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return;
	}

	if ( shader.numDeforms == MAX_SHADER_DEFORMS ) 
	{
		sprintf(ch, "MAX_SHADER_DEFORMS in '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return;
	}

	ds = &shader.deforms[ shader.numDeforms ];
	shader.numDeforms++;

	if ( !Q_stricmp( token, "projectionShadow" ) ) 
	{
		ds->deformation = DEFORM_PROJECTION_SHADOW;
		return;
	}

	if ( !Q_stricmp( token, "autosprite" ) ) 
	{
		ds->deformation = DEFORM_AUTOSPRITE;
		return;
	}

	if ( !Q_stricmp( token, "autosprite2" ) ) 
	{
		ds->deformation = DEFORM_AUTOSPRITE2;
		return;
	}

	if ( !Q_stricmpn( token, "text", 4 ) ) 
	{
		int		n;
		
		n = token[4] - '0';
		if ( n < 0 || n > 7 ) 
		{
			n = 0;
		}
		ds->deformation = (deform_t)(DEFORM_TEXT0 + n);
		return;
	}

	if ( !Q_stricmp( token, "bulge" ) )	
	{
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing deformVertexes bulge parm in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		ds->bulgeWidth = atof( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing deformVertexes bulge parm in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		ds->bulgeHeight = atof( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing deformVertexes bulge parm in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		ds->bulgeSpeed = atof( token );

		ds->deformation = DEFORM_BULGE;
		return;
	}

	if ( !Q_stricmp( token, "wave" ) )
	{
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing deformVertexes parm in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}

		if ( atof( token ) != 0 )
		{
			ds->deformationSpread = 1.0f / atof( token );
		}
		else
		{
			ds->deformationSpread = 100.0f;
			sprintf(ch, "illegal div value of 0 in deformVertexes command for shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);
		}

		ParseWaveForm( text, &ds->deformationWave );
		ds->deformation = DEFORM_WAVE;
		return;
	}

	if ( !Q_stricmp( token, "normal" ) )
	{
		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing deformVertexes parm in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		ds->deformationWave.amplitude = atof( token );

		token = COM_ParseExt( text, qfalse );
		if ( token[0] == 0 )
		{
			sprintf(ch, "missing deformVertexes parm in shader '%s'", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return;
		}
		ds->deformationWave.frequency = atof( token );

		ds->deformation = DEFORM_NORMALS;
		return;
	}

	if ( !Q_stricmp( token, "move" ) ) 
	{
		int		i;

		for ( i = 0 ; i < 3 ; i++ ) 
		{
			token = COM_ParseExt( text, qfalse );
			if ( token[0] == 0 ) 
			{
				sprintf(ch, "missing deformVertexes parm in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				return;
			}
			ds->moveVector[i] = atof( token );
		}

		ParseWaveForm( text, &ds->deformationWave );
		ds->deformation = DEFORM_MOVE;
		return;
	}

	sprintf(ch, "unknown deformVertexes subtype '%s' found in shader '%s'", token, shader.name );
	LOGGER.log(ch, io::ELL_WARNING);
}

//--------------------------------------------------------

void ParseSurfaceParm( char **text ) 
{
	char	*token;
	int		numInfoParms = sizeof(infoParms) / sizeof(infoParms[0]);
	int		i;

	token = COM_ParseExt( text, qfalse );
	for ( i = 0 ; i < numInfoParms ; i++ ) 
	{
		if ( !Q_stricmp( token, infoParms[i].name ) ) 
		{
			shader.surfaceFlags |= infoParms[i].surfaceFlags;
			shader.contentFlags |= infoParms[i].contents;
#if 0
			if ( infoParms[i].clearSolid ) {
				si->contents &= ~CONTENTS_SOLID;
			}
#endif
			break;
		}
	}
}

//--------------------------------------------------------

void ParseSkyParms( char **text ) 
{
	char		*token;
	static char	*suf[6] = {"rt", "bk", "lf", "ft", "up", "dn"};
	char		pathname[MAX_QPATH];
	int			i;

	// outerbox
	token = COM_ParseExt( text, qfalse );
	if ( token[0] == 0 ) 
	{
		sprintf(ch, "'skyParms' missing parameter in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return;
	}
	if ( strcmp( token, "-" ) ) 
	{
		for (i=0 ; i<6 ; i++) 
		{
			sprintf( pathname, "%s_%s.tga", token, suf[i] );
			shader.sky.outerbox[i] = CQ3LevelMesh::getQ3Texture(pathname);				
			if ( !shader.sky.outerbox[i] )
			{
				shader.sky.outerbox[i] = 0;
			}
		}
	}

	// cloudheight
	token = COM_ParseExt( text, qfalse );
	if ( token[0] == 0 ) 
	{
		sprintf(ch, "'skyParms' missing parameter in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return;
	}
	shader.sky.cloudHeight = atof( token );
	if ( !shader.sky.cloudHeight ) 
	{
		shader.sky.cloudHeight = 512;
	}

	// innerbox
	token = COM_ParseExt( text, qfalse );
	if ( token[0] == 0 ) 
	{
		sprintf(ch, "'skyParms' missing parameter in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return;
	}
	if ( strcmp( token, "-" ) ) 
	{
		for (i=0 ; i<6 ; i++) 
		{
			sprintf( pathname, "%s_%s.tga", token, suf[i] );
			shader.sky.innerbox[i] = CQ3LevelMesh::getQ3Texture(pathname);
			if ( !shader.sky.innerbox[i] )
			{
				shader.sky.innerbox[i] = 0;
			}
		}
	}

	shader.isSky = qtrue;
}

//--------------------------------------------------------

void ParseSort( char **text ) 
{
	char	*token;

	token = COM_ParseExt( text, qfalse );
	if ( token[0] == 0 ) 
	{
		sprintf(ch, "missing sort parameter in shader '%s'", shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return;
	}

	if ( !Q_stricmp( token, "portal" ) ) 
	{
		shader.sort = SS_PORTAL;
	} 
	else if ( !Q_stricmp( token, "sky" ) ) 
	{
		shader.sort = SS_ENVIRONMENT;
	} 
	else if ( !Q_stricmp( token, "opaque" ) ) 
	{
		shader.sort = SS_OPAQUE;
	}
	else if ( !Q_stricmp( token, "decal" ) ) 
	{
		shader.sort = SS_DECAL;
	} 
	else if ( !Q_stricmp( token, "seeThrough" ) ) 
	{
		shader.sort = SS_SEE_THROUGH;
	} 
	else if ( !Q_stricmp( token, "banner" ) ) 
	{
		shader.sort = SS_BANNER;
	} 
	else if ( !Q_stricmp( token, "additive" ) )
	{
		shader.sort = SS_BLEND1;
	} 
	else if ( !Q_stricmp( token, "nearest" ) ) 
	{
		shader.sort = SS_NEAREST;
	} 
	else if ( !Q_stricmp( token, "underwater" ) ) 
	{
		shader.sort = SS_UNDERWATER;
	} 
	else 
	{
		shader.sort = atof( token );
	}
}

//--------------------------------------------------------

qboolean ParseShader( char **text, bool load_textures )
{
	char *token;
	int s;

	s = 0;

	token = COM_ParseExt( text, qtrue );
	if ( token[0] != '{' )
	{
		sprintf(ch, "expecting '{', found '%s' instead in shader '%s'", token, shader.name );
		LOGGER.log(ch, io::ELL_WARNING);

		return qfalse;
	}

	while ( 1 )
	{
		token = COM_ParseExt( text, qtrue );
		if ( !token[0] )
		{
			sprintf(ch, "no concluding '}' in shader %s", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return qfalse;
		}

		// end of shader definition
		if ( token[0] == '}' )
		{
			break;
		}
		// stage definition
		else if ( token[0] == '{' )
		{			
			if ( !ParseStage( &stages[s], text, load_textures ) )
			{
				return qfalse;
			}
			stages[s].active = qtrue;
			s++;
			continue;
		}
		// skip stuff that only the QuakeEdRadient needs
		else if ( !Q_stricmpn( token, "qer", 3 ) ) 
		{
			SkipRestOfLine( text );
			continue;
		}
		// sun parms
		else if ( !Q_stricmp( token, "q3map_sun" ) ) 
		{
			float	a, b;

			token = COM_ParseExt( text, qfalse );
			tr.sunLight[0] = atof( token );
			token = COM_ParseExt( text, qfalse );
			tr.sunLight[1] = atof( token );
			token = COM_ParseExt( text, qfalse );
			tr.sunLight[2] = atof( token );
			
			VectorNormalize( tr.sunLight );

			token = COM_ParseExt( text, qfalse );
			a = atof( token );
			VectorScale( tr.sunLight, a, tr.sunLight);

			token = COM_ParseExt( text, qfalse );
			a = atof( token );
			a = a / 180 * M_PI;

			token = COM_ParseExt( text, qfalse );
			b = atof( token );
			b = b / 180 * M_PI;

			tr.sunDirection[0] = cos( a ) * cos( b );
			tr.sunDirection[1] = sin( a ) * cos( b );
			tr.sunDirection[2] = sin( b );
		}
		else if ( !Q_stricmp( token, "deformVertexes" ) ) 
		{
			ParseDeform( text );
			continue;
		}
		else if ( !Q_stricmp( token, "tesssize" ) ) 
		{
			SkipRestOfLine( text );
			continue;
		}
		else if ( !Q_stricmp( token, "clampTime" ) ) 
		{
			token = COM_ParseExt( text, qfalse );
			if (token[0]) 
			{
				shader.clampTime = atof(token);
			}
		}
		// skip stuff that only the q3map needs
		else if ( !Q_stricmpn( token, "q3map", 5 ) ) 
		{
			SkipRestOfLine( text );
			continue;
		}
		// skip stuff that only q3map or the server needs
		else if ( !Q_stricmp( token, "surfaceParm" ) ) 
		{
			ParseSurfaceParm( text );
			continue;
		}
		// no mip maps
		else if ( !Q_stricmp( token, "nomipmaps" ) )
		{
			shader.noMipMaps = qtrue;
			shader.noPicMip = qtrue;
			continue;
		}
		// no picmip adjustment
		else if ( !Q_stricmp( token, "nopicmip" ) )
		{
			shader.noPicMip = qtrue;
			continue;
		}
		// polygonOffset
		else if ( !Q_stricmp( token, "polygonOffset" ) )
		{
			shader.polygonOffset = qtrue;
			continue;
		}
		// entityMergable, allowing sprite surfaces from multiple entities
		// to be merged into one batch.  This is a savings for smoke
		// puffs and blood, but can't be used for anything where the
		// shader calcs (not the surface function) reference the entity color or scroll
		else if ( !Q_stricmp( token, "entityMergable" ) )
		{
			shader.entityMergable = qtrue;
			continue;
		}
		// fogParms
		else if ( !Q_stricmp( token, "fogParms" ) ) 
		{
			if ( !ParseVector( text, 3, shader.fogParms.color ) ) 
			{
				return qfalse;
			}

			token = COM_ParseExt( text, qfalse );
			if ( !token[0] ) 
			{
				sprintf(ch, "missing parm for 'fogParms' keyword in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				continue;
			}
			shader.fogParms.depthForOpaque = atof( token );

			// skip any old gradient directions
			SkipRestOfLine( text );
			continue;
		}
		// portal
		else if ( !Q_stricmp(token, "portal") )
		{
			shader.sort = SS_PORTAL;
			continue;
		}
		// skyparms <cloudheight> <outerbox> <innerbox>
		else if ( !Q_stricmp( token, "skyparms" ) )
		{
			ParseSkyParms( text );
			continue;
		}
		// light <value> determines flaring in q3map, not needed here
		else if ( !Q_stricmp(token, "light") ) 
		{
			token = COM_ParseExt( text, qfalse );
			continue;
		}
		// cull <face>
		else if ( !Q_stricmp( token, "cull") ) 
		{
			token = COM_ParseExt( text, qfalse );
			if ( token[0] == 0 )
			{
				sprintf(ch, "missing cull parms in shader '%s'", shader.name );
				LOGGER.log(ch, io::ELL_WARNING);

				continue;
			}

			if ( !Q_stricmp( token, "none" ) || !Q_stricmp( token, "twosided" ) || !Q_stricmp( token, "disable" ) )
			{
				shader.cullType = CT_TWO_SIDED;
			}
			else if ( !Q_stricmp( token, "back" ) || !Q_stricmp( token, "backside" ) || !Q_stricmp( token, "backsided" ) )
			{
				shader.cullType = CT_BACK_SIDED;
			}
			else if ( !Q_stricmp( token, "front" ) || !Q_stricmp( token, "frontside" ) || !Q_stricmp( token, "frontsided" ) )
			{
				shader.cullType = CT_FRONT_SIDED;
			}
			else
			{
				sprintf(ch, "invalid cull parm '%s' in shader '%s'", token, shader.name );
				LOGGER.log(ch, io::ELL_WARNING);
			}
			continue;
		}
		// sort
		else if ( !Q_stricmp( token, "sort" ) )
		{
			ParseSort( text );
			continue;
		}
		else
		{
			sprintf(ch, "unknown general shader parameter '%s' in '%s'", token, shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			return qfalse;
		}
	}

	//
	// ignore shaders that don't have any stages, unless it is a sky or fog
	//
	if ( s == 0 && !shader.isSky && !(shader.contentFlags & CONTENTS_FOG ) ) 
	{
		return qfalse;
	}

	shader.explicitlyDefined = qtrue;

	return qtrue;
}

//--------------------------------------------------------

// Attempt to combine two stages into a single multitexture stage
static qboolean CollapseMultitexture( void ) 
{
	int abits, bbits;
	int i;
	textureBundle_t tmpBundle;

	if ( !VIDEO_DRIVER.queryFeature(vid::EVDF_MULITEXTURE) ) 
	{
		return qfalse;
	}

	// make sure both stages are active
	if ( !stages[0].active || !stages[1].active ) 
	{
		return qfalse;
	}

	abits = stages[0].stateBits;
	bbits = stages[1].stateBits;

	// make sure that both stages have identical state other than blend modes
	if ( ( abits & ~( GLS_DSTBLEND_BITS | GLS_SRCBLEND_BITS | GLS_DEPTHMASK_TRUE ) ) !=
		( bbits & ~( GLS_DSTBLEND_BITS | GLS_SRCBLEND_BITS | GLS_DEPTHMASK_TRUE ) ) ) 
	{
		return qfalse;
	}

	abits &= ( GLS_DSTBLEND_BITS | GLS_SRCBLEND_BITS );
	bbits &= ( GLS_DSTBLEND_BITS | GLS_SRCBLEND_BITS );

	// search for a valid multitexture blend function
	for ( i = 0; collapse[i].blendA != -1 ; i++ ) 
	{
		if ( abits == collapse[i].blendA&& bbits == collapse[i].blendB )
		{
			break;
		}
	}

	// nothing found
	if ( collapse[i].blendA == -1 ) 
	{
		return qfalse;
	}

	// make sure waveforms have identical parameters
	if ( ( stages[0].rgbGen != stages[1].rgbGen ) ||
		( stages[0].alphaGen != stages[1].alphaGen ) )  
	{
		return qfalse;
	}

	// an add collapse can only have identity colors
	if ( collapse[i].multitextureEnv == _GL_ADD && stages[0].rgbGen != CGEN_IDENTITY ) 
	{
		return qfalse;
	}

	if ( stages[0].rgbGen == CGEN_WAVEFORM )
	{
		if ( memcmp( &stages[0].rgbWave,
					 &stages[1].rgbWave,
					 sizeof( stages[0].rgbWave ) ) )
		{
			return qfalse;
		}
	}
	if ( stages[0].alphaGen == CGEN_WAVEFORM )
	{
		if ( memcmp( &stages[0].alphaWave,
					 &stages[1].alphaWave,
					 sizeof( stages[0].alphaWave ) ) )
		{
			return qfalse;
		}
	}

	// make sure that lightmaps are in bundle 1 for 3dfx
	if ( stages[0].bundle[0].isLightmap )
	{
		tmpBundle = stages[0].bundle[0];
		stages[0].bundle[0] = stages[1].bundle[0];
		stages[0].bundle[1] = tmpBundle;
	}
	else
	{
		stages[0].bundle[1] = stages[1].bundle[0];
	}

	// set the new blend state bits
	shader.multitextureEnv = collapse[i].multitextureEnv;
	stages[0].stateBits &= ~( GLS_DSTBLEND_BITS | GLS_SRCBLEND_BITS );
	stages[0].stateBits |= collapse[i].multitextureBlend;

	//
	// move down subsequent shaders
	//
	memmove( &stages[1], &stages[2], sizeof( stages[0] ) * ( MAX_SHADER_STAGES - 2 ) );
	memset( &stages[MAX_SHADER_STAGES-1], 0, sizeof( stages[0] ) );

	return qtrue;
}

//--------------------------------------------------------

// Attempt to combine two stages into a single multitexture stage
static qboolean CollapseMultitexture( s32 s0, s32 s1 ) 
{
	int abits, bbits;
	int i;
	textureBundle_t tmpBundle;

	if ( !VIDEO_DRIVER.queryFeature(vid::EVDF_MULITEXTURE) ) 
	{
		return qfalse;
	}

	// make sure both stages are active
	if ( !stages[s0].active || !stages[s1].active ) 
	{
		return qfalse;
	}

	abits = stages[s0].stateBits;
	bbits = stages[s1].stateBits;

	// make sure that both stages have identical state other than blend modes
	if (( abits & ~( GLS_ATEST_BITS | GLS_DSTBLEND_BITS | GLS_SRCBLEND_BITS | GLS_DEPTHMASK_TRUE ) ) !=
		( bbits & ~( GLS_ATEST_BITS | GLS_DSTBLEND_BITS | GLS_SRCBLEND_BITS | GLS_DEPTHMASK_TRUE ) ) 
		) 
	{
		return qfalse;
	}

	abits &= ( GLS_DSTBLEND_BITS | GLS_SRCBLEND_BITS );
	bbits &= ( GLS_DSTBLEND_BITS | GLS_SRCBLEND_BITS );

	// search for a valid multitexture blend function
	for ( i = 0; collapse[i].blendA != -1 ; i++ ) 
	{
		if ( abits == collapse[i].blendA&& bbits == collapse[i].blendB )
		{
			break;
		}
	}

	// nothing found
	if ( collapse[i].blendA == -1 ) 
	{
		return qfalse;
	}

	// an add collapse can only have identity colors
	if ( collapse[i].multitextureEnv == _GL_ADD && stages[s0].rgbGen != CGEN_IDENTITY ) 
	{
		return qfalse;
	}

	// make sure waveforms have identical parameters
	if ( ( stages[s0].rgbGen != stages[s1].rgbGen ) ||
		( stages[s0].alphaGen != stages[s1].alphaGen ) )  
	{
		return qfalse;
	}

	if ( stages[s0].rgbGen == CGEN_WAVEFORM )
	{
		if ( memcmp( &stages[s0].rgbWave,
					 &stages[s1].rgbWave,
					 sizeof( stages[s0].rgbWave ) ) )
		{
			return qfalse;
		}
	}
	if ( stages[s0].alphaGen == CGEN_WAVEFORM )
	{
		if ( memcmp( &stages[s0].alphaWave,
					 &stages[s1].alphaWave,
					 sizeof( stages[s0].alphaWave ) ) )
		{
			return qfalse;
		}
	}

	// make sure that lightmaps are in bundle 1 for 3dfx
	if ( stages[s0].bundle[0].isLightmap )
	{
		tmpBundle = stages[s0].bundle[0];
		stages[s0].bundle[0] = stages[s1].bundle[0];
		stages[s0].bundle[1] = tmpBundle;
		if (!(stages[s1].stateBits&GLS_DEPTHMASK_TRUE) &&
			(stages[s1].stateBits&(GLS_DSTBLEND_BITS|GLS_SRCBLEND_BITS)))
			stages[s0].stateBits &= ~GLS_DEPTHMASK_TRUE;
	}
	else
	{
		stages[s0].bundle[1] = stages[s1].bundle[0];
	}

	// set the new blend state bits
	shader.multitextureEnv = collapse[i].multitextureEnv;
	stages[s0].stateBits &= ~( GLS_DSTBLEND_BITS | GLS_SRCBLEND_BITS );
	stages[s0].stateBits |= collapse[i].multitextureBlend;

	//
	// move down subsequent shaders
	//
	if ((s1+1)<MAX_SHADER_STAGES)
	{
		memmove( &stages[s1], &stages[s1+1], sizeof( stages[s0] ) * ( MAX_SHADER_STAGES - 2 ) );
		memset( &stages[MAX_SHADER_STAGES-1], 0, sizeof( stages[s0] ) );
	}
	else
	{
		stages[s1].active=false;
	}

	return qtrue;
}

//--------------------------------------------------------

void FinishShader( bool load_textures ) 
{
	int stage;
	qboolean		hasLightmapStage;
	qboolean		vertexLightmap;

	hasLightmapStage = qfalse;
	vertexLightmap = qfalse;

	//
	// set sky stuff appropriate
	//
	if ( shader.isSky ) 
	{
		shader.sort = SS_ENVIRONMENT;
	}

	//
	// set polygon offset
	//
	if ( shader.polygonOffset && !shader.sort ) 
	{
		shader.sort = SS_DECAL;
	}

	//
	// set appropriate stage information
	//
	for ( stage = 0; stage < MAX_SHADER_STAGES; stage++ ) 
	{
		shaderStage_t *pStage = &stages[stage];

		if ( !pStage->active ) 
		{
			break;
		}

		// check for a missing texture
		if ( !pStage->bundle[0].image[0] && load_textures ) 
		{
			sprintf(ch, "Shader %s has a stage with no image", shader.name );
			LOGGER.log(ch, io::ELL_WARNING);

			pStage->active = qfalse;

			continue;
		}

		//
		// ditch this stage if it's detail and detail textures are disabled
		//
		if ( pStage->isDetail) 
		{
			if ( stage < ( MAX_SHADER_STAGES - 1 ) ) 
			{
				memmove( pStage, pStage + 1, sizeof( *pStage ) * ( MAX_SHADER_STAGES - stage - 1 ) );
				memset( pStage + 1, 0, sizeof( *pStage ) );
			}
			continue;
		}

		//
		// default texture coordinate generation
		//
		if ( pStage->bundle[0].isLightmap ) 
		{
			if ( pStage->bundle[0].tcGen == TCGEN_BAD ) 
			{
				pStage->bundle[0].tcGen = TCGEN_LIGHTMAP;
			}
			hasLightmapStage = qtrue;
		} 
		else 
		{
			if ( pStage->bundle[0].tcGen == TCGEN_BAD ) 
			{
				pStage->bundle[0].tcGen = TCGEN_TEXTURE;
			}
		}

		// not a true lightmap but we want to leave existing 
		// behaviour in place and not print out a warning
		//if (pStage->rgbGen == CGEN_VERTEX) {
		//  vertexLightmap = qtrue;
		//}

		//
		// determine sort order and fog color adjustment
		//
		if ((pStage->stateBits & (GLS_SRCBLEND_BITS | GLS_DSTBLEND_BITS)) &&
			(stages[0].stateBits & (GLS_SRCBLEND_BITS | GLS_DSTBLEND_BITS))) 
		{
			int blendSrcBits = pStage->stateBits & GLS_SRCBLEND_BITS;
			int blendDstBits = pStage->stateBits & GLS_DSTBLEND_BITS;

			// fog color adjustment only works for blend modes that have a contribution
			// that aproaches 0 as the modulate values aproach 0 --
			// GL_ONE, GL_ONE
			// GL_ZERO, GL_ONE_MINUS_SRC_COLOR
			// GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA

			// modulate, additive
			if (((blendSrcBits == GLS_SRCBLEND_ONE) &&
					(blendDstBits == GLS_DSTBLEND_ONE)) ||
				((blendSrcBits == GLS_SRCBLEND_ZERO) &&
					(blendDstBits == GLS_DSTBLEND_ONE_MINUS_SRC_COLOR))) 
			{
				pStage->adjustColorsForFog = ACFF_MODULATE_RGB;
			}
			// strict blend
			else 
			if ((blendSrcBits == GLS_SRCBLEND_SRC_ALPHA) &&
				(blendDstBits == GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA))
			{
				pStage->adjustColorsForFog = ACFF_MODULATE_ALPHA;
			}
			// premultiplied alpha
			else 
			if ((blendSrcBits == GLS_SRCBLEND_ONE) &&
				(blendDstBits == GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA))
			{
				pStage->adjustColorsForFog = ACFF_MODULATE_RGBA;
			} 
			else 
			{
				// we can't adjust this one correctly, so it won't be exactly correct in fog
			}

			// don't screw with sort order if this is a portal or environment
			if ( !shader.sort ) 
			{
				// see through item, like a grill or grate
				if ( pStage->stateBits & GLS_DEPTHMASK_TRUE ) 
					shader.sort = SS_SEE_THROUGH;
				else 
					shader.sort = SS_BLEND0;
			}
		}
	}

	// there are times when you will need to manually apply a sort to
	// opaque alpha tested shaders that have later blend passes
	if ( !shader.sort ) 
		shader.sort = SS_OPAQUE;

	//
	// look for multitexture potential
	//
	if (stage > 1) 
	{
		for (s32 s = 0; s < MAX_SHADER_STAGES-1; s++) 
		{
			shaderStage_t *s0 = &stages[s];
			shaderStage_t *s1 = &stages[s+1];
			if ( !s0->active || !s1->active ) 
				break;
			if (CollapseMultitexture(s,s+1))
				stage--;
		}
	}

	if (shader.lightmapIndex >= 0 && !hasLightmapStage) 
	{
		if (vertexLightmap) 
		{
			LOGGER.logWarn("shader '%s' has VERTEX forced lightmap!",
				shader.name);
		}
		else 
		{
			LOGGER.logWarn("shader '%s' has lightmap but no lightmap stage!",
				shader.name);
  			shader.lightmapIndex = LIGHTMAP_NONE;
		}
	}

	//
	// compute number of passes
	//
	shader.numUnfoggedPasses = stage;

	// fogonly shaders don't have any normal passes
	if (stage == 0) 
		shader.sort = SS_FOG;

	for (stage = 0; stage < MAX_SHADER_STAGES; stage++) 
		shader.stages[stage] = &stages[stage];
}

//---------------------------------------------------------------------------
} // end namespace q3
//---------------------------------------------------------------------------

bool CQ3ShaderParser::parseShaderTextToMaterial(
	const c8 *shader_name, c8 *shaderText, SMeshBufferMaterial &material, 
	bool load_textures)
{
	// clear the global shader
	memset( &q3::shader, 0, sizeof( q3::shader ) );
	memset( &q3::stages, 0, sizeof( q3::stages ) );

	for (u32 s=0; s<strlen(shader_name)&&s<MAX_QPATH; s++)
		q3::shader.name[s] = shader_name[s];

	for (u32 i = 0 ; i < MAX_SHADER_STAGES ; i++ ) 
		q3::stages[i].bundle[0].texMods = q3::texMods[i];

	q3::shader.lightmapIndex = LIGHTMAP_NONE;

	q3::shader.needsNormal = qtrue;
	q3::shader.needsST1 = qtrue;
	q3::shader.needsST2 = qtrue;
	q3::shader.needsColor = qtrue;
	q3::shader.cullType=q3::CT_BACK_SIDED;

	s32 pass_idx = 0;

	// default material initialization
	material.Material   = vid::SMaterial();	
	material.VertexType = vid::EVT_2TCOORDS;
	
	vid::SRenderPass &pass = material.Material.getPass(0);

	pass.setAmbientColor(img::SColor(255, 255, 255, 255));
	pass.setDiffuseColor(img::SColor(255, 255, 255, 255));
	pass.setEmissiveColor(img::SColor(255, 255, 255, 255));

	pass.setShininess(0.0f);

	pass.setFlag(vid::EMF_BLENDING,			false);
	pass.setFlag(vid::EMF_ZWRITE_ENABLE,	true);
	pass.setFlag(vid::EMF_BACK_FACE_CULLING,true);
	pass.setLightingMode(vid::ELM_NONE);
	pass.setDepthTest(vid::ECT_LESS);

	// parsing q3 shader
	if (!shaderText || !q3::ParseShader(&shaderText, load_textures))
		return false;
	q3::FinishShader(load_textures);

	bool no_coloured_vertices = false;
	bool constColor = false, constAlpha = false;
	s32 active_stage_idx = 0;
	q3::shader_t &shader = q3::shader;

	material.Name = shader_name;

	for (s32 s=0; s<MAX_SHADER_STAGES; s++)
	{
		q3::shaderStage_t *stage = q3::shader.stages[s];

		if (!stage || !stage->active)
			break;			
		
		if (active_stage_idx>0)
			material.Material.addPass( vid::SRenderPass() );

		active_stage_idx++;

		vid::SRenderPass &curr_pass = 
			material.Material.getPass(material.Material.getPassesCount()-1);				

		for (s32 b=0; b<NUM_TEXTURE_BUNDLES; b++)
		{
			vid::ITexture *tex=0;	
			core::array<vid::ITexture*>textures;
			vid::SAnimatedTextureParams anitex_params(10, true);

			for (s32 t=0; t<MAX_IMAGE_ANIMATIONS; t++)
			{
				if (stage->bundle[b].image[t])
				{
					textures.push_back(stage->bundle[b].image[t]);	
				}
			}

			if (textures.size()>1)
			{
				anitex_params.TimePerFrame = 1000.0f /
					(f32)stage->bundle[b].imageAnimationSpeed + 0.5f;
				tex = VIDEO_DRIVER.addTextureAnim(textures, anitex_params);
			}
			else
			{
				tex = stage->bundle[b].image[0];					
			}

			bool is_lightmap = 
				stage->bundle[b].isLightmap || stage->bundle[b].tcGen==q3::TCGEN_LIGHTMAP;
			
			curr_pass.Layers[b].setTexture(tex,
				is_lightmap ? vid::ETLT_LIGHT_MAP : vid::ETLT_DIFFUSE_MAP,
				is_lightmap ? 1 : 0);

			curr_pass.Layers[b].setTexCoordGen ( 
				stage->bundle[b].tcGen==q3::TCGEN_ENVIRONMENT_MAPPED ? 
				vid::ETCGT_ENVIRONMENT_MAPPING : vid::ETCGT_MANUAL_MAPPING );

			curr_pass.Layers[b].setTexCoordAddr ( 
				stage->bundle[b].isClampMap ? vid::ETCAM_CLAMP : vid::ETCAM_WRAP );

			for (s32 tm=0; tm<stage->bundle[b].numTexMods; tm++)
			{					
				q3::texModInfo_t &tmi = stage->bundle[b].texMods[tm];

				vid::STexCoordModifier tmod;				

				if (tmi.type==q3::TMOD_NONE)
				{	
					continue;
				}
				else
				if (tmi.type==q3::TMOD_SCROLL)
				{	
					tmod.setType(vid::ETCMT_SCROLL);
					tmod.setScroll(0.5f*tmi.scroll[0], 0.5f*tmi.scroll[1]);

					curr_pass.Layers[b].addTexCoordModifier ( tmod );
				}
				else 
				if (tmi.type==q3::TMOD_TRANSFORM)
				{
				}
				else 
				if (tmi.type==q3::TMOD_TURBULENT)
				{
					tmod.setType(vid::ETCMT_TURBULENT);
					tmod.setTurbulent(core::wave(
						core::EFT_SIN,
						tmi.wave.base,
						tmi.wave.amplitude,
						tmi.wave.phase,
						tmi.wave.frequency));

					curr_pass.Layers[b].addTexCoordModifier(tmod);
				}
				else 
				if (tmi.type==q3::TMOD_SCALE)
				{
					tmod.setType(vid::ETCMT_SCALE);
					tmod.setScale(tmi.scale[0], tmi.scale[1]);

					curr_pass.Layers[b].addTexCoordModifier ( tmod );
				}
				else 
				if (tmi.type==q3::TMOD_STRETCH)
				{
				}
				else 
				if (tmi.type==q3::TMOD_ROTATE)
				{
					tmod.setType(vid::ETCMT_ROTATE);
					tmod.setRotateSpeed(tmi.rotateSpeed);

					curr_pass.Layers[b].addTexCoordModifier ( tmod );
				}
				else 
				if (tmi.type==q3::TMOD_ENTITY_TRANSLATE)
				{
				}					
			}
		}			

		curr_pass.setFlag(vid::EMF_MIP_MAP_OFF,
			q3::shader.noMipMaps);
		curr_pass.setFlag(vid::EMF_BLENDING,
			(stage->stateBits&GLS_DEPTHMASK_TRUE) != 0 ? false : true);
		curr_pass.setLightingMode(vid::ELM_NONE);
		curr_pass.setFlag(vid::EMF_FOG_ENABLE, true);
		curr_pass.setDepthTest(s == 0 ? vid::ECT_LESS : vid::ECT_EQUAL);

		if (stage->stateBits&GLS_ATEST_GE_80)
		{
			curr_pass.setAlphaTest ( vid::EAT_GE_80 );
		}
		else
		if (stage->stateBits&GLS_ATEST_LT_80)
		{
			curr_pass.setAlphaTest ( vid::EAT_LT_80 );
		}
		else
		if (stage->stateBits&GLS_ATEST_GT_0)
		{
			curr_pass.setAlphaTest ( vid::EAT_GT_0 );
		}
		else
		{
			curr_pass.setAlphaTest ( vid::EAT_ALWAYS );
		} 			
			
		curr_pass.setBlendFuncs(
			q3::Q3ToMySrcBlendMode(stage->stateBits&0x0000000f), 
			q3::Q3ToMyDstBlendMode(stage->stateBits&0x000000f0));			

		bool zwrite = ( curr_pass.getFlag ( vid::EMF_BLENDING ) || s > 0 ) ? false : true;
		curr_pass.setFlag(vid::EMF_ZWRITE_ENABLE, zwrite);
		if (s > 0)
			material.Material.getPass(0).setFlag(vid::EMF_ZWRITE_ENABLE, true);

		if (shader.cullType==q3::CT_FRONT_SIDED)
		{
			curr_pass.setFlag ( vid::EMF_FRONT_FACE_CCW,	false	);
			curr_pass.setFlag ( vid::EMF_BACK_FACE_CULLING,	true	);
		}
		else
		if (shader.cullType==q3::CT_BACK_SIDED)
		{
			curr_pass.setFlag ( vid::EMF_FRONT_FACE_CCW,	true	);
			curr_pass.setFlag ( vid::EMF_BACK_FACE_CULLING,	true	);
		}
		else
		// CT_TWO_SIDED
		{
			curr_pass.setFlag(vid::EMF_FRONT_FACE_CCW, true);
			curr_pass.setFlag(vid::EMF_BACK_FACE_CULLING, false);
		}

		if (stage->rgbGen == q3::CGEN_CONST)
		{
			curr_pass.setColorGen(vid::ECGT_CUSTOM);

			curr_pass.setDiffuseColor(0xff000000);
			curr_pass.setAmbientColor(0xff000000);				
			curr_pass.setSpecularColor(0xff000000);
			curr_pass.setEmissiveColor(img::SColor(255,
					stage->constantColor[0], 
					stage->constantColor[1], 
					stage->constantColor[2]));
			constColor = true;
		}
		else	
		if (stage->rgbGen==q3::CGEN_WAVEFORM)
		{
			curr_pass.setColorGen(vid::ECGT_FUNCTION);

			curr_pass.setDiffuseColor(0xff000000);
			curr_pass.setAmbientColor(0xff000000);				
			curr_pass.setSpecularColor(0xff000000);
			curr_pass.setEmissiveColor(0xffffffff);

			core::wave colorFunc;
			colorFunc.base     = stage->rgbWave.base;
			colorFunc.amplitude= stage->rgbWave.amplitude; 
			colorFunc.phase    = stage->rgbWave.phase;
			colorFunc.frequency= stage->rgbWave.frequency;

			if (stage->rgbWave.func==q3::GF_SIN)
				colorFunc.func = core::EFT_SIN; 
			else
			if (stage->rgbWave.func==q3::GF_SQUARE)
				colorFunc.func = core::EFT_SQUARE;
			else
			if (stage->rgbWave.func==q3::GF_TRIANGLE)
				colorFunc.func = core::EFT_TRIANGLE;
			else
			if (stage->rgbWave.func==q3::GF_SAWTOOTH)
				colorFunc.func = core::EFT_SAWTOOTH;
			else
			if (stage->rgbWave.func==q3::GF_INVERSE_SAWTOOTH)
				colorFunc.func = core::EFT_INVERSE_SAWTOOTH;
			else
			if (stage->rgbWave.func == q3::GF_NOISE)
				colorFunc.func = core::EFT_NOISE;
			else
				curr_pass.setColorGen(vid::ECGT_CUSTOM);

			curr_pass.setColorFunc(colorFunc);
		}
		else
		//CGEN_VERTEX
		//CGEN_IDENTITY
		//CGEN_EXACT_VERTEX
		//CGEN_IDENTITY_LIGHTING
		//CGEN_ENTITY
		//CGEN_LIGHTING_DIFFUSE
		//CGEN_ONE_MINUS_VERTEX
			curr_pass.setColorGen(vid::ECGT_VERTEX);

		if (stage->alphaGen == q3::AGEN_CONST)
		{	
			curr_pass.setAlphaGen(vid::EAGT_CUSTOM);

			img::SColor dc = curr_pass.getDiffuseColor();
			img::SColor ac = curr_pass.getAmbientColor();
			img::SColor sc = curr_pass.getSpecularColor();
			img::SColor ec = curr_pass.getEmissiveColor();

			dc.setAlpha(stage->constantColor[3]);
			ac.setAlpha(stage->constantColor[3]);
			sc.setAlpha(stage->constantColor[3]);
			ec.setAlpha(stage->constantColor[3]);

			curr_pass.setDiffuseColor(dc);
			curr_pass.setAmbientColor(ac);
			curr_pass.setSpecularColor(sc);
			curr_pass.setEmissiveColor(ec);

			constAlpha = true;
		}
		else	
		if ( stage->alphaGen == q3::AGEN_WAVEFORM )
		{
			curr_pass.setAlphaGen(vid::EAGT_FUNCTION);

			core::wave alphaFunc;
			alphaFunc.base     = stage->alphaWave.base;
			alphaFunc.amplitude= stage->alphaWave.amplitude; 
			alphaFunc.phase    = stage->alphaWave.phase;
			alphaFunc.frequency= stage->alphaWave.frequency;

			if (stage->alphaWave.func==q3::GF_SIN)
				alphaFunc.func = core::EFT_SIN; 
			else
			if (stage->alphaWave.func==q3::GF_SQUARE)
				alphaFunc.func = core::EFT_SQUARE;
			else
			if (stage->alphaWave.func==q3::GF_TRIANGLE)
				alphaFunc.func = core::EFT_TRIANGLE;
			else
			if (stage->alphaWave.func==q3::GF_SAWTOOTH)
				alphaFunc.func = core::EFT_SAWTOOTH;
			else
			if (stage->alphaWave.func==q3::GF_INVERSE_SAWTOOTH)
				alphaFunc.func = core::EFT_INVERSE_SAWTOOTH;
			else
			if (stage->alphaWave.func==q3::GF_NOISE)
				alphaFunc.func = core::EFT_NOISE;
			else
				curr_pass.setAlphaGen ( vid::EAGT_CUSTOM );

			curr_pass.setAlphaFunc ( alphaFunc );
		}
		else
		//AGEN_VERTEX
		//AGEN_IDENTITY
		//AGEN_ENTITY
		//AGEN_ONE_MINUS_ENTITY
		//AGEN_LIGHTING_SPECULAR
		//AGEN_ONE_MINUS_VERTEX
		//AGEN_PORTAL
			curr_pass.setAlphaGen(vid::EAGT_VERTEX);

		if (curr_pass.getColorGen() == vid::ECGT_FUNCTION ||
			curr_pass.getAlphaGen() == vid::EAGT_FUNCTION)
			no_coloured_vertices = true;

		// check the lightmap to be in the second texture unit
		for ( u32 l = 0; l < vid::MY_MATERIAL_MAX_LAYERS; l++ )
		{
			if (l != 1 && curr_pass.Layers[l].getType() == vid::ETLT_LIGHT_MAP)
			{
				// swapping texture layers, setting lighting map to the second
				vid::STextureLayer layer2 = curr_pass.Layers[1];

				if (!layer2.getTexture())
				{
					layer2.setTexture( CQ3LevelMesh::getQ3WhiteImage() );
				}

				curr_pass.Layers[1] = curr_pass.Layers[l];
				curr_pass.Layers[l] = layer2;

				break;
			}
		}
	}
	
	// check the lightmaps in the material

	bool has_lightmap = false;
	for (u32 p = 0; p < material.Material.getPassesCount(); p++)
	{
		vid::SRenderPass &pass = material.Material.getPass(p);	
		for (u32 l = 0; l < vid::MY_MATERIAL_MAX_LAYERS; l++)
		{
			if (pass.Layers[l].getType() == vid::ETLT_LIGHT_MAP)
			{
				has_lightmap = true;
				break;
			}
		}
	}

	if (!has_lightmap)
	{
		material.VertexType = vid::EVT_1TCOORDS_COLOURED;
	}
	else 
	if (material.Material.isTransparent())
	{	
		material.VertexType = vid::EVT_1TCOORDS_COLOURED;
		for (u32 p=0; p < material.Material.getPassesCount(); p++)
		{
			vid::SRenderPass &pass = material.Material.getPass(p);
			if (pass.Layers[1].getType() == vid::ETLT_LIGHT_MAP)
				pass.Layers[1].setTexture(NULL);
		}
	}

	if (!material.Material.isTransparent() &&
		(constColor || constAlpha))
		no_coloured_vertices = true;

	if (no_coloured_vertices)
		material.VertexType = vid::getVertexTypeFromComponents(
			(~vid::EVC_COL)&vid::VertexComponents[material.VertexType]);

	return true;
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
