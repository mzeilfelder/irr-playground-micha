// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Test-code for file-open dialog

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
public:
	MyEventReceiver(IGUIEnvironment* env) : DlgFileOpen(0), GuiEnv(env) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			//s32 id = event.GUIEvent.Caller->getID();
			switch(event.GUIEvent.EventType)
			{
				case EGET_DIRECTORY_SELECTED:
				{
					//IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
				}
				break;
				case EGET_FILE_SELECTED:
				{
					//IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
					DlgFileOpen = 0;
				}
				break;
				case EGET_FILE_CHOOSE_DIALOG_CANCELLED:
				{
					DlgFileOpen = 0;
				}
				break;
			default:
				break;
			}
		}
		else if (event.EventType == EET_KEY_INPUT_EVENT )
		{
			if ( event.KeyInput.PressedDown && !DlgFileOpen )
			{
				DlgFileOpen = GuiEnv->addFileOpenDialog(L"example file open dialog");
			}
		}

		return false;
	}

	IGUIFileOpenDialog* DlgFileOpen;
	IGUIEnvironment* GuiEnv;
};


int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	MyEventReceiver receiver(env);
	device->setEventReceiver(&receiver);

	receiver.DlgFileOpen = env->addFileOpenDialog(L"example file open dialog");

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
