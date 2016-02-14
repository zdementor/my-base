//|-------------------------------------------------------------------------
//| File:        CSceneManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CSceneManagerHPP
#define CSceneManagerHPP
//----------------------------------------------------------------------------

#include <my_inc.h>   
#include <scn/ISceneManager.h>
#include <scn/ISceneNode.h>
#include <io/ICursorControl.h>
#include <scn/IMeshLoader.h>
#include <dyn/IDynamicObject.h>
#include <io/IXMLWriter.h>
#include <io/IXMLReader.h>
#include <dev/IDevice.h>
#include <io/IEventReceiver.h>  
#include <scn/ISceneNodeCreator.h>
#include <scn/ISceneNodeCloner.h>
#include <scn/ISceneNodeLoader.h>
#include <scn/ISceneNodeSaver.h>
#include <vid/ITexture.h>

#define MY_MAX_SCENE_CLUSTERS 32

//----------------------------------------------------------------------------
namespace my {
namespace scn {

class CSceneCluster;

//! The Scene Manager manages scene nodes, mesh resources, 
//! cameras and all the other stuff.
class CSceneManager : public ISceneManager //, public ISceneNode
{
public:

    CSceneManager();
	virtual ~CSceneManager();

    virtual IAnimatedMesh* getMesh(
		const c8 * filename, E_LOD_LEVEL lodlevel = ELL_LOD_NEAREST);

    virtual void render();

    virtual const core::aabbox3d<f32>& getBoundingBox() const;

    virtual void updateAbsoluteTransformation();

    virtual void preRenderScene();

    virtual void postRenderScene();

	virtual ITestSceneNode* addTestSceneNode(
        core::vector3df size = core::vector3df(10.0f, 10.0f, 10.0f), 
		E_GEOM_PRIMITIVE_TYPE geom_type = EGPT_BOX, 
		ISceneNode * parent = 0, s32 id = -1);

	virtual IAnimatedMeshSceneNode* addAnimatedMeshSceneNode(ISceneNode* parent=0, s32 id=-1);    

	virtual IKTreeSceneNode* addKTreeSceneNode(ISceneNode* parent=0, s32 id=-1 );

    virtual ICameraSceneNode* addCameraSceneNode(ISceneNode* parent,
        const core::vector3df& position, 
        const core::vector3df& lookat, s32 id);

    virtual ICameraSceneNode* addCameraSceneNodeMaya(ISceneNode* parent,
		f32 rotateSpeed, f32 moveSpeed, f32 zoomSpeed,
		s32 id);

    virtual ICameraSceneNode* addCameraSceneNodeFPS(ISceneNode* player,
		f32 rotateSpeed, f32 moveSpeed, f32 zoomSpeed,
		s32 id);

    virtual ICameraSceneNode* addCameraSceneNodeOverview(ISceneNode* parent,
		f32 rotateSpeed, f32 zoomSpeed,
		s32 id);

    virtual ILightSceneNode* addLightSceneNode(ISceneNode* parent = 0, s32 id=-1,
		vid::SLight LightData = vid::SLight());

    virtual IBillboardSceneNode* addBillboardSceneNode(ISceneNode* parent = 0, s32 id=-1);

    virtual ISkyBoxSceneNode* addSkyBoxSceneNode(
        vid::ITexture* top, vid::ITexture* bottom,
        vid::ITexture* left, vid::ITexture* right, vid::ITexture* front,
        vid::ITexture* back, ISceneNode* parent = 0, s32 id=-1);

    virtual IParticleSystemSceneNode* addParticleSystemSceneNode(ISceneNode* parent=0, s32 id=-1);

    virtual ILensFlareSceneNode* addLensFlareSceneNode(ISceneNode* parent=0, s32 id=-1);

    virtual ITextSceneNode* addTextSceneNode(vid::IFont* font, const c8* text,
		img::SColor color=img::SColor(100,255,255,255), ISceneNode* parent = 0, s32 id=-1);

    virtual  ITerrainSceneNode* addLibMiniTerrainSceneNode(ISceneNode* parent, s32 id,
		const c8* hmap_filename, const c8* fogmap_filename,
        f32 gridPointSpacing, f32 heightScale);

    virtual IDummyTransformationSceneNode* addDummyTransformationSceneNode(
        ISceneNode* parent=0, s32 id=-1);

