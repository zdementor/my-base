//|-------------------------------------------------------------------------
//| File:        CWin32CursorControl.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CWin32CursorControl.h"

#include <dev/IDevice.h>
#include <vid/IVideoDriver.h>
#include <io/ILogger.h>

//---------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

//! constructor
CWin32CursorControl::CWin32CursorControl() 
	: 
m_IsVisible(true), m_InvWindowSize(0.0, 0.0), m_BorderX(0), m_BorderY(0),
m_CursorTexture(NULL), m_CenteredCursor(true), 
m_CursorOffset(0.0f, 0.0f), m_CursorOveralOffset(0.0f, 0.0f),
m_CursorSize(1, 1), m_Sensitivity(0.25f), m_CurCursorIdx(-1)
{
#if MY_DEBUG_MODE  
	setClassName("CWin32CursorControl");
#endif

	const dev::SExposedDeviceData& data = 
		DEVICE.getExposedDeviceData();

	bool fullscreen = data.Video.FullScreen;
	m_WindowSize    = data.Video.WindowSize;	
	m_HWnd          = reinterpret_cast<HWND>(data.Video.Win32.HWnd);	
		
    if (m_WindowSize.Width!=0)
		m_InvWindowSize.Width = 1.0 / m_WindowSize.Width;

    if (m_WindowSize.Height!=0)
		m_InvWindowSize.Height = 1.0 / m_WindowSize.Height;

	setRelativeSize(core::dimension2d<f32>(0.1f, 0.1f));

	if (!fullscreen)
    {
		m_BorderX = GetSystemMetrics(SM_CXDLGFRAME);
        m_BorderY = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME);
    }
}

//-------------------------------------------------------------------------------

//! destructor
CWin32CursorControl::~CWin32CursorControl()
{
	SAFE_DROP(m_CursorTexture);
}

//-------------------------------------------------------------------------------

//! Changes the visible state of the mouse cursor.
void CWin32CursorControl::setVisible(bool visible)
{
	if (!m_CursorTexture)
	{
		if (m_IsVisible!=visible)
		{
			m_IsVisible = visible;
			ShowCursor(m_IsVisible);
		}
	}
	else
	{
		m_IsVisible = visible;
	}
}

//-------------------------------------------------------------------------------

//! Returns if the cursor is currently visible.
bool CWin32CursorControl::isVisible()
{
	return m_IsVisible;
}

//-------------------------------------------------------------------------------

//! Sets the new position of the cursor.
void CWin32CursorControl::setRelativePosition(const core::position2d<f32> &pos)
{
	setRelativePosition(pos.X, pos.Y);
}

//-------------------------------------------------------------------------------

//! Sets the new position of the cursor.
void CWin32CursorControl::setRelativePosition(f32 x, f32 y)
{
	setPosition((s32)(x*m_WindowSize.Width+0.5f), (s32)(y*m_WindowSize.Height+0.5f));
}

//-------------------------------------------------------------------------------

//! Sets the new position of the cursor.
void CWin32CursorControl::setPosition(const core::position2d<s32> &pos)
{
	setPosition(pos.X, pos.Y);
}

//-------------------------------------------------------------------------------

//! Sets the new position of the cursor.
void CWin32CursorControl::setPosition(s32 x, s32 y)
{
	m_CursorPos.X = x;
	m_CursorPos.Y = y;

	RECT rect;

	if (GetWindowRect(m_HWnd, &rect))
		SetCursorPos(m_CursorPos.X + rect.left + m_BorderX, m_CursorPos.Y + rect.top + m_BorderY);
}

//-------------------------------------------------------------------------------

//! sets cursor position offset
void CWin32CursorControl::setPositionOffset(const core::position2di &offset)
{
	m_CursorOffset.set(offset.X, offset.Y);
	_calculateCursorOffsetValue();
}

//-------------------------------------------------------------------------------

const core::position2di& CWin32CursorControl::getPosition() const
{
	_updateInternalCursorPosition();
	return m_CursorPos;
}

//-------------------------------------------------------------------------------

const core::position2df& CWin32CursorControl::getRelativePosition() const
{
	_updateInternalCursorPosition();
	return m_CursorPosRel;
}

//-------------------------------------------------------------------------------

void CWin32CursorControl::_updateInternalCursorPosition() const
{
	POINT p;
	GetCursorPos(&p);
	RECT rect;
	GetWindowRect(m_HWnd, &rect);
	m_CursorPos.set(
		p.x - rect.left - m_BorderX,
		p.y - rect.top - m_BorderY);
	m_CursorPosRel.set(
		(f32)(m_CursorPos.X * m_InvWindowSize.Width),
        (f32)(m_CursorPos.Y * m_InvWindowSize.Height));
}

//----------------------------------------------------------------------------

void CWin32CursorControl::_calculateCursorOffsetValue()
{
	if (!m_CenteredCursor)
	{
		m_CursorOveralOffset = m_CursorOffset;
	}
	else
	{
		m_CursorOveralOffset = 
			m_CursorOffset + 
			core::position2d<s32>(-m_CursorSize.Width/2.0f, -m_CursorSize.Height/2.0f);
	}
}

//----------------------------------------------------------------------------

//! setting cursor
void CWin32CursorControl::setGraphicCursor(
	vid::ITexture *tex, 
	const core::dimension2df &size, const core::vector2df &offset, bool centered
	)
{
	if (!m_CursorTexture && m_IsVisible)
		ShowCursor(false);

	SAFE_DROP(m_CursorTexture);

	m_CursorTexture = tex;
	setRelativeSize(size);
	m_CursorOffset.set(offset.X*m_WindowSize.Width, offset.Y*m_WindowSize.Height);
	m_CenteredCursor = centered;

	_calculateCursorOffsetValue();	

	SAFE_GRAB(m_CursorTexture);
}

//----------------------------------------------------------------------------

void CWin32CursorControl::resetGraphicCursor()
{
	if (m_CursorTexture && m_IsVisible)
		ShowCursor(true);

	SAFE_DROP(m_CursorTexture);

	m_CenteredCursor = true; 
	m_CursorOveralOffset.set(0,0);
	m_CursorOffset.set(0.0f, 0.0f);
	m_CursorSize.set(1, 1);
}

//----------------------------------------------------------------------------

void CWin32CursorControl::setCursor(const c8* filename)
{
	SWin32Cursor is;
    if (!filename)
        filename = "";

    is.FileName = filename;
    is.FileName.make_lower();

    s32 index = m_Cursors.binary_search(is);
    if (index != -1)
	{
		m_CurCursorIdx = index;
	}
	else
	{
		is.Cursor = (HCURSOR)LoadImage(
			0, 	                 // handle of the instance that contains the image
			is.FileName.c_str(), // name or identifier of image
			IMAGE_CURSOR,	     // type of image
			0,	                 // desired width
			0,	                 // desired height
			LR_LOADFROMFILE	     // load flags
			);

		if (is.Cursor)
		{
			m_Cursors.push_back(is);
			m_CurCursorIdx = m_Cursors.size()-1;
		}
		else
		{
			LOGGER.logErr("Cant load cursor %s", is.FileName.c_str());
			m_CurCursorIdx = -1;
		}
	}
	refreshCursor();
}

//----------------------------------------------------------------------------

static const c8* Win32Cursor[E_CURSOR_TYPE_COUNT] =
{
	IDC_APPSTARTING,
	IDC_ARROW,
	IDC_CROSS,
	IDC_HAND,
	IDC_HELP,
	IDC_IBEAM,
	IDC_NO,
	IDC_SIZEALL,
	IDC_SIZENESW,
	IDC_SIZENS,
	IDC_SIZENWSE,
	IDC_SIZEWE,
	IDC_UPARROW,
	IDC_WAIT,
};

void CWin32CursorControl::setCursor(E_CURSOR_TYPE type)
{
	SWin32Cursor is;

	is.FileName = CursorTypeName[type];
    is.FileName.make_lower();

    s32 index = m_Cursors.binary_search(is);
    if (index != -1)
	{
		m_CurCursorIdx = index;
	}
	else
	{
		is.Cursor = LoadCursor(NULL, Win32Cursor[type]);
		if (is.Cursor)
		{
			m_Cursors.push_back(is);
			m_CurCursorIdx = m_Cursors.size()-1;
		}
		else
		{
			LOGGER.logErr("Cant load cursor %s", is.FileName.c_str());
			m_CurCursorIdx = -1;
		}
	}
	refreshCursor();
}

//----------------------------------------------------------------------------

//! refreshing cursor
void CWin32CursorControl::refreshCursor()
{
	if (m_CurCursorIdx!=-1)
	{
		SetCursor(m_Cursors[m_CurCursorIdx].Cursor);
	}
}

//----------------------------------------------------------------------------

//! setting cursor relative size
void CWin32CursorControl::setRelativeSize(const core::dimension2df &size)
{
	m_CursorRelSize = size;

	m_CursorSize.Width  = m_CursorRelSize.Width*m_WindowSize.Width;	
	m_CursorSize.Height = m_CursorRelSize.Height*m_WindowSize.Height;

	_calculateCursorOffsetValue();
}

//----------------------------------------------------------------------------

//! getting cursor relative size
const core::dimension2df& CWin32CursorControl::getRelativeSize() const
{
	return m_CursorRelSize;
}

//----------------------------------------------------------------------------

//! getting cursor  size
const core::dimension2di& CWin32CursorControl::getSize() const
{
	return m_CursorSize;
}

//----------------------------------------------------------------------------

//! returen cursor position offset
const core::position2di& CWin32CursorControl::getPositionOffset() const
{
	return m_CursorOffset;
}

//----------------------------------------------------------------------------

//! draw cursor on the frame, using video driver 2D functions
void CWin32CursorControl::registerForRendering()
{
	if (m_CursorTexture && m_IsVisible)
	{
		vid::IVideoDriver &driver = VIDEO_DRIVER;

		core::dimension2di size      = m_CursorTexture->getSize();
		core::dimension2di orig_size = m_CursorTexture->getOriginalSize();

		core::position2d<s32> cursor_pos = m_CursorPos + m_CursorOveralOffset;

		driver.registerGUIImageForRendering (
			m_CursorTexture, 1.0f,
			core::rectf(cursor_pos.X, cursor_pos.Y, 
				cursor_pos.X + m_CursorSize.Width, cursor_pos.Y + m_CursorSize.Height),
			core::rectf ( 0.0f, 0.0f, 1.0f, 1.0f ),
			0xffffffff, false, true);
	}
	
	_updateInternalCursorPosition();
}

//----------------------------------------------------------------------------

//! setting cursor sensitivity (0.0 - min, 1.0 - max)
void CWin32CursorControl::setSensitivity(f32 sens )
{
	m_Sensitivity = sens;

	CHECK_RANGE(m_Sensitivity, 0.01f, 1.0f);
}

//----------------------------------------------------------------------------

//! getting cursor sensitivity (0.0 - min, 1.0 - max)
f32 CWin32CursorControl::getSensitivity()
{
	return m_Sensitivity;
}

//----------------------------------------------------------------------------

io::ICursorControl* createCursorControl()
{
	return new CWin32CursorControl();
}

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------