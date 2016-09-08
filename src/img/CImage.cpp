//|-------------------------------------------------------------------------
//| File:        CImage.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CImage.h"
#include <io/ILogger.h>
#include <img/IImageLibrary.h>

//----------------------------------------------------------------------------
namespace my {
namespace img {
//----------------------------------------------------------------------------

CImage::CImage() 
{  
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CImage");
#endif
	_defaultInit();
}

//----------------------------------------------------------------------------

CImage::CImage(E_COLOR_FORMAT format, const core::dimension2di &size) 
{  
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CImage");
#endif
	_defaultInit();
	m_Format = format;
	m_Dim = size;
    _initData();
}

//----------------------------------------------------------------------------

CImage::CImage(E_COLOR_FORMAT format,
	const core::dimension2di &size, void* data, bool own_data)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CImage");
#endif
	_defaultInit();
	m_Format = format;
	m_Dim = size;
	m_OwnData = own_data;
    m_Data = (void*)0xbadf00d;
    _initData();
    m_Data = data;
}

//----------------------------------------------------------------------------

CImage::CImage(E_COLOR_FORMAT format,
	const core::dimension2di &size, u32 data_size, void* data, bool own_data)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CImage");
#endif
	_defaultInit();
	m_Format  = format;
	m_Dim = size;
	m_OwnData = own_data;
	m_DataSize = data_size;
    m_Data = (void*)0xbadf00d;
    _initData();
    m_Data = data;
}

//----------------------------------------------------------------------------

CImage::CImage(E_COLOR_FORMAT format, IImage* imageToCopy)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CImage");
#endif
	_defaultInit();
	m_Format = format;
    if (!imageToCopy)
        return;
    m_Dim = imageToCopy->getDimension();
    _initData();

    // now copy data from other image

    if (m_Format == imageToCopy->getColorFormat())
    {
        memcpy(m_Data, imageToCopy->getData(), imageToCopy->getDataSizeBytes());
    }
    else
    if (m_Format == ECF_A1R5G5B5)
        for (s32 x=0; x<(s32)m_Dim.Width; ++x)
            for (s32 y=0; y<(s32)m_Dim.Height; ++y)
                ((s16*)m_Data)[y*m_Dim.Width + x] = imageToCopy->getPixel(x,y).toA1R5G5B5();
    else
        LOGGER.log("CImage: Unsupported format to copy from.", io::ELL_ERROR);
}

//----------------------------------------------------------------------------

CImage::CImage(IImage *imageToCopy, const core::position2di &pos,
	const core::dimension2di &size)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CImage");
#endif
	_defaultInit();
	m_Dim = size;
    if (!imageToCopy)
        return;
    m_Format = imageToCopy->getColorFormat();
    m_Dim = size;
    _initData();

    // copy image part
    c8* data = (c8*)m_Data;
    c8* source = (c8*)imageToCopy->getData();
    s32 bytes = imageToCopy->getBytesPerPixel();
    s32 srcpitch = imageToCopy->getDimension().Width * bytes;
    s32 tgtpitch = m_Dim.Width*bytes;

    for (s32 x=0; x<size.Width; ++x)
	{
        for (s32 y=0; y<size.Height; ++y)
        {
            for (s32 b=0; b<bytes; ++b)
                data[y*tgtpitch + (x*bytes) + b] =
                    source[(y+pos.Y)*srcpitch + ((pos.X+x)*bytes) + b];
        }
	}
}

//----------------------------------------------------------------------------

void CImage::_defaultInit()
{
	m_Data = NULL;
	WithColorKey = false;
	NormalMap = false;
	m_OverridedFileName = "";
	m_OwnData = false;
	m_DataSize = 0;
	m_BytesPerPixel = 0;
	m_Dim.set(0, 0);
	m_Format = ECF_A8B8G8R8;
	RedMask = GreenMask = BlueMask = AlphaMask = 0;
}

//----------------------------------------------------------------------------

void CImage::_initData()
{
    m_BytesPerPixel = _getBitsPerPixelFromFormat() / 8;
	m_DataSize = m_DataSize ?
		m_DataSize : (m_Dim.Height * m_Dim.Width * m_BytesPerPixel);
    if (!m_Data)
	{
        m_Data = new u8[m_DataSize];
		memset(m_Data, 0x00, m_DataSize);
		m_OwnData = true;
	}
}

//----------------------------------------------------------------------------

CImage::~CImage()
{   
	if (m_Data && m_OwnData)
		delete [](u8*)m_Data;
}

//----------------------------------------------------------------------------

