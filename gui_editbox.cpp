// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// All kind of experiments with editboxes (used for hunting bugs)

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

enum EGUI_ID
{
	EID_EDIT_BOX,
};

struct SAppContext
{
	IrrlichtDevice * Device;
	IGUIListBox * ListBoxEvents;
};


class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			if ( id == EID_EDIT_BOX )
			{
				switch(event.GUIEvent.EventType)
				{
				case EGET_EDITBOX_ENTER:
					Context.ListBoxEvents->addItem(L"EGET_EDITBOX_ENTER");
					break;
				case EGET_EDITBOX_CHANGED:
					Context.ListBoxEvents->addItem(L"EGET_EDITBOX_CHANGED");
					break;
				case EGET_EDITBOX_MARKING_CHANGED:
					Context.ListBoxEvents->addItem(L"EGET_EDITBOX_MARKING_CHANGED");
					break;
				case EGET_SPINBOX_CHANGED:
					Context.ListBoxEvents->addItem(L"EGET_SPINBOX_CHANGED");
					break;
				default:
					break;
				}
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
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 600));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	SAppContext context;
	context.Device = device;
	core::rect< s32 > rectEventsList(100, 20, 400, 300);
	context.ListBoxEvents = env->addListBox (rectEventsList);
	core::rect< s32 > rectEditBox(10, 20, 80, 80);
	IGUIEditBox * edit1 = env->addEditBox (L"", rectEditBox, true, 0, EID_EDIT_BOX);
	edit1->setMultiLine(true);
	edit1->setWordWrap(true);

	rectEditBox += position2di(0, 100);
	IGUIEditBox * edit2 = env->addEditBox (L"", rectEditBox, false, 0, -1);
	edit2->setMultiLine(true);
	edit2->setDrawBackground(true);
	edit2->setWordWrap(true);

	rectEditBox += position2di(0, 100);
	IGUIEditBox * edit3 = env->addEditBox (L"", rectEditBox, false, 0, -1);
	edit3->setMultiLine(false);
	edit3->setDrawBackground(true);
	edit3->setWordWrap(true);
	edit3->setAutoScroll(true);


	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);


	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, SColor(0,50,100,200));

			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();

	return 0;
}

