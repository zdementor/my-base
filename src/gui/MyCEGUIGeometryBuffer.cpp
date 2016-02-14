#include "MyCEGUIGeometryBuffer.h"
#include "MyCEGUITexture.h"

#include <io/ILogger.h>

#include <CEGUIRenderEffect.h>
#include <CEGUIVertex.h>

//----------------------------------------------------------------------------
namespace CEGUI {
//----------------------------------------------------------------------------

f32 MyCEGUIGeometryBuffer::s_NextZ = 1.0f;

MyCEGUIGeometryBuffer::MyCEGUIGeometryBuffer(MyCEGUIRenderer &renderer)
	: m_ListIterator(0), m_Renderer(renderer), m_Driver(vid::IVideoDriver::getSingleton()),
m_ActiveTexture(0), m_Translation(0, 0, 0), m_Rotation(0, 0, 0),
m_Pivot(0, 0, 0), m_Effect(0), m_MatrixValid(false),
m_XViewDir((m_Driver.getDriverFamily() != vid::EDF_OPENGL) ? 1.0f : -1.0f),
m_TexelOffset((m_Driver.getDriverFamily() != vid::EDF_OPENGL) ? /*-0.5f*/ 0.0f : 0.0f),
m_ClipRectChanged(true), m_WasReseted(false)
{
}

//----------------------------------------------------------------------------

MyCEGUIGeometryBuffer::~MyCEGUIGeometryBuffer() {}

//----------------------------------------------------------------------------

void MyCEGUIGeometryBuffer::draw() const
{
	u32 qcnt = m_Quads.size();
	if (!qcnt)
		return;
	if (!m_ClipRect.isValid())
		return;

	if (!m_MatrixValid)
		_updateMatrix();

	core::vector3df ul(0.0f, 0.0f, 0.0f);
	core::vector3df lr(0.0f, 0.0f, 0.0f);
	core::rectf clipped_rect_draw;
	core::rectf clipped_rect_tc;

	for (u32 i = 0; i < qcnt; i++)
	{
		RenderQuad &quad = m_Quads[i];

		ul.X = quad.rect_draw.UpperLeftCorner.X;
		ul.Y = quad.rect_draw.UpperLeftCorner.Y;
		lr.X = quad.rect_draw.LowerRightCorner.X;
		lr.Y = quad.rect_draw.LowerRightCorner.Y;
		m_Matrix.transformVect(ul);
		m_Matrix.transformVect(lr);

		clipped_rect_draw.set(ul.X, ul.Y, lr.X, lr.Y);
		clipped_rect_draw.clipAgainst(m_ClipRect);
		if (!clipped_rect_draw.isValid())
			continue;

		f32 rd_w =  quad.rect_draw.getWidth();
		f32 rd_h =  quad.rect_draw.getHeight();

		f32 rtc_w =  quad.rect_tc.getWidth();
		f32 rtc_h =  quad.rect_tc.getHeight();

		f32 clip_ul_x = (clipped_rect_draw.UpperLeftCorner.X - ul.X) / rd_w;
		f32 clip_ul_y = (clipped_rect_draw.UpperLeftCorner.Y - ul.Y) / rd_h;
		f32 clip_lr_x = (clipped_rect_draw.LowerRightCorner.X - lr.X) / rd_w;
		f32 clip_lr_y = (clipped_rect_draw.LowerRightCorner.Y - lr.Y) / rd_h;

		clipped_rect_tc.set(
			quad.rect_tc.UpperLeftCorner.X + clip_ul_x * rtc_w,
			quad.rect_tc.UpperLeftCorner.Y + clip_ul_y * rtc_h,
			quad.rect_tc.LowerRightCorner.X + clip_lr_x * rtc_w,
			quad.rect_tc.LowerRightCorner.Y + clip_lr_y * rtc_h);

		m_Driver.registerGUIImageForRendering(
			quad.texture, s_NextZ, clipped_rect_draw, clipped_rect_tc,
			quad.colors[0], quad.colors[1], quad.colors[2], quad.colors[3], true);
	}

	s_NextZ -= 0.000001f;
}

//----------------------------------------------------------------------------

void MyCEGUIGeometryBuffer::setTranslation(const Vector3& v)
{
    m_Translation.X = v.d_x;
    m_Translation.Y = v.d_y;
    m_Translation.Z = v.d_z;
    m_MatrixValid = false;
}

//----------------------------------------------------------------------------

void MyCEGUIGeometryBuffer::setRotation(const Vector3& r)
{
    m_Rotation.X = r.d_x;
    m_Rotation.Y = r.d_y;
    m_Rotation.Z = r.d_z;
    m_MatrixValid = false;
}

//----------------------------------------------------------------------------

void MyCEGUIGeometryBuffer::setPivot(const Vector3& p)
{
    m_Pivot.X = p.d_x;
    m_Pivot.Y = p.d_y;
    m_Pivot.Z = p.d_z;
    m_MatrixValid = false;
}

//----------------------------------------------------------------------------

void MyCEGUIGeometryBuffer::setClippingRegion(const Rect& region)
{
	core::rectf clip_rect(
		region.d_left, region.d_top, region.d_right, region.d_bottom);
	if (m_ClipRect != clip_rect)
	{
		m_ClipRect = clip_rect;
		m_ClipRectChanged = true;
	}
}

//----------------------------------------------------------------------------

void MyCEGUIGeometryBuffer::appendVertex(const Vertex& vertex)
{
    appendGeometry(&vertex, 1);
}

//----------------------------------------------------------------------------

void MyCEGUIGeometryBuffer::appendGeometry(
	const Vertex* const vbuff, uint vertex_count)
{
	if (!m_ActiveTexture || vertex_count != 6)
		return;

    // see if we should start a new batch
    vid::ITexture *tex = ((MyCEGUITexture*)m_ActiveTexture)->getTexture();
	core::dimension2di sz = tex->getSize();

	static u16 quad_indices[4] = {0, 1, 2, 5};

	RenderQuad &quad = m_DummyQuad;

	quad.rect_draw.set(
		vbuff[0].position.d_x + m_TexelOffset, vbuff[0].position.d_y + m_TexelOffset,
		vbuff[2].position.d_x + m_TexelOffset, vbuff[2].position.d_y + m_TexelOffset);
	quad.rect_tc.set(
		vbuff[0].tex_coords.d_x,
        vbuff[0].tex_coords.d_y,
		vbuff[5].tex_coords.d_x,
        vbuff[5].tex_coords.d_y);

	for (u32 i = 0; i < sizeof(m_DummyQuad.colors)/sizeof(*m_DummyQuad.colors); i++)
		quad.colors[i].color = vbuff[quad_indices[i]].colour_val.getARGB();
	quad.texture = tex;

	m_Quads.push_back(quad);
}

//----------------------------------------------------------------------------

void MyCEGUIGeometryBuffer::setActiveTexture(Texture* texture)
{
    m_ActiveTexture = texture;
}

//----------------------------------------------------------------------------

void MyCEGUIGeometryBuffer::reset()
{
	m_WasReseted = true;
    m_ActiveTexture = 0;
	m_Quads.set_used(0);

	s_NextZ = 1.0f;
}

//----------------------------------------------------------------------------

Texture* MyCEGUIGeometryBuffer::getActiveTexture() const
{ return m_ActiveTexture; }

//----------------------------------------------------------------------------

uint MyCEGUIGeometryBuffer::getVertexCount() const
{ return m_Quads.size(); }

//----------------------------------------------------------------------------

uint MyCEGUIGeometryBuffer::getBatchCount() const
{ return m_Quads.size() ? 1 : 0; }

//----------------------------------------------------------------------------

void MyCEGUIGeometryBuffer::setRenderEffect(RenderEffect* effect)
{ m_Effect = effect; }

//----------------------------------------------------------------------------

RenderEffect* MyCEGUIGeometryBuffer::getRenderEffect()
{ return m_Effect; }

//----------------------------------------------------------------------------

void MyCEGUIGeometryBuffer::_updateMatrix() const
{
    m_Matrix.makeIdentity();
    m_Matrix.setTranslation(m_Translation + m_Pivot);

    core::matrix4 rot;
    rot.setRotationDegrees(m_Rotation);
    core::matrix4 ptrans;
    ptrans.setTranslation(m_Pivot*(-1.0f));

    m_Matrix *= rot;
    m_Matrix *= ptrans;

    m_MatrixValid = true;
}

//----------------------------------------------------------------------------
} // End of  CEGUI namespace section
//----------------------------------------------------------------------------