    virtual IAnimatedMesh* createHillPlaneMesh(const c8* name,
        const core::dimension2d<f32>& tileSize, const core::dimension2d<s32>& tileCount,
        vid::SMaterial* material = 0, f32 hillHeight = 0.0f, 
        const core::dimension2d<f32>& countHills = core::dimension2d<f32>(0.0f, 0.0f),
        const core::dimension2d<f32>& textureRepeatCount = core::dimension2d<f32>(1.0f, 1.0f));

	virtual  IAnimatedMesh* addBoxMesh(
		core::vector3df box_size, core::stringc mesh_name="");

	virtual  IAnimatedMesh* addSphereMesh(
		f32 radius, u32 segments, core::stringc mesh_name="");

	virtual  IAnimatedMesh* addCylinderMesh(
		f32 radius, f32 height, u32 segments, core::stringc mesh_name="");

	virtual  IAnimatedMesh* addCapedCylinderMesh(
		f32 radius, f32 height, u32 segments, core::stringc mesh_name="");

	virtual  IAnimatedMesh* addLimitedPlaneMesh(
		core::dimension2d<f32> dim, core::dimension2d<s32> tiles, core::stringc mesh_name="");

    virtual ICameraSceneNode* getActiveCamera();

	virtual const core::vector3df& getActiveCameraPosition();

    virtual void setActiveCamera(ICameraSceneNode* camera);

    virtual ISceneNodeAnimator* createRotationAnimator(const core::vector3df& rotationPerSecond);

    virtual ISceneNodeAnimator* createFlyCircleAnimator(
        const core::vector3df& normal, f32 radius, f32 speed);

    virtual ISceneNodeAnimator* createFlyStraightAnimator(
        const core::vector3df& startPoint, const core::vector3df& endPoint,
		u32 timeForWay, bool loop=false);

    virtual ISceneNodeAnimator* createTextureAnimator(
        const core::array<SAnimationFrame>& frames, s32 timePerFrame, bool loop);

    virtual ISceneNodeAnimator* createFollowSplineAnimator(
        s32 startTime, const core::array< core::vector3df >& points,
        f32 speed = 1.0f, f32 tightness = 0.5f);

    virtual ISceneNodeAnimator* createFollowSplineAnimator(
        s32 startTime, core::vector3df *points, s32 size,
        f32 speed = 1.0f, f32 tightness = 0.5f); 

    virtual void addExternalMeshLoader(IMeshLoader* externalLoader);
 
    virtual void addToDeletionQueue(ISceneNode* node, s32 delta_time_ms=0);

    virtual void addToVisibilityChangeQueue(ISceneNode* node, bool visibility, s32 telta_time_ms=0);

	virtual ISceneNode* getRootSceneNode();

    virtual ISceneNode* getSceneNodeFromId(s32 id, ISceneNode* start=0);

	virtual ISceneNode* getSceneClusterRootSceneNode(u32 idx)
	{
		return (idx <= MY_MAX_SCENE_CLUSTERS) ?
			(ISceneNode*)m_SceneClusters[idx] : NULL;
	}

	virtual u32 getCacheSceneClusterIndex()
	{ return MY_MAX_SCENE_CLUSTERS; }

	virtual u32 getCurrentSceneClusterIndex()
	{ return m_CurrentSceneClusterIdx; }

	virtual void setCurrentSceneClusterIndex(u32 idx)
	{
		if (idx <= MY_MAX_SCENE_CLUSTERS)
			m_CurrentSceneClusterIdx = idx;
	}

	virtual u32 getSceneClusterCount()
	{ return MY_MAX_SCENE_CLUSTERS; }

	virtual void setSceneClusterEnabled(u32 idx, bool enabled);

    virtual void clearScene(u32 clear_mask = 0xffffffff, bool clear_cache = true);

    virtual void setTexturesPath(core::stringc path);

	virtual core::stringc getTexturesPath();

    virtual void clearVisibleNodesList();

    //////////////////////////////////////////////////////////////////////

    virtual void clearSelectionList();
    virtual void addSceneNodeToSelectionList(ISceneNode *node);
    virtual bool remSceneNodeFromSelectionList(ISceneNode *node);

    virtual s32 getSelectedSceneNodesCount();
    virtual ISceneNode* getSelectedSceneNode(s32 idx);

    virtual const core::list <ISceneNode*>& getSelectionList()
	{ return m_SelectedNodesList; }

