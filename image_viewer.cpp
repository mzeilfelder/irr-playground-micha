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
		addImageForTexture(texture, irr::core::stringw(filename).c_str());
	}

	void addImageForTexture(irr::video::ITexture * texture, const wchar_t* text)
	{
		if ( !texture )
			return;
		bool useAlphaChannel=true;
		IGUIElement *parent=0;
		IGUIImage* image = Device->getGUIEnvironment()->addImage(texture, core::position2di(10, 40), useAlphaChannel, parent, -1, text);
		Images.insert(image, 0);
		core::recti r = image->getRelativePosition();

		// do some downscaling if images don't fit (too lazy to code atm)
		core::recti rootRect = Device->getGUIEnvironment()->getRootGUIElement()->getRelativePosition();
		core::vector2di maxSize(core::max_(rootRect.getWidth() - 20, 20), core::max_(rootRect.getHeight() - 50, 20));
		core::vector2df downscale(1.f, 1.f);
		if ( r.getWidth() > maxSize.X )
		{
			downscale.X = (f32)maxSize.X / r.getWidth();
		}
		if ( r.getHeight() > maxSize.Y )
		{
			downscale.Y = (f32)maxSize.Y / r.getHeight();
		}
		f32 ds = core::min_(downscale.X, downscale.Y);
		if (ds < 1)
		{
			r.LowerRightCorner.X = r.UpperLeftCorner.X + (s32)(r.getWidth()*ds);
			r.LowerRightCorner.Y = r.UpperLeftCorner.Y + (s32)(r.getHeight()*ds);
			image->setRelativePosition(r);
			image->setScaleImage(true);
		}

		irr::s32 m = r.getWidth() + 5;
		for ( u32 i=1; i<Images.size(); ++i )
		{
			Images[i]->move( core::position2d<s32>(m,0) );
		}
	}

	// Useful for debugging
	void loadAndRemoveAllTextures(IFileList& fileList)
	{
		for ( u32 i = 0; i < fileList.getFileCount(); ++i )
		{
			if ( fileList.isDirectory(i) )
				continue;
			irr::video::ITexture * texture = Device->getVideoDriver()->getTexture(fileList.getFileName(i));
			if ( texture )
			{
				Device->getVideoDriver()->removeTexture(texture);
			}
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
				case EGET_DIRECTORY_SELECTED:
				{
					// load all images and remove each again directly after loading (just load tests, no display)
					IFileSystem* fs = Context.Device->getFileSystem();
					path oldWorkingDir = fs->getWorkingDirectory();
					IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
					path selectedDir = dialog->getDirectoryName();
					fs->changeWorkingDirectoryTo(selectedDir);
					IFileList* fileList = fs->createFileList();
					fs->changeWorkingDirectoryTo(oldWorkingDir);
					if (fileList)
					{
						Context.loadAndRemoveAllTextures(*fileList);
						fileList->drop();
					}
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

	context.ButtonOpenFile = env->addButton(core::rect<s32>(10, 10, 120, 30), 0, -1, L"file open");

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
