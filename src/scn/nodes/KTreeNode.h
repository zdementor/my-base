//|-------------------------------------------------------------------------
//| File:        KTreeNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef KTreeNodeHPP
#define KTreeNodeHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/mesh_buffer.h>
#include <scn/SMesh.h>
#include <scn/ICullingSystem.h>
#include <vid/IVideoDriver.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {				   
//---------------------------------------------------------------------------

const f32 DIRT_PIXEL_SIZE      = 3.0f;
const f32 DIRT_HEIGHT_SIZE_MUL = 4.0f;
const s32 DIRT_TEX_MAX_SIZE    = 256;

//---------------------------------------------------------------------------

struct SRenderGeometry
{
	SRenderGeometry() 
		: MeshBuffer(0), DirtTextureTop(0), DirtTextureLeft(0), DirtTextureFront(0),
	RenderBuffer(NULL) {}

	SRenderGeometry(IMeshBuffer *mb, const core::aabbox3df &node_bb,
		vid::ITexture *dirt_tex_top, vid::ITexture *dirt_tex_left,
		vid::ITexture *dirt_tex_front)
		: MeshBuffer(mb), TreeNodeBBox(node_bb),
	DirtTextureTop(dirt_tex_top), DirtTextureLeft(dirt_tex_left),
	DirtTextureFront(dirt_tex_front), RenderBuffer(NULL)
	{
		TransformedBBox = MeshBuffer->getBoundingBox();
		mb->calculateTangents();
		RenderBuffer = VIDEO_DRIVER.createRenderBuffer(
			vid::ERBT_STATIC,
			mb->getVertexType(), mb->getVertexCount(),
			mb->getIndexType(), mb->getIndexCount(),
			mb->getDrawPrimitiveType());
		mb->uploadToRenderBuffer(RenderBuffer);
	}

	SRenderGeometry(const SRenderGeometry &other)
		: RenderBuffer(NULL)
	{ this->operator = (other); }

	virtual ~SRenderGeometry()
	{ SAFE_DROP(RenderBuffer); }

	void operator = (const SRenderGeometry &other)
	{
		SAFE_DROP(RenderBuffer);
		MeshBuffer = other.MeshBuffer;
		TreeNodeBBox = other.TreeNodeBBox;
		TransformedBBox = other.TransformedBBox;
		TreeNodeTransformedBBox = other.TreeNodeTransformedBBox;
		TreeNodeTransformedBBoxExtend = other.TreeNodeTransformedBBoxExtend;
		TreeNodeTransformedBBoxCenter = other.TreeNodeTransformedBBoxCenter;
		DirtTextureTop = other.DirtTextureTop;
		DirtTextureLeft = other.DirtTextureLeft;
		DirtTextureFront = other.DirtTextureFront;
		RenderBuffer = other.RenderBuffer;
		SAFE_GRAB(RenderBuffer);
	}

	void transformBoundingBox(const core::matrix4& transformation)
	{
		TransformedBBox = MeshBuffer->getBoundingBox();
		transformation.transformBox(TransformedBBox);

		TreeNodeTransformedBBox = TreeNodeBBox;
		transformation.transformBox(TreeNodeTransformedBBox);

		TreeNodeTransformedBBoxCenter = TreeNodeTransformedBBox.getCenter();
		TreeNodeTransformedBBoxExtend = TreeNodeTransformedBBox.getExtend();
	}

	IMeshBuffer *			MeshBuffer;
	core::aabbox3df			TransformedBBox, TreeNodeBBox, TreeNodeTransformedBBox;
	core::vector3df			TreeNodeTransformedBBoxExtend, TreeNodeTransformedBBoxCenter; 
	vid::ITexture *			DirtTextureTop, *DirtTextureLeft, *DirtTextureFront;
	vid::IRenderBuffer *	RenderBuffer;
};  

//---------------------------------------------------------------------------

