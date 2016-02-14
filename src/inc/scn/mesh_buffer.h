//|-------------------------------------------------------------------------
//| File:        mesh_buffer.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef mesh_bufferHPP
#define mesh_bufferHPP
//--------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/IMeshBuffer.h>
#include <vid/IVideoDriver.h>
#include <io/ILogger.h>

//--------------------------------------------------------------------------
namespace my {
namespace scn {
//--------------------------------------------------------------------------

template <class T>
class splited_face
{
public:

	//! constructor 1
	splited_face() 
	{
		for (s32 i=0; i<3; i++)
		{
			Indices[i]=i;
			Additional[i]=false;
		}
	}

	//! constructor 2
    splited_face(s32 idx_a, s32 idx_b, s32 idx_c)
	{
		Indices[0]=idx_a;
		Indices[1]=idx_b;
		Indices[2]=idx_c; 

		for (s32 i=0; i<3; i++)
		{
			Additional[i]=false;
		}
	}

	//! constructor 3
    splited_face(const splited_face& other)
	{
		set(other);
	}

    //! operator =
    inline splited_face& operator=(const splited_face& other)
    {
		set(other);

		return *this;
    } 

	inline void set(const splited_face& other)
	{
		for (s32 i=0; i<3; i++)
		{
			Indices[i]   =other.Indices[i];
			Additional[i]=other.Additional[i]; 
		}
	}

	s32  Indices[3]; 
	bool Additional[3];
};

//--------------------------------------------------------------------------

//!  Mesh Buffer template
template <typename T, typename Ti=u16>
class mesh_buffer : public IMeshBuffer
{
public:
    //! constructor
	mesh_buffer()
	{
	#if MY_DEBUG_MODE  
		setClassName(typeid(this).name());
	#endif
    }

    //! destructor
    virtual ~mesh_buffer() {} 

    //! returns the material of this meshbuffer
    virtual const vid::SMaterial& getMaterial() const
    {
        return Material;
    }

    //! returns the material of this meshbuffer
    vid::SMaterial& getMaterial()
    {
        return Material;
    } 

    //! returns which type of vertex data is stored.
    virtual vid::E_VERTEX_TYPE getVertexType() const
    {
        return T::Type;
    }

	//! adding new vertex to the mesh buffer, return true on success
	virtual bool addVertex(const vid::S3DBaseVertex* vertex, vid::E_VERTEX_TYPE type)
	{
		if (vertex && getVertexType() == type)
		{
			Vertices.push_back(*((const T*)vertex));
			return true;
		}
		return false;
	}

	//! adding new vertex to the mesh buffer, return true on success
	virtual bool addVertex(const vid::S3DBaseVertex& vertex, vid::E_VERTEX_TYPE type)
	{
		if (getVertexType() == type)
		{
			Vertices.push_back(*((T*)&vertex));
			return true;
		}
		return false;
	}

	//! return vertex by its index
	virtual const vid::S3DBaseVertex& getVertex(u32 idx) const
	{ return *((vid::S3DBaseVertex*)(&Vertices[idx])); }

	virtual vid::S3DBaseVertex& getVertex(u32 idx)
	{ return *((vid::S3DBaseVertex*)(&Vertices[idx])); }

    //! returns pointer to vertices
    virtual const void* getVertices() const
    {
        return Vertices.const_pointer();
    } 

    //! returns pointer to vertices
    virtual void* getVertices()
    {
        return Vertices.pointer();
    }

    //! returns amount of vertices
    virtual s32 getVertexCount() const
    {
        return Vertices.size();
    }

	virtual const core::vector3df& getVertexNormal(u32 idx) const
	{ return (Vertices[idx]).getNormal(); }

	virtual core::vector3df& getVertexNormal(u32 idx)
	{ return (Vertices[idx]).getNormal(); }

	//! returns index value
	virtual u32 getIndex(u32 idx) const
	{
		return (u32)Indices[idx];
	}

    //! returns pointer to Indices
    virtual const void* getIndices() const
    {
        return (void*)Indices.const_pointer();
    }

    //! returns pointer to Indices
    virtual void* getIndices()
    {
        return (void*)Indices.pointer();
    }

    //! returns amount of indices
    virtual s32 getIndexCount() const
    {
        return Indices.size();
    }

    //! returns an axis aligned bounding box
    virtual const core::aabbox3d<f32>& getBoundingBox() const
    {
        return BoundingBox;
    }

    //! returns an axis aligned bounding box
    virtual core::aabbox3d<f32>& getBoundingBox()
    {
        return BoundingBox;
    }

    //! recalculates the bounding box. should be called if the mesh changed.
    void recalculateBoundingBox()
    {
        if (Vertices.empty())
		{
			BoundingBox.reset(0,0,0);
		}
        else
        {
            BoundingBox.reset(Vertices[0].Pos);

            for (u32 i=1; i<Vertices.size(); ++i)
			{
                BoundingBox.addInternalPoint(Vertices[i].Pos);
			}
        }
    }

	//! rescale normals
	virtual void rescaleMeshBufferNormals(const core::vector3df &newScale)
	{
		mesh_buffer_manipulator::rescale_normals((IMeshBuffer*)this, newScale);
	}

	//! reserving memory for vertices
	virtual bool setVerticesUsed(u32 used) 
	{ 
		Vertices.set_used(used);

		return true; 
	}

	//! adding vertex to mesh buffer, return true on success
	virtual bool addVertex(const T& vertex)
	{
		Vertices.push_back(vertex);

		return true;
	}

	//! reserving memory for indices
	virtual bool setIndicesUsed(u32 used)
	{
		Indices.set_used(used);

		return true;
	}

	//! adding index to mesh buffer
	virtual void addIndex(u32 index_value)
	{
		Indices.push_back((Ti)index_value);
	}

	virtual bool calculateTangents()
	{
		mesh_buffer_manipulator::calculate_tangents((IMeshBuffer*)this);
		return true;
	}

