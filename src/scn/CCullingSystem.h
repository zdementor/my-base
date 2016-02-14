//|-------------------------------------------------------------------------
//| File:        CCullingSystem.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CCullingSystemHPP
#define CCullingSystemHPP
//----------------------------------------------------------------------------

#include <scn/ICullingSystem.h>
#include <scn/ISceneNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! The Cull System doing geometry culling
class CCullingSystem : public ICullingSystem
{
public:

	//! constructor
	CCullingSystem();

	//! destructor
	virtual ~CCullingSystem();

	//! return true if this Scene Node is culled
	virtual bool isCull(ISceneNode* node) const;

	//! return true if this Bounding box is culled
	virtual bool isCull(const core::aabbox3df &bb, E_CULLING_TESTS tests) const;

	//! return true if this 3D position is culled
	virtual bool isCull(const core::vector3df &pos, E_CULLING_TESTS tests) const;

	//! return true if this 3D sphere is culled
	virtual bool isCull(const core::vector3df &center, f32 radius, E_CULLING_TESTS tests) const;

	virtual bool addOccluder(ISceneNode* node);

	virtual s32 getOccludersCount();

	virtual void clearOccluders();

	virtual void initializeOccluders();

	virtual void setViewer(const core::view_frustrum &frustrum, const core::vector3df &look_at);

	virtual const core::view_frustrum& getViewerFrustrum();
	virtual const core::vector3df&     getViewerLookAt();

private:

	struct SOccluderEntry
    {
		SOccluderEntry() {}

		SOccluderEntry(const SOccluderEntry& other)	
		{
			(*this) = other;
		}

        SOccluderEntry(ISceneNode* n, core::vector3df camera, bool orthogonal)
        {   
			node = n;
			tbbox=n->getTransformedBoundingBox();

			if (!orthogonal)
				distance = (f32)(tbbox.getCenter().getDistanceFrom(camera));
			else
				distance=0;

			tbbox.MaxEdge.Y -= 1.25f;
			tbbox.MaxEdge.X += 8;
			tbbox.MaxEdge.Z += 8;
			tbbox.MinEdge.X -= 8;
			tbbox.MinEdge.Z -= 8;

			tbbox.getOcclusionPlanes( camera, planes, orthogonal );	
        }

        ISceneNode* node;
        f32 distance;
		core::aabbox3df tbbox;
		core::array< core::plane3df > planes;

		SOccluderEntry& operator = (const SOccluderEntry& other) 
        {
			node     = other.node;
			distance = other.distance;
			tbbox    = other.tbbox;
			planes   = other.planes;

			return *this;
		}

        bool operator < (const SOccluderEntry& other) const
        {
			// from back to front
			return (distance > other.distance);
        }

		bool isOccludeByBottomVolume(const core::vector3df& pos) const
		{
			if (pos.X > tbbox.MinEdge.X && 
				pos.Z > tbbox.MinEdge.Z && 
				pos.X < tbbox.MaxEdge.X &&
 				pos.Y < tbbox.MaxEdge.Y &&                
				pos.Z < tbbox.MaxEdge.Z
				)
			{
				return true;
			}

			return false;			
		}

		bool isOccludeByPlanes(const core::vector3df& pos) const
		{
			for (u32 op=0; op<planes.size(); op++)
			{		
				if (planes[op].classifyPointRelation(pos) != core::EIREL3D_BEHIND)
					return false;
			}

			return true;
		}

		bool isOccludeByPlanes(const core::vector3df& center, f32 radius) const
		{
			for (u32 op=0; op<planes.size(); op++)
			{		
				if (planes[op].classifySphereRelation(center, radius) != core::EIREL3D_BEHIND)
					return false;
			}

			return true;
		}	

		bool isOccludeByPlanes(const core::aabbox3df& bb) const
		{
			for (u32 op=0; op<planes.size(); op++)
			{
				if (bb.classifyPlaneRelation(planes[op]) != core::EIREL3D_BEHIND)
					return false;
			}

			return true;
		}

		bool isPointInside(const core::vector3df& pos) const
		{ return tbbox.isPointInside(pos); }
    };

	core::array<SOccluderEntry> m_Occluders;
	core::view_frustrum         m_ViewerFrustrum;

	core::vector3df m_ViewerLookAtDirection;
};

//----------------------------------------------------------------------------
} // end namespace scene
} // end namespace irr
//----------------------------------------------------------------------------

#endif // #ifndef CCullingSystemHPP

