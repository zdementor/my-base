//|-------------------------------------------------------------------------
//| File:        OGLHelper.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef OGLHelperHPP
#define OGLHelperHPP
//----------------------------------------------------------------------------

static const GLint OGLCompareFunctions[E_COMPARE_TEST_COUNT] =
{
	GL_ALWAYS,
	GL_NEVER,
	GL_LESS,
	GL_LEQUAL,
	GL_EQUAL,
	GL_NOTEQUAL,
	GL_GREATER,
	GL_GEQUAL
};

#define convertToOGLCompareFunction(func) \
	OGLCompareFunctions[func]

//---------------------------------------------------------------------------

static const GLint OGLSrcBlendFunctions[E_SRC_BLEND_FUNCTION_COUNT] =
{
	GL_ONE,
	GL_ZERO,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_SRC_ALPHA_SATURATE,
};

#define convertToOGLSrcBlendFunction(func) \
	OGLSrcBlendFunctions[func]

//---------------------------------------------------------------------------

static const GLint OGLDstBlendFunctions[E_DST_BLEND_FUNCTION_COUNT] =
{		
	GL_ONE,
	GL_ZERO,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
};

#define convertToOGLDstBlendFunction(func) \
	OGLDstBlendFunctions[func]

//---------------------------------------------------------------------------

static const GLint OGLFogModes[E_FOG_TYPE_COUNT] =
{
	GL_LINEAR,
	GL_EXP,
	GL_EXP2,
};

#define convertToOGLFogMode(mode) \
	OGLFogModes[mode]

//---------------------------------------------------------------------------

static const GLint OGLStencilOps[E_STENCIL_OPERATION_COUNT] =
{
	GL_KEEP,
	GL_ZERO,
	GL_REPLACE,
	0x8507, //GL_INCR_WRAP_EXT,
	0x8508, //GL_DECR_WRAP_EXT,
	GL_INVERT,
	GL_INCR,
	GL_DECR,
};

#define convertToOGLStencilOp(op) \
	OGLStencilOps[op]

//---------------------------------------------------------------------------

static const GLint OGLPrimitiveTypes[E_DRAW_PRIMITIVE_TYPE_COUNT] =
{
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
	GL_POINTS,
    GL_LINES,
    GL_LINE_STRIP,
};

#define convertToOGLPrimitiveType(type) \
	OGLPrimitiveTypes[type]

//----------------------------------------------------------------------------

//! test if an error occurred, prints the problem, and returns 
//! true if an error happened
inline bool testGLError()
{
#ifdef MY_DEBUG_MODE
	GLenum g = glGetError();
	u32 err_cnt = 0;
	while (g != GL_NO_ERROR)
	{
		err_cnt++;
		switch(g)
		{
		case GL_INVALID_ENUM:
			LOGGER.log("GL_INVALID_ENUM", io::ELL_ERROR); break;
		case GL_INVALID_VALUE:
			LOGGER.log("GL_INVALID_VALUE", io::ELL_ERROR); break;
		case GL_INVALID_OPERATION:
			LOGGER.log("GL_INVALID_OPERATION", io::ELL_ERROR); break;
		case GL_STACK_OVERFLOW:
			LOGGER.log("GL_STACK_OVERFLOW", io::ELL_ERROR); break;
		case GL_STACK_UNDERFLOW:
			LOGGER.log("GL_STACK_UNDERFLOW", io::ELL_ERROR); break;
		case GL_OUT_OF_MEMORY:
			LOGGER.log("GL_OUT_OF_MEMORY", io::ELL_ERROR); break;
		default:
			LOGGER.log("Unknown GL error", io::ELL_ERROR); break;
		};
		g = glGetError();
	};
    return err_cnt > 0;
#endif
    return false;   
}

//----------------------------------------------------------------------------

static const GLenum OGLBlendEquations[E_BLEND_EQUATION_COUNT] =
{
	0x8006, // GL_FUNC_ADD,
	0x800A, // GL_FUNC_SUBTRACT,
	0x800B, // GL_FUNC_REVERSE_SUBTRACT,
	0x8007, // GL_MIN,
	0x8008, // GL_MAX,
};

#define convertToOGLBlendEquation(eq) \
	OGLBlendEquations[eq]

//---------------------------------------------------------------------------

static const GLenum OGLLockMode[E_RENDER_BUFFER_LOCK_MODE_COUNT] =
{		
	0x88B9, // GL_WRITE_ONLY_ARB
	0x88B8, // GL_READ_ONLY_ARB
	0x88BA, // GL_READ_WRITE_ARB
};

#define convertToOGLLockMode(mode) \
	OGLLockMode[mode]

//----------------------------------------------------------------------------

#endif // OGLHelperHPP