	virtual bool isSelectionListFreezed()
	{ return m_SelectionListFreezed; }

	virtual void freezeSelectionList()
	{ m_SelectionListFreezed = true; }

	virtual void unfreezeSelectionList()
	{ m_SelectionListFreezed = false; }

	//////////////////////////////////////////////////////////////////////

	virtual void setSceneRenderFlag(E_SCENE_RENDER_FLAG flag, bool value)
	{
		m_SceneRenderFlags = (m_SceneRenderFlags & (~flag)) | (value ? flag : 0);
	}

	virtual bool getSceneRenderFlag(E_SCENE_RENDER_FLAG flag)
	{
		return (m_SceneRenderFlags & flag) != 0;
	}

	//////////////////////////////////////////////////////////////////////

	virtual ISceneNode* getSceneNodeFromScreenCoordinates(
		const core::position2di &scr_pos);

	virtual IShadow* createShadow(
		ISceneNode *parent, E_SHADOW_TYPE type = ESHT_SHADOW_VOLUME);

	virtual bool setMeshName(IAnimatedMesh *mesh, const c8* name);

    virtual bool removeMesh(IAnimatedMesh *mesh);

    virtual const c8* findMeshFileName(IAnimatedMesh* amesh);  

    virtual const c8* findMeshFullFileName(IAnimatedMesh* amesh);  
	 
    virtual IAnimatedMesh* findMesh(const c8* lowerMadeFilename);    
	
	virtual void setTerrainFogRenderingMode(
		E_TERRAIN_FOG_RENDERING_MODE terrain_fog_rendering_mode);

	virtual E_TERRAIN_FOG_RENDERING_MODE getTerrainFogRenderingMode();

	virtual void setSceneRenderingMode(E_SCENE_RENDERING_MODE srmode);

	virtual E_SCENE_RENDERING_MODE getSceneRenderingMode();

    virtual void clearMeshCache();

    virtual void clearUnusedMeshCache();

	virtual ISceneNode* findSceneNodeEtalon(const c8* file_name);
	virtual bool        removeSceneNodeEtalon(const c8* file_name);

	virtual ISceneNode* createSceneNode(SSceneNodeCommonParams &snparams, bool log_info=false);

	virtual scn::ISceneNode* cloneSceneNode(
		scn::ISceneNode *node_to_clone, bool with_animators=true, bool log_info=false);

	virtual ISceneNode* reloadSceneNode(ISceneNode *node);

	virtual ISceneNode* loadSceneNode(io::IXMLReader * xml, bool cache_it=true);
	virtual ISceneNode* loadSceneNode(const c8* filename, bool cache_it=true);

	virtual bool saveSceneNode(const c8* filename, ISceneNode *node);

	virtual void saveSceneNodeParameters(
		io::IXMLWriter* xml, SSceneNodeParameters &parameters);
	virtual void saveSceneNodeTransformation(
		io::IXMLWriter* xml, SSceneNodeTransformation &transformation);

	virtual SSceneNodeMaterial loadSceneNodeMaterial(
		io::IXMLReader* xml, bool load_textures = true);
	virtual SSceneNodeParameters loadSceneNodeParameters(io::IXMLReader* xml);
	virtual SSceneNodeTransformation loadSceneNodeTransformation(io::IXMLReader* xml);

	virtual const c8* getMaterialXMLText(
		const vid::SMaterial &material, bool with_textures = true);

	virtual f32 getLODDistance(E_LOD_LEVEL lod);
	virtual void setLODDistance(E_LOD_LEVEL lod, f32 dist);
	virtual E_LOD_LEVEL getLODFromDistance(f32 dist, E_LOD_LEVEL oldlod);

	virtual void setOctTreeCullingEnabled(bool enabled);
	virtual bool isOctTreeCullingEnabled();

	virtual void setOccludersCullingEnabled(bool enabled);
	virtual bool isOccludersCullingEnabled();

	virtual bool calculateAdjacency(IAnimatedMesh *amesh);

	virtual ISceneNode* getFirstSceneNodeInTypeList(scn::E_SCENE_NODE_TYPE type)
	{ return *m_SceneNodesListByType[type].begin(); }

	virtual ISceneNode* getNextSceneNodeInTypeList(ISceneNode *node)
	{
		if (!node)
			return NULL;
		core::list <ISceneNode*>::iterator it = node->m_SceneNodesListByTypeIterator;
		++it;
		return (it != m_SceneNodesListByType[node->getSceneNodeType()].end()) ? (*it) : NULL;
	}

