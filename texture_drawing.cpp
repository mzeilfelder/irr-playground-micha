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

// Helper define to compile with Irrlicht versions before 1.9
#if IRRLICHT_VERSION_MAJOR < 1 || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR <= 8 )
#define PRE_IRRLICHT_1_9
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
#ifdef PRE_IRRLICHT_1_9
			void* imgData = editableImage->lock();
#else
			void* imgData = editableImage->getData();
#endif
			void* texData = texture->lock(irr::video::ETLM_READ_WRITE);
			if ( editableImage->getPitch() == texture->getPitch() )
			{
				memcpy(texData, imgData, editableImage->getPitch()*editableImage->getDimension().Height);
			}
			else
			{
				// copy each line, too lazy to code that now
			}
#ifdef PRE_IRRLICHT_1_9
			editableImage->unlock();
#endif
			texture->unlock();
			texture->regenerateMipMapLevels(); // you may need that if you have a texture with mip data
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
	// no copy-constructor implementation, just don't copy those for now
	// If you need it you have 2 options: Either don't grab()/drop() in here and use weak-pointers 
	// or (better) write a copy-constructor which does the correct grab()/drop() stuff.
	SDrawableTexture(const SDrawableTexture& other); 
};

struct SAppContext
{
	SAppContext() : Device(0), ButtonOpenFile(0), ButtonScene(0), Image(0), CubeNode(0)
	{}

	void LoadImage(const irr::io::path& filename)
	{
		irr::video::ITexture * texture = Device->getVideoDriver()->getTexture(filename);
		addTexture(texture, irr::core::stringw(filename).c_str());
	}

	// Add texture and do stuff with it - create an image - make it drawble - add it to cube
	void addTexture(irr::video::ITexture * texture, const wchar_t* text)
	{
		if ( !texture )
			return;
		if ( Image )
			Image->remove();
		DrawableTex.setTexture(Device->getVideoDriver(), texture);
		if ( CubeNode )
			CubeNode->setMaterialTexture(0, texture);
		bool useAlphaChannel=true;
		IGUIElement *parent=0;
		Image = Device->getGUIEnvironment()->addImage(texture, core::position2di(10, 40), useAlphaChannel, parent, -1, text);
	}

	bool isMouseInsideDrawable(irr::s32 x, irr::s32 y)
	{
		return Image && Device->getGUIEnvironment()->getHovered() == Image;
	}

	IrrlichtDevice * Device;
	IGUIButton * ButtonOpenFile;
	IGUIButton * ButtonScene;
	irr::gui::IGUIImage* Image;
	SDrawableTexture DrawableTex;

	scene::ISceneNode* CubeNode;
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
	ISceneManager* smgr = device->getSceneManager();

	SAppContext context;
	context.Device = device;

	smgr->addCameraSceneNode(0, vector3df(0,0,0), vector3df(0,0,30));
	context.CubeNode = smgr->addCubeSceneNode();
	if (context.CubeNode)
	{
		context.CubeNode->setMaterialFlag(video::EMF_LIGHTING, false);
		scene::ISceneNodeAnimator* anim = smgr->createFlyCircleAnimator(vector3df(0,0,30), 20.0f);
		if (anim)
		{
			context.CubeNode->addAnimator(anim);
			anim->drop();
		}
	}

	ITexture* defaultTexture = driver->addTexture(dimension2du( 512, 512), path("dummy"));
	context.addTexture(defaultTexture, L"default");

	context.ButtonOpenFile = env->addButton(core::rect<s32>(10, 10, 120, 30), 0, -1, L"file open");
	context.ButtonScene = env->addButton(core::rect<s32>(150, 10, 260, 30), 0, -1, L"show cube");
	context.ButtonScene->setIsPushButton(true);
	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);


	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			context.DrawableTex.uploadChangedImageToTexture();

			driver->beginScene(true, true, SColor(0,200,200,200));

			env->drawAll();
			if ( context.ButtonScene->isPressed() )
				smgr->drawAll();

			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
