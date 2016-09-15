//|-------------------------------------------------------------------------
//| File:        CNullDriver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CNullDriverHPP
#define CNullDriverHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <vid/IVideoDriver.h>
#include <io/IFileSystem.h>
#include <img/IImagePresenter.h>
#include <img/IImageLoader.h>
#include <vid/S3DVertex.h>
#include <scn/IMeshBuffer.h>
#include <img/ISurfaceLoader.h>
#include <img/IImageLibrary.h>
#include <vid/IRenderBuffer.h>
#include <vid/SFog.h>
#include <io/ILogger.h>
#include <vid/S3DVertex.h>
#include <IRunnable.h>
#include <vid/SLight.h>

#include "CVideoModeList.h"
#include "CNullHardwareOcclusionQuery.h"
#include "CNullGPUProgram.h"
#include "CNullRenderTarget.h"
#include "SRenderPool.h"

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

const s32 MY_MAX_VERTICES_PER_PRIMITIVE_ARRAY  = 65535;
const s32 MY_MAX_TRIANGLES_PER_PRIMITIVE_ARRAY = 65535;

const u32 MY_STENCIL_ZERO_VALUE = 127;

const u32 MY_MAX_POINT_SPECULAR_LIGHTS = 3;

#define PRL_MAX_SHADER_LIGHTS	32
#define PRL_MAX_FFP_LIGHTS		8

//---------------------------------------------------------------------------

class CNullDriver : public IVideoDriver
{
public:

    CNullDriver(const core::dimension2d<s32>& screenSize);
    virtual ~CNullDriver();

	virtual img::E_COLOR_FORMAT getBackColorFormat() { return m_BackColorFormat; }

    virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature)
	{ return false; }

    virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);

    virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state);

    virtual const SRenderPass& getLastRenderPass();

    virtual const SRenderPass& getRenderPass();

    virtual void setRenderPass(const SRenderPass& material);

    virtual void setViewPort(const core::rect<s32>& area);
	virtual void setViewPort(s32 left, s32 top, s32 right, s32 bottom);

    virtual const core::rect<s32>& getViewPort() const;

    virtual void setFog(const SFog &fog);

    virtual const SFog& getFog();

	virtual void setGroundFogColor(img::SColor gfogcol);

	virtual img::SColor getGroundFogColor();

    virtual const core::dimension2d<s32>& getScreenSize() const;

    virtual s32 getCurrentFPS();

    virtual s32 getAverageFPS();

    virtual u32 getRenderedTrianglesCount ( E_RENDER_PASS pass );
	virtual u32 getRenderedTrianglesCount ();

	virtual u32 getRenderedDIPsCount ( E_RENDER_PASS pass );
	virtual u32 getRenderedDIPsCount ();

    virtual u32 getMaximalTextureUnitsAmount() const;

    virtual const core::dimension2di& getMaximalTextureSize() const;
	    
    virtual void setGlobalAmbientColor(const img::SColorf& color);

    virtual const img::SColorf& getGlobalAmbientColor() const;

    virtual void setGlobalLightParams(const SLight& light);
		
    virtual const SLight& getGlobalLightParams();

    virtual ITexture* getTexture(const c8* filename);
	virtual ITexture* getTexture(io::IReadFile* file);
    virtual vid::ITexture* findTexture(const c8 *name);
    virtual ITexture* addTexture(const core::dimension2di &size,
		const c8 *name, img::E_COLOR_FORMAT format = img::ECF_A8R8G8B8);
    virtual ITexture* addTexture(const c8 *name, img::IImage *image);
	virtual ITextureAnim* addTextureAnim( 
		core::array<ITexture*> &frames, SAnimatedTextureParams &params);
    virtual ITexture* createTexture(img::IImage* image);
    virtual ITexture* createTexture(core::dimension2di &size, img::E_COLOR_FORMAT format);
	virtual ITexture* createRenderTargetTexture(
		const core::dimension2di &size, img::E_COLOR_FORMAT colorFormat)
	{ return 0; }
    virtual bool removeTexture(ITexture* texture);

    virtual IRenderTarget* addRenderTarget(u32 width, u32 height,
		img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat)
	{ return addRenderTarget(core::dimension2di(width, height), colorFormat, depthFormat); }
    virtual IRenderTarget* addRenderTarget(const core::dimension2di &size,
		img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat)
	{ return 0; }
	virtual bool removeRenderTarget(IRenderTarget *rt);

	virtual bool setColorRenderTarget(ITexture* rtt,
		bool clearBackBuffer, bool clearZBuffer, img::SColor color);
	virtual ITexture* getColorRenderTarget();

	virtual bool setRenderTarget(IRenderTarget *rt);
	virtual IRenderTarget* getRenderTarget();

	virtual const c8* findTextureName(vid::ITexture *texture);
	virtual bool setTextureName(vid::ITexture *texture, const c8 *name);

    virtual const c8* getName()
	{ return DriverTypeReadableName[m_DriverType]; }

    virtual s32 getMaximalPrimitiveCount();

    virtual void setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled);

    virtual bool getTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag);

    virtual void OnResize(const core::dimension2d<s32>& size);

    virtual E_DRIVER_TYPE getDriverType()
	{ return m_DriverType; }

    virtual E_DRIVER_FAMILY getDriverFamily()
	{ return vid::getDriverFamily(m_DriverType); }
  
    virtual void makeScreenShot(ITexture* texture) {}

    virtual ITexture* makeScreenShot(void);

    void maxIndexWarning(u8 idxSize);

    virtual s32 getStencilFogTextureSize();

	virtual void clearDepth() {}
	virtual void clearStencil() {}
	virtual void clearColor(const img::SColor &color)
		{ clearColor(color.red, color.green, color.blue, color.alpha); }
	virtual void clearColor(u8 r, u8 g, u8 b, u8 a) {}

	virtual void setColorMask(bool r, bool g, bool b, bool a)
	{
		m_ColorMask = (r * ECM_RED) | (g * ECM_GREEN) | (b * ECM_BLUE) | (a * ECM_ALPHA);
	}

	virtual void setColorMask(u32 mask)
	{
		setColorMask(
			!!(mask & ECM_RED), !!(mask & ECM_GREEN), !!(mask & ECM_BLUE), !!(mask & ECM_ALPHA));
	}

	virtual u32 getColorMask() const
	{ return m_ColorMask; }

    virtual void setBackgroundColor(const img::SColor &color);
    virtual const img::SColor& getBackgroundColor();

	virtual void render2DRect(const SMaterial &material,
		const core::rectf &drawRect, const core::rectf &texRect) {}

	virtual void setTextureFilter(E_TEXTURE_FILTER textureFilter);

	virtual E_TEXTURE_FILTER getTextureFilter();

	virtual u32 getMaxAnisotropyLevel()
	{ return MaxAnisotropyLevel; }

    virtual void clearUnusedVideoCache();

    //! \return Returns a pointer to a list with all video modes supported
    //! by the gfx adapter.
    virtual IVideoModeList* getVideoModeList();

	virtual const core::position2di& getScreenCoordinatesFrom3DPosition(
		const core::vector3df &pos3d) const;
	virtual const core::position2df& getScreenRelativeCoordinatesFrom3DPosition(
		const core::vector3df &pos3d) const;
	virtual const core::position2df& getScreenProjectionCoordinatesFrom3DPosition(
		const core::vector3df &pos3d) const;

	virtual const core::recti& getScreenRectFrom3DVolume(
		const core::aabbox3df &box3d) const;

	virtual const core::vector3df& get3DPositionFromScreenCoordinates (
		const core::position2d<s32> &scr_pos) const;

	//! return hardware occlusion object
	virtual IHardwareOcclusionQuery& getHardwareOcclusionQuery();

	virtual u32 getMinimalMeshBufferSizeForVBORendering();

	virtual void setNormalMapCreationAmplitude(f32 amplitude);

	virtual f32 getNormalMapCreationAmplitude();

	//! Setting polygon fill mode
	virtual void setPolygonFillMode(E_POLYGON_FILL_MODE mode);

	//! Getting polygon fill mode
	virtual E_POLYGON_FILL_MODE getPolygonFillMode();

	//! Reseting all texture units
	virtual void resetTextureUnits();

	//! Turning off all texture units
	virtual void turnOffTextureUnits();

	//! Creating render buffer
	virtual IRenderBuffer * createRenderBuffer(
		E_RENDER_BUFFER_TYPE rbt,
		E_VERTEX_TYPE vtype, u32 vert_size,
		E_INDEX_TYPE itype, u32 ind_size,
		E_DRAW_PRIMITIVE_TYPE dpt )
	{ LOGGER.logErr("Can't create render buffer for NULL video driver"); return NULL; }

	virtual IRenderBuffer * createDynamicRenderBuffer(
		const void * varray, E_VERTEX_TYPE vtype, u32 vert_size,
		const void * iarray, E_INDEX_TYPE itype, u32 ind_size,
		E_DRAW_PRIMITIVE_TYPE dpt )
	{ LOGGER.logErr("Can't create dynamic render buffer for NULL video driver"); return NULL; }

    virtual IFont* getFont ( const c8* filename );

    virtual IFont* getBuiltInFont ();

	virtual void registerPoolForRendering(
		E_RENDER_PASS pass, const core::matrix4 &transf,
		const SLight *light0, u64 enabled_lights,
		const core::array <const SMaterial* > &materials,
		const core::array <IRenderBuffer*> &render_buffers, f32 zorder = 0.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL);

	virtual void registerPoolForRendering(
		E_RENDER_PASS pass, const core::matrix4 &transf,
		s32 enabled_light_idx,
		const core::array <const SMaterial*> &materials,
		const core::array <IRenderBuffer*> &render_buffers, f32 zorder = 0.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL);

	virtual void registerPoolForRendering(
		E_RENDER_PASS pass, const core::matrix4 &transf,
		const SLight *light0, u64 enabled_lights,
		const SMaterial &material,
		IRenderBuffer &render_buffer, f32 zorder = 0.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL);

	virtual void registerPoolForRendering(
		E_RENDER_PASS pass, const core::matrix4 &transf,
		s32 enabled_light_idx,
		const SMaterial &material,
		IRenderBuffer &render_buffer, f32 zorder = 0.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL);

	virtual void registerGeometryForRendering(
		E_RENDER_PASS pass, const core::matrix4 &transf,
		const SLight *light0, u64 enabled_lights,
		const SMaterial &material,
		void * vertices, u32 vert_count, E_VERTEX_TYPE vtype,
		void * indices, u32 ind_count, E_INDEX_TYPE itype,
		E_DRAW_PRIMITIVE_TYPE dpt, f32 zorder = 0.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL);

	virtual void registerGeometryForRendering(
		E_RENDER_PASS pass, const core::matrix4 & transf,
		s32 enabled_light_idx,
		const SMaterial & material,
		void * vertices, u32 vert_count, E_VERTEX_TYPE vtype,
		void * indices, u32 ind_count, E_INDEX_TYPE itype,
		E_DRAW_PRIMITIVE_TYPE dpt, f32 zorder = 0.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL);

	virtual SGeometryBufferEntry registerGeometryBufferForRendering(
		E_RENDER_PASS pass, const core::matrix4 &transf,
		const SLight *light0, u64 enabled_lights,
		const SMaterial &material,
		u32 vert_count, E_VERTEX_TYPE vtype,
		u32 ind_count, E_INDEX_TYPE itype,
		E_DRAW_PRIMITIVE_TYPE dpt, f32 zorder = -1.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL);

	virtual SGeometryBufferEntry registerGeometryBufferForRendering(
		E_RENDER_PASS pass, const core::matrix4 &transf,
		s32 enabled_light_idx,
		const SMaterial &material,
		u32 vert_count, E_VERTEX_TYPE vtype,
		u32 ind_count, E_INDEX_TYPE itype,
		E_DRAW_PRIMITIVE_TYPE dpt, f32 zorder = -1.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL);

    virtual void register2DImageForRendering(
		const SMaterial & material,
		const core::rectf & drawRect, const core::rectf & texRect,
		const img::SColor & color = img::SColor(255,255,255,255),
		E_RENDER_MODE mode = ERM_RENDER_STANDARD);
	
    virtual void register2DImageForRendering(
		const SMaterial & material,
		const core::rectf & drawRect, const core::rectf & texRect,
		const img::SColor & color0,	const img::SColor & color1,
		const img::SColor & color2,	const img::SColor & color3,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD);

    virtual void register2DImageForRendering(
		ITexture * texture,
		const core::rectf & drawRect, const core::rectf & texRect, 
        const img::SColor & color = img::SColor(255,255,255,255),
        bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD);

    virtual void register2DImageForRendering(
		ITexture * texture,
		const core::rectf & drawRect, const core::rectf & texRect, 
        const img::SColor & color0,	const img::SColor & color1,
		const img::SColor & color2,	const img::SColor & color3,
        bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD);

	virtual void registerGUIImageForRendering(
		ITexture * texture, f32 z,
		const core::rectf & drawRect, const core::rectf & texRect, 
        const img::SColor & color = img::SColor(255,255,255,255), 
        bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD);

    virtual void registerGUIImageForRendering(
		ITexture * texture, f32 z,
		const core::rectf & drawRect, const core::rectf & texRect, 
        const img::SColor & color0,	const img::SColor & color1,
		const img::SColor & color2,	const img::SColor & color3,
        bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD);

    virtual void registerGUIImageForRendering(
		ITexture * texture,
		const core::rectf & drawRect, const core::rectf & texRect, 
        const img::SColor & color = img::SColor(255,255,255,255), 
        bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD);

    virtual void registerGUIImageForRendering(
		ITexture * texture,
		const core::rectf & drawRect, const core::rectf & texRect, 
        const img::SColor & color0,	const img::SColor & color1,
		const img::SColor & color2,	const img::SColor & color3,
        bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD);

	virtual void registerGUIRectangleForRendering(
		const core::rectf & drawRect,
		const img::SColor & color = img::SColor(255,255,255,255),
		bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD);

    virtual void registerGUIRectangleForRendering(
		const core::rectf & drawRect,
		const img::SColor & color0,	const img::SColor & color1,
		const img::SColor & color2,	const img::SColor & color3,
		bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD);

	virtual void register3DLineForRendering(
		const core::matrix4 &transf,
		const core::vector3df &start, const core::vector3df &end,
		const img::SColor &color,
		bool depth_test,
		bool useAlphaBlending, bool useColorBlending,
		E_RENDER_MODE mode);

	virtual void register3DBoxForRendering(
		const core::matrix4 &transf,
		const core::aabbox3df &box, const img::SColor &color,
		bool depth_test,
		bool useAlphaBlending, bool useColorBlending,
		E_RENDER_MODE mode);

	virtual bool setRenderContextCurrent()
	{ LOGGER.logErr("Invalid call %s", __FUNCTION__ ); return false; }

	virtual bool setNullContextCurrent()
	{ LOGGER.logErr("Invalid call %s", __FUNCTION__ ); return false; }

	virtual bool beginRendering();
	virtual void renderAll();
	virtual void renderPass(E_RENDER_PASS pass);
	virtual void endRendering();

	virtual bool isRendering();

    virtual const u32 & getDynamicLightsMaximalAmount() const;
    virtual s32 getDynamicLightsCount();
    virtual void deleteAllDynamicLights();
    virtual void addDynamicLight(const SLight& light);
    virtual const SLight& getDynamicLight(s32 idx);
	virtual void setDynamicLight(s32 idx, const SLight & light);
    virtual void setDynamicLightEnabled(s32 idx, bool enabled);
    virtual bool isDynamicLightEnabled(s32 idx);

	virtual const core::array < SLight > getDynamicLights ()
	{ return m_Lights; }

	virtual void applyDynamicLights();

	virtual void setShadowColor(const img::SColor &color)
	{ m_ShadowColor = color; }

	virtual const img::SColor& getShadowColor() const
	{ return m_ShadowColor; }

	virtual void draw(IRenderBuffer * rbuf)
	{ _renderBuffer(rbuf); }

	virtual f64 getFrameCurrentRenderTimeSec();
	virtual f64 getFrameFilteredRenderTimeSec();

	virtual const c8* findGPUProgramFileName(vid::IGPUProgram *gpu_prog);
	virtual const c8* findGPUProgramFullFileName(vid::IGPUProgram *gpu_prog);

	virtual IGPUProgram* addGPUProgram(
		vid::E_VERTEX_TYPE vertex_type, const vid::SRenderPass &pass,
		u32 uniforms, u32 lightcnt,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader,
		const c8 *tag);

	virtual IGPUProgram* addGPUProgram(
		u32 uniforms, u32 lightcnt,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader,
		const c8 *tag);

	virtual IGPUProgram* getGPUProgram(
		vid::E_VERTEX_TYPE vertex_type, const SRenderPass &pass, u32 lightcnt);

	virtual IGPUProgram* getGPUProgram(
		vid::E_VERTEX_TYPE vertex_type, u64 hash, u32 lightcnt);

	virtual IGPUProgram* getGPUProgram(
		const c8 *file_name, const c8 *tag, bool reload_if_exists);

	virtual E_UNIFORM_TYPE getUniformType(const c8 *name);

	virtual void loadGPUProgramsFromDir(const c8 *dir, const c8 *tag, bool reload_if_exists);

	virtual bool compileGPUSources(u32 uniforms, u32 lights_count,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader);

	virtual bool isUseShaders() { return m_UseShaders; }

	virtual bool _bindGPUProgram(IGPUProgram* gpu_prog);
	virtual IGPUProgram* _getGPUProgramBound();

	virtual void _setVertexType(E_VERTEX_TYPE newType)
	{ m_CurrentVertexType = newType; }

	virtual void _setBasicRenderStates() {}
	virtual void _setFFPRenderStates() {}
	virtual void _setShaderRenderStates() {}

	virtual void _setTexture(s32 stage, ITexture * texture);

	ITexture * _getCurrentTexture(s32 stage);

	virtual bool _initDriver(SExposedVideoData &out_video_data);

