//|-------------------------------------------------------------------------
//| File:        KTree.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef KTreeHPP
#define KTreeHPP
//---------------------------------------------------------------------------

#include "KTreeNode.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! K-Tree - Bi-Tree, Quad-Tree, Oct-Tree in one (Mega Tree)
class KTree : public IUnknown
{
public:    

	u32 m_NodeCount, m_BadFacesCount, m_MaxDepth;
	SMesh *m_VBOMesh;
	core::aabbox3df m_BoundingBox;

    //! constructor
    KTree(IMesh *mesh, s32 max_triangles_per_node, bool create_dirt_textures) 
		: m_RootNode(0), m_VBOMesh(0), m_BadFacesCount(0), m_NodeCount(0), m_MaxDepth(0)
    {
	#if MY_DEBUG_MODE 
		IUnknown::setClassName("QuadTree");
	#endif
 
		// mesh for wich wee will build the oct-tree structur

		m_VBOMesh   = new SMesh();

		// create a copy of the initial mesh

		SMesh *mesh_copy = new SMesh();

		for (s32 m=0; m<mesh->getMeshBuffersCount(); m++)
		{   
			IMeshBuffer* mb = mesh->getMeshBuffer(m);  			

			IMeshBuffer* mbnew = mesh_buffer_factory::create_from(mb);
		
			if (!mbnew)
			{
				LOGGER.log("Can't create Quad Tree for this mesh buffer.", io::ELL_ERROR);

				continue;
			}
			mesh_copy->addMeshBuffer(mbnew);
			mbnew->drop();			
		}

        // create quad tree root node

		m_RootNode = new KTreeNode(	
			mesh_copy,
			m_NodeCount, 0, m_MaxDepth, m_VBOMesh, 
			max_triangles_per_node, m_BadFacesCount,   
			create_dirt_textures
			);

		m_VBOMesh->recalculateBoundingBox();

		m_BoundingBox = m_VBOMesh->getBoundingBox();

		// destroy a copy of the initial mesh

		delete mesh_copy;
    }
    
    //! destructor
    virtual ~KTree()
    {
		SAFE_DELETE(m_RootNode);
		SAFE_DROP(m_VBOMesh);
    }

	bool isOK()
	{
		return m_RootNode && m_VBOMesh;
	}

	// for debug purposes only, renders the bounding boxes of the tree
	void renderBoundingBoxes(
		const core::view_frustrum& frustrum,
        core::array< core::aabbox3d<f32> >&out_boxes
		)
	{
		out_boxes.clear();

		m_RootNode->renderBoundingBoxes(frustrum, out_boxes);
	}

    //! returns all mesh buffers partially or full enclosed by a view frustrum.
    void getVisibleRenderGeometry( E_CULLING_TESTS cull_tests, core::array<SRenderGeometry>& viewed_geometry )
    {
		viewed_geometry.set_used(0);
		
		m_RootNode->getVisibleRenderGeometry(cull_tests, viewed_geometry);	
    }

	void setAsoluteTransformation(const core::matrix4& transformation)
	{
		m_RootNode->setAsoluteTransformation(transformation);
	}

	//! returns the material based on the zero based index i
	vid::SMaterial& getMaterial(s32 i)
	{
		if (i>=0 && i<getMaterialsCount())
		{
			return m_VBOMesh->getMeshBuffer(i)->getMaterial();
		}

		return *((vid::SMaterial*)NULL);
	}

	//! returns amount of materials used by this scene node.
	s32 getMaterialsCount()
	{
		return 	isOK() ? (m_VBOMesh->getMeshBuffersCount()) : 0;
	}

	//!
	void unlockDirtTextures()
	{
		m_RootNode->unlockDirtTextures();
	}

	//!
	core::array<KTreeNode*>& getLeafNodesFromBoundingBox(core::aabbox3df &bb)
	{
		static core::array<KTreeNode*> leaf_nodes;
		leaf_nodes.set_used(0);

		m_RootNode->getLeafNodesFromBoundingBox(bb, leaf_nodes);

		return leaf_nodes;
	}

