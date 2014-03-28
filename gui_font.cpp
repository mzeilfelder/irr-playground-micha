// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Test loading an xml font.

#include <irrlicht.h>
#include <iostream>
#include "irr_key_names.h"

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
	irr::gui::IGUIListBox * listBoxEvents;
};

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_KEY_INPUT_EVENT)
		{
			IrrKeyName keyName;
			irr::core::stringw str(L"Char: ");
			str += event.KeyInput.Char;
			str += L" Code: ";
			str += core::stringw((int)event.KeyInput.Key);
			str += event.KeyInput.PressedDown ? L" D" : L" U";
			str += event.KeyInput.Shift ? L"S" : L" ";
			str += event.KeyInput.Control ? L"C " : L"  ";
			str += core::stringw( keyName.get(event.KeyInput.Key) );
			Context.listBoxEvents->insertItem(0, str.c_str(), 0);
		}

		return false;
	}

private:
	SAppContext & Context;
};

int main()
{
//	setlocale(LC_ALL, "");

	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	SAppContext context;
	context.device = device;
	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	
	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("media_foo/fonts/droid_sans12.xml");
	if (font)
		skin->setFont(font);	
	
	env->addStaticText(L"The quick brown fox jumps over the lazy dog", recti(10,10, 400, 200), true, true);
	env->addEditBox (L"", recti(10,210, 400, 250) );
	context.listBoxEvents = env->addListBox (recti(10, 260, 400, 450), 0, -1, true);

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