	//! split face into back and front faces
    static bool splitFaceByPlane(
		core::plane3df &plane, 
		core::face3d < T > &face_to_split,
		core::array < splited_face < T > > &back_verts,
		core::array < splited_face < T > > &front_verts,
		core::array < T > &added_vertices
		)
	{
		core::vector3df ab, bc, ac;

		core::E_INTERSECTION_RELATION_3D a_rel = plane.classifyPointRelation(face_to_split.A.Pos);
		core::E_INTERSECTION_RELATION_3D b_rel = plane.classifyPointRelation(face_to_split.B.Pos);
		core::E_INTERSECTION_RELATION_3D c_rel = plane.classifyPointRelation(face_to_split.C.Pos);

		if (a_rel==b_rel && b_rel==c_rel)
		{
			if (a_rel==core::EIREL3D_FRONT)
				front_verts.push_back( splited_face<T>() );
			else
				back_verts.push_back( splited_face<T>() );

			return true;
		}

		s32 planar_rel = 
			((a_rel==core::EIREL3D_PLANAR)?1:0) + ((b_rel==core::EIREL3D_PLANAR)?1:0) + 
			((c_rel==core::EIREL3D_PLANAR)?1:0);

		bool ab_intersect = 
			plane.getIntersectionWithLimitedLine(face_to_split.A.Pos, face_to_split.B.Pos, ab);
		bool bc_intersect = 
			plane.getIntersectionWithLimitedLine(face_to_split.B.Pos, face_to_split.C.Pos, bc);
		bool ac_intersect = 
			plane.getIntersectionWithLimitedLine(face_to_split.A.Pos, face_to_split.C.Pos, ac);
		
		T AB, BC, AC;

		if (ab_intersect)
		{			
			f32 k = 
				core::vector3df(face_to_split.A.Pos-ab).getLengthSQ() /
				core::vector3df(face_to_split.A.Pos - face_to_split.B.Pos).getLengthSQ();
			k = core::math::Sqrt(k);

			AB = face_to_split.A.getInterpolated(face_to_split.B, k, false);
			AB.Pos = ab;
		}

		if (bc_intersect)
		{	
			f32 k =
				core::vector3df(face_to_split.B.Pos - bc).getLengthSQ() /
				core::vector3df(face_to_split.B.Pos - face_to_split.C.Pos).getLengthSQ();
			k = core::math::Sqrt(k);
			
			BC = face_to_split.B.getInterpolated(face_to_split.C, k, false);
			BC.Pos = bc;
		}

		if (ac_intersect)
		{	
			f32 k = 
				core::vector3df(face_to_split.A.Pos - ac).getLengthSQ() /
				core::vector3df(face_to_split.A.Pos - face_to_split.C.Pos).getLengthSQ();
			k = core::math::Sqrt(k);

			AC = face_to_split.A.getInterpolated(face_to_split.C, k, false);
			AC.Pos = ac;
		}	

		if (planar_rel==1)
		{	
			if (a_rel == core::EIREL3D_PLANAR)
			{
				if (b_rel==c_rel)
				// this variant
				//      /  
				//   A o---------o C
				//    / \       /
				//   /   \     /
				//        \   /   
				//         \ /     
				//          o B  
				{
					if (b_rel == core::EIREL3D_FRONT)
					{
						front_verts.push_back( splited_face<T>() );
					}
					else
					{
						back_verts.push_back( splited_face<T>() );
					}
				}
				else
				// this variant
				//    \    
				//   A o\--------o C (2)
				//  (0) \ \     /
				//       \  \  /
				//        \   . bc (3)
				//         \ / \   
				//          o B (1)
				{
					if (c_rel == core::EIREL3D_FRONT)
					{
						front_verts.push_back( splited_face<T>(2, 0, 3 ) );
						back_verts.push_back ( splited_face<T>(1, 3, 0 ) );
					}
					else
					{
						back_verts.push_back ( splited_face<T>(2, 0, 3 ) );
						front_verts.push_back( splited_face<T>(1, 3, 0 ) );						
					}	

					added_vertices.push_back(BC);
				}
			}
			else
			if (b_rel == core::EIREL3D_PLANAR)
			{
				if (a_rel==c_rel)
				// this variant
				//          
				//   A o---------o C
				//      \       /
				//       \     /
				//        \   /   
				//         \ /B    
				//     -----o-----
				{
					if (a_rel == core::EIREL3D_FRONT)
					{
						front_verts.push_back( splited_face<T>() );
					}
					else
					{
						back_verts.push_back( splited_face<T>() );
					}	
				}
				else
				// this variant
				//          |ac (3)
				//   A o----.----o C (2)
				//  (0) \   |   /
				//       \  |  /    
				//        \ | /
				//         \|/
				//          o B (1)
				{
					if (a_rel == core::EIREL3D_FRONT)
					{
						front_verts.push_back( splited_face<T> ( 0, 1, 3 ) );
						back_verts.push_back ( splited_face<T> ( 2, 3, 1 ) );
					}
					else
					{
						back_verts.push_back ( splited_face<T>( 0, 1, 3 ) );
						front_verts.push_back( splited_face<T>( 2, 3, 1 ) );
					}

					added_vertices.push_back(AC);
				}
			}
			else
			{
				if (a_rel==b_rel)
				// this variant         
				//              \       
				//   A o---------o C    
				//      \       / \     
				//       \     /   \    
				//        \   /         
				//         \ /          
				//          o B         
				{
					if (a_rel == core::EIREL3D_FRONT)
					{
						front_verts.push_back( splited_face<T>() );
					}
					else
					{
						back_verts.push_back( splited_face<T>() );
					}	
				}
				else
				// this variant
				//                /   
				//   A o--------/o C  (2)
				//  (0) \     / /  
				//       \  /  /
				// (3) ab .   /
				//      /  \ /     
				//          o B (1)
				{
					if (b_rel == core::EIREL3D_FRONT)
					{
						front_verts.push_back( splited_face<T>( 1, 2, 3 ) );
						back_verts.push_back ( splited_face<T>( 0, 3, 2 ) );
					}
					else
					{
						back_verts.push_back ( splited_face<T>( 1, 2, 3 ) );
						front_verts.push_back( splited_face<T>( 0, 3, 2 ) );
					}

					added_vertices.push_back(AB);
				}
			}

			return true;
		}
		else if (planar_rel==2)
		// one of this variants:              /  \             
		//                                   /    \            
		// ----o---------o----    o---------o      o---------o   
		//    A \       / C      A \       / C    A \       / C
		//       \     /            \     /          \     /   
		//        \   /              \   /            \   /    
		//         \ /B               \ /B             \ /B    
		//          o                  o                o      
        //                            /                  \     
		{
			if (a_rel==core::EIREL3D_FRONT || b_rel==core::EIREL3D_FRONT || c_rel==core::EIREL3D_FRONT)			
			{
				front_verts.push_back( splited_face<T>() );				
			}
			else
			{
				back_verts.push_back( splited_face<T>() );
			}
			return true;
		}
		else if (planar_rel==3)
		{
			front_verts.push_back( splited_face<T>() );

			return true;
		}

		if (ab_intersect && bc_intersect && !ac_intersect)
		// this variant
		// A o---------o C (2)
		//(0) \       /
		//  ---.-----.----
		//   ab \   /  bc (3)
		//  (4)  \ /
		//        o B (1)
		{
			if (b_rel==core::EIREL3D_FRONT)
			{
				front_verts.push_back( splited_face<T>( 1, 3, 4 ) );				
				back_verts.push_back ( splited_face<T>( 2, 4, 3 ) );
				back_verts.push_back ( splited_face<T>( 2, 0, 4 ) );			
			}
			else
			{
				back_verts.push_back ( splited_face<T>( 1, 3, 4 ) );				
				front_verts.push_back( splited_face<T>( 2, 4, 3 ) );
				front_verts.push_back( splited_face<T>( 2, 0, 4 ) );				
			}

			added_vertices.push_back(BC);
			added_vertices.push_back(AB);

			return true;
		}
		else
		if (!ab_intersect && bc_intersect && ac_intersect)
		// this variant
		//        ac | (3)
		// A o-------.-o C (2)
		//(0) \      |/
		//     \     . bc (4)
		//      \   /|
		//       \ / |
		//        o B (1)
		{
			if (c_rel==core::EIREL3D_FRONT)
			{
				front_verts.push_back( splited_face<T>( 2, 3, 4 ) );				
				back_verts.push_back ( splited_face<T>( 0, 4, 3 ) );
				back_verts.push_back ( splited_face<T>( 0, 1, 4 ) );
			}
			else
			{
				back_verts.push_back ( splited_face<T>( 2, 3, 4 ) );				
				front_verts.push_back( splited_face<T>( 0, 4, 3) );
				front_verts.push_back( splited_face<T>( 0, 1, 4 ) );
			}

			added_vertices.push_back(AC);
			added_vertices.push_back(BC);

			return true;
		}
		else
		if (ab_intersect && !bc_intersect  && ac_intersect)
		// this variant
		//     | ac (4)
		// A o-.-------o C (2)
		//(0) \| (3)  / 
		//     . ab  /
		//     |\   /
		//     | \ /
		//        o B (1)
		{
			if (a_rel==core::EIREL3D_FRONT)
			{
				front_verts.push_back( splited_face<T>( 0, 3, 4 ) );				
				back_verts.push_back ( splited_face<T>( 1, 4, 3 ) );
				back_verts.push_back ( splited_face<T>( 1, 2, 4 ) );
			}
			else
			{
				back_verts.push_back ( splited_face<T>( 0, 3, 4   ) );				
				front_verts.push_back( splited_face<T>( 1, 4, 3  ) );
				front_verts.push_back( splited_face<T>( 1, 2, 4  ) );
			}	

			added_vertices.push_back(AB);
			added_vertices.push_back(AC);

			return true;
		}

		return false;
	}

