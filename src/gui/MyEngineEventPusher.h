//|-------------------------------------------------------------------------
//| File:        MyEngineEventPusher.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CCEGUIEVENTPUSHER_H_INCLUDED
#define CCEGUIEVENTPUSHER_H_INCLUDED
//---------------------------------------------------------------------------------

#include <CEGUI.h>
#include <my_inc.h>
#include <io/IInputDispatcher.h>
#include <io/ICursorControl.h>

using namespace my;

//---------------------------------------------------------------------------------
namespace CEGUI { // Start of CEGUI namespace section
//---------------------------------------------------------------------------------

class EventPusher
{
	io::IInputDispatcher *inputdisp;

public :

	EventPusher() : inputdisp(INPUT_DISPATCHER_PTR)
	{
		io::ICursorControl &cursor = CURSOR_CONTROL;
		cursor.setRelativePosition(0.5f,0.5f);
	}

	virtual ~EventPusher(){}

	bool OnEvent(const io::SEvent& event)
	{
		switch(event.EventType)
		{
		case io::EET_KEY_INPUT_EVENT :
			{
				if(event.KeyInput.Event == io::EKIE_KEY_PRESSED_DOWN)
					return OnKeyDown(event.KeyInput.Key, event.KeyInput.Char,
						event.KeyInput.Control, event.KeyInput.Shift);
				else
				if(event.KeyInput.Event == io::EKIE_KEY_PRESSED_UP)
					return OnKeyUp(event.KeyInput.Key, event.KeyInput.Char,
						event.KeyInput.Control,event.KeyInput.Shift);
			}
			break;
		case io::EET_MOUSE_INPUT_EVENT :
			return OnMouse(event.MouseInput.X,event.MouseInput.Y,
				event.MouseInput.Wheel,event.MouseInput.Event);
		default:
			break;
		}
		return false;
	}

	bool OnKeyDown(io::E_KEY_CODE key, wchar_t wch, bool ctrl, bool shift)
	{
		bool handled=false;
		CEGUI::System& cegui = CEGUI::System::getSingleton();
		handled = cegui.injectKeyDown(key);
		handled = cegui.injectChar(wch) || handled;
		return handled;
	}

	bool OnKeyUp(io::E_KEY_CODE key, wchar_t wch, bool ctrl, bool shift)
	{
		bool handled=false;
		CEGUI::System& cegui = CEGUI::System::getSingleton();
		handled = cegui.injectKeyUp(key);
		return handled;
	}

	bool OnMouse(s32 x, s32 y, f32 w, io::E_MOUSE_INPUT_EVENT e)
	{
		using namespace my;
		bool handled=false;
		switch(e)
		{
		//! Left mouse button was pressed down.
		case io::EMIE_LMOUSE_PRESSED_DOWN:
			handled=CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
			break;
		//! Right mouse button was pressed down.
		case io::EMIE_RMOUSE_PRESSED_DOWN: 			
			handled=CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
			break;
		//! Middle mouse button was pressed down.
		case io::EMIE_MMOUSE_PRESSED_DOWN:
			handled=CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
			break;
		//! Left mouse button was left up.
		case io::EMIE_LMOUSE_LEFT_UP:
			handled=CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
			break;
		//! Right mouse button was left up.
		case io::EMIE_RMOUSE_LEFT_UP:
			handled=CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
			break;
		//! Middle mouse button was left up.
		case io::EMIE_MMOUSE_LEFT_UP:
			handled=CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton);
			break;
		//! The mouse cursor changed its position.
		case io::EMIE_MOUSE_MOVED:
			handled = CEGUI::System::getSingleton().injectMousePosition(f32(x), f32(y));
			CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(f32(x), f32(y)));
			break;
		//! The mouse wheel was moved. Use Wheel value in event data to find out 
		//! in what direction and how fast.
		case io::EMIE_MOUSE_WHEEL: 
			handled=CEGUI::System::getSingleton().injectMouseWheelChange(w);
			break;
		default:
			break;
		}
		return handled;
	}
protected:
	
};

//------------------------------------------------------------------------------------
} // End of  CEGUI namespace section
//------------------------------------------------------------------------------------

#endif