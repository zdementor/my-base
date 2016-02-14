//|-------------------------------------------------------------------------
//| File:        CAnimatedMeshMD2.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CAnimatedMeshMD2HPP
#define CAnimatedMeshMD2HPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/IAnimatedMeshMD2.h>
#include <io/IReadFile.h>
#include <vid/S3DVertex.h>
#include <scn/SMesh.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class CAnimatedMeshMD2 : public IAnimatedMeshMD2
{
public:

    CAnimatedMeshMD2();
    virtual ~CAnimatedMeshMD2();

    bool loadFromFile(io::IReadFile* file);

	//////////////////////////////////////
	//      IAnimatedMeshMD2
	//////////////////////////////////////

    virtual IMesh* getMesh();
	virtual IMesh* getMesh(f32 frame);

    virtual void recalculateBoundingBox();

    virtual E_MESH_TYPE getAnimatedMeshType() const
	{ return EMT_MD2; }

    virtual u32 getOveralFramesCount()
	{ return m_OveralFramesCount; }

	virtual void rescaleAnimatedMeshNormals(const core::vector3df &newScale);

	virtual bool setCurrentAnimation(u32 idx, f32 delay=0.0f);
	
	virtual void startRegisteringAnimations();
	virtual void endRegisteringAnimations();

	virtual f32 getAnimationTimeSec();

	virtual f32 getAnimationDurationSec();

	virtual bool getAnimationState(SAnimationState& ani_state);

	virtual void updateMesh(s32 delta_ms);

	virtual bool setAnimationState(const SAnimationState &animstate);

	virtual void uploadToRenderBuffers(IAnimatedMeshSceneNode *node );

private:

	struct SFrameData
    {
        core::stringc name;
        s32 begin, end;
        s32 fps;
    };

	template <class T>
	struct SMD2MeshBuffer : public IMeshBuffer
	{
		SMD2MeshBuffer() 
			: Material(0), Vertices(0), VerticesCount(0),
		Indices(0), IndicesCount(0), BoundingBox(0)
		{
#if MY_DEBUG_MODE 
			setClassName(typeid(this).name());
#endif
		}

		void set(
			vid::SMaterial *	material,
			T *	vertices, u32 vert_cnt,
			u16 *	indices, u32 ind_cnt,
			core::aabbox3df *	bb )
		{
			Material		= material;
			Vertices		= vertices;
			VerticesCount	= vert_cnt;
			Indices			= indices;
			IndicesCount	= ind_cnt;
			BoundingBox		= bb;
		}

		virtual vid::SMaterial& getMaterial()
		{ return *Material; }

		virtual const vid::SMaterial& getMaterial() const
		{ return *Material; }

		virtual vid::E_VERTEX_TYPE getVertexType() const
		{ return T::Type; }

		virtual const vid::S3DBaseVertex& getVertex(u32 idx) const
		{ return *((vid::S3DBaseVertex*)(&Vertices[idx])); }

		virtual vid::S3DBaseVertex& getVertex(u32 idx)
		{ return *((vid::S3DBaseVertex*)(&Vertices[idx])); }

		virtual const void* getVertices() const
		{ return Vertices; } 

		virtual void* getVertices()
		{ return Vertices; }

		virtual s32 getVertexCount() const
		{ return VerticesCount; }

		virtual const core::vector3df& getVertexNormal(u32 idx) const
		{ return (Vertices[idx]).getNormal(); }

		virtual core::vector3df& getVertexNormal(u32 idx)
		{ return (Vertices[idx]).getNormal(); }

		virtual u32 getIndex(u32 idx) const
		{ return (u32)Indices[idx]; }

		virtual const void* getIndices() const
		{ return Indices; }

		virtual void* getIndices()
		{ return Indices; }

		virtual s32 getIndexCount() const
		{ return IndicesCount; }

		virtual const core::aabbox3d<f32>& getBoundingBox() const
		{ return *BoundingBox; }

		virtual core::aabbox3d<f32>& getBoundingBox() 
		{ return *BoundingBox; }

		virtual void recalculateBoundingBox() {}

		virtual void rescaleMeshBufferNormals(const core::vector3df &newScale) {}

		virtual void addIndex(s32 index_value) {}

		virtual void render() {}

		virtual void renderStatic() {}

		virtual void render(vid::E_RENDER_BUFFER_TYPE) {}

		virtual E_INDEX_TYPE getIndexType()
		{ return EIT_16_BIT; }

		vid::SMaterial *	Material;     
		T *		Vertices;
		u32		VerticesCount;
		u16 *	Indices;
		u32		IndicesCount;
		core::aabbox3d<f32> *	BoundingBox;
	};

	IMeshBuffer* m_InterfaceMeshBuffer;
	SMesh        m_InterfaceMesh;

	core::vector3df m_NormalScale;

	core::array < u16 > m_Indices;
	core::array < core::array < vid::S3DVertex1TCoords > > m_FrameList;
	core::array < vid::S3DVertex1TCoords > m_InterpolateBuffer;
	core::array < core::aabbox3df > m_BoxList;
	core::array< SFrameData > m_FrameData;
	vid::SMaterial m_Material;

	bool m_Looping;
	u32 m_OveralFramesCount, m_CurrentFrame, m_PreCurrentFrame;
	u32 m_BeginFrame, m_EndFrame;
	f32 m_CurrentFrameFloat;
	u32 m_VerticesCount;

	void _getMeshGeometry(
		vid::S3DVertex1TCoords * & vertices, u16 * indices = NULL );

    void _calculateNormals();
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

