//|-------------------------------------------------------------------------
//| File:        CImage.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageHPP
#define CImageHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <img/IImage.h>

//----------------------------------------------------------------------------
namespace my {
namespace img {
//----------------------------------------------------------------------------

//! IImage implementation
class CImage : public IImage
{
public:       

	CImage();
    CImage(E_COLOR_FORMAT format, const core::dimension2di &size);
    CImage(E_COLOR_FORMAT format, const core::dimension2di &size,
		void *data, bool own_data = true);
    CImage(E_COLOR_FORMAT format, const core::dimension2di &size,
		u32 data_size, void *data, bool own_data = true);
    CImage(E_COLOR_FORMAT format, IImage* imageToCopy);
    CImage(IImage *imageToCopy, const core::position2di &pos,
		const core::dimension2di &size);
    virtual ~CImage();

	virtual void* getData() { return getLevelData(); }
	virtual const void* getData() const { return getLevelData(); }
	virtual s32 getDataSizeBytes() const { return getLevelDataSizeBytes(); }

	virtual void* getLevelData(u32 level = 0, u32 face = 0) { return m_Data; }
	virtual const void* getLevelData(u32 level = 0, u32 face = 0) const { return m_Data; }
	virtual u32 getLevelDataSizeBytes(u32 level = 0) const { return m_DataSize; }
	virtual u32 getLevelCount() const { return 1; }
	virtual u32 getFaceCount() const { return 0; }

	virtual const core::dimension2di& getDimension() const { return m_Dim; }

	virtual u32 getDepth() const { return 0; }

	virtual bool isCompressed() const
	{
        if (getColorFormat() == ECF_DXT1 ||
			getColorFormat() == ECF_DXT3 ||
			getColorFormat() == ECF_DXT5)
			return true;
        return false;
    }

	virtual bool isCubeMap() const { return false; }

    virtual bool isVolume() const { return false; }

	virtual s32 getBytesPerPixel() const { return m_BytesPerPixel; }

    virtual SColor getPixel(s32 x, s32 y) const;
	virtual void setPixel(s32 x, s32 y, const SColor &color);

    virtual E_COLOR_FORMAT getColorFormat() const;

	virtual void operate(E_IMAGE_OPERATION op, f32 arg);
	virtual void blendWith(const IImage *img, E_IMAGE_OPERATION op);
	virtual void smoothingFilter(
		bool rmask = true, bool gmask = true, bool bmask = true, bool amask = true);
	virtual void stampingFilter();
    virtual bool convertTo(E_COLOR_FORMAT new_color_format);
    virtual bool copyTo(IImage* target, s32 x, s32 y);
    virtual bool copyTo(IImage* target, const core::position2di &pos);
    virtual bool copyTo(IImage* target, const core::position2di &pos,
		const core::rect<s32>& sourceRect, const core::recti *clipRect=0);
    virtual void fill(const SColor &color);
    virtual void floodFill(s32 startx, s32 starty, const SColor &color, const SColor &with_color);
	virtual bool requestToDecreaseSize(core::recti *newrect_out);
    virtual void drawBorder(const core::recti& r, const SColor &color);
    virtual void drawBorder(s32 x, s32 y, s32 x2, s32 y2, const SColor &color);
    virtual bool copyTo(IImage* target);
    virtual bool resizeTo(const core::dimension2di &size);
    virtual bool makeColorKey(const img::SColor &color);
    virtual bool makeColorKey(const core::position2di &colorKeyPixelPos);
    virtual bool makeNormalMap(f32 amplitude, bool swapX, bool swapY, bool hmapInAlpha);	

	virtual const c8* getOverridedFileName();	
	virtual void setOverridedFileName(const c8* fname);	
   
protected:

	void _defaultInit();

	s32 _getRedMask() { return RedMask; }
	s32 _getGreenMask() { return GreenMask; }
	s32 _getBlueMask() { return BlueMask; }
	s32 _getAlphaMask() { return AlphaMask; }

	s32 _getBitsPerPixelFromFormat();

	bool m_OwnData;

	//! fills the surface with color
    void _floodFill(s32 startx, s32 starty, 
		const SColor &color, const SColor &with_color, 
		bool *handle_flags, s32 &recurse_depth);

    //! assumes format and size has been set and creates the rest
    void _initData();

    //! exchanges two ints
    inline void _exchange(s32& a, s32& b);

    //! clips coordinates into the rect. returns false if the rects is completely outside
    //! and does not need to be drawn.
    inline bool _clipRect(const s32 targetWidth, const s32 targetHeight, s32& targetX, s32& targetY,
		s32& xInSource, s32& yInSource, s32& sourceWidth, s32& soureHeight);

    //! sets a pixel very fast and inline
    void _setPixelFast(s32 x, s32 y, s16 color)
	{ ((s16*)m_Data)[y*m_Dim.Width + x] = color; }

    void* m_Data;
	u32 m_DataSize;
    core::dimension2di m_Dim;
    s32 m_BytesPerPixel;
    E_COLOR_FORMAT m_Format;

    s32 RedMask;
    s32 GreenMask;
    s32 BlueMask;
    s32 AlphaMask;

	bool WithColorKey, NormalMap;

	//! normal map loopup 32 bit version
	inline float nml32(int x, int y, int pitch, int height, s32 *p)
	{
		if (x < 0) x = pitch-1; else if (x >= pitch) x = 0;
		if (y < 0) y = height-1; else if (y >= height) y = 0;
		return (float)(((p[(y * pitch) + x])>>16) & 0xff);
	}

	//! normal map loopup 16 bit version
	inline float nml16(int x, int y, int pitch, int height, s16 *p)
	{
		if (x < 0) x = pitch-1; else if (x >= pitch) x = 0;
		if (y < 0) y = height-1; else if (y >= height) y = 0;
		return (float)img::getLuminance(p[(y * pitch) + x]);
	}

	core::stringc m_OverridedFileName;
};

//----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//----------------------------------------------------------------------------

#endif

