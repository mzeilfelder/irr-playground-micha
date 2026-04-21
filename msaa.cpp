// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Using multi sample anti aliasing
#include <irrlicht.h>
#include "glad\glad.c"	// for modern GL functions

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver() : UseRT(false) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if ( event.EventType == EET_KEY_INPUT_EVENT
			&& event.KeyInput.Key == KEY_SPACE 
			&& event.KeyInput.PressedDown
			)
		{
			UseRT = !UseRT;
		}

		return false;
	}

	bool UseRT;
};

int main()
{
	core::dimension2du screenDim(1440, 960);

	SIrrlichtCreationParameters deviceParams;
	deviceParams.DriverType = video::EDT_OPENGL; // no support for other drivers
	deviceParams.WindowSize = screenDim;
	deviceParams.AntiAlias = 4;
	IrrlichtDevice *device = createDeviceEx(deviceParams);
	
	if (!device)
		return 1;

	if (gladLoadGL() <= 0)	// loading modern OpenGL functions
	{
		return 1;
	}

	MyEventReceiver receiver;
	device->setEventReceiver(&receiver);

	video::IVideoDriver* driver = device->getVideoDriver();

	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIFont* font = device->getGUIEnvironment()->getSkin()->getFont();

	smgr->addCameraSceneNode (0, core::vector3df(0, 0, 0),
								core::vector3df(0, 0, 100),
								-1);

	scene::ISceneNode* cubeNode = smgr->addCubeSceneNode (30.0f, 0, -1, 
	                        core::vector3df(0, 20, 100),
							core::vector3df(45, 45, 45),
							core::vector3df(1.0f, 1.0f, 1.0f));
	video::SMaterial& cubeMat = cubeNode->getMaterial(0);
	cubeMat.Lighting = false;
	cubeMat.AntiAliasing = video::EAAM_FULL_BASIC;

	video::IRenderTarget* rt = driver->addRenderTarget();
	const u32 multiSamples = 4;	// set to 0 to see without aliasing
	video::ITexture* rtt1 = driver->addRenderTargetTexture(screenDim, io::path("rtt1"), video::ECF_A16B16G16R16F, multiSamples);
	video::ITexture* rtt2 = driver->addRenderTargetTexture(screenDim, io::path("rtt2"), video::ECF_A16B16G16R16F, multiSamples);	// not using this one here, just added for testing
	video::ITexture* depth = driver->addRenderTargetTexture(screenDim, io::path("depth"), video::ECF_D32, multiSamples);
	core::array<video::ITexture*> textures;
	textures.push_back(rtt1);
	textures.push_back(rtt2);
	rt->setTexture(textures, depth);

	while(device->run())
	{
		video::SColor bkCol(255,120,120,120);
		driver->beginScene(true, true, bkCol);

		if ( receiver.UseRT )
		{
			driver->setRenderTargetEx(rt, video::ECBF_ALL, bkCol);
		}

		smgr->drawAll();

		if ( receiver.UseRT )
		{
			driver->setRenderTargetEx(nullptr, 0);

			if ( multiSamples == 0 )
			{
				driver->draw2DImage(rtt1, core::vector2di(0,0));
			}
			else
			{
				// Can't use textures directly with fixed function pipeline
				// Code below _only_ works if framebuffer has same sizes as rt 
				// (so currently breaks when resizing, but that's because I'm too lazy to re-create the rtt's - could be made to work)
				glBindFramebuffer(GL_READ_FRAMEBUFFER, rt->getExposedRenderTargetData().OpenGL.FramebufferName);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // default framebuffer
				glBlitFramebuffer(0, 0, screenDim.Width, screenDim.Height, 0, 0, screenDim.Width, screenDim.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			}
		}

		if ( receiver.UseRT )
		{
			font->draw(L"Rendertarget", core::recti(1,1, 100, 20), video::SColor(255, 255, 255, 255));
		}
		else
		{
			font->draw(L"Screen", core::recti(1,1, 100, 20), video::SColor(255, 255, 255, 255));
		}

		driver->endScene();
	}

	device->closeDevice();
	device->drop();

	return 0;
}
