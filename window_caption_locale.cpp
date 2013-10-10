// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Hunting bug #424: http://sourceforge.net/p/irrlicht/bugs/424/
// Problem is that (at least on X11) IrrlichtDevice::setWindowCaption needs a call to setlocale first to to work with certain strings.
// No idea if it can be improved.

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
//	setlocale(LC_ALL, "");
	
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	
//	device->setWindowCaption(L"Old Caption");
	device->setWindowCaption(L"A ½ of a New Caption");
	
	env->addStaticText(L"A ½ of a New Caption", core::rect< s32 >(10, 10, 100, 30) );

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