	//!
	void makeDirtSpot(
		core::aabbox3df &spot_bb, core::vector3df &pos, core::vector3df &size, img::SColor &color
		)
	{	
		const s32 size_mul = 4;

		s32 c_a = color.getAlpha();
		s32 c_r = color.getRed();
		s32 c_g = color.getGreen();
		s32 c_b = color.getBlue();

		core::vector3di sizei(
			size.X/DIRT_PIXEL_SIZE+0.5f, 
			size.Y/DIRT_PIXEL_SIZE+0.5f,
			size.Z/DIRT_PIXEL_SIZE+0.5f
			);
		sizei *= size_mul;

		f32 disp_x = s32(sizei.X/2.0f+0.5f);
		f32 disp_y = s32(sizei.Y/2.0f+0.5f);
		f32 disp_z = s32(sizei.Z/2.0f+0.5f); 

		s32 num_x = disp_x*2+1;
		s32 num_y = disp_y*2+1;
		s32 num_z = disp_z*2+1;

		s32 num_x_x2 = (num_x-1)/2+1;
		s32 num_z_x2 = (num_z-1)/2+1;
		s32 num_x_x4 = (num_x_x2-1)/2+1;
		s32 num_z_x4 = (num_z_x2-1)/2+1;
		s32 num_x_x8 = (num_x_x4-1)/2+1;
		s32 num_z_x8 = (num_z_x4-1)/2+1;

		bool do_spot_sample = (sizei.X>size_mul&&sizei.Z>size_mul);

		static core::array< core::array< img::SColor > >
			top_dirt_sphere, left_dirt_sphere, front_dirt_sphere;
		static core::array< core::array< img::SColor > > top_dirt_splash;
		static core::array< core::array< img::SColor > > top_dirt;
		
		top_dirt.set_used       (num_z);
		top_dirt_splash.set_used(num_z);
		top_dirt_sphere.set_used(num_z);
		s32	pitch_x = num_x*sizeof(img::SColor);
		for (s32 i=0; i<num_z; i++)
		{
			top_dirt[i].set_used       (num_x);
			top_dirt_splash[i].set_used(num_x);
			top_dirt_sphere[i].set_used(num_x);
		}    		

		/////////////////////

		s32	pitch_y = num_y*sizeof(img::SColor);

		left_dirt_sphere.set_used(num_z);
		for (s32 i=0; i<num_z; i++)
		{
			left_dirt_sphere[i].set_used(num_y);
		}

		front_dirt_sphere.set_used(num_x);
		for (s32 i=0; i<num_x; i++)
		{
			front_dirt_sphere[i].set_used(num_y);
		} 			

		f32 num_x_1 = num_x-1;
		f32 num_z_1 = num_z-1;    
		f32 num_x_x4_1 = num_x_x4-1;
		f32 num_z_x4_1 = num_z_x4-1;  
		f32 num_x_x8_1 = num_x_x8-1;
		f32 num_z_x8_1 = num_z_x8-1;

		if (do_spot_sample)
		{
			for (s32 xi=0; xi<num_x; xi++)
			{
				f32 fx = xi/num_x_1;

				for (s32 zi=0; zi<num_z; zi++)
				{   	  				
					f32 fz	= zi/num_z_1;

					f32 ka = m_DirtSLookUp.lookUpTop(fx, fz);
					top_dirt_sphere[zi][xi].set(c_a*ka, c_r*ka, c_g*ka, c_b*ka);

					f32 kc = m_DirtSLookUp.lookUpMaskX4(fx, fz)*m_DirtSLookUp.lookUpMaskX8(fx, fz);						 
					ka *= kc;

					top_dirt_splash[zi][xi].set(c_a*ka, c_r*kc, c_g*kc, c_b*kc);
				}
			}

			for (s32 xi=0; xi<num_x; xi++)
			{
				for (s32 zi=0; zi<num_z; zi++)
				{ 
					top_dirt[zi][xi].set(
						top_dirt_splash[zi][xi].getAlpha() + top_dirt_sphere[zi][xi].getAlpha(),
						top_dirt_splash[zi][xi].getRed()   + top_dirt_sphere[zi][xi].getRed(),
						top_dirt_splash[zi][xi].getGreen() + top_dirt_sphere[zi][xi].getGreen(),
						top_dirt_splash[zi][xi].getBlue()  + top_dirt_sphere[zi][xi].getBlue()
						);  					
				}
			}
		}
		else
		{
			for (s32 xi=0; xi<num_x; xi++)
			{
				f32 fx = xi/num_x_1;

				for (s32 zi=0; zi<num_z; zi++)
				{   	  				
					f32 fz	= zi/num_z_1;

					f32 ka = m_DirtSLookUp.lookUpTop(fx, fz);
					top_dirt[zi][xi].set(c_a*ka, c_r*ka, c_g*ka, c_b*ka);					
				}
			}  			
		}

		f32 num_y_1 = num_y-1; 

		for (s32 yi=0; yi<num_y; yi++)
		{ 
			f32 fy = yi/num_y_1;

			for (s32 zi=0; zi<num_z; zi++)
			{			  
				f32 fz	= zi/num_z_1;

				left_dirt_sphere[zi][yi] = m_DirtSLookUp.lookUpSide(fy, fz);
			}

			for (s32 xi=0; xi<num_x; xi++)
			{
				f32 fx	= xi/num_x_1;

				front_dirt_sphere[xi][yi] = m_DirtSLookUp.lookUpSide(fy, fx);	
			}
		}

		// make several simple spots, to make single nice

		core::array<KTreeNode*>& leaf_nodes = getLeafNodesFromBoundingBox(spot_bb);
		s32 leaf_cnt = leaf_nodes.size();

		disp_x = 0; 
		disp_z = 0;
	
		if (do_spot_sample)
		{
			for (s32 i=0; i<3; i++)
			{	
				if (i>0)
				{
					disp_x = size.X * core::math::RangeRandom(-300.0f, 300.0f)/1000.0f;
					disp_z = size.Z * core::math::RangeRandom(-300.0f, 300.0f)/1000.0f;	
				}

				core::vector3df _pos(pos.X+disp_x, pos.Y, pos.Z+disp_z);

				for (s32 j=0; j<leaf_cnt; j++)
				{
					leaf_nodes[j]->makeDirtSpot(
						_pos, 
						size,  
						top_dirt, left_dirt_sphere, front_dirt_sphere, 
						num_x, num_y, num_z,
						0.15f
						);  
				}
			}
		}
		else
		{
			for (s32 j=0; j<leaf_cnt; j++)
			{
				leaf_nodes[j]->makeDirtSpot(
					pos, 
					size,  
					top_dirt, left_dirt_sphere, front_dirt_sphere, 
					num_x, num_y, num_z,
					0.45f
					);  
				}
		}		
	}

private: 

