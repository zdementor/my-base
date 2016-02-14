//|-------------------------------------------------------------------------
//| File:        CStandardInputDispatcher.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CStandardInputDispatcherHPP
#define CStandardInputDispatcherHPP
//-----------------------------------------------------------------------------

#include <io/IInputDispatcher.h>
#include <io/SKeyMap.h>
#include <io/ICursorControl.h>

//-----------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

//! Base class for user input handling
class CStandardInputDispatcher : public IInputDispatcher
{
public:

	CStandardInputDispatcher();
	virtual ~CStandardInputDispatcher();

	virtual bool getKeyInputFilterFlag(E_KEY_INPUT_EVENT flag);
	virtual void setKeyInputFilterFlag(E_KEY_INPUT_EVENT flag, bool value);

	virtual bool getMouseInputFilterFlag(E_MOUSE_INPUT_EVENT flag);
	virtual void setMouseInputFilterFlag(E_MOUSE_INPUT_EVENT flag, bool value);

	virtual s32 getInputEventsQueueSize()
	{ return m_Events.size(); }

	virtual const core::array<SEvent>& getInputEventsQueue()
	{ return m_Events; }

	virtual E_INPUT_TYPE getInputType()
	{ return EIT_STANDARD_INPUT; }

	virtual void mapKey(E_KEY_CODE key, game::E_GAME_ACTION action);

	virtual void mapMouse(E_MOUSE_CODE mbutton, game::E_GAME_ACTION action);

	virtual E_KEY_CODE getActionKey(game::E_GAME_ACTION action)
	{ return m_ActionKey[action]; }

	virtual E_MOUSE_CODE getActionMouse(game::E_GAME_ACTION action)
	{ return m_ActionMButton[action]; }

	virtual game::E_GAME_ACTION getKeyAction(E_KEY_CODE kcode)
	{ return m_KeyAction[kcode]; }

	virtual game::E_GAME_ACTION getMouseAction(E_MOUSE_CODE mcode)
	{ return m_MButtonAction[mcode]; }

    virtual s32 getInputPS()
	{ return InputPSCounter.getEventPerSecond(); }

	// IEventCapturer
	virtual void captureInput();

	virtual void collectKeyPress(E_KEY_CODE key, wchar_t key_char);
	virtual void collectKeyUp(E_KEY_CODE key);
	virtual void collectMouseButtonPress(E_MOUSE_CODE mbutton);
	virtual void collectMouseButtonUp(E_MOUSE_CODE mbutton);
	virtual void collectMouseWheel(f32 wheel_delta);
	virtual void collectMouseMoveTo(s32 x, s32 y);

protected:

	E_KEY_CODE		m_ActionKey    [game::E_GAME_ACTION_COUNT];
	E_MOUSE_CODE	m_ActionMButton[game::E_GAME_ACTION_COUNT];

	game::E_GAME_ACTION m_KeyAction    [E_KEY_CODE_COUNT];
	game::E_GAME_ACTION m_MButtonAction[E_MOUSE_CODE_COUNT];

	core::array <SEvent> m_Events, m_CapturingEvents;

	dev::IDevice *Device;
	vid::IVideoDriver* Driver;
	io::ICursorControl* CursorControl;

	core::dimension2d <s32> mScreenSize;

	core::math::event_counter InputPSCounter;

	u8 MouseSensetivity;
	bool InverseMouse;

	bool m_MouseInputFilterFlags[E_MOUSE_INPUT_EVENT_COUNT];
	bool m_KeyInputFilterFlags[E_KEY_INPUT_EVENT_COUNT];

private:

	bool m_FirstCapture;		
};

//-----------------------------------------------------------------------------
} // namespace io
} // namespace my
//-----------------------------------------------------------------------------
#endif // #ifndef CStandardInputDispatcherHPP
