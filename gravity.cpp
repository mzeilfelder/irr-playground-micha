// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder based on Irrlicht example 12.
//
// Testing gravity (100 units = 1 m)

#include <irrlicht.h>
#include "exampleHelper.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


class MyEventReceiver : public IEventReceiver
{
public:

	MyEventReceiver(scene::ITerrainSceneNode* terrain, irr::ITimer * timer, scene::ICameraSceneNode* camera) :
		Terrain(terrain), Timer(timer), Camera(camera)
	{
	}

	bool OnEvent(const SEvent& event)
	{
		// check if user presses the key 'W' or 'D'
		if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
		{
			switch (event.KeyInput.Key)
			{
			case irr::KEY_KEY_X:
				if ( Timer->isStopped() )
					Timer->start();
				else
					Timer->stop();
				return true;
			case irr::KEY_KEY_Y:
			{
				irr::core::vector3df camPos = Camera->getPosition();
				camPos.Y = Terrain->getHeight(camPos.X, camPos.Z) + 5000.f;
				Camera->setPosition(camPos);
				Camera->updateAbsolutePosition();
				return true;
			}
			default:
				break;
			}
		}

		return false;
	}

private:
	scene::ITerrainSceneNode* Terrain;
	irr::ITimer * Timer;
	scene::ICameraSceneNode* Camera;
};

int main()
{
	irr::SIrrlichtCreationParameters params;
	params.DriverType=video::EDT_OPENGL;
	params.WindowSize=core::dimension2d<u32>(640, 480);
	IrrlichtDevice* device = createDeviceEx(params);

	if (device == 0)
		return 1; // could not create selected driver.


	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	const io::path mediaPath = getExampleMediaPath();

	// add camera
	scene::ICameraSceneNode* camera =
		smgr->addCameraSceneNodeFPS(0,100.0f,1.2f);

	camera->setPosition(core::vector3df(2700*2,255*2,2600*2));
	camera->setTarget(core::vector3df(2397*2,343*2,2700*2));
	camera->setFarValue(42000.0f);

	// add terrain scene node
	scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
		mediaPath + "terrain-heightmap.bmp",
		0,					// parent node
		-1,					// node id
		core::vector3df(0.f, 0.f, 0.f),		// position
		core::vector3df(0.f, 0.f, 0.f),		// rotation
		core::vector3df(40.f, 4.4f, 40.f),	// scale
		video::SColor ( 255, 255, 255, 255 ),	// vertexColor
		5,					// maxLOD
		scene::ETPS_17,				// patchSize
		4					// smoothFactor
		);

	terrain->setMaterialFlag(video::EMF_LIGHTING, false);

	terrain->setMaterialTexture(0,
			driver->getTexture(mediaPath + "terrain-texture.jpg"));
	terrain->setMaterialTexture(1,
			driver->getTexture(mediaPath + "detailmap3.jpg"));
	terrain->setMaterialType(video::EMT_DETAIL_MAP);
	terrain->scaleTexture(1.0f, 20.0f);

	// create triangle selector for the terrain
	scene::ITriangleSelector* selector
		= smgr->createTerrainTriangleSelector(terrain, 0);
	terrain->setTriangleSelector(selector);

	// create collision response animator and attach it to the camera
	scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(
		selector, camera, core::vector3df(60,100,60),
		core::vector3df(0,-1000,0),	// gravity
		core::vector3df(0,50,0));
	selector->drop();
	camera->addAnimator(anim);
	anim->drop();

	// create event receiver
	MyEventReceiver receiver(terrain, device->getTimer(), camera);
	device->setEventReceiver(&receiver);


	while(device->run())
	{
		if (device->isWindowActive())
		{
			core::stringw str(L"Z: ");
			str += camera->getAbsolutePosition().Y;
			device->setWindowCaption(str.c_str());

			driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(0));

			smgr->drawAll();

			driver->endScene();
		}
		else
		{
			device->sleep(10);
		}
	}

	device->drop();

	return 0;
}

