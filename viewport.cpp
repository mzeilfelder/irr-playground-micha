// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Check the viewport and resize behavior

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

struct SAppContext
{
	SAppContext() : device(0), infoStatic(0) {}
	
	IrrlichtDevice * device;
	irr::gui::IGUIStaticText * infoStatic;
	irr::gui::IGUIListBox* listBoxScreenSize;
	irr::core::array<irr::core::dimension2du> screenSizes;
	irr::gui::IGUIListBox* listBoxViewPort;
	irr::core::array<core::recti> viewPorts;
};

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
				}
				break;
				case EGET_LISTBOX_CHANGED:
					if ( event.GUIEvent.Caller == Context.listBoxViewPort )
					{
						OnViewPortSelected();
						return true;
					}
					else if ( event.GUIEvent.Caller == Context.listBoxScreenSize )
					{
						OnWindowSizeSelected();
						return true;
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
			Context.infoStatic->setText(infoText.c_str());
		}

		return false;
	}

	void OnWindowSizeSelected()
	{
		s32 selected = Context.listBoxScreenSize->getSelected();
		if ( selected < 0 )
			return;
		
		Context.device->setWindowSize(Context.screenSizes[selected]);
	}
	
	void OnViewPortSelected()
	{
		s32 selected = Context.listBoxViewPort->getSelected();
		if ( selected < 0 )
			return;
		
		video::IVideoDriver* driver = Context.device->getVideoDriver();
		if ( selected == 0 )
		{
			const core::dimension2d<u32>& screenSize = driver->getScreenSize();
			driver->setViewPort( core::recti(0,0,screenSize.Width,screenSize.Height) );
		}
		else
		{
			driver->setViewPort( Context.viewPorts[selected] );
		}
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
	
	context.infoStatic = env->addStaticText (L"", core::rect< s32 >(10,10, 200, 100), true, true);
	context.listBoxViewPort = env->addListBox (core::rect<s32>(10,200,100, 320));
	context.listBoxViewPort->addItem(L"Screen size");
	context.viewPorts.push_back(core::recti(0,0,1,1));	// dummy, doesn't matter
	context.listBoxViewPort->addItem(L"0,0,320,200");
	context.viewPorts.push_back(core::recti(0,0,320,200));
	context.listBoxViewPort->addItem(L"160,100,480,300");
	context.viewPorts.push_back(core::recti(160,100,480,300));
	context.listBoxViewPort->addItem(L"0,0,640,480");
	context.viewPorts.push_back(core::recti(0,0,640,480));
	context.listBoxViewPort->addItem(L"0,0,800,600");
	context.viewPorts.push_back(core::recti(0,0,800,600));
	context.listBoxViewPort->addItem(L"0,0,1024,768");
	context.viewPorts.push_back(core::recti(0,0,1024,768));
	
	context.listBoxScreenSize = env->addListBox (core::rect<s32>(110,200,200, 320));
	context.listBoxScreenSize->addItem(L"640x480");
	context.screenSizes.push_back( core::dimension2d<u32>(640, 480) );
	context.listBoxScreenSize->addItem(L"800x600");
	context.screenSizes.push_back( core::dimension2d<u32>(800, 600) );
	context.listBoxScreenSize->addItem(L"1024x768");
	context.screenSizes.push_back( core::dimension2d<u32>(1024, 768) );
	context.listBoxScreenSize->addItem(L"1280x800");
	context.screenSizes.push_back( core::dimension2d<u32>(1280, 800) );
	context.listBoxScreenSize->addItem(L"1600x900");
	context.screenSizes.push_back( core::dimension2d<u32>(1600, 900) );
	context.listBoxScreenSize->addItem(L"1920x1080");
	context.screenSizes.push_back( core::dimension2d<u32>(1920, 1080) );
	
	video::ITexture *image64x64 = driver->getTexture("my_media/64x64.jpg");
	env->addImage (image64x64, core::position2d<s32>(220, 10));
	for ( s32 i=0; i < 30; ++i )
	{
		env->addImage (image64x64, core::position2d<s32>(i*64, 128));
		env->addImage (image64x64, core::position2d<s32>(320, i*64));
	}
	
	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);

	while(device->run() && driver)
	{
		if (!device->isWindowMinimized())
		{
			const core::rect<s32>& viewPort = driver->getViewPort();
			const core::dimension2d<u32>& screenSize = driver->getScreenSize();
			const core::dimension2d<u32>& rtSize = driver->getCurrentRenderTargetSize();
			wchar_t caption[512];
			swprintf(caption, 512, L"screen(%4u/%4u) viewport(%4d,%4d,%4d,%4d) rt(%4u/%4u)",
				screenSize.Width, screenSize.Height,
				viewPort.UpperLeftCorner.X, viewPort.UpperLeftCorner.Y, viewPort.LowerRightCorner.X, viewPort.LowerRightCorner.Y,
				rtSize.Width, rtSize.Height);
			device->setWindowCaption(caption);

			driver->beginScene(true, true, SColor(0,200,200,200));
			env->drawAll();
			driver->endScene();
		}
		
		// be nice to CPU
		device->sleep(10);
		if (!device->isWindowActive())
			device->sleep(90);
	}

	device->drop();

	return 0;
}
