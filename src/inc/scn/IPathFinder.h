//|-------------------------------------------------------------------------
//| File:        IPathFinder.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IPathFinderHPP
#define IPathFinderHPP
//----------------------------------------------------------------------------

#include <scn/ISceneNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

struct SPathCell
{
	SPathCell()
		: Position(0.0f, 0.0f, 0.0f), Indices(-1, -1),
		InsideWalkmap(false), Walkable(false) {}
	core::vector2di Indices;
	core::vector3df	Position;
	bool			InsideWalkmap, Walkable;
};

//----------------------------------------------------------------------------

class __MY_SCN_LIB_API__ IPathFinder : public ISceneNode
{
public:

	//! Constructor
	IPathFinder(E_SCENE_NODE_TYPE type, ISceneNode* parent, s32 id)
		: ISceneNode(type, parent, id) {}

	//! Destructor
    virtual ~IPathFinder() {}

    //! finding path from start to destination point, return true on success
    virtual bool findPath(
        core::vector3df start_pos, core::vector3df dest_pos, 
        core::array< core::vector3df >& output_path,
		bool force_recalc = false) = 0;

	//! return path finder type
	virtual E_PATH_FINDER_TYPE getPathFinderType() = 0;

	virtual void mapUnitPosition  (const core::vector3df& pos) = 0;
	virtual void unmapUnitPosition(const core::vector3df& pos) = 0;

	virtual void clearUnitMappings() = 0;

	virtual core::dimension2df getCellSize() = 0;

	virtual core::dimension2di getCellsDimension() = 0;

	virtual bool isWalkableCell(s32 x, s32 y) = 0;

	virtual bool isWalkablePosition(const core::vector3df &pos) = 0;
	
	virtual bool isWalkableLine(const core::vector3df &src, const core::vector3df &dst) = 0;

	virtual bool isViewableLine(const core::vector3df &src, const core::vector3df &dst) = 0;

	virtual bool isPointInside(const core::vector3df &point) = 0;

	virtual core::vector3df getPositionOffsetFor(const core::vector3df &pos) = 0;

	virtual void validatePositionToGround(core::vector3df &pos) = 0;

	virtual const SPathCell& getPathCellFromPosition(const core::vector3df &position) = 0;

	virtual const SPathCell& getPathCell(s32 x, s32 y) = 0;

	virtual core::vector2di getNearestWalkableCell(const core::vector3df &pos) = 0;

	virtual u8 getInfluence(s32 x, s32 y) = 0;	
	virtual u16 getHeightInfluence(s32 x, s32 y) = 0;	

	virtual bool isBusy(const core::vector3df& pos) = 0;	
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IPathFinderHPP