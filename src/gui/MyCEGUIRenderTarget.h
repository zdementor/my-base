#ifndef MyCEGUIRenderTargetHPP
#define MyCEGUIRenderTargetHPP
//----------------------------------------------------------------------------

#include <gui/MyCEGUISystem.h>
#include <CEGUIRenderTarget.h>
#include <CEGUIRect.h>

#include <my_inc.h>

using namespace my;

//----------------------------------------------------------------------------
namespace CEGUI {
//----------------------------------------------------------------------------

class MyCEGUIRenderer;
class vid::IVideoDriver;

//! Intermediate RenderTarget implementing common parts for Irrlicht engine.
class MYENGINE_GUIRENDERER_API MyCEGUIRenderTarget : public virtual RenderTarget
{
public:

    MyCEGUIRenderTarget(MyCEGUIRenderer &owner);
    virtual ~MyCEGUIRenderTarget();

    // implement common parts of RenderTarget interface
    virtual void draw(const GeometryBuffer& buffer);
    virtual void draw(const RenderQueue& queue);
    virtual void setArea(const Rect& area);
    virtual const Rect& getArea() const;
    virtual void activate();
    virtual void deactivate();
    virtual void unprojectPoint(
		const GeometryBuffer& buff, const Vector2& p_in, Vector2& p_out) const;

protected:

    //! helper that initialises the cached matrix
    void updateMatrix() const;

    //! Renderer object that owns this RenderTarget
    MyCEGUIRenderer& m_Owner;
    //! video driver we are using
	vid::IVideoDriver& m_Driver;
    //! holds defined area for the RenderTarget
    Rect m_Area;
    //! projection / view matrix cache
    mutable core::matrix4 m_Matrix;
    //! true when d_matrix is valid and up to date
    mutable bool m_MatrixValid;
    //! tracks viewing distance (this is set up at the same time as d_matrix)
    mutable float m_ViewDistance;
    //! a coefficient used to handle OpenGL / D3D projection variations.
    const float m_XViewDir;
};

//----------------------------------------------------------------------------
} // End of  CEGUI namespace section
//----------------------------------------------------------------------------

#endif  // end of guard MyCEGUIRenderTargetHPP
