//|-------------------------------------------------------------------------
//| File:        IEventReceiver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IEventReceiverHPP
#define IEventReceiverHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <io/ILogger.h>

//---------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

struct SMouseEvent
{
	s32 X, Y; // mouse positions
	s32 DeltaX, DeltaY; // mouse position change

    //! mouse wheel delta, usually 1.0 or -1.0. Only valid if event was
    //! EMIE_MOUSE_WHEEL
    f32 Wheel;

    bool Shift; // true if key shift was also pressed
    bool Control; // true if key ctrl was also pressed

    //! type of mouse event
    E_MOUSE_INPUT_EVENT Event;
};

//---------------------------------------------------------------------------

struct SKeyEvent
{
	wchar_t Char; // Character corresponding to the key (0, if not a character)
	E_KEY_CODE Key; // Key pressed    
    bool Shift; // true if shift was also pressed
    bool Control; // true if ctrl was also pressed

	//! type of key event
	E_KEY_INPUT_EVENT Event;
};

//---------------------------------------------------------------------------

struct SPaintEvent
{
	s32 X, Y;
	u32 Width, Height;
};

//---------------------------------------------------------------------------

struct SLogEvent
{
	const c8* Text;
    E_LOG_LEVEL Level;
};

//---------------------------------------------------------------------------

//! Struct for holding event data. An event can be a gui, mouse or keyboard event.
struct SEvent
{
	EEVENT_TYPE EventType;

    union
    {
        SMouseEvent MouseInput;  
        SKeyEvent KeyInput;
		SPaintEvent PaintEvent;
        SLogEvent LogEvent;
    };
};

//---------------------------------------------------------------------------

//! User Input State
struct SInputState
{
	//! constructor
	SInputState()
	{
		MouseWheel=0;
		memset(MouseStates, 0, E_MOUSE_CODE_COUNT + 1);
		memset(KeysStates, 0, E_KEY_CODE_COUNT + 1);
		memset(KeysChars,  0, E_KEY_CODE_COUNT + 1);
		MouseX = MouseY = 0;
	}

	//! mouse wheel state
	f32 MouseWheel;

	//! mouse positions;
	s32 MouseX, MouseY; 
	
	//! mouse buttons states
	bool MouseStates[E_MOUSE_CODE_COUNT + 1];

	//! key states
	bool KeysStates[E_KEY_CODE_COUNT + 1];

	//! key chars
	wchar_t KeysChars[E_KEY_CODE_COUNT + 1];
};

//---------------------------------------------------------------------------

//! Interface of an object wich can receive events.
class IEventReceiver
{
public:
    //! called if an event happened. returns true if event was processed
    virtual bool OnEvent(const SEvent &event) = 0;
};

//---------------------------------------------------------------------------

//! Interface of an object wich can receive events and capture.
class IEventCapturer
{
public:

	IEventCapturer() : m_FirstCapture(true) {}

    //! called if an event happened. returns true if event was processed
    virtual bool collectEvent(const SEvent &event)
	{
		switch (event.EventType)
		{
		case EET_MOUSE_INPUT_EVENT:
			{
				const SMouseEvent &mouse = event.MouseInput;
				switch (mouse.Event)
				{
				case EMIE_LMOUSE_PRESSED_DOWN:
					m_CapturingInputState.MouseStates[EMC_LMOUSE] = true;
					break;
				case EMIE_RMOUSE_PRESSED_DOWN:
					m_CapturingInputState.MouseStates[EMC_RMOUSE] = true;
					break;
				case EMIE_MMOUSE_PRESSED_DOWN:
					m_CapturingInputState.MouseStates[EMC_MMOUSE] = true;
					break;
				case EMIE_LMOUSE_LEFT_UP:
					m_CapturingInputState.MouseStates[EMC_LMOUSE] = false;
					break;
				case EMIE_RMOUSE_LEFT_UP:
					m_CapturingInputState.MouseStates[EMC_RMOUSE] = false;
					break;
				case EMIE_MMOUSE_LEFT_UP:
					m_CapturingInputState.MouseStates[EMC_MMOUSE] = false;
					break;
				case EMIE_MOUSE_MOVED:
					m_CapturingInputState.MouseX += mouse.DeltaX;
					m_CapturingInputState.MouseY += mouse.DeltaY;
					break;
				case EMIE_MOUSE_WHEEL:
					m_CapturingInputState.MouseWheel += mouse.Wheel;
					break;
				}
			}
			break;
		case EET_KEY_INPUT_EVENT:
			{
				const SKeyEvent &key = event.KeyInput;
				m_CapturingInputState.KeysStates[key.Key] =
					(key.Event == EKIE_KEY_PRESSED_DOWN);
			}
			break;
		default:
			return false;
		}
		return true;
	}

	//! must be called after all events collected
	virtual void captureInput()
	{
		if (m_FirstCapture)
		{
			m_FirstCapture = false;
			m_CapturingInputState.MouseX = m_InputState.MouseX;
			m_CapturingInputState.MouseY = m_InputState.MouseY;
		}
		
		m_CapturingInputState.MouseStates[EMC_MWHEEL_UP] = 
			m_CapturingInputState.MouseStates[EMC_MWHEEL_DOWN] = false;
		if (m_CapturingInputState.MouseWheel > 0.0f)
			m_CapturingInputState.MouseStates[EMC_MWHEEL_UP] = true;
		else if (m_CapturingInputState.MouseWheel < 0.0f)
			m_CapturingInputState.MouseStates[EMC_MWHEEL_DOWN] = true;

		m_InputState = m_CapturingInputState;
		m_CapturingInputState.MouseWheel = 0.0f;
	}

	//! return mouse button state
	virtual bool getMouseState(E_MOUSE_CODE mbutton)
	{ return m_InputState.MouseStates[mbutton]; }

	//! reset mouse button state
	virtual void resetMouseState(E_MOUSE_CODE mbutton)
	{
		m_InputState.MouseStates[mbutton] =
			m_CapturingInputState.MouseStates[mbutton] = false;
	}

	//! return key state
	virtual bool getKeyState(E_KEY_CODE key)
	{ return m_InputState.KeysStates[key]; }

	//! reset key state
	virtual void resetKeyState(E_KEY_CODE key)
	{
		m_InputState.KeysStates[key] =
			m_CapturingInputState.KeysStates[key] = false;
		m_InputState.KeysChars[key] =
			m_CapturingInputState.KeysChars[key] = 0;
	}

	//! return mouse wheel position
	virtual f32 getMouseWheel()
	{ return m_InputState.MouseWheel; }

	//! return mouse position
	virtual s32 getMousePosX()
	{ return m_InputState.MouseX; }

	//! return mouse position
	virtual s32 getMousePosY()
	{ return m_InputState.MouseY; }

	//! capturing user key press input event
	virtual void collectKeyPress(E_KEY_CODE key, wchar_t key_char)
	{
		m_CapturingInputState.KeysChars[key] = key_char;
		m_CapturingInputState.KeysStates[key] = true;
	}

	//! capturing user key up input event
	virtual void collectKeyUp(E_KEY_CODE key)
	{
		m_CapturingInputState.KeysChars[key] = 0;
		m_CapturingInputState.KeysStates[key] = false;
	}

	//! capturing user mouse button press input event
	virtual void collectMouseButtonPress(E_MOUSE_CODE mbutton)
	{ m_CapturingInputState.MouseStates[mbutton] = true; }

	//! capturing user mouse button up input event
	virtual void collectMouseButtonUp(E_MOUSE_CODE mbutton)
	{ m_CapturingInputState.MouseStates[mbutton] = false; }

	//! capturing user mouse wheel input event
	virtual void collectMouseWheel(f32 wheel_delta)
	{ m_CapturingInputState.MouseWheel += wheel_delta; }

	//! capturing user mouse move input event
	virtual void collectMouseMoveTo(s32 x, s32 y)
	{
		m_CapturingInputState.MouseX = x;
		m_CapturingInputState.MouseY = y;
	}

	//! return game action control state
	bool getActionControlState(game::E_GAME_ACTION action)
	{ return io::getActionControlState(this, action); }

	//! reset game action control state
	void resetActionControlState(game::E_GAME_ACTION action)
	{ io::resetActionControlState(this, action); }

protected:

	SInputState m_InputState, m_CapturingInputState;
	bool m_FirstCapture;
};

//---------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//---------------------------------------------------------------------------

#endif