	virtual bool splitByPlane(
		core::plane3df div_plane, IMeshBuffer *front_mb, IMeshBuffer *back_mb
		)
	{
		if (getVertexType()!=front_mb->getVertexType() ||
			getVertexType()!=back_mb->getVertexType()
			) return false;

		s32 vert_cnt = getVertexCount();

		core::array< T > added_verts;
		core::array< splited_face<T> > front_faces, back_faces;
		core::array< splited_face<T> > front_faces_single, back_faces_single;

		u16 face_indices[3];

		// splitting faces into front and back arrays

		for (s32 i=0; i<getIndexCount(); i+=3)
		{	
			// fetch vertices

			for (s32 v=0; v<3;v++)
				face_indices[v]=getIndex(i+v);					
							
			T &v1 = *((T*)(&getVertex(face_indices[0])));					
			T &v2 =	*((T*)(&getVertex(face_indices[1])));						
			T &v3 =	*((T*)(&getVertex(face_indices[2])));

			core::face3d<T> face(v1, v2, v3);						

			back_faces_single.set_used(0);
			front_faces_single.set_used(0);

			s32 base_idx = added_verts.size();
	
			splitFaceByPlane(div_plane, face, back_faces_single, front_faces_single, added_verts);	

			for (u32 f=0; f<front_faces_single.size(); f++)
			{
				for (s32 v=0; v<3;v++)
				{
					s32 vidx = front_faces_single[f].Indices[v];

					if (vidx<3)
						front_faces_single[f].Indices[v] = face_indices[vidx];
					else
					{						
						front_faces_single[f].Indices[v] = base_idx + vidx-3;
						front_faces_single[f].Additional[v]=true;
					}
				}

				front_faces.push_back(front_faces_single[f]);
			}

			for (u32 f=0; f<back_faces_single.size(); f++)
			{
				for (s32 v=0; v<3; v++)
				{
					s32 vidx = back_faces_single[f].Indices[v];

					if (vidx<3)
						back_faces_single[f].Indices[v] = face_indices[vidx];
					else
					{
						back_faces_single[f].Indices[v] = base_idx + vidx - 3;
						back_faces_single[f].Additional[v]=true;
					}
				}

				back_faces.push_back(back_faces_single[f]);
			}
		}

		// setting materials

		front_mb->getMaterial() = getMaterial();
		back_mb->getMaterial() = getMaterial();

		// creating vertices remapping arrays and adding vertices

		core::array<s32> remap_front, remap_back;	
		remap_front.set_used(vert_cnt);
		remap_back.set_used(vert_cnt);

		for (s32 i=0; i<vert_cnt; i++)
		{
			remap_front[i]=-2; 
			remap_back[i] =-2;  
		}

		for (u32 f=0; f<front_faces.size(); f++)
		{
			splited_face<T> &ff = front_faces[f];

			for (s32 v=0; v<3; v++)
			{
				if (!ff.Additional[v])
					remap_front[ ff.Indices[v] ] = -1;
			}
		}		

		for (u32 f=0; f<back_faces.size(); f++)
		{		
			splited_face<T> &bf = back_faces[f];

			for (s32 v=0; v<3; v++)
			{
				if (!bf.Additional[v])
					remap_back[ bf.Indices[v] ] = -1;
			}
		}

		s32 front_vert_used_cnt=0, back_vert_used_cnt=0;
		for (s32 v=0; v<vert_cnt; v++)
		{
			const vid::S3DBaseVertex &vert = getVertex(v);

			if (remap_front[v]==-1)
			{
				front_mb->addVertex(vert, T::Type);
				remap_front[v] = front_vert_used_cnt++;
			}
			if (remap_back[v]==-1)
			{				
				back_mb->addVertex(vert, T::Type);
				remap_back[v] = back_vert_used_cnt++;
			}
		}
		
		s32 base_front_idx = front_mb->getVertexCount();
		s32 base_back_idx = back_mb->getVertexCount();

		for (u32 v=0; v<added_verts.size(); v++)
		{
			front_mb->addVertex(added_verts[v], T::Type);
			back_mb->addVertex (added_verts[v], T::Type);
		}

		// adding indices

		s32 index;

		for (u32 f=0; f<front_faces.size(); f++)
		{
			splited_face<T> &ff = front_faces[f];

			for (s32 v=0; v<3; v++)
			{				
				if (ff.Additional[v])
					index = base_front_idx + ff.Indices[v];
				else
					index = remap_front[ff.Indices[v]];

				front_mb->addIndex(index);
			}			
		} 

		for (u32 f=0; f<back_faces.size(); f++)
		{
			splited_face<T> &bf = back_faces[f];			

			for (s32 v=0; v<3; v++)
			{
				if (bf.Additional[v])
					index = base_back_idx + bf.Indices[v];
				else
					index = remap_back[bf.Indices[v]];

				back_mb->addIndex(index);
			}
		} 

		return true;
	}
	
