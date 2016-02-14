//|-------------------------------------------------------------------------
//| File:        MyEngineResourceProvider.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef _MYENGINE_RESOURCE_PROVIDER_H_INCLUDED_
#define _MYENGINE_RESOURCE_PROVIDER_H_INCLUDED_
//------------------------------------------------------------------------------------

#include <CEGUIDefaultResourceProvider.h>
#include <io/IFileSystem.h>
#include <gui/MyCEGUISystem.h>

#include <map>

//------------------------------------------------------------------------------------
namespace CEGUI { // Start of CEGUI namespace section
//------------------------------------------------------------------------------------

class MYENGINE_GUIRENDERER_API MyEngineResourceProvider : public DefaultResourceProvider
{
	friend class MyCEGUITexture;

protected:

    my::io::IFileSystem* fsys;

public:

    // constructor
    MyEngineResourceProvider();

    // destructor
    ~MyEngineResourceProvider() ;

    void loadRawDataContainer(
        const String& filename, RawDataContainer& output, const String& resourceGroup
        ); 
	
	void unloadRawDataContainer(RawDataContainer& data);
	
    /*void setResourceGroupDirectory(const String& resourceGroup, const String& directory);

    const String& getResourceGroupDirectory(const String& resourceGroup);

    void clearResourceGroupDirectory(const String& resourceGroup);

	String getFinalFilename(const String& filename, const String& resourceGroup) const; */ 

private:

	//typedef std::map<String, String, String::FastLessCompare> ResourceGroupMap;
    //ResourceGroupMap    d_resourceGroups;
};

//------------------------------------------------------------------------------------
} // End of  CEGUI namespace section
//------------------------------------------------------------------------------------

#endif  // end of guard _MYENGINE_RESOURCE_PROVIDER_H_INCLUDED_
