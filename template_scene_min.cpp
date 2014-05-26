// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Template for a minimal Irrlicht scene - just a camera, light + node.

#include <irrlicht.h>
#include <iostream>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


int main(int argc, char *argv[])
{
	IrrlichtDevice * Device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(640,480) );
	if (!Device)
		return false;
	
	scene::ISceneManager* smgr = Device->getSceneManager();
	gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();
    video::IVideoDriver* videoDriver =  Device->getVideoDriver();	

	smgr->addCameraSceneNode (0, core::vector3df(0, 0, 0),
								core::vector3df(0, 0, 100),
								-1);

	smgr->addCubeSceneNode (30.0f, 0, -1, 
	                        core::vector3df(0, 20, 100),
							core::vector3df(45, 45, 45),
							core::vector3df(1.0f, 1.0f, 1.0f));

	smgr->addLightSceneNode(0, core::vector3df(0, 0, 0),
							video::SColorf(1.0f, 1.0f, 1.0f),
							100.0f);

	while ( Device->run() )
	{
		if ( Device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			smgr->drawAll();
			guiEnv->drawAll();

			videoDriver->endScene();
		}
		Device->sleep( 1 );
	}
	
	Device->closeDevice();
	Device->drop();
	
	return 0;
}
