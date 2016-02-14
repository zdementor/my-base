//|-------------------------------------------------------------------------
//| File:        MyBSPTool.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifdef _DEBUG
#	pragma comment (lib, "MyDevice_debug.lib")
#	pragma comment (lib, "MyCore_debug.lib")
#else
#	pragma comment (lib, "MyDevice.lib")
#	pragma comment (lib, "MyCore.lib")
#endif

//---------------------------------------------------------------------------

#include "../../inc/my.h"

//---------------------------------------------------------------------------

using namespace my;  

/*
set src_fname=robo_hangar36
set dst_fname=robo_hangar

set src_path=..\robo.media.work\baseq3\maps\
set dst_path=..\robo.media\models\maps\

set light_intens_mul=0.5
set light_radius_mul=1.0

call mybsptool.exe -rempatch %src_path%%src_fname%.bsp %src_path%%src_fname%_rempatches.bsp
call mybsptool.exe -collapse %src_path%%src_fname%_rempatches.bsp %src_path%%src_fname%_colapsed.bsp

copy %src_path%%src_fname%.map %src_path%%src_fname%_colapsed.map

call mybsptool.exe -genmaxscr   %src_path%%src_fname%_colapsed.bsp %src_path%%src_fname%_colapsed.ms %light_intens_mul% %light_radius_mul%
            
rem del  %src_path%..\lightmaps\*.* /q
mkdir %src_path%..\lightmaps\
echo call mybsptool.exe -writelmap %src_path%%src_fname%.bsp %src_path%%src_fname%_lightmapped.bsp >mybsptool_writelightmaps.bat
echo copy %src_path%%src_fname%_lightmapped.bsp %dst_path%%dst_fname%.bsp >>mybsptool_writelightmaps.bat
*/

// call mybsptool.exe -collapse ..\robo.media.work\baseq3\maps\test.bsp ..\robo.media.work\baseq3\maps\test_colapsed.bsp

//---------------------------------------------------------------------------

c8 UsageInfo[] = {
"\n\
Usage:\n\
  MyBSPTool.exe [key] [source.bsp] [dest_file_name]\n\
\n\
Keys:\n\
  -rempatch  - removing all patches from bsp file\n\
  -collapse  - collapsing vertices in BSP file\n\
  -genmaxscr - generating 3DSMAX import script\n\
  -writelmap - rewrithing lightmaps from ..\\lightmaps directory\n\
"
};

//---------------------------------------------------------------------------

void rewriteLightMaps(scn::IBSPLevelMesh *q3mesh, const c8 *src_fname, const c8 *dst_fname)
{
	core::stringc lightmaps_path = core::extractFilePath(core::stringc(src_fname));
	lightmaps_path.append("..\\lightmaps\\");

	q3mesh->rewriteLightMapsFrom(lightmaps_path.c_str());
	q3mesh->saveToFile(dst_fname);
}

//---------------------------------------------------------------------------

void collapseVertices(scn::IBSPLevelMesh *q3mesh, const c8 *dst_fname)
{
	q3mesh->collapseVertices();
	q3mesh->saveToFile(dst_fname);
}

//---------------------------------------------------------------------------

void removePatches(scn::IBSPLevelMesh *q3mesh, const c8 *dst_fname)
{
	q3mesh->removePatches();
	q3mesh->saveToFile(dst_fname);
}

//---------------------------------------------------------------------------

void genMaxScr(
	scn::IBSPLevelMesh *q3mesh, const c8 *dst_fname, 
	f32 light_intens_mul, f32 light_radius_mul
	)
{
	q3mesh->generateMaxImportScript(dst_fname, light_intens_mul, light_radius_mul);
}

//---------------------------------------------------------------------------

void doJob(
	const c8 *key, const c8 *src_fname, const c8 *dst_fname,
	c8 *param1=NULL, c8 *param2=NULL
	)
{
	char * ext_src = strrchr ( src_fname, '.' );
	
	if ( strcmp(ext_src+1, "bsp") )
	{
		return;
	}

	io::IFileSystem::getSingleton().changeWorkingDirectoryTo(
		dev::IDevice::getSingleton().getStartupDir().c_str()
		);

	scn::IAnimatedMesh *mesh = 
		scn::ISceneManager::getSingleton().getMesh(src_fname);

	if (!mesh)
		return;

	if (mesh->getAnimatedMeshType() != scn::EMT_BSP)
	{
		return;
	}	

	scn::IBSPLevelMesh *q3mesh = static_cast<scn::IBSPLevelMesh *>(mesh);
	
	if (!q3mesh)
		return;

	if (core::stringc(key)==core::stringc("-rempatch"))
	{
		removePatches(q3mesh, dst_fname);		
	}	
	else if (core::stringc(key)==core::stringc("-collapse"))
	{
		collapseVertices(q3mesh, dst_fname);
	}
	else if (core::stringc(key)==core::stringc("-genmaxscr"))
	{
		f32 light_intens_mul = param1 ? atof(param1) : 0.5f;
		f32 light_radius_mul = param2 ? atof(param2) : 1.75f;

		core::stringc scripts_path = core::extractFilePath(core::stringc(src_fname));
		scripts_path.append("..\\scripts\\");

		scn::IMaterialsManager::getSingleton().loadMaterialsFromDir(scripts_path.c_str(), false);

		genMaxScr(q3mesh, dst_fname, light_intens_mul, light_radius_mul);
	}
	else if (core::stringc(key)==core::stringc("-writelmap"))
	{
		rewriteLightMaps(q3mesh, src_fname, dst_fname);
	}
}

//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	if (argc!=4 && argc!=5 && argc!=6)
	{
		printf(UsageInfo);

		return 0;
	}	

	//------------------------------------------------------------------------
    // create device
    //------------------------------------------------------------------------

	dev::SDeviceCreationParameters creation_params; 

	if (!dev::createDevice(creation_params))
	{
		return 0;
	}

	//------------------------------------------------------------------------
    // doing all job
    //------------------------------------------------------------------------

	if (argc==4)
	{
		doJob(argv[1], argv[2], argv[3]);
	}
	else
	if (argc==5)
	{
		doJob(argv[1], argv[2], argv[3], argv[4]);
	}
	else
	if (argc==6)
	{
		doJob(argv[1], argv[2], argv[3], argv[4], argv[5]);
	}

	//------------------------------------------------------------------------
    // free memory
    //------------------------------------------------------------------------

	// Memory audit	
	core::stringc fname = dev::IDevice::getSingleton().getStartupDir();
	core::stringc fnameBefore = fname;
    fnameBefore.append("/AllocatedPtrsBeforeDrop.txt");
	printAllocNamesList((char *)fnameBefore.c_str());     

	dev::IDevice::getSingleton().drop();
    
	// Memory audit
	core::stringc fnameAfter = fname;
    fnameAfter.append("/AllocatedPtrsAfterDrop.txt");
	printAllocNamesList((char *)fnameAfter.c_str());  
	
	return 0;
}
