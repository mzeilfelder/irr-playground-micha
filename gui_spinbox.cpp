// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Experimenting with spinboxes (used for hunting bugs)

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace gui;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

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
			switch(event.GUIEvent.EventType)
			{
			case EGET_EDITBOX_ENTER:
				Context.ListBoxEvents->insertItem(0, L"EGET_EDITBOX_ENTER");
				break;
			case EGET_EDITBOX_CHANGED:
				Context.ListBoxEvents->insertItem(0, L"EGET_EDITBOX_CHANGED");
				break;
			case EGET_EDITBOX_MARKING_CHANGED:
				Context.ListBoxEvents->insertItem(0, L"EGET_EDITBOX_MARKING_CHANGED");
				break;
			case EGET_SPINBOX_CHANGED:
			{
				IGUISpinBox* sb = static_cast<IGUISpinBox*>(event.GUIEvent.Caller);
				irr::core::stringw msg(L"EGET_SPINBOX_CHANGED");
				msg += sb->getValue();
				msg += L" old: ";
				msg += sb->getOldValue();
				Context.ListBoxEvents->insertItem(0, msg.c_str());
				break;
			}
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
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 600));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	SAppContext context;
	context.Device = device;
	core::rect< s32 > rectEventsList(250, 20, 500, 300);
	context.ListBoxEvents = env->addListBox (rectEventsList);

	core::rect< s32 > r(10, 20, 80, 80);

	IGUISpinBox * sb = env->addSpinBox(L"5.5", r);
	sb->setDecimalPlaces(1);
	sb->setStepSize(0.1f);
	//sb->setValidateOn(0);
	//sb->setValidateOn(EGUI_SBV_ENTER|EGUI_SBV_LOSE_FOCUS);	// default
	//sb->setValidateOn(EGUI_SBV_ENTER|EGUI_SBV_LOSE_FOCUS|EGUI_SBV_CHANGE);

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

