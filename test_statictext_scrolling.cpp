// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Test for some features which are not yet in Irrlicht.

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

enum
{
	GUI_ID_BUTTON_SCROLL_UP,
	GUI_ID_BUTTON_SCROLL_DOWN,
	GUI_ID_BUTTON_PUSH_TEXT,
	GUI_ID_BUTTON_PUSH_LONGTEXT,
};


struct SAppContext
{
	IrrlichtDevice * device;
	IGUIStaticText * staticA;
};

// Add text and scoll so that the bottom lines are visible
// TBD: Should somthing like this function be added to IGUIStaticText in the patch? 
void PushText(const stringw & txt, IGUIStaticText * staticText)
{
	stringw wstr( staticText->getText() );
	wstr += txt;
	staticText->setText(wstr.c_str());

	// to find out how much we have to scroll
	core::rect<s32> rect( staticText->getRelativePosition() );
	IGUIFont* font = staticText->getCurrentFont();
	if ( font )
	{
		s32 charHeight = font->getDimension(L"A").Height - font->getKerningHeight();
		s32 visibleLines = charHeight ? rect.getHeight() / charHeight : 0;
		s32 scrollLines = staticText->getTextLineCount() - visibleLines;
		-- scrollLines;
		if ( scrollLines < 0 )
			scrollLines = 0;
		
		// scroll 
		staticText->setScrolledTextLines(-scrollLines);
	}
}


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
				case EGET_BUTTON_CLICKED:
				{
					switch ( id )
					{
						case GUI_ID_BUTTON_SCROLL_UP:
							Context.staticA->setScrolledTextLines( Context.staticA->getScrolledTextLines() -1 );
							break;
						case GUI_ID_BUTTON_SCROLL_DOWN:
							Context.staticA->setScrolledTextLines( Context.staticA->getScrolledTextLines() +1 );
							break;
						case GUI_ID_BUTTON_PUSH_TEXT:
							PushText(L"one more line", Context.staticA);
							break;
						case GUI_ID_BUTTON_PUSH_LONGTEXT:
							PushText(L"a short line\na long long long long long long long long long long long long long long long long long long line", Context.staticA);
							break;
					}
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
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	
	env->addButton (rect<s32>(300, 50, 400, 70), /*IGUIElement *parent=*/0, /*s32 id=*/GUI_ID_BUTTON_SCROLL_UP, L"scroll up");
	env->addButton (rect<s32>(300, 80, 400, 100), /*IGUIElement *parent=*/0, /*s32 id=*/GUI_ID_BUTTON_SCROLL_DOWN, L"scroll down");
	env->addButton (rect<s32>(300, 110, 400, 130), /*IGUIElement *parent=*/0, /*s32 id=*/GUI_ID_BUTTON_PUSH_TEXT, L"push text");
	env->addButton (rect<s32>(300, 140, 400, 160), /*IGUIElement *parent=*/0, /*s32 id=*/GUI_ID_BUTTON_PUSH_LONGTEXT, L"push long text");
	
	SAppContext context;
	context.device = device;
	context.staticA = env->addStaticText (L"first line\nline two\nline three\nline four\nline five\nline six\nline seven\nline eight\nline nine\nlast line\n"
	                                      , rect<s32>(50,50, 200, 100), 
	                                      /*bool border=*/true, /*bool wordWrap=*/true, 
	                                      /*IGUIElement *parent=*/0, /*s32 id=*/-1, /*bool fillBackground=*/true);
	
	
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

