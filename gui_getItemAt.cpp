// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing getItemAt functions.

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
	IrrlichtDevice * device;
	IGUIStaticText* output;
	IGUIListBox * lb1;
	IGUIListBox * lb2;
	IGUIListBox * lb3;
	IGUIListBox * lb4;
	IGUIComboBox * combo;
};

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_MOUSE_INPUT_EVENT )
		{
			stringw text;

			s32 id = Context.lb1->getItemAt(event.MouseInput.X, event.MouseInput.Y);
			if ( id >= 0 )
			{
				text += L"lb1: ";
				text += stringw(id);
			}
			
			id = Context.lb2->getItemAt(event.MouseInput.X, event.MouseInput.Y);
			if ( id >= 0 )
			{
				text += L"lb2: ";
				text += stringw(id);
			}
			
			id = Context.lb3->getItemAt(event.MouseInput.X, event.MouseInput.Y);
			if ( id >= 0 )
			{
				text += L"lb3: ";
				text += stringw(id);
			}
			
			id = Context.lb4->getItemAt(event.MouseInput.X, event.MouseInput.Y);
			if ( id >= 0 )
			{
				text += L"lb4: ";
				text += stringw(id);
			}
					
			Context.output->setText( text.c_str() );
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
	context.output = env->addStaticText (L"", core::rect<s32>(10,450, 200, 470), true);
	context.lb1 = env->addListBox(core::rect<s32>(10,10, 120, 200), 0);
	context.lb2 = env->addListBox(core::rect<s32>(200,10, 300, 200), 0);
	context.lb2->addItem(L"abc");
	context.lb3 = env->addListBox(core::rect<s32>(20,210, 250, 400), 0);
	context.lb3->addItem(L"abc");
	context.lb3->addItem(L"yzs");
	context.lb4 = env->addListBox(core::rect<s32>(320,210, 450, 400), 0);
	context.combo = env->addComboBox(core::rect<s32>(460,210, 600, 400));
	for ( int i=0; i<50; ++i )
	{
		context.lb4->addItem(L"aaa");
		context.combo->addItem(L"bbb");
	}
	
	
	
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