	//! merge two besh buffer into one
	virtual bool mergeWith(IMeshBuffer *other) 
	{ 
		if (!other || this==other || 
			getMaterial()!=other->getMaterial() || 
			!other->getVertexCount() || !other->getIndexCount() || 
			getVertexType()!=other->getVertexType()
			)
			return false;

		u32 init_vsize = getVertexCount();
		u32 init_isize = getIndexCount();

		u32 new_vsize = init_vsize + other->getVertexCount();
		u32 new_isize = init_isize + other->getIndexCount();

		Vertices.set_used(new_vsize);

		memcpy( 
			(T*)getVertices() + init_vsize, 
			other->getVertices(), 
			other->getVertexCount()*sizeof(T)
			);

		Indices.set_used(new_isize);

		for (u32 i=init_isize, oi=0; i<new_isize; i++, oi++)
		{
			Indices[i] = init_vsize + other->getIndex(oi);
		}

		// recalculating bounding box

		if (!init_isize || !init_vsize)
			BoundingBox.reset(other->getBoundingBox());
		else
			BoundingBox.addInternalBox(other->getBoundingBox());

		return true; 
	}

	//! clearing geometry from this mesh buffer
	virtual void clearMeshBufferGeometry()
	{
		Vertices.clear();				
		Indices.clear();
	}

	//! returns index size 
	virtual E_INDEX_TYPE getIndexType()
	{
		if (sizeof(Ti) == 2)
			return EIT_16_BIT;

		return EIT_32_BIT;
	}

    vid::SMaterial      Material;     //! material for this meshBuffer.
    core::array<T>      Vertices;     //! Array of vertices
    core::array<Ti>     Indices;      //! Array of the Indices.
    core::aabbox3d<f32> BoundingBox;  //! bounding box, holding all mesh buffer geometry in
};

//--------------------------------------------------------------------------

class mesh_buffer_factory
{
public:

	static IMeshBuffer* create(vid::E_VERTEX_TYPE type, E_INDEX_TYPE itype=EIT_16_BIT)
	{		
		if (type==vid::EVT_SIMPLE)
		{
			if (itype==EIT_16_BIT)
				return new mesh_buffer<vid::S3DVertexSimple, u16>();
			else
				return new mesh_buffer<vid::S3DVertexSimple, u32>();
		}
		else
		if (type==vid::EVT_SIMPLE_COLOURED)
		{
			if (itype==EIT_16_BIT)
				return new mesh_buffer<vid::S3DVertexSimpleColoured, u16>();
			else
				return new mesh_buffer<vid::S3DVertexSimpleColoured, u32>();
		}
		else
		if (type==vid::EVT_1TCOORDS)
		{
			if (itype==EIT_16_BIT)
				return new mesh_buffer<vid::S3DVertex1TCoords, u16>();
			else
				return new mesh_buffer<vid::S3DVertex1TCoords, u32>();
		}
		else
		if (type==vid::EVT_1TCOORDS_COLOURED)
		{
			if (itype==EIT_16_BIT)
				return new mesh_buffer<vid::S3DVertex1TCoordsColoured, u16>();
			else
				return new mesh_buffer<vid::S3DVertex1TCoordsColoured, u32>();
		}
		else
		if (type==vid::EVT_2TCOORDS)
		{
			if (itype==EIT_16_BIT)
				return new mesh_buffer<vid::S3DVertex2TCoords, u16>();
			else
				return new mesh_buffer<vid::S3DVertex2TCoords, u32>();
		}
		else
		if (type==vid::EVT_2TCOORDS_COLOURED)
		{
			if (itype==EIT_16_BIT)
				return new mesh_buffer<vid::S3DVertex2TCoordsColoured, u16>();
			else
				return new mesh_buffer<vid::S3DVertex2TCoordsColoured, u32>();
		}
		else
		if (type==vid::EVT_1TCOORDS_TBN)
		{
			if (itype==EIT_16_BIT)
				return new mesh_buffer<vid::S3DVertex1TCoordsTBN, u16>();
			else
				return new mesh_buffer<vid::S3DVertex1TCoordsTBN, u32>();
		}
		else
		if (type==vid::EVT_1TCOORDS_TBN_COLOURED)
		{
			if (itype==EIT_16_BIT)
				return new mesh_buffer<vid::S3DVertex1TCoordsTBNColoured, u16>();
			else
				return new mesh_buffer<vid::S3DVertex1TCoordsTBNColoured, u32>();
		}
		else
		if (type==vid::EVT_2TCOORDS_TBN)
		{
			if (itype==EIT_16_BIT)
				return new mesh_buffer<vid::S3DVertex2TCoordsTBN, u16>();
			else
				return new mesh_buffer<vid::S3DVertex2TCoordsTBN, u32>();
		}
		else
		if (type==vid::EVT_2TCOORDS_TBN_COLOURED)
		{
			if (itype==EIT_16_BIT)
				return new mesh_buffer<vid::S3DVertex2TCoordsTBNColoured, u16>();
			else
				return new mesh_buffer<vid::S3DVertex2TCoordsTBNColoured, u32>();
		}
		else
		if (type==vid::EVT_1TCOORDS_RHW_COLOURED)
		{
			if (itype==EIT_16_BIT)
				return new mesh_buffer<vid::S3DVertex1TCoordsRHWColoured, u16>();
			else
				return new mesh_buffer<vid::S3DVertex1TCoordsRHWColoured, u32>();
		}

		return NULL;
	}

	static IMeshBuffer* create_from(IMeshBuffer* mb_init)
	{	
		if (!mb_init)
			return NULL;

		vid::E_VERTEX_TYPE vtype = mb_init->getVertexType();
		E_INDEX_TYPE       itype = mb_init->getIndexType();

		IMeshBuffer* mbnew = create(vtype, itype);

		if (mbnew)
		{
			// pre allocate memory for vertices and indices
			mbnew->setVerticesUsed(mb_init->getVertexCount());
			mbnew->setVerticesUsed(0);
			mbnew->setIndicesUsed(mb_init->getIndexCount());
			mbnew->setIndicesUsed(0);

			// fill vertices list
			for (s32 v=0; v<mb_init->getVertexCount(); v++)
				mbnew->addVertex(mb_init->getVertex(v), vtype);
			
			// fill indices list
			for (s32 i=0; i<mb_init->getIndexCount(); i++)	
				mbnew->addIndex(mb_init->getIndex(i));
			
			vid::SMaterial &mbnew_material = mbnew->getMaterial();
			mbnew_material = mb_init->getMaterial();

			mbnew->recalculateBoundingBox();
		}

		return mbnew;
	}
};

//--------------------------------------------------------------------------

class mesh_buffer_manipulator
{
public:

	static bool rescale_normals(IMeshBuffer* mb, const core::vector3df &new_scale)
	{
		if (!mb)
			return false;

		vid::E_VERTEX_TYPE type = mb->getVertexType();

		if (type==vid::EVT_1TCOORDS)
		{
			mesh_buffer<vid::S3DVertex1TCoords> *mbcasted	= 
				static_cast<mesh_buffer<vid::S3DVertex1TCoords>*>(mb);

			rescale_normals(mbcasted, new_scale);			
		}
		else
		if (type==vid::EVT_1TCOORDS_COLOURED)
		{
			mesh_buffer<vid::S3DVertex1TCoordsColoured> *mbcasted	= 
				static_cast<mesh_buffer<vid::S3DVertex1TCoordsColoured>*>(mb);

			rescale_normals(mbcasted, new_scale);			
		}
		else
		if (type==vid::EVT_2TCOORDS)
		{
			mesh_buffer<vid::S3DVertex2TCoords> *mbcasted	= 
				static_cast<mesh_buffer<vid::S3DVertex2TCoords>*>(mb);

			rescale_normals(mbcasted, new_scale);			
		}
		else
		if (type==vid::EVT_2TCOORDS_COLOURED)
		{
			mesh_buffer<vid::S3DVertex2TCoordsColoured>	*mbcasted = 
				static_cast<mesh_buffer<vid::S3DVertex2TCoordsColoured>*>(mb);

			rescale_normals(mbcasted, new_scale);						
		}
		else
		if (type==vid::EVT_1TCOORDS_TBN)
		{
			mesh_buffer<vid::S3DVertex1TCoordsTBN> *mbcasted	= 
				static_cast<mesh_buffer<vid::S3DVertex1TCoordsTBN>*>(mb);

			rescale_normals(mbcasted, new_scale);			
		}
		else
		if (type==vid::EVT_1TCOORDS_TBN_COLOURED)
		{
			mesh_buffer<vid::S3DVertex1TCoordsTBNColoured> *mbcasted	= 
				static_cast<mesh_buffer<vid::S3DVertex1TCoordsTBNColoured>*>(mb);

			rescale_normals(mbcasted, new_scale);			
		}
		else
		if (type==vid::EVT_2TCOORDS_TBN)
		{
			mesh_buffer<vid::S3DVertex2TCoordsTBN> *mbcasted	= 
				static_cast<mesh_buffer<vid::S3DVertex2TCoordsTBN>*>(mb);

			rescale_normals(mbcasted, new_scale);			
		}
		else
		if (type==vid::EVT_2TCOORDS_TBN_COLOURED)
		{
			mesh_buffer<vid::S3DVertex2TCoordsTBNColoured>	*mbcasted = 
				static_cast<mesh_buffer<vid::S3DVertex2TCoordsTBNColoured>*>(mb);

			rescale_normals(mbcasted, new_scale);						
		}
		else
		{
			return false;
		}

		return true;
	}

	template <class T>
	static void rescale_normals(mesh_buffer<T> *mb, const core::vector3df &new_scale)
	{
		if (!mb)
			return;

		core::array<T> &v = mb->Vertices;

		for (u32 i=0; i<v.size(); i++)
		{
			v[i].Normal.normalize();
			v[i].Normal *= new_scale;
		}			
	} 

	static bool calculate_tangents(IMeshBuffer* mb)
	{
		if (!mb)
			return false;

		vid::E_VERTEX_TYPE type = mb->getVertexType();
		
		if (type==vid::EVT_1TCOORDS_TBN)
		{
			mesh_buffer<vid::S3DVertex1TCoordsTBN> *mbcasted	= 
				static_cast<mesh_buffer<vid::S3DVertex1TCoordsTBN>*>(mb);

			calculate_tangents(mbcasted);			
		}
		else
		if (type==vid::EVT_1TCOORDS_TBN_COLOURED)
		{
			mesh_buffer<vid::S3DVertex1TCoordsTBNColoured> *mbcasted	= 
				static_cast<mesh_buffer<vid::S3DVertex1TCoordsTBNColoured>*>(mb);

			calculate_tangents(mbcasted);			
		}
		else
		if (type==vid::EVT_2TCOORDS_TBN)
		{
			mesh_buffer<vid::S3DVertex2TCoordsTBN> *mbcasted	= 
				static_cast<mesh_buffer<vid::S3DVertex2TCoordsTBN>*>(mb);

			calculate_tangents(mbcasted);			
		}
		else
		if (type==vid::EVT_2TCOORDS_TBN_COLOURED)
		{
			mesh_buffer<vid::S3DVertex2TCoordsTBNColoured>	*mbcasted = 
				static_cast<mesh_buffer<vid::S3DVertex2TCoordsTBNColoured>*>(mb);

			calculate_tangents(mbcasted);						
		}
		else
		{
			return false;
		}

		return true;
	}

