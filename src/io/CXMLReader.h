//|-------------------------------------------------------------------------
//| File:        CXMLReader.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CXMLReaderHPP
#define CXMLReaderHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <io/IReadFile.h>
#include <io/IXMLReader.h>
#include "CTextReader.h"

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

//! XML Reader providing easy read access to an XML file.
//! Read IXMLReader for more information.
class CXMLReader : public CTextReader, public IXMLReader
{
public:

    CXMLReader(IReadFile* file);
    virtual ~CXMLReader();

    virtual bool read();

    virtual EXML_NODE getNodeType();

    virtual s32 getAttributeCount();

    virtual const wchar_t* getAttributeName(s32 idx);

    virtual const wchar_t* getAttributeValue(s32 idx);

    virtual const wchar_t* getAttributeValue(const wchar_t* name);

    virtual const wchar_t* getAttributeValueSafe(const wchar_t* name);

    virtual s32 getAttributeValueAsInt(const wchar_t* name, s32 default_val = 0);
	virtual s64 getAttributeValueAsInt64(const wchar_t* name, s64 default_val = 0);
    virtual s32 getAttributeValueAsHexInt(const wchar_t* name, s32 default_val = 0);
	virtual s64 getAttributeValueAsHexInt64(const wchar_t* name, s64 default_val = 0);
   virtual u32 getAttributeValueAsUInt(const wchar_t* name, u32 default_val = 0);
	virtual u64 getAttributeValueAsUInt64(const wchar_t* name, u64 default_val = 0);
    virtual u32 getAttributeValueAsHexUInt(const wchar_t* name, u32 default_val = 0);
	virtual u64 getAttributeValueAsHexUInt64(const wchar_t* name, u64 default_val = 0);

    virtual f32 getAttributeValueAsFloat(const wchar_t* name, f32 default_val=0.0f);

    virtual bool getAttributeValueAsBool(const wchar_t* name, bool default_val=false);

	virtual core::vector3df getAttributeValueAsVector(
		const wchar_t* name, core::vector3df default_val=core::vector3df(0,0,0));

	virtual img::SColor getAttributeValueAsColor(
		const wchar_t* name, img::SColor default_val=img::SColor(255,0,0,0));

	virtual img::SColorf getAttributeValueAsColorF(
		const wchar_t* name, img::SColorf default_val=img::SColorf(0.0f,0.0f,0.0f,1.0f));

	virtual u32 getAttributeValueAsFlagBits(
		const wchar_t *name, const c8 **flag_names, const u32 *flag_bits, u32 flag_count,
		u32 default_val = 0);

	virtual u32 getAttributeValueAsIndexInArray(
		const wchar_t *name, const c8 **type_names, u32 type_count,
		u32 default_val = 0);

    virtual const wchar_t* getName();

    virtual const wchar_t* getNodeData();

    virtual bool isEmtpyElement();

	virtual const c8* getFileName()
	{
		return CTextReader::getFileName();
	}

	virtual const c8* getFilePath()
	{
		return CTextReader::getFilePath();
	}

	virtual void rewindToBegin();

	virtual s32 getPos();

	virtual bool setPos(s32 new_pos);

	virtual core::stringw getData(u32 start, u32 end);

    struct XMLSpecialCharacters
    {
        wchar_t Character;
        wchar_t* Symbol;
    };

private:

    void parseCurrentNode();

    //! sets the state that text was found
    void setText(wchar_t* start, wchar_t* end);

    //! ignores an xml definition like <?xml something />
    void ignoreDefinition();

    //! parses a comment
    void parseComment();

    //! parses an opening xml element and reads attributes
    void parseOpeningXMLElement();

    //! parses an closing xml tag
    void parseClosingXMLElement();

    inline bool isWhiteSpace(wchar_t c)
    {
        return (c==L' ' || c==L'\t' || c==L'\n' || c==L'\r');
    }

    wchar_t* P;
    wchar_t* TextBegin;
    u32 TextSize;

    EXML_NODE CurrentNodeType;
    core::stringw NodeName;
    bool IsEmptyElement;

    struct SAttribute
    {
        core::stringw Name;
        core::stringw Value;
    };

    SAttribute* getAttributeByName(const wchar_t* name);
    core::stringw replaceSpecialCharacters(core::stringw& str);

    core::array<SAttribute> Attributes;

};

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

#endif

