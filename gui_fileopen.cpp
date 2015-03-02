// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing the GUIFileOpenDialog.

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

struct SAppContext
{
	SAppContext() : device(0), buttonOpenFile(0), fileOpenDlg(0)
	{
	}

	IrrlichtDevice * device;
	IGUIButton * buttonOpenFile;
	IGUIFileOpenDialog * fileOpenDlg;
};


class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			if ( event.GUIEvent.EventType == EGET_BUTTON_CLICKED )
			{
				if ( event.GUIEvent.Caller == Context.buttonOpenFile )
				{
					Context.fileOpenDlg = Context.device->getGUIEnvironment()->addFileOpenDialog(L"title", true, 0);
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
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	
	SAppContext context;
	context.device = device;

	context.buttonOpenFile = env->addButton(core::rect<s32>(20, 20, 120, 40), 0, -1, L"file open");

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

