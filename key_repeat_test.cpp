// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Checking key-repeat behavior in Irrlicht.

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

class MyEventReceiver : public IEventReceiver
{
	bool OnEvent( const SEvent &event )
	{
		if (event.EventType == EET_KEY_INPUT_EVENT) 
		{
			if( event.KeyInput.PressedDown )
			{
				std::cout << "PressedDown:" << event.KeyInput.Key << std::endl;
			}
			else if( !event.KeyInput.PressedDown )
			{
				std::cout << "!PressedDown:" << event.KeyInput.Key  << std::endl;
			}
		}
	
		return false;
	}
};

int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	MyEventReceiver receiver;
	device->setEventReceiver(&receiver);
	
	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			//			device->sleep(1000);
			//
		}
	}

	device->drop();

	return 0;
}

