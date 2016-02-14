//|-------------------------------------------------------------------------
//| File:        IPathFinderManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IPathFinderManagerHPP
#define IPathFinderManagerHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <ISingleton.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class __MY_SCN_LIB_API__ IPathFinderManager :
	public IUnknown, public ISingleton<IPathFinderManager>
{
public:

	//! return path find calc count
	virtual u32 getPathFindCalcCount() = 0;

	//! zeroing path find calc counter
	virtual void zeroPathFindCalcCounter() = 0;

    //! finding path from start to destination point, return true on success
    virtual bool findPath(
        core::vector3df start_pos, core::vector3df dest_pos, 
        core::array <core::vector3df> &output_path,
		bool force_recalc = false) = 0;       

	//! adding new pathfinder to the pathfinders pool (A* pathfinder)
    virtual IPathFinder* addAStarPathFinder(
		ITerrainSceneNode* terrain, img::IImage* walkability) = 0;

	//! adding new pathfinder to the pathfinders pool (A* pathfinder)
    virtual IPathFinder* addAStarPathFinder(
		ISceneNode* parent, img::IImage* walkability) = 0;

	//! enabling/disabling our pathfinders, except default pathfinder, it always working
	virtual void setPathFindingEnabled(bool enabled) = 0;

	//! return true if our pathfinders are enabled
	virtual bool isPathFindingEnabled() = 0;

	virtual void mapUnitPosition  (const core::vector3df& pos) = 0;
	virtual void unmapUnitPosition(const core::vector3df& pos) = 0;

	virtual void clearUnitMappings() = 0;

	virtual bool isWalkablePosition(core::vector3df pos) = 0;
	
	virtual bool isWalkableLine(core::vector3df &src, core::vector3df &dst) = 0;

	virtual bool isViewableLine(core::vector3df &src, core::vector3df &dst) = 0;

	virtual void getPathCellLine(
        const core::vector3df &start_pos, const core::vector3df &dest_pos, 
        core::array <SPathCell> &output_line) = 0; 

	virtual core::vector3df getPositionOffsetFor(core::vector3df pos) = 0;

	virtual void validatePositionToGround(core::vector3df &pos) = 0;

	virtual f32 getHeightOverGround(const core::vector3df &pos) = 0;

	virtual bool isPointInsidePathFinding(const core::vector3df &pos) = 0;

	virtual core::vector3df getGroundPositionFor(const core::vector3df &_pos) = 0;

	virtual bool checkIntersectionWithRay(const core::line3df &ray) = 0;
	
	virtual bool isBusy(const core::vector3df& pos) = 0;

    //! Return singleton object
    static IPathFinderManager& getSingleton();

    //! Return pointer to singleton object
    static IPathFinderManager* getSingletonPtr();
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IPathFinderManagerHPP