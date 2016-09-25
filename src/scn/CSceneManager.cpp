//|-------------------------------------------------------------------------
//| File:        CSceneManager.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CSceneManager.h"   

#include "CGeometryCreator.h"
#include "CDefaultMeshFormatLoader.h"
#include "nodes/CTestSceneNode.h"
#include "nodes/CAnimatedMeshSceneNode.h"
#include "nodes/CKTreeSceneNode.h"
#include "nodes/CCameraSceneNode.h"
#include "nodes/CCameraMayaSceneNode.h"
#include "nodes/CCameraFPSSceneNode.h"
#include "nodes/CCameraOverviewSceneNode.h"
#include "nodes/CLightSceneNode.h"
#include "nodes/CBillboardSceneNode.h"    
#include "nodes/CSkyBoxSceneNode.h"
#include "nodes/CParticleSystemSceneNode.h"
#include "nodes/CDummyTransformationSceneNode.h"
#include "CSceneNodeAnimatorRotation.h"
#include "CSceneNodeAnimatorFlyCircle.h"
#include "CSceneNodeAnimatorFlyStraight.h"
#include "CSceneNodeAnimatorTexture.h"
#include "CSceneNodeAnimatorFollowSpline.h"
#include "nodes/CLensFlareSceneNode.h"
#include "nodes/CTerrainLibMiniSceneNode.h"
#include "CShadowVolume.h"
#include "CShadowFake.h"
#include "nodes/CTextSceneNode.h"

#include <io/ILogger.h>     
#include <io/IXMLWriter.h>
#include <io/IXMLReader.h>
#include <game/IGameManager.h>
#include <vid/IVideoDriver.h>
#include <io/IFileSystem.h> 
#include <scn/IAnimatedMesh.h> 
#include <img/IImageLibrary.h> 
#include <mm/ISoundDriver.h> 
#include <os/ICriticalSection.h>
#include <dyn/IDynamicManager.h>
#include <os/ITimer.h>
#include <os/os.h>
#include <scn/IPathFinderManager.h>
#include <res/IResourceManager.h>
#include <scn/ILightGridManager.h>
#include <scn/IMaterialsManager.h>
#include <scn/ICullingSystem.h>
#include <vid/SFog.h>
#include <scr/IScriptManager.h>
#include <dev/IProfiler.h>

#include "CAnimatorsCloner.hpp"
#include "CSceneNodeLoaders.hpp"
#include "CSceneNodeCreators.hpp"
#include "CSceneNodeCloners.hpp"

#include "CMaterialsManager.h"
#include "CMaterialFileParser.h"

//----------------------------------------------------------------------------
namespace my {  
namespace scn {
//----------------------------------------------------------------------------

IPathFinderManager* createPathFinderManager();
ILightGridManager* createQ3LightVolumeGridManager();
IMaterialsManager* createMaterialsManager();
ICullingSystem* createCullingSystem();

//----------------------------------------------------------------------------

class CSceneCluster : public CDummyTransformationSceneNode
{
public:
	CSceneCluster(s32 id)
		: CDummyTransformationSceneNode(NULL, id),
	m_Enabled(false)
	{
#if MY_DEBUG_MODE 
	    ISceneNode::setClassName("CSceneCluster CDummyTransformationSceneNode ISceneNode");
#endif
	}
	virtual ~CSceneCluster() {}

	virtual void OnBeforeCulling()
	{
		if (m_Enabled)
			CDummyTransformationSceneNode::OnBeforeCulling();
	}
	virtual void OnCheckCulling()
	{
		if (m_Enabled)
			CDummyTransformationSceneNode::OnCheckCulling();
	}
    virtual void OnPreRender(u32 timeMs)
	{
		if (m_Enabled)
			CDummyTransformationSceneNode::OnPreRender(timeMs);
	}
    virtual void OnPostRender(u32 timeMs)
	{
		if (m_Enabled)
			CDummyTransformationSceneNode::OnPostRender(timeMs);
	}
	virtual void getVisibleChildsIntoSingleList(
		core::list<ISceneNode*> &child_list,
		core::array< core::list<ISceneNode*> > &child_list_by_type) 
	{
		if (m_Enabled)
			CDummyTransformationSceneNode::getVisibleChildsIntoSingleList(
				child_list, child_list_by_type);
	}
	bool isEnabled()
	{ return m_Enabled; }
	void setEnabled(bool enabled)
	{ m_Enabled = enabled; }
private:
	bool m_Enabled;
};

//----------------------------------------------------------------------------

CSceneManager::CSceneManager() 
	: m_SceneEditorStarted(false),
m_ActiveCamera(0),
m_TerrainFogRenderingMode(ETFRM_STENCIL),
m_SceneRenderingMode(ESRM_INDOOR), 
m_Device(DEVICE), m_Driver(VIDEO_DRIVER),
m_FileSystem(FILE_SYSTEM), m_Profiler(PROFILER),
m_ImageLib(IMAGE_LIBRARY), m_SoundDriver(SOUND_DRIVER),
m_ScriptMgr(SCRIPT_MANAGER),
m_SphereMesh(NULL), m_CylinderMesh(NULL), m_CapedCylinderMesh(NULL),
m_BoxMesh(NULL), m_BoxMesh_50_100_50(NULL), m_BoxMesh_1000_50_1000(NULL),
m_DeletingSceneManager(false),
m_OctTreeCullingEnabled(true),
m_OccludersCullingEnabled(true),
m_SelectionListFreezed(false),
m_SceneRenderFlags(0)
{
#if MY_DEBUG_MODE 
    ISceneManager::setClassName("CSceneManager ISceneManager");
#endif

    m_Driver.grab();
    m_FileSystem.grab();
	m_Profiler.grab();
	m_ImageLib.grab();
	m_SoundDriver.grab();
	m_ScriptMgr.grab();

	m_PathFinderManager = createPathFinderManager();
	m_Q3LightVolManager = createQ3LightVolumeGridManager();
	m_MaterialsManager = createMaterialsManager();
	m_CullingSystem = createCullingSystem();

    // add mesh loaders
    MeshLoaderList.push_back(new CDefaultMeshFormatLoader());

	// add scene node cloners
	s32 i=0;
	SceneNodeClonerList.set_used(E_SCENE_NODE_TYPE_COUNT);
	for (i=0; i<E_SCENE_NODE_TYPE_COUNT; i++)
		SceneNodeClonerList[i] = new CSceneNodeCloner((E_SCENE_NODE_TYPE)i);

	// add scene node creators
	SceneNodeCreatorList.set_used(E_SCENE_NODE_TYPE_COUNT);
	for (i=0; i<E_SCENE_NODE_TYPE_COUNT; i++)
		SceneNodeCreatorList[i] = NULL;

	SceneNodeCreatorList[ESNT_ANIMATED_MESH_SCENE_NODE      ] = new CAnimatedMeshSceneNodeCreator;
	SceneNodeCreatorList[ESNT_BILLBOARD_SCENE_NODE          ] = new CBillboardSceneNodeCreator;
	SceneNodeCreatorList[ESNT_LIGHT_SCENE_NODE              ] = new CLightSceneNodeCreator;
	SceneNodeCreatorList[ESNT_KTREE_SCENE_NODE              ] = new CKTreeSceneNodeCreator;
	SceneNodeCreatorList[ESNT_PARTICLE_SYSTEM_SCENE_NODE    ] = new CParticleSystemSceneNodeCreator;
	SceneNodeCreatorList[ESNT_SKY_BOX_SCENE_NODE            ] = new CSkyBoxSceneNodeCreator;
	SceneNodeCreatorList[ESNT_TEST_SCENE_NODE               ] = new CTestSceneNodeCreator;
	SceneNodeCreatorList[ESNT_TERRAIN_SCENE_NODE            ] = new CTerrainLibMiniSceneNodeCreator;
	SceneNodeCreatorList[ESNT_LENS_FLARE_SCENE_NODE         ] = new CLensFlareSceneNodeCreator;
	SceneNodeCreatorList[ESNT_TEXT_SCENE_NODE               ] = new CTextSceneNodeCreator;
	SceneNodeCreatorList[ESNT_DUMMY_TANSFORMATION_SCENE_NODE] = new CDummySceneNodeCreator;

	// add scene node loaders
	SceneNodeLoaderList.set_used(E_SCENE_NODE_TYPE_COUNT);
	for (i=0; i<E_SCENE_NODE_TYPE_COUNT; i++)
		SceneNodeLoaderList[i] = NULL;

	SceneNodeLoaderList[ESNT_ANIMATED_MESH_SCENE_NODE      ] = new CAnimatedMeshSceneNodeLoader;
	SceneNodeLoaderList[ESNT_BILLBOARD_SCENE_NODE          ] = new CBillboardSceneNodeLoader;
	SceneNodeLoaderList[ESNT_LIGHT_SCENE_NODE              ] = new CLightSceneNodeLoader;
	SceneNodeLoaderList[ESNT_KTREE_SCENE_NODE              ] = new CKTreeSceneNodeLoader;
	SceneNodeLoaderList[ESNT_PARTICLE_SYSTEM_SCENE_NODE    ] = new CParticleSystemSceneNodeLoader;
	SceneNodeLoaderList[ESNT_SKY_BOX_SCENE_NODE            ] = new CSkyBoxSceneNodeLoader;
	SceneNodeLoaderList[ESNT_TEST_SCENE_NODE               ] = new CTestSceneNodeLoader;
	SceneNodeLoaderList[ESNT_TERRAIN_SCENE_NODE            ] = new CTerrainLibMiniSceneNodeLoader;
	SceneNodeLoaderList[ESNT_LENS_FLARE_SCENE_NODE         ] = new CLensFlareSceneNodeLoader;
	SceneNodeLoaderList[ESNT_TEXT_SCENE_NODE               ] = new CTextSceneNodeLoader;
	SceneNodeLoaderList[ESNT_DUMMY_TANSFORMATION_SCENE_NODE] = new CDummySceneNodeLoader;

	// reserving one light source
	m_Driver.addDynamicLight(vid::SLight());
	m_Driver.setDynamicLightEnabled(0, false);	
	
	LODDistances[ELL_LOD_NEAREST]  = 350.0f;
	LODDistances[ELL_LOD_NEAR]     = 750.0f;
	LODDistances[ELL_LOD_FAR]      = 1100.0f;
	LODDistances[ELL_LOD_FARTHEST] = 1000000.0f;

	m_VisibleSceneNodesListByType.set_used(E_SCENE_NODE_TYPE_COUNT);

	m_ProfilePreRenderId			= m_Profiler.allocProfileId("Pre render scene");
	m_ProfilePreRenderStuffId		= m_Profiler.allocProfileId("Stuff");
	m_ProfilePreRenderCheckCullingId= m_Profiler.allocProfileId("Culling");
	m_ProfilePreRenderSceneNodesId	= m_Profiler.allocProfileId("Nodes register");

	m_ProfilePreRenderNodesVisibilityListId	= m_Profiler.allocProfileId("Visibility list");
	m_ProfilePreRenderNodesOnPreRenderId	= m_Profiler.allocProfileId("On pre render");

	for (u32 i = 0; i < E_SCENE_NODE_TYPE_COUNT; i++)
	{
		m_ProfilePreRenderSceneNodeId[i] =
			m_Profiler.allocProfileId(scn::SceneNodeTypeShortStr[i]);
	}

	m_ProfilePostRenderId = m_Profiler.allocProfileId("Post render scene");

	for (u32 i = 0; i < sizeof(m_SceneClusters) / sizeof(*m_SceneClusters); i++)
	{
		m_SceneClusters[i] = new CSceneCluster(-1);
		m_SceneClusters[i]->setName(core::stringc().sprintf(
			"Scene Root %d%s", i, 
			i < MY_MAX_SCENE_CLUSTERS ? "" : " (Cache)").c_str());
	}
	setCurrentSceneClusterIndex(0);
	setSceneClusterEnabled(getCurrentSceneClusterIndex(), true);

	LOGGER.log("Scene manager created.");
}

//----------------------------------------------------------------------------

CSceneManager::~CSceneManager()
{
	m_DeletingSceneManager = true;

    clearDeletionQueue();
	clearVisibilityChangeQueue();

	clearMeshCache();

	SAFE_DROP(m_CullingSystem);
	SAFE_DROP(m_MaterialsManager);
	SAFE_DROP(m_Q3LightVolManager);
	SAFE_DROP(m_PathFinderManager);

    m_Driver.drop();
    m_FileSystem.drop();
	m_Profiler.drop();
	m_ImageLib.drop();
	m_SoundDriver.drop();
	m_ScriptMgr.drop();

    u32 i;

    for (i=0; i<MeshLoaderList.size(); ++i)
        SAFE_DROP(MeshLoaderList[i]);
	MeshLoaderList.clear();

	for (i=0; i<SceneNodeClonerList.size(); ++i)
        SAFE_DROP(SceneNodeClonerList[i]);	
	SceneNodeClonerList.clear();

	for (i=0; i<SceneNodeLoaderList.size(); ++i)
        SAFE_DROP(SceneNodeLoaderList[i]);	
	SceneNodeLoaderList.clear();

	for (i=0; i<SceneNodeCreatorList.size(); ++i)
        SAFE_DROP(SceneNodeCreatorList[i]);
	SceneNodeCreatorList.clear();

    SAFE_DROP(m_ActiveCamera);

	for (u32 i = 0; i < sizeof(m_SceneClusters) / sizeof(*m_SceneClusters); i++)
		SAFE_DROP(m_SceneClusters[i]);

	LOGGER.log("Scene manager destroyed successfully");
}

//----------------------------------------------------------------------------

void CSceneManager::createPrimitivesMeshes()
{
	if (m_DeletingSceneManager)
		return;

	if (!m_BoxMesh)
	{
		m_BoxMesh = addBoxMesh(core::vector3df(1.0f, 1.0f, 1.0f),
			core::stringc().sprintf("%s%s", RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES),
				"#box_mesh").c_str());
		m_BoxMesh->lockObject();
	}

