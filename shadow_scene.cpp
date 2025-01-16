// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Shadow scene to reproduce bug reported at https://irrlicht.sourceforge.io/forum/viewtopic.php?p=308069


#include <irrlicht.h>
//#include "driverChoice.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver()
	{
		for (u32 i=0; i<EKA_COUNT; ++i)
			CursorKeys[i] = false;

		KeyMap.push_back(SKeyMap(EKA_MOVE_FORWARD, irr::KEY_KEY_W));
		KeyMap.push_back(SKeyMap(EKA_MOVE_BACKWARD, irr::KEY_KEY_S));
		KeyMap.push_back(SKeyMap(EKA_STRAFE_LEFT, irr::KEY_KEY_A));
		KeyMap.push_back(SKeyMap(EKA_STRAFE_RIGHT, irr::KEY_KEY_D));
		KeyMap.push_back(SKeyMap(EKA_ROTATE_LEFT, irr::KEY_KEY_Q));
		KeyMap.push_back(SKeyMap(EKA_ROTATE_RIGHT, irr::KEY_KEY_E));
		KeyMap.push_back(SKeyMap(EKA_JUMP_UP, irr::KEY_SPACE));
	}

	virtual bool OnEvent(const SEvent &event)
	{
		switch(event.EventType)
		{
		case EET_KEY_INPUT_EVENT:
			for (u32 i=0; i<KeyMap.size(); ++i)
			{
				if (KeyMap[i].KeyCode == event.KeyInput.Key)
				{
					CursorKeys[KeyMap[i].Action] = event.KeyInput.PressedDown;
					return true;
				}
			}
			break;

		default:
			break;
		}

		return false;
	}

	void animateNode(irr::scene::ISceneNode* node, u32 timeMs)
	{
		if (LastAnimationTime <= 0 )
		{
			LastAnimationTime = timeMs;
			return;
		}

		// get time
		f32 timeDiff = (f32) ( timeMs - LastAnimationTime );
		LastAnimationTime = timeMs;

		core::vector3df rot = node->getRotation();

		// keyboard rotation
		if (CursorKeys[EKA_ROTATE_LEFT])
			rot.Y -= timeDiff * RotateSpeedKeyboard;

		if (CursorKeys[EKA_ROTATE_RIGHT])
			rot.Y += timeDiff * RotateSpeedKeyboard;
		node->setRotation( rot );

		core::vector3df movedir(0,0,-1);
		movedir.rotateXZBy(rot.Y);
		movedir.normalize();

		core::vector3df pos  = node->getPosition();
		if (CursorKeys[EKA_MOVE_FORWARD])
			pos += movedir * timeDiff * MoveSpeed;

		if (CursorKeys[EKA_MOVE_BACKWARD])
			pos -= movedir * timeDiff * MoveSpeed;

		core::vector3df strafevect = movedir.crossProduct(core::vector3df(0,1,0));
		strafevect.normalize();

		if (CursorKeys[EKA_STRAFE_LEFT])
			pos += strafevect * timeDiff * MoveSpeed;
		if (CursorKeys[EKA_STRAFE_RIGHT])
			pos -= strafevect * timeDiff * MoveSpeed;

		// For jumping, we find the collision response animator attached to our camera
		// and if it's not falling, we tell it to jump.
		if (CursorKeys[EKA_JUMP_UP])
		{
			// TODO: think about it tomorrow
			//const ISceneNodeAnimatorList& animators = camera->getAnimators();
			//ISceneNodeAnimatorList::ConstIterator it = animators.begin();
			//while(it != animators.end())
			//{
			//	if(ESNAT_COLLISION_RESPONSE == (*it)->getType())
			//	{
			//		ISceneNodeAnimatorCollisionResponse * collisionResponse =
			//			static_cast<ISceneNodeAnimatorCollisionResponse *>(*it);

			//		if(!collisionResponse->isFalling())
			//			collisionResponse->jump(JumpSpeed);
			//	}

			//	it++;
			//}
		}

		node->setPosition(pos);
	}


	f32 MoveSpeed = 0.5f;
	f32 RotateSpeedKeyboard = 0.3f;
	s32 LastAnimationTime = 0;
	core::array<SKeyMap> KeyMap;
	bool CursorKeys[EKA_COUNT];
};

