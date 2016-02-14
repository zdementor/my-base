//|-------------------------------------------------------------------------
//| File:        IXMLWriter.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IXMLWriterHPP
#define IXMLWriterHPP
//-----------------------------------------------------------------------------

#include <IUnknown.h>

//-----------------------------------------------------------------------------
namespace my {
namespace io {
//-----------------------------------------------------------------------------

//! Interface providing methods for making it easier to write XML files.
//! This XML Writer only writes UTF-16 xml files, because these are parsed 
//! faster than all other formats by IXMLReader.
class __MY_IO_LIB_API__ IXMLWriter : public IUnknown
{
public:

    //! Destructor
    virtual ~IXMLWriter() {};

    //! Writes a xml 1.0 header like <?xml version="1.0"?>. This should
    //! always be called before writing anything other, because also the
    //! text file header for unicode texts is written out with this method.
    virtual void writeXMLHeader() = 0;

    //! Writes an xml element with maximal 50 attributes like <foo /> or
    //! <foo optAttr="value" />. The element can be empty or not.
    //! \param name: Name of the element
    //! \param empty: Specifies if the element should be empty. Like <foo />. If
    //! You set this to false, something like this is written instead: <foo>.
    virtual void writeElement(
        const wchar_t* name, bool empty=false,

        const wchar_t* attr1Name = 0, const wchar_t* attr1Value = 0,
        const wchar_t* attr2Name = 0, const wchar_t* attr2Value = 0,
        const wchar_t* attr3Name = 0, const wchar_t* attr3Value = 0,
        const wchar_t* attr4Name = 0, const wchar_t* attr4Value = 0,
        const wchar_t* attr5Name = 0, const wchar_t* attr5Value = 0,
        const wchar_t* attr6Name = 0, const wchar_t* attr6Value = 0,
        const wchar_t* attr7Name = 0, const wchar_t* attr7Value = 0,
        const wchar_t* attr8Name = 0, const wchar_t* attr8Value = 0,
        const wchar_t* attr9Name = 0, const wchar_t* attr9Value = 0,
        const wchar_t* attr10Name= 0, const wchar_t* attr10Value= 0,   		

		const wchar_t* attr11Name = 0, const wchar_t* attr11Value = 0,
		const wchar_t* attr12Name = 0, const wchar_t* attr12Value = 0,
		const wchar_t* attr13Name = 0, const wchar_t* attr13Value = 0,
		const wchar_t* attr14Name = 0, const wchar_t* attr14Value = 0,
		const wchar_t* attr15Name = 0, const wchar_t* attr15Value = 0,
		const wchar_t* attr16Name = 0, const wchar_t* attr16Value = 0,
		const wchar_t* attr17Name = 0, const wchar_t* attr17Value = 0,
		const wchar_t* attr18Name = 0, const wchar_t* attr18Value = 0,
		const wchar_t* attr19Name = 0, const wchar_t* attr19Value = 0,
		const wchar_t* attr20Name = 0, const wchar_t* attr20Value = 0,

		const wchar_t* attr21Name = 0, const wchar_t* attr21Value = 0,
		const wchar_t* attr22Name = 0, const wchar_t* attr22Value = 0,
		const wchar_t* attr23Name = 0, const wchar_t* attr23Value = 0,
		const wchar_t* attr24Name = 0, const wchar_t* attr24Value = 0,
		const wchar_t* attr25Name = 0, const wchar_t* attr25Value = 0,
		const wchar_t* attr26Name = 0, const wchar_t* attr26Value = 0,
		const wchar_t* attr27Name = 0, const wchar_t* attr27Value = 0,
		const wchar_t* attr28Name = 0, const wchar_t* attr28Value = 0,
		const wchar_t* attr29Name = 0, const wchar_t* attr29Value = 0,
		const wchar_t* attr30Name = 0, const wchar_t* attr30Value = 0,

		const wchar_t* attr31Name = 0, const wchar_t* attr31Value = 0,
		const wchar_t* attr32Name = 0, const wchar_t* attr32Value = 0,
		const wchar_t* attr33Name = 0, const wchar_t* attr33Value = 0,
		const wchar_t* attr34Name = 0, const wchar_t* attr34Value = 0,
		const wchar_t* attr35Name = 0, const wchar_t* attr35Value = 0,
		const wchar_t* attr36Name = 0, const wchar_t* attr36Value = 0,
		const wchar_t* attr37Name = 0, const wchar_t* attr37Value = 0,
		const wchar_t* attr38Name = 0, const wchar_t* attr38Value = 0,
		const wchar_t* attr39Name = 0, const wchar_t* attr39Value = 0,
		const wchar_t* attr40Name = 0, const wchar_t* attr40Value = 0,

		const wchar_t* attr41Name = 0, const wchar_t* attr41Value = 0,
		const wchar_t* attr42Name = 0, const wchar_t* attr42Value = 0,
		const wchar_t* attr43Name = 0, const wchar_t* attr43Value = 0,
		const wchar_t* attr44Name = 0, const wchar_t* attr44Value = 0,
		const wchar_t* attr45Name = 0, const wchar_t* attr45Value = 0,
		const wchar_t* attr46Name = 0, const wchar_t* attr46Value = 0,
		const wchar_t* attr47Name = 0, const wchar_t* attr47Value = 0,
		const wchar_t* attr48Name = 0, const wchar_t* attr48Value = 0,
		const wchar_t* attr49Name = 0, const wchar_t* attr49Value = 0,
		const wchar_t* attr50Name = 0, const wchar_t* attr50Value = 0   
        ) = 0;

    //! Writes a comment into the xml file
    virtual void writeComment(const wchar_t* comment) = 0;

    //! Writes the closing tag for an element. Like </foo>
    virtual void writeClosingTag(const wchar_t* name) = 0;

    //! Writes a text into the file. All occurrences of special characters like
    //! & (&amp;), < (&lt;), > (&gt;), and " (&quot;) are automaticly replaced.
    virtual void writeText(const wchar_t* text) = 0;

	//! Writes a text into the file.
	virtual void write(const wchar_t* text) = 0;

    //! Writes a line break
    virtual void writeLineBreak() = 0;

	//! Returns name of file.
	//! \return Returns the file name as zero terminated character string.
	virtual const c8* getFileName()  = 0; 
};

//-----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//-----------------------------------------------------------------------------

#endif