	struct SDirtLookUp
	{       
		SDirtLookUp()
		{
			memset(LookUpTop,    0x00, sizeof(f32)*DIRT_SPHERE_MAX_SIZE*DIRT_SPHERE_MAX_SIZE);					
			memset(LookUpSide,   0x00, sizeof(img::SColor)*DIRT_SPHERE_MAX_SIZE*DIRT_SPHERE_MAX_SIZE);

			f32 disp = s32(DIRT_SPHERE_MAX_SIZE/2.0f+0.5f);   		

			for (s32 xi=0; xi<DIRT_SPHERE_MAX_SIZE; xi++)
			{
				f32 xx = (disp-(f32)xi)/(disp+0.001f);

				for (s32 yi=0; yi<DIRT_SPHERE_MAX_SIZE; yi++)
				{   				
					f32 yy = (disp-(f32)yi)/(disp+0.001f);
					f32 disp_k = core::math::Sqrt(xx*xx+yy*yy);
			
					static const f32 d=1.1f;
					static const f32 d0= d*0.25f;
					static const f32 d1= d*0.5f;
					static const f32 d2= d*0.75f;				

					if (disp_k<d)
					{
						f32 ka = 0;  
						if      (disp_k>d2) ka = 0.20f;
						else if (disp_k>d1) ka = 0.45f;
						else if (disp_k>d0) ka = 0.7f;
						else                ka = 1.00f;					

						LookUpTop [xi][yi]=ka;	
						LookUpSide[xi][yi].set(255,255,255,255);
					}			
				}
			}

			/////////////////////   
			
			memset(LookUpMaskX4, 0x00, sizeof(f32)*DIRT_SPHERE_MAX_SIZE_X4*DIRT_SPHERE_MAX_SIZE_X4);

			for (s32 xi=0; xi<DIRT_SPHERE_MAX_SIZE_X4; xi++)
			{
				for (s32 yi=0; yi<DIRT_SPHERE_MAX_SIZE_X4; yi++)
				{
					LookUpMaskX4[xi][yi] = 
						(s32(core::math::RangeRandom(0.0f, 1000.0f) / 1000.0f+0.5f));
				}
			}

			///////////////////// 

			memset(LookUpMaskX8, 0x00, sizeof(f32)*DIRT_SPHERE_MAX_SIZE_X8*DIRT_SPHERE_MAX_SIZE_X8);					

			for (s32 xi=0; xi<DIRT_SPHERE_MAX_SIZE_X8; xi++)
			{
				for (s32 yi=0; yi<DIRT_SPHERE_MAX_SIZE_X8; yi++)
				{
					LookUpMaskX8[xi][yi] = 
						(s32(core::math::RangeRandom(0.0f, 1000.0f) / 1000.0f+0.5f));
				}
			} 
		}

