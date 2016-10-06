//|-------------------------------------------------------------------------
//| File:        CD3D9Driver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CD3D9DriverHPP
#define CD3D9DriverHPP
//----------------------------------------------------------------------------

#include "../CNullDriver.h"
#include "CD3D9HardwareOcclusionQuery.h"

#define WIN32_LEAN_AND_MEAN

#include <d3d/d3d9.h>

#define FOURCC_INTZ ((D3DFORMAT)(MAKEFOURCC('I','N','T','Z'))) 

//----------------------------------------------------------------------------
namespace my {
namespace vid {	
//----------------------------------------------------------------------------

class CD3D9Driver : public CNullDriver
{
public:

    CD3D9Driver(const core::dimension2d<s32>& screenSize);

    virtual ~CD3D9Driver();

    bool initDriver(SExposedVideoData &out_video_data);

    virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature);

    virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);

    virtual void setViewPort(const core::rect<s32>& area);

    virtual void deleteAllDynamicLights();

    virtual void setGlobalAmbientColor(const img::SColorf& color);

    virtual s32 getMaximalPrimitiveCount();

    virtual void setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled);

    virtual void setFog(const SFog& fog);

    virtual void OnResize(const core::dimension2d<s32>& size);

	virtual ITexture* createRenderTargetTexture(
		const core::dimension2di &size, img::E_COLOR_FORMAT format);

	virtual IRenderTarget* addRenderTarget(const core::dimension2di &size,
		img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat);
	virtual IRenderTarget* addRenderTarget(
		ITexture *colorTexture, ITexture *depthTexture);
        
	virtual IHardwareOcclusionQuery& getHardwareOcclusionQuery();

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

	virtual bool setRenderContextCurrent();
	virtual bool setNullContextCurrent();

	virtual bool _bindGPUProgram(IGPUProgram *gpu_prog);

	virtual void _setTexture(s32 stage, ITexture *texture);

	IDirect3DDevice9 * _getDirect3DDevice()
	{ return m_D3DDevice; }

	IDirect3DSurface9* _getMainRenderTargetSurface()
	{ return m_D3DMainRenderTargetSurface; }

	IDirect3DSurface9* _getMainDepthStencilSurface()
	{ return m_D3DMainDepthStencilSurface; }

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

	virtual void _renderStencilVolume(IRenderBuffer *rbuf, const SRenderPass &pass, bool zfail);

	virtual bool _beginRendering();
	virtual bool _endRendering();
	virtual bool _swapBuffers();

	//!
	template <typename T, typename TIndexType>
	void  drawIndexedTriangleListTemplate(
		const T* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);

	//!
	template <typename T, typename TIndexType>
	void  drawIndexedTriangleFanTemplate(
		const T* vertices, s32 vertexCount, const TIndexType* indexList, s32 triangleCount
		);

	//!
	template <typename T, typename TIndexType>
	void drawStaticIndexedTriangleListTemplate( 
		IRenderBuffer*& vbo_inout, 
		const T* vertices, s32 vertexCount, 
		const TIndexType *indexList, s32 triangleCount 
		);

	//!
	template <class TVert, class TInd>
	void drawTemplate(
		const TVert * vertices, u32 vert_size, const TInd * indices, u32 ind_size,
		E_DRAW_PRIMITIVE_TYPE dpt );

	//!
	template < class TVert >
	IRenderBuffer * createRenderBufferTemplate(
		E_RENDER_BUFFER_TYPE rbt,
		u32 vert_size, E_INDEX_TYPE itype, u32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt );

	template < class TVert >
	IRenderBuffer * createDynamicRenderBufferTemplate(
		const void * varray, u32 vert_size,
		const void * iarray, E_INDEX_TYPE itype, u32 ind_size, E_DRAW_PRIMITIVE_TYPE dpt );

    //! set gamma correction value, directx only at the moment patch by reaper
    void setGammaRamp(f32 gamme, f32 contrast, f32 brightness);

    //! Stencil Fog Texture
    IDirect3DTexture9* StencilFogTexture;  
    
    virtual void _setVertexType(E_VERTEX_TYPE newType);
    virtual void _setBasicRenderStates();
	virtual void _setFFPRenderStates();
	virtual void _setShaderRenderStates();

    //! resets the device
    bool reset();	

    //! returns a device dependent texture from a software surface (IImage)
    //! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
    virtual ITexture* _createDeviceDependentTexture(img::IImage* surface);
	virtual ITexture* _createDeviceDependentTexture(core::dimension2di &size, img::E_COLOR_FORMAT format);

	//! Create An Empty D3D9 Texture
    IDirect3DTexture9 * createEmptyD3D9Texture(s32 w, s32 h);

	virtual CNullGPUProgram* _createGPUProgram(u32 uniforms, u32 lightcnt,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader);

    D3DPRESENT_PARAMETERS m_PresentParams;

    HINSTANCE m_D3DLibrary;
    IDirect3D9* m_D3D;
    IDirect3DDevice9* m_D3DDevice;

	IDirect3DSurface9* m_D3DMainRenderTargetSurface;
	IDirect3DSurface9* m_D3DMainDepthStencilSurface;

    D3DCAPS9 m_D3DCaps;

    f32 MaxLightDistance;
    bool DeviceLost;
	
    DWORD MaxAnisotropyLevel;

    struct D3DCEGUIQuadVertex 
	{
		D3DCEGUIQuadVertex() {}
		D3DCEGUIQuadVertex(	
			FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _rhw,	
			DWORD _diffuse,	float _tu1, float _tv1 
			): x(_x), y(_y), z(_z), rhw(_rhw), diffuse(_diffuse), 
			tu1(_tu1), tv1(_tv1) {;}

        FLOAT x, y, z, rhw;     //!< The transformed position for the vertex.
        DWORD diffuse;          //!< colour of the vertex
        float tu1, tv1;         //!< texture coordinates
    };

	// DirectX allows the insertion of tokens, called "events," into the command buffer 
	// and then allows to check whether the event has been processed. 
	IDirect3DQuery9 *pEventQuery;

	CD3D9HardwareOcclusionQuery* m_D3D9HardwareOcclusionQuery;

	IDirect3DVertexDeclaration9* m_VertexDecls[E_VERTEX_TYPE_COUNT];

	bool m_DepthStencilTexturesSupport;
};

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif // CD3D9DriverHPP