s32 CImage::_getBitsPerPixelFromFormat()
{
    switch(m_Format)
    {
    case ECF_A1R5G5B5:
        AlphaMask = 0x1<<15;
        RedMask = 0x1F<<10;
        GreenMask = 0x1F<<5;
        BlueMask = 0x1F;
        break;
    case ECF_R5G6B5:
        AlphaMask = 0x0;
        RedMask = 0x1F<<11;
        GreenMask = 0x3F<<5;
        BlueMask = 0x1F;
        break;
    case ECF_R8G8B8:
        AlphaMask = 0x0;
        RedMask = 0xFF<<16;
        GreenMask = 0xFF<<8;
        BlueMask = 0xFF;
        break;
    case ECF_A8R8G8B8:
        AlphaMask = 0xFF<<24;
        RedMask = 0xFF<<16;
        GreenMask = 0xFF<<8;
        BlueMask = 0xFF;
        break;
    case ECF_A8B8G8R8:
        AlphaMask = 0xFF<<24;
        BlueMask = 0xFF<<16;
        GreenMask = 0xFF<<8;
        RedMask = 0xFF;
        break;
    case ECF_DEPTH16:
        AlphaMask = 0;
        BlueMask = 0;
        GreenMask = 0xFF<<8;
        RedMask = 0xFF;
        break;
    case ECF_ALPHA32F:
        AlphaMask = 0xFF<<24;
        BlueMask = 0xFF<<16;
        GreenMask = 0xFF<<8;
        RedMask = 0xFF;
        break;
    }
    return getBitsPerPixelFromFormat(m_Format);
}

//----------------------------------------------------------------------------

SColor CImage::getPixel(s32 x, s32 y) const
{
    if (x < 0 || y < 0 || x >= m_Dim.Width || y >= m_Dim.Height)
        return SColor(0);

    switch(m_Format)
    {
    case ECF_A1R5G5B5:
        return A1R5G5B5toA8R8G8B8(((s16*)m_Data)[y*m_Dim.Width + x]);
    case ECF_R5G6B5:
        return R5G6B5toA8R8G8B8(((s16*)m_Data)[y*m_Dim.Width + x]);
    case ECF_A8R8G8B8:
        return ((s32*)m_Data)[y*m_Dim.Width + x];		
	case ECF_A8B8G8R8:
		return A8R8G8B8toA8B8G8R8(((s32*)m_Data)[y*m_Dim.Width + x]);
    case ECF_R8G8B8:
        {
            u8* p = &((u8*)m_Data)[(y*3)*m_Dim.Width + (x*3)];
            return SColor(255,p[0],p[1],p[2]);
        }
   case ECF_DEPTH16:
        {
            u8 *p = (u8*)&((s16*)m_Data)[y*m_Dim.Width + x];
            return SColor(0,0,p[1],p[0]);
        }
	default:
		LOGGER.logErr("%s - Incorrect operation with image format %s.",
			__FUNCTION__, img::getColorFormatName(m_Format));
    }
    return SColor(0);
}

//----------------------------------------------------------------------------

void CImage::setPixel(s32 x, s32 y, const SColor &color)
{   
	if (x < 0 || y < 0 || x >= m_Dim.Width || y >= m_Dim.Height)
        return;

    switch(m_Format)
    {
    case ECF_A1R5G5B5:
		{
		s16 *p = &((s16*)m_Data)[y*m_Dim.Width + x];
		*p = A8R8G8B8toA1R5G5B5(color.color);
		}
		break;
    case ECF_R5G6B5:
		{
		s16 *p = &((s16*)m_Data)[y*m_Dim.Width + x];
		*p = RGB16(color.getRed(), color.getGreen(), color.getBlue());
		}
		break;
    case ECF_A8R8G8B8:
		{
        s32 *p = &((s32*)m_Data)[y*m_Dim.Width + x];
		*p = color.color;
		}
		break;
    case ECF_A8B8G8R8:
		{
        s32 *p = &((s32*)m_Data)[y*m_Dim.Width + x];
		*p = A8B8G8R8toA8R8G8B8(color.color);
		}
		break;
    case ECF_R8G8B8:
		{
        u8 *p = &((u8*)m_Data)[(y*3)*m_Dim.Width + (x*3)];
        p[0]=color.getRed();
		p[1]=color.getGreen();
		p[2]=color.getBlue();       
		}
		break;
	default:
		LOGGER.logErr("%s - Incorrect operation with image format %s.",
			__FUNCTION__, img::getColorFormatName(m_Format));
    }
}

//----------------------------------------------------------------------------

void CImage::operate(E_IMAGE_OPERATION op, f32 arg)
{
	u8* data = (u8*)getData();

    const core::dimension2di &dim = getDimension(); 

    int size = dim.Width * dim.Height * getBytesPerPixel();

	int p=0;

	switch(m_Format)
    {
    case ECF_A1R5G5B5:
    case ECF_R5G6B5:
    case ECF_A8R8G8B8:
    case ECF_A8B8G8R8:
    case ECF_R8G8B8:
		if (op==EIO_MUL)
		{
			for (; p<size; p++)
			{   
				u8 b  = (u8)data[p];
				s32 res = f32(b)*arg;
				CHECK_RANGE(res, 0, 255);
				data[p] = (u8)res;  
			}
		}
		else
		if (op==EIO_ADD)
		{
			for (; p<size; p++)
			{   
				u8 b  = (u8)data[p];
				s32 res = b + s32(arg);
				CHECK_RANGE(res, 0, 255);
				data[p] = (u8)res;  
			}
		}
		else
		if (op==EIO_SUB)
		{
			for (; p<size; p++)
			{   
				u8 b  = (u8)data[p];
				s32 res = b - s32(arg);
				CHECK_RANGE(res, 0, 255);
				data[p] = (u8)res;  
			}
		}
		break;
	default:
		LOGGER.logErr("%s - Incorrect operation with image format %s.",
			__FUNCTION__, img::getColorFormatName(m_Format));
    }
}

//----------------------------------------------------------------------------

