//|-------------------------------------------------------------------------
//| File:        CAnimatedMeshCal3d.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CAnimatedMeshCal3dHPP
#define CAnimatedMeshCal3dHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/IAnimatedMeshCal3d.h>
#include <io/IReadFile.h>
#include <vid/S3DVertex.h> 
#include <scn/SMesh.h>
#include <scn/ISceneManager.h>
#include <scn/mesh_buffer.h>

#include <cal3d/cal3d.h>

//----------------------------------------------------------------------------

#define MY_CAL3D_MAX_ANIMATIONS_NUMBER  255

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class CAnimatedMeshCal3d : public IAnimatedMeshCal3d
{

public:

    CAnimatedMeshCal3d();
    virtual ~CAnimatedMeshCal3d();

    virtual bool loadFromFile(io::IReadFile* file);

	////////////////////////////////////////////////////
	//       IAnimatedMeshMD2
	////////////////////////////////////////////////////

    virtual IMesh* getMesh();
	virtual IMesh* getMesh(f32 frame); 
	
    virtual void recalculateBoundingBox();

    virtual E_MESH_TYPE getAnimatedMeshType() const
	{ return EMT_CAL3D; }

    virtual u32 getOveralFramesCount()
	{ return m_Cal3dMesh.m_OveralFramesCount ? m_Cal3dMesh.m_OveralFramesCount : 1; }

	virtual void rescaleAnimatedMeshNormals(const core::vector3df &newScale);

	virtual bool executeCal3dAction(u32 anim, f32 delay=0.3f, bool loop=false);  
	
	virtual bool removeCal3dAction(f32 delay=0.3f);  

	virtual bool setCurrentAnimation(u32 idx, f32 delay=0.0f);

	virtual void startRegisteringAnimations();
	virtual bool registerAnimation(const c8 *filename, f32 speed, bool looped);
	virtual bool unregisterAnimation(u32 idx);
	virtual bool swapAnimations(u32 idx1, u32 idx2);
	virtual void endRegisteringAnimations();

	virtual f32 getAnimationTimeSec();

	virtual f32 getAnimationDurationSec();

	virtual s32 getAnimationFPS();

	virtual void setAnimationFPS(s32 fps);

	virtual bool getAnimationState(SAnimationState& ani_state);

	virtual void updateMesh(s32 delta_ms);

	virtual bool setAnimationState(const SAnimationState &animstate);

	virtual const core::array<SSkeletonBone>& getBones();

	virtual bool mapBone(s32 bone_idx);

	virtual core::matrix4 getMapedBoneTransformation(s32 bone_idx);

	virtual void uploadToRenderBuffers(IAnimatedMeshSceneNode *node);

private:  
	
	bool _setCal3dAnimation_BlendCycleAndUpdate(u32 idx, f32 delay);
	bool _setCal3dAnimation(u32 idx);

	bool m_ExecutingAction;

	int m_currentBlendedAnimationId;

	//! configuration file for the character
	core::stringc m_ConfigFile;

	//! the path where the media (textures etc.) are stored
	core::stringc m_MediaPath;

	//! core model for cal3d, 
	//! this should raelly be in a seperate coremodel server class
	CalCoreModel *m_Cal3dCoreModel;

	//! instance model for cal3d
	CalModel *m_Cal3dModel;

	//! indicate if animation is paused
	bool m_bPaused;

	//! model scale
	float m_Scale;

	//! remaining time for the current animation
	float m_leftAnimationTime;

	//! time the animation blending will take
	float m_BlendTime;

	//! level of detail
	u32 m_lodLevel;

	//! true if initialized
	bool m_Initialized;    

	//! load and parse the configuration file
	bool _parseModelConfiguration();

	// load all textures
	bool init();

	core::vector3df NormalScale;	

	s32 m_OveralVertexCount, m_OveralFaceCount;

	static f32		m_Cal3dBonesLines[1024][2][3];
	static CalIndex	m_Cal3dMeshFaces[50000][3];	

	struct SCal3dSpecialAnimation : public ISpecialAnimation
	{
		SCal3dSpecialAnimation() 
			: AnimationId(-1), Time(0.0f), Ended(false), Duration(0.0f) {}
		SCal3dSpecialAnimation(s32 id)
			: AnimationId(id), Time(0.0f), Ended(false), Duration(0.0f) {}
		virtual ~SCal3dSpecialAnimation() {}
		f32 Time;
		f32 Duration;
		s32 AnimationId;
		bool Ended;
	}; 

	template <class T>
	struct SCal3dMeshBuffer : public IMeshBuffer
	{
		SCal3dMeshBuffer() 
			: m_Material(0), m_Vertices(0), m_Indices(0), m_BoundingBox(0),
			m_VerticesCount(0), m_IndicesCount(0)
		{
		#if MY_DEBUG_MODE 
			setClassName(typeid(this).name());
		#endif
		}

		void setGeom(
			vid::SMaterial	*material,
			T				*vertices, u32	vert_cnt,
			u16				*indices, u32 ind_cnt)
		{
			m_Material		= material;
			m_Vertices		= vertices;
			m_VerticesCount	= vert_cnt;
			m_Indices		= indices;
			m_IndicesCount	= ind_cnt;
		}

		void setBB(core::aabbox3d <f32>	*bb)
		{
			m_BoundingBox	= bb;
		}

		virtual vid::SMaterial& getMaterial()
		{ return *m_Material; }

		virtual const vid::SMaterial& getMaterial() const
		{ return *m_Material; }

		virtual vid::E_VERTEX_TYPE getVertexType() const
		{ return T::Type; }

		virtual const vid::S3DBaseVertex& getVertex(u32 idx) const
		{ return *((vid::S3DBaseVertex*)(&m_Vertices[idx])); }

		virtual vid::S3DBaseVertex& getVertex(u32 idx)
		{ return *((vid::S3DBaseVertex*)(&m_Vertices[idx])); }

		virtual const void* getVertices() const
		{ return m_Vertices; } 

		virtual void* getVertices()
		{ return m_Vertices; }

		virtual s32 getVertexCount() const
		{ return m_VerticesCount; }

		virtual const core::vector3df& getVertexNormal(u32 idx) const
		{ return (m_Vertices[idx]).getNormal(); }

		virtual core::vector3df& getVertexNormal(u32 idx)
		{ return (m_Vertices[idx]).getNormal(); }

		virtual u32 getIndex(u32 idx) const
		{ return (u32)m_Indices[idx]; }

		virtual const void* getIndices() const
		{ return (void*)m_Indices; }

		virtual void* getIndices()
		{ return (void*)m_Indices; }

		virtual s32 getIndexCount() const
		{ return m_IndicesCount; }

		virtual const core::aabbox3d<f32>& getBoundingBox() const
		{ return *m_BoundingBox; }

		virtual core::aabbox3d<f32>& getBoundingBox() 
		{ return *m_BoundingBox; }

		virtual void recalculateBoundingBox() {}

		virtual void rescaleMeshBufferNormals(const core::vector3df &newScale) {}

		virtual void addIndex(s32 index_value) {}

		virtual void render() {}

		virtual void renderStatic()	{}

		virtual void render(vid::E_RENDER_BUFFER_TYPE) {}

		virtual E_INDEX_TYPE getIndexType()
		{ return EIT_16_BIT; }
		
		vid::SMaterial			*m_Material;     
		T						*m_Vertices;     
		u16						*m_Indices;     
		core::aabbox3d <f32>	*m_BoundingBox;
		u32						m_VerticesCount, m_IndicesCount;
	};

	template < class TVert, class TInd >
	struct SCal3dMesh
	{
		SCal3dMesh()
			: m_OveralFramesCount(0), m_CurrentFrame(0), m_VertexType(TVert::Type),
			m_IndexType(sizeof(TInd) == 2 ? EIT_16_BIT : EIT_32_BIT) {}

		virtual ~SCal3dMesh()
		{ freeData(); }

		void freeData()
		{
			for (u32 i = 0; i < m_Vertices.size(); i++)
				SAFE_DELETE_ARRAY(m_Vertices[i]);
			for (u32 i = 0; i < m_Indices.size(); i++)
				SAFE_DELETE_ARRAY(m_Indices[i]);
			for (u32 i = 0; i < m_FrameList.size(); i++)
				for (u32 j = 0; j < m_FrameList[i].size(); j++)
					SAFE_DELETE_ARRAY(m_FrameList[i][j]);
		}

		// geometry 
		core::array <TVert*> m_Vertices;
		core::array <TInd*> m_Indices;
		core::array <u32> m_VerticesCount, m_MaxVerticesCount;
		core::array <u32> m_IndicesCount, m_MaxIndicesCount;
		core::array <vid::SMaterial> m_Materials;

		// animation
		core::array < core::array<TVert*> > m_FrameList;
		core::array < core::array<core::aabbox3df> > m_BoxList;

		core::array <TVert*> m_LockedVertices;
		core::array <TInd*> m_LockedIndices;

		u32 m_OveralFramesCount, m_CurrentFrame;

		vid::E_VERTEX_TYPE m_VertexType;
		E_INDEX_TYPE m_IndexType;
	};

	SCal3dMesh <vid::S3DVertex1TCoords, u16> m_Cal3dMesh;

	SMesh m_InterfaceMesh;

	core::array <SSkeletonBone> m_SkeletonBones;

	s32 m_KeyFrameFPS;

	bool m_IndicesOK;

	core::array <core::array<core::matrix4> > m_MappedBoneTransformations;

	void _getAnimatedGeometry(
		core::array <vid::S3DVertex1TCoords*> *vertices,
		core::array <u16*> *indices = NULL);

	void _getInterpolatedGeometry(
		core::array <vid::S3DVertex1TCoords*> *vertices,
		core::array <u16*> *indices = NULL);

	void _getKeyFrameGeometry(
		core::array <vid::S3DVertex1TCoords*> *&vertices,
		core::array <u16*> *indices = NULL);

	void _clearCal3dAnimations();
	void _clearCal3dAnimation(u32 idx);

	void _updateBoundingBox();
};

//----------------------------------------------------------------------------
} // end namespace scene
} // end namespace my
//----------------------------------------------------------------------------

#endif

