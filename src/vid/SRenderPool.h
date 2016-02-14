//|-------------------------------------------------------------------------
//| File:        SRenderPool.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SRenderPoolHPP
#define SRenderPoolHPP

//--------------------------------------------------------------------------
namespace my {
namespace vid {
//--------------------------------------------------------------------------

enum E_RENDER_POOL_TYPE
{
	ERPT_RENDER_POOL_ZERO = 0,
	ERPT_RENDER_POOL,
	ERPT_RENDER_CACHE,

	E_RENDER_POOL_TYPE_COUNT,

	E_RENDER_POOL_TYPE_FORCE_32_BIT = 0xffffffff
};

//--------------------------------------------------------------------------

struct SRenderPool
{
	struct SRenderPoolExFinder;

	core::matrix4								Transform;
	const SLight								*Light0;
	u64											EnabledLights;
	s32											EnabledLightIdx;
	const core::array <const SMaterial *>		*Materials;
	const core::array <IRenderBuffer *>			*RenderBuffers;
	E_DRAW_PRIMITIVE_TYPE						DrawPrimitiveType;
	E_RENDER_POOL_TYPE							Type;
	E_RENDER_PASS								Pass;
	f32											Z;
	u32											Id;
	E_RENDER_MODE								Mode;
	const scn::ISceneNode						*Owner;

	SRenderPool()
		: Pass((E_RENDER_PASS)0), Z(-1.0f), Id(0),
	Light0(NULL), EnabledLights(0), EnabledLightIdx(-1),
	Materials(NULL), RenderBuffers(NULL),
	Type(ERPT_RENDER_POOL_ZERO), RenderPoolData(NULL), Mode(ERM_RENDER_STANDARD),
	DrawPrimitiveType((E_DRAW_PRIMITIVE_TYPE)0), // Don't care about it
	Owner(NULL)
	{}

	SRenderPool(const SRenderPool & other)
		: Type(ERPT_RENDER_POOL_ZERO), RenderPoolData(NULL)
	{ (*this).operator = (other); }

    SRenderPool(
		E_RENDER_PASS							pass,
		f32										z,
		u32										id,
		const core::matrix4						&transf,
		const SLight							*light0,
		u64										enabled_lights,
		const core::array <const SMaterial *>	&materials,
		const core::array <IRenderBuffer *>		&render_buffers,
		E_RENDER_MODE							mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode					*owner = NULL
		) : Pass(pass), Transform(transf), Z(z), Id(id),
	Light0(light0), EnabledLights(enabled_lights), EnabledLightIdx(-1),
	Type(ERPT_RENDER_POOL), RenderPoolData(NULL), Mode(mode),
	DrawPrimitiveType((E_DRAW_PRIMITIVE_TYPE)0), // Don't care about it
	Owner(owner)
	{
		if ( materials.size() == render_buffers.size() )
		{
			Materials = &materials;
			RenderBuffers = &render_buffers;			
		}
	}

	SRenderPool(
		E_RENDER_PASS							pass,
		f32										z,
		u32										id,
		const core::matrix4						&transf,
		s32										enabled_light_idx,
		const core::array <const SMaterial *>	&materials,
		const core::array <IRenderBuffer *>		&render_buffers,
		E_RENDER_MODE							mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode					*owner = NULL
		) : Pass(pass), Transform(transf), Z(z), Id(id),
	Light0(NULL), EnabledLights(0), EnabledLightIdx(enabled_light_idx),
	Type(ERPT_RENDER_POOL), RenderPoolData(NULL), Mode(mode),
	DrawPrimitiveType((E_DRAW_PRIMITIVE_TYPE)0), // Don't care about it
	Owner(NULL)
	{
		if ( materials.size() == render_buffers.size() )
		{
			Materials = &materials;
			RenderBuffers = &render_buffers;			
		}
	}

	SRenderPool(
		E_RENDER_PASS				pass,
		f32							z,
		u32							id,
		const core::matrix4			&transf,
		const SLight				*light0,
		u64							enabled_lights,
		const SMaterial				&material,
		IRenderBuffer				&render_buffer,
		E_RENDER_MODE				mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode		*owner = NULL
		) : Pass(pass), Transform(transf), Z(z), Id(id),
	Light0(light0), EnabledLights(enabled_lights), EnabledLightIdx(-1),
	Type(ERPT_RENDER_POOL), RenderPoolData(NULL), Mode(mode),
	DrawPrimitiveType((E_DRAW_PRIMITIVE_TYPE)0), // Don't care about it
	Owner(owner)
	{
		OwnMaterials.push_back(&material);
		OwnRenderBuffers.push_back(&render_buffer);

		Materials = &OwnMaterials;
		RenderBuffers = &OwnRenderBuffers;
	}

