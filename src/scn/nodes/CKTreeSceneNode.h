//|-------------------------------------------------------------------------
//| File:        CKTreeSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CKTreeSceneNodeHPP
#define CKTreeSceneNodeHPP
//---------------------------------------------------------------------------

#include <scn/IKTreeSceneNode.h>
#include <scn/IMesh.h>
#include <scn/mesh_buffer.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class KTree;
struct SRenderGeometry;

//---------------------------------------------------------------------------

class CKTreeSceneNode : public IKTreeSceneNode
{
public:

    CKTreeSceneNode(ISceneNode* parent, s32 id);
    virtual ~CKTreeSceneNode();

	// ISceneNode implementation

	virtual void OnCheckCulling();
    virtual void OnPreRender(u32 timeMs);

    virtual const core::aabbox3df& getBoundingBox() const
	{ return Box; }

    virtual vid::SMaterial& getMaterial(s32 i);
    
    virtual s32 getMaterialsCount();

    virtual void updateAbsoluteTransformation();

	virtual void setPhysicalMesh(IAnimatedMesh* mesh);

	virtual IAnimatedMesh* getPhysicalMesh();

	virtual const c8* getPhysicalMeshFullFileName();

	virtual bool rescaleNormals(const core::vector3df& new_scale);

	virtual bool set(ISceneNode* other);

	virtual const c8* getSceneCorePropertiesXMLString();

	// ILightGrid implementation

	virtual vid::SLight getLightFromPos(core::vector3df pos);
	
	virtual bool isPointInside(core::vector3df point);

	// IAnimatedMeshSceneNode implementation

	virtual const c8* getAnimationFileName(u32 idx) { return 0; }
	virtual u32 getAnimationBeginFrame(u32 idx) { return 0; }
	virtual u32 getAnimationEndFrame(u32 idx) { return 0; }
	virtual f32 getAnimationSpeed(u32 idx) { return 0; }
	virtual bool getAnimationLoopMode(u32 idx) { return false; }

	virtual void setAnimationBeginFrame(u32 idx, u32 frame) {}
	virtual void setAnimationEndFrame(u32 idx, u32 frame) {}
	virtual void setAnimationSpeed(u32 idx, f32 speed) {}
	virtual void setAnimationLoopMode(u32 idx, bool looped) {}

	virtual void setAnimationOveralSpeed(f32 speed) {}
	virtual f32 getAnimationOveralSpeed() { return 0; }

	virtual bool isAnimationOveralSmooth() { return false; }
	virtual void setAnimationOveralSmooth(bool flag) {}

    virtual u32 getAnimationOveralFramesCount() { return 0; }

	virtual IAnimatedMesh* getAnimatedMesh(E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST)
	{ return m_AnimatedMesh; }

	virtual void setAnimatedMesh(IAnimatedMesh* amesh, E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST);

	virtual IMesh* getMesh(E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST);
	virtual IMesh* getMesh(f32 frame, E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST);

	virtual bool attachToBone(ISceneNode* node, const c8 *bone_name)
	{ return false; }

	// IKtreeSceneNode implementation

	virtual void setMaxTrianglesPerNode(s32 max_triangles_per_node);

	virtual s32 getMaxTrianglesPerNode();

    virtual bool recreateTree();

	virtual void makeDirtSpot(
		const core::vector3df &abs_pos, const core::vector3df &size,
		const img::SColor &color, bool hipriority = false);

private:

	struct SDirtUpdateTask
	{
		SDirtUpdateTask() {}
		SDirtUpdateTask(const core::vector3df &pos,
			const core::vector3df &size, const img::SColor &color)
			: Pos(pos), Size(size), Color(color) {}
		core::vector3df  Pos; 
		core::vector3df  Size;
		img::SColor      Color;		
	};

	inline void _completeDirtTasks(u32 timeMs);

    core::aabbox3d<f32> Box, m_RenderBBox;

    KTree *m_KTree, *m_KTreeDirt;

    s32 m_MaxTrianglesPerNode;	

	IAnimatedMesh *m_AnimatedMesh, *m_PhysicalMesh;

	core::stringc m_AnimatedMeshFullFileName, m_PhysicalMeshFullFileName;

	core::array<SRenderGeometry> m_ViewedVBOMeshBuffers, m_ViewedDirtMeshBuffers;

	core::list<SDirtUpdateTask>	m_DirtTextureUpdateTasks;

	static core::queue<CKTreeSceneNode*> m_DirtTextureUpdateQueue;
	static s32 m_LastTimeDirtUpdate;

	core::array<vid::IRenderBuffer*> m_SolidRenderBuffers, m_TransparentRenderBuffers,
		m_DirtRenderBuffers, m_GlobalLightSolidRenderBuffers;
	core::array<const vid::SMaterial*> m_SolidMaterials, m_TransparentMaterials,
		m_DirtMaterials, m_GlobalLightSolidMaterials;

	struct SDynamicLightRenderData
	{
		core::array < vid::IRenderBuffer * >	RenderBuffers;	
		core::array < const vid::SMaterial * >	Materials;
		s32										EnabledLightIdx;
	};

	core::array < SDynamicLightRenderData > m_DynamicLightRenderData;

	vid::SMaterial		m_StubMaterial;
	vid::IRenderBuffer	*m_StubRenderBuffer;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // CKTreeSceneNodeHPP

