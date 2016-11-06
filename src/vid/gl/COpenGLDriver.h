//|-------------------------------------------------------------------------
//| File:        COpenGLDriver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef COpenGLDriverHPP
#define COpenGLDriverHPP
//---------------------------------------------------------------------------

#include "OpenGL.h"

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

#include "OGLHelper.h"

class COpenGLTexture;
class COpenGLHardwareOcclusionQuery;

class COpenGLDriver : public CNullDriver
{
public:

#ifdef WIN32
    //! win32 constructor
    COpenGLDriver(const core::dimension2d<s32>& screenSize);
#endif

    virtual ~COpenGLDriver();

    bool queryFeature(E_VIDEO_DRIVER_FEATURE feature);

    virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);

    virtual void deleteAllDynamicLights();

    virtual void setGlobalAmbientColor(const img::SColorf& color);

    virtual void setViewPort(const core::rect<s32>& area);

    virtual void setFog(const SFog &fog);

    virtual void OnResize(const core::dimension2d<s32>& size);

	virtual void setTextureFilter(E_TEXTURE_FILTER textureFilter);

	virtual ITexture* createRenderTargetTexture(
		const core::dimension2di &size, img::E_COLOR_FORMAT format);

	virtual IRenderTarget* createRenderTarget();
	virtual IRenderTarget* createRenderTarget(const core::dimension2di &size,
		img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat);
	virtual IRenderTarget* createRenderTarget(
		ITexture *colorTexture, ITexture *depthTexture);
        
	virtual bool setRenderTarget(IRenderTarget *rt);

	virtual const core::vector3df& get3DPositionFromScreenCoordinates(
		const core::position2di &scr_pos) const;

	virtual IHardwareOcclusionQuery& getHardwareOcclusionQuery();

    GLuint createEmptyOGLTexture(s32 w, s32 h);

	virtual void setPolygonFillMode(E_POLYGON_FILL_MODE mode);

	virtual void resetTextureUnits();

	virtual IRenderBuffer * createRenderBuffer(
		E_RENDER_BUFFER_TYPE rbt,
		E_VERTEX_TYPE vtype, u32 vert_size,
		E_INDEX_TYPE itype, u32 ind_size,
		E_DRAW_PRIMITIVE_TYPE dpt);

	virtual IRenderBuffer * createDynamicRenderBuffer(
		const void * varray, E_VERTEX_TYPE vtype, u32 vert_size,
		const void * iarray, E_INDEX_TYPE itype, u32 ind_size,
		E_DRAW_PRIMITIVE_TYPE dpt);

	virtual void applyDynamicLights();

	virtual void setColorMask(bool r, bool g, bool b, bool a);

    virtual void clearDepth();
	virtual void clearStencil();
	virtual void clearColor(u8 r, u8 g, u8 b, u8 a);

	virtual void render2DRect(const SMaterial &material,
		const core::rectf &drawRect, const core::rectf &texRect);
	virtual void render2DRectWithLight(const SMaterial &material,
		const core::rectf &drawRect, const core::rectf &texRect,
		u32 enabledLight);

	virtual void renderPass(E_RENDER_PASS pass);

	virtual bool _bindGPUProgram(IGPUProgram* gpu_prog);

	void _setupTextureCoords(
		const void* tc = 0, const void* tc2 = 0,
		const void* tangents = 0, const void* binormals = 0,
		s32 tc_stride = 0);

	virtual void _setTexture(s32 stage, ITexture* texture);

	virtual bool _initDriver(SExposedVideoData &out_video_data);

	virtual bool _makeScreenShot(img::IImage *image);

