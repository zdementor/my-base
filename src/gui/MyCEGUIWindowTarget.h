#ifndef MyCEGUIWindowTargetHPP
#define MyCEGUIWindowTargetHPP
//----------------------------------------------------------------------------

#include <gui/MyCEGUISystem.h>
#include "MyCEGUIRenderTarget.h"

//----------------------------------------------------------------------------
namespace CEGUI {
//----------------------------------------------------------------------------

//! CEGUI::RenderTarget that targets a window, or a part of a window
class MYENGINE_GUIRENDERER_API MyCEGUIWindowTarget : public MyCEGUIRenderTarget
{
public:

    MyCEGUIWindowTarget(MyCEGUIRenderer &owner);
    virtual ~MyCEGUIWindowTarget();

    // implement parts of CEGUI::RenderTarget interface
    virtual bool isImageryCache() const;
};

//----------------------------------------------------------------------------
} // End of  CEGUI namespace section
//----------------------------------------------------------------------------

#endif  // end of guard MyCEGUIWindowTargetHPP
