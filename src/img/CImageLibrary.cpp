//|-------------------------------------------------------------------------
//| File:        CImageLibrary.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CImageLibrary.h"

#include <io/ILogger.h>
#include <io/IReadFile.h>
#include <io/IWriteFile.h>
#include <io/IFileSystem.h>
#include <img/IImage.h>
#include <io/IEventReceiver.h>
#include "CImage.h"

//----------------------------------------------------------------------------
namespace my {
namespace img {
//----------------------------------------------------------------------------

//! creates a surface loader wich is able to load image data 
//! from all kinds of image formats
ISurfaceLoader* createSurfaceLoader();

//! creates a surface saver wich is able to save image data 
//! to all kinds of image formats
ISurfaceSaver* createSurfaceSaver();

//----------------------------------------------------------------------------

//! constructor
CImageLibrary::CImageLibrary() : FileSystem(NULL), Logger(NULL)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CImageLibrary");
#endif

	SurfaceLoader = createSurfaceLoader();
	SurfaceSaver  = createSurfaceSaver();

	FileSystem = FILE_SYSTEM_PTR;
	Logger     = LOGGER_PTR;

	if (FileSystem)
		FileSystem->grab();
	else
	{
		LOGGER.log("ImageLibrary: can't grab File System singleton", io::ELL_ERROR);
	}

