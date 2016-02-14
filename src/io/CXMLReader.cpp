//|-------------------------------------------------------------------------
//| File:        CXMLReader.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CXMLReader.h"

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

CXMLReader::CXMLReader(IReadFile* file)
: CTextReader(file), P(0), TextBegin(0), TextSize(0), CurrentNodeType(EXN_NONE)
{
#if MY_DEBUG_MODE  
	IXMLReader::setClassName("CXMLReader IXMLReader");
	CTextReader::setClassName("CXMLReader CTextReader");
#endif
    
    P = getText();
    TextBegin = P;
    TextSize = getTextSize();
}

//----------------------------------------------------------------------------

CXMLReader::~CXMLReader()
{
}

//----------------------------------------------------------------------------

bool CXMLReader::read()
{
	wchar_t* old_pos = P;

    if ((u32)(P - TextBegin) < TextSize - 1)
    {
        parseCurrentNode();
        //++P;

		if (old_pos==P) return false;

        return true;
    }

    return false;
}

//----------------------------------------------------------------------------

void CXMLReader::parseCurrentNode()
{
    wchar_t* start = P;

    while(*P != L'<' && *P)
        ++P;

    if (!*P)
        return;

    if (P - start > 2)
    {
        // we found some text
        setText(start, P);
        --P;
        return;
    }

    ++P;

    switch(*P)
    {
    case L'/':
        parseClosingXMLElement(); break;
    case L'?':
		{
			wchar_t* start_header = P;
			ignoreDefinition(); 
			wchar_t* end_header = P;

			setText(++start_header, end_header-=2);
		}
		break;
    case L'!':
        parseComment(); break;
    default:
        parseOpeningXMLElement();
        break;
    }
}

//----------------------------------------------------------------------------

void CXMLReader::setText(wchar_t* start, wchar_t* end)
{
    core::stringw s(start, (s32)(end - start));
    NodeName = replaceSpecialCharacters(s);
    CurrentNodeType = EXN_TEXT;
}

//----------------------------------------------------------------------------

void CXMLReader::ignoreDefinition()
{
    CurrentNodeType = EXN_UNKNOWN;

    while(*P != L'>')
        ++P;

    ++P;
}

//----------------------------------------------------------------------------

void CXMLReader::parseComment()
{
    CurrentNodeType = EXN_COMMENT;
    P += 1;

    wchar_t *pCommentBegin = P;

    s32 count = 1;

    while(count)
    {
        if (*P == L'>')
            --count;
        else
        if (*P == L'<')
            ++count;

        ++P;
    }

    P -= 3;
    NodeName = core::stringw(pCommentBegin+2, (s32)(P - pCommentBegin-2));
    P += 3;
}

//----------------------------------------------------------------------------

void CXMLReader::parseOpeningXMLElement()
{
    CurrentNodeType = EXN_ELEMENT;
    IsEmptyElement = false;
    Attributes.clear();

    // find name
    const wchar_t* startName = P;

    while(*P != L'>' && !isWhiteSpace(*P))
        ++P;

    const wchar_t* endName = P;

    // find Attributes
    while(*P != L'>')
    {
        if (isWhiteSpace(*P))
            ++P;
        else
        {
            if (*P != L'/')
            {
                // we've got an attribute

                // read the attribute names
                const wchar_t* attributeNameBegin = P;

                while(!isWhiteSpace(*P) && *P != L'=')
                    ++P;

                const wchar_t* attributeNameEnd = P;
                ++P;

                // read the attribute value
                while(*P != L'\"')
                    ++P;

                ++P;
                const wchar_t* attributeValueBegin = P;
                
                while(*P != L'\"')
                    ++P;

                const wchar_t* attributeValueEnd = P;
                ++P;

                SAttribute attr;
                attr.Name = core::stringw(attributeNameBegin, attributeNameEnd - attributeNameBegin);
                core::stringw s(attributeValueBegin, attributeValueEnd - attributeValueBegin);
                attr.Value = replaceSpecialCharacters(s);
                Attributes.push_back(attr);
            }
            else
            {
                // tag is closed directly
                ++P;
                IsEmptyElement = true;
                break;
            }
        }
    }

    NodeName = core::stringw(startName, (s32)(endName - startName));

    ++P;
}

//----------------------------------------------------------------------------