	SRenderPool(
		E_RENDER_PASS			pass,
		f32						z,
		u32						id,
		const core::matrix4		&transf,
		s32						enabled_light_idx,
		const SMaterial			&material,
		IRenderBuffer			&render_buffer,
		E_RENDER_MODE			mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode	*owner = NULL
		) : Pass(pass), Transform(transf), Z(z), Id(id),
	Light0(NULL), EnabledLights(0), EnabledLightIdx(enabled_light_idx),
	Type(ERPT_RENDER_POOL), RenderPoolData(NULL), Mode(mode),
	DrawPrimitiveType((E_DRAW_PRIMITIVE_TYPE)0), // Don't care about it
	Owner(owner)
	{
		OwnMaterials.push_back(&material);
		OwnRenderBuffers.push_back(&render_buffer);

		Materials = &OwnMaterials;
		RenderBuffers = &OwnRenderBuffers;
	}

	SRenderPool(
		E_RENDER_PASS				pass,
		f32							z,
		u32							id,
		const core::matrix4			&transf,
		const SLight				*light0,
		u64							enabled_lights,
		s32							enabled_light_idx,
		const SMaterial				&material,
		E_VERTEX_TYPE				vtype,
		E_INDEX_TYPE				itype,
		E_DRAW_PRIMITIVE_TYPE		dpt,
		E_RENDER_MODE				mode = ERM_RENDER_STANDARD,
		const scn::ISceneNode		*owner = NULL
		) : Pass(pass), Z(z), Id(id), Transform(transf),
	Light0(light0), EnabledLights(enabled_lights), EnabledLightIdx(enabled_light_idx),
	DrawPrimitiveType(dpt), Type(ERPT_RENDER_CACHE), Mode(mode),
	Owner(owner)
	{
		RenderPoolData = new SRenderPoolData(material, vtype, itype);

		OwnMaterials.push_back(RenderPoolData->getMaterial());
		OwnRenderBuffers.push_back(NULL);

		Materials = &OwnMaterials;
		RenderBuffers = &OwnRenderBuffers;
	}

	virtual ~SRenderPool()
	{
		if ( Type == ERPT_RENDER_CACHE )
		{
			if (OwnRenderBuffers.size())
				SAFE_DROP(OwnRenderBuffers[0]);

			if (RenderPoolData)
				RenderPoolData->decRef();
		}
	}

	void update(u32 time_ms)
	{
		if (Type != ERPT_RENDER_CACHE)
			return;
		RenderPoolData->update(time_ms);
	}

	bool operator == (const SRenderPool::SRenderPoolExFinder & other) const
	{ return !(this->operator != ( other )); }

	bool operator != (const SRenderPool::SRenderPoolExFinder & other) const;

	bool operator < (const SRenderPool & other) const;

	void operator = (const SRenderPool & other);

	void addGeometry(void *vptr, u32 vcnt, E_VERTEX_TYPE vtype,
		void *iptr, u32 icnt, E_INDEX_TYPE itype)
	{
		if (Type != ERPT_RENDER_CACHE)
			return;

		RenderPoolData->addGeometry(vptr, vcnt, vtype, iptr, icnt, itype);

		IRenderBuffer * & rb = OwnRenderBuffers[0];

		if (!rb || RenderPoolData->getVerticesCount() > rb->getVertices()->getMaxSize() ||
			RenderPoolData->getIndicesCount() > rb->getIndices()->getMaxSize())
		{
			SAFE_DROP(rb);
			rb = VIDEO_DRIVER.createDynamicRenderBuffer(
				RenderPoolData->getVertices(),
				RenderPoolData->getVerticesType(),
				RenderPoolData->getVerticesCount(),
				RenderPoolData->getIndices(),
				RenderPoolData->getIndicesType(),
				RenderPoolData->getIndicesCount(),
				DrawPrimitiveType);
		}

		rb->getVertices()->setSize(RenderPoolData->getVerticesCount());
		rb->getIndices()->setSize(RenderPoolData->getIndicesCount());
	}