	if (!m_BoxMesh_50_100_50)
	{
		m_BoxMesh_50_100_50 = addBoxMesh(core::vector3df(50.0f, 100.0f, 50.0f),
			core::stringc().sprintf("%s%s", RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES),
				"#box_mesh_50_100_50").c_str());
		m_BoxMesh_50_100_50->lockObject();
	}

	if (!m_BoxMesh_1000_50_1000)
	{
		m_BoxMesh_1000_50_1000 = 
			addBoxMesh(core::vector3df(1000.0f, 50.0f, 1000.0f),
			core::stringc().sprintf("%s%s", RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES),
				"#box_mesh_1000_50_1000").c_str());
		m_BoxMesh_1000_50_1000->lockObject();
	}

	if (!m_SphereMesh)
	{
		m_SphereMesh = addSphereMesh(0.5f, 8,
			core::stringc().sprintf("%s%s", RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES),
				"#sphere_mesh").c_str());
		m_SphereMesh->lockObject();
	}
	
	if (!m_CylinderMesh)
	{
		m_CylinderMesh = addCylinderMesh(0.25f, 1.0f, 8,
			core::stringc().sprintf("%s%s", RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES),
				"#cylinder_mesh").c_str());
		m_CylinderMesh->lockObject();
	}

	if (!m_CapedCylinderMesh)
	{
		m_CapedCylinderMesh = addCapedCylinderMesh(0.25f, 1.0f, 8,
			core::stringc().sprintf("%s%s", RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES),
				"#caped_cylinder_mesh").c_str());
		m_CapedCylinderMesh->lockObject();
	}
}

//----------------------------------------------------------------------------

void CSceneManager::addMesh(const c8 *filename, IAnimatedMesh *mesh)
{
    mesh->grab();

    MeshListEntry *el = new MeshListEntry;
	el->Mesh = mesh;
    el->Name = filename;
    el->Name.make_lower();
	m_MeshesList.push_back(el);

	MeshArrayEntry ea;
	ea.ListIter = m_MeshesList.get_last();
	ea.ListEntry = el;
    m_MeshesArray.push_back(ea);

	m_MeshesHash.set_value(mesh, ea.ListIter);
}

//----------------------------------------------------------------------------

bool CSceneManager::wasMeshLoaded(const c8 *filename)
{
    core::stringc name = filename;
    name.make_lower();
    return findMesh(name.c_str()) != 0;
}

//----------------------------------------------------------------------------

