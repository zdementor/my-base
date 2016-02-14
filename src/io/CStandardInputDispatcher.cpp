//|-------------------------------------------------------------------------
//| File:        CStandardInputDispatcher.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CStandardInputDispatcher.h"

#include <dev/IDevice.h>
#include <vid/IVideoDriver.h>
#include <os/ITimer.h>

//---------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

ICursorControl* createCursorControl();

//---------------------------------------------------------------------------

CStandardInputDispatcher::CStandardInputDispatcher() 
	: Device(DEVICE_PTR)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CStandardInputDispatcher");
#endif
	for (s32 ke=0; ke<E_KEY_INPUT_EVENT_COUNT; ke++)
		m_KeyInputFilterFlags[ke] = false;
	for (s32 me=0; me<E_MOUSE_INPUT_EVENT_COUNT; me++)
		m_MouseInputFilterFlags[me] = false;
	for (s32 k=0; k<game::E_GAME_ACTION_COUNT; k++)
		m_ActionKey[k] = E_KEY_CODE_COUNT;
	for (s32 m=0; m<game::E_GAME_ACTION_COUNT; m++)
		m_ActionMButton[m] = E_MOUSE_CODE_COUNT;
	for (s32 k=0; k<E_KEY_CODE_COUNT; k++)
		m_KeyAction[k] = game::E_GAME_ACTION_COUNT;
	for (s32 m=0; m<E_MOUSE_CODE_COUNT; m++)
		m_MButtonAction[m] = game::E_GAME_ACTION_COUNT;

	Driver = VIDEO_DRIVER_PTR;
	Driver->grab();

	CursorControl = createCursorControl();
}

//---------------------------------------------------------------------------

CStandardInputDispatcher::~CStandardInputDispatcher()
{	
	SAFE_DROP(CursorControl);
	SAFE_DROP(Driver);
}

//---------------------------------------------------------------------------

bool CStandardInputDispatcher::getKeyInputFilterFlag(
	E_KEY_INPUT_EVENT flag)
{
	if (flag<0 || flag>=E_KEY_INPUT_EVENT_COUNT)
		return false;
	return m_KeyInputFilterFlags[flag];
}

//---------------------------------------------------------------------------

void CStandardInputDispatcher::setKeyInputFilterFlag(
	E_KEY_INPUT_EVENT flag, bool value)
{
	if (flag<0 || flag>=E_KEY_INPUT_EVENT_COUNT)
		return;
	m_KeyInputFilterFlags[flag] = value;
}

//---------------------------------------------------------------------------

bool CStandardInputDispatcher::getMouseInputFilterFlag(
	E_MOUSE_INPUT_EVENT flag)
{
	if (flag<0 || flag>=E_MOUSE_INPUT_EVENT_COUNT)
		return false;
	return m_MouseInputFilterFlags[flag];
}

//---------------------------------------------------------------------------

void CStandardInputDispatcher::setMouseInputFilterFlag(
	E_MOUSE_INPUT_EVENT flag, bool value)
{
	if (flag<0 || flag>=E_MOUSE_INPUT_EVENT_COUNT)
		return;
	m_MouseInputFilterFlags[flag] = value;
}

//----------------------------------------------------------------------------

void CStandardInputDispatcher::captureInput()
{
	InputPSCounter.registerEvent(TIMER_SECOND.getTime());

	IEventCapturer::captureInput();
	
	// mouse wheel
	f32 wheel = getMouseWheel();
	if (wheel != 0.0f)
	{
		SEvent event;
		event.EventType = EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = EMIE_MOUSE_WHEEL;	
		event.MouseInput.Wheel = wheel;
		event.MouseInput.Shift = getKeyState(EKC_LSHIFT) || getKeyState(EKC_RSHIFT);
		event.MouseInput.Control = getKeyState(EKC_LCONTROL) || getKeyState(EKC_RCONTROL);
		event.MouseInput.X = getMousePosX();
		event.MouseInput.Y = getMousePosY();

		m_CapturingEvents.push_back(event);
	}

	m_Events = m_CapturingEvents;
	m_CapturingEvents.set_used(0);
}

//----------------------------------------------------------------------------

void CStandardInputDispatcher::collectKeyPress(E_KEY_CODE key, wchar_t key_char)
{
	IEventCapturer::collectKeyPress(key, key_char);

	SEvent event;
	event.EventType = EET_KEY_INPUT_EVENT;
	event.KeyInput.Event = io::EKIE_KEY_PRESSED_DOWN;

	event.KeyInput.Key = key;
	event.KeyInput.Char = key_char;
	event.KeyInput.Shift = getKeyState(EKC_LSHIFT) || getKeyState(EKC_RSHIFT);
	event.KeyInput.Control = getKeyState(EKC_LCONTROL) || getKeyState(EKC_RCONTROL);

	m_CapturingEvents.push_back(event);
}

//----------------------------------------------------------------------------

void CStandardInputDispatcher::collectKeyUp(E_KEY_CODE key)
{
	IEventCapturer::collectKeyUp(key);

	SEvent event;
	event.EventType = EET_KEY_INPUT_EVENT;
	event.KeyInput.Event = io::EKIE_KEY_PRESSED_UP;

	event.KeyInput.Key = key;
	event.KeyInput.Char = 0;
	event.KeyInput.Shift = getKeyState(EKC_LSHIFT) || getKeyState(EKC_RSHIFT);
	event.KeyInput.Control = getKeyState(EKC_LCONTROL) || getKeyState(EKC_RCONTROL);

	m_CapturingEvents.push_back(event);
}

//----------------------------------------------------------------------------