	SGeometryBufferEntry reserveGeometry(u32 vcnt, u32 icnt)
	{
		SGeometryBufferEntry geom(NULL, NULL, 0);

		if (Type != ERPT_RENDER_CACHE)
			return geom;

		geom = RenderPoolData->reserveGeometry(vcnt, icnt);

		IRenderBuffer *&rb = OwnRenderBuffers[0];

		if (!rb || RenderPoolData->getVerticesCount() > rb->getVertices()->getMaxSize() ||
			RenderPoolData->getIndicesCount() > rb->getIndices()->getMaxSize())
		{
			SAFE_DROP(rb);
			rb = VIDEO_DRIVER.createDynamicRenderBuffer(
				RenderPoolData->getVertices(),
				RenderPoolData->getVerticesType(),
				RenderPoolData->getVerticesCount(),
				RenderPoolData->getIndices(),
				RenderPoolData->getIndicesType(),
				RenderPoolData->getIndicesCount(),
				DrawPrimitiveType);
		}

		rb->getVertices()->setSize(RenderPoolData->getVerticesCount());
		rb->getIndices()->setSize(RenderPoolData->getIndicesCount());

		return geom;
	}

	void setIdentity()
	{
		if (RenderPoolData)
			RenderPoolData->setIdentity();

		Z = -1.0f;
		Transform.makeIdentity();
		Light0 = NULL;
		EnabledLights = 0;
		EnabledLightIdx = -1;
		Owner = NULL;
	}

	struct SRenderPoolExFinder
	{
		SRenderPoolExFinder() {}

		void set(
			f32								z,
			const core::matrix4				*transf,
			const SLight					*light0,
			u64								enabled_lights,
			s32								enabled_light_idx,
			const SMaterial					*material,
			E_VERTEX_TYPE					vtype,
			E_INDEX_TYPE					itype,
			E_DRAW_PRIMITIVE_TYPE			dpt,
			E_RENDER_BUFFER_TYPE			type,
			E_RENDER_MODE					mode = ERM_RENDER_STANDARD,
			const scn::ISceneNode			*owner = NULL)
		{
			Z = z;
			Transform = transf;
			Light0 = light0;
			EnabledLights = enabled_lights;
			EnabledLightIdx = enabled_light_idx;
			Material = material;
			VerticesType = vtype;
			IndicesType = itype;
			DrawPrimitiveType = dpt;
			RenderBufferType = type;
			Mode = mode;
			Owner = owner;
		}

		const core::matrix4				*Transform;
		const SLight					*Light0;
		u64								EnabledLights;
		s32								EnabledLightIdx;
		const SMaterial					*Material;
		E_VERTEX_TYPE					VerticesType;
		E_INDEX_TYPE					IndicesType;
		E_DRAW_PRIMITIVE_TYPE			DrawPrimitiveType;
		f32								Z;
		E_RENDER_BUFFER_TYPE			RenderBufferType;
		E_RENDER_MODE					Mode;
		const scn::ISceneNode			*Owner;
	};

	class SRenderPoolData
	{
	public:

		SRenderPoolData(const SMaterial & m, E_VERTEX_TYPE vtype, E_INDEX_TYPE itype)
			: m_Material(m), m_Vertices(NULL), m_VerticesCount(0), m_VertexType(vtype),
		m_Indices(NULL), m_IndicesCount(0), m_IndexType(itype),	m_RefCnt(1),
		m_VerticesMaxCount(0), m_IndicesMaxCount(0)
		{
			m_VertexSize = VertexSize[m_VertexType];
			m_IndexSize = IndexSize[m_IndexType];
		}

		void incRef()
		{ m_RefCnt++; }

		void decRef()
		{
			if (--m_RefCnt == 0)
				delete this;
		}

		u32 getVertexSize() { return m_VertexSize; }
		u32 getIndexSize() { return m_IndexSize; }

		void setIdentity()
		{
			m_VerticesCount	= 0;
			m_IndicesCount	= 0;
		}

