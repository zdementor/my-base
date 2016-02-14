//|-------------------------------------------------------------------------
//| File:        IXMLReader.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IXMLReaderHPP
#define IXMLReaderHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <img/SColor.h>

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

//! Enumeration for all xml nodes which are parsed by IXMLReader
enum EXML_NODE
{
    //! No xml node. This is usually the node if you did not read anything yet.
    EXN_NONE,

    //! A xml element, like <foo>
    EXN_ELEMENT,

    //! End of an xml element, like </foo>
    EXN_ELEMENT_END,

    //! Text within a xml element: <foo> this is the text. </foo>
    EXN_TEXT,

    //! An xml comment like <!-- I am a comment --> or a DTD definition.
    EXN_COMMENT,

    //! Unknown element.
    EXN_UNKNOWN
};

//----------------------------------------------------------------------------

//! Interface providing easy read access to an XML file.
/** The Irrlicht Engine XML Reader provides forward-only, read-only
 access to a stream of non validated XML data. It was fully implemented by
 Nikolaus Gebhardt, and does not rely on external XML software.
 It is quite fast, and has the advantage, that it can read everywhere the Irrlicht
 Engine can, for example directly from compressed .zip files. This XML parser
 is very basic and simple. It was created with the intention to parse configuration
 data, please use a more advanced XML parser if you have a more complicated task.
 It currently has the following limitations:
    - The reader only recognizes some special xml characters:
      & (&amp;), < (&lt;), > (&gt;), and " (&quot;)
    - The XML Parser is able to parse ASCII and UTF-16 text files, 
      in little and big endian. Other text formats like UTF-8 are not supported yet,
      but maybe will be added later.
    - The xml file is not validated and assumed to be correct. If the is malformatted,
      the parser may crash. But in most cases, the parser should just ignore the problem.

The following code demonstrates the basic usage of the xml reader. A simple xml
file like this is parsed:
\code
<?xml version="1.0"?>
<config>
    <!-- This is a config file for the mesh viewer -->
    <startUpModel file="../data/dwarf.x" />
    <messageText caption="Irrlicht Engine Mesh Viewer">
    Welcome to the Mesh Viewer of the &quot;Irrlicht Engine&quot;.
    </messageText>
</config>
\endcode

The code for parsing this file would look like this:
\code
io::IXMLReader* xml = device->getFileSystem()->createXMLReader("config.xml");

core::stringw startUpModelFile;
core::stringw messageText;
core::stringw caption;

while(xml && xml->read())
{
    switch(xml->getNodeType())
    {
    case io::EXN_TEXT:
        // in this xml file, the only text which occurs is the messageText
        messageText = xml->getNodeData();
        break;
    case io::EXN_ELEMENT:
        {
            if (core::stringw("startUpModel") == xml->getName())
                startUpModelFile = xml->getAttributeValue(L"file");
            else
            if (core::stringw("messageText") == xml->getName())
                caption = xml->getAttributeValue(L"caption");
        }
        break;
    }
}

if (xml)
    xml->drop(); // don't forget to delete the xml reader
\endcode
*/
class __MY_IO_LIB_API__ IXMLReader : public IUnknown                          
{
public:

    //! Destructor
    virtual ~IXMLReader() {};

    //! Reads forward to the next xml node.
    //! \return Returns false, if there was no further node.
    virtual bool read() = 0;

    //! Returns the type of the current XML node.
    virtual EXML_NODE getNodeType() = 0;

    //! Returns attribute count of the current XML node. This is usually
    //! non null if the current node is EXN_ELEMENT, and the element has attributes.
    //! \return Returns amount of attributes of this xml node.
    virtual s32 getAttributeCount() = 0;

    //! Returns name of an attribute.
    //! \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
    //! \return Name of the attribute, 0 if an attribute with this index does not exist.
    virtual const wchar_t* getAttributeName(s32 idx) = 0;

    //! Returns the value of an attribute.
    //! \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
    //! \return Value of the attribute, 0 if an attribute with this index does not exist.
    virtual const wchar_t* getAttributeValue(s32 idx) = 0;