void CStandardInputDispatcher::collectMouseButtonPress(E_MOUSE_CODE mbutton)
{
	IEventCapturer::collectMouseButtonPress(mbutton);

	SEvent event;
	event.EventType = EET_MOUSE_INPUT_EVENT;
	do
	{
		if (mbutton == EMC_LMOUSE)
			event.MouseInput.Event = EMIE_LMOUSE_PRESSED_DOWN;
		else 
		if (mbutton == EMC_MMOUSE)
			event.MouseInput.Event = EMIE_MMOUSE_PRESSED_DOWN;
		else 
		if (mbutton == EMC_RMOUSE) 
			event.MouseInput.Event = EMIE_RMOUSE_PRESSED_DOWN;
		else
			break;

		event.MouseInput.Shift = getKeyState(EKC_LSHIFT) || getKeyState(EKC_RSHIFT);
		event.MouseInput.Control = getKeyState(EKC_LCONTROL) || getKeyState(EKC_RCONTROL);

		event.MouseInput.X = getMousePosX();
		event.MouseInput.Y = getMousePosY();
		event.MouseInput.DeltaX = 0;
		event.MouseInput.DeltaY = 0;

		m_CapturingEvents.push_back(event);
	}
	while (false);
}

//----------------------------------------------------------------------------

void CStandardInputDispatcher::collectMouseButtonUp(E_MOUSE_CODE mbutton)
{
	IEventCapturer::collectMouseButtonUp(mbutton);

	SEvent event;
	event.EventType = EET_MOUSE_INPUT_EVENT;
	do
	{
		if (mbutton == EMC_LMOUSE)
			event.MouseInput.Event = EMIE_LMOUSE_LEFT_UP;
		else 
		if (mbutton == EMC_MMOUSE)
			event.MouseInput.Event = EMIE_MMOUSE_LEFT_UP;
		else 
		if (mbutton == EMC_RMOUSE) 
			event.MouseInput.Event = EMIE_RMOUSE_LEFT_UP;
		else
			break;

		event.MouseInput.Shift = getKeyState(EKC_LSHIFT) || getKeyState(EKC_RSHIFT);
		event.MouseInput.Control = getKeyState(EKC_LCONTROL) || getKeyState(EKC_RCONTROL);

		event.MouseInput.X = getMousePosX();
		event.MouseInput.Y = getMousePosY();
		event.MouseInput.DeltaX = 0;
		event.MouseInput.DeltaY = 0;

		m_CapturingEvents.push_back(event);
	}
	while (false);
}

//----------------------------------------------------------------------------

void CStandardInputDispatcher::collectMouseWheel(f32 wheel_delta)
{
	IEventCapturer::collectMouseWheel(wheel_delta);
}

//----------------------------------------------------------------------------

void CStandardInputDispatcher::collectMouseMoveTo(s32 x, s32 y)
{
	IEventCapturer::collectMouseMoveTo(x, y);

	SEvent event;
	event.EventType        = EET_MOUSE_INPUT_EVENT;
	event.MouseInput.Event = EMIE_MOUSE_MOVED;

	event.MouseInput.Shift = getKeyState(EKC_LSHIFT) || getKeyState(EKC_RSHIFT);
	event.MouseInput.Control = getKeyState(EKC_LCONTROL) || getKeyState(EKC_RCONTROL);

	event.MouseInput.X = x;
	event.MouseInput.Y = y;
	event.MouseInput.DeltaX = x - getMousePosX();
	event.MouseInput.DeltaY = y - getMousePosY();

	m_CapturingEvents.push_back(event);
}

//----------------------------------------------------------------------------

void CStandardInputDispatcher::mapKey(
	E_KEY_CODE key, game::E_GAME_ACTION action)
{
	E_KEY_CODE old_key             = m_ActionKey[action];
	game::E_GAME_ACTION old_action = m_KeyAction[key];
	if (old_key < E_KEY_CODE_COUNT)
		m_KeyAction[old_key] = game::E_GAME_ACTION_COUNT;
	if (old_action < game::E_GAME_ACTION_COUNT)
		m_ActionKey[old_action] = E_KEY_CODE_COUNT;
	m_ActionKey[action] = key;
	m_KeyAction[key]    = action;
}

//----------------------------------------------------------------------------

void CStandardInputDispatcher::mapMouse(
	E_MOUSE_CODE mbutton, game::E_GAME_ACTION action)
{
	E_MOUSE_CODE old_mbtn          = m_ActionMButton[action];
	game::E_GAME_ACTION old_action = m_MButtonAction[mbutton];
	if (old_mbtn<  E_MOUSE_CODE_COUNT)
		m_MButtonAction[old_mbtn] = game::E_GAME_ACTION_COUNT;
	if (old_action < game::E_GAME_ACTION_COUNT)
		m_ActionMButton[old_action] = E_MOUSE_CODE_COUNT;
	m_ActionMButton[action]  = mbutton;
	m_MButtonAction[mbutton] = action;
}

//----------------------------------------------------------------------------

__MY_EXPORT__ IInputDispatcher* createInputDispatcher(io::E_INPUT_TYPE inputType)
{
	IInputDispatcher *disp = NULL;
	if (inputType == io::EIT_STANDARD_INPUT)
		disp = new CStandardInputDispatcher();
	if (!disp)
	// creating standard input Dispatcher
	{
		LOGGER.logWarn("Can't create Direct Input Dispatcher, fall back to standard.");
		disp = new CStandardInputDispatcher();
	}
	return disp;
}

//-----------------------------------------------------------------------------
} // namespace io
} // namespace my
//-----------------------------------------------------------------------------