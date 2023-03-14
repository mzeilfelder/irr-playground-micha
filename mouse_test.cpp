// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// All kind of tests with the mouse-cursor.

#include <irrlicht.h>
#include <iostream>
#include <vector>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

const int DELAY_TIME = 3000;

enum ETimerAction
{
	ETA_MOUSE_VISIBLE,
	ETA_MOUSE_INVISIBLE,
};

struct TimerAction
{
	u32 TargetTime;
	ETimerAction Action;
};

struct SAppContext
{
	SAppContext() : device(0), infoStatic(0), infoBox(0), buttonSetVisible(0), buttonSetInvisible(0) {}
	
	void Update()
	{
		if (!device) 
			return;
		u32 timeNow = device->getTimer()->getTime();
		for ( std::vector<TimerAction>::iterator it = timerActions.begin(); it != timerActions.end(); )
		{
			if ( timeNow >= it->TargetTime )
			{
				RunAction(*it);
				it = timerActions.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void RunAction(const TimerAction& action)
	{
		if (ETA_MOUSE_VISIBLE == action.Action)
		{
			device->getCursorControl()->setVisible(true);
			buttonSetVisible->setEnabled(true);
		}
		else if ( ETA_MOUSE_INVISIBLE == action.Action)
		{
			device->getCursorControl()->setVisible(false);
			buttonSetInvisible->setEnabled(true);
		}
	}

	
	IrrlichtDevice * device;
	irr::gui::IGUIStaticText * infoStatic;
	irr::gui::IGUIListBox * infoBox;
	irr::gui::IGUIButton * buttonSetVisible;
	irr::gui::IGUIButton * buttonSetInvisible;
	std::vector<TimerAction> timerActions;	
};

void PrintMouseEventName(const SEvent& event, irr::core::stringw &result)
{
	switch ( event.MouseInput.Event )
	{
		case EMIE_LMOUSE_PRESSED_DOWN: 	result += stringw(L"EMIE_LMOUSE_PRESSED_DOWN"); break;
		case EMIE_RMOUSE_PRESSED_DOWN: 	result += stringw(L"EMIE_RMOUSE_PRESSED_DOWN"); break;
		case EMIE_MMOUSE_PRESSED_DOWN: 	result += stringw(L"EMIE_MMOUSE_PRESSED_DOWN"); break;
		case EMIE_LMOUSE_LEFT_UP: 		result += stringw(L"EMIE_LMOUSE_LEFT_UP"); break;
		case EMIE_RMOUSE_LEFT_UP: 		result += stringw(L"EMIE_RMOUSE_LEFT_UP"); break;
		case EMIE_MMOUSE_LEFT_UP: 		result += stringw(L"EMIE_MMOUSE_LEFT_UP"); break;
		case EMIE_MOUSE_MOVED: 			result += stringw(L"EMIE_MOUSE_MOVED"); break;
		case EMIE_MOUSE_WHEEL: 			result += stringw(L"EMIE_MOUSE_WHEEL"); break;
		case EMIE_LMOUSE_DOUBLE_CLICK: 	result += stringw(L"EMIE_LMOUSE_DOUBLE_CLICK"); break;
		case EMIE_RMOUSE_DOUBLE_CLICK: 	result += stringw(L"EMIE_RMOUSE_DOUBLE_CLICK"); break;
		case EMIE_MMOUSE_DOUBLE_CLICK: 	result += stringw(L"EMIE_MMOUSE_DOUBLE_CLICK"); break;
		case EMIE_LMOUSE_TRIPLE_CLICK: 	result += stringw(L"EMIE_LMOUSE_TRIPLE_CLICK"); break;
		case EMIE_RMOUSE_TRIPLE_CLICK: 	result += stringw(L"EMIE_RMOUSE_TRIPLE_CLICK"); break;
		case EMIE_MMOUSE_TRIPLE_CLICK: 	result += stringw(L"EMIE_MMOUSE_TRIPLE_CLICK"); break;
		default:
		break;
	}
}

void PrintCursorState(IrrlichtDevice * device, irr::core::stringw &result)
{
	gui::ICursorControl * cursor = device->getCursorControl();
	
	core::position2d<f32> relPos = cursor->getRelativePosition();
	result += stringw(L"relPos.X: ");
	result += stringw(relPos.X);
	result += stringw(L"\n");
	result += stringw(L"relPos.Y: ");
	result += stringw(relPos.Y);
	result += stringw(L"\n");
	
	core::position2d<s32> position = cursor->getPosition();
	result += stringw(L"position.X: ");
	result += stringw(position.X);
	result += stringw(L"\n");
	result += stringw(L"position.Y: ");
	result += stringw(position.Y);
	result += stringw(L"\n");

#if IRRLICHT_VERSION_MAJOR > 1 || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)
	irr::core::recti refRect;
	cursor->getReferenceRect(refRect);
	result += L"ReferenceRect: (";
	result += stringw(refRect.UpperLeftCorner.X);
	result += L",";
	result += stringw(refRect.UpperLeftCorner.Y);
	result += L"/";
	result += stringw(refRect.LowerRightCorner.X);
	result += L",";
	result += stringw(refRect.LowerRightCorner.Y);
	result += L")\n";
#endif
}

void PrintMouseState(const SEvent& event, irr::core::stringw &result)
{
	result += stringw(L"event X: ");
	result += stringw(event.MouseInput.X);
	result += stringw(L"\n");
		
	result += stringw(L"event Y: ");
	result += stringw(event.MouseInput.Y);
	result += stringw(L"\n");


	result += stringw(L"event Wheel: ");
	result += stringw(event.MouseInput.Wheel);
	result += stringw(L"\n");
	
	result += stringw(L"event Shift: ");
	if ( event.MouseInput.Shift )
		result += stringw(L"true\n");
	else
		result += stringw(L"false\n");

	result += stringw(L"event Control: ");
	if ( event.MouseInput.Control )
		result += stringw(L"true\n");
	else
		result += stringw(L"false\n");

	result += stringw(L"event ButtonStates: ");
	result += stringw(event.MouseInput.ButtonStates);
	result += stringw(L"\n");

	result += stringw(L"event isLeftPressed: ");
	if ( event.MouseInput.isLeftPressed() )
		result += stringw(L"true\n");
	else
		result += stringw(L"false\n");

	result += stringw(L"event isRightPressed: ");
	if ( event.MouseInput.isRightPressed() )
		result += stringw(L"true\n");
	else
		result += stringw(L"false\n");

	result += stringw(L"event isMiddlePressed: ");
	if ( event.MouseInput.isMiddlePressed() )
		result += stringw(L"true\n");
	else
		result += stringw(L"false\n");

	result += stringw(L"Event: ");
	
	PrintMouseEventName(event, result);
	
	result += stringw(L"\n");
}

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT )
		{
			switch ( event.GUIEvent.EventType )
			{
				case EGET_BUTTON_CLICKED:
				{
					u32 timeNow = Context.device->getTimer()->getTime();
					TimerAction action;
					action.TargetTime = timeNow + DELAY_TIME;
					if ( event.GUIEvent.Caller == Context.buttonSetVisible )
					{
						action.Action = ETA_MOUSE_VISIBLE;
						Context.timerActions.push_back(action);
						Context.buttonSetVisible->setEnabled(false);
					}
					else if ( event.GUIEvent.Caller == Context.buttonSetInvisible )
					{
						action.Action = ETA_MOUSE_INVISIBLE;
						Context.timerActions.push_back(action);
						Context.buttonSetInvisible->setEnabled(false);
					}
				}
				break;
				default:
				break;
			}
		}
		
		if (event.EventType == EET_MOUSE_INPUT_EVENT)
		{
			irr::core::stringw infoText;
			PrintCursorState(Context.device, infoText);
			PrintMouseState(event, infoText);
			Context.infoStatic->setText(infoText.c_str());
			if ( event.MouseInput.Event != EMIE_MOUSE_MOVED && event.MouseInput.Event != EMIE_MOUSE_WHEEL ) // no spam
			{
				infoText = L"";
				PrintMouseEventName(event, infoText);
				Context.infoBox->addItem(infoText.c_str());
			}
		}

		return false;
	}

private:
	SAppContext & Context;
};


