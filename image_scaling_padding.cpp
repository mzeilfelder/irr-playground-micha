// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Do experiment with scaling and padding NPOT textures.

#include <irrlicht.h>
#include <iostream>

using namespace irr;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif


int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	//video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	video::IImage* imgLogo = driver->createImageFromFile("../../media/irrlichtlogo.jpg");
	
	video::ITexture * logo = driver->addTexture("logo", imgLogo);
	driver->disableFeature(video::EVDF_TEXTURE_NPOT, true);
	video::ITexture * logoScaled = driver->addTexture("logoscaled", imgLogo);


	// Create a texture with padding
	video::ITexture * logoPadded = 0;
	const core::dimension2du origDim = imgLogo->getDimension();
	const bool needSquare = false;
	const u32 maxTextureSize = core::min_(driver->getMaxTextureSize().Width, driver->getMaxTextureSize().Height);
	core::dimension2du dimOptimal = origDim.getOptimalSize(!driver->queryFeature(video::EVDF_TEXTURE_NPOT), needSquare, true, maxTextureSize);
	if ( dimOptimal != origDim)
	{
		video::IImage* imgLogo2 = driver->createImage(imgLogo->getColorFormat(), dimOptimal);
		memset(imgLogo2->getData(), 0, imgLogo2->getDataSizeFromFormat(imgLogo2->getColorFormat(), dimOptimal.Width, dimOptimal.Height));	// set to black
		core::vector2di pos((dimOptimal.Width-origDim.Width)/2, (dimOptimal.Height-origDim.Height)/2);
		imgLogo->copyTo(imgLogo2, pos, core::recti(0,0, origDim.Width, origDim.Height), 0);

		logoPadded =  driver->addTexture("logopadded", imgLogo2);
		imgLogo2->drop();
	}

	imgLogo->drop();

	gui::IGUIImage* uiLogo = env->addImage(core::recti(10, 200, 200, 290));
	uiLogo->setImage(logo);
	uiLogo->setScaleImage(true);

	gui::IGUIImage* uiLogoScaled = env->addImage(core::recti(10, 300, 200, 390));
	uiLogoScaled->setImage(logoScaled);
	uiLogoScaled->setScaleImage(true);

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, video::SColor(0,200,200,200));

			driver->draw2DImage(logo, core::position2d<int>(10, 10));
			driver->draw2DImage(logoScaled, core::position2d<int>(120, 10));
			driver->draw2DImage(logoPadded, core::position2d<int>(120, 50));
			driver->draw2DImage(logoScaled, core::recti(120, 100, logoScaled->getSize().Width+120, logoScaled->getSize().Height+100),
					core::recti(0,0,logoScaled->getOriginalSize().Width,logoScaled->getOriginalSize().Height));

			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
