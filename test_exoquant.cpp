// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing exoquant (https://github.com/exoticorn/exoquant)
// exoquant.h and exoquant.c have to be added to the project.

#include "exoquant.h"
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

// works in both directions
void flipBGRAandRGBA(irr::video::IImage& image)
{
	if ( image.getBitsPerPixel() != 32 )
		return;

	irr::u8* data = static_cast<irr::u8*>(image.getData());
	irr::u32 numPixels = image.getImageDataSizeInPixels();
	for ( irr::u32 i=0; i < numPixels; ++ i)
	{
		irr::u8 b=*data;
		*data = *(data+2);
		*(data+2) = b;
		data += 4;
	}
}

irr::video::IImage* createQuantizedColorImage(irr::video::IVideoDriver* driver, const irr::video::IImage* imageIn, unsigned char numColors, bool dithered)
{
	// TODO: could optimize speed by changing exoquant so I can aovid the flipBGRAandRGBA calls.

	if ( !imageIn )
		return nullptr;

	void * ptrDbg = imageIn->getData();

	// ExoQuant needs data in r8g8b8a8
	flipBGRAandRGBA(*(const_cast<irr::video::IImage*>(imageIn)));	// data changed, but we change it back below, so const_cast is fine

	exq_data *pExq = exq_init(); // init quantizer (per image)
	
	exq_no_transparency(pExq);	// we don't want to multiply all pixels by the alpha

	int numPixels = static_cast<int>(imageIn->getImageDataSizeInPixels());
	unsigned char* inData = static_cast<unsigned char*>(imageIn->getData());
	exq_feed(pExq, inData, numPixels);
	exq_quantize(pExq, numColors); // find palette

	unsigned char * palette = new unsigned char[4*numColors];

	exq_get_palette(pExq, palette, numColors); // get palette

	unsigned char * paletteImage = new unsigned char[numPixels];
	if ( !dithered )
	{
		exq_map_image(pExq, static_cast<int>(imageIn->getImageDataSizeInPixels()), 
			inData,	paletteImage);	// map image to palette
	}
	else
	{
		exq_map_image_ordered(pExq, imageIn->getDimension().Width, imageIn->getDimension().Height, 
			inData,	paletteImage);	// alternative image mapping
	}

	flipBGRAandRGBA(*(const_cast<irr::video::IImage*>(imageIn)));	// just converting it back to original

	irr::video::IImage* imageOut = driver->createImage(irr::video::ECF_A8R8G8B8, imageIn->getDimension());
	irr::video::SColor * colData = static_cast<irr::video::SColor *>(imageOut->getData());
	for ( int i=0; i < numPixels; ++i )
	{
		const unsigned char index = paletteImage[i];
		*colData = irr::video::SColor( palette[index*4+3], palette[index*4], palette[index*4+1], palette[index*4+2] );
		++colData;
	}

	exq_free(pExq); 
	delete[] palette;
	delete[] paletteImage;

	return imageOut;
}

struct SAppContext
{
	SAppContext() : Device(0), ButtonOpenFile(0), ParentImages(0), ParentQuantImages(0)
	{
	}

	void Setup()
	{
		IGUIEnvironment* env = Device->getGUIEnvironment();

		ParentImages = env->addStaticText(L"", irr::core::recti(0,0, 99999, 9999), false, false, 0);
		ParentQuantImages = env->addStaticText(L"", irr::core::recti(0,0, 99999, 9999), false, false, 0);
		ParentQuantImages->setVisible(false);

		ButtonOpenFile = env->addButton(core::rect<s32>(20, 20, 120, 40), 0, -1, L"file open");
		ButtonSwitch = env->addButton(core::rect<s32>(20, 50, 120, 70), 0, -1, L"switch");
	}

	void LoadImage(const irr::io::path& filename)
	{
		irr::video::ITexture * texture = Device->getVideoDriver()->getTexture(filename);
		InsertImage(texture, Images, ParentImages, filename.c_str());

		irr::video::IImage * img1 = Device->getVideoDriver()->createImage(texture, irr::core::position2di(0,0),texture->getSize());
		irr::video::IImage * img2 = createQuantizedColorImage(Device->getVideoDriver(), img1, 255, true);
		irr::video::ITexture* texQuant = Device->getVideoDriver()->addTexture (irr::io::path("quant") + filename, img2);
		img1->drop();
		img2->drop();
		InsertImage(texQuant, ImagesQuantisized, ParentQuantImages, filename.c_str());
	}

	void InsertImage(irr::video::ITexture* texture, irr::core::array<irr::gui::IGUIImage*>& targetImageArray, irr::gui::IGUIElement *parent, const irr::io::path& filename)
	{
		bool useAlphaChannel=true;

		irr::gui::IGUIImage* image = Device->getGUIEnvironment()->addImage(texture, core::position2di(20, 50), useAlphaChannel, parent, -1, irr::core::stringw(filename).c_str());
		targetImageArray.insert(image, 0);
		core::recti r= image->getRelativePosition();
		irr::s32 m = r.getWidth() + 5;
		for ( u32 i=1; i<targetImageArray.size(); ++i )
		{
			targetImageArray[i]->move( core::position2d<s32>(m,0) );
		}
	}

	void Switch()
	{
		if ( ParentImages && ParentQuantImages )
		{
			if ( !ParentImages->isVisible() )
			{
				ParentImages->setVisible(true);
				ParentQuantImages->setVisible(false);
			}
			else
			{
				ParentImages->setVisible(false);
				ParentQuantImages->setVisible(true);
			}
		}
	}
	
	IrrlichtDevice * Device;
	IGUIButton * ButtonOpenFile;
	IGUIButton * ButtonSwitch;
	irr::gui::IGUIElement * ParentImages;
	irr::gui::IGUIElement * ParentQuantImages;
	irr::core::array<irr::gui::IGUIImage*> Images;
	irr::core::array<irr::gui::IGUIImage*> ImagesQuantisized;
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
					else if ( event.GUIEvent.Caller == Context.ButtonSwitch )
					{
						Context.Switch();
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
	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(800, 600));
	if (device == 0)
		return 1; // could not create selected driver.

	device->setResizable(true);

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	
	SAppContext context;
	context.Device = device;

	context.Setup();

	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);
	
	context.LoadImage("my_media/palette.png");

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