class KTreeNode
{
public:

    // constructor
    KTreeNode(
		SMesh *mesh, 
		u32& node_count, u32 currentdepth, u32 &max_depth,
		SMesh* mesh_vbo_create,
		u32 max_triangles_per_node, u32 &bad_faces_count,
		bool creaate_dirt_textures
		) : 
		m_ChildCount(0), 
		m_DirtTextureTop(0), m_DirtTextureFront(0), m_DirtTextureLeft(0),
		m_DirtTexSize(0,0,0),
		m_DirtImageTop(0), m_DirtImageFront(0), m_DirtImageLeft(0),  
		m_DirtTexturesLocked(false), m_BBoxExt(0,0,0)
    {
		memset(m_Children, 0, m_MaxChildCount*sizeof(KTreeNode*));

		currentdepth++;

        m_Depth = currentdepth;   

		if (m_Depth>max_depth)
		{
			max_depth = m_Depth;
		}
        
		s32 mbi;

        if (mesh->getMeshBuffersCount()<=0)
        {
            return;
        }

		// now letz calculate our bounding box

		bool first_point = true;
		
		for (mbi=0; mbi<mesh->getMeshBuffersCount(); mbi++)
		{
			IMeshBuffer *mb = mesh->getMeshBuffer(mbi);

			for (s32 v=0; v<mb->getVertexCount(); v++)
			{				
				if (first_point)
				{
					m_BBox.reset( mb->getVertex(v).Pos );

					first_point = false;
				}
				else
				{
					m_BBox.addInternalPoint( mb->getVertex(v).Pos );
				}
			}
		}

		m_TransformedBBox = m_BBox;

		u32 totalPrimitives=0;

		for (mbi=0; mbi<mesh->getMeshBuffersCount(); mbi++)
		{
			IMeshBuffer *mb = mesh->getMeshBuffer(mbi);

			totalPrimitives += mb->getIndexCount();
		}

		totalPrimitives = totalPrimitives/3;

		bool store_verts_here = false;

		if (totalPrimitives < max_triangles_per_node)
		{	
			store_verts_here = true;

			if (creaate_dirt_textures)
			{
				m_BBoxExt = m_BBox.getExtend();
				m_DirtTexSize.set(
					m_BBoxExt.X/(f32)DIRT_PIXEL_SIZE+0.5, 
					m_BBoxExt.Y/(f32)DIRT_PIXEL_SIZE+0.5, 
					m_BBoxExt.Z/(f32)DIRT_PIXEL_SIZE+0.5);

				f32 nearest_x = core::math::GetNearestPowerOfTwo(m_DirtTexSize.X);
				f32 nearest_y = core::math::GetNearestPowerOfTwo(m_DirtTexSize.Y);
				f32 nearest_z = core::math::GetNearestPowerOfTwo(m_DirtTexSize.Z);  

				f32 nearest_x_div2 = nearest_x/2.0f;
				f32 nearest_y_div2 = nearest_y/2.0f;
				f32 nearest_z_div2 = nearest_z/2.0f;

				if (core::math::Abs(m_DirtTexSize.X-nearest_x) >
						core::math::Abs(m_DirtTexSize.X-nearest_x_div2))
					nearest_x = nearest_x_div2;
				if (core::math::Abs(m_DirtTexSize.Y-nearest_y) >
						core::math::Abs(m_DirtTexSize.Y-nearest_y_div2))
					nearest_y = nearest_y_div2;
				if (core::math::Abs(m_DirtTexSize.Z-nearest_z) >
						core::math::Abs(m_DirtTexSize.Z-nearest_z_div2))
					nearest_z = nearest_z_div2;

				if ((m_BBoxExt.X/nearest_x)*0.5f>(m_BBoxExt.Z/nearest_z))
					nearest_z /= 2.0f;
				else  
				if ((m_BBoxExt.Z/nearest_z)*0.5f>(m_BBoxExt.X/nearest_x))
					nearest_x /= 2.0f;

				m_DirtTexSize.X = nearest_x;
				m_DirtTexSize.Y = nearest_y;
				m_DirtTexSize.Z = nearest_z;

				if (nearest_x <= DIRT_TEX_MAX_SIZE &&
					nearest_y <= DIRT_TEX_MAX_SIZE &&
					nearest_z <= DIRT_TEX_MAX_SIZE)
				{
					m_DirtTexPixelSize.X = m_BBoxExt.X/nearest_x;
					m_DirtTexPixelSize.Y = m_BBoxExt.Y/nearest_y;
					m_DirtTexPixelSize.Z = m_BBoxExt.Z/nearest_z;

					vid::IVideoDriver &driver = VIDEO_DRIVER;
					core::dimension2di dim;
					bool mips = driver.getTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS);

					driver.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS, false);

					dim.set(m_DirtTexSize.Z, m_DirtTexSize.X);
					m_DirtTextureTop = driver.createTexture(dim, img::ECF_A8R8G8B8);

					dim.set(m_DirtTexSize.Z, (s32)(m_DirtTexSize.Y / DIRT_HEIGHT_SIZE_MUL + 0.5));
					m_DirtTextureLeft = driver.createTexture(dim, img::ECF_A8R8G8B8);

					dim.set(m_DirtTexSize.X, (s32)(m_DirtTexSize.Y / DIRT_HEIGHT_SIZE_MUL + 0.5));
					m_DirtTextureFront = driver.createTexture(dim, img::ECF_A8R8G8B8);

					driver.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS, mips);
				}
				else
				{
					store_verts_here = false;
				}
			}			

			if (store_verts_here)
				++node_count;
		}

		s32 minVBOSize = (
			DEVICE.getDeviceFlagValue(dev::EDCF_LIGHTING_MEDIUM_QUALITY) || 
			DEVICE.getDeviceFlagValue(dev::EDCF_LIGHTING_HIGH_QUALITY)) ?
				0 : 3 * 1024;

		for (mbi = 0; mbi < mesh->getMeshBuffersCount(); mbi++)
		{
			IMeshBuffer* mb = mesh->getMeshBuffer(mbi);

			vid::SMaterial& mb_mat = mb->getMaterial();
		
			if (store_verts_here || (
					!creaate_dirt_textures &&
					mb->getIndexCount() < minVBOSize))
			// store mesh buffer here
			{
				m_RenderGeometry.push_back(SRenderGeometry(
					mb, m_BBox, m_DirtTextureTop, m_DirtTextureLeft, m_DirtTextureFront));
				mesh_vbo_create->addMeshBuffer(mb);
			}
		}	

		if (store_verts_here)
		{    
			return;
		}

		core::vector3df middle = m_BBox.getCenter();

        core::vector3df edges[8];
        m_BBox.getEdges(edges);

		//    1-------5
		//   /|      /|
		//  3-------7 |
		//  | 0-----|-4
		//  |/      |/
		//  2-------6 

		core::plane3df div_plane1, div_plane2, div_plane3;
		core::vector3df edge1, edge2, edge3;

		edge1 = (edges[0]-edges[2])*-1.0f;
		edge2 = (edges[0]-edges[4])*-1.0f;	
		edge3 = (edges[0]-edges[1])*-1.0f;

		f32 edge1_len=edge1.getLength(), edge2_len=edge2.getLength(), edge3_len=edge3.getLength();

		div_plane1.set(middle, edge1);
		div_plane2.set(middle, edge2);
		div_plane3.set(middle, edge3);

		if ((edge1_len>1.5*edge2_len && edge1_len>1.5*edge3_len) ||
			(edge2_len>1.5*edge1_len && edge2_len>1.5*edge3_len) ||
			(edge3_len>1.5*edge1_len && edge3_len>1.5*edge2_len) 
			)
		{
			m_ChildCount = 2;
		}
		else
		if (edge3_len>0.5*edge2_len && edge3_len>0.5*edge1_len)
		{
			m_ChildCount = 8;
		}
		else			
		{
			m_ChildCount = 4;
		}

		// calculate all possible childrens

		SMesh * mesh_for_child[m_MaxChildCount];
		memset(mesh_for_child, 0, m_MaxChildCount*sizeof(SMesh*));
			
		for (u32 ch=0; ch<m_ChildCount; ch++) 
		{ 
			mesh_for_child[ch] = new SMesh();
		}

		E_INDEX_TYPE itype = (totalPrimitives>65536)?EIT_32_BIT:EIT_16_BIT;

		for (s32 mbi=0; mbi<mesh->getMeshBuffersCount(); mbi++)
		{
			IMeshBuffer *mb = mesh->getMeshBuffer(mbi);

			if (!creaate_dirt_textures &&
				mb->getIndexCount() < minVBOSize)
			// already added to the current node
				continue;
				
			vid::E_VERTEX_TYPE vtype = mb->getVertexType();				

			IMeshBuffer *mbch[m_MaxChildCount];
			memset(mbch, 0, m_MaxChildCount*sizeof(IMeshBuffer*));
			
			for (u32 ch=0; ch<m_ChildCount; ch++) 
			{  					
				mbch[ch] = mesh_buffer_factory::create(vtype, itype);
			}

			if (m_ChildCount==2)
			{
				if (edge1_len>edge2_len && edge1_len>edge3_len)
				{
					mb->splitByPlane( div_plane1, mbch[0],  mbch[1] );
				}
				else if (edge2_len>edge1_len && edge2_len>edge3_len)
				{
					mb->splitByPlane( div_plane2, mbch[0],  mbch[1] );
				}
				else 
				{
					mb->splitByPlane( div_plane3, mbch[0],  mbch[1] );
				}
			}
			else if (m_ChildCount==4)
			{
				IMeshBuffer *front_mb = mesh_buffer_factory::create(vtype, itype);
				IMeshBuffer *back_mb  = mesh_buffer_factory::create(vtype, itype);

				mb->splitByPlane      ( div_plane1, front_mb, back_mb );
				front_mb->splitByPlane( div_plane2, mbch[0],  mbch[1] );
				back_mb->splitByPlane ( div_plane2, mbch[2],  mbch[3] );

				front_mb->drop();
				back_mb->drop();
			}
			else if (m_ChildCount==8)
			{
				IMeshBuffer *front_mb = mesh_buffer_factory::create(vtype, itype);
				IMeshBuffer *back_mb  = mesh_buffer_factory::create(vtype, itype);
				IMeshBuffer *mb0 = mesh_buffer_factory::create(vtype, itype);
				IMeshBuffer *mb1 = mesh_buffer_factory::create(vtype, itype);
				IMeshBuffer *mb2 = mesh_buffer_factory::create(vtype, itype);
				IMeshBuffer *mb3 = mesh_buffer_factory::create(vtype, itype);

				mb->splitByPlane      ( div_plane1, front_mb,  back_mb );

				front_mb->splitByPlane( div_plane2, mb0, mb1 );
				back_mb->splitByPlane ( div_plane2, mb2, mb3 );

				front_mb->drop();
				back_mb->drop();

				mb0->splitByPlane( div_plane3, mbch[0], mbch[1] );
				mb1->splitByPlane( div_plane3, mbch[2], mbch[3] );
				mb2->splitByPlane( div_plane3, mbch[4], mbch[5] );
				mb3->splitByPlane( div_plane3, mbch[6], mbch[7] );			

				mb0->drop();
				mb1->drop();
				mb2->drop();
				mb3->drop();
			}

			for (u32 ch=0; ch<m_ChildCount; ++ch)
			{
				if (mbch[ch]->getVertexCount()>0)
				{
					mesh_for_child[ch]->addMeshBuffer(mbch[ch]);						
				}

				mbch[ch]->recalculateBoundingBox();

				// one grab is much enough
				SAFE_DROP(mbch[ch]);					
			}   
		}

		s32 child_count = 0;

		for (u32 ch=0; ch<m_ChildCount; ++ch)
		{
			if (mesh_for_child[ch]->getMeshBuffersCount()>0)
			{
				m_Children[ch] = new KTreeNode(
					mesh_for_child[ch],
					node_count, currentdepth, max_depth,
					mesh_vbo_create,  
					max_triangles_per_node, bad_faces_count,
					creaate_dirt_textures
					);
			}

			SAFE_DROP(mesh_for_child[ch]);	

			if (!m_Children[ch])
				continue;

			if (child_count==0)
				m_BBox.reset(m_Children[ch]->m_BBox);
			else
				m_BBox.addInternalBox(m_Children[ch]->m_BBox);

			child_count++;
		}

		m_ChildCount=child_count;

		m_TransformedBBox=m_BBox;
    }

    // destructor
    ~KTreeNode()
    {
        for (u32 i=0; i<m_ChildCount; ++i)
            SAFE_DELETE(m_Children[i]);

		m_RenderGeometry.clear();

		SAFE_DROP(m_DirtTextureTop);
		SAFE_DROP(m_DirtTextureFront);
		SAFE_DROP(m_DirtTextureLeft);
    } 	

    void renderBoundingBoxes(
		const core::view_frustrum& frustrum,
        core::array< core::aabbox3d<f32> >&out_boxes
		)
    {
		bool visible = true;
			
		if (frustrum.classifyBoxRelation(m_TransformedBBox)==0) visible = false;

		if (visible)
        {
			bool has_childs = false;
            
            for (u32 i=0; i<m_ChildCount; ++i)
			{
                m_Children[i]->renderBoundingBoxes(frustrum, out_boxes);
				has_childs = true;
			}

			if (!has_childs)
			{
				core::aabbox3df bb = m_TransformedBBox;					
				bb.MaxEdge += core::vector3df(0,1,0);

				out_boxes.push_back(bb);
			}
        }
    }

	//! returns all mesh buffers partially or full enclosed by a view frustrum.
	void getVisibleRenderGeometry( E_CULLING_TESTS cull_tests, core::array<SRenderGeometry> &viewed_geometry )
	{
		ICullingSystem& cullsys = ICullingSystem::getSingleton();		

		bool visible = !cullsys.isCull(	m_TransformedBBox, cull_tests );		

		if (visible)
		{
			if (m_RenderGeometry.size()>0)
			{
				s32 mb_cnt = m_RenderGeometry.size();

				for (s32 mbi=0; mbi<mb_cnt; mbi++)
				{
					if (cullsys.isCull( m_RenderGeometry[mbi].TransformedBBox, cull_tests )) 
						continue;

					viewed_geometry.push_back( m_RenderGeometry[mbi] );
				}
			}				

			for (u32 i=0; i<m_ChildCount; ++i)
				m_Children[i]->getVisibleRenderGeometry(cull_tests, viewed_geometry);
		}
	} 	

	void setAsoluteTransformation(const core::matrix4& transformation)
	{
		u32 i=0;

		bool need_to_reset = true;

		for (i=0; i<m_RenderGeometry.size(); i++)
		{
			m_RenderGeometry[i].transformBoundingBox(transformation);

			if (i==0)
			{
				m_TransformedBBox.reset(m_RenderGeometry[i].TransformedBBox);
				need_to_reset = false;
			}
			else
			{
				m_TransformedBBox.addInternalBox(m_RenderGeometry[i].TransformedBBox);
			}
		}

		for (i=0; i<m_ChildCount; ++i)
		{
			m_Children[i]->setAsoluteTransformation(transformation);

			if (i==0 && need_to_reset)
			{
				m_TransformedBBox.reset(m_Children[i]->m_TransformedBBox);
				need_to_reset = false;
			}
			else
				m_TransformedBBox.addInternalBox(m_Children[i]->m_TransformedBBox);
		}
	}

	//!
	void unlockDirtTextures()
	{
		if (m_DirtTexturesLocked)
		{
			m_DirtTextureTop->unlock(); 			
			m_DirtTextureLeft->unlock();
			m_DirtTextureFront->unlock();

			m_DirtImageTop = m_DirtImageLeft = m_DirtImageFront = NULL;

			m_DirtTexturesLocked=false;
		}

		for (u32 i=0; i<m_ChildCount; ++i)
			m_Children[i]->unlockDirtTextures(); 
	}

	//!
	void makeDirtSpot(
		core::vector3df &pos, core::vector3df &size,
		const core::array< core::array< img::SColor > > &top_dirt,   
		const core::array< core::array< img::SColor > > &left_dirt,    
		const core::array< core::array< img::SColor > > &front_dirt, 
		s32 num_x, s32 num_y, s32 num_z,
		f32 interp
		)
	{
		_makeDirtSpot(
			pos, size, 
			top_dirt, left_dirt, front_dirt,
			num_x, num_y, num_z,
			interp
			);				
	}

	//!
	void getLeafNodesFromBoundingBox(core::aabbox3df &bb, core::array<KTreeNode*>&leaf_nodes)
	{
		if (m_BBox.intersectsWithBox(bb))
		{
			if (m_ChildCount==0)
			{
				// its a leaf node
				leaf_nodes.push_back(this);						
			}
			else
			{
				for (u32 i=0; i<m_ChildCount; ++i)
					m_Children[i]->getLeafNodesFromBoundingBox(bb, leaf_nodes); 
			}
		}		
	}