bool CSceneManager::setMeshName(IAnimatedMesh *mesh, const c8* name)
{
	core::list <MeshListEntry*>::iterator it = m_MeshesHash.get_value(mesh);
	if (it != 0)
	{
		MeshListEntry *mle = *it;

		MeshListEntry el;
		MeshArrayEntry ea;
		
		el.Name = mle->Name;
		ea.ListEntry = &el;

		s32 idx = m_MeshesArray.binary_search(ea);
		if (idx != -1)
		{
			mle->Name = name;
			m_MeshesArray.set_sorted(false);
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------

bool CSceneManager::removeMesh(IAnimatedMesh *mesh)
{
	core::list <MeshListEntry*>::iterator it = m_MeshesHash.get_value(mesh);
	if (it != 0)
	{
		MeshListEntry *mle = *it;

		MeshListEntry el;
		MeshArrayEntry ea;
		
		el.Name = mle->Name;
		ea.ListEntry = &el;

		s32 idx = m_MeshesArray.binary_search(ea);

		m_MeshesHash.erase_value(mesh);
		if (idx != -1)
		{
			SAFE_DROP(mesh);
			LOGGER.logInfo("Mesh %s was removed from cache.", mle->Name.c_str());
			m_MeshesArray.erase(idx, 1);
			m_MeshesList.erase(it);
			delete mle;
			return true;
		}
		else
		{
			LOGGER.logErr("Can't remove mesh, due to hash errors!");
		}
		m_MeshesList.erase(it);
		delete mle;
	}
	return false;
}

//----------------------------------------------------------------------------

IAnimatedMesh* CSceneManager::getMesh(const c8* filename, E_LOD_LEVEL lodlevel)
{
	createPrimitivesMeshes();

    IAnimatedMesh *msh = findMesh(filename);
    if (msh)
        return msh;

    io::IReadFile* file = m_FileSystem.createAndOpenFile(filename);
    if (!file)
    {
		LOGGER.logErr("Could not load mesh, because file could not be opened: %s",
			filename);
        return 0;
    }

    s32 count = MeshLoaderList.size();
    for (s32 i=0; i<count; ++i)
        if (MeshLoaderList[i]->isALoadableFileExtension(filename))
        {
            msh = MeshLoaderList[i]->createMesh(file, lodlevel);
            if (msh)
            {
                addMesh(filename, msh);
                msh->drop();
                break;
            }
        }

    file->drop();

    if (!msh)
		LOGGER.logErr("Could not load mesh, file format seems to be unsupported: %s",
			filename);
    else
        LOGGER.logInfo("Loaded mesh %s", filename);

    return msh;
}

//----------------------------------------------------------------------------

ITestSceneNode* CSceneManager::addTestSceneNode(
	core::vector3df size, E_GEOM_PRIMITIVE_TYPE geom_type, 
	ISceneNode* parent, s32 id)
{
    if (!parent)
        parent = getRootSceneNode();

    ITestSceneNode* node = new CTestSceneNode(size, geom_type, parent, id);
    node->drop();

    return node;    
}

//----------------------------------------------------------------------------   
 
IAnimatedMeshSceneNode* CSceneManager::addAnimatedMeshSceneNode(ISceneNode* parent, s32 id)
{
    if (!parent)
        parent = getRootSceneNode();

    IAnimatedMeshSceneNode* node = new CAnimatedMeshSceneNode(parent, id);
    node->drop();    

	node->setAnimationCallback(this);

    return node;    
}

//----------------------------------------------------------------------------

IKTreeSceneNode* CSceneManager::addKTreeSceneNode(ISceneNode* parent, s32 id)
{
    if (!parent)
        parent = getRootSceneNode();

    IKTreeSceneNode* node = new CKTreeSceneNode(parent, id);
	node->drop();  

    return node;
}

//----------------------------------------------------------------------------

ICameraSceneNode* CSceneManager::addCameraSceneNode(
	ISceneNode *parent,
	const core::vector3df& position, const core::vector3df& lookat,
	s32 id)
{
    if (!parent)
        parent = getRootSceneNode();

    ICameraSceneNode* node = new CCameraSceneNode(
		parent, id, NULL, position, lookat);
    node->drop();

    setActiveCamera(node);

    return node;
}

//----------------------------------------------------------------------------

ICameraSceneNode* CSceneManager::addCameraSceneNodeMaya(
	ISceneNode *parent,
	f32 rotateSpeed, f32 moveSpeed, f32 zoomSpeed,
	s32 id)
{
    if (!parent)
        parent = getRootSceneNode();
    ICameraSceneNode *node = new CCameraMayaSceneNode(
		parent, id, rotateSpeed, zoomSpeed, moveSpeed);
    node->drop();
    setActiveCamera(node);     
    return node;
}

//----------------------------------------------------------------------------

ICameraSceneNode* CSceneManager::addCameraSceneNodeFPS(
	ISceneNode *player,                 
	f32 rotateSpeed, f32 moveSpeed, f32 zoomSpeed,
	s32 id)
{   
    ICameraSceneNode *node = new CCameraFPSSceneNode(
		player, id, rotateSpeed, moveSpeed, zoomSpeed);     
    node->drop();
    setActiveCamera(node);
    return node;
}

//----------------------------------------------------------------------------

ICameraSceneNode* CSceneManager::addCameraSceneNodeOverview(
	ISceneNode *parent,
	f32 rotateSpeed, f32 zoomSpeed,
	s32 id)
{
	if (!parent)
        parent = getRootSceneNode();
    ICameraSceneNode *node = new CCameraOverviewSceneNode(
		parent, id, rotateSpeed, zoomSpeed);     
    node->drop();
    setActiveCamera(node);
    return node;
}

//----------------------------------------------------------------------------

ILightSceneNode* CSceneManager::addLightSceneNode(
	ISceneNode *parent, s32 id, vid::SLight LightData)
{
	if (!parent)
        parent = getRootSceneNode();

    ILightSceneNode* node = new CLightSceneNode(parent, id, LightData);
    node->drop();

    return node;
}

//----------------------------------------------------------------------------

IBillboardSceneNode* CSceneManager::addBillboardSceneNode(
	ISceneNode* parent, s32 id)
{
    if (!parent)
        parent = getRootSceneNode();

    IBillboardSceneNode* node = new CBillboardSceneNode(parent, id);
    node->drop();

    return node;
}

//----------------------------------------------------------------------------

ISkyBoxSceneNode* CSceneManager::addSkyBoxSceneNode(
	vid::ITexture* top, vid::ITexture* bottom,
    vid::ITexture* left, vid::ITexture* right, vid::ITexture* front,
    vid::ITexture* back, ISceneNode* parent, s32 id)
{
    if (!parent)
        parent = getRootSceneNode();

    ISkyBoxSceneNode* node = new CSkyBoxSceneNode(parent, id);

	node ->setSides(top, bottom, left, right, front, back);

    node->drop();
    return node;
}

//----------------------------------------------------------------------------

IParticleSystemSceneNode* CSceneManager::addParticleSystemSceneNode(
	ISceneNode* parent, s32 id)
{
    if (!parent)
        parent = getRootSceneNode();

    IParticleSystemSceneNode* node = new CParticleSystemSceneNode(parent, id);
    node->drop();

    return node;
}

//----------------------------------------------------------------------------

ILensFlareSceneNode* CSceneManager::addLensFlareSceneNode(
    ISceneNode* parent, s32 id)
{   
	if (!parent)
        parent = getRootSceneNode();

    ILensFlareSceneNode* node = new CLensFlareSceneNode( parent, id );
    node->drop();

    return node;   
}

//----------------------------------------------------------------------------

ITextSceneNode* CSceneManager::addTextSceneNode(
	vid::IFont* font, const c8* text, img::SColor color, 
	ISceneNode* parent, s32 id)
{
    if (!font)
		font = VIDEO_DRIVER.getBuiltInFont();

    if (!parent)
        parent = getRootSceneNode();

    ITextSceneNode* t = new CTextSceneNode( parent, id, font, text, color );
    t->drop();   
    return t;
}

//----------------------------------------------------------------------------

ITerrainSceneNode* CSceneManager::addLibMiniTerrainSceneNode(ISceneNode* parent, s32 id,
	const c8* hmap_filename, const c8* fogmap_filename,
    f32 gridPointSpacing, f32 heightScale)
{   
	if (!parent)
        parent = getRootSceneNode();

    CTerrainLibMiniSceneNode *node = new CTerrainLibMiniSceneNode(parent, id); 

	node->setMaps(
		hmap_filename, fogmap_filename, gridPointSpacing);
	node->setHeightScale(heightScale);
    node->drop();

    return node;
}

//----------------------------------------------------------------------------

IDummyTransformationSceneNode* CSceneManager::addDummyTransformationSceneNode(
    ISceneNode* parent, s32 id)
{
    if (!parent)
        parent = getRootSceneNode();
    IDummyTransformationSceneNode* node =
		new CDummyTransformationSceneNode(parent, id);
    node->drop();
    return node;
}

//----------------------------------------------------------------------------

IAnimatedMesh* CSceneManager::createHillPlaneMesh(const c8* name,
    const core::dimension2d<f32>& tileSize, const core::dimension2d<s32>& tileCount,
    vid::SMaterial* material, f32 hillHeight, const core::dimension2d<f32>& countHills,
    const core::dimension2d<f32>& textureRepeatCount)
{
    if (!name)
        return 0;
	return CGeometryCreator::createHillPlaneMesh(
		tileSize, tileCount, material, hillHeight, countHills, textureRepeatCount);
}

//----------------------------------------------------------------------------

IAnimatedMesh* CSceneManager::addBoxMesh(
	core::vector3df box_size, core::stringc mesh_name)
{
	IMeshBuffer *buffer = NULL;
	if (DEVICE.getDeviceFlagValue(dev::EDCF_USE_SHADERS))
		buffer = CGeometryCreator::createBoxGeometry<vid::S3DVertex1TCoordsTBN>(
			box_size);
	else
		buffer = CGeometryCreator::createBoxGeometry<vid::S3DVertex1TCoords>(
			box_size);
	
	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->recalculateBoundingBox();

	SAnimatedMesh* animatedMesh = new SAnimatedMesh();	
	animatedMesh->addMesh(mesh);
	animatedMesh->recalculateBoundingBox();

	mesh->drop();
	buffer->drop();

	c8 name[255];

	if (mesh_name.size()==0)
		sprintf(name, "BoxMesh%p", animatedMesh);
	else
		sprintf(name, "%s", mesh_name.c_str());

	addMesh(name, animatedMesh);

	animatedMesh->drop();

	return animatedMesh;
}

//----------------------------------------------------------------------------

 IAnimatedMesh* CSceneManager::addSphereMesh(
	 f32 radius, u32 segments, core::stringc mesh_name)
{
	IMeshBuffer *buffer = NULL;
	if (DEVICE.getDeviceFlagValue(dev::EDCF_USE_SHADERS))
		buffer = CGeometryCreator::createSphereGeometry<vid::S3DVertex1TCoordsTBN>(
			radius, segments);
	else
		buffer = CGeometryCreator::createSphereGeometry<vid::S3DVertex1TCoords>(
			radius, segments);

	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->recalculateBoundingBox();

	SAnimatedMesh* animatedMesh = new SAnimatedMesh();	
	animatedMesh->addMesh(mesh);
	animatedMesh->recalculateBoundingBox();

	mesh->drop();
	buffer->drop();

	c8 name[255];

	if (mesh_name.size()==0)
		sprintf(name, "SphereMesh%p", animatedMesh);
	else
		sprintf(name, "%s", mesh_name.c_str());

	addMesh(name, animatedMesh);

	animatedMesh->drop();

	return animatedMesh;
}

//----------------------------------------------------------------------------

IAnimatedMesh* CSceneManager::addCylinderMesh(
	f32 radius, f32 height, u32 segments, core::stringc mesh_name)
{
	IMeshBuffer *buffer = NULL;
	if (DEVICE.getDeviceFlagValue(dev::EDCF_USE_SHADERS))
		buffer = CGeometryCreator::createCapedCylinderGeometry<vid::S3DVertex1TCoordsTBN>(
			radius, height, segments);
	else
		buffer = CGeometryCreator::createCapedCylinderGeometry<vid::S3DVertex1TCoords>(
			radius, height, segments);

	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->recalculateBoundingBox();

	SAnimatedMesh* animatedMesh = new SAnimatedMesh();	
	animatedMesh->addMesh(mesh);
	animatedMesh->recalculateBoundingBox();

	mesh->drop();
	buffer->drop();

	c8 name[255];

	if (mesh_name.size()==0)
		sprintf(name, "CylinderMesh%p", animatedMesh);
	else
		sprintf(name, "%s", mesh_name.c_str());

	addMesh(name, animatedMesh);

	animatedMesh->drop();

	return animatedMesh;
}

//----------------------------------------------------------------------------

IAnimatedMesh* CSceneManager::addCapedCylinderMesh(
	f32 radius, f32 height, u32 segments, core::stringc mesh_name)
{
	IMeshBuffer *buffer = NULL;
	if (DEVICE.getDeviceFlagValue(dev::EDCF_USE_SHADERS))
		buffer = CGeometryCreator::createCapedCylinderGeometry<vid::S3DVertex1TCoordsTBN>(
			radius, height, segments);
	else
		buffer = CGeometryCreator::createCapedCylinderGeometry<vid::S3DVertex1TCoords>(
			radius, height, segments);

	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->recalculateBoundingBox();

	SAnimatedMesh* animatedMesh = new SAnimatedMesh();	
	animatedMesh->addMesh(mesh);
	animatedMesh->recalculateBoundingBox();

	mesh->drop();
	buffer->drop();

	c8 name[255];

	if (mesh_name.size()==0)
		sprintf(name, "CapedCylinderMesh%p", animatedMesh);
	else
		sprintf(name, "%s", mesh_name.c_str());

	addMesh(name, animatedMesh);

	animatedMesh->drop();

	return animatedMesh;
}

//----------------------------------------------------------------------------

IAnimatedMesh* CSceneManager::addLimitedPlaneMesh( 
	core::dimension2df dim, core::dimension2di tiles, core::stringc mesh_name)
{
	IMeshBuffer *buffer = NULL;
	if (DEVICE.getDeviceFlagValue(dev::EDCF_USE_SHADERS))
		buffer = CGeometryCreator::createLimitedPlaneGeometry<vid::S3DVertex1TCoordsTBN>(
			dim, tiles);
	else
		buffer = CGeometryCreator::createLimitedPlaneGeometry<vid::S3DVertex1TCoords>(
			dim, tiles);

	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->recalculateBoundingBox();

	SAnimatedMesh* animatedMesh = new SAnimatedMesh();	
	animatedMesh->addMesh(mesh);
	animatedMesh->recalculateBoundingBox();

	mesh->drop();
	buffer->drop();

	c8 name[255];

	if (mesh_name.size()==0)
		sprintf(name, "LimitedPlaneMesh%p", animatedMesh);
	else
		sprintf(name, "%s", mesh_name.c_str());

	addMesh(name, animatedMesh);

	animatedMesh->drop();

	return animatedMesh;
}

//----------------------------------------------------------------------------

ISceneNode* CSceneManager::getRootSceneNode()
{
	return m_SceneClusters[m_CurrentSceneClusterIdx];
}

//----------------------------------------------------------------------------

ICameraSceneNode* CSceneManager::getActiveCamera()
{
    return m_ActiveCamera;
}

//----------------------------------------------------------------------------

const core::vector3df& CSceneManager::getActiveCameraPosition()
{
	return CameraCurrentPos;
}

//----------------------------------------------------------------------------

void CSceneManager::setActiveCamera(ICameraSceneNode* camera)
{
	if (isSceneEditorStarted() && ((
			camera && !camera->isSceneEditorCamera()) || (
			!camera && m_ActiveCamera && m_ActiveCamera->isSceneEditorCamera())
			))
		return;
	if (m_ActiveCamera)
	{
		bool locked = m_ActiveCamera->isLocked();
		u32 refcnt = m_ActiveCamera->getReferenceCounter();
		if (locked)
			m_ActiveCamera->unlockObject();
		m_ActiveCamera->drop();
		if (locked && refcnt > 1)
			m_ActiveCamera->lockObject();
	}

    m_ActiveCamera = camera;

    if (m_ActiveCamera)
	{
		bool locked = m_ActiveCamera->isLocked();
		if (locked)
			m_ActiveCamera->unlockObject();
        m_ActiveCamera->grab();
		if (locked )
			m_ActiveCamera->lockObject();

		const core::view_frustrum& frust = m_ActiveCamera->getViewFrustrum();
		m_Driver.setTransform(vid::ETS_PROJ, frust.Projection);
		m_Driver.setTransform(vid::ETS_VIEW, frust.View);
	}
}

//----------------------------------------------------------------------------

void CSceneManager::render()
{
}

//----------------------------------------------------------------------------

const core::aabbox3d<f32>& CSceneManager::getBoundingBox() const
{
#if MY_DEBUG_MODE
    LOGGER.logWarn("Bounding Box of Scene Manager wanted.");
#if MY_COMPILER == MY_COMPILER_MSVC && !MY_COMP_ARCH_64
    _asm int 3;
#else
	assert(!"Bounding Box of Scene Manager wanted.");
#endif
#endif

    // should never be used.
    return *((core::aabbox3d<f32>*)0);
}

//----------------------------------------------------------------------------

void CSceneManager::updateAbsoluteTransformation() {}

//----------------------------------------------------------------------------

void CSceneManager::preRenderScene()
{
	core::list<ISceneNode*>::iterator it = 0;

	m_Profiler.startProfiling(m_ProfilePreRenderId);
	m_Profiler.startProfiling(m_ProfilePreRenderStuffId);

	m_CurrentTimeMs = TIMER.getTime();

	// delete all dynamic lights
	m_Driver.deleteAllDynamicLights(); 

	// one light reserved for lightgrid lighting (directional),
	// disabling it for now
	vid::SLight light0;
	light0.Type = vid::ELT_DIRECTIONAL;
	m_Driver.addDynamicLight(light0);

	// second light for global directional lighting
	const SLight &global_light = m_Driver.getGlobalLightParams();
	if (global_light.Enabled && global_light.Position != core::vector3df(0,0,0) )
		m_Driver.addDynamicLight(global_light);

	// calculate camera pos.
    CameraCurrentPos.set(0,0,0);
    if (m_ActiveCamera)
	{
        CameraCurrentPos = m_ActiveCamera->getAbsolutePosition();	

		core::view_frustrum frust = m_ActiveCamera->getViewFrustrum();

		m_CullingSystem->setViewer(frust, m_ActiveCamera->getTarget());

		m_Driver.setTransform(vid::ETS_PROJ, frust.Projection);
		m_Driver.setTransform(vid::ETS_VIEW, frust.View);
	}

	handleVisibilityChangeQueue();

	m_Profiler.stopProfiling(m_ProfilePreRenderStuffId);
	m_Profiler.startProfiling(m_ProfilePreRenderCheckCullingId);

	//////////////////////////////////////////////
	// lets check nodes culling

	for (u32 i = 0; i < MY_MAX_SCENE_CLUSTERS; i++)
		m_SceneClusters[i]->OnBeforeCulling();

	// occluders
	m_CullingSystem->clearOccluders();
	it = m_OccludersList.begin();
	for (; it != m_OccludersList.end(); ++it)
		(*it)->OnCheckCulling();
	m_CullingSystem->initializeOccluders();
	
	// other scene nodes
	for (u32 i = 0; i < MY_MAX_SCENE_CLUSTERS; i++)
		m_SceneClusters[i]->OnCheckCulling();

	m_Profiler.stopProfiling(m_ProfilePreRenderCheckCullingId);
	m_Profiler.startProfiling(m_ProfilePreRenderSceneNodesId);

	//////////////////////////////////////////////
	// pre render

	m_Profiler.startProfiling(m_ProfilePreRenderNodesVisibilityListId);

	for (u32 i = 0; i < MY_MAX_SCENE_CLUSTERS; i++)
		m_SceneClusters[i]->OnPreRender(m_CurrentTimeMs);

	for (u32 i = 0; i < E_SHADOW_TYPE_COUNT; i++)
		m_LightShadowCasted[i].clear();

	// getting visible childs array
	clearVisibleNodesList();
	for (u32 i = 0; i < MY_MAX_SCENE_CLUSTERS; i++)
		m_SceneClusters[i]->getVisibleChildsIntoSingleList(
			m_VisibleSceneNodesList, m_VisibleSceneNodesListByType);

	IPathFinderManager::getSingleton().zeroPathFindCalcCounter();

	m_Profiler.stopProfiling(m_ProfilePreRenderNodesVisibilityListId);
	m_Profiler.startProfiling(m_ProfilePreRenderNodesOnPreRenderId);

	// let all nodes register themselfes in properly order
	static E_SCENE_NODE_TYPE type_order[E_SCENE_NODE_TYPE_COUNT] =
	{
		ESNT_LIGHT_SCENE_NODE,
		ESNT_CAMERA_SCENE_NODE,
		ESNT_PARTICLE_SYSTEM_SCENE_NODE,
		ESNT_ANIMATED_MESH_SCENE_NODE,
		ESNT_BILLBOARD_SCENE_NODE,
		ESNT_SKY_BOX_SCENE_NODE,
		ESNT_TEST_SCENE_NODE,
		ESNT_LENS_FLARE_SCENE_NODE,
		ESNT_TEXT_SCENE_NODE,
		ESNT_DUMMY_TANSFORMATION_SCENE_NODE,
		ESNT_TERRAIN_SCENE_NODE,
		ESNT_KTREE_SCENE_NODE,
		ESNT_UNKNOWN_SCENE_NODE,
		ESNT_PATH_FINDER_SCENE_NODE
	};	
	for (s32 i = 0; i < E_SCENE_NODE_TYPE_COUNT; i++)
	{
		E_SCENE_NODE_TYPE sntype = type_order[i];

		it = m_VisibleSceneNodesListByType[sntype].begin();
		if (it != m_VisibleSceneNodesListByType[sntype].end())
		{
			m_Profiler.startProfiling(m_ProfilePreRenderSceneNodeId[sntype]);
			u32 cnt = 0;
			for (; it != m_VisibleSceneNodesListByType[sntype].end(); ++it, cnt++)
				(*it)->OnPreRender(m_CurrentTimeMs);
			m_Profiler.stopProfiling(m_ProfilePreRenderSceneNodeId[sntype],
				core::stringc().sprintf("%d nodes", cnt).c_str());
		}
	}

	m_Profiler.stopProfiling(m_ProfilePreRenderNodesOnPreRenderId);

	// registering shadow grounds geometry
	{
		f32 bord = 100.0f;
		static u16 indices[] =
		{
			0, 1, 2, 0, 2, 3,
			4, 5, 0, 0, 5, 1,
			5, 6, 1, 1, 6, 2,
			6, 7, 2, 2, 7, 3,
			7, 4, 3, 3, 4, 0
		};
		static vid::S3DVertexSimpleColoured vertices[8];
		static vid::SMaterial sg_mat;
		sg_mat.setFlag(vid::EMF_ZWRITE_ENABLE, false);
		sg_mat.getPass(0).setDepthTest(vid::ECT_ALWAYS);
		sg_mat.setFlag(vid::EMF_BLENDING,true);
		sg_mat.setFlag(vid::EMF_GOURAUD_SHADING, true);
		sg_mat.setLightingMode(vid::ELM_NONE);

		sg_mat.getPass(0).setBlendFuncs(
			vid::ESBF_SRC_ALPHA, vid::EDBF_ONE_MINUS_SRC_ALPHA);

		bool shadow_debug = getSceneRenderFlag(ESRF_RENDER_SHADOW_DEBUG_DATA);

		u32 dlc = m_Driver.getDynamicLightsCount();
		for (u32 l = 0; l < dlc; l++)
		{
			const vid::SLight &light = m_Driver.getDynamicLight(l);
			u32 key = l;
			SShadowGround *sg = m_ShadowGrounds.get_value(key);
			if (!sg)
				continue;
			u32 shcnt = sg->Array.size();
			
			for (u32 i = 0 ; i < shcnt; i++)
			{
				SShadowGroundEntry &sge = sg->Array[i];

				if (sge.Rect.isValid())
				// shadow ground plane
				{
					u32 vcnt = 0, icnt = sizeof(indices) / sizeof(*indices);
					vertices[vcnt++].Pos.set(sge.Rect.Left,	sge.Y, sge.Rect.Top);
					vertices[vcnt++].Pos.set(sge.Rect.Left,	sge.Y, sge.Rect.Bottom);
					vertices[vcnt++].Pos.set(sge.Rect.Right,sge.Y, sge.Rect.Bottom);
					vertices[vcnt++].Pos.set(sge.Rect.Right,sge.Y, sge.Rect.Top);
					vertices[0].Color.color = (m_Driver.getDriverFamily() == vid::EDF_OPENGL) ?
						sge.Color.toOpenGLColor() : sge.Color.color;
					for (u32 v = 1; v < vcnt; v++)
						vertices[v].Color.color = vertices[0].Color.color;
					vertices[vcnt++].Pos.set(sge.Rect.Left	- bord, sge.Y, sge.Rect.Top		- bord);
					vertices[vcnt++].Pos.set(sge.Rect.Left	- bord, sge.Y, sge.Rect.Bottom	+ bord);
					vertices[vcnt++].Pos.set(sge.Rect.Right	+ bord, sge.Y, sge.Rect.Bottom	+ bord);
					vertices[vcnt++].Pos.set(sge.Rect.Right	+ bord, sge.Y, sge.Rect.Top		- bord);
					for (u32 v = 4; v < vcnt; v++)
					{
						vertices[v].Color.color = vertices[0].Color.color;
						vertices[v].Color.setAlpha(0);
					}

					m_Driver.registerGeometryForRendering(
						vid::ERP_3D_LIGHTING_PASS,
						core::matrix4(), l, sg_mat,
						vertices, vcnt, vid::EVT_SIMPLE_COLOURED,
						indices, icnt, EIT_16_BIT,
						vid::EDPT_TRIANGLE_LIST, sge.ZOrder,
						vid::ERM_RENDER_WITH_STENCIL_TEST_SHADOW_GROUND_COLOR,
						sge.Owner);
					for (u32 v = 0; v < 4; v++)
						vertices[v].Color.setAlpha(255);
					m_Driver.registerGeometryForRendering(
						vid::ERP_3D_LIGHTING_PASS,
						core::matrix4(), l, sg_mat,
						vertices, vcnt, vid::EVT_SIMPLE_COLOURED,
						indices, icnt, EIT_16_BIT,
						vid::EDPT_TRIANGLE_LIST, sge.ZOrder,
						vid::ERM_RENDER_WITH_STENCIL_TEST_SHADOW_GROUND_ALPHA,
						sge.Owner);

					if (shadow_debug)
					{
						m_Driver.register3DLineForRendering(vid::ERP_3D_SOLID_PASS, core::matrix4(),
							vertices[0].Pos, vertices[1].Pos, 0xffffffff, true);
						m_Driver.register3DLineForRendering(vid::ERP_3D_SOLID_PASS, core::matrix4(),
							vertices[1].Pos, vertices[2].Pos, 0xffffffff, true);
						m_Driver.register3DLineForRendering(vid::ERP_3D_SOLID_PASS, core::matrix4(),
							vertices[2].Pos, vertices[3].Pos, 0xffffffff, true);
						m_Driver.register3DLineForRendering(vid::ERP_3D_SOLID_PASS, core::matrix4(),
							vertices[3].Pos, vertices[0].Pos, 0xffffffff, true);
					}

				}
				else
				// shadow screen rect
				{
					vertices[0].Pos.set(-1.f, -1.f, 0.f);
					vertices[1].Pos.set(-1.f,  1.f, 0.f);
					vertices[2].Pos.set( 1.f,  1.f, 0.f);
					vertices[3].Pos.set( 1.f,  1.f, 0.f);
					vertices[4].Pos.set( 1.f, -1.f, 0.f);
					vertices[5].Pos.set(-1.f, -1.f, 0.f);

					vertices[0].Color.color = (m_Driver.getDriverFamily() == vid::EDF_OPENGL) ?
						sge.Color.toOpenGLColor() : sge.Color.color;
					for (u32 v = 1; v < 6; v++)
						vertices[v].Color.color = vertices[0].Color.color;

					// to render in a screen basis [ (-1,-1), (1,1) ]
					core::matrix4 view_proj_inv = m_Driver.getTransform(vid::ETS_PROJ) * m_Driver.getTransform(vid::ETS_VIEW);
					view_proj_inv.makeInversed();

					m_Driver.registerGeometryForRendering(
						vid::ERP_3D_LIGHTING_PASS, view_proj_inv, l,
						sg_mat, vertices, 6, EVT_SIMPLE_COLOURED, NULL, 0, EIT_16_BIT,
						EDPT_TRIANGLE_LIST, sge.ZOrder,
						vid::ERM_RENDER_WITH_STENCIL_TEST_SHADOW_GROUND_COLOR,
						sge.Owner);
				}
			}
		}
	}

	bool collis_debug = getSceneRenderFlag(scn::ESRF_RENDER_COLLISION_POINTS);

	if (collis_debug)
	{
		const core::array<dyn::SCollisionPoint> *collisions[] =
		{
			&DYNAMIC_MANAGER.getCustomCollisionPoints().getPoints(),
			&DYNAMIC_MANAGER.getDynamicCollisionPoints().getPoints(),
		};
		u32 cols[sizeof(collisions)/sizeof(*collisions)] =
		{
			0xff0000ff,
			0xffff0000
		};
		f32 pSizeDiv2 = 1.5f;
		core::matrix4 matr;

		for (u32 i = 0; i < sizeof(collisions)/sizeof(*collisions); i++)
		{
			u32 color = cols[i];
			const core::array<dyn::SCollisionPoint> *collidArr = collisions[i];

			u32 colCnt = collidArr->size();
			for (u32 j = 0; j < colCnt; j++)
			{
				const dyn::SCollisionPoint &collision = collidArr->operator[](j);
				core::aabbox3df box(
					collision.Pos.X - pSizeDiv2,
					collision.Pos.Y - pSizeDiv2,
					collision.Pos.Z - pSizeDiv2,
					collision.Pos.X + pSizeDiv2,
					collision.Pos.Y + pSizeDiv2,
					collision.Pos.Z + pSizeDiv2);
				m_Driver.register3DBoxForRendering(vid::ERP_3D_TRANSP_1ST_PASS, matr,
					box, color, false);
				m_Driver.register3DLineForRendering(vid::ERP_3D_TRANSP_1ST_PASS, matr,
					collision.Pos, collision.Pos + collision.Normal * 25.f,
					0xffffffff, false);
			}
		}
	}

	m_Profiler.stopProfiling(m_ProfilePreRenderSceneNodesId);
	m_Profiler.stopProfiling(m_ProfilePreRenderId);
}

//----------------------------------------------------------------------------

void CSceneManager::postRenderScene()
{
	m_Profiler.startProfiling(m_ProfilePostRenderId);

	m_CurrentTimeMs = TIMER.getTime();

	for (u32 i = 0; i < MY_MAX_SCENE_CLUSTERS; i++)
		m_SceneClusters[i]->OnPostRender(m_CurrentTimeMs);

	core::list<ISceneNode*>::iterator it = m_VisibleSceneNodesList.begin();
    for (; it!=m_VisibleSceneNodesList.end(); ++it)
		(*it)->OnPostRender(m_CurrentTimeMs);

	handleDeletionQueue();
	m_ShadowGrounds.clear();

	m_Profiler.stopProfiling(m_ProfilePostRenderId);
}

//----------------------------------------------------------------------------

ISceneNodeAnimator* CSceneManager::createRotationAnimator(
	const core::vector3df& rotationPerSecond)
{
    ISceneNodeAnimator* anim = new CSceneNodeAnimatorRotation(
		DEVICE.getDeviceTime(),  rotationPerSecond );
    return anim;
}

//----------------------------------------------------------------------------

ISceneNodeAnimator* CSceneManager::createFlyCircleAnimator(
	const core::vector3df& normal, f32 radius, f32 speed)
{
	SFlyCircleParameters params;
	params.Radius = radius;
	params.Speed = speed;
	params.Center = normal;

    return new CSceneNodeAnimatorFlyCircle( 
		DEVICE.getDeviceTime(), params);
}

//----------------------------------------------------------------------------

ISceneNodeAnimator* CSceneManager::createFlyStraightAnimator(
	const core::vector3df& startPoint, const core::vector3df& endPoint, 
	u32 timeForWay, bool loop
	)
{
	SFlyStraightParameters params;
	params.Start = startPoint;
	params.End   = endPoint;
	params.TimeForWay = timeForWay;
	params.Loop  = loop;

    return new CSceneNodeAnimatorFlyStraight(
		DEVICE.getDeviceTime(), params);
}

//----------------------------------------------------------------------------

ISceneNodeAnimator* CSceneManager::createTextureAnimator(
	const core::array<SAnimationFrame>& frames, s32 timePerFrame, bool loop)
{
	SAnimationFrameParameters params;
	params.TimePerFrame = timePerFrame;
	params.Loop = loop;

    return new CSceneNodeAnimatorTexture( 
		DEVICE.getDeviceTime(), frames, params);
}

//----------------------------------------------------------------------------

ISceneNodeAnimator* CSceneManager::createFollowSplineAnimator(s32 when,
    const core::array< core::vector3df >& points, f32 speed, f32 tightness)
{
	SFollowSplineParameters params;

	params.Points = points;
	params.Speed  = speed;
	params.Tightness = tightness;

	return new CSceneNodeAnimatorFollowSpline(
		DEVICE.getDeviceTime() + when, params);
}

//----------------------------------------------------------------------------

ISceneNodeAnimator* CSceneManager::createFollowSplineAnimator(
	s32 when, core::vector3df *points, s32 size,
    f32 speed, f32 tightness)
{	
	if (!points || size<=0)
		return NULL;
	core::array< core::vector3df > points_array;
	for (s32 i=0; i<size; i++)
		points_array.push_back(points[i]);
	return createFollowSplineAnimator(when, points_array, speed, tightness);
}

//----------------------------------------------------------------------------

void CSceneManager::addExternalMeshLoader(IMeshLoader* externalLoader)
{
    if (!externalLoader)
        return;
    externalLoader->grab();
    MeshLoaderList.push_back(externalLoader);
}

//----------------------------------------------------------------------------

void CSceneManager::handleDeletionQueue()
{
	if (m_DeletionList.empty())
        return;
	core::list<SActionQueueEntry>::iterator it = m_DeletionList.begin();
	for (; it!=m_DeletionList.end();)
	{
		ISceneNode* sn = (*it).Node;
		if ((*it).ActionTimeMs<=m_CurrentTimeMs)
		{
			it = m_DeletionList.erase(sn->m_DeletionQueueIterator);
			sn->m_DeletionQueueIterator=0;
			sn->remove();
		}
		else
		{			
			if (sn->getSceneNodeType()==ESNT_PARTICLE_SYSTEM_SCENE_NODE)
			{
				IParticleSystemSceneNode *ps =
					SAFE_CAST_TO_PARTICLE_SYSTEM_SCENE_NODE(sn);
				s32 ecnt = ps->getEmittersCount();
				for (s32 i=0; i<ecnt; i++)
				{
					IParticleEmitter* emitter = ps->getEmitter(i);
					if ((*it).ActionTimeMs<=(m_CurrentTimeMs+(s32)emitter->getMaxLifeTime()))
						emitter->setEnabled(false);
				} 
			}			
			++it;
		}
    }
} 

//----------------------------------------------------------------------------

void CSceneManager::clearDeletionQueue()
{
    if (m_DeletionList.empty())
        return;
	core::list<SActionQueueEntry>::iterator it = m_DeletionList.begin();
	for (; it!=m_DeletionList.end();)
	{
		ISceneNode* sn = (*it).Node;
		it = m_DeletionList.erase(sn->m_DeletionQueueIterator);
		sn->m_DeletionQueueIterator=0;
		sn->remove();
    }
    m_DeletionList.clear();
} 

//----------------------------------------------------------------------------

void CSceneManager::addToDeletionQueue(ISceneNode *node, s32 delta_time_ms)
{
	if (node->m_DeletionQueueIterator==0)
	{
		SActionQueueEntry e(node, m_CurrentTimeMs+delta_time_ms);
		m_DeletionList.push_back(e);
		node->m_DeletionQueueIterator = m_DeletionList.get_last();
	}
}

//----------------------------------------------------------------------------

void CSceneManager::handleVisibilityChangeQueue()
{
	if (!m_VisibilityChangeToFalseList.empty())
	{
		core::list<SActionQueueEntry>::iterator it =
			m_VisibilityChangeToFalseList.begin();
		for (; it!=m_VisibilityChangeToFalseList.end();)
		{
			if ((*it).ActionTimeMs<=m_CurrentTimeMs)
			{
				ISceneNode* sn = (*it).Node;
				it = m_VisibilityChangeToFalseList.erase(
					sn->m_VisibilityChangeToFalseQueueIterator);
				sn->m_VisibilityChangeToFalseQueueIterator=0;
				sn->setVisible(false);
			}
			else
			{
				++it;
			}
		}
	}

	if (!m_VisibilityChangeToTrueList.empty())
	{
		core::list<SActionQueueEntry>::iterator it =
			m_VisibilityChangeToTrueList.begin();
		for (; it!=m_VisibilityChangeToTrueList.end();)
		{
			if ((*it).ActionTimeMs<=m_CurrentTimeMs)
			{
				ISceneNode* sn = (*it).Node;
				it = m_VisibilityChangeToTrueList.erase(
					sn->m_VisibilityChangeToTrueQueueIterator);
				sn->m_VisibilityChangeToTrueQueueIterator=0;
				sn->setVisible(true);
			}
			else
			{
				++it;
			}
		}
	}
} 

//----------------------------------------------------------------------------

void CSceneManager::clearVisibilityChangeQueue()
{    
	if (!m_VisibilityChangeToFalseList.empty())
	{
		core::list<SActionQueueEntry>::iterator it =
			m_VisibilityChangeToFalseList.begin();
		for (; it!=m_VisibilityChangeToFalseList.end(); ++it)
			(*it).Node->m_VisibilityChangeToFalseQueueIterator = 0;
		m_VisibilityChangeToFalseList.clear();
	}

	if (!m_VisibilityChangeToTrueList.empty())
	{
		core::list<SActionQueueEntry>::iterator it =
			m_VisibilityChangeToTrueList.begin();
		for (; it!=m_VisibilityChangeToTrueList.end(); ++it)
			(*it).Node->m_VisibilityChangeToTrueQueueIterator = 0;
		m_VisibilityChangeToTrueList.clear();
	}
} 

//----------------------------------------------------------------------------

void CSceneManager::addToVisibilityChangeQueue(
	ISceneNode* node, bool visibility, s32 delta_time_ms)
{
	if (!visibility)
	{
		if (node->m_VisibilityChangeToFalseQueueIterator == 0)
		{			
			SActionQueueEntry e(node, m_CurrentTimeMs+delta_time_ms);
			m_VisibilityChangeToFalseList.push_back(e);
			node->m_VisibilityChangeToFalseQueueIterator =
				m_VisibilityChangeToFalseList.get_last();
		}
		else
		{
			(*(node->m_VisibilityChangeToFalseQueueIterator)).ActionTimeMs =
				m_CurrentTimeMs+delta_time_ms;
		}
	}
	else
	{
		if (node->m_VisibilityChangeToTrueQueueIterator == 0)
		{			
			SActionQueueEntry e(node, m_CurrentTimeMs+delta_time_ms);
			m_VisibilityChangeToTrueList.push_back(e);
			node->m_VisibilityChangeToTrueQueueIterator =
				m_VisibilityChangeToTrueList.get_last();
		}
		else
		{
			(*(node->m_VisibilityChangeToTrueQueueIterator)).ActionTimeMs =
				m_CurrentTimeMs+delta_time_ms;
		}
	}
}

//----------------------------------------------------------------------------

ISceneNode* CSceneManager::getSceneNodeFromId(s32 id, ISceneNode* start)
{
	if (id == -1)
		return 0;
	core::list<ISceneNode*>::iterator it =
		start ? start->m_SceneNodesListIterator : 0;
	if (it == 0)
		it = m_SceneNodesList.begin();
	for (; it != m_SceneNodesList.end(); ++it)
		if ((*it)->getID() == id)
			return (*it);
    return NULL;
}

//----------------------------------------------------------------------------

void CSceneManager::setSceneClusterEnabled(u32 idx, bool enabled)
{
	if (idx <= MY_MAX_SCENE_CLUSTERS)
		m_SceneClusters[idx]->setEnabled(enabled);
}

//----------------------------------------------------------------------------

void CSceneManager::clearScene(u32 clear_mask, bool clear_cache)
{
	setActiveCamera(NULL);

	clearDeletionQueue();
	clearVisibilityChangeQueue();
	_clearSelectionList();
	clearVisibleNodesList();

	for (u32 i = 0; i < MY_MAX_SCENE_CLUSTERS; i++)
		if (clear_mask & (1 << i))
			m_SceneClusters[i]->removeAll();

	if (clear_cache)
	{	
		m_SceneClusters[MY_MAX_SCENE_CLUSTERS]->removeAll();
		m_LoadedEtalones.clear();   

		clearUnusedMeshCache();
		m_Driver.clearUnusedVideoCache();
		m_ImageLib.clearUnusedImageCache();
		m_SoundDriver.clearUnusedSoundCache();	
		m_ScriptMgr.clearScriptMemory();
	}
}

//----------------------------------------------------------------------------

void CSceneManager::setTexturesPath(core::stringc path)
{   
	TexturesPath = path;
}

//----------------------------------------------------------------------------

core::stringc CSceneManager::getTexturesPath()
{
	return TexturesPath;
}

//----------------------------------------------------------------------------

void CSceneManager::clearVisibleNodesList()
{
	core::list<ISceneNode*>::iterator it = m_VisibleSceneNodesList.begin();
	for (; it!=m_VisibleSceneNodesList.end(); ++it)
		(*it)->m_VisibleSceneNodesListIterator=0;
	m_VisibleSceneNodesList.clear();

	for (s32 snt=0; snt<E_SCENE_NODE_TYPE_COUNT; snt++)
	{
		it = m_VisibleSceneNodesListByType[snt].begin();
		for (; it!=m_VisibleSceneNodesListByType[snt].end(); ++it)
			(*it)->m_VisibleSceneNodesListByTypeIterator=0;
		m_VisibleSceneNodesListByType[snt].clear();
	}
}

//----------------------------------------------------------------------------

void CSceneManager::clearSelectionList()
{
	if (m_SelectionListFreezed)
		return;
	_clearSelectionList();
}

//----------------------------------------------------------------------------

void CSceneManager::_clearSelectionList()
{
	while (m_SelectedNodesList.size())
		_remSceneNodeFromSelectionList(*m_SelectedNodesList.begin());
}

//----------------------------------------------------------------------------

void CSceneManager::addSceneNodeToSelectionList(ISceneNode *node)
{
	if (m_SelectionListFreezed)
		return;
    if (node->m_SelectedNodesListIterator==0)
    {
        node->setDebugDataVisible(true);           
        m_SelectedNodesList.push_back(node);
		node->m_SelectedNodesListIterator = m_SelectedNodesList.get_last();

		SCRIPT_MANAGER.runScriptCallback(
			scr::ESCT_SCENE_EDITOR_ON_SELECT_SCENE_NODE, (bool*)NULL,
			(void*)node);
    }
}

//----------------------------------------------------------------------------

bool CSceneManager::remSceneNodeFromSelectionList(ISceneNode *node)
{ 
    if (m_SelectionListFreezed)
		return false;
    return _remSceneNodeFromSelectionList(node);
}

//----------------------------------------------------------------------------

bool CSceneManager::_remSceneNodeFromSelectionList(ISceneNode *node)
{
	if (node->m_SelectedNodesListIterator != 0)
    {
        node->setDebugDataVisible(false);
		m_SelectedNodesList.erase(node->m_SelectedNodesListIterator);
		node->m_SelectedNodesListIterator = 0;
		SCRIPT_MANAGER.runScriptCallback(
			scr::ESCT_SCENE_EDITOR_ON_DESELECT_SCENE_NODE, (bool*)NULL,
			(void*)node);
		return true;
    }
    return false;
}

//----------------------------------------------------------------------------

s32 CSceneManager::getSelectedSceneNodesCount()
{
    return m_SelectedNodesList.size();
}

//----------------------------------------------------------------------------

ISceneNode* CSceneManager::getSelectedSceneNode(s32 idx)
{
    if (idx>=0 && idx<(s32)m_SelectedNodesList.size())
	{
		ISceneNode *node = *(m_SelectedNodesList.begin() + idx);
		return node;
	}
    return NULL;
}

//---------------------------------------------------------------------------

ISceneNode* CSceneManager::getSceneNodeFromRayBB(core::line3d<f32> ray)
{
	ISceneNode* best = 0;
    f32 bestdistance = 9999999999.0f;
    core::vector3df linemiddle = ray.getMiddle();
    core::vector3df linevect   = ray.getVector().normalize();
	core::vector3df pos = ray.start; 		
	f32 halflength = (f32)(ray.getLength() * 0.5);

    core::list<ISceneNode*>::iterator it = m_SceneNodesList.begin();

    for (; it != m_SceneNodesList.end(); ++it)
    {
        ISceneNode* current = *it;

        if (current->isVisible())
        {
			core::vector3df abs_pos = current->getAbsolutePosition();
			core::aabbox3df tbb = current->getTransformedBoundingBox();

			if (current->getSceneNodeType()==ESNT_LIGHT_SCENE_NODE)
			{
				tbb.reset(current->getAbsolutePosition());
				tbb.MinEdge-=core::vector3df(10,10,10);
				tbb.MaxEdge+=core::vector3df(10,10,10);
			}

            if (tbb.intersectsWithLine(linemiddle, linevect, halflength))
            {
                f32 dist =  (f32)abs_pos.getDistanceFrom(pos);

                if (dist < bestdistance)
                {
                    best = current;
                    bestdistance = dist;
                }
            }
        }      
    }

	return best;
}

//---------------------------------------------------------------------------

const core::line3df& CSceneManager::getRayFromScreenCoordinates(const core::position2di &pos)
{
    static core::line3df ln;

    ICameraSceneNode* camera = getActiveCamera();

    if (!camera)
        return ln;

    const core::view_frustrum& f = camera->getViewFrustrum();

    core::vector3df farLeftUp   = f.getFarLeftUp();
    core::vector3df lefttoright = f.getFarRightUp() - farLeftUp;
    core::vector3df uptodown    = f.getFarLeftDown() - farLeftUp;

	core::dimension2d<s32> screenSize = VIDEO_DRIVER.getScreenSize();

    f32 dx = pos.X / (f32)screenSize.Width;
    f32 dy = pos.Y / (f32)screenSize.Height;

	if (camera->getProjectionType()==ECPT_PERSPECTIVE)
	{
		ln.start = f.Position;
	}
	else
	{
		core::vector3df nearLeftUp = f.getNearLeftUp();
		ln.start = nearLeftUp + (lefttoright * dx) + (uptodown * dy);
	}

    ln.end = farLeftUp + (lefttoright * dx) + (uptodown * dy);

    return ln;
}

//----------------------------------------------------------------------------

ISceneNode* CSceneManager::getSceneNodeFromScreenCoordinates(
	const core::position2di &scr_pos)
{	
	dyn::IDynamicManager        &dynmgr  = DYNAMIC_MANAGER;
	dev::IDevice                &device  = DEVICE;	

	core::line3d<f32> ray = getRayFromScreenCoordinates( scr_pos );

    scn::ISceneNode *node =0;

	core::vector3df dir = ray.getVector();
	dir.normalize();
	dir*=1000000;

	ray.end = ray.start + dir;    

	dyn::SCollisionPoints collisions;

	node = getSceneNodeFromRayBB(ray); 

	if (!node || node->getSceneNodeType()!=ESNT_LIGHT_SCENE_NODE)
	{
		if (dynmgr.findCollisionsWithRay(
				ray, dyn::EDCF_MONOLITH_COLLID|dyn::EDCF_DYNAMIC_COLLID, collisions
				)
			)
		// Dynamic Engine collision stuff
		{		 		
			// find nearest dynamic object

			scn::ISceneNode *node1 =0;

			s32 idx = collisions.getNearestTo(ray.start);

			dyn::IDynamicObject *dynobj = (idx != -1) ?
				collisions.getPoints()[idx].DynObj : NULL;
	                    
			if (dynobj) 
			{
				node1 = dynobj->getSceneNode();
			}

			if (node1)
			{
				node = node1;
			}
		}
	}

	return node;  
}

//---------------------------------------------------------------------------

const core::position2di& CSceneManager::getScreenCoordinatesFrom3DPosition(const core::vector3df &pos3d)
{
    static core::position2di pos2d(-1000,-1000);

    ICameraSceneNode* camera = getActiveCamera();

    if (!camera)
        return pos2d;

	core::dimension2d<s32> dim = VIDEO_DRIVER.getScreenSize();
    dim.Width /= 2;
    dim.Height /= 2;

    f32 transformedPos[4];

    core::matrix4 trans = camera->getProjectionMatrix();
    trans *= camera->getViewMatrix();

    transformedPos[0] = pos3d.X;
    transformedPos[1] = pos3d.Y;
    transformedPos[2] = pos3d.Z;
    transformedPos[3] = 1.0f;

    trans.multiplyWith1x4Matrix(transformedPos);

    if (transformedPos[3] < 0)
		pos2d.set(-10000,-10000);
	else
	{
		f32 zDiv = transformedPos[3] == 0.0f ? 1.0f :
			(1.0f / transformedPos[3]);

		pos2d.X = (s32)(dim.Width * transformedPos[0] * zDiv) + dim.Width;
		pos2d.Y = ((s32)(dim.Height - (dim.Height * (transformedPos[1] * zDiv))));
	}
    return pos2d;
}

//---------------------------------------------------------------------------

void CSceneManager::makeDirtSpot(
	core::vector3df abs_pos, core::vector3df size, img::SColor color, bool hipriority)
{
	core::list<ISceneNode*>::iterator it =
		m_SceneNodesListByType[ESNT_KTREE_SCENE_NODE].begin();
	for (; it!=m_SceneNodesListByType[ESNT_KTREE_SCENE_NODE].end(); ++it)
		((IKTreeSceneNode*)(*it))->makeDirtSpot(
			abs_pos, size, color, hipriority);
}

//---------------------------------------------------------------------------

void CSceneManager::onLightShadowCasted(const vid::SLight &light, scn::E_SHADOW_TYPE type,
	u32 index, const scn::ISceneNode *owner, const img::SColor &shadow_ground_color,
	const core::rectf& shadow_ground_rect, f32 shadow_ground_y, f32 zorder)
{
	u32 val = m_LightShadowCasted[type].get_value(light.Identificator);
	m_LightShadowCasted[type].set_value(light.Identificator, val + 1);

	if (light.Dynamic)
		return;

	u32 key		= index;
	SShadowGround* sg = m_ShadowGrounds.get_value(key);
	if (!sg)
	{
		sg = new SShadowGround();
		m_ShadowGrounds.set_value(key, sg);
	}
	s32 i = sg->Array.binary_search(SShadowGroundEntry(
		zorder, owner));
	if (i == -1)
	{
		sg->Array.push_back(SShadowGroundEntry(
			shadow_ground_rect, shadow_ground_y, zorder,
			owner, shadow_ground_color));
	}
	else
	{
		if (sg->Array[i].Rect.Left > shadow_ground_rect.Left)
			sg->Array[i].Rect.Left = shadow_ground_rect.Left;
		if (sg->Array[i].Rect.Top > shadow_ground_rect.Top)
			sg->Array[i].Rect.Top = shadow_ground_rect.Top;
		if (sg->Array[i].Rect.Right < shadow_ground_rect.Right)
			sg->Array[i].Rect.Right = shadow_ground_rect.Right;
		if (sg->Array[i].Rect.Bottom < shadow_ground_rect.Bottom)
			sg->Array[i].Rect.Bottom = shadow_ground_rect.Bottom;
		if (sg->Array[i].Y < shadow_ground_y)
			sg->Array[i].Y = shadow_ground_y;
	}
}

//---------------------------------------------------------------------------

bool CSceneManager::isLightShadowCasted(const vid::SLight &light, scn::E_SHADOW_TYPE type)
{
	return m_LightShadowCasted[type].get_value(light.Identificator) > 0;
}

//---------------------------------------------------------------------------

bool CSceneManager::isLightShadowCasted(const vid::SLight &light)
{
	for (u32 i = 0; i < E_SHADOW_TYPE_COUNT; i++)
		if (m_LightShadowCasted[i].get_value(light.Identificator) > 0)
			return true;
	return false;
}

//---------------------------------------------------------------------------

IShadow* CSceneManager::createShadow(ISceneNode *parent, E_SHADOW_TYPE type)
{
	if (!parent || 
		!DEVICE.getDeviceFlagValue(dev::EDCF_SHADOWS))
        return NULL;

	return (type == ESHT_SHADOW_VOLUME) ?
		new CShadowVolume(parent) : new CShadowFake(parent);
}

//---------------------------------------------------------------------------

const c8* CSceneManager::findMeshFileName(IAnimatedMesh* amesh)
{
	const c8* fullfilename = findMeshFullFileName(amesh);
	static core::stringc filename;
	filename = core::extractFileName(fullfilename);
	if (filename.size()==0)
		filename = NONAME_FILE_NAME;
    return filename.c_str();
}

//---------------------------------------------------------------------------

const c8* CSceneManager::findMeshFullFileName(IAnimatedMesh* amesh)
{
    static core::stringc filename;

	core::list <MeshListEntry*>::iterator it = m_MeshesHash.get_value(amesh);
	if (it != 0)
		filename = (*it)->Name;
	else
		filename = NONAME_FILE_NAME;
	filename.make_lower();

	if (filename.size() == 0)
		filename = NONAME_FILE_NAME;

    return filename.c_str();
}

//----------------------------------------------------------------------------

IAnimatedMesh* CSceneManager::findMesh(const c8* lowerMadeFilename)
{
	MeshListEntry el;
	MeshArrayEntry ea;
	
	el.Name = lowerMadeFilename;
    el.Name.make_lower();

	ea.ListEntry = &el;

	if (el.Name == "#box_mesh" ||
		el.Name == "#box_mesh_1000_50_1000" ||
		el.Name == "#box_mesh_50_100_50" ||
		el.Name == "#sphere_mesh" ||
		el.Name == "#cylinder_mesh" ||
		el.Name == "#caped_cylinder_mesh")
	{
		el.Name = core::stringc().sprintf("%s%s", RESOURCE_MANAGER.getMediaDirFull(res::EMT_MESHES),
			el.Name.c_str());
		el.Name.make_lower();
	}

    s32 idx = m_MeshesArray.binary_search(ea);
    return (idx != -1) ? m_MeshesArray[idx].ListEntry->Mesh : 0;
}

//---------------------------------------------------------------------------

void CSceneManager::setTerrainFogRenderingMode(
	E_TERRAIN_FOG_RENDERING_MODE terrain_fog_rendering_mode)
{
	m_TerrainFogRenderingMode = terrain_fog_rendering_mode;
}

//---------------------------------------------------------------------------

E_TERRAIN_FOG_RENDERING_MODE CSceneManager::getTerrainFogRenderingMode()
{
	return m_TerrainFogRenderingMode;
}

//---------------------------------------------------------------------------

void CSceneManager::setSceneRenderingMode(E_SCENE_RENDERING_MODE srmode)
{
	m_SceneRenderingMode = srmode;
}

//---------------------------------------------------------------------------

E_SCENE_RENDERING_MODE CSceneManager::getSceneRenderingMode()
{
	return m_SceneRenderingMode;
}

//----------------------------------------------------------------------------

void CSceneManager::clearMeshCache()
{
	u32 nMeshes = m_MeshesArray.size(), nDrops = 0;
	core::list <MeshListEntry*>::iterator it = m_MeshesList.begin();
	for (; it != m_MeshesList.end(); ++it)
	{
		MeshListEntry *mle = *it;
		IAnimatedMesh *amesh = mle->Mesh;
        if (amesh->isLocked())
			amesh->unlockObject();
		amesh->drop();
		delete mle;
		nDrops++;
	}
	m_MeshesArray.clear();
	m_MeshesList.clear();
	m_MeshesHash.clear();

	m_SphereMesh = m_CylinderMesh = m_CapedCylinderMesh = NULL;
	m_BoxMesh = m_BoxMesh_50_100_50 = m_BoxMesh_1000_50_1000 = NULL;	

	if (nMeshes != nDrops)
	{
		LOGGER.logWarn("Seems like, meshes hash corrupted.");
		LOGGER.logInfo("Mesh cache (%d meshes)(from %d at all) has been cleared.", nDrops, nMeshes);
	}
	else
		LOGGER.logInfo("Mesh cache (%d meshes) has been cleared.", nDrops );

	createPrimitivesMeshes();
}

//----------------------------------------------------------------------------

void CSceneManager::clearUnusedMeshCache()
{
	u32 nLocks = 0, nDrops = 0, m = 0;
	s32 nMeshes = m_MeshesArray.size();
	core::list<MeshListEntry*>::iterator it = 0;

	while (m_MeshesArray.size() > nLocks)
	{
		it = m_MeshesArray[m].ListIter;
		MeshListEntry *mle = *it;
		IAnimatedMesh *amesh = mle->Mesh;

		if (!amesh->isLocked() && amesh->getReferenceCounter()==1)
		{
			m_MeshesArray.erase(m);
			m_MeshesList.erase(it);
			delete mle;
			m_MeshesHash.erase_value(amesh);
			SAFE_DROP(amesh);
			nDrops++;
		}
		else
		{
			nLocks++;  
			m++;
		}
	}	

	if (nDrops > 0)
	{
		LOGGER.logInfo("Unused mesh cache (%d meshes)(from %d at all) has been cleared.",
			nDrops, nMeshes);
	}
}

//----------------------------------------------------------------------------

ISceneNode* CSceneManager::findSceneNodeEtalon(const c8* file_name)
{       
	SEtalonEntry e(NULL, file_name);
	s32 idx = m_LoadedEtalones.binary_search(e);
	if (idx!=-1)
	{
		return m_LoadedEtalones[idx].Node;
	} 

	return NULL;
}

//----------------------------------------------------------------------------

bool CSceneManager::removeSceneNodeEtalon(const c8* file_name)
{          
	SEtalonEntry e(NULL,file_name);
	s32 idx = m_LoadedEtalones.binary_search(e);
	if (idx != -1)
	{
		ISceneNode *node = m_LoadedEtalones[idx].Node;
		IAnimatedMeshSceneNode *ameshnode = SAFE_CAST_TO_ANIMATED_MESH_SCENE_NODE(node);
		if (ameshnode)
		{
			for (u32 i = 0; i < E_LOD_LEVEL_COUNT; i++)
			{
				IAnimatedMesh *amesh = ameshnode->getAnimatedMesh((E_LOD_LEVEL)i);
				if (amesh)
					removeMesh(amesh);
			}
		}		
		node->remove();
		
		m_LoadedEtalones.erase(idx);    
		return true;
	} 

	return false;
}

//----------------------------------------------------------------------------

ISceneNode* CSceneManager::createSceneNode(
	SSceneNodeCommonParams &snparams, bool log_info)
{
	scn::ISceneNode *node = 0;

	if (SceneNodeCreatorList[snparams.Parameters.Type])
	{
		node = SceneNodeCreatorList[snparams.Parameters.Type]->createSceneNode(
			snparams, log_info);
	}

	if (!node)
	{
		LOGGER.logErr("Can not create %s Scene Node!", 
			SceneNodeTypeShortStr[snparams.Parameters.Type]);
	}

	return node;
}

//----------------------------------------------------------------------------

scn::ISceneNode* CSceneManager::cloneSceneNode(
	scn::ISceneNode *node_to_clone, bool with_animators, bool log_info)
{
	if (!node_to_clone) return 0;

	scn::ISceneNode *node = 0;

	if (SceneNodeClonerList[node_to_clone->getSceneNodeType()])
	{
		node = SceneNodeClonerList[node_to_clone->getSceneNodeType()]->cloneSceneNode(
			node_to_clone, log_info);

		if (node)
		{
			node->setParameters( node_to_clone->getParameters() );				
			node->setFileName(node_to_clone->getFileName());

			if (with_animators)
			{
				const core::list<ISceneNodeAnimator*>&ani =  node_to_clone->getAnimators();

				core::list<ISceneNodeAnimator*>::iterator ait = ani.begin();
				for (; ait != ani.end(); ++ait)
				{
					ISceneNodeAnimator *a =(*ait);

					if (a->getAnimatorType() != ESNAT_UNKNOWN_ANIMATOR)
					{
						ISceneNodeAnimator *new_a = CAnimatorsCloner::clone(a);

						if (new_a)
						{
							node->addAnimator(new_a);

							// one grab is much enough
							new_a->drop();
						}
					}
				}
			}
		}
	}

	if (!node)
	{
		LOGGER.logErr("Can not clone %s Scene Node!", 
			SceneNodeTypeShortStr[node_to_clone->getSceneNodeType()]);
	}

	return node;
}

//----------------------------------------------------------------------------

ISceneNode* CSceneManager::reloadSceneNode(ISceneNode *node)
{
	if (!node)
		return NULL;

	SSceneNodeCommonParams node_params = node->getParameters();

	bool dbg_vis = node->isDebugDataVisible();

	scn::ISceneNode* parent = node->getParent();
	core::stringc file_name = node->getFileName();
	core::stringc node_name = node->getName();	

	// removing scene node etalon, if it exists
	if (findSceneNodeEtalon(file_name.c_str()))
		removeSceneNodeEtalon(file_name.c_str());

	// removing scene node
	if (node->remove())	LOGGER.logInfo("Node '%s' was dropped", node_name.c_str());	
	else                LOGGER.logInfo("Can't drop '%s' Node",  node_name.c_str());	

	// loading scene node again
	node = loadSceneNode(file_name.c_str(), true);
	if (node)
	{
		node->setDebugDataVisible(dbg_vis);
		node->setParent(parent);
		node->setParameters(node_params.Transformation);
		node->setParameters(node_params.Parameters);			
	}

	return node;
}

//----------------------------------------------------------------------------

ISceneNode* CSceneManager::loadSceneNode(io::IXMLReader* xml, bool cache_it)
{
	if (!xml)
		return NULL;	

	scn::ISceneNode *etalon_node = findSceneNodeEtalon(xml->getFileName());

	if (etalon_node)
		return cloneSceneNode(etalon_node, true, true);

	scn::ISceneNode *node = 0;

	core::stringc sntype_str_fromxml = SceneNodeTypeStr[ESNT_UNKNOWN_SCENE_NODE];;
	E_SCENE_NODE_TYPE sntype_fromxml = ESNT_UNKNOWN_SCENE_NODE;
	
	bool finded_type = false;
	s32 xml_pos = xml->getPos();
	while(xml && xml->read() && !finded_type)
	{
		if (xml->getNodeType()==io::EXN_ELEMENT)
		{    
			if (core::stringw("Scene") == xml->getName())
			{
				sntype_str_fromxml = xml->getAttributeValue(L"scene_node_type");	
					
				for (s32 snt=0; snt<ESNT_UNKNOWN_SCENE_NODE; ++snt)
				{
					if (sntype_str_fromxml==SceneNodeTypeStr[snt])
					{
						sntype_fromxml = (E_SCENE_NODE_TYPE)snt;
						finded_type = true;						
					}
				}
			}
		}
	}	
	xml->setPos(xml_pos);

	if (finded_type && sntype_fromxml != ESNT_UNKNOWN_SCENE_NODE)
	{
		u32 rootIdx = getCurrentSceneClusterIndex();
		if (cache_it)
			setCurrentSceneClusterIndex(getCacheSceneClusterIndex()); // cache cluster
		if (SceneNodeLoaderList[sntype_fromxml])
			node = SceneNodeLoaderList[sntype_fromxml]->loadSceneNode(xml);
		if (cache_it)
			setCurrentSceneClusterIndex(rootIdx);
	}
	else
	{
		LOGGER.logErr("Unable load Scene Node from file %s !", xml->getFileName());
	}
	if (!node)
	{
		LOGGER.logErr("Can not load Scene Node from file %s !", xml->getFileName());
		return NULL;
	}

	if (cache_it)
	{
		core::const_stringc fname(node->getFileName());
		if (fname.size() > 0 && fname != NONAME_FILE_NAME)
		{
			m_LoadedEtalones.push_back(SEtalonEntry(node,fname.c_str()));  		
			node = cloneSceneNode(node, true, true);       
			m_LoadedEtalones[m_LoadedEtalones.size()-1].Node->setVisible(false);
		}
		else
		{
			if (node->getParent() == getSceneClusterRootSceneNode(getCacheSceneClusterIndex()))
				node->setParent(getSceneClusterRootSceneNode(getCurrentSceneClusterIndex()));
		}
	}
	
	return node;			
}

//----------------------------------------------------------------------------

ISceneNode* CSceneManager::loadSceneNode(const c8* filename, bool cache_it)
{
	ISceneNode *node = 0;
	io::IXMLReader *xml = FILE_SYSTEM.createXMLReader(filename);
	if (xml)
	{
		node = loadSceneNode(xml, cache_it);
		xml->drop();
	}
	else
		LOGGER.logErr("Can not load Scene Node from %s !", filename);
	return node;
}

//----------------------------------------------------------------------------

bool CSceneManager::saveSceneNode(const c8* filename, ISceneNode *node)
{
	if (!node)
		return false;

	io::IXMLWriter *xml =
		FILE_SYSTEM.createXMLWriter(filename);

	if (!xml)
	{
		LOGGER.logErr("Can not save Scene Node to %s !", filename);
		return false;
	}

	core::stringw strw = L"Saved at ";
	strw.append(core::stringw(os::getCurrentTimeString()));

	xml->writeXMLHeader();
	xml->writeComment(strw.c_str());
	xml->writeLineBreak();
	xml->writeElement(L"Scene", false, L"scene_node_type", core::stringw(
		scn::SceneNodeTypeStr[node->getSceneNodeType()]).c_str());
	xml->writeLineBreak();
	xml->writeElement(core::stringw(scn::getSceneNodeCorePropertiesTagValue()).c_str(), false);
	xml->writeLineBreak();
	xml->write(core::stringw(node->getSceneCorePropertiesXMLString()).c_str());
	xml->writeClosingTag(core::stringw(scn::getSceneNodeCorePropertiesTagValue()).c_str());
	xml->writeLineBreak();
	xml->writeClosingTag(L"Scene");

	SAFE_DROP(xml);

	return true;
}

//----------------------------------------------------------------------------

const s32 MAX_STRLEN=255;

//----------------------------------------------------------------------------

const c8* CSceneManager::getMaterialXMLText(
	const vid::SMaterial &material, bool with_textures)
{
	static core::stringc material_xml;
	s32 mat_idx = m_MaterialsManager->findMaterialByHash(material.getHash());
	const c8 *name = m_MaterialsManager->getMaterialName(mat_idx);
	if (name)
		material_xml.sprintf("<Material name=\"%s\" />\n", name);
	else
		CMaterialFileParser::getMaterialXMLTextTo(
			material_xml, material, with_textures);
	return material_xml.c_str();
}

//----------------------------------------------------------------------------

void CSceneManager::saveSceneNodeParameters(
	io::IXMLWriter* xml, SSceneNodeParameters &parameters)
{
	if (!xml)
		return;

	core::stringw strw1, strw2;

	strw1.sprintf(L"%d", parameters.ID);
	strw2.sprintf(L"%d", parameters.ParentID);

	xml->writeElement(
		L"Parameters", true, 
		L"name",      core::stringw(parameters.Name.c_str()).c_str(),
		L"id",        strw1.c_str(),	
		L"parent_id", strw2.c_str());
	xml->writeLineBreak();
}

//----------------------------------------------------------------------------

void CSceneManager::saveSceneNodeTransformation(
	io::IXMLWriter* xml, SSceneNodeTransformation &transformation
	)
{
	if (!xml)
		return;

	wchar_t strw1[MAX_STRLEN], strw2[MAX_STRLEN], strw3[MAX_STRLEN];

	xml->writeElement(L"Transformation");
	xml->writeLineBreak();	

	swprintf(strw1, MAX_STRLEN, L"%.4f", transformation.Position.X);
	swprintf(strw2, MAX_STRLEN, L"%.4f", transformation.Position.Y);
	swprintf(strw3, MAX_STRLEN, L"%.4f", transformation.Position.Z);
	xml->writeElement(L"Position", true, L"x", strw1, L"y", strw2, L"z", strw3);
	xml->writeLineBreak();

	swprintf(strw1, MAX_STRLEN, L"%.4f", transformation.Rotation.X);
	swprintf(strw2, MAX_STRLEN, L"%.4f", transformation.Rotation.Y);
	swprintf(strw3, MAX_STRLEN, L"%.4f", transformation.Rotation.Z);
	xml->writeElement(L"Rotation", true, L"x", strw1, L"y", strw2, L"z", strw3);
	xml->writeLineBreak();

	swprintf(strw1, MAX_STRLEN, L"%.4f", transformation.Scale.X);
	swprintf(strw2, MAX_STRLEN, L"%.4f", transformation.Scale.Y);
	swprintf(strw3, MAX_STRLEN, L"%.4f", transformation.Scale.Z);
	xml->writeElement(L"Scale", true, L"x", strw1, L"y", strw2, L"z", strw3);
	xml->writeLineBreak();

	xml->writeClosingTag(L"Transformation");
	xml->writeLineBreak();
}

//----------------------------------------------------------------------------

SSceneNodeMaterial CSceneManager::loadSceneNodeMaterial(
	io::IXMLReader* xml, bool load_textures)
{
	SSceneNodeMaterial material;

	if (xml &&
		core::stringw("Material") == xml->getName() && 
		xml->getNodeType()==io::EXN_ELEMENT)
    {
		core::stringc name = xml->getAttributeValue(L"name");

		vid::E_VERTEX_TYPE vtype;
		if (name.size() > 0)
			if (m_MaterialsManager->getMaterialAndVertexType(
					name.c_str(), 0, material.Material, vtype))
				return material;

		SMeshBufferMaterial mb_material;
		CMaterialFileParser::loadMaterialTo(xml, mb_material, load_textures);
		material.Material = mb_material.Material;
	}

	return material;
}

//----------------------------------------------------------------------------

SSceneNodeParameters CSceneManager::loadSceneNodeParameters(io::IXMLReader* xml)
{
	SSceneNodeParameters parameters;

	if (xml && core::stringw("Parameters") == xml->getName() && 
			xml->getNodeType() == io::EXN_ELEMENT)
	{
		parameters.Type		= scn::ESNT_UNKNOWN_SCENE_NODE;			
		parameters.Name		= xml->getAttributeValue(L"name");
		parameters.ID		= xml->getAttributeValueAsInt(L"id");
		parameters.ParentID	= xml->getAttributeValueAsInt(L"parent_id");
	}
	return parameters;
}

//----------------------------------------------------------------------------

SSceneNodeTransformation CSceneManager::loadSceneNodeTransformation(io::IXMLReader* xml)
{
	SSceneNodeTransformation transformation;

	if (xml && core::stringw("Transformation") == xml->getName() && 
		xml->getNodeType()==io::EXN_ELEMENT)
	{
		int xml_elem_cnt =0;

		s32 xml_pos = xml->getPos();

		while(xml && xml_elem_cnt<3 && xml->read())
		{
			switch(xml->getNodeType())
			{
			case io::EXN_ELEMENT:
				{
					xml_elem_cnt++;
					core::stringw xml_node_name = xml->getName();

					if (core::stringw("Position") == xml_node_name)
					{
						transformation.Position.X = xml->getAttributeValueAsFloat(L"x");
						transformation.Position.Y = xml->getAttributeValueAsFloat(L"y");
						transformation.Position.Z = xml->getAttributeValueAsFloat(L"z");
					}
					else if (core::stringw("Rotation") == xml_node_name)
					{
						transformation.Rotation.X = xml->getAttributeValueAsFloat(L"x");
						transformation.Rotation.Y = xml->getAttributeValueAsFloat(L"y");
						transformation.Rotation.Z = xml->getAttributeValueAsFloat(L"z");
					}
					else if (core::stringw("Scale") == xml_node_name)
					{
						transformation.Scale.X = xml->getAttributeValueAsFloat(L"x");
						transformation.Scale.Y = xml->getAttributeValueAsFloat(L"y");
						transformation.Scale.Z = xml->getAttributeValueAsFloat(L"z");
					}
					else
					{
						xml_elem_cnt=3; 

						xml->setPos(xml_pos);						
					}
					xml_pos = xml->getPos();
				}
				break;
			}
		}
	}
	return transformation;
}

//----------------------------------------------------------------------------

f32 CSceneManager::getLODDistance(E_LOD_LEVEL lodlevel)
{
	return LODDistances[lodlevel];
}

//----------------------------------------------------------------------------

void CSceneManager::setLODDistance(E_LOD_LEVEL lodlevel, f32 dist)
{
	if (lodlevel != ELL_LOD_FARTHEST)
		LODDistances[lodlevel] = dist;
}

//----------------------------------------------------------------------------

E_LOD_LEVEL CSceneManager::getLODFromDistance(f32 dist, E_LOD_LEVEL oldlod)
{
	E_LOD_LEVEL newlod = oldlod;

	f32 histeresis = 0.999f;

	dist = core::math::Abs(dist);

	if (dist > histeresis * LODDistances[ELL_LOD_FAR])
	{		
		newlod = ELL_LOD_FARTHEST;     
	}
	else 
	if (dist > LODDistances[ELL_LOD_NEAR] &&
		dist < histeresis*LODDistances[ELL_LOD_FAR]) 
	{
		newlod = ELL_LOD_FAR;     
	}
	else 
	if (dist > LODDistances[ELL_LOD_NEAREST] &&
		dist < histeresis*LODDistances[ELL_LOD_NEAR])      
	{
		newlod = ELL_LOD_NEAR; 
	}
	else 
	if (dist < histeresis*LODDistances[ELL_LOD_NEAREST])      
	{
		newlod = ELL_LOD_NEAREST; 
	}	

	return newlod;
}

//----------------------------------------------------------------------------

void CSceneManager::setOctTreeCullingEnabled(bool enabled)
{
	m_OctTreeCullingEnabled = enabled;
}

//----------------------------------------------------------------------------

bool CSceneManager::isOctTreeCullingEnabled()
{
	return m_OctTreeCullingEnabled;
}

//----------------------------------------------------------------------------

void CSceneManager::setOccludersCullingEnabled(bool enabled)
{
	m_OccludersCullingEnabled = enabled;
}

//----------------------------------------------------------------------------

bool CSceneManager::isOccludersCullingEnabled()
{
	return m_OccludersCullingEnabled;
}

//----------------------------------------------------------------------------

bool CSceneManager::calculateAdjacency(IAnimatedMesh *amesh)
{
	if (!amesh)
		return false;

	core::stringc fname = findMeshFullFileName(amesh);

	LOGGER.logInfo("Creating adjacency info for mesh '%s' . . .",
		fname.c_str());
	LOGGER.increaseFormatLevel();

	IMesh* mesh = amesh->getMesh();

	s32 bufcnt = mesh->getMeshBuffersCount();
	IMeshBuffer* b;
	s32 i;

	for (i = 0; i < bufcnt; ++i)
    {
        b = mesh->getMeshBuffer(i);
		if (b->getIndexType()!=EIT_16_BIT)
			return false;
    }

	SAdjacencyData &adj = amesh->getAdjacencyData();

	if (mesh && adj.Adjacency==NULL)
	{
		LOGGER.increaseFormatLevel();

		// calculate total amount of vertices and indices

		s32 i;
		s32 totalVertices = 0;
		s32 totalIndices = 0;
		s32 bufcnt = mesh->getMeshBuffersCount();
		IMeshBuffer* b;

		for (i = 0; i < bufcnt; ++i)
		{
			b = mesh->getMeshBuffer(i);
			if (b->getMaterial().getPass(0).isTransparent())
				continue;
			totalIndices += b->getIndexCount();
			totalVertices += b->getVertexCount();
		}

		if (!totalVertices || !totalIndices)
			return false;

		// allocate memory

		vid::S3DVertexSimple *Vertices = new vid::S3DVertexSimple[totalVertices];
		u16 *Indices = new u16[totalIndices];

		// copy vertices and indices from original mesh

		s32 VertexCount = 0;
		s32 IndexCount = 0;
    
		for (i = 0; i < bufcnt; ++i)
		{
			b = mesh->getMeshBuffer(i);
			if (b->getMaterial().getPass(0).isTransparent())
				continue;
			u32 vtxcnt = b->getVertexCount();
			u32 idxcnt = b->getIndexCount();
			s32 vtxnow = VertexCount;
			for (u32 i = 0; i < idxcnt; i++)
				Indices[IndexCount++] = b->getIndex(i) + vtxnow;
			for (u32 i = 0; i < vtxcnt; i++)
				Vertices[VertexCount++].Pos = b->getVertex(i).Pos;	
		}

		u32 errors = 0;
		adj.Adjacency = mesh_buffer_manipulator::create_edge_adjacency_data<u16>(
			vid::S3DVertexSimple::Type, Vertices, Indices, IndexCount, &errors, 0.0001f);
		adj.IndexCount = IndexCount;

		SAFE_DELETE_ARRAY(Vertices);
		SAFE_DELETE_ARRAY(Indices);

		LOGGER.decreaseFormatLevel();
		if (errors)
			LOGGER.logInfo("Adjacency info created (mesh errors %d)",
				errors);
		else
			LOGGER.logInfo("Adjacency info created");
		LOGGER.decreaseFormatLevel();

		return true;
	}

	LOGGER.logErr("Can't create adjacency info");

	LOGGER.decreaseFormatLevel();

	return false;
}

//----------------------------------------------------------------------------

void CSceneManager::attachCamera(ISceneNode *scene_node, ICameraSceneNode* camera)
{
	SAFE_DROP(scene_node->m_Camera);
	scene_node->m_Camera = camera; 	
	SAFE_GRAB(camera);
}

//----------------------------------------------------------------------------

void CSceneManager::detachCamera(ISceneNode *scene_node)
{
	ICameraSceneNode *camera = scene_node->m_Camera;
	SAFE_DROP(scene_node->m_Camera);
	if (camera && camera->getReferenceCounter()==1)
	// remove camera, attached to this scene node, if it no needs to some others
		camera->remove();
}

//----------------------------------------------------------------------------
//!                       scene callbacks
//----------------------------------------------------------------------------

void CSceneManager::onStartSceneEditor()
{
	if (m_SceneEditorStarted)
	{
		LOGGER.logWarn("Scene Editor already started");
		return;
	}
	m_SceneEditorStarted = true;
	_clearSelectionList();
	LOGGER.logInfo("My Scene Editor started");
}

//----------------------------------------------------------------------------

void CSceneManager::onStopSceneEditor()
{
	if (!m_SceneEditorStarted)
	{
		LOGGER.logWarn("There is no Scene Editor to stop");
		return;
	}
	m_SceneEditorStarted = false;
	_clearSelectionList();
	LOGGER.logInfo("My Scene Editor stopped");
}

//----------------------------------------------------------------------------

void CSceneManager::onCreateSceneNode(scn::ISceneNode* snode)
{
	m_SceneNodesList.push_back(snode);     
	snode->m_SceneNodesListIterator = m_SceneNodesList.get_last();

	E_SCENE_NODE_TYPE snt = snode->getSceneNodeType();
	m_SceneNodesListByType[snt].push_back(snode);
	snode->m_SceneNodesListByTypeIterator = m_SceneNodesListByType[snt].get_last();

	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_SCENE_EDITOR_ON_CREATE_SCENE_NODE, (bool*)NULL,
		(void*)snode);
}

//----------------------------------------------------------------------------

void CSceneManager::onDeleteSceneNode(scn::ISceneNode* snode)
{
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_SCENE_EDITOR_ON_DELETE_SCENE_NODE, (bool*)NULL,
		(void*)snode);

	_remSceneNodeFromSelectionList(snode);

	m_SceneNodesList.erase(snode->m_SceneNodesListIterator);
	snode->m_SceneNodesListIterator=0;

	m_SceneNodesListByType[snode->getSceneNodeType()].erase(
		snode->m_SceneNodesListByTypeIterator);
	snode->m_SceneNodesListByTypeIterator=0;

	if (snode->m_OccludersListIterator!=0)
		m_OccludersList.erase(snode->m_OccludersListIterator);
	snode->m_OccludersListIterator=0;

	if (snode->m_VisibleSceneNodesListIterator!=0)
		m_VisibleSceneNodesList.erase(
			snode->m_VisibleSceneNodesListIterator);
	snode->m_VisibleSceneNodesListIterator=0;

	if (snode->m_VisibleSceneNodesListByTypeIterator!=0)
		m_VisibleSceneNodesListByType[snode->getSceneNodeType()].erase(
			snode->m_VisibleSceneNodesListByTypeIterator);
	snode->m_VisibleSceneNodesListByTypeIterator=0;

	if (snode->m_DeletionQueueIterator!=0)
		m_DeletionList.erase(snode->m_DeletionQueueIterator);
	snode->m_DeletionQueueIterator=0;

	if (snode->m_VisibilityChangeToFalseQueueIterator!=0)
		m_VisibilityChangeToFalseList.erase(
			snode->m_VisibilityChangeToFalseQueueIterator);
	snode->m_VisibilityChangeToFalseQueueIterator=0;

	if (snode->m_VisibilityChangeToTrueQueueIterator!=0)
		m_VisibilityChangeToTrueList.erase(
			snode->m_VisibilityChangeToTrueQueueIterator);
	snode->m_VisibilityChangeToTrueQueueIterator=0;
}

//----------------------------------------------------------------------------

void CSceneManager::onSetOccluder(scn::ISceneNode* snode, bool occluder)
{
	if (occluder)
	{
		if (snode->m_OccludersListIterator==0)
		{
			m_OccludersList.push_back(snode);     
			snode->m_OccludersListIterator = m_OccludersList.get_last();			
		}
	}
	else
	{
		if (snode->m_OccludersListIterator!=0)
			m_OccludersList.erase(snode->m_OccludersListIterator);
		snode->m_OccludersListIterator=0;
	}
}

//----------------------------------------------------------------------------

void CSceneManager::onAnimationBegin(ISceneNode *snode, u32 animation_idx)
{
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_ANIMATION_BEGIN, (bool*)NULL,
		(void*)snode, animation_idx);
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_SCENE_EDITOR_ON_ANIMATION_BEGIN, (bool*)NULL,
		(void*)snode, animation_idx);
}

//---------------------------------------------------------------------------

void CSceneManager::onAnimationEnd(ISceneNode *snode, u32 animation_idx)
{
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_ON_ANIMATION_END, (bool*)NULL,
		(void*)snode, animation_idx);
	SCRIPT_MANAGER.runScriptCallback(
		scr::ESCT_SCENE_EDITOR_ON_ANIMATION_END, (bool*)NULL,
		(void*)snode, animation_idx);
}

//----------------------------------------------------------------------------

__MY_EXPORT__ ISceneManager* createSceneManager()
{
    return new CSceneManager();
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my 
//----------------------------------------------------------------------------


