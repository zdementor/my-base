#ifndef MyCEGUIGeometryBufferHPP
#define MyCEGUIGeometryBufferHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <vid/IVideoDriver.h>
#include <vid/S3DVertex.h>
#include <vid/IRenderBuffer.h>
#include <gui/MyCEGUISystem.h>
#include <CEGUIGeometryBuffer.h>
#include <CEGUIRect.h>
#include <CEGUIVector.h>

using namespace my;

//---------------------------------------------------------------------------
namespace CEGUI {
//---------------------------------------------------------------------------

class MyCEGUIRenderer;
class vid::IRenderBuffer;

//---------------------------------------------------------------------------

//! GeometryBuffer implementation for the MyEngine.
class MYENGINE_GUIRENDERER_API MyCEGUIGeometryBuffer : public GeometryBuffer
{
	friend class MyCEGUIRenderer;
public:

	// implement GeometryBuffer interface
    virtual void draw() const;
    virtual void setTranslation(const Vector3& v);
    virtual void setRotation(const Vector3& r);
    virtual void setPivot(const Vector3& p);
    virtual void setClippingRegion(const Rect& region);
    virtual void appendVertex(const Vertex& vertex);
    virtual void appendGeometry(const Vertex* const vbuff, uint vertex_count);
    virtual void setActiveTexture(Texture* texture);
    virtual void reset();
    virtual Texture* getActiveTexture() const;
    virtual uint getVertexCount() const;
    virtual uint getBatchCount() const;
    virtual void setRenderEffect(RenderEffect* effect);
    virtual RenderEffect* getRenderEffect();

protected:

	MyCEGUIGeometryBuffer(MyCEGUIRenderer &renderer);
	virtual ~MyCEGUIGeometryBuffer();

	core::list <GeometryBuffer*>::iterator m_ListIterator;

    //! update cached matrix
    void _updateMatrix() const;

	//! My CEGUI renderer we're to use.
	MyCEGUIRenderer &m_Renderer;
    //! Video driver we're to use.
    vid::IVideoDriver &m_Driver;
    //! Texture that is set as active
    Texture* m_ActiveTexture;
    //! rectangular clip region
	mutable core::rectf m_ClipRect;
    //! translation vector
    core::vector3df m_Translation;
    //! rotation vector
    core::vector3df m_Rotation;
    //! pivot point for rotation
    core::vector3df m_Pivot;
    //! RenderEffect that will be used by the GeometryBuffer
    RenderEffect* m_Effect;
    //! model matrix cache
    mutable core::matrix4 m_Matrix;
    //! true when d_matrix is valid and up to date
    mutable bool m_MatrixValid;
    //! a coefficient used to handle OpenGL / D3D projection variations.
    const float m_XViewDir;
    //! an offset applied to geometry to get correct texel to pixel mapping.
    const float m_TexelOffset;

	//! quad structure used for rendering the gui
	struct RenderQuad
	{
		RenderQuad() : texture(NULL), useAlphaBlending(true) {}
		core::rectf rect_draw;
		core::rectf rect_tc;
		img::SColor colors[4];
		vid::ITexture *texture;
		bool useAlphaBlending;
	};

	RenderQuad m_DummyQuad;

	// list ot quads we want to render
	mutable core::array <RenderQuad> m_Quads;

	mutable bool m_ClipRectChanged, m_WasReseted;

	static f32 s_NextZ;
};

//----------------------------------------------------------------------------
} // End of  CEGUI namespace section
//----------------------------------------------------------------------------

#endif  // end of guard MyCEGUIGeometryBufferHPP
