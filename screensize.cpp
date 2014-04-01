// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing resizing and finding out what size-values actually mean in windowed mode (aka client vs. window-size stuff).

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

enum GUI_ID
{
	BUTTON_QUIT,
	BUTTON_RESIZEABLE,
};

struct SAppContext
{
	SAppContext() : device(0), quit(false)
	{
	}

	IrrlichtDevice * device;
	bool quit;
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
				case EGET_BUTTON_CLICKED:
					if ( event.GUIEvent.Caller->getID() == BUTTON_QUIT )
					{
						Context.quit = true;
					}
					else if ( event.GUIEvent.Caller->getID() == BUTTON_RESIZEABLE )
					{
						Context.device->setResizable( static_cast<irr::gui::IGUIButton*>(event.GUIEvent.Caller)->isPressed());
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
	core::dimension2d<s32> dimension(900, 1280);
	int bitdepth = 32;
	bool UseScreenResolution = false;	
	bool FullScreen = false;

    IrrlichtDevice *dummyDevice = 0;
    dummyDevice = createDevice(video::EDT_NULL,core::dimension2d<u32>(100,100),false,false);
    if ( dummyDevice )
    {
		dummyDevice->clearSystemMessages();

        video::IVideoModeList *videoModes = dummyDevice->getVideoModeList();
        core::dimension2d<u32> desktopRes = videoModes->getDesktopResolution();
        s32 desktopDepth = videoModes->getDesktopDepth();
        dummyDevice->drop();

        // use the resolution of the desktop
		if ( UseScreenResolution
            && desktopRes.Width && desktopRes.Height && desktopDepth )
        {
            dimension = desktopRes;
            bitdepth = desktopDepth;
        }
    }

	SIrrlichtCreationParameters creationParameters;
    creationParameters.Bits = bitdepth;
	creationParameters.DriverType = video::EDT_OPENGL;
    creationParameters.EventReceiver = 0;
	creationParameters.Fullscreen = FullScreen;
    creationParameters.HighPrecisionFPU  = false;
    creationParameters.Stencilbuffer = false;
	creationParameters.Vsync = false;
	creationParameters.AntiAlias = 0;
	creationParameters.Doublebuffer = true;
    creationParameters.WindowId = 0;
    creationParameters.WindowSize = dimension;
    creationParameters.LoggingLevel = ELL_INFORMATION;

    IrrlichtDevice * device = createDeviceEx(creationParameters);
	
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	
	SAppContext context;
	context.device = device;
	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);
	
	const core::dimension2d< u32 > & screen = driver->getScreenSize();
	std::cout << "Width: " << screen.Width << " Height: " << screen.Height << "\n";
	
	env->addButton(core::rect<s32>(0,0, 100, 100), 0, -1, L"top left");
	env->addButton(core::rect<s32>(dimension.Width-100,dimension.Height-100, dimension.Width,dimension.Height), 0, -1, L"bottom right");

	IGUIWindow * wnd = env->addWindow(core::rect< s32 >(60, 155, 580, 460), false, L"", 0, -1);

	env->addButton(core::rect<s32>(50,50, 100, 100), wnd, BUTTON_QUIT, L"QUIT");
	IGUIButton * btnResizable = env->addButton(core::rect<s32>(150,50, 200, 100), wnd, BUTTON_RESIZEABLE, L"resizeable");
	btnResizable->setIsPushButton(true);
	env->addEditBox(L"", core::rect<s32>(50,150, 100, 200), true, wnd);

	while(!context.quit && device->run() && driver)
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, SColor(0,200,200,200));
	
			env->drawAll();
		
			driver->endScene();
		}
		device->sleep(10);
	}

	device->drop();

	return 0;
}