int main()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType= video::EDT_DIRECT3D9;
//	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;

	IrrlichtDevice *device =createDevice(driverType, core::dimension2d<u32>(800, 600), 16, false, true);
	if (!device)
		return 1;

	MyEventReceiver eventReceiver;
	device->setEventReceiver(&eventReceiver);

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	// scene (just a plane for now)
	scene::IMesh* meshPlane = smgr->getGeometryCreator()->createPlaneMesh(core::dimension2df(100.f, 100.f), core::dimension2du(20, 20));
	scene::IMeshSceneNode* nodePlane = smgr->addMeshSceneNode(meshPlane);
	nodePlane->setMaterialTexture(0, driver->getTexture("../../media/terrain-texture.jpg"));
	meshPlane->drop(); meshPlane = 0;


	scene::ITriangleSelector * sceneSelector = smgr->createTriangleSelector(nodePlane->getMesh(), nodePlane);
	nodePlane->setTriangleSelector(sceneSelector);

	const bool zfail = true;
	const f32 infinity = 150.f;

	scene::IAnimatedMeshSceneNode* playerNode = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/dwarf.x"),0, 1);
	if ( playerNode )
	{
		playerNode->setPosition(core::vector3df(50,1,50)); // Put its feet on the floor and bit off from center
		playerNode->setAnimationSpeed(20.f);
		playerNode->getMaterial(0).Lighting = true;

		scene::IShadowVolumeSceneNode* shadow = playerNode->addShadowVolumeSceneNode(0, -1, zfail, infinity);
	//	shadow->setDebugDataVisible(scene::EDS_MESH_WIRE_OVERLAY);

		// TODO: not yet working, sinks into floor
		//scene::ISceneNodeAnimatorCollisionResponse* ani = smgr->createCollisionResponseAnimator(sceneSelector, playerNode, playerNode->getBoundingBox().getExtent());
		//playerNode->addAnimator(ani);
		//ani->drop();
	}
	sceneSelector->drop(); sceneSelector = 0;

	core::vector3df camRelDist(0, 200, 150);
	core::vector3df camRelTarget(0,0,0);
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNode(0, camRelDist, camRelTarget);

	// Add a spotlight
	scene::ILightSceneNode* spotlight = smgr->addLightSceneNode(0, core::vector3df(0, 0, 0), video::SColorf(1.0f, 1.0f, 1.0f), 21.7f);
	if (spotlight) 
	{
		spotlight->getLightData().Type = irr::video::ELT_POINT;
		spotlight->enableCastShadow(true);

		//scene::IBillboardSceneNode* bbNode = smgr->addBillboardSceneNode(spotlight, core::dimension2d<f32>(50, 50));
		//bbNode->setMaterialFlag(video::EMF_LIGHTING, false);
		//bbNode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		//bbNode->setMaterialTexture(0, driver->getTexture("../../media/particlewhite.bmp"));
	}

	scene::ILightSceneNode* camplight = smgr->addLightSceneNode(0, core::vector3df(-10, 20, -20), video::SColorf(1.0f, 1.0f, 1.0f), 10.7f);
	if (camplight) 
	{
		camplight->getLightData().Type = irr::video::ELT_POINT;
		camplight->getLightData().DiffuseColor = video::SColorf(1.0f, 0.2f, 0.0f); // (orange)
		camplight->enableCastShadow(false);

		scene::IBillboardSceneNode* bbNode = smgr->addBillboardSceneNode(camplight, core::dimension2d<f32>(50, 50));
		bbNode->setMaterialFlag(video::EMF_LIGHTING, false);
		bbNode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		bbNode->setMaterialTexture(0, driver->getTexture("../../media/particlewhite.bmp"));
	}

	smgr->setAmbientLight(video::SColor(255, 50, 50, 50));
//	smgr->setShadowColor(video::SColor(150,0,255,0));

	while(device->run())
	{
		if (device->isWindowActive())
		{
			eventReceiver.animateNode(playerNode, device->getTimer()->getRealTime());
			
			core::vector3df playerPos = playerNode->getPosition();
			camera->setPosition(playerPos+camRelDist);
			camera->setTarget(playerPos+camRelTarget);

			driver->beginScene(true, true);

			spotlight->setPosition(core::vector3df(playerNode->getPosition().X, 20, playerNode->getPosition().Z));

			smgr->drawAll();

			driver->endScene();
		}
		else
		{
			device->yield();
		}
	}

	device->drop();

	return 0;
}