private:

	virtual void _enableStencil();
	virtual void _setupStencil(
		E_STENCIL_OPERATION fail,  // to do if stencil test fails
		E_STENCIL_OPERATION zfail, // to do if stencil test passes and Z test fails
		E_STENCIL_OPERATION zpass, // to do if both stencil and Z tests pass
		E_COMPARE_TEST  func = ECT_ALWAYS,  // Stencil Test passes if ((ref & mask) func (stencil & mask)) is true
		s32 ref=0,        // Reference value used in stencil test
		u32 mask=0xFFFFFFFF // Mask value used in stencil test
		);
	virtual void _disableStencil();

	virtual void _enableScissor();
	virtual void _setupScissor(s32 left, s32 top, u32 width, u32 height);
	virtual void _disableScissor();

	virtual void _renderScreenRect(const img::SColor &color);

	virtual void _renderStencilVolume(IRenderBuffer *rbuf, const SRenderPass &pass, bool zfail);

	virtual bool _beginRendering();
	virtual bool _endRendering();
	virtual bool _swapBuffers();

	//! draws an indexed triangle list
	template <typename TIndexType>
	inline void drawIndexedTriangleListTemplate(
		const S3DVertexSimple* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);
	template <typename TIndexType>
	inline void drawIndexedTriangleListTemplate(
		const S3DVertex1TCoords* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);
	template <typename TIndexType>
	inline void drawIndexedTriangleListTemplate(
		const S3DVertex1TCoordsColoured* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);
	template <typename TIndexType>
	inline void drawIndexedTriangleListTemplate(
		const S3DVertex2TCoords* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);
	template <typename TIndexType>
	inline void drawIndexedTriangleListTemplate(
		const S3DVertex2TCoordsColoured* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);
	template <typename TIndexType>
	inline void drawIndexedTriangleListTemplate(
		const S3DVertex1TCoordsTBN* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);
	template <typename TIndexType>
	inline void drawIndexedTriangleListTemplate(
		const S3DVertex1TCoordsTBNColoured* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);
	template <typename TIndexType>
	inline void drawIndexedTriangleListTemplate(
		const S3DVertex2TCoordsTBN* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);
	template <typename TIndexType>
	inline void drawIndexedTriangleListTemplate(
		const S3DVertex2TCoordsTBNColoured* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);

	//! draws an indexed triangle fan
	template <typename TIndexType>
	inline void drawIndexedTriangleFanTemplate(
		const S3DVertex1TCoords* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);
	template <typename TIndexType>
	inline void drawIndexedTriangleFanTemplate(
		const S3DVertex1TCoordsColoured* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);
	template <typename TIndexType>
	inline void drawIndexedTriangleFanTemplate(
		const S3DVertex2TCoords* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);
	template <typename TIndexType>
	inline void drawIndexedTriangleFanTemplate(
		const S3DVertex2TCoordsColoured* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);

	//! draws a static indexed triangle list
	template <typename T, typename TIndexType>
	inline void drawStaticIndexedTriangleListTemplate(
		IRenderBuffer*& vbo_inout, 
		const T * vertices, s32 vertexCount, const TIndexType *indexList, s32 triangleCount 
		);

	//!
	template <class TVert, typename TInd>
	inline void drawTemplate(
		const TVert * vertices, u32 vert_size, const TInd * indices, u32 ind_size,
		E_DRAW_PRIMITIVE_TYPE dpt );

	void polygonOffsetEnable(f32 factor, f32 units);
	void polygonOffsetDisable();

	//!
	template < class TVert >
	IRenderBuffer * createRenderBufferTemplate(
		E_RENDER_BUFFER_TYPE rbt,
		u32 vert_size, E_INDEX_TYPE itype, u32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt );

	template < class TVert >
	IRenderBuffer * createDynamicRenderBufferTemplate(
		const void * varray, u32 vert_size,
		const void * iarray, E_INDEX_TYPE itype, u32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt );
 
    //! returns a device dependent texture from a software surface (IImage)
    //! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
    virtual ITexture* _createDeviceDependentTexture(img::IImage* surface);
	virtual vid::ITexture* _createDeviceDependentTexture(core::dimension2di &size, img::E_COLOR_FORMAT format);

    inline void createGLMatrix(GLfloat gl_matrix[16], const core::matrix4& m)
    {
        s32 i = 0;
        for (s32 r=0; r<4; ++r)
            for (s32 c=0; c<4; ++c)
                gl_matrix[i++] = m(c,r);
    }

    virtual void _setVertexType(E_VERTEX_TYPE newType);
    virtual void _setBasicRenderStates();
	virtual void _setFFPRenderStates();
	virtual void _setShaderRenderStates();

    //! prints error if an error happened.
    void printGLError();

	virtual CNullGPUProgram* _createGPUProgram(u32 uniforms, u32 attributes, u32 lightcnt,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
	E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader);

    core::array<s32> ColorBuffer;

	MyGLWindow	*m_Window;
	MyGLContext *m_Context;
    
	CNullHardwareOcclusionQuery *m_OpenGLHardwareOcclusionQuery;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif

