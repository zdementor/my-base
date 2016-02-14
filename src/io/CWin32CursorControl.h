//|-------------------------------------------------------------------------
//| File:        CWin32CursorControl.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CWin32CursorControlHPP
#define CWin32CursorControlHPP
//----------------------------------------------------------------------------

#include <io/ICursorControl.h>

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

//! Implementation of the win32 cursor control
class CWin32CursorControl : public ICursorControl
{
public:

	CWin32CursorControl();
	virtual ~CWin32CursorControl();

    virtual void setVisible(bool visible);
    virtual bool isVisible();

    virtual void setRelativePosition(const core::position2d<f32> &pos);
    virtual void setRelativePosition(f32 x, f32 y);
    virtual void setPosition(const core::position2d<s32> &pos);
    virtual void setPosition(s32 x, s32 y);
	virtual void setPositionOffset(const core::position2di &offset);

    virtual const core::position2di& getPosition() const;
    virtual const core::position2df& getRelativePosition() const;

	virtual void setGraphicCursor(
		vid::ITexture *tex, const core::dimension2df &size,
		const core::vector2df &offset, bool centered = true);
	virtual void resetGraphicCursor();

	virtual void setCursor(const c8* filename);
	virtual void setCursor(E_CURSOR_TYPE type);

	virtual void refreshCursor();

	virtual void setRelativeSize(const core::dimension2df &size);

	virtual const core::dimension2df& getRelativeSize() const;

	virtual const core::dimension2di& getSize() const;

	virtual const core::position2di& getPositionOffset() const;

	virtual void registerForRendering();

	virtual void setSensitivity(f32 sens );

	virtual f32 getSensitivity();

private:

	void _updateInternalCursorPosition() const;
	void _calculateCursorOffsetValue();

	struct SWin32Cursor
	{
		HCURSOR       Cursor;
		core::stringc FileName;

        bool operator < (const SWin32Cursor& other) const
        {		
            return FileName<other.FileName;
        }
	};
	
	core::array<SWin32Cursor> m_Cursors;
	s32 m_CurCursorIdx;

	mutable core::position2df m_CursorPosRel;
	mutable core::position2di m_CursorPos;
    core::dimension2d<s32> m_WindowSize;
    core::dimension2d<f32> m_InvWindowSize;
    bool m_IsVisible;
    HWND m_HWnd;
    s32 m_BorderX, m_BorderY;

	vid::ITexture *m_CursorTexture;
	core::dimension2d<f32> m_CursorRelSize;
	core::dimension2d<s32> m_CursorSize;

	bool m_CenteredCursor;
	
	core::position2di m_CursorOffset, m_CursorOveralOffset;

	f32 m_Sensitivity;
};

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

#endif

 