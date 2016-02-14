//|-------------------------------------------------------------------------
//| File:        ICursorControl.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ICursorControlHPP
#define ICursorControlHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <ISingleton.h>

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

//! Interface to manipulate the mouse cursor.
class __MY_IO_LIB_API__ ICursorControl :
	public IUnknown, public ISingleton<ICursorControl>
{
public:	

    //! Changes the visible state of the mouse cursor.
    //! \param visible: The new visible state. If true, the cursor will be visible,
    //! if false, it will be invisible.
    virtual void setVisible(bool visible) = 0;

    //! Returns if the cursor is currently visible.
    //! \return Returns true if the cursor is visible, false if not.
    virtual bool isVisible() = 0;

    //! Sets the new position of the cursor.
    //! \param pos: New position of the cursor. The position must be between
    //! (0.0f, 0.0f) and (1.0f, 1.0f), where (0.0f, 0.0f) is 
    //! the top left corner and (1.0f, 1.0f) is the bottom right corner of the
    //! render window.
    virtual void setRelativePosition(const core::position2d<f32> &pos) = 0;

    //! Sets the new position of the cursor.
    //! \param pos: New position of the cursor. The position must be between
    //! (0.0f, 0.0f) and (1.0f, 1.0f), where (0.0f, 0.0f) is 
    //! the top left corner and (1.0f, 1.0f) is the bottom right corner of the
    //! render window.
    virtual void setRelativePosition(f32 x, f32 y) = 0;

    //! Sets the new position of the cursor.
    //! \param pos: New position of the cursor. The coordinates are pixel units.
    virtual void setPosition(const core::position2d<s32> &pos) = 0;

    //! Sets the new position of the cursor.
    //! \param pos: New position of the cursor. The coordinates are pixel units.
    virtual void setPosition(s32 x, s32 y) = 0;

	//! sets cursor position offset
	virtual void setPositionOffset(const core::position2di &offset) = 0;

    //! Returns the current position of the mouse cursor.
    //! \return Returns the current position of the cursor. The returned position
    //! is the position of the mouse cursor in pixel units.
    virtual const core::position2di& getPosition() const = 0;

    //! Returns the current position of the mouse cursor.
    //! \return Returns the current position of the cursor. The returned position
    //! is a value between (0.0f, 0.0f) and (1.0f, 1.0f), where (0.0f, 0.0f) is 
    //! the top left corner and (1.0f, 1.0f) is the bottom right corner of the
    //! render window.
    virtual const core::position2df& getRelativePosition() const = 0;

	//! setting cursor
	virtual void setGraphicCursor(vid::ITexture *tex, 
		const core::dimension2df &size, const core::vector2df &offset,
		bool centered = true) = 0;

	//! resetting cursor
	virtual void resetGraphicCursor() = 0;

	//! setting cursor from file
	virtual void setCursor(const c8* filename) = 0;

	//! setting cursor by type
	virtual void setCursor(E_CURSOR_TYPE type) = 0;	

	//! refreshing cursor
	virtual void refreshCursor() = 0;

	//! setting cursor relative size
	virtual void setRelativeSize(const core::dimension2df &size) = 0;

	//! getting cursor relative size
	virtual const core::dimension2df& getRelativeSize() const = 0;

	//! getting cursor  size
	virtual const core::dimension2di& getSize() const = 0;	

	//! return cursor position offset
	virtual const core::position2di& getPositionOffset() const = 0;

	//! draw cursor on the frame, using video driver 2D functions
	virtual void registerForRendering() = 0;

	//! setting cursor sensitivity (0.0 - min, 1.0 - max)
	virtual void setSensitivity(f32 sens ) = 0;

	//! getting cursor sensetivity (0.0 - min, 1.0 - max)
	virtual f32 getSensitivity() = 0;

	//!	Return singleton object
	static ICursorControl& getSingleton();

	//! Return pointer to singleton object
	static ICursorControl* getSingletonPtr();
};

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

#endif