	virtual ISceneNode* getFirstSceneNodeInList()
	{ return *m_SceneNodesList.begin(); }

	virtual ISceneNode* getNextSceneNodeInList(ISceneNode *node)
	{
		if (!node)
			return NULL;
		core::list <ISceneNode*>::iterator it = node->m_SceneNodesListIterator;
		++it;
		return (it != m_SceneNodesList.end()) ? (*it) : NULL;
	}

	virtual s32 getSceneNodesCount()
	{ return m_SceneNodesList.size(); }

	virtual s32 getVisibleSceneNodesCount()
	{ return m_VisibleSceneNodesList.size(); }

	virtual const core::list<ISceneNode*>& getVisibleSceneNodesList()
	{ return m_VisibleSceneNodesList; }

	virtual void attachCamera(ISceneNode* scene_node, ICameraSceneNode* camera);

	virtual void detachCamera(ISceneNode* scene_node);

	virtual const core::line3df& getRayFromScreenCoordinates(const core::position2di &pos);

	virtual const core::position2di& getScreenCoordinatesFrom3DPosition(const core::vector3df &pos3d);

	virtual void makeDirtSpot(core::vector3df abs_pos,
		core::vector3df size, img::SColor color, bool hipriority = false);

	virtual void onLightShadowCasted(const vid::SLight &light, scn::E_SHADOW_TYPE type,
		u32 index, const scn::ISceneNode *owner, const img::SColor &shadow_ground_color,
		const core::rectf& shadow_ground_rect, f32 shadow_ground_y, f32 zorder);
	virtual bool isLightShadowCasted(const vid::SLight &light, scn::E_SHADOW_TYPE type);
	virtual bool isLightShadowCasted(const vid::SLight &light);

	virtual bool isSceneEditorStarted()
	{ return m_SceneEditorStarted; }

	//////////////////////////////////
	//!      scene callbacks
	//////////////////////////////////
	
	virtual void onStartSceneEditor();
	virtual void onStopSceneEditor();

	virtual void onCreateSceneNode(scn::ISceneNode* snode);
	virtual void onDeleteSceneNode(scn::ISceneNode* snode);
	virtual void onSetOccluder(scn::ISceneNode* snode, bool occluder);

	virtual void onAnimationBegin(ISceneNode *snode, u32 animation_idx);
	virtual void onAnimationEnd(ISceneNode *snode, u32 animation_idx);

private:

	void _clearSelectionList();
    bool _remSceneNodeFromSelectionList(ISceneNode *node);

	bool m_SceneEditorStarted;

	void createPrimitivesMeshes();

	//! Returns the nearest scene node which collides with a 3d ray and 
	//! which id matches a bitmask. 
	ISceneNode* getSceneNodeFromRayBB(core::line3d<f32> ray);

	f32 LODDistances[E_LOD_LEVEL_COUNT];

	core::array<ISceneNodeCloner*> SceneNodeClonerList;
	core::array<ISceneNodeLoader*> SceneNodeLoaderList;
	core::array<ISceneNodeCreator*> SceneNodeCreatorList;

    //! adds a mesh to the list
    void addMesh(const c8* filename, IAnimatedMesh* mesh);

    //! returns if a mesh already was loaded
    bool wasMeshLoaded(const c8* filename);

	void handleDeletionQueue();   
    void clearDeletionQueue();   
    
	void handleVisibilityChangeQueue();  
	void clearVisibilityChangeQueue();      

	struct MeshListEntry
    {
        core::stringc Name;
        IAnimatedMesh *Mesh;
    };

	struct MeshArrayEntry
    {
		core::list <MeshListEntry*>::iterator ListIter;
		MeshListEntry *ListEntry;
        bool operator < (const MeshArrayEntry &other) const
        {
			return (ListEntry->Name < other.ListEntry->Name);
		}
    };

    //! loaded meshes
    core::list <MeshListEntry*> m_MeshesList;
	core::array <MeshArrayEntry> m_MeshesArray;
	core::hash_table <IAnimatedMesh*, core::list <MeshListEntry*>::iterator> m_MeshesHash;

	dev::IDevice		&m_Device;
    vid::IVideoDriver	&m_Driver;
    io::IFileSystem		&m_FileSystem;
	dev::IProfiler		&m_Profiler;
	img::IImageLibrary	&m_ImageLib;
	mm::ISoundDriver	&m_SoundDriver;
	scr::IScriptManager	&m_ScriptMgr;

