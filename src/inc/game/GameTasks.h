//|-------------------------------------------------------------------------
//| File:        GameTasks.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef GameTasksHPP
#define GameTasksHPP
//----------------------------------------------------------------------------

#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

struct SGameTask
{
	friend void deleteGameTask(SGameTask* gt);

protected:	

	virtual ~SGameTask() 
	{
	}

public:

	SGameTask (IGameNode *sender=NULL, IGameNode *receiver=NULL) 
		: Sender(sender), Receiver(receiver), Completed(false), Handled(false),
	SenderIterator(0), ReceiverIterator(0), Defered(false)
	{
	}	
	
	virtual E_GAME_TASK_TYPE getType() = 0;

	IGameNode *Sender, *Receiver;	
	bool Completed, Handled, Defered;

	core::list< core::list< SGameTask* >::iterator >::iterator SenderIterator;
	core::list< core::list< SGameTask* >::iterator >::iterator ReceiverIterator;
};

//----------------------------------------------------------------------------

struct SGameTaskActivate : public SGameTask
{
protected:

	virtual ~SGameTaskActivate() 
	{
	}

public:

	SGameTaskActivate(IGameNode *sender=NULL, IGameNode *receiver=NULL) 
		: SGameTask(sender, receiver), Delay(0) 
	{		
	}	

	virtual E_GAME_TASK_TYPE getType() { return EGTT_ACTIVATE; }

	s32 Delay;
};

//----------------------------------------------------------------------------

struct SGameTaskLoadScene : public SGameTask
{
protected:

	virtual ~SGameTaskLoadScene() 
	{
	}

public:

	enum E_SCENE_LOAD_PHASE	{ ESLF_SHOW_WAIT_WINDOW=0, ESLF_LOAD_SCENE=1 };

	SGameTaskLoadScene(IGameNode *sender=NULL, IGameNode *receiver=NULL) 
		: SGameTask(sender, receiver), Phase(ESLF_SHOW_WAIT_WINDOW), FileName(NONAME_FILE_NAME)
	{
		Defered = true;
	}	

	virtual E_GAME_TASK_TYPE getType() { return EGTT_LOAD_SCENE; }

	core::stringc FileName;
	E_SCENE_LOAD_PHASE Phase; 
};

//----------------------------------------------------------------------------

struct SGameTaskHited : public SGameTask
{
protected:

	virtual ~SGameTaskHited() 
	{
	}

public:

	SGameTaskHited (IGameNode *sender=NULL, IGameNode *receiver=NULL)
		: SGameTask (sender, receiver), HitPower(0)
	{
	}	

	virtual E_GAME_TASK_TYPE getType() { return EGTT_HITED; }

	s32 HitPower;
};

//----------------------------------------------------------------------------

struct SGameTaskDie : public SGameTask
{
protected:

	virtual ~SGameTaskDie() 
	{
	}

public:

	SGameTaskDie (IGameNode *sender=NULL, IGameNode *receiver=NULL)
		: SGameTask (sender, receiver), DieTimeMs(0)
	{
	}	

	virtual E_GAME_TASK_TYPE getType() { return EGTT_DIE; }

	s32 DieTimeMs;
};

//----------------------------------------------------------------------------

struct SGameTaskFindPathMove : public SGameTask
{
protected:

	virtual ~SGameTaskFindPathMove() 
	{
	}

public:

	SGameTaskFindPathMove(IGameNode *sender=NULL, IGameNode *receiver=NULL) 
		: SGameTask(sender, receiver), NeedToRepath(true)
	{
	}
	
	virtual E_GAME_TASK_TYPE getType() { return EGTT_FIND_PATH_AND_MOVE; }

	core::vector3df DestPos; 
	bool NeedToRepath;
};

//----------------------------------------------------------------------------

struct SGameTaskOrient : public SGameTask
{
protected:

	virtual ~SGameTaskOrient() 
	{
	}

public:

	SGameTaskOrient(IGameNode *sender=NULL, IGameNode *receiver=NULL) 
		: SGameTask(sender, receiver), OrientDir(0,0,0)
	{
	}	
	
	virtual E_GAME_TASK_TYPE getType() { return EGTT_ORIENT; }

	core::vector3df OrientDir; 
};

//----------------------------------------------------------------------------

struct SGameTaskAttack : public SGameTask
{
protected:

	virtual ~SGameTaskAttack() 
	{
	}

public:

	SGameTaskAttack(IGameNode *sender=NULL, IGameNode *receiver=NULL) 
		: SGameTask(sender, receiver), Victim(0), CanMeleAttack(false)
	{
	}	
	
	virtual E_GAME_TASK_TYPE getType() { return EGTT_ATTACK; }

	IGameNode *Victim; 
	core::vector3df VectorAttack;
	bool CanMeleAttack;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef GameTasksHPP 