	static void calculate_tangents(
		core::vector3df& normal,
		core::vector3df& tangent,
		core::vector3df& binormal,
		const core::vector3df& vt1, const core::vector3df& vt2, const core::vector3df& vt3, // vertices
		const core::vector2df& tc1, const core::vector2df& tc2, const core::vector2df& tc3  // texture coords
		) 
	{			
		core::vector3df v1 = vt1 - vt2;
		core::vector3df v2 = vt3 - vt1;
		
		// Calculate binormal

		f32 deltaX1 = tc1.X - tc2.X;
		f32 deltaX2 = tc3.X - tc1.X;
		binormal = (v1 * deltaX2) - (v2 * deltaX1);
		binormal.normalize();

		// Calculate tangent

		f32 deltaY1 = tc1.Y - tc2.Y;
		f32 deltaY2 = tc3.Y - tc1.Y;
		tangent = (v1 * deltaY2) - (v2 * deltaY1);
		tangent.normalize();

		//Now, we take the cross product of the tangents to get a vector which 
	    //should point in the same direction as our normal calculated above. 
	    //If it points in the opposite direction (the dot product between the normals is less than zero), 
	    //then we need to reverse the s and t tangents. 
	    //This is because the triangle has been mirrored when going from tangent space to object space.
	    //reverse tangents if necessary

		core::vector3df txb = tangent.getCrossProduct(binormal);
		if (txb.getDotProduct(normal) < 0.0f)
		{
			tangent *= -1.0f;
			binormal *= -1.0f;
		}  		
	}

	template <class T>
	static void calculate_tangents(mesh_buffer<T> *mb)
	{
		if (!mb)
			return;

		s32 idxCnt = mb->getIndexCount();    		
		core::array<T> &v = mb->Vertices;

		for (s32 i=0; i<idxCnt; i+=3)
		{
			u32 idx0 = mb->getIndex(i  );
			u32 idx1 = mb->getIndex(i+1);
			u32 idx2 = mb->getIndex(i+2);

			calculate_tangents(
				v[idx0].Normal,
				v[idx0].Tangent,
				v[idx0].Binormal,
				v[idx0].Pos,
				v[idx1].Pos,
				v[idx2].Pos,
				v[idx0].TCoords,
				v[idx1].TCoords,
				v[idx2].TCoords);

			calculate_tangents(
				v[idx1].Normal,
				v[idx1].Tangent,
				v[idx1].Binormal,
				v[idx1].Pos,
				v[idx2].Pos,
				v[idx0].Pos,
				v[idx1].TCoords,
				v[idx2].TCoords,
				v[idx0].TCoords);

			calculate_tangents(
				v[idx2].Normal,
				v[idx2].Tangent,
				v[idx2].Binormal,
				v[idx2].Pos,
				v[idx0].Pos,
				v[idx1].Pos,
				v[idx2].TCoords,
				v[idx0].TCoords,
				v[idx1].TCoords);
		}
	}

	static bool reset_t1_to_t0(IMeshBuffer* mb)
	{	
		if (!mb)
			return false;

		vid::E_VERTEX_TYPE type = mb->getVertexType();

		if (type==vid::EVT_2TCOORDS)
		{
			mesh_buffer<vid::S3DVertex2TCoords> *mbcasted	= 
				static_cast<mesh_buffer<vid::S3DVertex2TCoords>*>(mb);

			if (mbcasted)
			{
				u32 vert_cnt = mb->getVertexCount();

				for (u32 v=0; v<vert_cnt; v++)
				{
					mbcasted->Vertices[v].TCoords2 = mbcasted->Vertices[v].TCoords;						
				}

				return true;
			}
		}
		else
		if (type==vid::EVT_2TCOORDS_COLOURED)
		{
			mesh_buffer<vid::S3DVertex2TCoordsColoured>	*mbcasted = 
				static_cast<mesh_buffer<vid::S3DVertex2TCoordsColoured>*>(mb);

			if (mbcasted)
			{
				u32 vert_cnt = mb->getVertexCount();

				for (u32 v=0; v<vert_cnt; v++)
				{
					mbcasted->Vertices[v].TCoords2 = mbcasted->Vertices[v].TCoords;						
				}	

				return true;
			}
		}
		else
		if (type==vid::EVT_2TCOORDS_TBN)
		{
			mesh_buffer<vid::S3DVertex2TCoordsTBN> *mbcasted	= 
				static_cast<mesh_buffer<vid::S3DVertex2TCoordsTBN>*>(mb);

			if (mbcasted)
			{
				u32 vert_cnt = mb->getVertexCount();

				for (u32 v=0; v<vert_cnt; v++)
				{
					mbcasted->Vertices[v].TCoords2 = mbcasted->Vertices[v].TCoords;						
				}

				return true;
			}
		}
		else
		if (type==vid::EVT_2TCOORDS_TBN_COLOURED)
		{
			mesh_buffer<vid::S3DVertex2TCoordsTBNColoured>	*mbcasted = 
				static_cast<mesh_buffer<vid::S3DVertex2TCoordsTBNColoured>*>(mb);

			if (mbcasted)
			{
				u32 vert_cnt = mb->getVertexCount();

				for (u32 v=0; v<vert_cnt; v++)
				{
					mbcasted->Vertices[v].TCoords2 = mbcasted->Vertices[v].TCoords;						
				}	

				return true;
			}
		}
		
		return false;		
	}

	//----------------------------------------------------------------------------

