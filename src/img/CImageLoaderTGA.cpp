//|-------------------------------------------------------------------------
//| File:        CImageLoaderTGA.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CImageLoaderTGA.h"
#include <img/SColor.h>
#include "CColorConverter.h"
#include "CImage.h"
#include <io/ILogger.h>

//-------------------------------------------------------------
namespace my {
namespace img {
//-------------------------------------------------------------

//! constructor
CImageLoaderTGA::CImageLoaderTGA()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CImageLoaderTGA");
#endif
}

//-------------------------------------------------------------

//! destructor
CImageLoaderTGA::~CImageLoaderTGA()
{
}

//-------------------------------------------------------------

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderTGA::isALoadableFileExtension(const c8* fileName)
{
    return strstr(fileName, ".tga") != 0;
}

//-------------------------------------------------------------

//! loads a compressed tga. 
c8 *CImageLoaderTGA::loadCompressedImage(io::IReadFile *file, STGAHeader *header)
{
    // This was written and sent in by Jon Pry, thank you very much!
    // I only changed the formatting a little bit.

    s32 bytesPerPixel = header->PixelDepth/8;
    s32 imageSize =  header->ImageHeight * header->ImageWidth * bytesPerPixel;
    c8* data = new c8[imageSize];
    s32 currentByte = 0;

    while(currentByte < imageSize)                  
    {
        u8 chunkheader = 0;
        file->read(&chunkheader, sizeof(c8)); // Read The Chunk's Header

        if(chunkheader < 128) // If The Chunk Is A 'RAW' Chunk
        {
            chunkheader++; // Add 1 To The Value To Get Total Number Of Raw Pixels

            file->read(&data[currentByte], bytesPerPixel * chunkheader);
            currentByte += bytesPerPixel * chunkheader;
        }
        else
        {
            // If It's An RLE Header 
            chunkheader -= 127; // Subtract 127 To Get Rid Of The ID Bit 

            s32 dataOffset = currentByte; 
            file->read(&data[dataOffset], bytesPerPixel); 

            currentByte += bytesPerPixel; 

            for(s32 counter = 1; counter < chunkheader; counter++) 
            { 
                for(s32 elementCounter=0; elementCounter < bytesPerPixel; elementCounter++) 
                    data[currentByte + elementCounter] = 
                        data[dataOffset + elementCounter]; 

                currentByte += bytesPerPixel; 
            }
        }
    }

    return data;
}
//-------------------------------------------------------------
//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderTGA::isALoadableFileFormat(io::IReadFile* file)
{
    if (!file)
        return false;

    u8 type[3];
    file->read(&type, sizeof(u8)*3);

    return (type[2]==1 || type[2]==2 || type[2]==10); 
}

//-------------------------------------------------------------

//! creates a surface from the file
IImage* CImageLoaderTGA::loadImage(io::IReadFile* file)
{
    STGAHeader header;

    file->seek(0);
    file->read(&header, sizeof(STGAHeader));

    // skip image identifikation field
    if (header.IdLength)
        file->seek(header.IdLength, true);

    // read image
    s32 bytesPerPixel = header.PixelDepth/8;
    s32 imageSize = header.ImageHeight * header.ImageWidth * bytesPerPixel;
    u8* data = 0;
	u8* map_data = 0;
	s32 map_bytes_num = header.ColorMapLength*(header.ColorMapEntrySize/8);

	switch(header.ImageType)
	{
		case 1:

			if(header.ColorMapType==1 && header.ColorMapEntrySize==24)
			{
				map_data = 
					new u8 [map_bytes_num];

				file->read(map_data, map_bytes_num);				
			}
			else
			{
				LOGGER.log("Only 24 bit paletted TGA images are supported.", io::ELL_ERROR);
				return 0;
			}

			break;

		case 2:

			// skip color map
			file->seek((header.ColorMapEntrySize/8 * header.ColorMapLength), true);

			data = new u8[imageSize];
			file->read(data, imageSize);

			break;

		case 10:

			// skip color map
			file->seek((header.ColorMapEntrySize/8 * header.ColorMapLength), true);

			data = (u8*)loadCompressedImage(file, &header); 

			break;

		default:

			LOGGER.log("Unsupported TGA file type", file->getFileName(), io::ELL_ERROR);
			return 0; 
	}

    IImage* image = 0;

    switch(bytesPerPixel)
    {
    case 1: 
		{	// 8 bit indexed image
			
			if(header.ColorMapType==1 && header.ColorMapEntrySize==24)
			{
				s32 wh = header.ImageHeight * header.ImageWidth;

				u8 *buffer = new u8 [wh];

				file->read( buffer, wh );
				
				data = new u8 [wh*3];

				for(s32 i=0; i<wh; i++)
				{
					data[3*i+0] = map_data[3*buffer[i]  ];
					data[3*i+1] = map_data[3*buffer[i]+1];
					data[3*i+2] = map_data[3*buffer[i]+2];
				}

				SAFE_DELETE_ARRAY(buffer);

				image = new CImage(
					ECF_R8G8B8, core::dimension2d<s32>(header.ImageWidth, header.ImageHeight)
					);

				CColorConverter::convert24BitTo24BitFlipMirrorColorShuffle(
					(c8*)data, (c8*)image->getData(), header.ImageWidth, header.ImageHeight, 0
					);

				break;
			}
			else
			{
				LOGGER.log("Unsupported TGA 8 bit format", file->getFileName(), io::ELL_ERROR);			
				break; 
			}
		}
        break;

    case 2:
        {   // 16 bit image
			img::E_COLOR_FORMAT fmt = ECF_A1R5G5B5;
            image = new CImage(fmt,
				core::dimension2d<s32>(header.ImageWidth, header.ImageHeight));
            CColorConverter::convert16BitTo16Bit((s16*)data,
				(s16*)image->getData(), header.ImageWidth,	header.ImageHeight, 0, (header.ImageDescriptor&0x20)==0);           
        }
        break;

    case 3:
        {   // 24 bit image
            image = new CImage(
				ECF_R8G8B8, core::dimension2d<s32>(header.ImageWidth, header.ImageHeight)
				);

            CColorConverter::convert24BitTo24Bit(
				(u8*)data, 
				(u8*)image->getData(), header.ImageWidth, header.ImageHeight, 0, (header.ImageDescriptor&0x20)==0, true
				);
        }
        break;

    case 4:
        {   // 32 bit image       
            image = new CImage(
				ECF_A8R8G8B8, core::dimension2d<s32>(header.ImageWidth, header.ImageHeight)
				);

            CColorConverter::convert32BitTo32Bit(
				(s32*)data,
				(s32*)image->getData(), header.ImageWidth, header.ImageHeight, 0, (header.ImageDescriptor&0x20)==0
				);
        }       
        break;

	default:
			
		LOGGER.log("Unsupported TGA format", file->getFileName(), io::ELL_ERROR);
			
		return 0; 
    }

    SAFE_DELETE_ARRAY(data);
	SAFE_DELETE_ARRAY(map_data);

    return image;
}

//-----------------------------------------------------------------------------

//! loading image from file
IImage* CImageLoaderTGA::loadImage(const c8* fileName)
{   
	return CImageLoader::loadImage(fileName);
}

//-------------------------------------------------------------

//! creates a loader which is able to load tgas
IImageLoader* createImageLoaderTGA()
{
    return new CImageLoaderTGA();
}

//-------------------------------------------------------------
} // end namespace img
} // end namespace my
//-------------------------------------------------------------
