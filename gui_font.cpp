// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Test loading an xml font.

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif


int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	
	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("media_foo/fonts/droid_sans12.xml");
	if (font)
		skin->setFont(font);	
	
	env->addStaticText(L"The quick brown fox jumps over the lazy dog", core::rect<s32>(10,10, 400, 200), true, true);
	env->addEditBox (L"", recti(10,210, 400, 250) );

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, SColor(0,200,200,200));
	
			env->drawAll();
		
			driver->endScene();
		}
	}

	device->drop();

	return 0;
}

