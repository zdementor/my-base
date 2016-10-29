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

//----------------------------------------------------------------------------

#define convertToOGLLockMode(mode) \
	OGLLockMode[mode]

#define MY_ENABLE_CLIENT_STATE(index, state, preFunc) \
	if (!enabledAttribs[index]) \
	{ \
		preFunc; \
		glEnableClientState(state); \
		enabledAttribs[index] = true; \
	}

#define MY_DISABLE_CLIENT_STATE(index, state, preFunc) \
	if (enabledAttribs[index]) \
	{ \
		preFunc; \
		glDisableClientState(state); \
		enabledAttribs[index] = false; \
	}

MY_FORCEINLINE void setupGLAttributes(
	bool *enabledAttribs,
	GLenum type0 = GL_NONE, s32 size0 = 0, s32 stride0 = 0, const void *ptr0 = NULL,
	GLenum type1 = GL_NONE, s32 size1 = 0, s32 stride1 = 0, const void *ptr1 = NULL,
	GLenum type2 = GL_NONE, s32 size2 = 0, s32 stride2 = 0, const void *ptr2 = NULL,
	GLenum type3 = GL_NONE, s32 size3 = 0, s32 stride3 = 0, const void *ptr3 = NULL,
	GLenum type4 = GL_NONE, s32 size4 = 0, s32 stride4 = 0, const void *ptr4 = NULL,
	GLenum type5 = GL_NONE, s32 size5 = 0, s32 stride5 = 0, const void *ptr5 = NULL,
	GLenum type6 = GL_NONE, s32 size6 = 0, s32 stride6 = 0, const void *ptr6 = NULL
	)
{
	(void)size1; // normal

	if (type0 != GL_NONE)
	{
		MY_ENABLE_CLIENT_STATE(0, GL_VERTEX_ARRAY, (void)0)
		glVertexPointer(size0, type0, stride0, ptr0);
	}
	else
	{
		MY_DISABLE_CLIENT_STATE(0, GL_VERTEX_ARRAY, (void)0)
	}

	if (type1 != GL_NONE)
	{
		MY_ENABLE_CLIENT_STATE(1, GL_NORMAL_ARRAY, (void)0)
		glNormalPointer(type1, stride1, ptr1);
	}
	else
	{
		MY_DISABLE_CLIENT_STATE(1, GL_NORMAL_ARRAY, (void)0)
	}

	if (type2 != GL_NONE)
	{
		MY_ENABLE_CLIENT_STATE(2, GL_COLOR_ARRAY, (void)0)
		glColorPointer(size2, type2, stride2, ptr2);
	}
	else
	{
		MY_DISABLE_CLIENT_STATE(2, GL_COLOR_ARRAY, (void)0)
	}

	if (type3 != GL_NONE)
	{
#ifdef GL_VERSION_1_2
		glClientActiveTexture(GL_TEXTURE0);
#endif
		MY_ENABLE_CLIENT_STATE(3, GL_TEXTURE_COORD_ARRAY, (void)0)
		glTexCoordPointer(size3, type3, stride3, ptr3);
	}
	else
	{
#ifdef GL_VERSION_1_2
		MY_DISABLE_CLIENT_STATE(3, GL_TEXTURE_COORD_ARRAY, glClientActiveTexture(GL_TEXTURE0))
#else
		MY_DISABLE_CLIENT_STATE(3, GL_TEXTURE_COORD_ARRAY, (void)0)
#endif
	}

#ifdef GL_VERSION_1_2
	if (type4 != GL_NONE)
	{
		glClientActiveTexture(GL_TEXTURE1);
		MY_ENABLE_CLIENT_STATE(4, GL_TEXTURE_COORD_ARRAY, (void)0)
		glTexCoordPointer(size4, type4, stride4, ptr4);
	}
	else
	{
		MY_DISABLE_CLIENT_STATE(4, GL_TEXTURE_COORD_ARRAY, glClientActiveTexture(GL_TEXTURE1))
	}

	if (type5 != GL_NONE)
	{
		glClientActiveTexture(GL_TEXTURE2);
		MY_ENABLE_CLIENT_STATE(5, GL_TEXTURE_COORD_ARRAY, (void)0)
		glTexCoordPointer(size5, type5, stride5, ptr5);
	}
	else
	{
		MY_DISABLE_CLIENT_STATE(5, GL_TEXTURE_COORD_ARRAY, glClientActiveTexture(GL_TEXTURE2))
	}

	if (type6 != GL_NONE)
	{
		glClientActiveTexture(GL_TEXTURE3);
		MY_ENABLE_CLIENT_STATE(6, GL_TEXTURE_COORD_ARRAY, (void)0)
		glTexCoordPointer(size6, type6, stride6, ptr6);
	}
	else
	{
		MY_DISABLE_CLIENT_STATE(6, GL_TEXTURE_COORD_ARRAY, glClientActiveTexture(GL_TEXTURE3))
	}
#endif
}

//----------------------------------------------------------------------------

#endif // OGLHelperHPP
