
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

	//if (!scr::createCoreScriptManager())
	//{
	//	printf("Error! Can not create CoreScriptManager.");
	//	return 0;	
	//}

	if (!dev::createDevice(creation_params)) 
	{
		printf("Error! Can not create MyEngine device.");
		return 0;	
	} 
	
	//------------------------------------------------------------------------
    // main loop
    //------------------------------------------------------------------------	

	DEVICE.setWindowCaption("Test02");
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
	//CORE_SCRIPT_MANAGER.drop();
    
#if MY_DEBUG_MODE 
	// Memory audit
	core::stringc fnameAfter = "LeakObjects.txt";
	printAllocNamesList((char *)fnameAfter.c_str());
#endif // #if MY_DEBUG_MODE 

	return 0;    
}
