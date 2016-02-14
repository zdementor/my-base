//|-------------------------------------------------------------------------
//| File:        CGameTasksManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameTasksManagerHPP
#define CGameTasksManagerHPP
//----------------------------------------------------------------------------

#include <game/IGameTasksManager.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Tasks Manager
class __MY_GAME_LIB_API__ CGameTasksManager : public IGameTasksManager
{
public:

	//! constructor
	CGameTasksManager();

	//! destructor
	virtual ~CGameTasksManager();
	
	//! register game action in the actions pool
	virtual void registerGameTask(SGameTask *gtask);

	virtual void clearGameTasks();

	//! validating game tasks pool
	virtual void validateGameTasks();

	//! executing game tasks pool
	virtual void performGameTasks(bool defered);
	
	virtual void removeCompletedTasks();

	//! calling on deleting game node
	virtual void onDeleteNode(IGameNode* gnode);

	//! returns game tasks count
    virtual s32 getGameTasksCount();	

private:

	core::list<SGameTask*> NewGameTasksPool[E_GAME_TASK_TYPE_COUNT];
	core::list<SGameTask*> GameTasksPool[E_GAME_TASK_TYPE_COUNT];

	s32 GameTasksCount;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameTasksManagerHPP