// Written by RdR, somewhat rewritten by Michael Zeilfelder.
// See: http://irrlicht.sourceforge.net/forum/viewtopic.php?f=7&t=45901&p=264967
// It's about what happens when rendering alpha's to rtt's.

#include <irrlicht.h>
#include "driverChoice.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

using namespace irr;

int main()
{
	// ask user for driver
	//video::E_DRIVER_TYPE driverType = driverChoiceConsole();
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;

	// create device with full flexibility over creation parameters
	// you can add more parameters if desired, check irr::SIrrlichtCreationParameters
	irr::SIrrlichtCreationParameters params;
	params.DriverType = driverType;
	params.WindowSize = core::dimension2d<u32>(1024, 768);
	params.Bits = 32;
	IrrlichtDevice* device = createDeviceEx(params);

	if (device == 0)
			return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	// Create RTT
	core::dimension2du dimension = core::dimension2du(1024, 200);

	video::ITexture* texture = driver->addRenderTargetTexture(dimension, "RTT", video::ECF_A8R8G8B8);

	gui::IGUIFont* font = env->getFont("../../media/fontlucida.png"); // Has alpha

	while (device->run())
	{
		if (device->isWindowActive())
		{
			if (texture)
			{
				driver->setRenderTarget(texture, true, true, video::SColor(0, 0, 0, 0));
				driver->draw2DRectangle(video::SColor(150, 0, 0, 0), core::recti(0, 0, 800, 180));
				font->draw(L"Lorem ipsum dolor", core::recti(10, 0, 100, 20), video::SColor(255, 255, 255, 255));

				driver->setRenderTarget(0, false, false, 0);
			}

			driver->beginScene(true, true, video::SColor(255, 255, 255, 255));

			// Draw fonts normal
			driver->draw2DRectangle(video::SColor(150, 0, 0, 0), core::recti(0, 200, 800, 380));
			font->draw(L"Lorem ipsum dolor", core::recti(10, 200, 100, 220), video::SColor(255, 255, 255, 255));

			// Draw RTT
			const video::SColor color = video::SColor(255, 255, 255, 255);
			driver->draw2DImage(texture, core::vector2di(0, 0), core::recti(0, 0, texture->getOriginalSize().Width, texture->getOriginalSize().Height), 0, color, true);


			driver->endScene();
		}
	}

	device->drop();
	return 0;
}
