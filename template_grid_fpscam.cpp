// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Template for a grid + fps camera.
// use: # make GRID=1

#include <irrlicht.h>
#include "CGridSceneNode.h"
 
using namespace irr;
using namespace core;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
 
int main()
{
	IrrlichtDevice *  Device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800,600));
	if (!Device)
		return false;
   
	scene::ISceneManager* smgr = Device->getSceneManager();
	video::IVideoDriver * videoDriver = Device->getVideoDriver ();
   
	irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );
	camera->updateAbsolutePosition();
	irr::core::vector3df center(0,0,0);	
	camera->setTarget(center);
	camera->updateAbsolutePosition();
	camera->setPosition(center+irr::core::vector3df(0, 50, -100));
	camera->updateAbsolutePosition();
	
	CGridSceneNode * gridNode = new CGridSceneNode(smgr-> getRootSceneNode(), smgr, -1);
	gridNode->drop();

	while ( Device->run() )
	{
		if ( Device->isWindowActive() )
		{
			videoDriver->beginScene();

			smgr->drawAll();

			videoDriver->endScene();
		}

		Device->sleep( 5 );
	}

	Device->closeDevice();
	Device->drop();
	Device = NULL;

	return 0;
}