	if (Logger)
		Logger->grab();
	else
	{
		LOGGER.log("ImageLibrary: can't grab Logger singleton", io::ELL_ERROR);
	}
}

//----------------------------------------------------------------------------

//! destructor
CImageLibrary::~CImageLibrary()
{
	clearImageCache();

	SAFE_DROP(SurfaceSaver);
	SAFE_DROP(SurfaceLoader);
	SAFE_DROP(FileSystem);
	SAFE_DROP(Logger);
}

//----------------------------------------------------------------------------

//! Creates a software image from a file. 
IImage* CImageLibrary::createImageFromFile(const char* filename)
{	
	IImage* image = SurfaceLoader->loadImageFromFile(filename);

	if (image)
    {
		LOGGER.logInfo("Loaded image ( %s, %dx%d ): %s", 
			img::ColorFormatStr[image->getColorFormat()],
			image->getDimension().Width, image->getDimension().Height,
			filename);
	}

	return image;
}

//----------------------------------------------------------------------------

IImage* CImageLibrary::createImageFromFile(io::IReadFile* file)
{
	IImage *image = SurfaceLoader->loadImageFromFile(file);
	if (image)
    {
		LOGGER.logInfo("Loaded image ( %s, %dx%d ): %s", 
			img::ColorFormatStr[image->getColorFormat()],
			image->getDimension().Width, image->getDimension().Height,
			file->getFileName());
	}

	return image;
}

//----------------------------------------------------------------------------

IImage* CImageLibrary::createImageFromData(
	img::E_COLOR_FORMAT format, const core::dimension2d<s32> &size,
	u32 data_size, void *data, bool own_data)
{
	if (!data_size && (
			format == ECF_DXT1 ||
			format == ECF_DXT3 ||
			format == ECF_DXT5))
	{
		LOGGER.logErr("Can't create %s compressed image without its data size",
			ColorFormatStr[format]);
		return NULL;
	}
	return new CImage(format, size, data_size, data, own_data);
}

//---------------------------------------------------------------------------

IImage* CImageLibrary::createEmptyImage(
	const core::dimension2d<s32>& size, img::E_COLOR_FORMAT format)
{
	if (format == ECF_DXT1 ||
		format == ECF_DXT3 ||
		format == ECF_DXT5)
	{
		LOGGER.logErr("Can't create empty image with %s format",
			ColorFormatStr[format]);
		return NULL;
	}
	return new CImage(format, size);
}

//---------------------------------------------------------------------------

IImage* CImageLibrary::createImageFromAnother( 
	IImage* imageToCopy, E_COLOR_FORMAT format)
{
	if (format == ECF_DXT1 ||
		format == ECF_DXT3 ||
		format == ECF_DXT5)
	{
		LOGGER.logErr("Can't copy image with %s format",
			ColorFormatStr[format]);
		return NULL;
	}
	return new CImage(format, imageToCopy);
}

//---------------------------------------------------------------------------

IImage* CImageLibrary::getImage(const c8* filename)
{
	if (!filename || core::stringw(NONAME_FILE_NAME) == core::stringw(filename))
		return NULL;

	IImage* image = findImage(filename);

    if (image)
        return image;

    io::IReadFile* file = FileSystem->createAndOpenFile(filename);		

    if (file)
    {
        image = createImageFromFile(file);        

        if (image)
        {
            addImage(file->getFileName(), image);
            image->drop(); // drop it because we created it, one grab is much enough
        }
		else
			LOGGER.logErr("Could not load image from file '%s' "
				"(format is seems to be unsupported)", filename);

		file->drop();
    }
    else
        LOGGER.logErr("Could not open file of image '%s'", filename);

    return image;
}

//---------------------------------------------------------------------------

IImage* CImageLibrary::getImage(io::IReadFile* file)
{
	IImage* image = 0;

	if (file)		
		image = findImage(file->getFileName());

    if (image)
        return image;

    if (file)
    {
        image = createImageFromFile(file);

        if (image)
        {
            addImage(file->getFileName(), image);
            image->drop(); // drop it becaus we created it, one grab to much
        }

		if (!image)
			LOGGER.log("Could not load image", file->getFileName(), io::ELL_ERROR);
    }
	else
	{
		LOGGER.log("Could not load texture because file is null", io::ELL_ERROR);
	}

    return image;
}

//---------------------------------------------------------------------------

void CImageLibrary::addImage(const c8* name, IImage* image)
{
	if (image)
    {
		SImageSurface is;

		is.Filename = name;
		
        if ( !(is.Filename.size()>0) )
		{
			c8 imgname[255];
			sprintf(imgname, "image%p", image);
			is.Filename = imgname;
		}
        
        is.Filename = name;
        is.Filename.make_lower();
        is.ImageSurface = image;
        image->grab();

        Images.push_back(is);
    }
}

//---------------------------------------------------------------------------

IImage* CImageLibrary::findImage(const c8* filename)
{
	SImageSurface is;
    if (!filename)
        filename = "";

    is.Filename = filename;
    is.Filename.make_lower();

    s32 index = Images.binary_search(is);
    if (index != -1)
        return Images[index].ImageSurface;

    return 0;
}

//---------------------------------------------------------------------------

core::stringc CImageLibrary::findImageFileName(IImage* img_ptr)
{
	core::stringc filename=NONAME_FILE_NAME;

	for (u32 i=0; i<Images.size(); i++)
	{
		if (Images[i].ImageSurface == img_ptr)
		{
			filename = core::extractFileName(Images[i].Filename);

			break;
		}
	}

	filename.make_lower();

	if (filename.size()==0) filename=NONAME_FILE_NAME;

    return filename;
}

//---------------------------------------------------------------------------

core::stringc CImageLibrary::findImageFullFileName(IImage* img_ptr)
{
	core::stringc filename=NONAME_FILE_NAME;

	for (u32 i=0; i<Images.size(); i++)
	{
		if (Images[i].ImageSurface == img_ptr)
		{
			filename = Images[i].Filename.c_str();

			break;
		}
	}

	filename.make_lower();

	if (filename.size()==0) filename=NONAME_FILE_NAME;

    return filename;
}

//---------------------------------------------------------------------------

bool CImageLibrary::removeImage(IImage* image)
{
	c8 ch[255];

	for (u32 i=0; i<Images.size(); ++i)
	{
        if (Images[i].ImageSurface == image)
        {
			if (image && image->getReferenceCounter()==1)
			{
				SAFE_DROP(image);

				sprintf(ch, "Image '%s' was removed.", Images[i].Filename);
				LOGGER.log(ch, io::ELL_INFORMATION);

				Images.erase(i);

				return true;
			}

			sprintf(ch, "Can't remove '%s' image, it used.", Images[i].Filename);
			LOGGER.log(ch, io::ELL_INFORMATION);

			return false;
        }
	}

	LOGGER.log("Can't remove image.", io::ELL_INFORMATION);

	return false;
}

//---------------------------------------------------------------------------

void CImageLibrary::clearImageCache()
{
	s32 cleared_images = Images.size();

    for (u32 i=0; i<Images.size(); ++i)
        SAFE_DROP(Images[i].ImageSurface);
    Images.clear();   	

	if (cleared_images>0)
	{
		core::stringc msg;

		msg.sprintf("Image cache (%d images) has been cleared.", cleared_images);
		LOGGER.log(msg.c_str());
	}
}

//---------------------------------------------------------------------------

void CImageLibrary::clearUnusedImageCache()
{
	s32 all_images = Images.size();
	s32 cleared_images = 0;

    for (u32 i=0; i<Images.size(); )
	{
		if (Images[i].ImageSurface && Images[i].ImageSurface->getReferenceCounter()==1)
		{
			SAFE_DROP(Images[i].ImageSurface);
			Images.erase(i);

			cleared_images++;
		}
		else
		{
			++i;
		}
	} 

	if (cleared_images>0)
	{
		LOGGER.logInfo("Unused image cache (%d images)(from %d at all) has been cleared.",
			cleared_images, all_images);
	}
}

//---------------------------------------------------------------------------

void CImageLibrary::addExternalSurfaceLoader(IImageLoader* loader)
{
	SurfaceLoader->addSurfaceLoader(loader);
}

//---------------------------------------------------------------------------

bool CImageLibrary::saveImageToFile(IImage* image, io::IWriteFile *file)
{
	if (SurfaceSaver->saveImageToFile(image, file))
	{
		LOGGER.logInfo("Saved image ( %s, %dx%d ): %s",
			img::ColorFormatStr[image->getColorFormat()],
			image->getDimension().Width,
			image->getDimension().Height,
			file->getFileName());
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------

bool CImageLibrary::saveImageToFile(IImage* image, const c8 *filename)
{
	if (SurfaceSaver->saveImageToFile(image, filename))
	{
		LOGGER.logInfo("Saved image ( %s, %dx%d ): %s",
			img::ColorFormatStr[image->getColorFormat()],
			image->getDimension().Width, image->getDimension().Height,
			filename);
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------

void CImageLibrary::addExternalSurfaceSaver(IImageSaver* saver)
{
	SurfaceSaver->addSurfaceSaver(saver);
}

//---------------------------------------------------------------------------

__MY_EXPORT__ IImageLibrary* createImageLibrary()
{
	if (!IMAGE_LIBRARY_PTR)
	{
		bool need_to_create_fs = false;
		bool need_to_create_log = false;

		if (!FILE_SYSTEM_PTR)
			need_to_create_fs = true;
		if (!LOGGER_PTR)
			need_to_create_log = true;

		if (need_to_create_fs)
			io::createFileSystem();
		if (need_to_create_log)
			io::createLogger(NULL);

		new CImageLibrary();

		// drop this singletons, because in image library contructor 
		// we already grabbed it
		if (need_to_create_fs)
			FILE_SYSTEM.drop();
		if (need_to_create_log)
			LOGGER.drop();
	}

	return IMAGE_LIBRARY_PTR;
}

//----------------------------------------------------------------------------

static void copyToAlpha32F  (u32 level, img::IImage* src_img, img::IImage* dst_img);
static void copyToDepth16   (u32 level, img::IImage *src_img, img::IImage *dst_img);
static void copyToA8R8G8B8	(u32 level, img::IImage *src_img, img::IImage *dst_img);
static void copyToR8G8B8	(u32 level, img::IImage *src_img, img::IImage *dst_img);
static void copyToA1R5G5B5	(u32 level, img::IImage *src_img, img::IImage *dst_img);
static void copyToR5G6B5	(u32 level, img::IImage *src_img, img::IImage *dst_img);
static void copyToDXT		(u32 level, img::IImage *src_img, img::IImage *dst_img);

bool CImageLibrary::copy(img::IImage* image, img::IImage* tex_image)
{
	bool res = false;

	if (!image || !tex_image)
		return false;

	void (*copyTo)(u32, img::IImage*, img::IImage*) = NULL;
	switch (tex_image->getColorFormat())
	{
	case img::ECF_A8R8G8B8:
		copyTo = copyToA8R8G8B8;
		break;
	case img::ECF_R8G8B8:
		copyTo = copyToR8G8B8;
		break;
	case img::ECF_A1R5G5B5:
		copyTo = copyToA1R5G5B5; 
		break;
	case img::ECF_R5G6B5:
		copyTo = copyToR5G6B5;
		break;
	case img::ECF_DXT1:
	case img::ECF_DXT3:
	case img::ECF_DXT5:
		copyTo = copyToDXT;
		break;
	case img::ECF_DEPTH16:
		copyTo = copyToDepth16;
		break;
	case img::ECF_ALPHA32F:
		copyTo = copyToAlpha32F;
		break;
	default:
		{
			LOGGER.logErr("Unsupported image format");
			return false;
		}
	}
	copyTo(0, image, tex_image);

    return true;
}

//----------------------------------------------------------------------------

static void copyToAlpha32F(
	u32 level, img::IImage* src_img, img::IImage* dst_img)
{
	core::dimension2di src_dim = src_img->getDimension();
	core::dimension2di dst_dim = dst_img->getDimension();
	bool sufaceHasSameSize = src_dim == dst_dim;
	float* source = (float*)src_img->getLevelData(level);
	float* dest = (float*)dst_img->getLevelData(level);

	if (src_img->getColorFormat() != img::ECF_ALPHA32F || dst_img->getColorFormat() != img::ECF_ALPHA32F)
	{
		LOGGER.logErr("Can't copy data format %s to %s",
			img::ColorFormatStr[src_img->getColorFormat()], img::ColorFormatStr[dst_img->getColorFormat()]);
        return;
	}

    if (sufaceHasSameSize)
    {
		// direct copy, fast             
		memcpy(dest, source, dst_img->getLevelDataSizeBytes(level));
    }
    else
    {
        // scale texture
        f32 sourceXStep = (f32)src_dim.Width / (f32)dst_dim.Width;
        f32 sourceYStep = (f32)src_dim.Height / (f32)dst_dim.Height;
        f32 sy;

        if (src_img->getColorFormat() == img::ECF_ALPHA32F)
        {
			// copy texture scaling, fast
            for (s32 x = 0; x < dst_dim.Width; ++x)
            {
                sy = 0.0f;
                for (s32 y = 0; y < dst_dim.Height; ++y)
                {
                    dest[(s32)(y * dst_dim.Width + x)] = 
						source[(s32)(((s32)sy) * src_dim.Width + x * sourceXStep)];
					sy += sourceYStep;
                }
            }
        }
    }  
}

//----------------------------------------------------------------------------

static void copyToDepth16(
	u32 level, img::IImage* src_img, img::IImage* dst_img)
{
	core::dimension2di src_dim = src_img->getDimension();
	core::dimension2di dst_dim = dst_img->getDimension();
	bool sufaceHasSameSize = src_dim == dst_dim;
	s16* source = (s16*)src_img->getLevelData(level);
	s16* dest = (s16*)dst_img->getLevelData(level);

	if (src_img->getColorFormat() != img::ECF_DEPTH16 || dst_img->getColorFormat() != img::ECF_DEPTH16)
	{
		LOGGER.logErr("Can't copy data format %s to %s",
			img::ColorFormatStr[src_img->getColorFormat()], img::ColorFormatStr[dst_img->getColorFormat()]);
        return;
	}

    if (sufaceHasSameSize)
    {
		// direct copy, fast             
		memcpy(dest, source, dst_img->getLevelDataSizeBytes(level));
    }
    else
    {
        // scale texture
        f32 sourceXStep = (f32)src_dim.Width / (f32)dst_dim.Width;
        f32 sourceYStep = (f32)src_dim.Height / (f32)dst_dim.Height;
        f32 sy;

        if (src_img->getColorFormat() == img::ECF_DEPTH16)
        {
			// copy texture scaling, fast
            for (s32 x = 0; x < dst_dim.Width; ++x)
            {
                sy = 0.0f;
                for (s32 y = 0; y < dst_dim.Height; ++y)
                {
                    dest[(s32)(y * dst_dim.Width + x)] = 
						source[(s32)(((s32)sy) * src_dim.Width + x * sourceXStep)];
					sy += sourceYStep;
                }
            }
        }

    }  
}

//----------------------------------------------------------------------------

static void copyToA8R8G8B8(
	u32 level, img::IImage* src_img, img::IImage* dst_img)
{
	core::dimension2di src_dim = src_img->getDimension();
	core::dimension2di dst_dim = dst_img->getDimension();
	bool sufaceHasSameSize = src_dim == dst_dim;
	s32 *source = (s32*)src_img->getLevelData(level);
	s32 *dest = (s32*)dst_img->getLevelData(level);

    if (sufaceHasSameSize)
    {
        if (src_img->getColorFormat() == img::ECF_A8R8G8B8)
			// direct copy, fast
			memcpy(dest, source, dst_img->getLevelDataSizeBytes(level));
		else
			// slow convert
			for (s32 y=0; y < src_dim.Height; ++y)
				for (s32 x=0; x < src_dim.Width; ++x)
					dest[x + y * dst_dim.Width] = src_img->getPixel(x,y).color;
    }
    else
    {
        // scale texture
        f32 sourceXStep = (f32)src_dim.Width / (f32)dst_dim.Width;
        f32 sourceYStep = (f32)src_dim.Height / (f32)dst_dim.Height;
        f32 sy;

        if (src_img->getColorFormat() == img::ECF_A8R8G8B8)
        {
            // copy texture scaling, fast
            for (s32 x = 0; x < dst_dim.Width; ++x)
            {
                sy = 0.0f;
                for (s32 y = 0; y < dst_dim.Height; ++y)
                {
                    dest[(s32)(y * dst_dim.Width + x)] = 
						source[(s32)(((s32)sy) * src_dim.Width + x * sourceXStep)];
					sy += sourceYStep;
                }
            }
        }
		else
		{
			// convert texture scaling, slow
			for (s32 x=0; x < dst_dim.Width; ++x)
			{
				sy = 0.0f;
				for (s32 y=0; y < dst_dim.Height; ++y)
				{
					dest[(s32)(y * dst_dim.Width + x)] =
						src_img->getPixel((s32)(x * sourceXStep), (s32)sy).color;
					sy += sourceYStep;
				}
			}
		}
    }
}

//----------------------------------------------------------------------------

static void copyToR8G8B8(
	u32 level, img::IImage* src_img, img::IImage* dst_img)
{
	core::dimension2di src_dim = src_img->getDimension();
	core::dimension2di dst_dim = dst_img->getDimension();
	bool sufaceHasSameSize = src_dim == dst_dim;

	if (src_img->getColorFormat() != img::ECF_R8G8B8)
	{
		LOGGER.logErr("Cant copy data to 24 bit texture");
        return;
	}

	u8 *source = (u8*)src_img->getLevelData(level);
	u8 *dest = (u8*)dst_img->getLevelData(level);

    if (sufaceHasSameSize)
    {
        // copy texture
        if (src_img->getColorFormat() == img::ECF_R8G8B8)
            // direct copy, fast             
			memcpy(dest, source, dst_img->getLevelDataSizeBytes(level));
    }
    else
    {
        // scale texture
        f32 sourceXStep = (f32)src_dim.Width / (f32)dst_dim.Width;
        f32 sourceYStep = (f32)src_dim.Height / (f32)dst_dim.Height;
        f32 sy;

        if (src_img->getColorFormat() == img::ECF_R8G8B8)
        {
			// copy texture scaling, fast
            for (s32 x = 0; x < dst_dim.Width; ++x)
            {
                sy = 0.0f;
                for (s32 y = 0; y < dst_dim.Height; ++y)
                {
					s32 di = (s32)(y * dst_dim.Width + x);
					s32 si = (s32)(((s32)sy) * src_dim.Width + x * sourceXStep);
                    dest[di * 3    ] = source[si * 3    ];
					dest[di * 3 + 1] = source[si * 3 + 1];
					dest[di * 3 + 2] = source[si * 3 + 2];
					sy += sourceYStep;
                }
            }
        }
    }
}

//----------------------------------------------------------------------------

static void copyToA1R5G5B5(
	u32 level, img::IImage* src_img, img::IImage* dst_img)
{
	core::dimension2di src_dim = src_img->getDimension();
	core::dimension2di dst_dim = dst_img->getDimension();
	bool sufaceHasSameSize = src_dim == dst_dim;
	s16* source = (s16*)src_img->getLevelData(level);
	s16* dest = (s16*)dst_img->getLevelData(level);

    if (sufaceHasSameSize)
    {
        // copy texture
        if (src_img->getColorFormat() == img::ECF_A1R5G5B5)
            // direct copy, fast             
			memcpy(dest, source, dst_img->getLevelDataSizeBytes(level));
		else
			// slow convert
			for (s32 y = 0; y < src_dim.Height; ++y)
				for (s32 x = 0; x < src_dim.Width; ++x)
					dest[x + y * dst_dim.Width] = src_img->getPixel(x,y).toA1R5G5B5();
    }
    else
    {
        // scale texture
        f32 sourceXStep = (f32)src_dim.Width / (f32)dst_dim.Width;
        f32 sourceYStep = (f32)src_dim.Height / (f32)dst_dim.Height;
        f32 sy;

        if (src_img->getColorFormat() == img::ECF_A1R5G5B5)
        {
			// copy texture scaling, fast
            for (s32 x = 0; x < dst_dim.Width; ++x)
            {
                sy = 0.0f;
                for (s32 y = 0; y < dst_dim.Height; ++y)
                {
                    dest[(s32)(y * dst_dim.Width + x)] = 
						source[(s32)(((s32)sy) * src_dim.Width + x * sourceXStep)];
					sy += sourceYStep;
                }
            }
        }
		else
		{
			// convert texture scaling, slow
			for (s32 x = 0; x < dst_dim.Width; ++x)
			{
				sy = 0.0f;
				for (s32 y = 0; y < dst_dim.Height; ++y)
				{
					dest[(s32)(y * dst_dim.Width + x)] =
						src_img->getPixel((s32)(x*sourceXStep), (s32)sy).toA1R5G5B5();
					sy += sourceYStep;
				}
			}
		}
    }  
}

//----------------------------------------------------------------------------

static void copyToR5G6B5(
	u32 level, img::IImage *src_img, img::IImage *dst_img)
{
	core::dimension2di src_dim = src_img->getDimension();
	core::dimension2di dst_dim = dst_img->getDimension();
	bool sufaceHasSameSize = src_dim == dst_dim;
	s16* source = (s16*)src_img->getLevelData(level);
	s16* dest = (s16*)dst_img->getLevelData(level);

    if (sufaceHasSameSize)
    {
        // copy texture
        if (src_img->getColorFormat() == img::ECF_R5G6B5)
            // direct copy, fast             
			memcpy(dest, source, dst_img->getLevelDataSizeBytes(level));
		else
			// slow convert
			for (s32 y = 0; y < src_dim.Height; ++y)
				for (s32 x = 0; x < src_dim.Width; ++x)
					dest[x + y * dst_dim.Width] = src_img->getPixel(x,y).toR5G6B5();
    }
    else
    {
        // scale texture
        f32 sourceXStep = (f32)src_dim.Width / (f32)dst_dim.Width;
        f32 sourceYStep = (f32)src_dim.Height / (f32)dst_dim.Height;
        f32 sy;

        if (src_img->getColorFormat() == img::ECF_R5G6B5)
        {
			// copy texture scaling, fast
            for (s32 x = 0; x < dst_dim.Width; ++x)
            {
                sy = 0.0f;
                for (s32 y = 0; y < dst_dim.Height; ++y)
                {
                    dest[(s32)(y * dst_dim.Width + x)] = 
						source[(s32)(((s32)sy) * src_dim.Width + x * sourceXStep)];
					sy += sourceYStep;
                }
            }
        }
		else
		{
			// convert texture scaling, slow
			for (s32 x = 0; x < dst_dim.Width; ++x)
			{
				sy = 0.0f;
				for (s32 y = 0; y < dst_dim.Height; ++y)
				{
					dest[(s32)(y * dst_dim.Width + x)] =
						src_img->getPixel((s32)(x * sourceXStep), (s32)sy).toR5G6B5();
					sy += sourceYStep;
				}
			}
		}
    }     
}

//----------------------------------------------------------------------------

static void copyToDXT(
	u32 level, img::IImage* src_img, img::IImage* dst_img)
{
	core::dimension2di src_dim = src_img->getDimension();
	core::dimension2di dst_dim = dst_img->getDimension();
	bool sufaceHasSameSize = src_dim == dst_dim;

	if (src_img->getColorFormat() != dst_img->getColorFormat() ||
		!sufaceHasSameSize)
	{
		LOGGER.logErr("Can't copy data to DXT image");
        return;
	}

	u8 *source = (u8*)src_img->getLevelData(level);
	u8 *dest = (u8*)dst_img->getLevelData(level);

    // direct copy, fast
	memcpy(dest, source, dst_img->getLevelDataSizeBytes(level));	
}

//----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//----------------------------------------------------------------------------