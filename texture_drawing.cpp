// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Simple drawing tool example

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

struct SDrawableTexture
{
	SDrawableTexture() : hasImageChanged(false), editableImage(0), texture(0) {}
	~SDrawableTexture() 
	{
		clear();
	}

	void uploadChangedImageToTexture()
	{
		if ( editableImage && texture && hasImageChanged )
		{
			void* imgData = editableImage->getData();
			void* texData = texture->lock(irr::video::ETLM_READ_WRITE);
			if ( editableImage->getPitch() == texture->getPitch() )
			{
				memcpy(texData, imgData, editableImage->getPitch()*editableImage->getDimension().Height);
			}
			else
			{
				// copy each line, too lazy to code that now
			}
			texture->unlock();
			// texture->regenerateMipMapLevels(); // you may need that if you have a texture with mip data
			hasImageChanged  = false;
		}
	}

	void setPixel(irr::u32 x, irr::u32 y, irr::video::SColor col)
	{
		if ( editableImage )
		{
			editableImage->setPixel(x,y, col);
			hasImageChanged = true;
		}
	}

	void setTexture(irr::video::IVideoDriver* driver, irr::video::ITexture* tex)
	{
		clear();
		texture = tex;
		if ( texture )
		{
			texture->grab();
			editableImage = driver->createImage(texture, irr::core::vector2di(0,0), texture->getSize());
		}
	}

	void clear()
	{
		if ( texture )
			texture->drop();
		texture = 0;
		if ( editableImage )
			editableImage->drop();
		editableImage = 0;
	}

	bool hasImageChanged;
	irr::video::IImage* editableImage;
	irr::video::ITexture* texture;

private:
	SDrawableTexture(const SDrawableTexture& other); // no implementation, just don't copy those for now
};

struct SAppContext
{
	SAppContext() : Device(0), ButtonOpenFile(0), Image(0)
	{}

	void LoadImage(const irr::io::path& filename)
	{
		irr::video::ITexture * texture = Device->getVideoDriver()->getTexture(filename);
		addImageForTexture(texture, irr::core::stringw(filename).c_str());
		DrawableTex.setTexture(Device->getVideoDriver(), texture);
	}

	void addImageForTexture(irr::video::ITexture * texture, const wchar_t* text)
	{
		if ( !texture )
			return;
		if ( Image )
			Image->remove();
		bool useAlphaChannel=true;
		IGUIElement *parent=0;
		Image = Device->getGUIEnvironment()->addImage(texture, core::position2di(10, 40), useAlphaChannel, parent, -1, text);
	}

	bool isMouseInsideDrawable(irr::s32 x, irr::s32 y)
	{
		return Image && Image->getAbsolutePosition().isPointInside(core::vector2di(x,y));
	}

	IrrlichtDevice * Device;
	IGUIButton * ButtonOpenFile;
	irr::gui::IGUIImage* Image;
	SDrawableTexture DrawableTex;
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
						return true;
					}
				}
				break;
				case EGET_FILE_SELECTED:
				{
					// load the model file, selected in the file open dialog
					IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
					Context.LoadImage( dialog->getFileName() );
					return true;
				}
				break;
				default:
					break;
			}
		}
		else if ( event.EventType == EET_MOUSE_INPUT_EVENT )
		{
			switch ( event.MouseInput.Event )
			{
				case irr::EMIE_LMOUSE_PRESSED_DOWN:
					if ( Context.isMouseInsideDrawable(event.MouseInput.X, event.MouseInput.Y) )
					{
						irr::core::vector2di leftTop = Context.Image->getAbsolutePosition().UpperLeftCorner;
						Context.DrawableTex.setPixel(event.MouseInput.X-leftTop.X, event.MouseInput.Y-leftTop.Y, irr::video::SColor(255, 255, 255, 255));
						return true;
					}
					break;

				case irr::EMIE_RMOUSE_PRESSED_DOWN:
					if ( Context.isMouseInsideDrawable(event.MouseInput.X, event.MouseInput.Y) )
					{
						irr::core::vector2di leftTop = Context.Image->getAbsolutePosition().UpperLeftCorner;
						Context.DrawableTex.setPixel(event.MouseInput.X-leftTop.X, event.MouseInput.Y-leftTop.Y, irr::video::SColor(255, 0, 0, 0));
						return true;
					}
					break;

				case irr::EMIE_MOUSE_MOVED:
					if ( event.MouseInput.isLeftPressed() && Context.isMouseInsideDrawable(event.MouseInput.X, event.MouseInput.Y) )
					{
						irr::core::vector2di leftTop = Context.Image->getAbsolutePosition().UpperLeftCorner;
						Context.DrawableTex.setPixel(event.MouseInput.X-leftTop.X, event.MouseInput.Y-leftTop.Y, irr::video::SColor(255, 255, 0, 0));
						return true;
					}
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
			context.DrawableTex.uploadChangedImageToTexture();

			driver->beginScene(true, true, SColor(0,200,200,200));

			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