	IPathFinderManager	*m_PathFinderManager;
	ILightGridManager	*m_Q3LightVolManager;
	IMaterialsManager	*m_MaterialsManager;
	ICullingSystem		*m_CullingSystem;

    core::array <IMeshLoader*> MeshLoaderList;    

    // current active camera
    ICameraSceneNode* m_ActiveCamera;

	// Current Position of camera
    core::vector3df CameraCurrentPos; 

	//! current paths for loading resources
	core::stringc TexturesPath;

	E_TERRAIN_FOG_RENDERING_MODE m_TerrainFogRenderingMode;

	E_SCENE_RENDERING_MODE m_SceneRenderingMode;

	IAnimatedMesh *m_SphereMesh, *m_CylinderMesh, *m_CapedCylinderMesh;

	IAnimatedMesh *m_BoxMesh, *m_BoxMesh_50_100_50, *m_BoxMesh_1000_50_1000;

	bool m_DeletingSceneManager;
	
	bool m_OctTreeCullingEnabled, m_OccludersCullingEnabled;

	struct SEtalonEntry
	{
		ISceneNode* Node;
		core::stringc FileName;

		SEtalonEntry()
			: Node(NULL), FileName("") {}
		SEtalonEntry(ISceneNode* node, const c8* filename) 
			: Node(node), FileName(filename) {}

		bool operator < (const SEtalonEntry& other) const
        {
			return (FileName < other.FileName);
        }
	};

	core::array<SEtalonEntry> m_LoadedEtalones;

	core::list<ISceneNode*> m_SceneNodesList, m_OccludersList;
	core::list<ISceneNode*> m_SceneNodesListByType[E_SCENE_NODE_TYPE_COUNT];

	core::list<ISceneNode*> m_VisibleSceneNodesList;
	core::array< core::list<ISceneNode*> > m_VisibleSceneNodesListByType;
	
	//! Nodes selected by user for editing purposes (used by SceneEditor)
    core::list<ISceneNode*> m_SelectedNodesList;   	

	core::list <SActionQueueEntry> m_DeletionList, m_VisibilityChangeToFalseList,
		m_VisibilityChangeToTrueList;

	s32 m_CurrentTimeMs;

	u32 m_ProfilePreRenderId, m_ProfilePreRenderStuffId,
		m_ProfilePreRenderCheckCullingId, m_ProfilePreRenderSceneNodesId,
		m_ProfilePreRenderNodesVisibilityListId, m_ProfilePreRenderNodesOnPreRenderId;

	u32 m_ProfilePostRenderId;

	u32 m_ProfilePreRenderSceneNodeId[E_SCENE_NODE_TYPE_COUNT];

	core::hash_table <void*, u32> m_LightShadowCasted[E_SHADOW_TYPE_COUNT];

	struct SShadowGroundEntry
	{
		SShadowGroundEntry()
			: Y(FLT_MIN) {}
		SShadowGroundEntry(f32 zorder, const scn::ISceneNode *owner)
			: ZOrder(zorder), Owner(owner) {}
		SShadowGroundEntry(
			const core::rectf &rect, f32 y, f32 zorder,
			const scn::ISceneNode *owner, const img::SColor &color)
			: Rect(rect), Y(y), ZOrder(zorder), Owner(owner), Color(color) {}
		bool operator < (const SShadowGroundEntry& other) const
		{
			if (Owner != other.Owner)
				return Owner < other.Owner;
			return ZOrder < other.ZOrder;
		}
		core::rectf				Rect;
		f32						Y;
		f32						ZOrder;
		const scn::ISceneNode	*Owner;
		img::SColor				Color;
	};

	struct SShadowGround
	{
		SShadowGround()
		{
			Array.set_used(8);
			Array.set_used(0);
		}
		core::array <SShadowGroundEntry> Array;
	};

	core::hash_table <u32, SShadowGround*> m_ShadowGrounds;

	bool m_SelectionListFreezed;

	CSceneCluster* m_SceneClusters[MY_MAX_SCENE_CLUSTERS + 1]; // +1 for cache
	u32 m_CurrentSceneClusterIdx;

	u32 m_SceneRenderFlags;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my 
//----------------------------------------------------------------------------

#endif


