//|-------------------------------------------------------------------------
//| File:        ISceneManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ISceneManagerHPP
#define ISceneManagerHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <img/SColor.h>
#include <vid/SMaterial.h>
#include <io/IEventReceiver.h>
#include <scn/IShadow.h>
#include <scn/ICameraSceneNode.h>
#include <scn/IAnimatedMesh.h>
#include <ISingleton.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! The Scene Manager manages scene nodes, mesh recources, cameras and all the other stuff.
//! All Scene nodes can be created only here. There is a always growing list of scene 
//! nodes for lots of purposes: Outdoor rendering scene nodes like the Octree (addKTreeSceneNode(),
//! scene nodes for Light (addLightSceneNode()), Billboards (addBillboardSceneNode()) and so on.
//! A scene node is a node in the hirachical scene graph. Every scene node may have children,
//! which are other scene nodes. Children move relative the their parents position.
//! If the parent of a node is not visible, its children won't be visible too.
//! In this way, it is for example easily possible
//! to attach a light to a moving car, or to place a walking character on a moving platform
//! on a moving ship.
class __MY_SCN_LIB_API__ ISceneManager :
	public IAnimationCallback, public ISingleton <ISceneManager>
{
public:

    //! Gets pointer to an animateable mesh. Loads it if needed.
    //! Currently there are the following mesh formats supported:
    //! .bsp(Quake3 Level), .md2(Quake2 Model), .3ds(3D Studio), .cfg(Cal3d)
    //! make a feature request on the Irrlicht Engine homepage if you like.
    //! \param  filename: Filename of the mesh to load.
    //! \param  lodlevel: detail level (EMLL_MESH_LOD_FARTHEST - min, EMLL_MESH_LOD_NEAREST - max).
    //! \return Returns NULL if failed and the pointer to the mesh if 
    //! successful. 
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    virtual IAnimatedMesh* getMesh(const c8* filename, 
        E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST) = 0;

    //! Adds a test scene node for test purposes of the scene.
	//! It is a simple cube of (1,1,1) size. 
    //! \param size: Size of the cube.
    //! \param parent: Parent of the scene node. Can be NULL if no parent.
    //! \param id: Id of the node. This id can be used to identify the scene node.
    //! \param position: Position of the space relative to its parent where the
    //! scene node will be placed.
    //! \param rotation: Initital rotation of the scene node.
    //! \param scale: Initial scale of the scene node.
    //! \return Returns pointer to the created test scene node.
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    virtual ITestSceneNode* addTestSceneNode(
		core::vector3df size=core::vector3df(10.0f, 10.0f, 10.0f), 
		E_GEOM_PRIMITIVE_TYPE geom_type = EGPT_BOX, 
		ISceneNode* parent=0, s32 id=-1
        ) = 0;
	
    //! Adds a scene node for rendering an animated mesh model.
    virtual IAnimatedMeshSceneNode* addAnimatedMeshSceneNode(ISceneNode* parent=0, s32 id=-1) = 0;

    //! Adds a scene node for rendering using a k-tree to the scene graph.
	//! This a good method for rendering scenes with lots of geometry.
	//! The k-tree is built on the fly from the mesh.
    virtual IKTreeSceneNode* addKTreeSceneNode(ISceneNode* parent=0, s32 id=-1) = 0;

    //! Adds a camera scene node to the scene graph and sets it as active camera.
    //! \param position: Position of the space relative to its parent where the camera will be placed.
    //! \param lookat: Position where the camera will look at. Also known as target.
    //! \param parent: Parent scene node of the camera. Can be null. If the parent moves,
    //! the camera will move too.
    //! \param id: id of the camera. This id can be used to identify the camera.
    //! \return Returns pointer to interface to camera if successful, otherwise 0.
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    virtual ICameraSceneNode* addCameraSceneNode(ISceneNode* parent = 0,
        const core::vector3df& position = core::vector3df(0,0,0), 
        const core::vector3df& lookat = core::vector3df(0,0,100), s32 id=-1) = 0;

    //! Adds a camera scene node which is able to be controlled with the mouse similar
    //! like in the 3D Software Maya by Alias Wavefront.
    //! \param parent: Parent scene node of the camera. Can be null.
    //! \param rotateSpeed: Rotation speed of the camera.
    //! \param zoomSpeed: Zoom speed of the camera.
    //! \param tranlationSpeed: TranslationSpeed of the camera.
    //! \param id: id of the camera. This id can be used to identify the camera.
    //! \return Returns a pointer to the interface of the camera if successful, otherwise 0.
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    virtual ICameraSceneNode* addCameraSceneNodeMaya(ISceneNode *parent = NULL,
		f32 rotateSpeed = 100.0f, f32 moveSpeed = 100.0f, f32 zoomSpeed = 100.0f, 
        s32 id = -1) = 0;

    //! Adds a camera scene node which is able to be controled with the mouse and keys
    //! like in most first person shooters (FPS): 
    //! \param parent: Parent scene node of the camera. Can be null.
    //! \param rotateSpeed: Speed with wich the camera is rotated. This can be done
    //! only with the mouse.
    //! \param movespeed: Speed with which the camera is moved. Movement is done with
    //! the cursor keys.
	//! \param zoomSpeed: Speed with which the camera is zooming.
    //! \param id: id of the camera. This id can be used to identify the camera.
    //! \return Returns a pointer to the interface of the camera if successful, otherwise 0.
    //! This pointer should not be dropped. See IUnknown::drop() for more information.   
	virtual ICameraSceneNode* addCameraSceneNodeFPS(ISceneNode *player = NULL, 
        f32 rotateSpeed = 100.0f, f32 moveSpeed = 100.0f, f32 zoomSpeed = 100.0f,
		s32 id = -1) = 0;

    //! Add Overview Camera Scene Node
    virtual ICameraSceneNode* addCameraSceneNodeOverview(ISceneNode *parent = NULL, 
        f32 rotateSpeed = 100.0f, f32 zoomSpeed = 100.0f,
		s32 id = -1) = 0;

    //! Adds a dynamic light scene node to the scene graph. The light will cast dynamic light on all
    virtual ILightSceneNode* addLightSceneNode(ISceneNode* parent = 0,
		s32 id=-1, vid::SLight LightData=vid::SLight()) = 0;

    //! Adds a billboard scene node to the scene graph. A billboard is like a 3d sprite: A 2d element,
    //! which always looks to the camera. It is usually used for things like explosions, fire,
    //! lensflares and things like that.
    virtual IBillboardSceneNode* addBillboardSceneNode(
		ISceneNode* parent = 0, s32 id=-1) = 0;

    //! Adds a skybox scene node to the scene graph.
	//! A skybox is a big cube with 6 textures on it and is drawed around the camera position. 
    //! \param top: Texture for the top plane of the box.
    //! \param bottom: Texture for the bottom plane of the box.
    //! \param left: Texture for the left plane of the box.
    //! \param right: Texture for the right plane of the box.
    //! \param front: Texture for the front plane of the box.
    //! \param parent: Parent scene node of the skybox. A skybox usually has no parent,
    //! so this should be null. Note: If a parent is set to the skybox, the box will not 
    //! change how it is drawed.
    //! \param id: An id of the node. This id can be used to identify the node.
    //! \return Returns a pointer to the sky box if successful, otherwise NULL.
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    virtual ISkyBoxSceneNode* addSkyBoxSceneNode(
        vid::ITexture* top, vid::ITexture* bottom,
        vid::ITexture* left, vid::ITexture* right, vid::ITexture* front,
        vid::ITexture* back, ISceneNode* parent = 0, s32 id=-1) = 0;

    //! Adds a particle system scene node to the scene graph.
    virtual IParticleSystemSceneNode* addParticleSystemSceneNode(
		ISceneNode* parent=0, s32 id=-1) = 0;

    //! add lens flare effect
    virtual ILensFlareSceneNode* addLensFlareSceneNode(ISceneNode* parent=0, s32 id=-1) = 0;

    //! Adds a text scene node, which is able to display  2d text at a position in three dimensional space
    virtual ITextSceneNode * addTextSceneNode(vid::IFont * font,
		const c8* text, img::SColor color = img::SColor(100,255,255,255),
        ISceneNode *parent = 0, s32 id = -1) = 0;

    //! Adds a Lib Mini terrain scene node to the scene graph.
    //! \param parent: Parent scene node of the camera. Can be null.
    //! \param id: id of the camera. This id can be used to identify the camera.
	//! \param hmap_filename, file name of the height map image
	//! \param fogmap_filename, file name of the fog map image
    //! \param gridPointSpacing, spacing between the gridpoints of the heightfield 
    //! \param heightScale, scaling value for image height values 
    virtual  ITerrainSceneNode* addLibMiniTerrainSceneNode(ISceneNode* parent = 0, s32 id = -1,
		const c8* hmap_filename = NULL, const c8* fogmap_filename = NULL,
        f32 gridPointSpacing = 64.f , f32 heightScale = 0.01f) = 0;

    //! Adds a dummy transformation scene node to the scene graph.
    //! This scene node does not render itself, and does not respond to set/getPosition,
    //! set/getRotation and set/getScale. Its just a simple scene node that takes a 
    //! matrix as relative transformation, making it possible to insert any transformation
    //! anywhere into the scene graph.
    //! \return Returns pointer to the created scene node.
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    virtual IDummyTransformationSceneNode* addDummyTransformationSceneNode(
        ISceneNode* parent=0, s32 id=-1) = 0;

    //! Creates Hill Plane. The mesh is generated on the fly
    //! and looks like a plane with some hills on it. It is uses mostly for quick
    //! tests of the engine only. You can specify how many hills there should be 
    //! on the plane and how high they should be. Also you must specify a name for
    //! the mesh, because the mesh is added to the mesh pool, and can be retieved
    //! again using ISceneManager::getMesh() with the name as parameter.
    //! \param name: The name of this mesh which must be specified in order
    //! to be able to retrieve the mesh later with ISceneManager::getMesh().
    //! \param tileSize: Size of a tile of the mesh. (10.0f, 10.0f) would be a
    //! good value to start, for example.
    //! \param tileCount: Specifies how much tiles there will be. If you specifiy 
    //! for example that a tile has the size (10.0f, 10.0f) and the tileCount is
    //! (10,10), than you get a field of 100 tiles wich has the dimension 100.0fx100.0f.
    //! \param material: Material of the hill mesh.
    //! \param hillHeight: Height of the hills. If you specify a negative value
    //! you will get holes instead of hills. If the height is 0, no hills will be
    //! created.
    //! \param countHills: Amount of hills on the plane. There will be countHills.X
    //! hills along the X axis and countHills.Y along the Y axis. So in total there 
    //! will be countHills.X * countHills.Y hills.
    //! \param textureRepeatCount: Defines how often the texture will be repeated in
    //! x and y direction.
    //! \return Returns null if the creation failed. The reason could be that you 
    //! specified some invalid parameters or that a mesh with that name already
    //! exists. If successful, a pointer to the mesh is returned. 
    //! This pointer should not be dropped. See IUnknown::drop() for more information.
    virtual IAnimatedMesh* createHillPlaneMesh(const c8* name,
        const core::dimension2d<f32>& tileSize, const core::dimension2d<s32>& tileCount,
        vid::SMaterial* material = 0, f32 hillHeight = 0.0f, 
        const core::dimension2d<f32>& countHills = core::dimension2d<f32>(0.0f, 0.0f),
        const core::dimension2d<f32>& textureRepeatCount = core::dimension2d<f32>(1.0f, 1.0f)) = 0;

	//! Adds a Box mesh to the mesh pool. The mesh is generated on the fly
	virtual  IAnimatedMesh* addBoxMesh(
		core::vector3df box_size, core::stringc mesh_name="") = 0;

	//! Adds a Sphere mesh to the mesh pool. The mesh is generated on the fly
	virtual  IAnimatedMesh* addSphereMesh(
		f32 radius, u32 segments, core::stringc mesh_name="") = 0;

	//! Adds a Cylinder mesh to the mesh pool. The mesh is generated on the fly
	virtual  IAnimatedMesh* addCylinderMesh(
		f32 radius, f32 height, u32 segments, core::stringc mesh_name="") = 0;

	//! Adds a Capped Cylinder (capsula) mesh to the mesh pool. The mesh is generated on the fly
	virtual  IAnimatedMesh* addCapedCylinderMesh(
		f32 radius, f32 height, u32 segments, core::stringc mesh_name="") = 0;

	//! Adds a Limited Plane mesh to the mesh pool. The mesh is generated on the fly
	virtual  IAnimatedMesh* addLimitedPlaneMesh(
		core::dimension2d<f32> dim, core::dimension2d<s32> tiles, core::stringc mesh_name="") = 0;

    //! Returns the root scene node in the currently activates Scene Cluster.
	//! This is the scene node wich is parent of all scene nodes in current Scene Cluster.
	//! The root scene node is a special scene node which
    //! only exists to manage all scene nodes. It is not rendered and cannot
    //! be removed from the scene.
    //! \return Returns a pointer to the root scene node.
    virtual ISceneNode* getRootSceneNode() = 0;

    //! Returns the first scene node with the specified id.
    //! \param id: The id to search for
    //! \param start: Scene node to start from.
	//! If null is specified, the first scene node is taken.
    //! \return Returns pointer to the first scene node with this id,
    //! and null if no scene node could be found.
    virtual ISceneNode* getSceneNodeFromId(s32 id, ISceneNode* start=0) = 0;

	//! Return Root Scene Node of the Scene Cluster by its index
	virtual ISceneNode* getSceneClusterRootSceneNode(u32 idx) = 0;

	//! Returns index of the Scene Cluster with all cached nodes
	virtual u32 getCacheSceneClusterIndex() = 0;

	//! Returns index of the currently activated Scene Cluster
	virtual u32 getCurrentSceneClusterIndex() = 0;

	//! Activeate current Scene Cluster by its index
	virtual void setCurrentSceneClusterIndex(u32 idx) = 0;

	//! Returns count of the Scene Clusters
	virtual u32 getSceneClusterCount() = 0;

	//! Enable/disable Scene Cluster by its index
	virtual void setSceneClusterEnabled(u32 idx, bool enabled) = 0;

    //! Returns the current active camera.
    //! \return The active camera is returned. Note that this can be NULL, if there
    //! was no camera created yet.
    virtual ICameraSceneNode* getActiveCamera() = 0;

    //! Returns the current active camera position.    
	virtual const core::vector3df& getActiveCameraPosition() = 0;

    //! Sets the active camera. The previous active camera will be deactivated.
    //! \param camera: The new camera which should be active.
    virtual void setActiveCamera(ICameraSceneNode* camera) = 0;

    //! Must be called before each call renderScene()
    virtual void preRenderScene() = 0;

	//! Must be called after each call renderScene()
    virtual void postRenderScene() = 0;

	//! Creates a rotation animator, which rotates the attached scene node around itself.
    //! \param rotationPerSecond: Specifies the speed of the animation
    //! \return Returns the animator. Attach it to a scene node with ISceneNode::addAnimator()
    //! and the animator will animate it.
    //! If you no longer need the animator, you should call ISceneNodeAnimator::drop().
    //! See IUnknown::drop() for more information.
    virtual ISceneNodeAnimator* createRotationAnimator(
        const core::vector3df& rotationPerSecond) = 0;

    //! Creates a fly circle animator, which lets the attached scene node fly 
    //! around a center.
    //! \param center: Center of the circle.
    //! \param radius: Radius of the circle.
    //! \param speed: Specifies the speed of the flight.
    //! \return Returns the animator. Attach it to a scene node with ISceneNode::addAnimator()
    //! and the animator will animate it.
    //! If you no longer need the animator, you should call ISceneNodeAnimator::drop().
    //! See IUnknown::drop() for more information.
    virtual ISceneNodeAnimator* createFlyCircleAnimator(
        const core::vector3df& center, f32 radius, f32 speed = 0.001f) = 0;

    //! Creates a fly straight animator, which lets the attached scene node
    //! fly or move along a line between two points.
    //! \param startPoint: Start point of the line.
    //! \param endPoint: End point of the line.
    //! \param timeForWay: Time in milli seconds how long the node should need to
    //! move from the start point to the end point.
    //! \param loop: If set to false, the node stops when the end point is reached.
    //! If loop is true, the node begins again at the start.
    //! \return Returns the animator. Attach it to a scene node with ISceneNode::addAnimator()
    //! and the animator will animate it.
    //! If you no longer need the animator, you should call ISceneNodeAnimator::drop().
    //! See IUnknown::drop() for more information.
    virtual ISceneNodeAnimator* createFlyStraightAnimator(const core::vector3df& startPoint,
        const core::vector3df& endPoint, u32 timeForWay, bool loop=false) = 0;

    //! Creates a texture animator, which switches the textures of the target scene
    //! node based on a list of textures.
    //! \param textures: List of textures to use.
    //! \param timePerFrame: Time in milliseconds, how long any texture in the list
    //! should be visible.
    //! \param loop: If set to to false, the last texture remains set, and the animation
    //! stops. If set to true, the animation restarts with the first texture.
    //! \return Returns the animator. Attach it to a scene node with ISceneNode::addAnimator()
    //! and the animator will animate it.
    //! If you no longer need the animator, you should call ISceneNodeAnimator::drop().
    //! See IUnknown::drop() for more information.
    virtual ISceneNodeAnimator* createTextureAnimator(
        const core::array<SAnimationFrame>& textures,
        s32 timePerFrame, bool loop = true) = 0;

    //! Creates a follow spline animator. The animator modifies the position of
    //! the attached scene node to make it follow a hermite spline. 
    //! The code of the is based on a scene node
    //! Matthias Gall sent in. Thanks! I adapted the code just a little bit. Matthias 
    //! wrote:
    //! Uses a subset of hermite splines: either cardinal splines (tightness != 0.5
	//! or catmull-rom-splines (tightness == 0.5)
    //! but this is just my understanding of this stuff,
	//! I'm not a mathematician, so this might be wrong ;)
    virtual ISceneNodeAnimator* createFollowSplineAnimator(
        s32 startTime, const core::array< core::vector3df >& points,
        f32 speed = 1.0f, f32 tightness = 0.5f) = 0;

    //! Creates a follow spline animator.
    virtual ISceneNodeAnimator* createFollowSplineAnimator(
        s32 startTime, core::vector3df *points, s32 size,
        f32 speed = 1.0f, f32 tightness = 0.5f) = 0;   

    //! Adds an external mesh loader. If you want the engine to be extended with
    //! file formats it currently is not able to load (e.g. .cob), just implement
    //! the IMeshLoader interface in your loading class and add it with this method.
    //! \param externalLoader: Implementation of a new mesh loader.
    virtual void addExternalMeshLoader(IMeshLoader* externalLoader) = 0;

    //! Adds a scene node to the deletion queue. 
    virtual void addToDeletionQueue(ISceneNode* node, s32 delta_time_ms=0) = 0;

	//! Adds a scene node to the visibility queue. 
    virtual void addToVisibilityChangeQueue(
		ISceneNode* node, bool visibility, s32 delta_time_ms=0) = 0;

    //! Clears scene clusters with needed mask, and cache if wished. 
    virtual void clearScene(
		u32 clear_mask = 0xffffffff, bool clear_cache = true) = 0;

    //! setting path for loading textures
    virtual void setTexturesPath(core::stringc path) = 0;

    //! return current path for texture loading
    virtual core::stringc getTexturesPath() = 0;

	//! clear visible nodes list
    virtual void clearVisibleNodesList() = 0;

    //////////////////////////////////////////////////////////////////////

    //! clear selected nodes list
    virtual void clearSelectionList() = 0;

    //! add node to the selection list
    virtual void addSceneNodeToSelectionList(ISceneNode *node) = 0;

    //! rem node from the selection list
    virtual bool remSceneNodeFromSelectionList(ISceneNode *node) = 0;

    //! returns selected nodes count
    virtual s32 getSelectedSceneNodesCount() = 0;

    //! returns selected node by index
    virtual ISceneNode* getSelectedSceneNode(s32 idx) = 0;

     //! returns selected nodes list
    virtual const core::list<ISceneNode*>& getSelectionList() = 0;

	virtual bool isSelectionListFreezed() = 0;
    virtual void freezeSelectionList() = 0;
	virtual void unfreezeSelectionList() = 0;

	//////////////////////////////////////////////////////////////////////

	virtual void setSceneRenderFlag(E_SCENE_RENDER_FLAG flag, bool value) = 0;
	virtual bool getSceneRenderFlag(E_SCENE_RENDER_FLAG flag) = 0;

	//////////////////////////////////////////////////////////////////////

    //! choosing node from screen coordinates
    virtual ISceneNode* getSceneNodeFromScreenCoordinates(const core::position2di &scr_pos) = 0;

    //! Attach shadow for scene node and returns a pointer to it. 
    virtual IShadow * createShadow(ISceneNode * parent, E_SHADOW_TYPE type) = 0;

    //////////////////////////////////////////////////////////////////////

	virtual bool setMeshName(IAnimatedMesh * mesh, const c8* name) = 0;

    //! remove loaded mesh
    virtual bool removeMesh(IAnimatedMesh * mesh)=0;

    //! returns file name of the already loaded mesh
    virtual const c8* findMeshFileName(IAnimatedMesh* mesh) = 0;  

	//! returns file name of the already loaded mesh
    virtual const c8* findMeshFullFileName(IAnimatedMesh* amesh) = 0;

	//! returns an already loaded mesh
    virtual IAnimatedMesh* findMesh(const c8* lowerMadeFilename) = 0;  

	//! setting terrain fog rendering mode
	virtual void setTerrainFogRenderingMode(
		E_TERRAIN_FOG_RENDERING_MODE terrain_fog_rendering_mode)=0;

	//! returning terrain fog rendering mode
	virtual E_TERRAIN_FOG_RENDERING_MODE getTerrainFogRenderingMode()=0;

	//! set scene rendering mode
	virtual void setSceneRenderingMode(E_SCENE_RENDERING_MODE srmode) = 0;

	//! return scene rendering mode
	virtual E_SCENE_RENDERING_MODE getSceneRenderingMode() = 0;

    //! Removes all unused meshes from the meshes cache and deletes them, freeing lot of
    //! memory. Please note that after calling this, the pointer to all meshes
    //! may not be longer valid
    virtual void clearUnusedMeshCache() = 0;

	virtual ISceneNode* findSceneNodeEtalon(const c8* file_name) = 0;	
	virtual bool        removeSceneNodeEtalon(const c8* file_name) = 0;

	//! Creating scene node from parameters
	virtual ISceneNode* createSceneNode(
		SSceneNodeCommonParams &snparams, bool log_info = false) = 0;

	//! Cloning scene node (return new created scene node)
	//! \param node_to_clone : scene node, wich we wants to clone
	//! \param with_animators : this parameter must be true if we wants 
	//! to attach all animators of initial scene node to new one
    virtual scn::ISceneNode* cloneSceneNode(
		scn::ISceneNode *node_to_clone, bool with_animators=true, bool log_info=false) = 0;

	//! Reloading scene node from the scratch
	virtual ISceneNode* reloadSceneNode(ISceneNode *node) = 0;

	//! Loading scene node from xml-file
	virtual ISceneNode* loadSceneNode(io::IXMLReader* xml, bool cache_it=true) = 0;
	virtual ISceneNode* loadSceneNode(const c8* filename, bool cache_it=true) = 0;

	//! Saving scene node to xml-file
	virtual bool saveSceneNode(const c8* filename, ISceneNode *node) = 0;

	//! Saving scene node parameters to xml-file
	virtual void saveSceneNodeParameters(
		io::IXMLWriter* xml, SSceneNodeParameters &parameters) = 0;

	//! Saving scene node transformations to xml-file
	virtual void saveSceneNodeTransformation(
		io::IXMLWriter* xml, SSceneNodeTransformation &transformation) = 0;

	//! Loading scene node material from xml-file
	virtual SSceneNodeMaterial loadSceneNodeMaterial(
		io::IXMLReader* xml, bool load_textures = true) = 0;

	//! Loading scene node parameters from xml-file
	virtual SSceneNodeParameters loadSceneNodeParameters(
		io::IXMLReader* xml) = 0;

	//! Loading scene node transformations from xml-file
	virtual SSceneNodeTransformation loadSceneNodeTransformation(
		io::IXMLReader* xml) = 0;

	//! Return material xml-text
	virtual const c8* getMaterialXMLText(
		const vid::SMaterial &material, bool with_textures = true) = 0;

	virtual f32 getLODDistance(E_LOD_LEVEL lod) = 0;
	virtual void setLODDistance(E_LOD_LEVEL lod, f32 dist) = 0;
	virtual E_LOD_LEVEL getLODFromDistance(f32 dist, E_LOD_LEVEL oldlod) = 0;

	virtual void setOctTreeCullingEnabled(bool enabled) = 0;
	virtual bool isOctTreeCullingEnabled() = 0;

	virtual void setOccludersCullingEnabled(bool enabled) = 0;
	virtual bool isOccludersCullingEnabled() = 0;

	//! calculating adjacency data for animated mesh
	virtual bool calculateAdjacency(IAnimatedMesh *amesh) = 0;

	//! return first scene node in scene node typed list
	virtual ISceneNode* getFirstSceneNodeInTypeList(scn::E_SCENE_NODE_TYPE type) = 0;

	//! return scene node in scene node typed list
	virtual ISceneNode* getNextSceneNodeInTypeList(ISceneNode *node) = 0;

	//! return first scene node in scene node list
	virtual ISceneNode* getFirstSceneNodeInList() = 0;

	//! return scene node in scene node list
	virtual ISceneNode* getNextSceneNodeInList(ISceneNode *node) = 0;

	virtual s32 getSceneNodesCount() = 0;

	virtual s32 getVisibleSceneNodesCount() = 0;

	virtual const core::list<ISceneNode*>& getVisibleSceneNodesList()= 0;

	//! attaching camera to this scene node
	virtual void attachCamera(ISceneNode* scene_node, ICameraSceneNode* camera) = 0;

	//! detaching camera from this scene node
	virtual void detachCamera(ISceneNode* scene_node) = 0;

	//! Returns a 3d ray which would go through the 2d screen coodinates.
	virtual const core::line3df& getRayFromScreenCoordinates(const core::position2di &pos) = 0;

	//! Calculates 2d screen position from a 3d position.
	virtual const core::position2di& getScreenCoordinatesFrom3DPosition(const core::vector3df &pos3d) = 0;

	//! make dirt spot
	virtual void makeDirtSpot(
		core::vector3df abs_pos, core::vector3df size, img::SColor color, bool hipriority = false) = 0;

	virtual void onLightShadowCasted(const vid::SLight &light, scn::E_SHADOW_TYPE type,
		u32 index, const scn::ISceneNode *owner, const img::SColor &shadow_ground_color,
		const core::rectf& shadow_ground_rect, f32 shadow_ground_y, f32 zorder) = 0;
	virtual bool isLightShadowCasted(const vid::SLight &light, scn::E_SHADOW_TYPE type) = 0;
	virtual bool isLightShadowCasted(const vid::SLight &light) = 0;

	virtual bool isSceneEditorStarted() = 0;

	virtual void OnAnimationBegin(scn::ISceneNode *node, u32 animation_idx)
	{ onAnimationBegin(node, animation_idx); }

	virtual void OnAnimationEnd(scn::ISceneNode *node, u32 animation_idx)
	{ scn::onAnimationEnd(node, animation_idx); }

	//////////////////////////////////
	//!      scene callbacks
	//////////////////////////////////

	virtual void onStartSceneEditor() = 0;
	virtual void onStopSceneEditor() = 0;

	//! called each time when ISceneNode creating
	virtual void onCreateSceneNode(scn::ISceneNode* snode) = 0;

	//! called each time when ISceneNode deleting
	virtual void onDeleteSceneNode(scn::ISceneNode* snode) = 0;

	//! called each time when ISceneNode's method setOccluder run
	virtual void onSetOccluder(scn::ISceneNode* snode, bool occluder) = 0;

	virtual void onAnimationBegin(ISceneNode *snode, u32 animation_idx) = 0;
	virtual void onAnimationEnd(ISceneNode *snode, u32 animation_idx) = 0;

	//////////////////////////////////

	//!	Return singleton object
	static ISceneManager& getSingleton();

	//! Return pointer to singleton object
	static ISceneManager* getSingletonPtr();
};

//----------------------------------------------------------------------------

__MY_SCN_LIB_API__ ISceneManager* createSceneManager();

//----------------------------------------------------------------------------
} // end namespace scene
} // end namespace irr
//----------------------------------------------------------------------------

#endif // #ifndef ISceneManagerHPP

