// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Basically the same as example 30 in Irrlicht (example was based on this code)

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
	EP_APP_TIME_ONCE,
	EP_APP_TIME_UPDATED,
	EP_SCOPE1,
	EP_SCOPE2,
	EP_DRAW_SCENE
};

// For our example scenes
enum EScenes
{
	ES_NONE,	// no scene set
    ES_CUBE,
    ES_QUAKE_MAP,
    ES_DWARVES,

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
					case KEY_F5:
						IncludeOverview = !IncludeOverview;
					break;
					case KEY_F6:
						GuiProfiler->setIgnoreUncalled( !GuiProfiler->getIgnoreUncalled() );
					break;
					case KEY_F8:
						NextScene();
					break;
					case KEY_F9:
					{
						u32 index = 0;
						if ( getProfiler().findGroupIndex(index, L"group a") )
						{
							getProfiler().resetGroup(index);
						}
					}
					break;
					case KEY_F10:
					{
						u32 index = 0;
						if ( getProfiler().findDataIndex(index, L"scope 3") )
						{
							getProfiler().resetDataByIndex(index);
						}
					}
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
				scene::ISceneNode* node = 0;

				if (mesh)
					node = SceneManager->addOctreeSceneNode(mesh->getMesh(0), 0, -1, 1024);
				if (node)
					node->setPosition(core::vector3df(-1300,-144,-1249));
				SceneManager->addCameraSceneNodeFPS();
			}
			break;
			case ES_DWARVES:
			{
				// Stress-test Irrlicht a little bit by creating many objects.
				MY_PROFILE(CProfileScope p(L"dwarfes", L"scenes");)

				scene::IAnimatedMesh* aniMesh = SceneManager->getMesh( "../../media/dwarf.x" );
				if (aniMesh)
				{
					scene::IMesh * mesh = aniMesh->getMesh (0);
					if ( !mesh )
						break;

					// You can never have too many dwarves. So let's make some.
					const int nodesX = 20;
					const int nodesY = 5;
					const int nodesZ = 20;

					aabbox3df bbox = mesh->getBoundingBox();
					vector3df extent = bbox.getExtent();
					const f32 GAP = 10.f;
					f32 halfSizeX = 0.5f * (nodesX*extent.X + GAP*(nodesX-1));
					f32 halfSizeY = 0.5f * (nodesY*extent.Y + GAP*(nodesY-1));
					f32 halfSizeZ = 0.5f * (nodesZ*extent.Z + GAP*(nodesZ-1));

					for ( int x = 0; x < nodesX; ++x )
					{
						irr::f32 gapX = x > 0 ? (x-1)*GAP : 0.f;
						irr::f32 posX = -halfSizeX + x*extent.X + gapX;
						for ( int y = 0; y < nodesY; ++y )
						{
							irr::f32 gapY = y > 0 ? (y-1)*GAP : 0.f;
							irr::f32 posY = -halfSizeY + y*extent.Y + gapY;
							for ( int z=0; z < nodesZ; ++z )
							{
								irr::f32 gapZ = z > 0 ? (z-1)*GAP : 0.f;
								irr::f32 posZ = -halfSizeZ + z*extent.Z + gapZ;
								scene::IMeshSceneNode * node = SceneManager->addMeshSceneNode (mesh, NULL, -1, vector3df(posX, posY, posZ) );
								node->setMaterialFlag(video::EMF_LIGHTING, false);
							}
						}
					}

					irr::scene::ICameraSceneNode * camera = SceneManager->addCameraSceneNodeFPS(0, 20.f, 0.1f );
					camera->updateAbsolutePosition();
					camera->setTarget( vector3df(0,0,0) );
					camera->updateAbsolutePosition();
					camera->setPosition(irr::core::vector3df(halfSizeX+extent.X, halfSizeY+extent.Y, halfSizeZ+extent.Z));
					camera->updateAbsolutePosition();
				}
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
			L"F5 to flip between including the group overview\n"
			L"F6 to flip between ignoring and showing uncalled data\n"
			L"F8 to change our scene\n"
			L"F9 to reset the \"group a\" data\n"
			L"F10 to reset the scope 3 data\n"
			L"F11 to reset all data\n"
			, recti(10,10, 250, 120), true, true, 0, -1, true);
	staticText->setWordWrap(false);

	receiver.GuiProfiler = env->addProfilerDisplay(core::recti(40, 140, 600, 470));

	IGUIFont* font = env->getFont("../../media/fontcourier.bmp");
	if (font)
		receiver.GuiProfiler->setOverrideFont(font);

	MY_PROFILE(
		getProfiler().add(EP_APP_TIME_ONCE, L"full time", L"group a");
		getProfiler().add(EP_APP_TIME_UPDATED, L"full time updated", L"group a");
		getProfiler().add(EP_SCOPE1, L"scope 1", L"group a");
		getProfiler().add(EP_DRAW_SCENE, L"draw scene", L"group a");
	)

	MY_PROFILE(getProfiler().start(EP_APP_TIME_ONCE);)
	MY_PROFILE(getProfiler().start(EP_APP_TIME_UPDATED);)

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

			MY_PROFILE(getProfiler().stop(EP_APP_TIME_UPDATED);)
			MY_PROFILE(getProfiler().start(EP_APP_TIME_UPDATED);)

			MY_PROFILE(CProfileScope p3(L"scope 3", L"group a");)
			MY_PROFILE(CProfileScope p2(EP_SCOPE2, L"scope 2", L"group a");)
			MY_PROFILE(CProfileScope p1(EP_SCOPE1));
			
			// just filling up - need to blow the amount of profiles fitting into the display
			for ( int i=0; i<50; ++i )
			{
				core::stringw s(i);
				MY_PROFILE(CProfileScope p3(s.c_str(), L"group a");)
			}
			

			driver->beginScene(true, true, SColor(0,200,200,200));

			MY_PROFILE(getProfiler().start(EP_DRAW_SCENE);)
			smgr->drawAll();
			MY_PROFILE(getProfiler().stop(EP_DRAW_SCENE);)

			MY_PROFILE(s32 pEnv = getProfiler().add(L"draw env", L"group a");)
			MY_PROFILE(getProfiler().start(pEnv);)
			env->drawAll();
			MY_PROFILE(getProfiler().stop(pEnv);)

			driver->endScene();
		}
	}

	device->drop();

	MY_PROFILE(getProfiler().stop(EP_APP_TIME_UPDATED));
	MY_PROFILE(getProfiler().stop(EP_APP_TIME_ONCE));

	MY_PROFILE(core::stringw output);
	MY_PROFILE(getProfiler().printAll(output));
	MY_PROFILE(printf("%s", core::stringc(output).c_str() ));

	return 0;
}

