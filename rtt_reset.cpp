// Testing behavior of resetting rtt's 

#include <irrlicht.h>
#include "driverChoice.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


#if IRRLICHT_VERSION_MAJOR > 1 || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)
#define HAS_IRR_VERSION_1_9
#endif

int main()
{
	// ask user for driver
#if 1
	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
#else
	video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;
#endif
	if (driverType==video::EDT_COUNT)
		return 1;

	IrrlichtDevice *device = createDevice(driverType, core::dimension2d<u32>(640, 480), 16, false, false);
	if (!device)
		return 1;

	video::IVideoDriver* driver = device->getVideoDriver();
	if (!driver->queryFeature(video::EVDF_RENDER_TO_TARGET))
		return 1;

	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	// create render target
	const core::dimension2d<u32> rtDim(256, 256);	// always use same size for render target texture and it's depth-buffer

	video::ITexture* renderTargetTex1 = driver->addRenderTargetTexture(rtDim, "RTT1", video::ECF_A8R8G8B8);
#if defined (HAS_IRR_VERSION_1_9)
	video::IRenderTarget* renderTarget1 = driver->addRenderTarget();
	video::ITexture* renderTargetDepth1 = driver->addRenderTargetTexture(rtDim, "DepthStencil1", video::ECF_D16); 
	renderTarget1->setTexture(renderTargetTex1, renderTargetDepth1);
#endif

	video::ITexture* renderTargetTex2 = driver->addRenderTargetTexture(rtDim, "RTT2", video::ECF_A8R8G8B8);
#if defined (HAS_IRR_VERSION_1_9)
	video::IRenderTarget* renderTarget2 = driver->addRenderTarget();
	renderTarget2 = driver->addRenderTarget();
	video::ITexture* renderTargetDepth2 = driver->addRenderTargetTexture(rtDim, "DepthStencil2", video::ECF_D16); 
	renderTarget2->setTexture(renderTargetTex2, renderTargetDepth2);
#endif

	if (!renderTargetTex1 || !renderTargetTex2 )
		return 1;

#if defined (HAS_IRR_VERSION_1_9)
	if ( !renderTarget1 || !renderTarget2 )
		return 1;
#endif
	
	while(device->run())
	{
		if (device->isWindowActive())
		{
#if defined (HAS_IRR_VERSION_1_9)
			driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(0));

			driver->setRenderTargetEx(renderTarget1, video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,0,255,0));
			driver->draw2DRectangle( core::recti(118, 118, 138, 138), video::SColor(255, 255, 0, 0 ), video::SColor(255, 0, 0, 255), video::SColor(255, 127, 0, 0), video::SColor(255, 0, 0, 127));
			{
				driver->setRenderTargetEx(renderTarget2, video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,0,0,255));
				driver->draw2DRectangle( core::recti(118, 118, 138, 138), video::SColor(255, 0, 255, 0 ), video::SColor(255, 0, 0, 255), video::SColor(255, 0, 227, 0), video::SColor(255, 0, 0, 127));
				driver->setRenderTargetEx(0, 0);
			}
			driver->draw2DRectangle( core::recti(18, 18, 38, 38), video::SColor(255, 255, 255, 0 ), video::SColor(255, 0, 0, 255), video::SColor(255, 127, 127, 0), video::SColor(255, 0, 0, 127));
			driver->setRenderTargetEx(0, 0);
#else
			driver->beginScene(true, true, video::SColor(0));

			driver->setRenderTarget(renderTargetTex1, true, true , video::SColor(255,0,255,0));
			driver->draw2DRectangle( core::recti(118, 118, 138, 138), video::SColor(255, 255, 0, 0 ), video::SColor(255, 0, 0, 255), video::SColor(255, 127, 0, 0), video::SColor(255, 0, 0, 127));
			{
				driver->setRenderTarget(renderTargetTex2, true, true, video::SColor(255,0,0,255));
				driver->draw2DRectangle( core::recti(118, 118, 138, 138), video::SColor(255, 0, 255, 0 ), video::SColor(255, 0, 0, 255), video::SColor(255, 0, 227, 0), video::SColor(255, 0, 0, 127));
				driver->setRenderTarget(0, false, false);
			}
			driver->draw2DRectangle( core::recti(18, 18, 38, 38), video::SColor(255, 255, 255, 0 ), video::SColor(255, 0, 0, 255), video::SColor(255, 127, 127, 0), video::SColor(255, 0, 0, 127));
			driver->setRenderTarget(0, false, false);
#endif
			driver->draw2DImage(renderTargetTex1, core::vector2di(10,50));
			driver->draw2DImage(renderTargetTex2, core::vector2di(300,50));

			driver->endScene();
		}
	}

	device->closeDevice();
	device->drop(); // drop device
	return 0;
}
