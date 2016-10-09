//|-------------------------------------------------------------------------
//| File:        IVideoDriver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IVideoDriverHPP
#define IVideoDriverHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <ISingleton.h>
#include <io/IReadFile.h>
#include <img/SColor.h>
#include <img/IImageLoader.h>
#include <vid/ITexture.h>
#include <vid/IVideoModeList.h>

//----------------------------------------------------------------------------
namespace my {
namespace vid {
//----------------------------------------------------------------------------

struct SGeometryBufferEntry
{
	SGeometryBufferEntry()
		: Vertices(0), Indices(0), IndicesOffset(0) {}
	SGeometryBufferEntry(void *vertices, void *indices, u32 iOffset)
		: Vertices(vertices), Indices(indices), IndicesOffset(iOffset) {}
	void *Vertices, *Indices;
	u32 IndicesOffset;
};

//----------------------------------------------------------------------------

//! Interface to driver which is able to perform 2d and 3d gfx functions.
//! The IVideoDriver interface is one of the most important interfaces of
//! the Irrlicht Engine: All rendering and texture manipulating is done with
//! this interface. You are able to use the Irrlicht Engine by only invoking methods
//! of this interface if you would like to, although the my::scn::ISceneManager interface
//! provides a lot of powerful classes and methods to make the programmers life
//! easier.
class __MY_VID_LIB_API__ IVideoDriver :
	public IUnknown, public ISingleton<IVideoDriver>
{
public:

	//! Set render path used for rendering geometry an lighting
	virtual void setRenderPath(E_RENDER_PATH renderPath) = 0;

	//! Returns render path used for rendering geometry an lighting
	virtual E_RENDER_PATH getRenderPath() = 0;

	//! Returns color format of the Back Color Buffer
	virtual img::E_COLOR_FORMAT getBackColorFormat() = 0;

    //! Queries the features of the driver, returns true if a feature is available
    //! \param feature: A feature to query.
    //! \return Returns true if the feature is available, false if not.
    virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) = 0;

    //! Sets the view, world or projection transformation. 
    //! \param state: Transformation type to be set. Can be view, world or projection.
    //! \param mat: Matrix describing the transformation.
    virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat) = 0;

    //! Returns the transformation set by setTransform
    virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state) = 0;

	//! Returns a material, using to draw geometry now
    virtual const SRenderPass& getLastRenderPass() = 0;

	 //! Returns a material, using to draw geometry now
    virtual const SRenderPass& getRenderPass() = 0;

    //! Sets a material. All 3d drawing functions draw geometry now
    //! using this material.
    //! \param material: Material to be used from now on.
    virtual void setRenderPass(const SRenderPass& material) = 0;

    //! Returns a pointer to a texture. Loads the texture if it is not
    //! already loaded, and generates mipmap levels if wished.
    //! The texture can be in BMP, JPG, TGA, PCX and PSD format.
    //! For loading BMP, TGA, PCX and PSD files, the engine uses its own methods.
    //! PCX loading is based on some code by Dean P. Macri, who sent
    //! it in for free use by the engine. It currently not supports
    //! all .pcx formats. Also, compressed BMP's and TGAs don't work correctly.
    //! For loading JPG-Files the JPEG LIB 6b, written by 
    //! The Independent JPEG Group is used by the engine. Thanx for such a great
    //! library!
    //! \param name: Name/Filename of the texture to be loaded.
    //! \return Returns a pointer to the texture and NULL if the texture
    //! could not be loaded. 
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    virtual ITexture* getTexture(const c8* name) = 0;

    //! Returns a pointer to a texture. Loads the texture if it is not
    //! already loaded, and generates mipmap levels if wished.
    //! The texture can be in BMP, JPG, TGA, PCX and PSD format.
    //! For loading BMP, TGA, PCX and PSD files, the engine uses its own methods.
    //! PCX loading is based on some code by Dean P. Macri, who sent
    //! it in for free use by the engine. It currently not supports
    //! all .pcx formats. Also, compressed BMP's and TGAs don't work correctly.
    //! For loading JPG-Files the JPEG LIB 6b, written by 
    //! The Independent JPEG Group is used by the engine. Thanx for such a great
    //! library!
    //! \param file: Pointer to an already opened file.
    //! \return Returns a pointer to the texture and NULL if the texture
    //! could not be loaded. 
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    virtual ITexture* getTexture(io::IReadFile* file) = 0;

    //! Creates an empty Texture of specified size.
    //! \param size: Size of the texture.
    //! \param name: A name for the texture. Later calls of getTexture() with this name
    //! will return this texture
    //! \param format: Desired color format of the texture. Please note that
    //! the driver may choose to create the texture in another color format.
    //! \return Returns a pointer to the new created Texture. 
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    //! The format of the new texture will be chosen by the driver, and will in most 
    //! cases have the ECF_A1R5G5B5 or ECF_A8R8G8B8 format.
    virtual ITexture* addTexture(const c8 *name,
		const core::dimension2di &size, img::E_COLOR_FORMAT format = img::ECF_A8R8G8B8) = 0;
    virtual ITexture* addTexture(const c8 *name,
		u32 width, u32 height, img::E_COLOR_FORMAT format = img::ECF_A8R8G8B8) = 0;
 
    //! Creates a texture from a loaded IImage.
    //! \param name: A name for the texture. Later calls of getTexture() with this name
    //! will return this texture
    //! \param image: Image from which the texture is created from.
    //! \return Returns a pointer to the new created Texture. 
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    //! The format of the new texture will be chosen by the driver, and will in most 
    //! cases have the ECF_A1R5G5B5 or ECF_A8R8G8B8 format.
    virtual ITexture* addTexture(const c8 *name,
		img::IImage *image) = 0;

	//! Creates an Animatred Texture of and adds it to textures cache
	virtual ITextureAnim* addTextureAnim( 
		core::array<ITexture*> &frames, SAnimatedTextureParams &params) = 0;

    //! Creates an empty Render Target Texture of specified size and format.
    //! \param size: Size of the texture.
    //! \param name: A name for the texture. Later calls of getTexture() with this name
    //! will return this texture
    //! \param format: Desired color format of the texture.
	virtual ITexture* addRenderTargetTexture(const c8 *name,
		const core::dimension2di &size, img::E_COLOR_FORMAT format = img::ECF_A8R8G8B8) = 0;
	virtual ITexture* addRenderTargetTexture(const c8 *name,
		u32 width, u32 height, img::E_COLOR_FORMAT format = img::ECF_A8R8G8B8) = 0;

	//! Creates a texture.
    virtual ITexture* createTexture(img::IImage* image) = 0;

	//! Creates a texture.
    virtual ITexture* createTexture(core::dimension2di &size, img::E_COLOR_FORMAT format) = 0;

	//! Creates a render target texture.
    //! \param size: Size of the texture, in pixels. Width and height should
    //! be a power of two (for example 64, 128, 256, 512, ...) and it should not
    //! be bigger than the backbuffer, because it shares the zbuffer with the 
    //! screen buffer.
    //! \return Returns a pointer to the created texture or 0 if the texture could not
    //! be created. If you no longer need the image, you should call ITexture::drop().
    //! See IUnknown::drop() for more information. */
    virtual ITexture* createRenderTargetTexture(
		const core::dimension2di &size, img::E_COLOR_FORMAT format = img::ECF_A8R8G8B8) = 0;

    //! Removes a texture from the texture cache and deletes it, freeing lot of
    //! memory. Please note that after calling this, the pointer to the ITexture
    //! may not be longer valid, if it was not grabbed before by other parts of 
    //! the engine for storing it longer. So it would be a good idea to set all
    //! materials which are using this texture to null or another texture first.
    //! \param texture: Texture to delete from the engines cache.
    virtual bool removeTexture(ITexture* texture) = 0;

	//! Creates Render Target object (unmanaged).
	virtual IRenderTarget* createRenderTarget() = 0;
	virtual IRenderTarget* createRenderTarget(const core::dimension2di &size,
		img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat) = 0;
	virtual IRenderTarget* createRenderTarget(
		ITexture *colorTexture, ITexture *depthTexture) = 0;

	//! Add/remove a Render Target object (managed by video friver).
    virtual IRenderTarget* addRenderTarget() = 0;
    virtual IRenderTarget* addRenderTarget(u32 width, u32 height,
		img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat) = 0;
    virtual IRenderTarget* addRenderTarget(const core::dimension2di &size,
		img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat) = 0;
	virtual IRenderTarget* addRenderTarget(
		ITexture *colorTexture, ITexture *depthTexture) = 0;
	virtual bool removeRenderTarget(IRenderTarget *renderTarget) = 0;

	//! Set/Get a Render Target for the whole rendering.
    virtual bool setRenderTarget(IRenderTarget *rt) = 0;
	virtual IRenderTarget* getRenderTarget() = 0;

	//! looks if the image is already loaded
    virtual vid::ITexture* findTexture(const c8 *name) = 0;

	//! looks if the image is already loaded
	virtual const c8* findTextureName(vid::ITexture *texture) = 0;

	//! set up texture name
	virtual bool setTextureName(vid::ITexture *texture, const c8 *name) = 0;

    //! Sets a new viewport. Every rendering operation is done into this
    //! new area.
    //! \param Rectangle defining the new area of rendering operations.
    virtual void setViewPort(const core::rect<s32>& area) = 0;
	virtual void setViewPort(s32 left, s32 top, s32 right, s32 bottom) = 0;

    //! Gets the area of the current viewport. 
    //! \return Returns rectangle of the current vieport.
    virtual const core::rect<s32>& getViewPort() const = 0;

    //! Sets the current fog mode. These are global values attached to each 3d object
    virtual void setFog(const SFog &fog)=0;

    //! Returns Current Fog Mode
    virtual const SFog& getFog()=0;

	//! setting ground fog color
	virtual void setGroundFogColor(img::SColor gfogcol) = 0;

	//! returning ground fog color
	virtual img::SColor getGroundFogColor() = 0;

    //! Returns the size of the screen or render window.
    //! \return Size of screen or render window.
    virtual const core::dimension2d<s32>& getScreenSize() const = 0;

    //! Returns current frames per second value.
    //! \return Returns amount of frames per second drawn.
    virtual s32 getCurrentFPS() = 0;

    //! Returns average frames per second value.
    //! \return Returns average amount of frames per second drawn.
    virtual s32 getAverageFPS() = 0;

    //! Returns amount of triangle) which were drawn in the last frame.
    //! very useful method for statistics.
    //! \return Amount of primitives drawn in the last frame.
    virtual u32 getRenderedTrianglesCount ( E_RENDER_PASS pass ) = 0;
	virtual u32 getRenderedTrianglesCount () = 0;

	//! returns amount of DIP calls were drawn in the last frame.
	//! very useful method for statistics.
	//! \return Amount of dip calls in the last frame.
	virtual u32 getRenderedDIPsCount ( E_RENDER_PASS pass ) = 0;
	virtual u32 getRenderedDIPsCount () = 0;

	//! Returns the maximal amount of the draw buffers (simultanious render targets)
	virtual u32 getMaximalColorAttachmentsAmount() const = 0;

	//! Returns the maximal amount of texture units in single render pass
    //! \return Maximal amount of texture units
    virtual u32 getMaximalTextureUnitsAmount() const = 0;

	//! Returns the maximal resolution of texture
    //! \return Maximal texture resolution
    virtual const core::dimension2di& getMaximalTextureSize() const = 0;

	//! Sets the global scene ambient color. The default color is
    //! (0,0,0,0) which means it is dark.
    //! \param color: New color of the ambient light.
    virtual void setGlobalAmbientColor(const img::SColorf& color) = 0;
		
	//! gets the global scene ambient color.
    virtual const img::SColorf& getGlobalAmbientColor() const= 0;

	//! Sets the global light
    virtual void setGlobalLightParams(const SLight& light) = 0;
		
	//! gets the global light
    virtual const SLight& getGlobalLightParams() = 0;
	    
    //! \return Returns the name of the video driver. Example: In case of the DirectX9
    //! driver, it would return "Direct3D9".
    virtual const c8* getName() = 0;

    //! Returns the maximum amount of primitives (mostly vertices) which
    //! the device is able to render with one drawIndexedTriangleList
    //! call.
    virtual s32 getMaximalPrimitiveCount() = 0;

    //! Enables or disables a texture creation flag. This flag defines how
    //! textures should be created. By changing this value, you can influence
    //! the speed of rendering a lot. But please note that the video drivers
    //! take this value only as recommendation. It could happen that you
    //! enable the ETCM_ALWAYS_16_BIT mode, but the driver creates 32 bit
    //! textures.
    //! \param flag: Texture creation flag.
    //! \param enbabled: Specifies if the given flag should be enabled or disabled.
    virtual void setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled) = 0;

    //! Returns if a texture creation flag is enabled or disabled.
    //! You can change this value using setTextureCreationMode().
    //! \param flag: Texture creation flag.
    //! \return Returns the current texture creation mode.
    virtual bool getTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag) = 0;

    //! Only used by the internal engine. Used to notify the driver that
    //! the window was resized.
    virtual void OnResize(const core::dimension2d<s32>& size) = 0;

    //! getting video driver type
    virtual E_DRIVER_TYPE getDriverType()=0;

    //! getting video driver family
    virtual E_DRIVER_FAMILY getDriverFamily()=0;

	//! Clear Depth buffer
	virtual void clearDepth() = 0;

	//! Clear Stencil buffer
	virtual void clearStencil() = 0;

	//! Clear Color buffer
	virtual void clearColor(const img::SColor &color) = 0;
	virtual void clearColor(u8 alpha, u8 red, u8 green, u8 blue) = 0;

	//! Set/Get Color buffer write mask
	virtual void setColorMask(bool r, bool g, bool b, bool a) = 0;
	virtual void setColorMask(u32 mask) = 0;
	virtual u32 getColorMask() const = 0;

	//! Set/Get Background Color
    virtual void setBackgroundColor(const img::SColor &color) = 0;
    virtual const img::SColor& getBackgroundColor() = 0;

	//! Immediate rendering
	virtual void render2DRect(const SMaterial &material,
		const core::rectf &drawRect, const core::rectf &texRect) = 0;

	//! sets texture filtering mode
	virtual void setTextureFilter(E_TEXTURE_FILTER textureFilter) = 0;

	//! get texture filtering mode
	virtual E_TEXTURE_FILTER getTextureFilter() = 0;

	//! retrun maximal anisotropy texture filter level, supported by hardware
	virtual u32 getMaxAnisotropyLevel() = 0;

    //! Makes a screenshot and stores it into a newly created texture
    //! \param void
    //! \return ITexture* pointer to the new texture
    virtual ITexture* makeScreenShotTexture() = 0;

    //! Makes a screenshot and stores it into a newly created image
    //! \param void
    //! \return IImage* pointer to the new imae
	virtual img::IImage* makeScreenShotImage() = 0;

	//! clear unused VRAM
    virtual void clearUnusedVideoCache() = 0;

    //! Gets a list with all video modes available. If you are confused 
    //! now, because you think you have to create an Irrlicht Device with a video
    //! mode before being able to get the video mode list, let me tell you that
    //! there is no need to start up an Irrlicht Device with DT_DIRECTX9, DT_OPENGL or
    //! DT_NULL.
    //! \return Returns a pointer to a list with all video modes supported
    //! by the gfx adapter.
    virtual IVideoModeList* getVideoModeList() = 0;

	//! Calculates 2d screen coordinates from a 3d position.
	// This coordinates looks like:
	// (0,0) o---------------------|
	//       |                     | 
	//       |                     |
	//       |                     |
	//       |          o          |
	//       |  (Width/2,Height/2) |
	//       |                     |
	//       |                     |
	//       |---------------------o (Width,Height)
	virtual const core::position2di& getScreenCoordinatesFrom3DPosition(
		const core::vector3df &pos3d) const = 0;

	//! Calculates 2d screen coordinates from a 3d position.
	// This coordinates looks like:
	// (0,0) o----------o----------o (1,0)
	//       |          |          | 
	//       |          |          |
	//       |          |(0.5,0.5) |
	//       o----------o----------o
	//       |          |          |
	//       |          |          |
	//       |          |          |
	// (0,1) o----------o----------o (1,1)
	virtual const core::position2df& getScreenRelativeCoordinatesFrom3DPosition(
		const core::vector3df &pos3d) const = 0;

	//! Calculates 2d screen coordinates from a 3d position.
	// This coordinates looks like:
	// (-1,1) o----------o----------o (1,1)
	//        |          |          |
	//        |          |          |
	//        |          |(0,0)     |
	//        o----------o----------o
	//        |          |          |
	//        |          |          |
	//        |          |          |
	// (-1,1) o----------o----------o (1,-1)
	virtual const core::position2df& getScreenProjectionCoordinatesFrom3DPosition(
		const core::vector3df &pos3d) const = 0;

	//! Calculates 2d screen rectangle from a 3d volume.
	virtual const core::recti& getScreenRectFrom3DVolume(
		const core::aabbox3df &box3d) const = 0;

	//! Calculates 3d position from 2d screen position
	virtual const core::vector3df& get3DPositionFromScreenCoordinates(
		const core::position2d<s32> &scr_pos) const = 0;

	//! return hardware occlusion object
	virtual IHardwareOcclusionQuery& getHardwareOcclusionQuery() = 0;

	virtual u32 getMinimalMeshBufferSizeForVBORendering() = 0;

	virtual void setNormalMapCreationAmplitude(f32 amplitude) = 0;

	virtual f32 getNormalMapCreationAmplitude() = 0;

	//! Setting polygon fill mode
	virtual void setPolygonFillMode(E_POLYGON_FILL_MODE mode) = 0;

	//! Getting polygon fill mode
	virtual E_POLYGON_FILL_MODE getPolygonFillMode() = 0;

	//! Creating static render buffer
	virtual IRenderBuffer * createRenderBuffer(
		E_RENDER_BUFFER_TYPE rbt,
		E_VERTEX_TYPE vtype, u32 vert_size,
		E_INDEX_TYPE itype, u32 ind_size,
		E_DRAW_PRIMITIVE_TYPE dpt) = 0;

	//! Creating dynamic render buffer
	virtual IRenderBuffer * createDynamicRenderBuffer(
		const void * varray, E_VERTEX_TYPE vtype, u32 vert_size,
		const void * iarray, E_INDEX_TYPE itype, u32 ind_size,
		E_DRAW_PRIMITIVE_TYPE dpt) = 0;

	//! Returns pointer to the font with the specified file name. Loads the font if 
    //! it was not loaded before. Returns 0 if the font could not be loaded.
    //! \return
    //! returns a pointer to the font.
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    virtual IFont* getFont(const c8 * filename) = 0;

    //! Returns the default built-in font.
    virtual IFont* getBuiltInFont() = 0;

	/////////////////////////////////////////////////////////

	virtual void registerPoolForRendering(
		E_RENDER_PASS pass,	const core::matrix4 &transf,
		const SLight *light0, u64 enabled_lights,
		const core::array <const SMaterial*> &materials,
		const core::array <IRenderBuffer*> &render_buffers, f32 zorder = -1.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL) = 0;

	virtual void registerPoolForRendering(
		E_RENDER_PASS pass, const core::matrix4 &transf,
		s32 enabled_light_idx,
		const core::array <const SMaterial*> &materials,
		const core::array <IRenderBuffer*> &render_buffers, f32 zorder = -1.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL) = 0;

	virtual void registerPoolForRendering(
		E_RENDER_PASS pass, const core::matrix4 & transf,
		const SLight *light0, u64 enabled_lights,
		const SMaterial &material,
		IRenderBuffer &render_buffer, f32 zorder = -1.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL) = 0;

	virtual void registerPoolForRendering(
		E_RENDER_PASS pass, const core::matrix4 &transf,
		s32 enabled_light_idx,
		const SMaterial &material,
		IRenderBuffer &render_buffer, f32 zorder = -1.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL) = 0;

	virtual void registerGeometryForRendering(
		E_RENDER_PASS pass, const core::matrix4 & transf,
		const SLight *light0, u64 enabled_lights,
		const SMaterial &material,
		void *vertices, u32 vert_count, E_VERTEX_TYPE vtype,
		void *indices, u32 ind_count, E_INDEX_TYPE itype,
		E_DRAW_PRIMITIVE_TYPE dpt, f32 zorder = -1.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL) = 0;

	virtual void registerGeometryForRendering(
		E_RENDER_PASS pass, const core::matrix4 & transf,
		s32 enabled_light_idx,
		const SMaterial & material,
		void * vertices, u32 vert_count, E_VERTEX_TYPE vtype,
		void * indices, u32 ind_count, E_INDEX_TYPE itype,
		E_DRAW_PRIMITIVE_TYPE dpt, f32 zorder = -1.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL) = 0;

	virtual SGeometryBufferEntry registerGeometryBufferForRendering(
		E_RENDER_PASS pass, const core::matrix4 &transf,
		const SLight *light0, u64 enabled_lights,
		const SMaterial &material,
		u32 vert_count, E_VERTEX_TYPE vtype,
		u32 ind_count, E_INDEX_TYPE itype,
		E_DRAW_PRIMITIVE_TYPE dpt, f32 zorder = -1.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL) = 0;

	virtual SGeometryBufferEntry registerGeometryBufferForRendering(
		E_RENDER_PASS pass, const core::matrix4 & transf,
		s32 enabled_light_idx,
		const SMaterial &material,
		u32 vert_count, E_VERTEX_TYPE vtype,
		u32 ind_count, E_INDEX_TYPE itype,
		E_DRAW_PRIMITIVE_TYPE dpt, f32 zorder = -1.0f,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode *owner = NULL) = 0;

    virtual void register2DImageForRendering(
		const SMaterial & material,
		const core::rectf & drawRect, const core::rectf & texRect,
		const img::SColor & color = img::SColor(255,255,255,255),
		E_RENDER_MODE mode = ERM_RENDER_STANDARD) = 0;

    virtual void register2DImageForRendering(
		const SMaterial & material,
		const core::rectf & drawRect, const core::rectf & texRect,
		const img::SColor & color0,	const img::SColor & color1,
		const img::SColor & color2,	const img::SColor & color3,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD) = 0;
	
    virtual void register2DImageForRendering(
		ITexture * texture,
		const core::rectf & drawRect, const core::rectf & texRect, 
        const img::SColor & color = img::SColor(255,255,255,255), 
        bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD) = 0;

    virtual void register2DImageForRendering(
		ITexture * texture,
		const core::rectf & drawRect, const core::rectf & texRect, 
        const img::SColor & color0,	const img::SColor & color1,
		const img::SColor & color2,	const img::SColor & color3,
        bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD) = 0;

    virtual void registerGUIImageForRendering(
		ITexture * texture, f32 z,
		const core::rectf & drawRect, const core::rectf & texRect, 
        const img::SColor & color = img::SColor(255,255,255,255), 
        bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD) = 0;

    virtual void registerGUIImageForRendering(
		ITexture * texture, f32 z,
		const core::rectf & drawRect, const core::rectf & texRect, 
        const img::SColor & color0,	const img::SColor & color1,
		const img::SColor & color2,	const img::SColor & color3,
        bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD) = 0;

    virtual void registerGUIImageForRendering(
		ITexture * texture,
		const core::rectf & drawRect, const core::rectf & texRect, 
        const img::SColor & color = img::SColor(255,255,255,255), 
        bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD) = 0;

    virtual void registerGUIImageForRendering(
		ITexture * texture,
		const core::rectf & drawRect, const core::rectf & texRect, 
        const img::SColor & color0,	const img::SColor & color1,
		const img::SColor & color2,	const img::SColor & color3,
        bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD) = 0;

    virtual void registerGUIRectangleForRendering(
		const core::rectf & drawRect,
		const img::SColor & color = img::SColor(255,255,255,255),
		bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD) = 0;

    virtual void registerGUIRectangleForRendering(
		const core::rectf & drawRect,
		const img::SColor & color0,	const img::SColor & color1,
		const img::SColor & color2,	const img::SColor & color3,
		bool useAlphaBlending = false, bool useColorBlending = true,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD) = 0;

	virtual void register3DLineForRendering(
		E_RENDER_PASS pass,
		const core::matrix4 &transf,
		const core::vector3df &start, const core::vector3df &end,
		const img::SColor &color = img::SColor(255,255,255,255),
		bool depth_test = false,
		bool useAlphaBlending = false, bool useColorBlending = false,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD) = 0;

	virtual void register3DBoxForRendering(
		E_RENDER_PASS pass,
		const core::matrix4 &transf,
		const core::aabbox3df &box, const img::SColor &color = img::SColor(255,255,255,255),
		bool depth_test = false,
		bool useAlphaBlending = false, bool useColorBlending = false,
		E_RENDER_MODE mode = ERM_RENDER_STANDARD) = 0;

	virtual bool setRenderContextCurrent() = 0;
	virtual bool setNullContextCurrent() = 0;

	//! Manages rendering pipeline
	virtual bool beginRendering() = 0;
	virtual void renderAll() = 0;
	virtual void renderPass(E_RENDER_PASS pass) = 0;
	virtual void endRendering() = 0;
	virtual void swapBuffers() = 0;

	//! Returns true if renering is still in progress, otherwise false
	virtual bool isRendering() = 0;

	//! Returns the maximal amount of dynamic lights the device can handle
    //! \return Maximal amount of dynamic lights.
    virtual const u32& getDynamicLightsMaximalAmount()  const = 0;

    //! Returns current amount of dynamic lights set
    //! \return Current amount of dynamic lights set
    virtual s32 getDynamicLightsCount() = 0;
	
	//! Deletes all dynamic lights which were previously added with addDynamicLight().
    virtual void deleteAllDynamicLights() = 0;

    //! Adds a dynamic light.
    //! \param light: Data specifing the dynamic light.
    virtual void addDynamicLight(const SLight& light) = 0;

    //! Returns light data which was previously set with IVideDriver::addDynamicLight().
    //! \param idx: Zero based index of the light. Must be greater than 0 and smaller
    //! than IVideoDriver()::getDynamicLightsCount.
    //! \return Light data.
    virtual const SLight& getDynamicLight(s32 idx) = 0;

	//!  setting dynamic light params
	virtual void setDynamicLight(s32 idx, const SLight & light) = 0;

	//! enabling/disabling dynamic light 	
    virtual void setDynamicLightEnabled(s32 idx, bool enabled) = 0;

	//! return enabling/disabling flag of dynamic light 
    virtual bool isDynamicLightEnabled(s32 idx) = 0;

	//! returns const array of dynamic lights, to get fast access to them all
	virtual const core::array < SLight > getDynamicLights () = 0;

	//! apply dynamic light, to activate their influence on lighting calculations
	virtual void applyDynamicLights() = 0;

	//! Sets the color of stencil buffers shadows drawed by the scene manager.
    virtual void setShadowColor(const img::SColor &color) = 0;

    //! Returns the current color of shadows.
    virtual const img::SColor& getShadowColor() const = 0;

	//! Immediately draw render buffer
	virtual void renderBuffer(IRenderBuffer *rbuf, const SRenderPass &pass) = 0;
	virtual void renderBuffer(IRenderBuffer *rbuf, const SMaterial &mat) = 0;

	virtual f64 getFrameCurrentRenderTimeSec() = 0;
	virtual f64 getFrameFilteredRenderTimeSec() = 0;

	virtual const c8* findGPUProgramFileName(vid::IGPUProgram *gpu_prog) = 0;
	virtual const c8* findGPUProgramFullFileName(vid::IGPUProgram *gpu_prog) = 0;

	virtual IGPUProgram* addGPUProgram(
		vid::E_VERTEX_TYPE vertex_type, const vid::SRenderPass &pass,
		u32 uniforms, u32 lightcnt,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader,
		const c8 *tag = NULL) = 0;

	virtual IGPUProgram* addGPUProgram(
		u32 uniforms, u32 lightcnt,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader,
		const c8 *tag = NULL) = 0;

	virtual IGPUProgram* getGPUProgram(
		vid::E_VERTEX_TYPE vertex_type, const SRenderPass &pass, u32 lightcnt) = 0;

	virtual IGPUProgram* getGPUProgram(
		vid::E_VERTEX_TYPE vertex_type, u64 hash, u32 lightcnt) = 0;

	virtual IGPUProgram* getGPUProgram(
		const c8 *file_name, const c8 *tag = NULL, bool reload_if_exists = false) = 0;

	virtual E_UNIFORM_TYPE getUniformType(const c8 *name) = 0;

	virtual void loadGPUProgramsFromDir(
		const c8 *dir, const c8 *tag = NULL, bool reload_if_exists = true) = 0;

	virtual bool compileGPUSources(u32 uniforms, u32 lights_count,
		E_VERTEX_SHADER_VERSION vertex_shader_ver, const c8 *vertex_shader,
		E_PIXEL_SHADER_VERSION pixel_shader_ver, const c8 *pixel_shader) = 0;

	virtual bool isUseShaders() = 0;

	/////////////////////////////////////////////////////////

	//!	Return singleton object
	static IVideoDriver& getSingleton();

	//! Return pointer to singleton object
	static IVideoDriver* getSingletonPtr();
};

//----------------------------------------------------------------------------

__MY_VID_LIB_API__ IVideoDriver* createVideoDriver(
	SExposedVideoData &out_video_data, E_TEXTURE_FILTER textureFilter);

//----------------------------------------------------------------------------
} // end namespace video
} // end namespace irr
//----------------------------------------------------------------------------

#endif // #ifndef IVideoDriverHPP

