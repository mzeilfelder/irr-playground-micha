// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Using multi-render-target
#include <irrlicht.h>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

int main()
{
	core::dimension2du screenDim(640, 480);
	IrrlichtDevice *device = createDevice(video::EDT_OPENGL, screenDim);

	if (!device)
		return 1;

	video::IVideoDriver* driver = device->getVideoDriver();

	video::IRenderTarget* rt = driver->addRenderTarget();
	video::ITexture* rtt1 = driver->addRenderTargetTexture(screenDim, io::path("rtt1"), video::ECF_A16B16G16R16F);
	video::ITexture* rtt2 = driver->addRenderTargetTexture(screenDim, io::path("rtt2"), video::ECF_A16B16G16R16F);
	video::ITexture* depth = driver->addRenderTargetTexture(screenDim, io::path("depth"), video::ECF_D32);
	core::array<video::ITexture*> textures;
	textures.push_back(rtt1);
	textures.push_back(rtt2);
	rt->setTexture(textures, depth);

	while(device->run())
	{
		driver->beginScene(true, true, video::SColor(255,100,101,140));

		// just so we see original which should show
		driver->draw2DRectangle(video::SColor(255, 255, 100, 100), core::recti(100, 100, 500, 200));

		// now render same into rendertarget
		driver->setRenderTargetEx(rt, video::ECBF_ALL, video::SColor(255,0,50,100));

		// draw same again, this time into render target
		driver->draw2DRectangle(video::SColor(255, 255, 100, 100), core::recti(100, 100, 500, 200));
		driver->setRenderTargetEx(0, video::ECBF_NONE);

		// draw rtt, but smaller
		driver->draw2DImage(rtt1, core::recti(0, 240, 320, 480), core::recti(0,0,640,480));
		driver->draw2DImage(rtt2, core::recti(320, 240, 640, 480), core::recti(0,0,640,480));

		driver->endScene();
	}

	device->closeDevice();
	device->drop();

	return 0;
}