void CXMLReader::parseClosingXMLElement()
{
    CurrentNodeType = EXN_ELEMENT_END;
    IsEmptyElement = false;
    Attributes.clear();

    ++P;
    const wchar_t* pBeginClose = P;

    while(*P != L'>')
        ++P;

    NodeName = core::stringw(pBeginClose, (s32)(P - pBeginClose));
    ++P;
}

//----------------------------------------------------------------------------

EXML_NODE CXMLReader::getNodeType()
{
    return CurrentNodeType;
}

//----------------------------------------------------------------------------

s32 CXMLReader::getAttributeCount()
{
    return Attributes.size();
}

//----------------------------------------------------------------------------

const wchar_t* CXMLReader::getAttributeName(s32 idx)
{
    if (idx < 0 || idx >= (s32)Attributes.size())
        return 0;

    return Attributes[idx].Name.c_str();
}

//----------------------------------------------------------------------------

const wchar_t* CXMLReader::getAttributeValue(s32 idx)
{
    if (idx < 0 || idx >= (s32)Attributes.size())
        return 0;

    return Attributes[idx].Value.c_str();
}

//----------------------------------------------------------------------------

const wchar_t* CXMLReader::getAttributeValue(const wchar_t* name)
{
    SAttribute* attr = getAttributeByName(name);
    if (!attr)
        return 0;

    return attr->Value.c_str();
}

//----------------------------------------------------------------------------

const wchar_t* CXMLReader::getAttributeValueSafe(const wchar_t* name)
{
    SAttribute* attr = getAttributeByName(name);
    if (!attr)
        return L"";

    return attr->Value.c_str();
}

//----------------------------------------------------------------------------

s32 CXMLReader::getAttributeValueAsInt(const wchar_t* name, s32 default_val)
{
    SAttribute *attr = getAttributeByName(name);
    if (!attr)
        return default_val;
    static core::stringc c;
	c = attr->Value.c_str();
    return atoi(c.c_str());
}

//----------------------------------------------------------------------------

s64 CXMLReader::getAttributeValueAsInt64(const wchar_t* name, s64 default_val)
{
    SAttribute *attr = getAttributeByName(name);
    if (!attr)
        return default_val;
    static core::stringc c;
	c = attr->Value.c_str();
	s64 val = 0;
	if (sscanf(c.c_str(), "%I64d", &val) != 1)
		val = default_val;
    return val;
}

//----------------------------------------------------------------------------

s32 CXMLReader::getAttributeValueAsHexInt(const wchar_t* name, s32 default_val)
{
    SAttribute *attr = getAttributeByName(name);
    if (!attr)
        return default_val;
    static core::stringc c;
	c = attr->Value.c_str();
	s32 val = 0;
	if (sscanf(c.c_str(), "%X", &val) != 1)
		val = default_val;
    return val;
}

//----------------------------------------------------------------------------

s64 CXMLReader::getAttributeValueAsHexInt64(const wchar_t* name, s64 default_val)
{
    SAttribute *attr = getAttributeByName(name);
    if (!attr)
        return default_val;
    static core::stringc c;
	c = attr->Value.c_str();
	s64 val = 0;
	if (sscanf(c.c_str(), "%I64X", &val) != 1)
		val = default_val;
    return val;
}



//----------------------------------------------------------------------------

u32 CXMLReader::getAttributeValueAsUInt(const wchar_t* name, u32 default_val)
{
    SAttribute *attr = getAttributeByName(name);
    if (!attr)
        return default_val;
    static core::stringc c;
	c = attr->Value.c_str();
	u32 val = 0;
	if (sscanf(c.c_str(), "%u", &val) != 1)
		val = default_val;
    return val;
}

//----------------------------------------------------------------------------

u64 CXMLReader::getAttributeValueAsUInt64(const wchar_t* name, u64 default_val)
{
    SAttribute *attr = getAttributeByName(name);
    if (!attr)
        return default_val;
    static core::stringc c;
	c = attr->Value.c_str();
	u64 val = 0;
	if (sscanf(c.c_str(), "%I64u", &val) != 1)
		val = default_val;
    return val;
}

//----------------------------------------------------------------------------

u32 CXMLReader::getAttributeValueAsHexUInt(const wchar_t* name, u32 default_val)
{
    SAttribute *attr = getAttributeByName(name);
    if (!attr)
        return default_val;
    static core::stringc c;
	c = attr->Value.c_str();
	u32 val = 0;
	if (sscanf(c.c_str(), "%X", &val) != 1)
		val = default_val;
    return val;
}

