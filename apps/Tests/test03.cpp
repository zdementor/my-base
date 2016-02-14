
#include <my.h>

#if MY_DEBUG_MODE
#	ifdef VLD_DEBUG
#		undef VLD_DEBUG
#	endif
//#	define VLD_DEBUG	
#	ifndef VLD_DEBUG
#		define _CRTDBG_MAP_ALLOC
#		include <stdlib.h>
#		include <crtdbg.h>
#	else
#		include "vld.h"
#	endif
#endif

using namespace my;

//-------------------------------------------------------------------------

class MyEventReceiver : public io::IEventReceiver
{
public:
    virtual bool OnEvent(const io::SEvent &event)
	{	
		if(event.EventType==io::EET_KEY_INPUT_EVENT)
		{
			if (event.KeyInput.Event == io::EKIE_KEY_PRESSED_UP)
			{				
				if (event.KeyInput.Key == io::EKC_ESCAPE)
				{
					DEVICE.close();
					return true;
				}	
			}
		}
		return false;
    }
}
receiver;

struct SMediaEntry
{
	res::E_MEDIA_TYPE	type;
	c8					*dir;
};

SMediaEntry MediaEntry[res::E_MEDIA_TYPE_COUNT] =
{
	{res::EMT_ROOT,            "../test.media/"  },
	{res::EMT_TEXTURES,        "textures/"       },
	{res::EMT_FONTS,           "gui/fonts/"      },
	{res::EMT_MESHES,          "models/"         },
	{res::EMT_XML_SCRIPTS,     "xml/"            },
	{res::EMT_GAME_SCRIPTS,    "scripts/"        },
	{res::EMT_SOUND_TRACKS,    "sounds/tracks/"  },
	{res::EMT_SOUND_EFFECTS,   "sounds/effects/" },
	{res::EMT_TEMP_DATA,       "temp/"           },
	{res::EMT_MATERIALS,       "materials/"      },
	{res::EMT_GPU_PROGRAMS,    "materials/gpu/"  },
};

//-------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{       
#if MY_DEBUG_MODE

	#ifndef VLD_DEBUG 

	//_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);

	int tmpDbgFlag; 

	HANDLE hLogFile = CreateFile(
		"VS_log.txt",GENERIC_WRITE,FILE_SHARE_WRITE, 
		NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL
		); 
	_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW | _CRTDBG_MODE_DEBUG); 
	_CrtSetReportMode(_CRT_WARN,_CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG); 
	_CrtSetReportMode(_CRT_ERROR,_CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW | _CRTDBG_MODE_DEBUG); 

	_CrtSetReportFile(_CRT_ASSERT,hLogFile); 
	_CrtSetReportFile(_CRT_WARN,hLogFile); 
	_CrtSetReportFile(_CRT_ERROR,hLogFile); 

	tmpDbgFlag=_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); 
	tmpDbgFlag|=_CRTDBG_ALLOC_MEM_DF; 
	tmpDbgFlag|=_CRTDBG_DELAY_FREE_MEM_DF; 
	tmpDbgFlag|=_CRTDBG_LEAK_CHECK_DF; 
	
	_CrtSetDbgFlag(tmpDbgFlag);

	//_CrtSetBreakAlloc(811031); 

	#endif
#endif 

	//-------------------------------------------------------------------------
	// show MyEngine Setup
	//-------------------------------------------------------------------------
   	
	vid::E_DRIVER_TYPE driverType = vid::EDT_OPENGL21;	
	core::dimension2d<s32> resolution(640, 480);
	s32 colorBit=32;
	s32 Flags=0;
	vid::E_TEXTURE_FILTER TexFilter = vid::ETF_NONE;

	Flags |= dev::EDCF_SOUND_ON;

	dev::SDeviceCreationParameters creation_params; 

	creation_params.DriverType		= driverType;
	creation_params.WindowSize		= resolution;
	creation_params.Bits			= colorBit;
	creation_params.TextureFilter	= TexFilter;
	creation_params.Flags			= Flags;
	creation_params.EventReceiver	= &receiver;
	creation_params.WindowHandle	= 0;

	//----------------------------------------------------
	// create MyEngine device and exit if creation failed
	//----------------------------------------------------

	if (!dev::createDevice(creation_params)) 
	{
		printf("Error! Can not create MyEngine device.");
		return 0;	
	} 

	for (u32 i = 0 ; i < sizeof(MediaEntry) /sizeof(*MediaEntry); i++)
		RESOURCE_MANAGER.setMediaDirRel(
			MediaEntry[i].type, MediaEntry[i].dir);


	const c8 *load_files[] = { "test_image.dds", "test_image.tga", "test_image.jpg", "test_image.bmp" };

	for (u32 i = 0; i < sizeof(load_files) / sizeof(*load_files); i++)
	{
		img::IImage *image = IMAGE_LIBRARY.getImage(
			core::stringc().sprintf("%s%s",
				RESOURCE_MANAGER.getMediaDirFull(res::EMT_TEXTURES),
				load_files[i]).c_str());

		core::stringc save_file = load_files[i];
		const c8 *save_exts[] = { "dds", "tga", "jpg", "bmp" };

		save_file.replace_char('.', '_');

		for (u32 j = 0; image && j < sizeof(save_exts) / sizeof(*save_exts); j++)
		{
			IMAGE_LIBRARY.saveImageToFile(
				image,
				core::stringc().sprintf("%sout_%s.%s",
					RESOURCE_MANAGER.getMediaDirFull(res::EMT_TEXTURES),
					save_file.c_str(), save_exts[j]).c_str());
		}
	}
	//------------------------------------------------------------------------
    // main loop
    //------------------------------------------------------------------------	

	DEVICE.setWindowCaption("Test03");
	DEVICE.show();

	while (DEVICE.run())
	{
		GAME_MANAGER.preRenderFrame();
		SCENE_MANAGER.preRenderScene();
		VIDEO_DRIVER.render();
		VIDEO_DRIVER.swapBuffers();
		SCENE_MANAGER.postRenderScene();
		GAME_MANAGER.postRenderFrame();
	}
	  
	//------------------------------------------------------------------------
    // free memory
    //------------------------------------------------------------------------	
	
	DEVICE.drop();
    
#if MY_DEBUG_MODE 
	// Memory audit
	core::stringc fnameAfter = "LeakObjects.txt";
	printAllocNamesList((char *)fnameAfter.c_str());
#endif // #if MY_DEBUG_MODE 

	return 0;    
}