	template <class TVert, class TInd>
	static TInd* create_edge_adjacency_data(
		const void *vertices, const void *indices, u32 IndexCount,
		f32 epsilon = 0.000001f)
	{
		if (IndexCount > ~((TInd)0))
			return NULL;

		const TVert *Vertices = (const TVert *)vertices;
		const TInd *Indices = (const TInd *)indices;
		TInd* Adjacency = new TInd[IndexCount];
		f32 t = 0;
		u32 f = 0;
		core::hash_array<u64> vert_hash;

		bool log = IndexCount == 60;

		epsilon = 1.0f / epsilon;

		if (log)
		{
			LOGGER.logInfo("Indices list:");
		}

		for (f = 0; f < IndexCount; f ++)
		{
			if (log)
			{
				LOGGER.logInfo(" [%d]=index=%d",
					f,  Indices[f]);
			}
		}

		if (log)
		{
			LOGGER.logInfo("Vertices hash:");
		}

		for (f = 0; f < IndexCount; f++)
		{
			const core::vector3df &v = Vertices[Indices[f]].Pos;
			u64 hash = 0xffffffff & (
				s32(v.X * epsilon) ^ s32(v.Y * epsilon) ^ s32(v.Z * epsilon));
			u64 value = (hash << 32) | f;
			vert_hash.push_back(hash, value);
			Adjacency[f] = (TInd)-1;

			if (log)
			{
				LOGGER.logInfo(" [%d]={%f %f %f} index=%d hash=%016I64X value=%016I64X", f, v.X, v.Y, v.Z, Indices[f], hash, value);
			}
		}

		if (log)
		{
			LOGGER.logInfo("Creating adjacency:");
		}
		
		// go through all faces and fetch their three neighbours
		for (f = 0; f < IndexCount; f += 3)
		{
			for (s32 edge = 0; edge < 3; ++edge)
			{
				u32 idx[] = {f+edge, f+((edge+1)%3)};
				core::hash_table<TInd, bool, 16> tmp_hash;

				for (u32 i = 0; i < sizeof(idx) / sizeof(*idx); i++)
				{
					const core::vector3df &v = Vertices[Indices[idx[i]]].Pos;
					u64 hash = 0xffffffff & (
						s32(v.X * epsilon) ^ s32(v.Y * epsilon) ^ s32(v.Z * epsilon));
					const core::array<u64> &values = vert_hash(hash);
					u32 vsz = values.size();
					for (u32 v = 0; v < vsz; v++)
					{
						u64 _value = values[v];
						TInd _idx = (TInd)(0xffffffff & _value);
						TInd face = (TInd)(_idx / 3);
						const core::vector3df &vert = Vertices[Indices[idx[i]]].Pos;
						if (_idx == idx[i] || Vertices[Indices[_idx]].Pos != vert)
							continue;

			if (log)
			{
				LOGGER.logInfo(" [%d]={%f %f %f} index=%d value=%016I64X",
					_idx, vert.X, vert.Y, vert.Z, Indices[_idx], _value);
			}
						
						if (tmp_hash.get_value(face))
						{
							Adjacency[f + edge] = face;
							break;
						}
						else
							tmp_hash.set_value(face, true);
					}
				}
			}
		}

		if (log)
		{
			LOGGER.logInfo("Adjacency list:");
		}

		for (f = 0; f < IndexCount; f++)
		{
			if (log)
			{
				const core::vector3df &vert = Vertices[Indices[f]].Pos;

				LOGGER.logInfo(" [%d]={%f %f %f} index=%d",
					f, vert.X, vert.Y, vert.Z, Adjacency[f]);
			}
		}

		if (log)
		{
			LOGGER.logInfo("Adjacency check:");

			for (f = 0; f < IndexCount; f+=3)
			{
				s32 face = f / 3;
				for (s32 edge = 0; edge < 3; ++edge)
				{
					u32 idx[] = {f+edge, f+((edge+1)%3)};
					s32 adj = Adjacency[f + edge];
					u32 neigh = 0;
					for (s32 _edge = 0; _edge < 3; ++_edge)
					{
						u32 _f = adj * 3;
						u32 _idx[] = {_f+_edge,_f+((_edge+1)%3)};
						s32 _adj = Adjacency[_f + _edge];
						if (_adj == face && (
								(
								Vertices[Indices[_idx[0]]].Pos == Vertices[Indices[idx[0]]].Pos &&
								Vertices[Indices[_idx[1]]].Pos == Vertices[Indices[idx[1]]].Pos
								) ||
								(
								Vertices[Indices[_idx[0]]].Pos == Vertices[Indices[idx[1]]].Pos &&
								Vertices[Indices[_idx[1]]].Pos == Vertices[Indices[idx[0]]].Pos
								)
								)
								)
							neigh++;
					}
					if (neigh != 1)
					{
						LOGGER.logInfo(" Face[%d] Edge[%d] error (neigh=%d)", face, edge, neigh);
					}
				}
			}

			LOGGER.logInfo("********************");
		}

		return Adjacency;
	}

	//----------------------------------------------------------------------------

	template <class TInd>
	struct SAdjFace
	{
		SAdjFace() : Next(0), Index((TInd)-1) {}
		~SAdjFace() { delete Next; }
		SAdjFace	*Next;
		TInd		Index;
	};

	template <class TVert, class TInd>
	static SAdjFace<TInd>* create_vertex_adjacency_data(
		const void *vertices, const void *indices, u32 IndexCount,
		f32 epsilon = 0.000001f)
	{
		if (IndexCount > ~((TInd)0))
			return NULL;

		const TVert *Vertices = (const TVert *)vertices;
		const TInd *Indices = (const TInd *)indices;

		SAdjFace<TInd>* Adjacency = new SAdjFace<TInd>[IndexCount];

		f32 t = 0;
		u32 f = 0;
		core::hash_array<u64> vert_hash;

		epsilon = 1.0f / epsilon;

		for (f = 0; f < IndexCount; f++)
		{
			const core::vector3df &v = Vertices[Indices[f]].Pos;
			u64 hash = 0xffffffff & (
				s32(v.X * epsilon) ^ s32(v.Y * epsilon) ^ s32(v.Z * epsilon));
			u64 value = (hash << 32) | f;
			vert_hash.push_back(hash, value);
		}

		for (f = 0; f < IndexCount; f ++)
		{
			const core::vector3df &v = Vertices[Indices[f]].Pos;
			u64 hash = 0xffffffff & (
				s32(v.X * epsilon) ^ s32(v.Y * epsilon) ^ s32(v.Z * epsilon));
			u64 value = (hash << 32) | f;
			const core::array<u64> &values = vert_hash(hash);
			u32 vsz = values.size();
			SAdjFace<TInd> *adj = &Adjacency[f];
			for (u32 v = 0; v < vsz; v++)
			{
				u64 _value = values[v];
				u64 _hash = 0xffffffff & (_value >> 32);
				if (hash != _hash || value == _value)
					continue;
				adj->Index = (TInd)(0xffffffff & _value);
				adj->Next = new SAdjFace<TInd>;
				adj = adj->Next;
			}
		}   
		return Adjacency;
	}

	//----------------------------------------------------------------------------

