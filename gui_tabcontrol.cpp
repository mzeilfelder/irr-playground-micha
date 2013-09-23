// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing the tab-controls.

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

enum EMenuCommands
{
	EMC_INSERT,
	EMC_REMOVE,
	EMC_CLEAR
};

struct SPopupParams
{
	IGUITabControl * hovered;
	s32 index;
};

struct SAppContext
{
	IrrlichtDevice * device;
	array<IGUIElement*> mGuiElements;
	SPopupParams mPopupParams;
};

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_MOUSE_INPUT_EVENT )
		{
			if ( event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN  )	// EMIE_RMOUSE_LEFT_UP
			{
				IGUITabControl * hovered = checkHoverTabControl(event.MouseInput.X, event.MouseInput.Y);
				if ( hovered )
				{
					Context.mPopupParams.hovered = hovered;
					Context.mPopupParams.index = hovered->getTabAt(event.MouseInput.X, event.MouseInput.Y);
					makePopup(event.MouseInput.X, event.MouseInput.Y);
				}
			}
		}
		else if (event.EventType == EET_GUI_EVENT)
		{
			switch(event.GUIEvent.EventType)
			{
				case EGET_MENU_ITEM_SELECTED:
				{
					IGUIContextMenu * menu = static_cast<IGUIContextMenu*>(event.GUIEvent.Caller);
					s32 idx = menu->getSelectedItem();
					if ( idx >= 0 )
					{
						EMenuCommands command = (EMenuCommands)menu->getItemCommandId((u32)idx);
						switch ( command )
						{
							case EMC_INSERT:
							{
								s32 idxTab = Context.mPopupParams.index < 0 ? 0:Context.mPopupParams.index;
								Context.mPopupParams.hovered->insertTab( idxTab, L"inserted");
								break;
							}
							case EMC_REMOVE:
								Context.mPopupParams.hovered->removeTab( Context.mPopupParams.index );
								break;
							case EMC_CLEAR:
								Context.mPopupParams.hovered->clear();
								break;
						}
					}
				}
				default:
					break;
			}
		}
		
		return false;
	}
	
	void makePopup(s32 x, s32 y)
	{
		core::rect<s32> rect(x, y, x+50, y+50);
		gui::IGUIContextMenu* menu = Context.device->getGUIEnvironment()->addContextMenu (rect);
		menu->addItem(L"insert", EMC_INSERT, true, false);
		if ( Context.mPopupParams.index >= 0 )
			menu->addItem(L"remove", EMC_REMOVE, true, false);
		menu->addItem(L"clear", EMC_CLEAR, true, false);
	}

	IGUITabControl * checkHoverTabControl( s32 x, s32 y)
	{
		IGUIElement * hovered =	Context.device->getGUIEnvironment()->getRootGUIElement ()->getElementFromPoint(core::position2di(x, y));
		if ( hovered )
		{
			while ( hovered && hovered != Context.device->getGUIEnvironment()->getRootGUIElement () )
			{
				if ( hovered->hasType(EGUIET_TAB_CONTROL) )
					return static_cast<IGUITabControl*>(hovered);
				hovered = hovered->getParent();
			}
		}
		return NULL;
	}


private:
	SAppContext & Context;
};


