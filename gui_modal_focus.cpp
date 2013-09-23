// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing focus-behavior with modal dialogs.

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

struct SAppContext
{
    SAppContext() : device(0), quit(false), removeButton(0) {}
	IrrlichtDevice * device;
    bool quit;
	array<IGUIElement*> modals;
	IGUIButton * removeButton;
};


class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_MOUSE_INPUT_EVENT)
		{
			if ( 	!Context.modals.empty()
				&& 	event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP
				&& 	Context.removeButton->isPointInside(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y))
				)
			{
				IGUIElement* element = Context.modals.getLast();
				element->remove();
				Context.modals.erase( Context.modals.size()-1 );
			}
		}

		return false;
	}

private:
	SAppContext & Context;
};



int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	SAppContext context;
	context.device = device;
	context.removeButton = env->addButton (core::rect<s32>(300, 50, 400, 70), 0, -1, L"remove");

	s32 top = 10;
	s32 default_height = 100;
	s32 default_width = 180;
	core::rect<s32> rect(10, top, 10 + default_width, top + default_height);

	IGUIWindow * wndParent = env->addWindow (core::rect<s32>(100, 100, 300, 300), false, L"", 0, -1);

	IGUIWindow * wnd = NULL;
	wnd = env->addWindow (rect, /*modal*/true, L"child_window", 0, -1);
	context.modals.push_back(wnd);

	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);

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