//----------------------------------------------------------------------------

u64 CXMLReader::getAttributeValueAsHexUInt64(const wchar_t* name, u64 default_val)
{
    SAttribute *attr = getAttributeByName(name);
    if (!attr)
        return default_val;
    static core::stringc c;
	c = attr->Value.c_str();
	u64 val = 0;
	if (sscanf(c.c_str(), "%I64X", &val) != 1)
		val = default_val;
    return val;
}

//----------------------------------------------------------------------------

f32 CXMLReader::getAttributeValueAsFloat(const wchar_t* name, f32 default_val)
{
    SAttribute* attr = getAttributeByName(name);
    if (!attr)
        return default_val;
    static core::stringc c;
	c = attr->Value.c_str();
    return core::fast_atof(c.c_str());
}

//----------------------------------------------------------------------------

bool CXMLReader::getAttributeValueAsBool(const wchar_t* name, bool default_val)
{
    SAttribute* attr = getAttributeByName(name);

    if (!attr)
        return default_val;

    static core::stringc c;
	c = attr->Value.c_str();

	s32 v = atoi(c.c_str());

	return v==0?false:true;
}

//----------------------------------------------------------------------------

core::vector3df CXMLReader::getAttributeValueAsVector(
	const wchar_t* name, core::vector3df default_val)
{
    SAttribute* attr = getAttributeByName(name);

    if (!attr)
        return default_val;

	const char seps[]   = ",";

    static core::stringc c;
	c = attr->Value.c_str();
	char ch[1024];
	u32 sz = c.size() < (sizeof(ch) - 1) ? c.size() : (sizeof(ch) - 1);
	memcpy(ch, c.c_str(), sz);
	ch[sz] = 0;
		
	char* tooken0 = strtok(ch,   seps);
	char* tooken1 = strtok(NULL, seps);
	char* tooken2 = strtok(NULL, seps);

	if (tooken0&&tooken1&&tooken2)
	{
		return core::vector3df( atof(tooken0), atof(tooken1), atof(tooken2) );
	}

	return default_val;
}

//----------------------------------------------------------------------------

img::SColor CXMLReader::getAttributeValueAsColor(
	const wchar_t* name, img::SColor default_val)
{
	SAttribute* attr = getAttributeByName(name);

    if (!attr)
        return default_val;

	const char seps[]   = ",";

    static core::stringc c;
	c = attr->Value.c_str();
	char ch[1024];
	u32 sz = c.size() < (sizeof(ch) - 1) ? c.size() : (sizeof(ch) - 1);
	memcpy(ch, c.c_str(), sz);
	ch[sz] = 0;
		
	char* tooken0 = strtok(ch,   seps);
	char* tooken1 = strtok(NULL, seps);
	char* tooken2 = strtok(NULL, seps);
	char* tooken3 = strtok(NULL, seps);

	if (tooken0&&tooken1&&tooken2&&tooken3)
		return img::SColor( atoi(tooken3), atoi(tooken0), atoi(tooken1), atoi(tooken2) );

	return default_val;
}

//----------------------------------------------------------------------------

img::SColorf CXMLReader::getAttributeValueAsColorF(
	const wchar_t* name, img::SColorf default_val)
{
	SAttribute* attr = getAttributeByName(name);

    if (!attr)
        return default_val;

	const char seps[]   = ",";

    static core::stringc c;
	c = attr->Value.c_str();
	char ch[1024];
	u32 sz = c.size() < (sizeof(ch) - 1) ? c.size() : (sizeof(ch) - 1);
	memcpy(ch, c.c_str(), sz);
	ch[sz] = 0;
		
	char* tooken0 = strtok(ch,   seps);
	char* tooken1 = strtok(NULL, seps);
	char* tooken2 = strtok(NULL, seps);
	char* tooken3 = strtok(NULL, seps);

	if (tooken0 && tooken1 && tooken2 && tooken3)
		return img::SColorf(
			atof(tooken0), atof(tooken1), atof(tooken2), atof(tooken3));

	return default_val;
}

//----------------------------------------------------------------------------

