// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Serialization tests for modal dialogs.

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _MSC_VER
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

	s32 top = 10;
	s32 default_height = 100;
	s32 default_width = 180;
	s32 default_gap = 30;
	core::rect<s32> rect(10, top, 10 + default_width, top + default_height);

	IGUIWindow * wndParent = env->addWindow (core::rect<s32>(100, 100, 300, 300), false, L"", 0, -1);
	
	IGUIWindow * wnd = NULL;
	wnd = env->addWindow (rect, /*modal*/true, L"child_window", wndParent, -1);

	rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + default_height;
	wnd = env->addWindow (rect, /*modal*/true, L"text_window2", NULL, -1);
	wnd->setVisible(false);

	env->addMessageBox(L"message box", L"I am modal", true, EMBF_OK);

	env->saveGUI ("wnd_gui.xml");
	env->clear();
	env->loadGUI ("wnd_gui.xml");

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

