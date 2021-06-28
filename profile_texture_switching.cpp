// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Test speed of switching textures

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace video;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


irr::video::ITexture* addColorTexture(video::IVideoDriver * videoDriver, irr::video::SColor c)
{
	SColor bgra(c.getBlue(), c.getGreen(), c.getRed(), c.getAlpha());	// ECF_A8R8G8B8 is BGRA in memory
	ITexture* result = 0;
	dimension2du dim(16,16);
	size_t num = dim.Width*dim.Height;
	SColor* data = new SColor[num];
	for ( size_t i=0; i < num; ++i )
	{
		data[i] = bgra;
	}
	IImage* img = videoDriver->createImageFromData(ECF_A8R8G8B8, dim, (void*)data, true, false);
	result = videoDriver->addTexture(io::path(c.color), img);
	delete img;
	delete[] data;
	return result;
}


int main(int argc, char* argv[])
{
	IrrlichtDevice *  device = createDevice(irr::video::EDT_DIRECT3D9, irr::core::dimension2d<irr::u32>(800,600));
	if (!device)
		return 0;

	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver * videoDriver = device->getVideoDriver ();
	IRandomizer* randomizer = device->getRandomizer();

	//videoDriver->setTextureCreationFlag(ETCF_SUPPORT_VERTEXT_TEXTURE, true);

	SMaterial m1, m2;
	for ( irr::u32 t = 0; t < irr::video::MATERIAL_MAX_TEXTURES_USED; ++t )
	{
		ITexture * t1 = addColorTexture(videoDriver, SColor(t*111));
		ITexture * t2 = addColorTexture(videoDriver, SColor(t*5000));
		m1.setTexture(t, t1);
		m2.setTexture(t, t2);
	}

	irr::u32 oldTimeDiff = INT_MAX;
	while ( device->run() )
	{
		if ( device->isWindowActive() )
		{
			irr::u32 timeStart = device->getTimer()->getRealTime();
			for ( size_t i=0; i<100000; ++i)	// just loop enough until it number have some meaning
			{
				videoDriver->setMaterial(m1);
				videoDriver->setMaterial(m2);
			}
			irr::u32 timeDiff = device->getTimer()->getRealTime() - timeStart;

			
			// update information 
			if ( timeDiff < oldTimeDiff )
			{
				oldTimeDiff = timeDiff;
				core::stringw str(L"MS: ");
				str.append(core::stringw(timeDiff));
				device->setWindowCaption( str.c_str() );
			}
		}
		else
		{
			device->sleep( 5 );
		}
	}

	device->closeDevice();
	device->drop();
	device = NULL;

	return 0;
}
