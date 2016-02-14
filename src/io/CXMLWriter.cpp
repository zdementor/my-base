//|-------------------------------------------------------------------------
//| File:        CXMLWriter.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CXMLWriter.h"

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

//! Constructor
CXMLWriter::CXMLWriter(IWriteFile* file)
: File(file)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CXMLWriter");
#endif

    SAFE_GRAB(File);
}

//----------------------------------------------------------------------------

//! Destructor
CXMLWriter::~CXMLWriter()
{
    SAFE_DROP(File);
}

//----------------------------------------------------------------------------

//! Writes a xml 1.0 header like <?xml version="1.0"?>
void CXMLWriter::writeXMLHeader()
{
    if (!File)
        return;

    u16 h = 0xFEFF;
    File->write(&h, 2);

    wchar_t* p = L"<?xml version=\"1.0\"?>";
    File->write(p, (s32)wcslen(p)*2);

    writeLineBreak();
}

//----------------------------------------------------------------------------

//! Writes an xml element with maximal 50 attributes
void CXMLWriter::writeElement(
    const wchar_t* name, bool empty,
    const wchar_t* attr1Name, const wchar_t* attr1Value,
    const wchar_t* attr2Name, const wchar_t* attr2Value,
    const wchar_t* attr3Name, const wchar_t* attr3Value,
    const wchar_t* attr4Name, const wchar_t* attr4Value,
    const wchar_t* attr5Name, const wchar_t* attr5Value,
    const wchar_t* attr6Name, const wchar_t* attr6Value,
    const wchar_t* attr7Name, const wchar_t* attr7Value,
    const wchar_t* attr8Name, const wchar_t* attr8Value,
    const wchar_t* attr9Name, const wchar_t* attr9Value,
    const wchar_t* attr10Name,const wchar_t* attr10Value,

    const wchar_t* attr11Name, const wchar_t* attr11Value,
    const wchar_t* attr12Name, const wchar_t* attr12Value,
    const wchar_t* attr13Name, const wchar_t* attr13Value,
    const wchar_t* attr14Name, const wchar_t* attr14Value,
    const wchar_t* attr15Name, const wchar_t* attr15Value,
    const wchar_t* attr16Name, const wchar_t* attr16Value,
    const wchar_t* attr17Name, const wchar_t* attr17Value,
    const wchar_t* attr18Name, const wchar_t* attr18Value,
    const wchar_t* attr19Name, const wchar_t* attr19Value,
    const wchar_t* attr20Name, const wchar_t* attr20Value,

    const wchar_t* attr21Name, const wchar_t* attr21Value,
    const wchar_t* attr22Name, const wchar_t* attr22Value,
    const wchar_t* attr23Name, const wchar_t* attr23Value,
    const wchar_t* attr24Name, const wchar_t* attr24Value,
    const wchar_t* attr25Name, const wchar_t* attr25Value,
    const wchar_t* attr26Name, const wchar_t* attr26Value,
    const wchar_t* attr27Name, const wchar_t* attr27Value,
    const wchar_t* attr28Name, const wchar_t* attr28Value,
    const wchar_t* attr29Name, const wchar_t* attr29Value,
    const wchar_t* attr30Name, const wchar_t* attr30Value,

    const wchar_t* attr31Name, const wchar_t* attr31Value,
    const wchar_t* attr32Name, const wchar_t* attr32Value,
    const wchar_t* attr33Name, const wchar_t* attr33Value,
    const wchar_t* attr34Name, const wchar_t* attr34Value,
    const wchar_t* attr35Name, const wchar_t* attr35Value,
    const wchar_t* attr36Name, const wchar_t* attr36Value,
    const wchar_t* attr37Name, const wchar_t* attr37Value,
    const wchar_t* attr38Name, const wchar_t* attr38Value,
    const wchar_t* attr39Name, const wchar_t* attr39Value,
    const wchar_t* attr40Name, const wchar_t* attr40Value,

    const wchar_t* attr41Name, const wchar_t* attr41Value,
    const wchar_t* attr42Name, const wchar_t* attr42Value,
    const wchar_t* attr43Name, const wchar_t* attr43Value,
    const wchar_t* attr44Name, const wchar_t* attr44Value,
    const wchar_t* attr45Name, const wchar_t* attr45Value,
    const wchar_t* attr46Name, const wchar_t* attr46Value,
    const wchar_t* attr47Name, const wchar_t* attr47Value,
    const wchar_t* attr48Name, const wchar_t* attr48Value,
    const wchar_t* attr49Name, const wchar_t* attr49Value,
    const wchar_t* attr50Name, const wchar_t* attr50Value    
    )
{
    if (!File || !name)
        return;
    
    // write name

    File->write(L"<", 2);
    File->write(name, (s32)wcslen(name)*2);

    // write attributes

    writeAttribute(attr1Name, attr1Value);
    writeAttribute(attr2Name, attr2Value);
    writeAttribute(attr3Name, attr3Value);
    writeAttribute(attr4Name, attr4Value);
    writeAttribute(attr5Name, attr5Value);
    writeAttribute(attr6Name, attr6Value);
    writeAttribute(attr7Name, attr7Value);
    writeAttribute(attr8Name, attr8Value);
    writeAttribute(attr9Name, attr9Value);
    writeAttribute(attr10Name,attr10Value);

    writeAttribute(attr11Name, attr11Value);
    writeAttribute(attr12Name, attr12Value);
    writeAttribute(attr13Name, attr13Value);
    writeAttribute(attr14Name, attr14Value);
    writeAttribute(attr15Name, attr15Value);
    writeAttribute(attr16Name, attr16Value);
    writeAttribute(attr17Name, attr17Value);
    writeAttribute(attr18Name, attr18Value);
    writeAttribute(attr19Name, attr19Value);
    writeAttribute(attr20Name, attr20Value);

    writeAttribute(attr21Name, attr21Value);
    writeAttribute(attr22Name, attr22Value);
    writeAttribute(attr23Name, attr23Value);
    writeAttribute(attr24Name, attr24Value);
    writeAttribute(attr25Name, attr25Value);
    writeAttribute(attr26Name, attr26Value);
    writeAttribute(attr27Name, attr27Value);
    writeAttribute(attr28Name, attr28Value);
    writeAttribute(attr29Name, attr29Value);
    writeAttribute(attr30Name, attr30Value);

    writeAttribute(attr31Name, attr31Value);
    writeAttribute(attr32Name, attr32Value);
    writeAttribute(attr33Name, attr33Value);
    writeAttribute(attr34Name, attr34Value);
    writeAttribute(attr35Name, attr35Value);
    writeAttribute(attr36Name, attr36Value);
    writeAttribute(attr37Name, attr37Value);
    writeAttribute(attr38Name, attr38Value);
    writeAttribute(attr39Name, attr39Value);
    writeAttribute(attr40Name, attr40Value);

    writeAttribute(attr41Name, attr41Value);
    writeAttribute(attr42Name, attr42Value);
    writeAttribute(attr43Name, attr43Value);
    writeAttribute(attr44Name, attr44Value);
    writeAttribute(attr45Name, attr45Value);
    writeAttribute(attr46Name, attr46Value);
    writeAttribute(attr47Name, attr47Value);
    writeAttribute(attr48Name, attr48Value);
    writeAttribute(attr49Name, attr49Value);
    writeAttribute(attr50Name, attr50Value);

    // write closing tag
    if (empty)
        File->write(L" />", 6);
    else
        File->write(L">", 2);
}

