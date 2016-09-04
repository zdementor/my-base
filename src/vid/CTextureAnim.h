//|-------------------------------------------------------------------------
//| File:        CTextureAnim.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CTextureAnimHPP
#define CTextureAnimHPP
//---------------------------------------------------------------------------

#include <vid/ITextureAnim.h>
#include <io/IFileSystem.h>
#include <io/IXMLReader.h>
#include <io/IReadFile.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! class Animated Texture
class CTextureAnim : public ITextureAnim
{
public:

	CTextureAnim();
	CTextureAnim(ITextureAnim* other);
	virtual ~CTextureAnim();

	virtual void clear();

	virtual img::IImage* lock(u32 level) { return NULL; }
	virtual void unlock(u32 level) {}

	virtual const core::dimension2d<s32>& getOriginalSize() { return OriginalSize; }

	virtual const core::dimension2d<s32>& getSize() { return Size; }

	virtual img::E_COLOR_FORMAT getColorFormat() { return ColorFormat; }

	virtual s32 getPitch() { return Pitch; }

	virtual bool freeImageData() { return false; }

	virtual void regenerateMipMapLevels() {}

    virtual E_TEXTURE_TYPE getType() { return ETT_ANIMATED; }

	virtual E_TEXTURE_FORMAT getFormat() { return m_Format; }

	virtual bool hasMipMaps() { return false; }

	virtual u32 getMaxMipMapLevels() const { return 1; }
	
	virtual void* getHardwareTexture();

	virtual bool loadFromFile(io::IReadFile* file);

	virtual SAnimatedTextureParams& getParameters();

	virtual void setParameters(SAnimatedTextureParams& params);

	virtual bool addFrame(ITexture* tex);

	virtual ITexture* getFrame(s32 idx);

	virtual s32 getFramesCount();

	static core::stringc getFirstTextureFileName(c8* buffer, s32 size)
	{
		if (!buffer || size<=0)
			return "";

		io::IReadFile* file = 
			FILE_SYSTEM.createMemoryReadFile(buffer, size, "emptyname", false);

		if (!file)
			return "";

		io::IXMLReader* xml = FILE_SYSTEM.createXMLReader(file);

		if (!xml)
		{
			file->drop();

			return "";
		}

		SAnimatedTextureParams anim_params;
		core::array<core::stringc> filenames;

		filenames.clear();

		bool ani_tex_tag_founded = false; 

		while (xml->read())
		{
			if (xml->getNodeType()== io::EXN_ELEMENT)
			{
				core::stringw xml_node_name = xml->getName();

				if (core::stringw("AnimatedTexture") == xml_node_name)
				{
					ani_tex_tag_founded = true;	
					
					anim_params.TimePerFrame = xml->getAttributeValueAsInt(L"time_per_frame");
					CHECK_RANGE(anim_params.TimePerFrame, 10, 10000);

					anim_params.Loop = xml->getAttributeValueAsInt(L"loop")==0?false:true;
				}
				else
				if (ani_tex_tag_founded && core::stringw("Frame") == xml_node_name)
				{
					core::stringc filename = xml->getAttributeValue(L"filename");
					
					if (filename.size()>0)
					{
						filenames.push_back(filename);
					}
				}
			}
		}

		xml->drop();
		file->drop();

		if (filenames.size()>0)
			return filenames[0];

		return "";
	}

	virtual bool isCompressed() { return false; }

	virtual bool isRenderTarget() { return false; }

private:

	inline ITexture* _getCurrentTexture();
	
	core::array<ITexture*> Frames;

	core::dimension2d<s32> OriginalSize;
	core::dimension2d<s32> Size;
	img::E_COLOR_FORMAT ColorFormat;
	s32 Pitch;

	bool FirstTime;

	u32 StartTime;

	SAnimatedTextureParams AnimatedTextureParameters;

	E_TEXTURE_FORMAT m_Format;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef CTextureAnimHPP

