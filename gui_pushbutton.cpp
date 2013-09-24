// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing pushbutton behavior.

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
	skin->setSize(EGDS_BUTTON_PRESSED_IMAGE_OFFSET_X, 5 );
	skin->setSize(EGDS_BUTTON_PRESSED_IMAGE_OFFSET_Y, 1 );
	skin->setSize(EGDS_BUTTON_PRESSED_TEXT_OFFSET_X, 5 );
	skin->setSize(EGDS_BUTTON_PRESSED_TEXT_OFFSET_Y, 10 );

	IGUIButton * btn = env->addButton(recti(20, 20, 100, 50), 0, -1, L"text");
	btn->setIsPushButton(true);
	btn->setImage( driver->getTexture("../../media/help.png") );
	btn->setUseAlphaChannel(true);

	IGUIButton * btn2 = env->addButton(recti(20, 70, 100, 100), 0, -1, L"text2");
	btn2->setIsPushButton(true);

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
