#include "MyCEGUIWindowTarget.h"
#include <vid/IVideoDriver.h>

//----------------------------------------------------------------------------
namespace CEGUI {
//----------------------------------------------------------------------------

MyCEGUIWindowTarget::MyCEGUIWindowTarget(MyCEGUIRenderer &owner)
	: MyCEGUIRenderTarget(owner)
{
    core::dimension2di sz(m_Driver.getScreenSize());

    const Rect init_area(0, 0,
		static_cast<float>(sz.Width),
        static_cast<float>(sz.Height));
    setArea(init_area);
}

//----------------------------------------------------------------------------

MyCEGUIWindowTarget::~MyCEGUIWindowTarget() {}

//----------------------------------------------------------------------------

bool MyCEGUIWindowTarget::isImageryCache() const
{ return false; }

//----------------------------------------------------------------------------
} // End of  CEGUI namespace section
//----------------------------------------------------------------------------
