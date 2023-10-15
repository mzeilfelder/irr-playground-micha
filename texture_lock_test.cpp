// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Texture lock test

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
	SAppContext() : Device(0)
	{
	}

	void LoadImage(const irr::io::path& filename)
	{
		irr::video::ITexture * texture = Device->getVideoDriver()->getTexture(filename);
		addImageForTexture(texture, irr::core::stringw(filename).c_str());
	}

	// Each image added shows up left before the old ones 
	void addImageForTexture(irr::video::ITexture * texture, const wchar_t* text=0)
	{
		if ( !texture )
			return;
		bool useAlphaChannel=true;
		irr::gui::IGUIElement *parent=0;
		irr::gui::IGUIImage* image = Device->getGUIEnvironment()->addImage(texture, core::position2di(20, 50), useAlphaChannel, parent, -1, text);
		Images.insert(image, 0);
		core::recti r= image->getRelativePosition();
		irr::s32 m = r.getWidth() + 5;
		for ( u32 i=1; i<Images.size(); ++i )
		{
			Images[i]->move( core::position2d<s32>(m,0) );
		}
	}

	IrrlichtDevice * Device;
	irr::core::array<irr::gui::IGUIImage*> Images;
};

int main()
{
//	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;
//	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
//	video::E_DRIVER_TYPE driverType = video::EDT_BURNINGSVIDEO;
	video::E_DRIVER_TYPE driverType = video::EDT_SOFTWARE;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(800, 600));
	if (device == 0)
		return 1; // could not create selected driver.

	device->setResizable(true);

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	SAppContext context;
	context.Device = device;

	driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT, true);
	//driver->setTextureCreationFlag(ETCF_ALLOW_NON_POWER_2, true);
	//dimension2du dimNPOT(128, 256);	// fine POT
	dimension2du dimNPOT(111, 211);	// evil NPOT
	IImage * img = driver->createImage(video::ECF_A8R8G8B8, dimNPOT);
	for ( u32 y = 0; y < dimNPOT.Height; ++y )
	{
		for (u32 x = 0; x < dimNPOT.Width; ++x )
		{
			irr::video::SColor col(255, x%255, y%255, 100);
			img->setPixel(x,y,col);
		}
	}
	ITexture * tex = driver->addTexture(io::path(L"npot"), img);
	context.addImageForTexture(tex);

#if 1	// modify
	ECOLOR_FORMAT colFormat = tex->getColorFormat();
	if ( colFormat == ECF_A8R8G8B8 )
	{
		dimension2du dimTex = tex->getSize();
		irr::video::SColor* data = (irr::video::SColor*)tex->lock();

		for ( u32 y = 0; y < dimTex.Height; ++y )
		{
			for (u32 x = 0; x < dimTex.Width; ++x )
			{
				// draw some borders
				if ( y == 0 )
					data[y*dimTex.Width+x].set(255, 255, 0, 0);
				else if ( y == dimTex.Height-1 )
					data[y*dimTex.Width+x].set(255, 255, 255, 0);
				else if ( x == 0 )
					data[y*dimTex.Width+x].set(255, 0, 255, 0);
				else if ( x == dimTex.Width-1 )
					data[y*dimTex.Width+x].set(255, 0, 0, 255);
				else
					// draw alternating lines
					data[y*dimTex.Width+x].set(255, 127, (y%2)*200, (x%2)*200);
			}
		}
	}
	else if ( colFormat == ECF_A1R5G5B5 )	// only software driver right now
	{
		// Software driver returns data to original texture.
		// Reason is a bit tricky:
		// In 3D it works with POT textures
		// In 2D it works with original (sometimes NPOT) textures
		// So there isn't a single data to return here in the NPOT case.
		//dimension2du dimTex = tex->getSize();	
		dimension2du dimTex = tex->getOriginalSize();
		u16* data = (u16*)tex->lock();
		for ( u32 y = 0; y < dimTex.Height; ++y )
		{
			for (u32 x = 0; x < dimTex.Width; ++x )
			{
				// draw some borders
				if ( y == 0 )
					data[y*dimTex.Width+x] = RGBA16(255, 0, 0);
				else if ( y == dimTex.Height-1 )
					data[y*dimTex.Width+x] = RGBA16(255, 255, 0);
				else if ( x == 0 )
					data[y*dimTex.Width+x] = RGBA16(0, 255, 0);
				else if ( x == dimTex.Width-1 )
					data[y*dimTex.Width+x] = RGBA16(0, 0, 255);
				else
					// draw alternating lines
					data[y*dimTex.Width+x] = RGBA16(127, (y%2)*200, (x%2)*200);
			}
		}
	}

	tex->unlock();
#endif

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