	template <class TVert, class TInd>
	static TInd* create_edge_adjacency_data(
		const void *vertices, const void *indices, u32 IndexCount,
		u32 *errors = NULL, f32 epsilon = 0.000001f)
	{
		if (IndexCount > ~((TInd)0))
			return NULL;

		const TVert *Vertices = (const TVert *)vertices;
		const TInd *Indices = (const TInd *)indices;
		TInd* Adjacency = new TInd[IndexCount];
		f32 t = 0;
		u32 f = 0;
		core::hash_array<u64> vert_hash;

		bool log = IndexCount == 60;

		epsilon = 1.0f / epsilon;

		for (f = 0; f < IndexCount; f++)
		{
			const core::vector3df &v = Vertices[Indices[f]].Pos;
			u64 hash = 0xffffffff & (
				s32(v.X * epsilon) ^ s32(v.Y * epsilon) ^ s32(v.Z * epsilon));
			u64 value = (hash << 32) | f;
			vert_hash.push_back(hash, value);
			Adjacency[f] = (TInd)-1;
		}
		
		// go through all faces and fetch their three neighbours
		for (f = 0; f < IndexCount; f += 3)
		{
			if (!core::triangle3df(
					Vertices[Indices[f]].Pos,
					Vertices[Indices[f + 1]].Pos,
					Vertices[Indices[f + 2]].Pos).isValide())
			{
				if (errors)
					*errors += 1;
				LOGGER.logWarn("Face %d is degradeted, ignore it", f);
				continue;
			}

			for (s32 edge = 0; edge < 3; ++edge)
			{
				u32 idx[] = {f+edge, f+((edge+1)%3)};
				core::hash_table<TInd, u64, 16> tmp_hash;

				for (u32 i = 0; i < sizeof(idx) / sizeof(*idx); i++)
				{
					const core::vector3df &v = Vertices[Indices[idx[i]]].Pos;
					u64 hash = 0xffffffff & (
						s32(v.X * epsilon) ^ s32(v.Y * epsilon) ^ s32(v.Z * epsilon));
					const core::array<u64> &values = vert_hash(hash);
					u32 vsz = values.size();
					for (u32 v = 0; v < vsz; v++)
					{
						u64 _value = values[v];
						TInd _idx = (TInd)(0xffffffff & _value);
						TInd face = (TInd)(_idx / 3);

						if (!core::triangle3df(
								Vertices[Indices[face * 3]].Pos,
								Vertices[Indices[face * 3 + 1]].Pos,
								Vertices[Indices[face * 3 + 2]].Pos).isValide())
							continue;

						const core::vector3df &vert = Vertices[Indices[idx[i]]].Pos;
						if (_idx == idx[i] || Vertices[Indices[_idx]].Pos != vert)
							continue;

						if (tmp_hash.get_value(face))
						{
							Adjacency[f + edge] = face;
							break;
						}
						else
						{
							tmp_hash.set_value(face, true);
						}
					}
				}
			}
		}

		return Adjacency;
	}

	template <class TInd>
	static TInd* create_edge_adjacency_data(vid::E_VERTEX_TYPE vtype,
		const void *vertices, const void *indices, u32 indexCount,
		u32 *errors = NULL, f32 epsilon = 0.000001f)
	{
		switch (vtype)
		{
		case vid::EVT_SIMPLE:
			return create_edge_adjacency_data<vid::S3DVertexSimple, TInd>(
				vertices, indices, indexCount, errors, epsilon);
		case vid::EVT_SIMPLE_COLOURED:
			return create_edge_adjacency_data<vid::S3DVertexSimpleColoured, TInd>(
				vertices, indices, indexCount, errors, epsilon);
		case vid::EVT_1TCOORDS:
			return create_edge_adjacency_data<vid::S3DVertex1TCoords, TInd>(
				vertices, indices, indexCount, errors, epsilon);
		case vid::EVT_1TCOORDS_COLOURED:
			return create_edge_adjacency_data<vid::S3DVertex1TCoordsColoured, TInd>(
				vertices, indices, indexCount, errors, epsilon);
		case vid::EVT_2TCOORDS:
			return create_edge_adjacency_data<vid::S3DVertex2TCoords, TInd>(
				vertices, indices, indexCount, errors, epsilon);
		case vid::EVT_2TCOORDS_COLOURED:
			return create_edge_adjacency_data<vid::S3DVertex2TCoordsColoured, TInd>(
				vertices, indices, indexCount, errors, epsilon);
		case vid::EVT_1TCOORDS_TBN:
			return create_edge_adjacency_data<vid::S3DVertex1TCoordsTBN, TInd>(
				vertices, indices, indexCount, errors, epsilon);
		case vid::EVT_1TCOORDS_TBN_COLOURED:
			return create_edge_adjacency_data<vid::S3DVertex1TCoordsTBNColoured, TInd>(
				vertices, indices, indexCount, errors, epsilon);
		case vid::EVT_2TCOORDS_TBN:
			return create_edge_adjacency_data<vid::S3DVertex2TCoordsTBN, TInd>(
				vertices, indices, indexCount, errors, epsilon);
		case vid::EVT_2TCOORDS_TBN_COLOURED:
			return create_edge_adjacency_data<vid::S3DVertex2TCoordsTBNColoured, TInd>(
				vertices, indices, indexCount, errors, epsilon);
		}
		return NULL;
	}

	//----------------------------------------------------------------------------

	template <class TInd>
	static SAdjFace<TInd>* create_vertex_adjacency_data(vid::E_VERTEX_TYPE vtype,
		const void *vertices, const void *indices, u32 indexCount,
		f32 epsilon = 0.000001f)
	{
		switch (vtype)
		{
		case vid::EVT_SIMPLE:
			return create_vertex_adjacency_data<vid::S3DVertexSimple, TInd>(
				vertices, indices, indexCount, epsilon);
		case vid::EVT_SIMPLE_COLOURED:
			return create_vertex_adjacency_data<vid::S3DVertexSimpleColoured, TInd>(
				vertices, indices, indexCount, epsilon);
		case vid::EVT_1TCOORDS:
			return create_vertex_adjacency_data<vid::S3DVertex1TCoords, TInd>(
				vertices, indices, indexCount, epsilon);
		case vid::EVT_1TCOORDS_COLOURED:
			return create_vertex_adjacency_data<vid::S3DVertex1TCoordsColoured, TInd>(
				vertices, indices, indexCount, epsilon);
		case vid::EVT_2TCOORDS:
			return create_vertex_adjacency_data<vid::S3DVertex2TCoords, TInd>(
				vertices, indices, indexCount, epsilon);
		case vid::EVT_2TCOORDS_COLOURED:
			return create_vertex_adjacency_data<vid::S3DVertex2TCoordsColoured, TInd>(
				vertices, indices, indexCount, epsilon);
		case vid::EVT_1TCOORDS_TBN:
			return create_vertex_adjacency_data<vid::S3DVertex1TCoordsTBN, TInd>(
				vertices, indices, indexCount, epsilon);
		case vid::EVT_1TCOORDS_TBN_COLOURED:
			return create_vertex_adjacency_data<vid::S3DVertex1TCoordsTBNColoured, TInd>(
				vertices, indices, indexCount, epsilon);
		case vid::EVT_2TCOORDS_TBN:
			return create_vertex_adjacency_data<vid::S3DVertex2TCoordsTBN, TInd>(
				vertices, indices, indexCount, epsilon);
		case vid::EVT_2TCOORDS_TBN_COLOURED:
			return create_vertex_adjacency_data<vid::S3DVertex2TCoordsTBNColoured, TInd>(
				vertices, indices, indexCount, epsilon);
		}
		return NULL;
	}
};

//--------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//--------------------------------------------------------------------------

#endif // #ifndef mesh_bufferHPP