int main()
{
	SIrrlichtCreationParameters param;
	param.DriverType = video::EDT_OPENGL;
	param.WindowSize = core::dimension2d<u32>(640, 480);
	IrrlichtDevice* device = irr::createDeviceEx(param);
	if (device == 0)
		return 1; // could not create selected driver.

#if 0 // set manual reference rect (should have ui elements for this...)
	irr::core::recti refRect(500, 500, 800, 800);
	device->getCursorControl()->setReferenceRect(&refRect);
#endif
	device->setResizable(true);
	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	
	SAppContext context;
	context.device = device;
	
	core::rect< s32 > rectInfoStatic(10,10, 200, 400);
	context.infoStatic = env->addStaticText (L"", rectInfoStatic, true, true);
	core::rect< s32 > rectInfoBox(210,10, 400, 400);
	context.infoBox = env->addListBox(rectInfoBox);
	context.buttonSetVisible = env->addButton( rect<s32>( 410, 20, 500, 40 ), 0, -1, L"set visible (delayed)" );
	context.buttonSetInvisible = env->addButton( rect<s32>( 410, 50, 500, 70 ), 0, -1, L"set invisible (delayed)" );
	
	
	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);
	

	while(device->run() && driver)
	{
		// if (device->isWindowActive())
		{
			/*			static int triggerMousePos = 0;
			if ( !(++triggerMousePos % 100) )
			{
				gui::ICursorControl * cursor = device->getCursorControl();
				core::position2d<f32> relPos = cursor->getRelativePosition();
				cursor->setPosition(relPos);
			}*/

			context.Update();
			
			driver->beginScene(true, true, SColor(0,200,200,200));
	
			env->drawAll();
		
			driver->endScene();
		}
		
		device->sleep(10);
	}

	device->drop();

	return 0;
}