u32 CXMLReader::getAttributeValueAsFlagBits(
	const wchar_t *name, const c8 **flag_names, const u32 *flag_bits, u32 flag_count,
	u32 default_val)
{
	SAttribute* attr = getAttributeByName(name);

    if (!attr)
        return default_val;

	u32 flags = 0;

	const char seps[]   = ",";

    static core::stringc c;
	c = attr->Value.c_str();
	char ch[4096];
	u32 sz = c.size() < (sizeof(ch) - 1) ? c.size() : (sizeof(ch) - 1);
	memcpy(ch, c.c_str(), sz);
	ch[sz] = 0;
		
	char *tooken = strtok(ch, seps);
	bool get_data = false;
	while (tooken)
	{
		core::const_stringc tooken_str(tooken);
		for (u32 i = 0; i < flag_count; i++)
		{
			if (tooken_str == flag_names[i])
			{
				flags |= flag_bits[i];
				get_data = true;
				break;
			}
		}
		tooken = strtok(NULL, seps);
	}
	if (!get_data)
		flags = default_val;
	return flags;
}

//----------------------------------------------------------------------------

u32 CXMLReader::getAttributeValueAsIndexInArray(
	const wchar_t *name, const c8 **type_names, u32 type_count,
	u32 default_val)
{
	SAttribute *attr = getAttributeByName(name);
    if (!attr)
        return default_val;
	for (u32 i = 0; i < type_count; i++)
		if (attr->Value == type_names[i])
			return i;
	return default_val;
}

//----------------------------------------------------------------------------

const wchar_t* CXMLReader::getName()
{
    return NodeName.c_str();
}

//----------------------------------------------------------------------------

const wchar_t* CXMLReader::getNodeData()
{
    return NodeName.c_str();
}

//----------------------------------------------------------------------------

bool CXMLReader::isEmtpyElement()
{
    return IsEmptyElement;
}

//----------------------------------------------------------------------------

CXMLReader::SAttribute* CXMLReader::getAttributeByName(const wchar_t* name)
{
    if (!name)
        return 0;

    core::stringw n = name;

    for (s32 i=0; i<(s32)Attributes.size(); ++i)
        if (Attributes[i].Name == n)
            return &Attributes[i];

    return 0;
}

//----------------------------------------------------------------------------

const CXMLReader::XMLSpecialCharacters XMLRSChar[] = 
{
    // all start with &
    { L'&', L"amp;" },
    { L'<', L"lt;" },
    { L'>', L"gt;" },
    { L'"', L"quot;" },
    { L' ', 0}
};

//----------------------------------------------------------------------------

core::stringw CXMLReader::replaceSpecialCharacters(core::stringw& origstr)
{
    s32 pos = origstr.find_first(L'&');
    s32 oldPos = 0;

    if (pos == -1)
        return origstr;

    core::stringw newstr;

    while (pos != -1 && pos < (s32)origstr.size() - 2)
    {
        // check if it is one of the special characters

        s32 specialChar = -1;
        for (s32 i=0; XMLRSChar[i].Symbol != 0; ++i)
        {
            const wchar_t* p = &origstr.c_str()[pos]+1;
            if (!wcsncmp(p, XMLRSChar[i].Symbol, wcslen(XMLRSChar[i].Symbol)))
            {
                specialChar = i;
                break;
            }
        }

        if (specialChar != -1)
        {
            newstr.append(origstr.sub_string(oldPos, pos - oldPos));
            newstr.append(XMLRSChar[specialChar].Character);
            pos += (s32)wcslen(XMLRSChar[specialChar].Symbol) + 1;
        }
        else
        {
            newstr.append(origstr.sub_string(oldPos, pos - oldPos + 1));
            pos += 1;
        }

        // find next &
        oldPos = pos;
        pos = origstr.find_next(L'&', pos);      
    }

    if (oldPos < (s32)origstr.size())
        newstr.append(origstr.sub_string(oldPos, origstr.size()-oldPos));

    return newstr;
}

//----------------------------------------------------------------------------

void CXMLReader::rewindToBegin()
{
	setPos(0);
}

//----------------------------------------------------------------------------

s32 CXMLReader::getPos()
{
	return P-TextBegin;
}

//----------------------------------------------------------------------------

bool CXMLReader::setPos(s32 new_pos)
{
	if (new_pos>=0 && new_pos<(s32)TextSize)
	{
		P = TextBegin + new_pos;

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

core::stringw CXMLReader::getData(u32 start, u32 end)
{
	if (end<start)
		return L"";

	return core::stringw(TextBegin+start, end-start+1);
}

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

