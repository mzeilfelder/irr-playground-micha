// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// All kind of tests for the menu system.

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
	E_RIGHT_CLICK_MENUES = 1,
	E_LEFT_CLICK_AREA,
	E_LEFT_CLICK_AREA_OPEN,
};

struct SAppContext
{
	IrrlichtDevice * Device;
	gui::IGUIEnvironment* Environment;
	gui::IGUIStaticText* LeftClickArea;
	gui::IGUIStaticText* LeftClickAreaOpen;
	gui::IGUIStaticText* StatusBar;
	gui::IGUIButton * CreateRightClickMenues;
	gui::IGUIButton * EatCloseEvent;
};

void FillMenu( gui::IGUIContextMenu* menu )
{
	menu->addItem(L"top1", -1, true, true);
	menu->addItem(L"top2", -1, true, true);
	menu->addItem(L"no sub", -1, true, false);
	IGUIContextMenu* submenu = menu->getSubMenu(0);
	if ( submenu )
	{
        submenu->addItem(L"item1", -1);
        submenu->addSeparator();
        submenu->addItem(L"item2", -1);
        submenu->addItem(L"checked", -1, true, false, true);
        submenu->addItem(L"sub1", -1, true, true, false );
        submenu->addItem(L"sub2", -1, true, true, false );
		submenu->addItem(L"disabled", -1, false, false, false);
		submenu->addItem(L"autochecked", -1, true, false, false, true);
		submenu->insertItem(3, L"inserted at 3", -1);
	}
	IGUIContextMenu* subsubmenu = submenu->getSubMenu(4);
	if ( subsubmenu )
	{
        subsubmenu->addItem(L"subitemA", -1);
        subsubmenu->addItem(L"subitemB", -1);
	}
	subsubmenu = submenu->getSubMenu(5);
	if ( subsubmenu )
	{
		subsubmenu->addItem(L"subitemC", -1);
		subsubmenu->addItem(L"subitemD", -1);
	}
	
	submenu = menu->getSubMenu(1);
	if ( submenu )
	{
        submenu->addItem(L"item", -1);
	}	
}

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context), ContextMenu(0) { }
	
	void ShowContextMenu(irr::s32 x, irr::s32 y, bool keepOpen)
	{
		core::rect<s32> rect(x, y, x+50, y+50);
		ContextMenu = Context.Environment->addContextMenu (rect);
		if ( keepOpen )
			ContextMenu->setCloseHandling(ECMC_IGNORE);
		FillMenu(ContextMenu);
	};

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_MOUSE_INPUT_EVENT )
		{
			if ( event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN )
			{
				if ( 	!Context.LeftClickArea->isPointInside( core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y) )
				     && !Context.LeftClickAreaOpen->isPointInside( core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y) ) 
				     && Context.CreateRightClickMenues->isPressed()
				   )
				{
					ShowContextMenu(event.MouseInput.X, event.MouseInput.Y, false);
				}
			}
			if ( event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP )
			{
				Context.StatusBar->setText(L"");
				if ( Context.LeftClickArea->isPointInside( core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y) ) )
				{
					ShowContextMenu(event.MouseInput.X, event.MouseInput.Y, false);
					return true;
				}
				if ( Context.LeftClickAreaOpen->isPointInside( core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y) ) )
				{
					ShowContextMenu(event.MouseInput.X, event.MouseInput.Y, true);
					return true;
				}
			}
		}
		if (event.EventType == EET_GUI_EVENT)
		{
			switch(event.GUIEvent.EventType)
			{
			case EGET_MENU_ITEM_SELECTED:
			{
				IGUIContextMenu * menu = static_cast<IGUIContextMenu*>(event.GUIEvent.Caller);
				s32 idx = menu->getSelectedItem();
				if ( idx >= 0 )
					Context.StatusBar->setText(menu->getItemText(idx));
				else
					Context.StatusBar->setText(L"selected event, but no item selected - should never happen.");
				break;
			}
			case EGET_ELEMENT_CLOSED:
			{
				if ( Context.EatCloseEvent->isPressed() )
					return true;
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
	gui::IGUIContextMenu* ContextMenu;
};


int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	//IrrlichtDevice * device = createDevice(driverType, core::dimension2d<s32>(640, 480));	// <= irrlicht 1.5
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));	// >= irrlicht 1.6
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	
	SAppContext context;
	context.Device = device;
	context.Environment = env;
	
	// menu bar in main window
	gui::IGUIContextMenu* menuBar = env->addMenu (0, -1);
	FillMenu(menuBar);
	
	// some button below menu to see if mouse-clicks are correctly eaten by the menu
	core::rect<s32> rectBtn(0, 15, 150, 90);
	context.CreateRightClickMenues = env->addButton (rectBtn, 0, E_RIGHT_CLICK_MENUES, L"right-click context menu");
	context.CreateRightClickMenues->setIsPushButton(true);

	rectBtn = core::rect<s32>(0, 100, 150, 120);
	context.EatCloseEvent = env->addButton (rectBtn, 0, E_RIGHT_CLICK_MENUES, L"eat close event");
	context.EatCloseEvent->setIsPushButton(true);
	
	// menu bar in sub-window
	core::rect<s32> rectWnd(10, 100, 300, 200);
	IGUIWindow * wnd = env->addWindow (rectWnd, false, L"window", NULL, -1);
	gui::IGUIContextMenu* menuBar2 = env->addMenu (wnd, -1);
	FillMenu(menuBar2);
		
	core::rect<s32> rectStatic(10, 359, 150, 400);
	context.LeftClickArea = env->addStaticText (L"left click menu", rectStatic, true, true, 0, E_LEFT_CLICK_AREA, true);
	
	core::rect<s32> rectStaticOpen(160, 359, 300, 400);
	context.LeftClickAreaOpen = env->addStaticText (L"left click menu staying open", rectStaticOpen, true, true, 0, E_LEFT_CLICK_AREA_OPEN, true);
	
	core::rect<s32> rectStatusBar(0, 450, 640, 480);
	context.StatusBar = env->addStaticText (L"", rectStatusBar, true, false);
	
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

