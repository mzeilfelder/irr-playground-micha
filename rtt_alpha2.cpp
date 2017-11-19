// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Testing alpha's and RTT's

#include <irrlicht.h>
#include "driverChoice.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

using namespace irr;

int main()
{
	irr::SIrrlichtCreationParameters params;
	params.DriverType = video::EDT_OPENGL;
	//params.DriverType = video::EDT_BURNINGSVIDEO;
	params.WindowSize = core::dimension2d<u32>(1024, 768);
	params.Bits = 32;
	IrrlichtDevice* device = createDeviceEx(params);

	if (device == 0)
			return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	irr::video::ITexture* alphaTex = driver->getTexture("my_media/alpha256x256.png");
	if ( !alphaTex )
		return 1;

	video::ITexture* rttTexture = driver->addRenderTargetTexture(alphaTex->getOriginalSize(), "RTT", video::ECF_A8R8G8B8);

	while (device->run())
	{
		if (device->isWindowActive())
		{
			//driver->beginScene(true, true, video::SColor(255, 255, 255, 255));
			driver->beginScene(true, true, video::SColor(255, 0, 0, 0));

			const core::rect<s32> sourceRect(0,0,256,256);
			const video::SColor imgColor(255,255,255,255); // white does nothing

			// draw texture into RTT
			driver->setRenderTarget(rttTexture, true, true, video::SColor(0, 0, 0, 0));
			driver->draw2DImage(alphaTex, core::vector2di(0,0), sourceRect, 0, imgColor, true);
			driver->setRenderTarget(0, false, false, video::SColor(0, 0, 0, 0));

			// Draw texture without RTT
			driver->draw2DImage(alphaTex, core::vector2di(0,0), sourceRect, 0, imgColor, true);

			// Draw RTT
			driver->draw2DImage(rttTexture, core::vector2di(512, 0), sourceRect, 0, imgColor, true);

			driver->endScene();
		}
	}

	device->drop();
	return 0;
}