void CImage::blendWith(const IImage *img, E_IMAGE_OPERATION op)
{	
	if (!img)
	{	
		LOGGER.log("CImage: Can not blend with null image!", io::ELL_ERROR);
		return;
	}

    u8* data1 = (u8*)getData();
    const u8* data2 = (const u8*)img->getData();

    core::dimension2d<s32> dim1 = getDimension(); 
    core::dimension2d<s32> dim2 = img->getDimension();

    int size1 = dim1.Width * dim1.Height * getBytesPerPixel();
    int size2 = dim2.Width * dim2.Height * img->getBytesPerPixel();

    int p=0;

    if (size1 != size2)
    {   
		LOGGER.logErr("To merge, images has to be the same width, height and bpp!");
        return;;
    }

	switch(m_Format)
    {
    case ECF_A1R5G5B5:
    case ECF_R5G6B5:
    case ECF_A8R8G8B8:
    case ECF_A8B8G8R8:
    case ECF_R8G8B8:
		if (op==EIO_MUL)
		{
			for (; p<size1; p++)
			{   
				u8 b1  = (u8)data1[p];
				u8 b2 = (u8)data2[p];
				u32 res = b1*b2/255;
				CHECK_RANGE(res, 0, 255);
				data1[p] = (u8)res;  
			}
		}
		else
		if (op==EIO_ADD)
		{
			for (; p<size1; p++)
			{   
				u8 b1 = (u8)data1[p];
				u8 b2 = (u8)data2[p];	
				u32 res = b1 + b2;
				CHECK_RANGE(res, 0, 255);
				data1[p] = (u8)res;  
			}
		}
		else
		if (op==EIO_SUB)
		{
			for (; p<size1; p++)
			{   
				u8 b1  = (u8)data1[p];
				u8 b2 = (u8)data2[p];	
				s32 res = b1 - b2;
				CHECK_RANGE(res, 0, 255);
				data1[p] = (u8)res;  
			}
		}
		break;
	default:
		LOGGER.logErr("%s - Incorrect operation with image format %s.",
			__FUNCTION__, img::getColorFormatName(m_Format));
    }
}

//----------------------------------------------------------------------------

void CImage::smoothingFilter(bool rmask, bool gmask, bool bmask, bool amask)
{	
	s32 filter[3][3] = {{1,1,1}, {1,0,1}, {1,1,1}};
	
	SColor c00, c01, c02, c10, c11, c12, c20, c21, c22;
	SColor res;
	s32 r, g, b, a;

	switch(m_Format)
    {
    case ECF_A1R5G5B5:
    case ECF_R5G6B5:
    case ECF_A8R8G8B8:
    case ECF_A8B8G8R8:
    case ECF_R8G8B8:
		break;
	default:
		LOGGER.logErr("%s - Incorrect operation with image format %s.",
			__FUNCTION__, img::getColorFormatName(m_Format));
		return;
	}

	for (s32 w=1; w<m_Dim.Width-1; w++)
	{
		for (s32 h=1; h<m_Dim.Height-1; h++)
		{
			c00 = getPixel(w-1, h-1);
			c01 = getPixel(w  , h-1);
			c02 = getPixel(w+1, h-1);
			c10 = getPixel(w-1, h  );					
			c11 = getPixel(w  , h  );
			c12 = getPixel(w+1, h  );
			c20 = getPixel(w-1, h+1);
			c21 = getPixel(w  , h+1);
			c22 = getPixel(w+1, h+1);

			if (rmask)
				r =(c00.getRed()*filter[0][0]+
					c01.getRed()*filter[0][1]+
					c02.getRed()*filter[0][2]+
					c10.getRed()*filter[1][0]+
					c11.getRed()*filter[1][1]+
					c12.getRed()*filter[1][2]+
					c20.getRed()*filter[2][0]+
					c21.getRed()*filter[2][1]+
					c22.getRed()*filter[2][2])/8;
			else
				r = c11.getRed();
			if (gmask)
				g =(c00.getGreen()*filter[0][0]+
					c01.getGreen()*filter[0][1]+
					c02.getGreen()*filter[0][2]+
					c10.getGreen()*filter[1][0]+
					c11.getGreen()*filter[1][1]+
					c12.getGreen()*filter[1][2]+
					c20.getGreen()*filter[2][0]+
					c21.getGreen()*filter[2][1]+
					c22.getGreen()*filter[2][2])/8;
			else
				g = c11.getGreen();
			if (bmask)
				b =(c00.getBlue()*filter[0][0]+
					c01.getBlue()*filter[0][1]+
					c02.getBlue()*filter[0][2]+
					c10.getBlue()*filter[1][0]+
					c11.getBlue()*filter[1][1]+
					c12.getBlue()*filter[1][2]+
					c20.getBlue()*filter[2][0]+
					c21.getBlue()*filter[2][1]+
					c22.getBlue()*filter[2][2])/8;
			else
				b = c11.getBlue();
			if (amask)
				a =(c00.getAlpha()*filter[0][0]+
					c01.getAlpha()*filter[0][1]+
					c02.getAlpha()*filter[0][2]+
					c10.getAlpha()*filter[1][0]+
					c11.getAlpha()*filter[1][1]+
					c12.getAlpha()*filter[1][2]+
					c20.getAlpha()*filter[2][0]+
					c21.getAlpha()*filter[2][1]+
					c22.getAlpha()*filter[2][2])/8;
			else
				a = c11.getAlpha();
			
			res = SColor(a, r, g, b);

			setPixel(w, h, res);
		}
	}
}

//----------------------------------------------------------------------------

void CImage::stampingFilter()
{	
	s32 filter[3][3] = {{1,1,1}, {1,0,-1}, {-1,-1,-1}};

	SColor c00, c01, c02, c10, c11, c12, c20, c21, c22;
	SColor res;
	s32 r, g, b;

	switch(m_Format)
    {
    case ECF_A1R5G5B5:
    case ECF_R5G6B5:
    case ECF_A8R8G8B8:
    case ECF_A8B8G8R8:
    case ECF_R8G8B8:
		break;
	default:
		LOGGER.logErr("%s - Incorrect operation with image format %s.",
			__FUNCTION__, getColorFormatName(m_Format));
		return;
	}
				
	for (s32 w=1; w<m_Dim.Width-1; w++)
	{
		for (s32 h=1; h<m_Dim.Height-1; h++)
		{
			c00 = getPixel(w-1, h-1);
			c01 = getPixel(w  , h-1);
			c02 = getPixel(w+1, h-1);
			c10 = getPixel(w-1, h  );					
			c11 = getPixel(w  , h  );
			c12 = getPixel(w+1, h  );
			c20 = getPixel(w-1, h+1);
			c21 = getPixel(w  , h+1);
			c22 = getPixel(w+1, h+1);

			r =(c00.getRed()*filter[0][0]+
				c01.getRed()*filter[0][1]+
				c02.getRed()*filter[0][2]+
				c10.getRed()*filter[1][0]+
				c11.getRed()*filter[1][1]+
				c12.getRed()*filter[1][2]+
				c22.getRed()*filter[2][0]+
				c22.getRed()*filter[2][1]+
				c22.getRed()*filter[2][2]
				)/8;
			g =(c00.getGreen()*filter[0][0]+
				c01.getGreen()*filter[0][1]+
				c02.getGreen()*filter[0][2]+
				c10.getGreen()*filter[1][0]+
				c11.getGreen()*filter[1][1]+
				c12.getGreen()*filter[1][2]+
				c22.getGreen()*filter[2][0]+
				c22.getGreen()*filter[2][1]+
				c22.getGreen()*filter[2][2]
				)/8;
			b =(c00.getBlue()*filter[0][0]+
				c01.getBlue()*filter[0][1]+
				c02.getBlue()*filter[0][2]+
				c10.getBlue()*filter[1][0]+
				c11.getBlue()*filter[1][1]+
				c12.getBlue()*filter[1][2]+
				c22.getBlue()*filter[2][0]+
				c22.getBlue()*filter[2][1]+
				c22.getBlue()*filter[2][2]
				)/8;

//			r+=128; if (r>255) r=255;
//			g+=128; if (g>255) g=255;
//			b+=128; if (b>255) b=255;

			res = SColor(255, r, g, b);

			setPixel(w, h, res);
		}
	}	
}

//----------------------------------------------------------------------------

E_COLOR_FORMAT CImage::getColorFormat() const
{
    return m_Format;
}

//----------------------------------------------------------------------------

inline void CImage::_exchange(s32& a, s32& b)
{
    s32 tmp;
    tmp = a;
    a = b;
    b = tmp;
}

//----------------------------------------------------------------------------

void CImage::drawBorder(const core::recti &r, const SColor &color)
{
	drawBorder(r.UpperLeftCorner.X, r.UpperLeftCorner.Y,
        r.LowerRightCorner.X, r.LowerRightCorner.Y, color);   
}

//----------------------------------------------------------------------------

void CImage::drawBorder(s32 x, s32 y, s32 x2, s32 y2, const SColor &color)
{
	switch(m_Format)
    {
    case ECF_A1R5G5B5:
    case ECF_R5G6B5:
    case ECF_A8R8G8B8:
    case ECF_A8B8G8R8:
    case ECF_R8G8B8:
		break;
	default:
		LOGGER.logErr("%s - Incorrect operation with image format %s.",
			__FUNCTION__, getColorFormatName(m_Format));
		return;
	}

    // clip
    CHECK_RANGE(x, 0, m_Dim.Width-1); 
	CHECK_RANGE(x2, 0, m_Dim.Width-1);
    CHECK_RANGE(y, 0, m_Dim.Height-1);
	CHECK_RANGE(y2, 0, m_Dim.Height-1);

    // switch x and x2 if neccecary

    if (x > x2)
        _exchange(x,x2);

    if (y > y2)
        _exchange(y,y2);        

	for (s32 ix=x; ix<=x2; ++ix)
    {
		setPixel(ix, y, color);
		setPixel(ix, y2, color);
	}

	for (s32 iy=y+1; iy<y2; ++iy)
    {
		setPixel(x, iy, color);
		setPixel(x2, iy, color);
	}
}

//----------------------------------------------------------------------------

inline bool CImage::_clipRect(
	const s32 targetWidth, const s32 targetHeight, s32& targetX, s32& targetY,
	s32& xInSource, s32& yInSource, s32& sourceWidth, s32& soureHeight)
{
    if (targetX < 0)
    {
        sourceWidth += targetX;
        if (sourceWidth <= 0)
            return false;

        xInSource -= targetX;
        targetX = 0;
    }

    if (targetX+sourceWidth>targetWidth)
    {
        sourceWidth -= (targetX + sourceWidth) - targetWidth;
        if (sourceWidth <= 0)
            return false;
    }

    if (targetY<0)
    {
        soureHeight += targetY;
        if (soureHeight <= 0)
            return false;
        yInSource -= targetY;
        targetY = 0;
    }

    if (targetY+soureHeight>targetHeight)
    {
        soureHeight -= (targetY + soureHeight) - targetHeight;
        if (soureHeight <= 0)
            return false;
    }

    return true;
}

//----------------------------------------------------------------------------

bool CImage::convertTo(E_COLOR_FORMAT new_color_format)
{
	if (m_Format == new_color_format)
		return true;
	
	switch(m_Format)
    {
    case ECF_A1R5G5B5:
    case ECF_R5G6B5:
    case ECF_A8R8G8B8:
    case ECF_A8B8G8R8:
    case ECF_R8G8B8:
		break;
	default:
		LOGGER.logErr("%s - Incorrect operation with image format %s.",
			__FUNCTION__, getColorFormatName(m_Format));
		return false;
	}

    // converting

	CImage *tmp_img = new CImage(new_color_format, m_Dim);
	copyTo(tmp_img, 0, 0);

	if (m_Data) delete [] (s8*)m_Data;

	m_Format = new_color_format;
	m_Data = tmp_img->m_Data;
	m_OwnData = true;
	m_BytesPerPixel = tmp_img->getBytesPerPixel();
	m_DataSize = tmp_img->getLevelDataSizeBytes();

	tmp_img->m_OwnData = false;
	tmp_img->m_Data = NULL;

	tmp_img->drop();	

	return true;
}

//----------------------------------------------------------------------------

bool CImage::copyTo(IImage* target, const core::position2di &pos)
{
    return copyTo(target, pos.X, pos.Y);
}

//----------------------------------------------------------------------------

bool CImage::copyTo(IImage* target, s32 x, s32 y)
{
	if (!target)
		return false;

    c8* data = (c8*)target->getData();
    core::dimension2d<s32> size = target->getDimension();

    // clip

    s32 ownWidth = m_Dim.Width;
    s32 ownHeight = m_Dim.Height;
    s32 ownX = 0;
    s32 ownY = 0;

    if (!_clipRect(size.Width, size.Height, x, y, ownX, ownY, ownWidth, ownHeight))
        return false;

    // copy

	if (m_Format == target->getColorFormat())
	{
		s32 byte_per_pixel = 0;

		if (m_Format == ECF_R5G6B5)
			byte_per_pixel = 2;
		else if (m_Format == ECF_A1R5G5B5)
			byte_per_pixel = 2;
		else if (m_Format == ECF_R8G8B8)
			byte_per_pixel = 3;
		else if (m_Format == ECF_A8R8G8B8)
			byte_per_pixel = 4;
		else if (m_Format == ECF_A8B8G8R8)
			byte_per_pixel = 4;
		else
		{
			LOGGER.log("Unknown format of the CImage", io::ELL_ERROR);
			return false;
		}

		s32 ltarget=y*size.Width + x;
		s32 lown=ownY*m_Dim.Width + ownX;
		size_t dataRowSizeInBytes = m_Dim.Width*byte_per_pixel;

		for (s32 iy=0; iy<ownHeight; ++iy)
		{
			memcpy(&data[ltarget*byte_per_pixel], &((c8*)m_Data)[lown*byte_per_pixel], dataRowSizeInBytes);

			lown += m_Dim.Width;
		    ltarget += size.Width;
		}
	}
	else
	{		
		s32 ownWidth = m_Dim.Width;
		s32 ownHeight = m_Dim.Height;           

		for (s32 iy=0; iy<ownHeight; ++iy)
		{
			for (s32 ix=0; ix<ownWidth; ++ix)
			{
				target->setPixel(x+ix, y+iy, getPixel(ix, iy));
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------

bool CImage::copyTo(IImage *target,
	const core::position2di &pos, const core::recti &sourceRect,
	const core::recti *clipRect)
{
	if (!target)
		return false;

    if (sourceRect.UpperLeftCorner.X > sourceRect.LowerRightCorner.X ||
        sourceRect.UpperLeftCorner.Y > sourceRect.LowerRightCorner.Y)
        return false;

    core::position2d<s32> targetPos = pos;
    core::position2d<s32> sourcePos = sourceRect.UpperLeftCorner;
    core::dimension2d<s32> sourceSize(sourceRect.getWidth()+1, sourceRect.getHeight()+1);
    const core::dimension2d<s32> targetSurfaceSize = target->getDimension();

    if (clipRect)
    {
        if (targetPos.X < clipRect->UpperLeftCorner.X)
        {
            sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
            if (sourceSize.Width <= 0)
                return false;

            sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
            targetPos.X = clipRect->UpperLeftCorner.X;
        }

        if (targetPos.X + sourceSize.Width > clipRect->LowerRightCorner.X)
        {
            sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
            if (sourceSize.Width <= 0)
                return false;
        }

        if (targetPos.Y < clipRect->UpperLeftCorner.Y)
        {
            sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
            if (sourceSize.Height <= 0)
                return false;

            sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
            targetPos.Y = clipRect->UpperLeftCorner.Y;
        }

        if (targetPos.Y + sourceSize.Height > clipRect->LowerRightCorner.Y)
        {
            sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
            if (sourceSize.Height <= 0)
                return false;
        }
    }

    // clip these coordinates

    if (targetPos.X<0)
    {
        sourceSize.Width += targetPos.X;
        if (sourceSize.Width <= 0)
            return false;

        sourcePos.X -= targetPos.X;
        targetPos.X = 0;
    }

    if (targetPos.X + sourceSize.Width > targetSurfaceSize.Width)
    {
        sourceSize.Width -= (targetPos.X + sourceSize.Width) - targetSurfaceSize.Width;
        if (sourceSize.Width <= 0)
            return false;
    }

    if (targetPos.Y<0)
    {
        sourceSize.Height += targetPos.Y;
        if (sourceSize.Height <= 0)
            return false;

        sourcePos.Y -= targetPos.Y;
        targetPos.Y = 0;
    }

    if (targetPos.Y + sourceSize.Height > targetSurfaceSize.Height)
    {
        sourceSize.Height -= (targetPos.Y + sourceSize.Height) - targetSurfaceSize.Height;
        if (sourceSize.Height <= 0)
            return false;
    }

    // copy to

	if (m_Format == target->getColorFormat())
	{
		s32 byte_per_pixel = 0;

		if (m_Format == ECF_R5G6B5)
			byte_per_pixel = 2;
		else if (m_Format == ECF_A1R5G5B5)
			byte_per_pixel = 2;
		else if (m_Format == ECF_R8G8B8)
			byte_per_pixel = 3;
		else if (m_Format == ECF_A8R8G8B8)
			byte_per_pixel = 4;
		else if (m_Format == ECF_A8B8G8R8)
			byte_per_pixel = 4;
		else
		{
			LOGGER.log("Unknown format of the CImage", io::ELL_ERROR);
			return false;
		}

		c8* targetData = (c8*)target->getData();
		s32 ltarget = targetPos.Y * targetSurfaceSize.Width + targetPos.X;
		s32 lsource = sourcePos.Y * m_Dim.Width + sourcePos.X;
		size_t dataRowSizeInBytes = sourceSize.Width * byte_per_pixel;

		for (s32 iy=0; iy<sourceSize.Height; ++iy)
	    {
		    memcpy(&targetData[ltarget*byte_per_pixel], &((c8*)m_Data)[lsource*byte_per_pixel], dataRowSizeInBytes);
			lsource += m_Dim.Width;
		    ltarget += targetSurfaceSize.Width;
	    }
	}
	else
	{		
		for (s32 iy=sourceRect.UpperLeftCorner.Y, dy=0; iy<=sourceRect.LowerRightCorner.Y; ++iy, ++dy)
		{
			for (s32 ix=sourceRect.UpperLeftCorner.X, dx=0; ix<=sourceRect.LowerRightCorner.X; ++ix, ++dx)
			{
				target->setPixel(targetPos.X+dx, targetPos.Y+dy, getPixel(ix, iy));
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------

void CImage::fill(const SColor &color)
{
	s32 ownWidth = m_Dim.Width;
    s32 ownHeight = m_Dim.Height;  

	switch(m_Format)
    {
    case ECF_A1R5G5B5:
    case ECF_R5G6B5:
    case ECF_A8R8G8B8:
    case ECF_A8B8G8R8:
    case ECF_R8G8B8:
		break;
	default:
		LOGGER.logErr("%s - Incorrect operation with image format %s.",
			__FUNCTION__, img::getColorFormatName(m_Format));
		return;
	}

    for (s32 iy=0; iy<ownHeight; ++iy)
		for (s32 ix=0; ix<ownWidth; ++ix)
			setPixel(ix, iy, color);
}

//----------------------------------------------------------------------------

void CImage::floodFill(s32 startx, s32 starty, const SColor &color, const SColor &with_color)
{
	switch(m_Format)
    {
    case ECF_A1R5G5B5:
    case ECF_R5G6B5:
    case ECF_A8R8G8B8:
    case ECF_A8B8G8R8:
    case ECF_R8G8B8:
		break;
	default:
		LOGGER.logErr("%s - Incorrect operation with image format %s.",
			__FUNCTION__, img::getColorFormatName(m_Format));
		return;
	}

	bool *handle_flags = new bool[m_Dim.Width*m_Dim.Height];
	memset(handle_flags, 0x00, m_Dim.Width*m_Dim.Height);

	s32 recurse_depth = 0;

	_floodFill(startx, starty, color, with_color, handle_flags, recurse_depth);

	SAFE_DELETE_ARRAY(handle_flags);
}

//----------------------------------------------------------------------------

void CImage::_floodFill(s32 x, s32 y, 
	const SColor &color, const SColor &with_color, 	
	bool *handle_flags, s32 &recurse_depth)
{
	if (recurse_depth++ > 5000)
		return;

	if (x>=0 && x<m_Dim.Width && y>=0 && y<m_Dim.Height)
	{
		s32 offset = y*m_Dim.Width + x;
		if (!handle_flags[offset])
		{
			handle_flags[offset] = true;
			if (getPixel(x, y)==color)
			{
				setPixel(x, y, with_color);	
				_floodFill(x-1, y,   color, with_color, handle_flags, recurse_depth);
				_floodFill(x,   y-1, color, with_color, handle_flags, recurse_depth);
				_floodFill(x+1, y,   color, with_color, handle_flags, recurse_depth);
				_floodFill(x,   y+1, color, with_color, handle_flags, recurse_depth);
			}			
		}		
	}
	recurse_depth--;
}

//----------------------------------------------------------------------------

bool CImage::requestToDecreaseSize(core::recti *newrect_out)
{
	if (!newrect_out)
		return false;

	switch(m_Format)
    {
    case ECF_A1R5G5B5:
    case ECF_R5G6B5:
    case ECF_A8R8G8B8:
    case ECF_A8B8G8R8:
    case ECF_R8G8B8:
		break;
	default:
		LOGGER.logErr("%s - Incorrect operation with image format %s.",
			__FUNCTION__, img::getColorFormatName(m_Format));
		return false;
	}

	core::recti &newrect = *newrect_out;

	newrect = core::recti(0,0,m_Dim.Width-1, m_Dim.Height-1);

	s32 LeftOffset=0, TopOffset=0, RightOffset=0, BottomOffset=0;

	s32 ownWidth = m_Dim.Width;
    s32 ownHeight = m_Dim.Height;   

	SColor first_pixel_color = getPixel(0,0);

	s32 ix=0, iy=0;
	bool want_to_break = false;

	for (iy=0; iy<ownHeight; iy++)
	{
		for (ix=0; ix<ownWidth; ix++)
		{
			if (getPixel(ix, iy) != first_pixel_color)
			{
				want_to_break = true;
				break;			
			}
		}

		if (want_to_break)
			break;

		TopOffset++;
	}

	want_to_break = false;

	for (iy=(ownHeight-1); iy>=0; iy--)
	{
		for (ix=0; ix<ownWidth; ix++)
		{
			if (getPixel(ix, iy) != first_pixel_color)
			{
				want_to_break = true;
				break;			
			}		
		}

		if (want_to_break)
			break;

		BottomOffset++;
	}

	want_to_break = false;

	for (ix=0; ix<ownWidth; ix++)
	{
		for (iy=0; iy<ownHeight; iy++)
		{
			if (getPixel(ix, iy) != first_pixel_color)
			{
				want_to_break = true;
				break;			
			}		
		}

		if (want_to_break)
			break;

		LeftOffset++;
	}

	want_to_break = false;

	for (ix=(ownWidth-1); ix>=0; ix--)
	{
		for (iy=0; iy<ownHeight; iy++)
		{
			if (getPixel(ix, iy) != first_pixel_color)
			{
				want_to_break = true;
				break;			
			}		
		}

		if (want_to_break)
			break;

		RightOffset++;
	}

	newrect.UpperLeftCorner.X  += LeftOffset;
	newrect.UpperLeftCorner.Y  += TopOffset;
	newrect.LowerRightCorner.X -= RightOffset;
	newrect.LowerRightCorner.Y -= BottomOffset;

	return (LeftOffset!=0 || TopOffset!=0 || RightOffset!=0 || BottomOffset!=0);
}

//----------------------------------------------------------------------------

bool CImage::resizeTo(const core::dimension2di &size)
{
	if (!size.Width || !size.Height)
        return false;
	if (m_Dim == size)
        return true;

	bool res = false;
    u8 *data = new u8[size.Width * size.Height * getBytesPerPixel()];

	CImage *img = new CImage(getColorFormat(), size, data, false);
	res = IMAGE_LIBRARY.copy(this, img);
	img->drop();

	if (res)
	{
		if (m_Data && m_OwnData)
			delete [](u8 *)m_Data;

		m_OwnData = true;
		m_Dim = size;
		m_Data = data;
	}
	else
		delete [](u8 *)data;

	return res;
}

//----------------------------------------------------------------------------

bool CImage::copyTo(IImage* target)
{
	return IMAGE_LIBRARY.copy(this, target);
}

//----------------------------------------------------------------------------

bool CImage::makeColorKey(const SColor &color)
{
	if (NormalMap)
	{
		LOGGER.logErr("It's a normal map image, not for making color key.");
		return false;
	}

	if (WithColorKey)
		return true;

    if (m_Format == img::ECF_A1R5G5B5)
    {
        s16 *p = (s16*)getData();

        if (!p)
        {
            LOGGER.logErr("Could not lock image for making color key channel.");
            return false;
        }

        s16 ref = (0x0<<15) | (~(0x1<<15) & color.toA1R5G5B5());
        s16 blackalpha = (0x0<<15) | (~(0x1<<15) & 0);

		s32 width = m_Dim.Width, height = m_Dim.Height;

        for (s32 x=0; x<width; ++x)
		{
            for (s32 y=0; y<height; ++y)
            {
                s16 c = (0x0<<15) | (~(0x1<<15) & p[y*width + x]);
                p[y*width + x] = (c == ref) ? blackalpha : ((0x1<<15) | (~(0x1<<15) & c));
            }
		}
    }
    else if (m_Format == img::ECF_A8R8G8B8 || m_Format == img::ECF_A8B8G8R8)
    {
        s32 *p = (s32*)getData();

        if (!p)
        {
            LOGGER.logErr("Could not lock image for making color key channel.");
            return false;
        }

        s32 ref = (0x0<<24) | (~(0xFF<<24) & color.color);
        s32 blackalpha = (0x0<<24) | (~(0xFF<<24) & 0);

		s32 width = m_Dim.Width, height = m_Dim.Height;

        for (s32 x=0; x<width; ++x)
		{
            for (s32 y=0; y<height; ++y)
            {
                s32 c = (0x0<<24) | (~(0xFF<<24) & p[y*width + x]);
                p[y*width + x] = (c == ref) ? blackalpha : ((0xFF<<24) | (~(0xFF<<24) & c));
            }
		}
    }	
	else
	{
        LOGGER.logErr("Unsupported image color format for making color key channel.");
		return false;
	}	
	WithColorKey = true;

	return true;
}

//----------------------------------------------------------------------------

bool CImage::makeColorKey(const core::position2di &colorKeyPixelPos)
{
	return makeColorKey(getPixel(colorKeyPixelPos.X, colorKeyPixelPos.Y));
}  

//---------------------------------------------------------------------------- 

bool CImage::makeNormalMap(f32 amplitude)
{
	if (WithColorKey)
	{
		LOGGER.logErr("It's a color key image, not for making normal map.");
		return false;
	}

	if (NormalMap)
		return true;

	CHECK_RANGE(amplitude, 0.0f, 9.0f);
	amplitude += 1.0f;

	core::dimension2d<s32> dim = m_Dim;
	amplitude = amplitude / 255.0f;
	f32 vh = dim.Height / (f32)dim.Width;
	f32 hh = dim.Width / (f32)dim.Height;

	s32 pitch = dim.Width;

    if (m_Format == img::ECF_A8R8G8B8)
    {
        s32 *p = (s32*)getData();

        if (!p)
        {
            LOGGER.logErr("Could not lock image for making normal map.");
            return false;
        }

		// copy texture

		s32* in = new s32[dim.Height * pitch];
		memcpy(in, p, dim.Height * pitch * 4);

		for (s32 x=0; x<pitch; ++x)
		{
			for (s32 y=0; y<dim.Height; ++y)
			{
				// TODO: this could be optimized really a lot

				core::vector3df h1((x-1)*hh, nml32(x-1, y, pitch, dim.Height, in)*amplitude, y*vh);
				core::vector3df h2((x+1)*hh, nml32(x+1, y, pitch, dim.Height, in)*amplitude, y*vh);
				core::vector3df v1(x*hh, nml32(x, y+1, pitch, dim.Height, in)*amplitude, (y-1)*vh);
				core::vector3df v2(x*hh, nml32(x, y-1, pitch, dim.Height, in)*amplitude, (y+1)*vh);

				core::vector3df v = v1-v2;
				core::vector3df h = h1-h2;

				core::vector3df n = v.getCrossProduct(h);
				n.normalize();
				n *= 0.5f;
				n += core::vector3df(0.5f,0.5f,0.5f); // now between 0 and 1
				n *= 255.0f;

				s32 height = (s32)nml32(x, y, pitch, dim.Height, in);
				p[y*pitch + x] = SColor(
					height, // store height in alpha
					(s32)n.X, (s32)n.Z, (s32)n.Y
					).color;
			}
		}
		delete [] in;
    }
	else
	if (m_Format == img::ECF_R5G6B5)
	{
		s32 *p = (s32*)getData();

        if (!p)
        {
            LOGGER.logErr("Could not lock image for making normal map.");
            return false;
        }

		// copy texture

		s16* in = new s16[dim.Height * pitch];
		memcpy(in, p, dim.Height * pitch * 2);

		for (s32 x=0; x<pitch; ++x)
		{
			for (s32 y=0; y<dim.Height; ++y)
			{
				// TODO: this could be optimized really a lot

				core::vector3df h1((x-1)*hh, nml16(x-1, y, pitch, dim.Height, in)*amplitude, y*vh);
				core::vector3df h2((x+1)*hh, nml16(x+1, y, pitch, dim.Height, in)*amplitude, y*vh);
				core::vector3df v1(x*hh, nml16(x, y-1, pitch, dim.Height, in)*amplitude, (y-1)*vh);
				core::vector3df v2(x*hh, nml16(x, y+1, pitch, dim.Height, in)*amplitude, (y+1)*vh);

				core::vector3df v = v1-v2;
				core::vector3df h = h1-h2;

				core::vector3df n = v.getCrossProduct(h);
				n.normalize();
				n *= 0.5f;
				n += core::vector3df(0.5f,0.5f,0.5f); // now between 0 and 1
				n *= 255.0f;

				p[y*pitch + x] = RGB16((s32)n.X, (s32)n.Z, (s32)n.Y);
			}
		}
		delete [] in;
	}
	else
	{
        LOGGER.logErr("Unsupported image color format for making normal map.");
		return false;		
	}
	NormalMap = true;

	return true;
}

//----------------------------------------------------------------------------

const c8* CImage::getOverridedFileName()
{
	if (m_OverridedFileName.size()>0)
		return m_OverridedFileName.c_str();
	return ((m_OverridedFileName.size() > 0) ? (m_OverridedFileName.c_str()) : 0);
}

//----------------------------------------------------------------------------

void CImage::setOverridedFileName(const c8* fname)
{
	m_OverridedFileName = fname;
}

//----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//----------------------------------------------------------------------------