    //! Returns the value of an attribute.
    //! \param name: Name of the attribute.
    //! \return Value of the attribute, 0 if an attribute with this name does not exist.
    virtual const wchar_t* getAttributeValue(const wchar_t* name) = 0;

    //! Returns the value of an attribute, like getAttributeValue(), but does not
    //! return 0 if the attribute does not exist. An empty string ("") is returned then.
    //! \param name: Name of the attribute.
    //! \return Value of the attribute, and "" if an attribute with this name does not exist
    virtual const wchar_t* getAttributeValueSafe(const wchar_t* name) = 0;

    //! Returns the value of an attribute as integer.
    virtual s32 getAttributeValueAsInt(const wchar_t* name, s32 default_val = 0) = 0;
	virtual s64 getAttributeValueAsInt64(const wchar_t* name, s64 default_val = 0) = 0;
    virtual s32 getAttributeValueAsHexInt(const wchar_t* name, s32 default_val = 0) = 0;
	virtual s64 getAttributeValueAsHexInt64(const wchar_t* name, s64 default_val = 0) = 0;
    virtual u32 getAttributeValueAsUInt(const wchar_t* name, u32 default_val = 0) = 0;
	virtual u64 getAttributeValueAsUInt64(const wchar_t* name, u64 default_val = 0) = 0;
    virtual u32 getAttributeValueAsHexUInt(const wchar_t* name, u32 default_val = 0) = 0;
	virtual u64 getAttributeValueAsHexUInt64(const wchar_t* name, u64 default_val = 0) = 0;

    //! Returns the value of an attribute as float.
    virtual f32 getAttributeValueAsFloat(const wchar_t* name, f32 default_val=0.0f) = 0;

	//! Returns the value of an attribute as boolean.
    virtual bool getAttributeValueAsBool(const wchar_t* name, bool default_val=false) = 0;

	//! Returns the value of an attribute as vector. 
	virtual core::vector3df getAttributeValueAsVector(
		const wchar_t *name, core::vector3df default_val=core::vector3df(0,0,0)) = 0;

	//! Returns the value of an attribute as img::SColor. 
	virtual img::SColor getAttributeValueAsColor(
		const wchar_t *name, img::SColor default_val=img::SColor(255,0,0,0)) = 0;

	//! Returns the value of an attribute as img::SColof. 
	virtual img::SColorf getAttributeValueAsColorF(
		const wchar_t *name, img::SColorf default_val=img::SColorf(0.0f,0.0f,0.0f,1.0f)) = 0;

	//! Returns the value of the attribure as composition of the flags
	virtual u32 getAttributeValueAsFlagBits(
		const wchar_t *name, const c8 **flag_names, const u32 *flag_bits, u32 flag_count,
		u32 default_val = 0) = 0;

	virtual u32 getAttributeValueAsIndexInArray(
		const wchar_t *name, const c8 **type_names, u32 type_count,
		u32 default_val = 0) = 0;

    //! Returns the name of the current node. Only non null, if the node type is
    //! EXN_ELEMENT.
    //! \return Name of the current node or 0 if the node has no name.
    virtual const wchar_t* getName() = 0;

    //! Returns data of the current node. Only non null if the node has some
    //! data and it is of type EXN_TEXT or EXN_UNKNOWN.
    virtual const wchar_t* getNodeData() = 0;

    //! Returns if an element is an empty element, like <foo />
    virtual bool isEmtpyElement() = 0;

	//! Returns name of file.
	//! \return Returns the file name as zero terminated character string.
	virtual const c8* getFileName()  = 0; 

	//! Returns file path.
	//! \return Returns the file path as zero terminated character string.
	virtual const c8* getFilePath()  = 0; 

	//! rewind xml-reader to the bwgining od xml data
	virtual void rewindToBegin() = 0;

	//! get current position in parsing data
	virtual s32 getPos() = 0;

	//! get current position in parsing data, return true on success
	virtual bool setPos(s32 new_pos) = 0;

	//!
	virtual core::stringw getData(u32 start, u32 end) = 0;
};

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

#endif
                                                
