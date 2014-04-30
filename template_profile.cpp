// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Template for quickly profiling stuff (just add/replace a scene)

#define ENABLE_MY_PROFILE
#ifdef ENABLE_MY_PROFILE
	#define MY_PROFILE(X) X
#else
	#define MY_PROFILE(X)
#endif // IRR_PROFILE

#include <irrlicht.h>

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif


using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

enum EProfiles
{
    EP_WELD
};

// For our example scenes
enum EScenes
{
	ES_NONE,	// no scene set
    ES_CUBE,
    ES_QUAKE_MAP,

    ES_COUNT	// counting how many scenes we have
};

class MyEventReceiver : public IEventReceiver
{
public:
	// constructor
	MyEventReceiver(ISceneManager * smgr) : GuiProfiler(0), IncludeOverview(true), ActiveScene(ES_NONE), SceneManager(smgr) {}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_KEY_INPUT_EVENT)
		{
			if ( event.KeyInput.PressedDown )
			{
				switch ( event.KeyInput.Key )
				{
					case KEY_F1:
						GuiProfiler->setVisible( !GuiProfiler->isVisible() );
					break;
					case KEY_F2:
						GuiProfiler->nextPage(IncludeOverview);
					break;
					case KEY_F3:
						GuiProfiler->previousPage(IncludeOverview);
					break;
					case KEY_F4:
						GuiProfiler->firstPage(IncludeOverview);
					break;
					case KEY_F6:
						GuiProfiler->setIgnoreUncalled( !GuiProfiler->getIgnoreUncalled() );
					break;
					case KEY_F8:
						NextScene();
					break;
					case KEY_F11:
						getProfiler().resetAll();
					break;
					default:
					break;
				}
			}
		}

		return false;
	}

	/*
		Some example scenes so we have something to profile
	*/
	void NextScene()
	{
		SceneManager->clear();
		ActiveScene = (ActiveScene+1) % ES_COUNT;
		if ( ActiveScene == 0 )
			ActiveScene = ActiveScene+1;

		switch ( ActiveScene )
		{
			case ES_CUBE:
			{
				// Simple scene with cube and light.
				MY_PROFILE(CProfileScope p(L"cube", L"scenes");)

				SceneManager->addCameraSceneNode (0, core::vector3df(0, 0, 0),
											core::vector3df(0, 0, 100),
											-1);

				SceneManager->addCubeSceneNode (30.0f, 0, -1,
										core::vector3df(0, 20, 100),
										core::vector3df(45, 45, 45),
										core::vector3df(1.0f, 1.0f, 1.0f));

				SceneManager->addLightSceneNode(0, core::vector3df(0, 0, 0),
										video::SColorf(1.0f, 1.0f, 1.0f),
										100.0f);
			}
			break;
			case ES_QUAKE_MAP:
			{
				// Our typical Irrlicht example quake map.
				MY_PROFILE(CProfileScope p(L"quake map", L"scenes");)

				scene::IAnimatedMesh* mesh = SceneManager->getMesh("20kdm2.bsp");
				if ( !mesh )
					break;

				scene::IMeshManipulator * manipulator = SceneManager->getMeshManipulator();
				MY_PROFILE(	getProfiler().start(EP_WELD);)
				manipulator->createMeshWelded(mesh->getMesh(0));
				MY_PROFILE(	getProfiler().stop(EP_WELD);)

				scene::ISceneNode* node = SceneManager->addOctreeSceneNode(mesh->getMesh(0), 0, -1, 1024);
				if (node)
					node->setPosition(core::vector3df(-1300,-144,-1249));
				SceneManager->addCameraSceneNodeFPS();
			}
			break;
		}
	}

	IGUIProfiler * GuiProfiler;
	bool IncludeOverview;
	u32 ActiveScene;
	scene::ISceneManager* SceneManager;
};

int main()
{
	video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
	{
		return 1; // could not create selected driver.
	}

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	scene::ISceneManager* smgr = device->getSceneManager();

	// A map we use for one of our test-scenes.
	device->getFileSystem()->addFileArchive("../../media/map-20kdm2.pk3");

	MyEventReceiver receiver(smgr);
	device->setEventReceiver(&receiver);
	receiver.NextScene();

	IGUIStaticText * staticText = env->addStaticText(
			L"F1 to show/hide the profiling display\n"
			L"F2 to show the next page\n"
			L"F3 to show the previous page\n"
			L"F4 to show the first page\n"
			L"F6 to flip between ignoring and showing uncalled data\n"
			L"F8 to change our scene\n"
			L"F11 to reset all data\n"
			, recti(10,10, 250, 120), true, true, 0, -1, true);
	staticText->setWordWrap(false);

	receiver.GuiProfiler = env->addProfilerDisplay(core::recti(40, 140, 600, 470));

	IGUIFont* font = env->getFont("../../media/fontcourier.bmp");
	if (font)
		receiver.GuiProfiler->setOverrideFont(font);

	MY_PROFILE(	getProfiler().add(EP_WELD, L"weld", L"main");)

	s32 lastFPS = -1;
	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			s32 fps = driver->getFPS();
			if (lastFPS != fps)
			{
				core::stringw str = L"FPS: ";
				str += fps;
				device->setWindowCaption(str.c_str());
				lastFPS = fps;
			}

			driver->beginScene(true, true, SColor(0,200,200,200));

			smgr->drawAll();
			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();

	MY_PROFILE(core::stringw output);
	MY_PROFILE(getProfiler().printAll(output));
	MY_PROFILE(printf("%s", core::stringc(output).c_str() ));

	return 0;
}

