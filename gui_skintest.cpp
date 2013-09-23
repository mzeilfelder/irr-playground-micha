// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing a few skin drawing functions with different parameters directly.

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
	IGUISkin * skin = env->getSkin();
	core::rect<s32> rect;
	
	rect = core::rect<s32>(10, 210, 60, 260);
	env->addListBox (rect, 0, -1, true);
	rect = core::rect<s32>(110, 210, 160, 260);
	env->addListBox (rect, 0, -1, false);

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, SColor(0,0,0,200));
	
			env->drawAll();
			
			video::SColor bgcolor(255, 50, 150, 50);
			
			//skin->draw3DSunkenPane(NULL, bgcolor, flat, fillBackGround, rect);
			rect = core::rect<s32>(10, 10, 60, 60);
			skin->draw3DSunkenPane(NULL, bgcolor, true, true, rect);
			rect = core::rect<s32>(110, 10, 160, 60);
			skin->draw3DSunkenPane(NULL, bgcolor, true, false, rect);
			rect = core::rect<s32>(10, 110, 60, 160);
			skin->draw3DSunkenPane(NULL, bgcolor, false, true, rect);
			rect = core::rect<s32>(110, 110, 160, 160);
			skin->draw3DSunkenPane(NULL, bgcolor, false, false, rect);
			
		
			driver->endScene();
		}
	}

	device->drop();

	return 0;
}