protected:

	//! enabling stencil
	virtual void _enableStencil()
	{ m_StencilEnabled = true; }

	virtual bool _isStencilEnabled()
	{ return m_StencilEnabled; }

	//! setting up stencil buffer
	virtual void _setupStencil(
		E_STENCIL_OPERATION fail,  // to do if stencil test fails
		E_STENCIL_OPERATION zfail, // to do if stencil test passes and Z test fails
		E_STENCIL_OPERATION zpass, // to do if both stencil and Z tests pass
		E_COMPARE_TEST  func = ECT_ALWAYS,  // Stencil Test passes if ((ref & mask) func (stencil & mask)) is true
		s32 ref=0x1,        // Reference value used in stencil test
		u32 mask=0xFFFFFFFF // Mask value used in stencil test
		) {}

	//! disabling stencil
	virtual void _disableStencil ()
	{ m_StencilEnabled = false; }

	//! enabling scissor
	virtual void _enableScissor()
	{ m_ScissorEnabled = true; }

	virtual bool _isScissorEnabled()
	{ return m_ScissorEnabled; }

	//! setting up Scissor test
	virtual void _setupScissor(s32 left, s32 top, u32 width, u32 height) {}

	//! disabling scissor
	virtual void _disableScissor ()
	{ m_ScissorEnabled = false; }

	void _renderLightedRenderPools(core::array <SRenderPool*> & rpools);

	virtual void _renderBuffer(IRenderBuffer * rbuf);

	virtual void _renderStencilVolume(IRenderBuffer * rbuf, bool zfail);

	void _sort();
	void _render();
	virtual bool _beginRendering();
	virtual bool _endRendering();
	virtual bool _swapBuffers() { return true; }

	void _registerImageForRendering(
		E_RENDER_PASS pass, f32 z,
		ITexture * texture,
		const core::rectf & drawRect, const core::rectf & texRect, 
		const img::SColor & color0,	const img::SColor & color1,
		const img::SColor & color2,	const img::SColor & color3,
		bool useAlphaBlending, bool useColorBlending,
		E_RENDER_MODE mode);

	void _registerImageForRendering(
		E_RENDER_PASS pass, f32 z,
		const SMaterial & material,
		const core::rectf & drawRect, const core::rectf & texRect,
		const img::SColor & color0,	const img::SColor & color1,
		const img::SColor & color2,	const img::SColor & color3,
		E_RENDER_MODE mode);

	void _registerGeometryForRendering(
		E_RENDER_PASS pass, const core::matrix4 &transf,
		const SLight *light0, u64 enabled_lights,
		s32 enabled_light_idx,
		const SMaterial &material,
		void *vertices, u32 vert_count, E_VERTEX_TYPE vtype,
		void *indices, u32 ind_count, E_INDEX_TYPE itype,
		E_DRAW_PRIMITIVE_TYPE dpt, f32 z, E_RENDER_MODE mode,
		const scn::ISceneNode *owner = NULL);

	SGeometryBufferEntry _registerGeometryBufferForRendering(
		E_RENDER_PASS pass, const core::matrix4 &transf,
		const SLight *light0, u64 enabled_lights,
		s32 enabled_light_idx,
		const SMaterial &material,
		u32 vert_count, E_VERTEX_TYPE vtype,
		u32 ind_count, E_INDEX_TYPE itype,
		E_DRAW_PRIMITIVE_TYPE dpt, f32 z, E_RENDER_MODE mode,
		const scn::ISceneNode *owner);

	//! Setting up dynamic lights, and appply them to all 3d scene
	void _applyDynamicLights(
		const SLight * light0, u64 enabled_lights, s32 lidx);

	//! sets the needed renderstates
    virtual void _setRenderStates();

	//! Clip a 2D Line to 2D Rectangle : 0: outside, 1:inside, 2:clipped patch by AticAtac
    int _clip2DLineRect(
		const core::rect<s32>* clip, core::position2d<s32>& p1, core::position2d<s32>& p2);

	core::array < SRenderPool > m_RenderPools[E_RENDER_PASS_COUNT];
	u32 m_RenderPoolsCacheIndex;
	core::hash_array < SRenderPool* > m_RenderPoolsCache[2][E_RENDER_PASS_COUNT];

	void _createEmbeddedTextures();

	void free();

	virtual class CNullGPUProgram* _createGPUProgram(u32 uniforms, u32 lightcnt,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader)
	{
		LOGGER.logErr("%s: invalid call", __FUNCTION__);
		return NULL;
	}

	ITexture* CurrentTexture[MY_MATERIAL_MAX_LAYERS];

	core::matrix4 Matrices[E_TRANSFORMATION_STATE_COUNT];
	core::matrix4 m_ModelViewProjMatrix, m_ModelViewMatrix, m_ViewProjMatrix;
	core::matrix4 m_ModelInvMatrix, m_ViewInvMatrix, m_ModelViewInvMatrix;

	bool Transformation3DChanged;

    //! for automated screenshot texture naming
    u32 screenshot_counter;

    vid::ITexture* _loadTextureFromFile(io::IReadFile* file, core::stringc& desired_texname);

    void _addTexture(vid::ITexture* surface, const c8* filename);

	bool _addRenderTarget(CNullRenderTarget *rt);

    //! returns a device dependent texture from a software surface (IImage)
    //! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
    virtual vid::ITexture* _createDeviceDependentTexture(img::IImage* surface);
	virtual vid::ITexture* _createDeviceDependentTexture(core::dimension2di &size, img::E_COLOR_FORMAT format);

    //! checks triangle count and print warning if wrong
    bool checkPrimitiveCount(s32 vtxCount);	

	const core::stringc& _getGPUProgramName(
		vid::E_VERTEX_TYPE vertex_type, const SRenderPass &pass, u32 lightcnt);
	const core::stringc& _getShaderName(
		vid::E_VERTEX_TYPE vertex_type, const SRenderPass &pass, u32 lightcnt,
		const c8 *tag);
	const core::stringc& _getShaderTagName();

    struct SSurface
    {
        core::stringc  Filename;
        vid::ITexture* Surface;		
        bool operator < (const SSurface& other) const
        { return Filename<other.Filename; }
    };

    struct SFont
    {
        core::stringc Filename;
        IFont* Font;
        bool operator < (const SFont& other) const
        { return (Filename < other.Filename); }
    };

	core::array < SFont >		m_Fonts;
    core::array < SSurface >	m_Textures;
    core::array < SLight >		m_Lights;
	core::array <u8>			m_EnabledLights;
	u64							m_LightsBits;

	core::rect<s32> ViewPort;
    core::dimension2d<s32> m_ScreenSize;

	core::math::event_counter FPSCounter;

    u32 TextureCreationFlags;

	SFog Fog; 

    img::SColor BackColor; 

    static const s32 fog_blur_num;
    static const s32 fog_blend_times;
    static const f32 fog_blur[][2];

	vid::E_TEXTURE_FILTER m_TextureFilter;

	img::SColor GroundFogColor;

	img::SColorf GlobalAmbientColor;

	SLight GlobalLight;

	CVideoModeList VideoModeList;

	bool StencilPreInitialized;

	s32 AverageFPS;

	s32 m_LastEnabledLightsCount; 

	SRenderPass m_CurrentRenderPass, m_LastRenderPass;

	u32 m_MaxTextureUnits, m_MaxLights, MaxAnisotropyLevel;

	core::dimension2di m_MaxTextureSize;	

	CNullHardwareOcclusionQuery m_HardwareOcclusionQueryStub;

	f32 m_NormalMapCreationAmplitude;

	E_POLYGON_FILL_MODE m_PolygonFillMode;

	bool m_Rendering;

	ITexture *m_LightSphereTexture, *m_LightGradientTexture;
	ITexture *m_LightSphereWhiteTexture, *m_LightGradientWhiteTexture;
	ITexture *m_EnvTexture, *m_WhiteTexture, *m_DefaultTexture;

	//! render time values

	u32 m_RenderCount;

	u32 m_RenderedDIPsCount		[E_RENDER_PASS_COUNT], m_RenderedLightedDIPsCount[E_RENDER_MODE_COUNT];
	u32 m_RenderedTrianglesCount[E_RENDER_PASS_COUNT], m_RenderedLightedTrianglesCount[E_RENDER_MODE_COUNT];

	u32 m_MinimalMeshBufferSizeForVBORendering;

	bool m_ResetRenderStates, m_StencilEnabled, m_ScissorEnabled;

	img::SColor m_ShadowColor;

	bool m_Fullscreen;
	bool m_Antialiasing;
	bool m_VerticalSync;
	bool m_Shadows;

	bool m_StencilBuffer;
	bool m_TwoSidedStencil;
	bool m_UseShaders, m_CacheShaders, m_UseFFP;
	bool m_LightingMediumQuality, m_LightingHighQuality, m_ShadersHighQuality;

	E_DRIVER_TYPE m_DriverType;

	dev::IProfiler &m_Profiler;

	u32 m_ProfileIds[E_RENDER_PASS_COUNT], m_ProfileLightingIds[E_RENDER_MODE_COUNT];
	u32 m_ProfileSwapBuffers;
	u32 m_ProfileRender, m_ProfileSortDIPs;

	E_VERTEX_TYPE m_CurrentVertexType;

	IGPUProgram* _getGPUProgramFromFile(
		const c8 *file_name, const c8 *tag,
		bool reload_if_exists, bool additional_logging, bool return_any_if_no_tagged);

	core::array <IGPUProgram*> m_GPUPrograms;
	core::hash_table <u64,				IUnknown*> m_GPUProgramsHash[E_VERTEX_TYPE_COUNT][PRL_MAX_SHADER_LIGHTS + 1];
	core::hash_table <core::stringc,	IUnknown*> m_GPUProgramsHashByContent, m_GPUProgramsHashByFileName;

	core::hash_table <IGPUProgram*, core::stringc> m_GPUProgramsHashFileNames;

	u32 m_ColorMask;

	E_RENDER_PASS m_CurrentRenderPassType;

	u32 m_TrianglesDrawn, m_DIPsDrawn;
	s32 m_DirtyTexUnit;

	core::hash_table<core::stringc, vid::E_UNIFORM_TYPE> m_UniformTypes;

	IGPUProgram *m_CurrentGPUProgram;

	core::array <SRenderPool*> m_RenderData[E_RENDER_PASS_COUNT];
	core::list <CNullRenderTarget*> m_RTs;

	s32 m_GenShaderMaxLights;

	img::E_COLOR_FORMAT m_BackColorFormat;

	ITexture* m_CurrentColorRenderTarget;
	IRenderTarget* m_CurrentRenderTarget;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------
#endif