//----------------------------------------------------------------------------

void CXMLWriter::writeAttribute(const wchar_t* name, const wchar_t* value)
{
    if (!name || !value)
        return;

	core::stringw strw;

    File->write(L" ", 2);
    File->write(name, (s32)wcslen(name)*2);
    File->write(L"=\"", 4);

	strw = replaceSpecialCharacters(core::stringw(value));
	File->write(strw.c_str(), strw.size()*2);

    File->write(L"\"", 2);
}

//----------------------------------------------------------------------------

//! Writes a comment into the xml file
void CXMLWriter::writeComment(const wchar_t* comment)
{
    if (!File || !comment)
        return;

    File->write(L"<!--", 8);
    File->write(comment, (s32)wcslen(comment)*2); // TODO: replace special characters
    File->write(L"-->", 6);
}

//----------------------------------------------------------------------------

//! Writes the closing tag for an element. Like </foo>
void CXMLWriter::writeClosingTag(const wchar_t* name)
{
    if (!File || !name)
        return;

    File->write(L"</", 4);
    File->write(name, (s32)wcslen(name)*2);
    File->write(L">", 2);
}

//----------------------------------------------------------------------------

const CXMLWriter::XMLSpecialCharacters XMLWSChar[] = 
{
    { L'&', L"&amp;" },
    { L'<', L"&lt;" },
    { L'>', L"&gt;" },
    { L'"', L"&quot;" },
    { L'\0', 0}
};

//----------------------------------------------------------------------------

core::stringw CXMLWriter::replaceSpecialCharacters(core::stringw& origstr)
{
    core::stringw s;
	const wchar_t* p = origstr.c_str();

    while(*p)
    {
        // check if it is matching
        bool found = false;
        for (s32 i=0; XMLWSChar[i].Character != '\0'; ++i)
            if (*p == XMLWSChar[i].Character)
            {
                s.append(XMLWSChar[i].Symbol);
                found = true;
                break;
            }

        if (!found)
            s.append(*p);
        ++p;
    }

	return s;
}

//----------------------------------------------------------------------------

//! Writes a text into the file. All occurrences of special characters like
//! & (&amp;), < (&lt;), > (&gt;), and " (&quot;) are automaticly replaced.
void CXMLWriter::writeText(const wchar_t* text)
{
    if (!File || !text)
        return;

    core::stringw s;
    const wchar_t* p = text;

    while(*p)
    {
        // check if it is matching
        bool found = false;
        for (s32 i=0; XMLWSChar[i].Character != '\0'; ++i)
            if (*p == XMLWSChar[i].Character)
            {
                s.append(XMLWSChar[i].Symbol);
                found = true;
                break;
            }

        if (!found)
            s.append(*p);
        ++p;
    }

    // write new string
    File->write(s.c_str(), s.size()*2);
}

//----------------------------------------------------------------------------

//! Writes a text into the file.
void CXMLWriter::write(const wchar_t* text)
{
    if (!File || !text)
        return;

    core::stringw s = text;
    // write new string
    File->write(s.c_str(), s.size()*2);
}

//----------------------------------------------------------------------------

//! Writes a line break
void CXMLWriter::writeLineBreak()
{   
	if (!File)
        return;

    File->write("\n", 2);
}

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------


