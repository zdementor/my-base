//|-------------------------------------------------------------------------
//| File:        IInputDispatcher.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IInputDispatcherHPP
#define IInputDispatcherHPP
//-----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <io/IEventReceiver.h>
#include <io/SKeyMap.h>
#include <ISingleton.h>

//-----------------------------------------------------------------------------
namespace my {
namespace io {
//-----------------------------------------------------------------------------

//!	Interface class for user input handling
class __MY_IO_LIB_API__ IInputDispatcher :
	public IUnknown, public ISingleton <IInputDispatcher>, public IEventCapturer
{
public:	

	//! getting keyboard input events filter flags
	virtual bool getKeyInputFilterFlag(E_KEY_INPUT_EVENT flag) = 0;

	//! setting keyboard input events filter flags
	virtual void setKeyInputFilterFlag(E_KEY_INPUT_EVENT flag, bool value) = 0;

	//! getting mouse input events filter flags
	virtual bool getMouseInputFilterFlag(E_MOUSE_INPUT_EVENT flag) = 0;

	//! setting mouse input events filter flags
	virtual void setMouseInputFilterFlag(E_MOUSE_INPUT_EVENT flag, bool value) = 0;

	//! return input event queue size
	virtual s32 getInputEventsQueueSize() = 0;

	//! not handled events queue
	virtual const core::array<SEvent>& getInputEventsQueue() = 0;

	//! getting input type
	virtual E_INPUT_TYPE getInputType() = 0;

	//! assosiate keyboard key with game action
	virtual void mapKey(E_KEY_CODE key, game::E_GAME_ACTION action) = 0;

	//! assosiate mouse button with game action
	virtual void mapMouse(E_MOUSE_CODE mbutton, game::E_GAME_ACTION action) = 0;

	//! return key assotiated with action
	virtual E_KEY_CODE getActionKey(game::E_GAME_ACTION action) = 0;

	//! return mouse button assotiated with action
	virtual E_MOUSE_CODE getActionMouse(game::E_GAME_ACTION action) = 0;

	//! return action assotiated with key 
	virtual game::E_GAME_ACTION getKeyAction(E_KEY_CODE kcode) = 0;

	//! return action assotiated with mouse button  
	virtual game::E_GAME_ACTION getMouseAction(E_MOUSE_CODE mcode) = 0;

	//! returns current input dispatch per second value
    virtual s32 getInputPS() = 0;

	//!	Return singleton object
	static IInputDispatcher& getSingleton();

	//! Return pointer to singleton object
	static IInputDispatcher* getSingletonPtr();
};

//-----------------------------------------------------------------------------

__MY_IO_LIB_API__ IInputDispatcher *createInputDispatcher(E_INPUT_TYPE type);

//-----------------------------------------------------------------------------
} // namespace io
} // namespace my
//-----------------------------------------------------------------------------

#endif // #ifndef IInputDispatcherHPP