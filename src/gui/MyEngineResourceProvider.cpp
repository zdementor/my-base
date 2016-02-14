//|-------------------------------------------------------------------------
//| File:        MyEngineResourceProvider.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "MyEngineResourceProvider.h"
#include <CEGUIExceptions.h>
#include <io/IReadFile.h>
#include <io/IFileSystem.h>

//------------------------------------------------------------------------------------
namespace CEGUI { // Start of CEGUI namespace section
//------------------------------------------------------------------------------------

MyEngineResourceProvider::MyEngineResourceProvider() 
	: fsys(FILE_SYSTEM_PTR) 
{
	if(fsys!=0) fsys->grab();
}
//------------------------------------------------------------------------------------
MyEngineResourceProvider::~MyEngineResourceProvider(void) 
{
	if(fsys!=0)fsys->drop();
}
//------------------------------------------------------------------------------------
void MyEngineResourceProvider::loadRawDataContainer(
	const String& filename, RawDataContainer& output, const String& resourceGroup)
{
	String final_filename(getFinalFilename(filename, resourceGroup));

	if(!fsys->existFile(final_filename.c_str()))
	{
		String sMsg=(utf8*)"MyEngineResourceProvider::loadRawDataContainer - Filename supplied for loading must be valid";
		sMsg+=(utf8*)" ["+resourceGroup+(utf8*)"]";
		sMsg+=(utf8*)" ["+final_filename+(utf8*)"]";
		throw InvalidRequestException(sMsg);
	}
	else
	{
		my::u8* input;
		my::u32 input_size;
		my::io::IReadFile* f=fsys->createAndOpenFile(final_filename.c_str());
		input_size=f->getSize();
		input= new my::u8[input_size];
		f->read(input,input_size);
		output.setData(input);
		output.setSize(input_size);
		f->drop();
	}
}

//------------------------------------------------------------------------------------

void MyEngineResourceProvider::unloadRawDataContainer(RawDataContainer& rawdata)
{
	my::u8 *data = (my::u8*)rawdata.getDataPtr();
	rawdata.setData(NULL);
	rawdata.setSize(0);
	delete []data;
}

//------------------------------------------------------------------------------------
} // End of  CEGUI namespace section
//------------------------------------------------------------------------------------