void AddTestGuiElements(IGUIEnvironment* env, IGUIElement * parent, SAppContext & context)
{
	context.mGuiElements.push_back( env->addToolBar (parent, /*s32 id=*/-1) );
	
	core::rect<s32> rect1(10, 40, 10 + 150, 40 + 100);
	IGUITabControl * tabctrl = env->addTabControl (rect1, parent, /*bool fillbackground=*/true, /*bool border=*/true, /*s32 id=*/-1);
	tabctrl->setTabHeight(10);
	IGUITab * invisibleTab = tabctrl->addTab(L"INVISIBLE start", /*s32 id=*/-1);
	invisibleTab->setVisible(false);
	env->addStaticText (L"INVISIBLE start", core::rect<s32>(10,10, 100,30), true, false, invisibleTab, -1, true);
	IGUITab * normalTab = tabctrl->addTab(/*const wchar_t *caption*/L"tab1", /*s32 id=*/-1);
	env->addStaticText (L"tab1", core::rect<s32>(10,10, 100,30), true, false, normalTab, -1, true);
	normalTab = tabctrl->addTab(/*const wchar_t *caption*/L"tab2", /*s32 id=*/-1);
	env->addStaticText (L"tab2", core::rect<s32>(10,10, 100,30), true, false, normalTab, -1, true);
	invisibleTab = tabctrl->addTab(L"INVISIBLE", /*s32 id=*/-1);
	invisibleTab->setVisible(false);
	env->addStaticText (L"INVISIBLE", core::rect<s32>(10,10, 100,30), true, false, invisibleTab, -1, true);
	normalTab = tabctrl->addTab(/*const wchar_t *caption*/L"tab3", /*s32 id=*/-1);
	env->addStaticText (L"tab3", core::rect<s32>(10,10, 100,30), true, false, normalTab, -1, true);
	normalTab = tabctrl->addTab(/*const wchar_t *caption*/L"tab4", /*s32 id=*/-1);
	env->addStaticText (L"tab4", core::rect<s32>(10,10, 100,30), true, false, normalTab, -1, true);
	normalTab = tabctrl->addTab(/*const wchar_t *caption*/L"tab5", /*s32 id=*/-1);
	env->addStaticText (L"tab5", core::rect<s32>(10,10, 100,30), true, false, normalTab, -1, true);
	invisibleTab = tabctrl->addTab(L"INVISIBLE end", /*s32 id=*/-1);
	invisibleTab->setVisible(false);
	env->addStaticText (L"INVISIBLE end", core::rect<s32>(10,10, 100,30), true, false, invisibleTab, -1, true);	
	
	core::rect<s32> rect2(180, 40, 180 + 150, 40 + 100);
	tabctrl = env->addTabControl (rect2, parent, /*bool fillbackground=*/true, /*bool border=*/true, /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"tab1", /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"tab2", /*s32 id=*/-1);
	
	core::rect<s32> rect3(10, 150, 10 + 150, 150 + 100);
	tabctrl = env->addTabControl (rect3, parent, /*bool fillbackground=*/true, /*bool border=*/true, /*s32 id=*/-1);
	tabctrl->setTabHeight(50);	
	tabctrl->addTab(/*const wchar_t *caption*/L"a", /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"b", /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"c", /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"", /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"d", /*s32 id=*/-1);	
	tabctrl->addTab(/*const wchar_t *caption*/L"", /*s32 id=*/-1);	
	tabctrl->addTab(/*const wchar_t *caption*/L"e", /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"", /*s32 id=*/-1);	
	tabctrl->addTab(/*const wchar_t *caption*/L"f", /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"", /*s32 id=*/-1);	
	tabctrl->addTab(/*const wchar_t *caption*/L"g", /*s32 id=*/-1);
	
	core::rect<s32> rect4(180, 150, 180 + 150, 150 + 100);
	tabctrl = env->addTabControl (rect4, parent, /*bool fillbackground=*/true, /*bool border=*/true, /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"how long can tabs be anyway? Give me the max!", /*s32 id=*/-1);
	
	core::rect<s32> rect5(10, 260, 10 + 150, 260 + 100);
	tabctrl = env->addTabControl (rect5, parent, /*bool fillbackground=*/true, /*bool border=*/true, /*s32 id=*/-1);
	tabctrl->setTabVerticalAlignment(EGUIA_LOWERRIGHT);
	tabctrl->addTab(/*const wchar_t *caption*/L"tab1", /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"tab2", /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"tab3", /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"tab4", /*s32 id=*/-1);
	tabctrl->addTab(/*const wchar_t *caption*/L"tab5", /*s32 id=*/-1);
	
	
	context.mGuiElements.push_back( tabctrl );
}

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
	AddTestGuiElements(env, 0, context);
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

