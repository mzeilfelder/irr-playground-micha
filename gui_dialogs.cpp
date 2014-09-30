// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Show different dialog windows in Irrlicht.
// Lot of work ahead there:
// - OK, Cancel should always show up in the same place and same size (right or bottom). Can be a parameter maybe.
// - FileOpen dialog should at least allow switching drives. But really - lot's of stuff it could have, just compare with any other file open dialog.
// - FileOpen should allow to exclude hidden/system files
// - Color select ring does not work (selecting color would be easy to code, selecting position on ring harder - also middle triangle like in Gimp missing).
// - Color select also should have options to only hide on closing (others maybe as well), so it can save last data.

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
	IrrlichtDevice * device;
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
			switch(event.GUIEvent.EventType)
			{
			default:
				break;
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
	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);

	IGUIFileOpenDialog * fileOpenDlg = env->addFileOpenDialog(L"File open title", false);
	IGUIColorSelectDialog* colorSelectDlg = env->addColorSelectDialog(L"Color select title", false);

	video::ITexture * tex64 = driver->getTexture("my_media/64x64.jpg");
	env->addMessageBox (L"Caption", L"text.\text", false, gui::EMBF_OK | gui::EMBF_CANCEL , 0, -1, tex64);

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

