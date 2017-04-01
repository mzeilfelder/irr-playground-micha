// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Template for loading a model + a fps camera.

#include <irrlicht.h>

using namespace irr;
using namespace core;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

int main()
{
	IrrlichtDevice *  device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800,600));
	if (!device)
		return false;

	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver * videoDriver = device->getVideoDriver ();

	irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );
	camera->updateAbsolutePosition();
	camera->setTarget(core::vector3df(0,0,0));
	camera->updateAbsolutePosition();
	camera->setPosition(irr::core::vector3df(0, 50, -100));
	camera->updateAbsolutePosition();

	irr::video::SMaterial m;
	m.Lighting = false;
	m.Thickness = 5.f;

	s32 lastFPS = -1;
	while ( device->run() )
	{
		if ( device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			videoDriver->setMaterial(m);
			videoDriver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

			for ( float f=1.f; f<10000.f; f += 1.1f )
				videoDriver->draw3DBox( core::aabbox3df(-f, -f, -f, f, f, f), video::SColor(255, f, 255-f, 127+f));

			smgr->drawAll();

			videoDriver->endScene();

			s32 fps = videoDriver->getFPS();
			if (lastFPS != fps)
			{
				core::stringw str = L"FPS: ";
				str += fps;
				device->setWindowCaption(str.c_str());
				lastFPS = fps;
			}
		}

		device->sleep( 5 );
	}

	device->closeDevice();
	device->drop();
	device = NULL;

	return 0;
}
