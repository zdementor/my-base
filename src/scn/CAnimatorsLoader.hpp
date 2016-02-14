//|-------------------------------------------------------------------------
//| File:        CAnimatorsLoader.hpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

bool parseTypeTag(io::IXMLReader* xml, core::stringc &Type)
{
	if (xml)
    {
		if (core::stringw("Type") == xml->getName())
        {
			Type = xml->getAttributeValue(L"value");        
            return true;
        }       
    }
    return false;
}

//----------------------------------------------------------------------------

class CAnimatorsLoader
{
private:

	static void parseRotationAnimator(
		io::IXMLReader *xml, 
		core::vector3df &rotation_per_second
		)
	{
		while(xml && xml->read())
		// парсим xml - файл
		{
			switch(xml->getNodeType())
			{
			case io::EXN_ELEMENT:
				{
					if (core::stringw("RotationPerSecond") == xml->getName())
					{
						core::stringc _x = xml->getAttributeValue(L"x");
						core::stringc _y = xml->getAttributeValue(L"y");
						core::stringc _z = xml->getAttributeValue(L"z");	
						rotation_per_second = core::vector3df( (f32)atof(_x.c_str()), (f32)atof(_y.c_str()), (f32)atof(_z.c_str()) );
					}					
				}
			}
		}	
	}

	static void parseFlyCircleAnimator(
		io::IXMLReader *xml, 
		core::vector3df &Normal, f32 &Radius, f32 &Speed
		)
	{
		while(xml && xml->read())
		// парсим xml - файл
		{
			switch(xml->getNodeType())
			{
			case io::EXN_ELEMENT:
				{
					if (core::stringw("Normal") == xml->getName())
					{
						core::stringc _x = xml->getAttributeValue(L"x");
						core::stringc _y = xml->getAttributeValue(L"y");
						core::stringc _z = xml->getAttributeValue(L"z");	
						Normal = core::vector3df( (f32)atof(_x.c_str()), (f32)atof(_y.c_str()), (f32)atof(_z.c_str()) );
					}
					else if (core::stringw("Radius") == xml->getName())
					{
						core::stringc _radius = xml->getAttributeValue(L"value");
						Radius = (f32)atof(_radius.c_str());
					}
					else if (core::stringw("Speed") == xml->getName())
					{
						core::stringc _speed = xml->getAttributeValue(L"value");
						Speed = (f32)atof(_speed.c_str());
					}
				}
			}
		}	
	}

	static void parseFlyStraightAnimator(
		io::IXMLReader *xml, 
		core::vector3df &StartPoint, core::vector3df &EndPoint, 
		s32 &TimeForWay, bool &Loop
		)
	{
		while(xml && xml->read())
		{
			switch(xml->getNodeType())
			{
			case io::EXN_ELEMENT:
				{
					if (core::stringw("StartPoint") == xml->getName())
					{
						core::stringc _x = xml->getAttributeValue(L"x");
						core::stringc _y = xml->getAttributeValue(L"y");
						core::stringc _z = xml->getAttributeValue(L"z");	
						StartPoint = core::vector3df( (f32)atof(_x.c_str()), (f32)atof(_y.c_str()), (f32)atof(_z.c_str()) );
					}
					else if (core::stringw("EndPoint") == xml->getName())
					{
						core::stringc _x = xml->getAttributeValue(L"x");
						core::stringc _y = xml->getAttributeValue(L"y");
						core::stringc _z = xml->getAttributeValue(L"z");	
						EndPoint = core::vector3df( (f32)atof(_x.c_str()), (f32)atof(_y.c_str()), (f32)atof(_z.c_str()) );
					}
					else if (core::stringw("TimeForWay") == xml->getName())
					{
						core::stringc _time = xml->getAttributeValue(L"value");
						TimeForWay = atoi(_time.c_str());
					}
					else if (core::stringw("Loop") == xml->getName())
					{
						core::stringc _loop = xml->getAttributeValue(L"value");
						Loop = (atoi(_loop.c_str())==0?false:true);
					}
				}
			}
		}
	}

	static void parseTextureAnimator(io::IXMLReader *xml, 
		core::array<core::stringc> &TextureFileName, s32 &TimePerFrame, bool &Loop
		)
	{
		while(xml && xml->read())
		// парсим xml - файл
		{
			switch(xml->getNodeType())
			{
			case io::EXN_ELEMENT:
				{
					if (core::stringw("TextureFileName") == xml->getName())
						TextureFileName.push_back( xml->getAttributeValue(L"value") );
					else if (core::stringw("TimePerFrame") == xml->getName())
					{
						core::stringc _time = xml->getAttributeValue(L"value");
						TimePerFrame = atoi(_time.c_str());
					}
					else if (core::stringw("Loop") == xml->getName())
					{
						core::stringc _loop = xml->getAttributeValue(L"value");
						Loop = atoi(_loop.c_str())==0?false:true;
					}
				}
			}
		}
	}

public:

	static scn::ISceneNodeAnimator* load(core::stringc fname)
	{
		scn::ISceneNodeAnimator *nodeAnim=0;

		core::stringc &xmlFileName = fname;

		io::IXMLReader* xml = FILE_SYSTEM.createXMLReader(xmlFileName.c_str());

		core::stringc Msg;

		if (!xml) 
		{   
			Msg.sprintf("Can't open xml-file %s", xmlFileName.c_str());
			LOGGER.log(Msg.c_str(), io::ELL_WARNING);
		}
		else
		{
			core::stringc old_tex_path = SCENE_MANAGER.getTexturesPath();

			core::stringc basePath = core::extractFilePath(xmlFileName);

			core::stringc Type="";
    
			Msg.sprintf("Loading Animator from xml-file %s . . .", xmlFileName.c_str());
			LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);

			while(xml && xml->read())
			{   
				switch(xml->getNodeType())
				{
				case io::EXN_ELEMENT:
					{	
						if (parseTypeTag(xml, Type)) { xml->drop(); xml=0; }
					}
				}
			}
			if (xml) xml->drop(); 	

			xml =  FILE_SYSTEM.createXMLReader(xmlFileName.c_str());

			if (!xml) 
			{   
				Msg.sprintf("Can't open xml-file %s", xmlFileName.c_str());
				LOGGER.log(Msg.c_str(), io::ELL_WARNING);
			}
			else	
			{
				if (Type==SceneRenderingAnimatorTypeString[ESNAT_ROTATION_ANIMATOR])
				// Rotation Animator	
				{		
					core::vector3df rotation_per_second(0,1,0);

					parseRotationAnimator(xml, rotation_per_second);
					
					nodeAnim = SCENE_MANAGER.createRotationAnimator(
						rotation_per_second);
				}
				else if (Type==SceneRenderingAnimatorTypeString[ESNAT_FLY_CIRCLE_ANIMATOR])
				// Fly Circle Animator	
				{		
					core::vector3df Normal;
					f32 Radius, Speed;

					parseFlyCircleAnimator(xml, Normal, Radius, Speed);
					
					nodeAnim = SCENE_MANAGER.createFlyCircleAnimator(
						Normal, Radius, Speed);
				}
				else if (Type==SceneRenderingAnimatorTypeString[ESNAT_FLY_STRAIGHT_ANIMATOR])
				// Fly Straight Animator
				{
					core::vector3df StartPoint, EndPoint;
					s32 TimeForWay=10000;
					bool Loop=true;

					parseFlyStraightAnimator(xml, StartPoint, EndPoint, TimeForWay, Loop);

					nodeAnim = SCENE_MANAGER.createFlyStraightAnimator(
						StartPoint, EndPoint, TimeForWay, Loop);
				}
				else if (Type==SceneRenderingAnimatorTypeString[ESNAT_TEXTURE_ANIMATOR])
				// Texture Animator
				{			
					core::array<core::stringc> TextureFileName;
					s32 TimePerFrame =100;
					bool Loop=true;

					parseTextureAnimator(xml, TextureFileName, TimePerFrame, Loop);		

					core::array<SAnimationFrame> frames;
					for (s32 g=0; g<(s32)TextureFileName.size(); ++g)
					{
						core::stringc texFName = SCENE_MANAGER.getTexturesPath();
						texFName.append(TextureFileName[g]);
						vid::ITexture* t = VIDEO_DRIVER.getTexture(texFName.c_str());
						frames.push_back(SAnimationFrame(t));
					}
					nodeAnim = SCENE_MANAGER.createTextureAnimator(
						frames, TimePerFrame, Loop);  
				}
				else
				{					
					LOGGER.log("Cant load this animator type !", io::ELL_ERROR);
				}
			}

			SCENE_MANAGER.setTexturesPath(old_tex_path);
			
			xml->drop(); 	
		}

		if (!nodeAnim)  
		{
			Msg.sprintf("Couldn't load Animator from %s", xmlFileName.c_str());
			LOGGER.log(Msg.c_str(), io::ELL_ERROR);
		}
		else
		{   
			Msg.sprintf("Animator successfully loaded from %s", xmlFileName.c_str());
			LOGGER.log(Msg.c_str(), io::ELL_INFORMATION);
		}

		return nodeAnim;		
	}
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------