		bool addGeometry(void *vptr, u32 vcnt, E_VERTEX_TYPE vtype,
			void *iptr, u32 icnt, E_INDEX_TYPE itype)
		{
			bool reallocated = false;

			if (m_VertexType != vtype || m_IndexType != itype)
				return reallocated;
			
			reallocated =
				_realloc(m_VerticesCount + vcnt, m_IndicesCount + icnt);

			// just copy vertices array

			memcpy(((u8*)m_Vertices) + m_VerticesCount * m_VertexSize,
				vptr, vcnt * m_VertexSize);

			// copy and remap indices array

			if (m_IndexType == EIT_32_BIT)
			{
				u32 *indices_dst = (u32*)m_Indices;
				u32 *indices_src = (u32*)iptr;
				for (u32 i = 0, idx = m_IndicesCount; i < icnt; i++, idx++)
					indices_dst[idx] = m_VerticesCount + indices_src[i];
			}
			else
			// EIT_16_BIT
			{
				u16 *indices_dst = (u16*)m_Indices;
				u16 *indices_src = (u16*)iptr;
				for (u32 i = 0, idx = m_IndicesCount; i < icnt; i++, idx++)
					indices_dst[idx] = m_VerticesCount + indices_src[i];
			}

			m_VerticesCount += vcnt;
			m_IndicesCount += icnt;

			return reallocated;
		}

		SGeometryBufferEntry reserveGeometry(u32 vcnt, u32 icnt)
		{
			bool reallocated = _realloc(
				m_VerticesCount + vcnt, m_IndicesCount + icnt);

			SGeometryBufferEntry geom(
				((u8*)m_Vertices) + m_VerticesCount * m_VertexSize,
				((u8*)m_Indices) + m_IndicesCount * m_IndexSize,
				m_VerticesCount);

			m_VerticesCount += vcnt;
			m_IndicesCount += icnt;

			return geom;
		}

		const void* getVertices()
		{ return m_Vertices; }

		u32 getVerticesCount()
		{ return m_VerticesCount; }

		E_VERTEX_TYPE getVerticesType()
		{ return m_VertexType; }

		const void* getIndices()
		{ return m_Indices; }

		u32 getIndicesCount()
		{ return m_IndicesCount; }

		E_INDEX_TYPE getIndicesType()
		{ return m_IndexType; }

		const SMaterial* getMaterial()
		{ return &m_Material; }

		void update(u32 time_ms)
		{ m_Material.update (time_ms); }

	private:

		bool _realloc(u32 vert_cnt, u32 ind_cnt)
		{
			bool reallocated = false;
			if (vert_cnt > m_VerticesMaxCount)
			{
				while (m_VerticesMaxCount < vert_cnt)
					m_VerticesMaxCount = m_VerticesMaxCount ?
						(m_VerticesMaxCount * 2) : vert_cnt;
				u32 vsize_new = m_VerticesMaxCount * m_VertexSize;
				void * vnew = new c8 [vsize_new];
				memcpy(vnew, m_Vertices, m_VerticesCount * m_VertexSize);
				delete []m_Vertices;
				m_Vertices = vnew;

				reallocated = true;
			}
			if (ind_cnt > m_IndicesMaxCount)
			{
				while (m_IndicesMaxCount < ind_cnt)
					m_IndicesMaxCount = m_IndicesMaxCount ?
						(m_IndicesMaxCount * 2) : ind_cnt;
				u32 isize_new = m_IndicesMaxCount * m_IndexSize;
				void * inew = new c8 [isize_new];
				memcpy(inew, m_Indices, m_IndicesCount * m_IndexSize);
				delete []m_Indices;
				m_Indices = inew;

				reallocated = true;
			}
			return reallocated;
		}

		virtual ~SRenderPoolData()
		{
			SAFE_DELETE_ARRAY(m_Vertices);
			SAFE_DELETE_ARRAY(m_Indices);	
		}

		void *			m_Vertices;
		u32				m_VerticesCount, m_VerticesMaxCount;
		E_VERTEX_TYPE	m_VertexType;
		u32				m_VertexSize;
		void *			m_Indices;
		u32				m_IndicesCount, m_IndicesMaxCount;
		E_INDEX_TYPE	m_IndexType;
		u32				m_IndexSize;
		s32				m_RefCnt;
		SMaterial		m_Material;
	}
	*RenderPoolData;

private:

