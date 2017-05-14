// Test written by lumlum in Irrlicht forum (http://irrlicht.sourceforge.net/forum/viewtopic.php?f=4&t=51243)
// Loads and removes textures constantly.

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
    IrrlichtDevice *device = createDevice(video::EDT_DIRECT3D9, dimension2d<u32>(640, 480));
//	IrrlichtDevice *device = createDevice(video::EDT_OPENGL, dimension2d<u32>(640, 480));

    if (!device)
        return 1;

    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();
    IGUIEnvironment* guienv = device->getGUIEnvironment();

    int j = 0;
    ITexture *t;
    while(device->run())
    {
        if(j<20)
		{
            driver->removeAllTextures();
            t = driver->getTexture("my_media/64x64.jpg");
            j++;
		}
        else if(j<30)
		{
            driver->removeAllTextures();
            t = driver->getTexture("my_media/4096x4096.jpg");
            j++;
		}
        else
		{
            driver->removeAllTextures();
            t = driver->getTexture("my_media/64x64.jpg");
		}

        driver->beginScene(true, true, SColor(255,100,101,140));

        driver->draw2DImage(t, vector2d<s32>(0, 0));

        driver->endScene();
    }

	device->closeDevice();
    device->drop();

    return 0;
}
