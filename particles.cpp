// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing the particle system. Especially how positions are handled when it's invisible in between.

#include <irrlicht.h>
#include <iostream>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

namespace	// empty namespace makes sure all variables in here are only defined local in this file.
{
    const wchar_t* const DriverTypeNames[] =
    {
        L"NULL",
        L"SOFTWARE",
        L"BURNINGSVIDEO",
        L"DIRECT3D8",
        L"DIRECT3D9",
        L"EDT_OPENGL",
        0,
    };
};	// namespace


/*
Application configuration
*/
struct SConfig
{
	SConfig()
	: RenderInBackground(true)
	, DriverType(video::EDT_OPENGL)
	, ScreenSize(640, 480)
	{
	}

	bool RenderInBackground;
	video::E_DRIVER_TYPE DriverType;
	core::dimension2d<u32> ScreenSize;
};

/*
Main application class
*/
class CApp : public IEventReceiver
{
    friend int main(int argc, char *argv[]);

public:
    CApp()
	: IsRunning(false)
	, Device(0)
	, Camera(0)
	{
	}

    ~CApp()
	{
	}

	// stop running - will quit at end of mainloop
    void stopApp()
	{
		IsRunning = false;
	}

	virtual bool OnEvent(const SEvent &event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			if ( event.GUIEvent.Caller == ParticleVisibility )
			{
				for ( u32 i=0; i<ParticleNodes.size(); ++i )
				{
					ParticleNodes[i]->setVisible( !((gui::IGUIButton*)(event.GUIEvent.Caller))->isPressed() );
				}
			}
			else if( event.GUIEvent.Caller == ClearParticles )
			{
				for ( u32 i=0; i<ParticleNodes.size(); ++i )
				{
					ParticleNodes[i]->clearParticles();
				}			
			}
		}
	
		return false;
	}

protected:

    bool init(int argc, char *argv[])
	{
		Device = createDevice(Config.DriverType, Config.ScreenSize);
		if (!Device)
			return false;
		
        Device->setWindowCaption( DriverTypeNames[Config.DriverType] );
		Device->setEventReceiver(this);
		scene::ISceneManager* smgr = Device->getSceneManager();
		//video::IVideoDriver * driver = Device->getVideoDriver ();
		gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();

		// set a nicer font
		gui::IGUISkin* skin = guiEnv->getSkin();
		gui::IGUIFont* font = guiEnv->getFont("../../media/fonthaettenschweiler.bmp");
		if (font)
			skin->setFont(font);

		Camera = smgr->addCameraSceneNode (0, core::vector3df(0, 0, 300),
											core::vector3df(0, 0, 0));

		/*scene::ILightSceneNode* nodeLight = */smgr->addLightSceneNode(0, core::vector3df(0, 0, 0),
														video::SColorf(1.0f, 1.0f, 1.0f),
														100.0f);

		scene::ISceneNodeAnimator * animator =	smgr->createFlyCircleAnimator();
		scene::IParticleSystemSceneNode * node = smgr->addParticleSystemSceneNode(true, 0, -1, core::vector3df(0.f, 0.f, 0.f));
		node->addAnimator(animator);
		animator->drop();
		ParticleNodes.push_back( node );
		node = smgr->addParticleSystemSceneNode(true, 0, -1, core::vector3df(100.f, 0.f, 0.f));
		ParticleNodes.push_back( node );

		ParticleVisibility = guiEnv->addButton (core::recti(10, 10, 100, 30), 0, -1, L"visibility");
		ParticleVisibility->setIsPushButton(true); 
		ClearParticles = guiEnv->addButton (core::recti(10, 40, 100, 60), 0, -1, L"clear particles");

		return true;
	}

	video::E_DRIVER_TYPE getDriverTypeFromConsole()
	{
		printf("Please select the driver you want for this example:\n"\
			" (a) Direct3D 9.0c\n (b) Direct3D 8.1\n (c) OpenGL 1.5\n"\
			" (d) Software Renderer\n (e) Burning's Software Renderer\n"\
			" (f) NullDevice\n (otherKey) exit\n\n");

		char i;
		std::cin >> i;

		switch(i)
		{

			case 'a': return video::EDT_DIRECT3D9;
			case 'b': return video::EDT_DIRECT3D8;
			case 'c': return video::EDT_OPENGL;
			case 'd': return video::EDT_SOFTWARE;
			case 'e': return video::EDT_BURNINGSVIDEO;
			case 'f': return video::EDT_NULL;
			default: return video::E_DRIVER_TYPE(-1);
		}
	}

	bool update()
	{
		using namespace irr;

    	video::IVideoDriver* videoDriver =  Device->getVideoDriver();
		if ( !Device->run() )
			return false;

		if ( Device->isWindowActive() || Config.RenderInBackground )
		{
			gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();
			scene::ISceneManager* smgr = Device->getSceneManager();
			gui::IGUISkin * skin = guiEnv->getSkin();

			video::SColor bkColor( skin->getColor(gui::EGDC_APP_WORKSPACE) );
			videoDriver->beginScene(true, true, bkColor);

			smgr->drawAll();
			guiEnv->drawAll();

			videoDriver->endScene();
		}

		return true;
	}

    void run()
	{
		IsRunning = true;

		if ( !Device )
			return;

		// main application loop
		while(IsRunning)
		{
			if ( !update() )
				break;

			Device->sleep( 5 );
		}
	}

    void quit()
	{
		IsRunning = false;
		if ( Device )
		{
			Device->closeDevice();
			Device->drop();
			Device = NULL;
		}
	}

private:
	SConfig						Config;
    volatile bool				IsRunning;
	IrrlichtDevice * 			Device;
	scene::ICameraSceneNode *	Camera;
	gui::IGUIButton *			ParticleVisibility;
	gui::IGUIButton * 			ClearParticles;
	core::array< scene::IParticleSystemSceneNode*> ParticleNodes;
};

int main(int argc, char *argv[])
{
	CApp APP;

    if ( !APP.init(argc, argv) )
	{
		printf("init failed\n");
        return 1;
	}

	APP.run();
	APP.quit();

	return 0;
}
