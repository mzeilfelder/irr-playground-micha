// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Checking key-repeat behavior in Irrlicht.

#include <irrlicht.h>
#include <iostream>

using namespace irr;

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
				std::cout << "PressedDown:";
			}
			else if( !event.KeyInput.PressedDown )
			{
				std::cout << "Released:";
			}
			std::wcout << " " << event.KeyInput.Char;
			std::cout << " " << event.KeyInput.Key;
			std::cout << " shift:" << event.KeyInput.Shift;
			std::cout << " ctrl:" << event.KeyInput.Control;

#if (IRRLICHT_VERSION_MAJOR > 1) || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)
			std::cout << " repeat:" << event.KeyInput.AutoRepeat;
			std::cout << " extended:" << event.KeyInput.Extended;
#endif

			std::cout << std::endl;
		}

		return false;
	}
};

int main()
{
	SIrrlichtCreationParameters params;
	params.WindowSize = core::dimension2d<u32>(640, 480);
	params.DriverType = video::EDT_OPENGL;
	//params.DeviceType = EIDT_SDL;
	IrrlichtDevice * device = createDeviceEx(params);
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	MyEventReceiver receiver;
	device->setEventReceiver(&receiver);

	while(device->run() && driver)
	{
	}

	device->drop();

	return 0;
}

