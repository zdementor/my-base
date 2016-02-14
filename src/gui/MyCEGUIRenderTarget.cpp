
#include "MyCEGUIRenderTarget.h"
#include "MyCEGUIGeometryBuffer.h"

//----------------------------------------------------------------------------
namespace CEGUI {
//----------------------------------------------------------------------------

MyCEGUIRenderTarget::MyCEGUIRenderTarget(MyCEGUIRenderer &owner)
	: m_Owner(owner), m_Driver(vid::IVideoDriver::getSingleton()),
    m_Area(0, 0, 0, 0),
    m_MatrixValid(false),
    m_XViewDir((m_Driver.getDriverFamily() != vid::EDF_OPENGL) ? 1.0f : -1.0f)
{
}

//----------------------------------------------------------------------------

MyCEGUIRenderTarget::~MyCEGUIRenderTarget()
{
}

//----------------------------------------------------------------------------

void MyCEGUIRenderTarget::draw(const GeometryBuffer& buffer)
{
    buffer.draw();
}

//----------------------------------------------------------------------------

void MyCEGUIRenderTarget::draw(const RenderQueue& queue)
{
    queue.draw();
}

//----------------------------------------------------------------------------

void MyCEGUIRenderTarget::setArea(const Rect& area)
{
    m_Area = area;
    m_MatrixValid = false;
}

//----------------------------------------------------------------------------

const Rect& MyCEGUIRenderTarget::getArea() const
{
    return m_Area;
}

//----------------------------------------------------------------------------

void MyCEGUIRenderTarget::activate()
{}

//----------------------------------------------------------------------------

void MyCEGUIRenderTarget::deactivate()
{}

//----------------------------------------------------------------------------

void MyCEGUIRenderTarget::unprojectPoint(
	const GeometryBuffer& buff, const Vector2& p_in, Vector2& p_out) const
{
}

//----------------------------------------------------------------------------

void MyCEGUIRenderTarget::updateMatrix() const
{
    const f32 w = m_Area.getWidth();
    const f32 h = m_Area.getHeight();
    const f32 aspect = w / h;
    const f32 midx = w * 0.5f;
    const f32 midy = h * 0.5f;
    m_ViewDistance = midx / (aspect * 0.267949192431123f);

    m_Matrix.buildProjectionMatrixPerspectiveRH(
		0.523598776f, aspect,
        m_ViewDistance * 0.5f,
        m_ViewDistance * 2.0f);

    core::matrix4 tmp;
    tmp.buildViewMatrixRH(
        core::vector3df(m_XViewDir * midx, midy, -m_ViewDistance),
        core::vector3df(m_XViewDir * midx, midy, 1),
        core::vector3df(0, -1, 0));

    m_Matrix *= tmp;

    m_MatrixValid = true;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
