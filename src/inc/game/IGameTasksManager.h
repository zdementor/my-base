//|-------------------------------------------------------------------------
//| File:        IGameTasksManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameTasksManagerHPP
#define IGameTasksManagerHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <ISingleton.h>
#include <game/GameTasks.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Tasks Manager
class __MY_GAME_LIB_API__ IGameTasksManager : public IUnknown, public ISingleton<IGameTasksManager>
{
public:

	//! destructor
	virtual ~IGameTasksManager() {};

	//! register game action in the actions pool
	virtual void registerGameTask(SGameTask *gtask) = 0;

	virtual void clearGameTasks() = 0;

	//! validating game tasks pool
	virtual void validateGameTasks() = 0;

	//! executing game tasks pool
	virtual void performGameTasks(bool defered) = 0;

	virtual void removeCompletedTasks() = 0;

	//! calling on deleting game node
	virtual void onDeleteNode(IGameNode* gnode) = 0;

	//! returns game tasks count
    virtual s32 getGameTasksCount() = 0;	

	//!	Return singleton object
	static IGameTasksManager& getSingleton();

	//! Return pointer to singleton object
	static IGameTasksManager* getSingletonPtr();	
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameTasksManagerHPP