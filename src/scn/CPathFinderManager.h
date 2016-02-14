//|-------------------------------------------------------------------------
//| File:        CPathFinderManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CPathFinderManagerHPP
#define CPathFinderManagerHPP
//----------------------------------------------------------------------------

#include <scn/IPathFinderManager.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class CPathFinderManager : public IPathFinderManager
{
public:

	//! constructor
	CPathFinderManager();

	//! destructor
	virtual ~CPathFinderManager();

	//! return path find calc count
	virtual u32 getPathFindCalcCount();

	//! zeroing path find calc counter
	virtual void zeroPathFindCalcCounter();

	//! finding path from start to destination point, return true on success
    virtual bool findPath(
        core::vector3df start_pos, core::vector3df dest_pos, 
        core::array< core::vector3df >& output_path,
		bool force_recalc = false
        );       

	//! adding new pathfinder to the pathfinders pool (A* pathfinder)
    virtual IPathFinder*  addAStarPathFinder(
		ITerrainSceneNode* terrain, img::IImage* walkability);

	//! adding new pathfinder to the pathfinders pool (A* pathfinder)
    virtual IPathFinder*  addAStarPathFinder(
		ISceneNode* parent, img::IImage* walkability);

	//! enabling/disabling our pathfinders, except default pathfinder, it always working
	virtual void setPathFindingEnabled(bool enabled);

	//! return true if our pathfinders are enabled
	virtual bool isPathFindingEnabled();

	virtual void mapUnitPosition  (const core::vector3df& pos);
	virtual void unmapUnitPosition(const core::vector3df& pos);

	virtual void clearUnitMappings();

	virtual bool isWalkablePosition(core::vector3df pos);
	
	virtual bool isWalkableLine(core::vector3df &src, core::vector3df &dst);

	virtual bool isViewableLine(core::vector3df &src, core::vector3df &dst);

	virtual void getPathCellLine(
        const core::vector3df &start_pos, const core::vector3df &dest_pos, 
        core::array <SPathCell> &output_line); 

	virtual core::vector3df getPositionOffsetFor(core::vector3df pos);

	virtual void validatePositionToGround(core::vector3df &pos);

	virtual core::vector3df getGroundPositionFor(const core::vector3df &pos);

	virtual f32 getHeightOverGround(const core::vector3df &pos);

	virtual bool isPointInsidePathFinding(const core::vector3df &pos);

	virtual bool checkIntersectionWithRay(const core::line3df &ray);

	virtual bool isBusy(const core::vector3df& pos);	

private:	

	bool PathFindingEnabled;
	
	u32 PathFindCalcCount;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CPathFinderManagerHPP