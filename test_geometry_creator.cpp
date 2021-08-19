// Code is under the zlib license.
// Written by Michael Zeilfelder
//
// Test geometry creator.
// Only torus so far - but might add more later

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#include <irrlicht.h>

using namespace irr;

namespace
{
	// some useful color constants
	const video::SColor SCOL_BLACK     = video::SColor(255, 0,   0,   0);
	const video::SColor SCOL_BLUE      = video::SColor(255, 0,   0,  255);
	const video::SColor SCOL_CYAN      = video::SColor(255, 0,  255, 255);
	const video::SColor SCOL_GRAY      = video::SColor(255, 128,128, 128);
	const video::SColor SCOL_GREEN     = video::SColor(255, 0,  255,  0);
	const video::SColor SCOL_MAGENTA   = video::SColor(255, 255, 0,  255);
	const video::SColor SCOL_RED       = video::SColor(255, 255, 0,   0);
	const video::SColor SCOL_YELLOW    = video::SColor(255, 255, 255, 0);
	const video::SColor SCOL_WHITE     = video::SColor(255, 255, 255, 255);
};

void createCaroTexture(video::IVideoDriver * driver)
{
	const u32 width = 256;
	const u32 height = 256;
	video::IImage * imageA8R8G8B8 = driver->createImage (video::ECF_A8R8G8B8, core::dimension2d<u32>(width, height));
	if ( !imageA8R8G8B8 )
		return;
	const u32 pitch = imageA8R8G8B8->getPitch();

	// Some nice square-pattern with 9 typical colors
	// Note that the function put readability over speed, you shouldn't use setPixel at runtime but for initialization it's nice.
	for ( u32 y = 0; y < height; ++ y )
	{
		for ( u32 x = 0; x < pitch; ++x )
		{
			if ( y < height/3 )
			{
				if ( x < width/3 )
					imageA8R8G8B8->setPixel (x, y, SCOL_BLACK);
				else if ( x < 2*width/3 )
					imageA8R8G8B8->setPixel (x, y, SCOL_BLUE);
				else
					imageA8R8G8B8->setPixel (x, y, SCOL_CYAN);
			}
			else if ( y < 2*height/3 )
			{
				if ( x < width/3 )
					imageA8R8G8B8->setPixel (x, y, SCOL_GRAY);
				else if ( x < 2*width/3 )
					imageA8R8G8B8->setPixel (x, y, SCOL_GREEN);
				else
					imageA8R8G8B8->setPixel (x, y, SCOL_MAGENTA);
			}
			else
			{
				if ( x < width/3 )
					imageA8R8G8B8->setPixel (x, y, SCOL_RED);
				else if ( x < 2*width/3 )
					imageA8R8G8B8->setPixel (x, y, SCOL_YELLOW);
				else
					imageA8R8G8B8->setPixel (x, y, SCOL_WHITE);
			}
		}
	}
	driver->addTexture (io::path("CARO_A8R8G8B8"), imageA8R8G8B8);

	imageA8R8G8B8->drop();
}


class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		/*
		Always return false by default. If you return true you tell the engine
		that you handled this event completely and the Irrlicht should not
		process it any further. So for example if you return true for all 
		EET_KEY_INPUT_EVENT events then Irrlicht would not pass on key-events
		to it's GUI system.
		*/
		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	
	MyEventReceiver()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};


int main()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType= video::EDT_OPENGL;

	MyEventReceiver receiver;
	IrrlichtDevice* device = createDevice(driverType,
			core::dimension2d<u32>(640, 480), 16, false, false, false, &receiver);
	if (device == 0)
		return 1; 

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	createCaroTexture(driver);

	scene::IMesh* torusMesh = smgr->getGeometryCreator()->createTorusMesh(30, 10, 32, 16, 135.f, 350.f, 3);
	scene::ISceneNode * torusNode = smgr->addMeshSceneNode(torusMesh);
	torusMesh->drop();
	if (torusNode)
	{
		torusNode->setPosition(core::vector3df(0,0,30));
		torusNode->getMaterial(0).setTexture(0, driver->getTexture("CARO_A8R8G8B8"));
		torusNode->setMaterialFlag(video::EMF_LIGHTING, false);
		torusNode->setDebugDataVisible(scene::EDS_NORMALS);
	}

	scene::ICameraSceneNode* cam = smgr->addCameraSceneNodeFPS();
	device->getCursorControl()->setVisible(false);

	u32 then = device->getTimer()->getTime();
	const f32 MOVEMENT_SPEED = 5.f;

	while(device->run())
	{
		cam->setInputReceiverEnabled(device->isWindowActive());

		// Work out a frame delta time.
		const u32 now = device->getTimer()->getTime();
		const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
		then = now;

		/* Check if keys W, S, A or D are being held down, and move the
		sphere node around respectively. */
		core::vector3df nodePosition = torusNode->getPosition();

		if(receiver.IsKeyDown(irr::KEY_KEY_W))
			nodePosition.Y += MOVEMENT_SPEED * frameDeltaTime;
		else if(receiver.IsKeyDown(irr::KEY_KEY_S))
			nodePosition.Y -= MOVEMENT_SPEED * frameDeltaTime;

		if(receiver.IsKeyDown(irr::KEY_KEY_A))
			nodePosition.X -= MOVEMENT_SPEED * frameDeltaTime;
		else if(receiver.IsKeyDown(irr::KEY_KEY_D))
			nodePosition.X += MOVEMENT_SPEED * frameDeltaTime;

		torusNode->setPosition(nodePosition);

		driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,113,113,133));

		smgr->drawAll(); // draw the 3d scene
		device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)

		driver->endScene();
	}

	device->drop();
	
	return 0;
}