		f32 lookUpTop(f32 fx, f32 fy) const 
		{
			s32 x = DIRT_SPHERE_MAX_SIZE_MINUS_1*fx;
			s32 y = DIRT_SPHERE_MAX_SIZE_MINUS_1*fy;
			return LookUpTop[x][y];
		}

		const img::SColor& lookUpSide(f32 fx, f32 fy) const 
		{
			s32 x = DIRT_SPHERE_MAX_SIZE_MINUS_1*fx;
			s32 y = DIRT_SPHERE_MAX_SIZE_MINUS_1*fy;
			return LookUpSide[x][y];
		}

		f32 lookUpMaskX4(f32 fx, f32 fy) const 
		{
			s32 x = DIRT_SPHERE_MAX_SIZE_X4_MINUS_1*fx;
			s32 y = DIRT_SPHERE_MAX_SIZE_X4_MINUS_1*fy;
			return LookUpMaskX4[x][y];
		}

		f32 lookUpMaskX8(f32 fx, f32 fy) const 
		{
			s32 x = DIRT_SPHERE_MAX_SIZE_X8_MINUS_1*fx;
			s32 y = DIRT_SPHERE_MAX_SIZE_X8_MINUS_1*fy;
			return LookUpMaskX8[x][y];
		}

	private:

		static const u32 DIRT_SPHERE_MAX_SIZE            = 128;
		static const u32 DIRT_SPHERE_MAX_SIZE_MINUS_1    = DIRT_SPHERE_MAX_SIZE-1;
		static const u32 DIRT_SPHERE_MAX_SIZE_X4         = DIRT_SPHERE_MAX_SIZE/4;
		static const u32 DIRT_SPHERE_MAX_SIZE_X4_MINUS_1 = DIRT_SPHERE_MAX_SIZE_X4-1;
		static const u32 DIRT_SPHERE_MAX_SIZE_X8         = DIRT_SPHERE_MAX_SIZE/8;
		static const u32 DIRT_SPHERE_MAX_SIZE_X8_MINUS_1 = DIRT_SPHERE_MAX_SIZE_X8-1;

		f32          LookUpTop [DIRT_SPHERE_MAX_SIZE][DIRT_SPHERE_MAX_SIZE];
		img::SColor  LookUpSide[DIRT_SPHERE_MAX_SIZE][DIRT_SPHERE_MAX_SIZE];

		f32 LookUpMaskX4 [DIRT_SPHERE_MAX_SIZE_X4][DIRT_SPHERE_MAX_SIZE_X4];
		f32 LookUpMaskX8 [DIRT_SPHERE_MAX_SIZE_X8][DIRT_SPHERE_MAX_SIZE_X8];
	};

	static SDirtLookUp m_DirtSLookUp;

	KTreeNode* m_RootNode;
};

//---------------------------------------------------------------------------

KTree::SDirtLookUp KTree::m_DirtSLookUp;

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // MyKTreeHPP
