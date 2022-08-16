// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Tiny stupid image viewer

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
	SAppContext() : Device(0), ButtonOpenFile(0)
	{
	}

	void LoadImage(const irr::io::path& filename)
	{
		irr::video::ITexture * texture = Device->getVideoDriver()->getTexture(filename);
		if ( texture )
		{
			bool useAlphaChannel=true;
			irr::gui::IGUIElement *parent=0;
			irr::gui::IGUIImage* image = Device->getGUIEnvironment()->addImage(texture, core::position2di(20, 50), useAlphaChannel, parent, -1, irr::core::stringw(filename).c_str());
			Images.insert(image, 0);
			core::recti r= image->getRelativePosition();
			irr::s32 m = r.getWidth() + 5;
			for ( u32 i=1; i<Images.size(); ++i )
			{
				Images[i]->move( core::position2d<s32>(m,0) );
			}
			const core::dimension2d<u32>& origSize = texture->getOriginalSize();
			irr::core::stringw caption(filename.c_str());
			caption += L" ";
			caption += irr::core::stringw(origSize.Width);
			caption += L":";
			caption += irr::core::stringw(origSize.Height);
			Device->setWindowCaption(caption.c_str());
		}
	}
	
	IrrlichtDevice * Device;
	IGUIButton * ButtonOpenFile;
	irr::core::array<irr::gui::IGUIImage*> Images;
};


class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			switch ( event.GUIEvent.EventType )
			{
				case EGET_BUTTON_CLICKED:
				{
					if ( event.GUIEvent.Caller == Context.ButtonOpenFile )
					{
						Context.Device->getGUIEnvironment()->addFileOpenDialog(L"Open Image", true, 0);
					}
				}
				break;
				case EGET_FILE_SELECTED:
				{
					// load the model file, selected in the file open dialog
					IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
					Context.LoadImage( dialog->getFileName() );
				}
				break;
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
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(800, 600));
	if (device == 0)
		return 1; // could not create selected driver.

	device->setResizable(true);

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	
	SAppContext context;
	context.Device = device;

	context.ButtonOpenFile = env->addButton(core::rect<s32>(20, 20, 120, 40), 0, -1, L"file open");

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
