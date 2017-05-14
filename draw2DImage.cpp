// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Test-cases for draw2DImage

#include <irrlicht.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

int main()
{
	dimension2d<u32> screenDim(640, 480);
	IrrlichtDevice *device = createDevice(video::EDT_OPENGL, screenDim);

    if (!device)
        return 1;

    IVideoDriver* driver = device->getVideoDriver();
//    ISceneManager* smgr = device->getSceneManager();
//    IGUIEnvironment* guienv = device->getGUIEnvironment();

    ITexture *t = driver->getTexture("my_media/4096x4096.jpg");
    while(device->run())
    {
        driver->beginScene(true, true, SColor(255,100,101,140));

		// grid
		for ( u32 i=0; i < screenDim.Width; i+= 16)
			driver->draw2DLine(core::position2di(i, 0), core::position2di(i, screenDim.Height-1), SColor(255, 127, 127, 127));
		for ( u32 i=0; i < screenDim.Height; i+= 16)
			driver->draw2DLine(core::position2di(0, i), core::position2di(screenDim.Width-1,i), SColor(255, 127, 127, 127));
		
		// clip against screen borders
		core::position2d<s32> destPos(-16,-16);
		core::rect<s32> sourceRect(32,32,64,64);
		driver->draw2DImage(t, destPos, sourceRect, 0, SColor(255,255,255,255), true);
		
		destPos = core::position2d<s32>(screenDim.Width-16, -16);
		driver->draw2DImage(t, destPos, sourceRect, 0, SColor(255,255,255,255), true);
		
		destPos = core::position2d<s32>(screenDim.Width-16, screenDim.Height-16);
		driver->draw2DImage(t, destPos, sourceRect, 0, SColor(255,255,255,255), true);
		
		destPos = core::position2d<s32>(-16, screenDim.Height-16);
		driver->draw2DImage(t, destPos, sourceRect, 0, SColor(255,255,255,255), true);

		// unclipped
		destPos = core::position2d<s32>(32, 16);
		driver->draw2DImage(t, destPos, sourceRect, 0, SColor(255,255,255,255), true);

		// clipped against clipping rect
		core::rect<s32> clipRect(80,16, 96, 32);
		destPos = core::position2d<s32>(70,5);
		driver->draw2DImage(t, destPos, sourceRect, &clipRect, SColor(255,255,255,255), true);

		clipRect = core::rect<s32>(112,16, 128, 32);
		destPos = core::position2d<s32>(120,20);
		driver->draw2DImage(t, destPos, sourceRect, &clipRect, SColor(255,255,255,255), true);

		clipRect = core::rect<s32>(144,16, 1000, 1000);
		destPos = core::position2d<s32>(150,20);
		driver->draw2DImage(t, destPos, sourceRect, &clipRect, SColor(255,255,255,255), true);

		driver->endScene();
    }

	device->closeDevice();
    device->drop();

    return 0;
}