private:

	//!
	void _lockDirtTextures()
	{
		if (!m_DirtTexturesLocked)
		{
			if (m_DirtTextureTop&&m_DirtTextureLeft&&m_DirtTextureFront)
			{
				m_DirtImageTop  = m_DirtTextureTop->lock();
				m_DirtImageLeft = m_DirtTextureLeft->lock();
				m_DirtImageFront= m_DirtTextureFront->lock();

				m_DirtTexturesLocked = true;
			}
		} 
	}

	//!
	void _makeDirtSpot(
		core::vector3df &pos, core::vector3df &size,
		const core::array< core::array< img::SColor > > &top_dirt,
		const core::array< core::array< img::SColor > > &left_dirt,
		const core::array< core::array< img::SColor > > &front_dirt,
		s32 _num_x, s32 _num_y, s32 _num_z,
		f32 interp
		)
	{
		_lockDirtTextures();

		core::vector3di sizei(
			size.X/m_DirtTexPixelSize.X+0.5f, 
			size.Y/m_DirtTexPixelSize.Y+0.5f,
			size.Z/m_DirtTexPixelSize.Z+0.5f
			);
		if (sizei.Y<=2)
			sizei.Y = 3;

		core::vector3di posi(
			(f32)m_DirtTexSize.X*(f32)((pos.X - m_BBox.MinEdge.X)/m_BBoxExt.X)+0.5f,
			(f32)m_DirtTexSize.Y*(f32)((pos.Y - m_BBox.MinEdge.Y)/m_BBoxExt.Y)+0.5f,
			(f32)m_DirtTexSize.Z*(f32)((pos.Z - m_BBox.MinEdge.Z)/m_BBoxExt.Z)+0.5f
			);	

		s32 x = posi.X;
		s32 y = posi.Y;
		s32 z = posi.Z;
	      
		f32 disp_x = s32(sizei.X/2.0f+0.5f);
		f32 disp_y = s32(((f32)sizei.Y/2.0f+0.5f)/DIRT_HEIGHT_SIZE_MUL+0.5f);
		f32 disp_z = s32(sizei.Z/2.0f+0.5f); 

		s32 num_x = disp_x*2+1;
		s32 num_y = disp_y*2+1;
		s32 num_z = disp_z*2+1;

		f32 m1 = 1.0f, m2 = -1.0f; 

		if (m_DirtImageTop)
		{
			for (s32 xi = 0; xi < _num_x; xi++)
			{
				s32 _xi = ((f32)(xi + 1)/(f32)_num_x)*(f32)(num_x - 1);
				s32 _x = x + _xi - disp_x;
				_x = (m_DirtTexSize.X - 1) * m1 + _x * m2;

				for (s32 zi=0; zi<_num_z; zi++)
				{					
					s32 _zi = ((f32)(zi + 1)/(f32)_num_z)*(f32)(num_z - 1);
					s32 _z = z + _zi - disp_z;					

					img::SColor c1 = top_dirt[zi][xi];  
					if (c1.color)
					{
						img::SColor c2 = m_DirtImageTop->getPixel(_z, _x);
						f32 c2a = c2.getAlpha();
						f32 c1a = c1.getAlpha();
						c1 = c1.getInterpolated(c2,interp*(c1a/255.0f));
						if (c1.getAlpha()<c2a)
							c1.setAlpha(c2a);
						m_DirtImageTop->setPixel(_z, _x, c1);
					}
				}
			}
		}

		f32 texY = m_DirtTexSize.Y/DIRT_HEIGHT_SIZE_MUL+0.5f;

		y = (f32)y/DIRT_HEIGHT_SIZE_MUL;

		if (m_DirtImageLeft)
		{			
			for (s32 yi = 0; yi < _num_y; yi++)
			{
				s32 _yi = ((f32)(yi + 1)/(f32)_num_y)*(f32)(num_y - 1);
				s32 _y = y + _yi - disp_y;
				_y = (texY - 1) * m1 + _y * m2;

				for (s32 zi=0; zi<_num_z; zi++)
				{			
					s32 _zi = ((f32)(zi + 1)/(f32)_num_z)*(f32)(num_z - 1);		
					s32 _z = z + _zi - disp_z; 					

					const img::SColor &c = left_dirt[zi][yi];   
					if (c.color)
						m_DirtImageLeft->setPixel(_z, _y, c);
				}
			}
		}

		if (m_DirtImageFront)
		{
			for (s32 yi = 0; yi < _num_y; yi++)
			{
				s32 _yi = ((f32)(yi + 1)/(f32)_num_y)*(f32)(num_y - 1);
				s32 _y = y + _yi - disp_y;
				_y = (texY - 1) * m1 + _y * m2;

				for (s32 xi=0; xi<_num_x; xi++)
				{			
					s32 _xi = ((f32)(xi + 1)/(f32)_num_x)*(f32)(num_x - 1);
					s32 _x = x + _xi - disp_x; 					

					const img::SColor &c = front_dirt[xi][yi];  
					if (c.color)
						m_DirtImageFront->setPixel(_x, _y, c);
				}
			}
		}		
	}	

	const static u32 m_MaxChildCount=8;
	KTreeNode* m_Children[m_MaxChildCount];
	u32 m_ChildCount;

    core::aabbox3d<f32> m_BBox, m_TransformedBBox;        
    u32 m_Depth;

	core::vector3df m_BBoxExt;
	
	core::array<SRenderGeometry> m_RenderGeometry;

	vid::ITexture *m_DirtTextureTop, *m_DirtTextureLeft, *m_DirtTextureFront;
	core::vector3di m_DirtTexSize;
	core::vector3df m_DirtTexPixelSize;

	img::IImage *m_DirtImageTop, *m_DirtImageLeft, *m_DirtImageFront;

	bool m_DirtTexturesLocked;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // KTreeNodeHPP