	core::array < const SMaterial * >	OwnMaterials;
	core::array < IRenderBuffer * >		OwnRenderBuffers;
};
//--------------------------------------------------------------------------

MY_FORCEINLINE bool SRenderPool::operator != (
	const SRenderPool::SRenderPoolExFinder & other) const
{
//	if ((*RenderBuffers)[0]->getType() != other.RenderBufferType)
//		return true;

	if (Mode != other.Mode ||
		Owner != other.Owner ||		
		Z != other.Z ||
		EnabledLightIdx != other.EnabledLightIdx ||
		Transform != *other.Transform ||
		*(*Materials)[0] != *other.Material	||
		RenderPoolData->getVerticesType() != other.VerticesType ||
		RenderPoolData->getIndicesType() != other.IndicesType)
		return true;

	if (EnabledLights && other.EnabledLights)
	{
		if (EnabledLights != other.EnabledLights)
			return true;
		if (Light0 && other.Light0 &&
				*Light0 != *other.Light0)
			return true;
	}
	else
	if ((!EnabledLights && other.EnabledLights) ||
		(EnabledLights && !other.EnabledLights))
		return true;

	return false;
}

//--------------------------------------------------------------------------

MY_FORCEINLINE bool SRenderPool::operator < (const SRenderPool & other) const
{
	if (Pass == ERP_3D_LIGHTING_PASS)
	{
		if (EnabledLightIdx != other.EnabledLightIdx)
			return EnabledLightIdx < other.EnabledLightIdx;

		if (EnabledLights && other.EnabledLights)
		{
			if (EnabledLights != other.EnabledLights)
				return EnabledLights < other.EnabledLights;
			if (Light0 && other.Light0 &&
					*Light0 != *other.Light0)
				return *Light0 < *other.Light0;
		}
	}
	else
		// Sorting render buffers according to their types (dynamic/static),
		// dynamic buffers must be rendered before static,
		// it gain ~10% performance boost.
		if ((*RenderBuffers)[0]->getType() != (*other.RenderBuffers)[0]->getType())
			return (*RenderBuffers)[0]->getType() < (*other.RenderBuffers)[0]->getType();

	if (Owner != other.Owner)
		return Owner > other.Owner;

	if (Z != other.Z)
		return Z > other.Z;

	if (Mode != other.Mode)
		return Mode < other.Mode;

	if (Id != other.Id)
		return Id < other.Id;

	if (Pass == ERP_3D_LIGHTING_PASS)
		return !EnabledLights && other.EnabledLights;

	const SMaterial &mat_self = *(*Materials)[Materials->size()-1];
	const SMaterial &mat_other = *(*other.Materials)[0];

	return mat_self < mat_other;
}

//--------------------------------------------------------------------------

MY_FORCEINLINE void SRenderPool::operator = (const SRenderPool & other)
{
	if ((ERPT_RENDER_POOL_ZERO != Type && other.Type != Type) ||
		this == &other)
		return;
	
	Type			= other.Type;
	Pass			= other.Pass;

	Owner			= other.Owner;
	Z				= other.Z;
	Id				= other.Id;
	Transform		= other.Transform;
	Light0			= other.Light0;
	EnabledLights	= other.EnabledLights;
	EnabledLightIdx	= other.EnabledLightIdx;
	Mode			= other.Mode;

	if (Type == ERPT_RENDER_CACHE)
	{
		if (RenderPoolData)
			RenderPoolData->decRef();
		RenderPoolData = other.RenderPoolData;
		if (RenderPoolData)
			RenderPoolData->incRef();

		DrawPrimitiveType = other.DrawPrimitiveType;

		Materials = &OwnMaterials;
		RenderBuffers = &OwnRenderBuffers;

		OwnMaterials = other.OwnMaterials;

		if (OwnRenderBuffers.size())
			SAFE_DROP(OwnRenderBuffers[0]);
		OwnRenderBuffers = other.OwnRenderBuffers;
		SAFE_GRAB(OwnRenderBuffers[0]);
		
	}
	else if (Type == ERPT_RENDER_POOL)
	{
		OwnMaterials		= other.OwnMaterials;
		OwnRenderBuffers	= other.OwnRenderBuffers;

		Materials = (other.Materials == &other.OwnMaterials) ?
			&OwnMaterials : other.Materials;

		RenderBuffers = (other.RenderBuffers == &other.OwnRenderBuffers) ?
			&OwnRenderBuffers : other.RenderBuffers;
	}
}

//--------------------------------------------------------------------------
} // end namespace vid
} // end namespace vid
//--------------------------------------------------------------------------

#endif // #ifndef SRenderPoolHPP