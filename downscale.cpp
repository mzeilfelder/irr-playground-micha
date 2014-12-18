// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Downscale a texture

#include <irrlicht.h>
#include "driverChoice.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

using namespace irr;


irr::video::ITexture* downscaleTexture(irr::video::IVideoDriver* videoDriver, irr::video::ITexture* texture, irr::u32 textureDivider)
{
	const irr::core::dimension2d<irr::u32>& texSize = texture->getSize();
	texture->grab();
	videoDriver->removeTexture(texture);
	irr::video::IImage* image = videoDriver->createImage (texture, irr::core::position2d<irr::s32>(0,0), texSize);
	irr::video::IImage* imageSmall = videoDriver->createImage(texture->getColorFormat(), texSize/textureDivider);
	image->copyToScaling(imageSmall);
	irr::video::ITexture* texSmall = videoDriver->addTexture(texture->getName(), imageSmall);
	image->drop();
	imageSmall->drop();
	texture->drop();

	return texSmall;
}


int main()
{
	irr::SIrrlichtCreationParameters params;
	params.DriverType = driverChoiceConsole();
	params.WindowSize = core::dimension2d<u32>(800, 600);
	IrrlichtDevice* device = createDeviceEx(params);

	if (!device)
		return 1;

	video::IVideoDriver* driver = device->getVideoDriver();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	video::ITexture* texture = driver->getTexture("my_media/512x512.jpg");
	texture = downscaleTexture(driver, texture, 2);

	video::ITexture* textureAlpha = driver->getTexture("my_media/glas.tga");
	textureAlpha = downscaleTexture(driver, textureAlpha, 2);

	env->addImage(texture, core::position2d<s32>(70, 70));
	env->addImage(textureAlpha, core::position2d<s32>(1, 1));

	while (device->run())
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, video::SColor(255, 0, 0, 255));

			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();
	return 0;
}
