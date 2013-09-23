// Code is under the zlib license.
//
// Simple example for a 3rd person camera.
// Some notes:
// - Unlike Irrlicht cameras this camera is not using an animator, which means you have to call Update each frame
// - This camera does not do the movement. Instead I expect the target-node to move and the camera is just following that node.
// - This camera doesn't do input handling - it expects another class to do that (makes sense if you support more than one input device)
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")
#endif

#include <irrlicht.h>
#include <iostream>

using namespace irr;


class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

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


struct ThirdPersonCamSettings
{
    // camera target relative to the pivot of the target node
    irr::core::vector3df mTargetRelativeToNode;

    // with no other factors influencing it this is the direction of the camera starting from the target.
    irr::core::vector3df mDefaultDistanceDir;

    float mMaxDistance;
    float mMinDistance;
    float mZoomStepSize;
    float mFarValue;
    float mRotationSpeedHorizontal;
    float mRotationSpeedVertical;
};

class ThirdPersonCam
{
public:
    ThirdPersonCam()
	: mSceneManager(0)
	, mIrrCamNode(0)
	, mTargetNode(0)
	, mCurrentDistance(210.f)
	, mRotateVertical(0)
	, mRotateHorizontal(0)
	{
		mSettings.mTargetRelativeToNode = irr::core::vector3df(0.f, 50.f, 0.f);
		mSettings.mDefaultDistanceDir = irr::core::vector3df(0.f, 2.0f, -100.f);
		mSettings.mMaxDistance = 300.f;
		mSettings.mMinDistance = 60.f;
		mSettings.mZoomStepSize = 30.f;
		mSettings.mFarValue = 20000.f;
		mSettings.mRotationSpeedHorizontal = 10.f;
		mSettings.mRotationSpeedVertical = 10.f;
	}
	
    virtual void Init(irr::scene::ISceneManager* sceneManager)
	{
		mSceneManager = sceneManager;

		mIrrCamNode = mSceneManager->addCameraSceneNode();
		mIrrCamNode->setFarValue(mSettings.mFarValue);
		mIrrCamNode->setInputReceiverEnabled ( false );
		mIrrCamNode->bindTargetAndRotation(true);
	}

    void SetTargetNode(irr::scene::ISceneNode * node)
	{
		if ( mTargetNode == node )
			return;
	
		if ( mTargetNode )
		{
			mTargetNode->drop();
			mTargetNode = NULL;
		}
		mTargetNode = node;
		if ( mTargetNode )
		{
			mTargetNode->grab();
			mOldTargetPos = mTargetNode->getAbsolutePosition();
		}		
	}
	
    void Update()
	{
		mOldTargetPos = mTargetNode->getAbsolutePosition();
	
		irr::core::vector3df dist( mSettings.mDefaultDistanceDir );
	
		irr::core::matrix4 mat = mTargetNode->getRelativeTransformation();
		core::matrix4 matRot;
		float rotateX = mRotateVertical*mSettings.mRotationSpeedVertical;
		float rotateY = mRotateHorizontal*mSettings.mRotationSpeedHorizontal;
		matRot.setRotationDegrees(core::vector3df(rotateX, rotateY, 0));
		mat *= matRot;
	
		mat.rotateVect(dist);
		dist.setLength( mCurrentDistance );
	
		irr::core::vector3df targetPos = mTargetNode->getPosition() + dist + mSettings.mTargetRelativeToNode;
		mIrrCamNode->setPosition( targetPos );
		mIrrCamNode->updateAbsolutePosition();
		// setTarget will set also the rotation.
		// This only works if setPosition and updateAbsolutePosition are called first otherwise the rotation would show to the old target.
		mIrrCamNode->setTarget( mTargetNode->getPosition() + mSettings.mTargetRelativeToNode );
		mIrrCamNode->updateAbsolutePosition();		
	}

	void ZoomOut()
	{
		mCurrentDistance += mSettings.mZoomStepSize;
		if ( mCurrentDistance > mSettings.mMaxDistance )
			mCurrentDistance = mSettings.mMaxDistance;
	}
	
	void ZoomIn()
	{
		mCurrentDistance -= mSettings.mZoomStepSize;
		if ( mCurrentDistance < mSettings.mMinDistance )
			mCurrentDistance = mSettings.mMinDistance;
	}
	
	void RotateVertical(float val)
	{
		mRotateVertical -= val;
		if ( mRotateVertical < 0.f )
			mRotateVertical = 0.f;
		if ( mRotateVertical*mSettings.mRotationSpeedVertical > 50.f )
		{
			mRotateVertical = 50/mSettings.mRotationSpeedVertical;
		}
	}
	
	void RotateHorizontal(float val)
	{
		mRotateHorizontal -= val;
	}
	
	void SetHorizontalRotation(float val)
	{
		mRotateHorizontal = val;
	}
	
	void SetVerticalRotation(float val)
	{
		mRotateVertical = val;
	}
	
	irr::scene::ICameraSceneNode* GetIrrCamNode() { return mIrrCamNode;	}
	
private:
    irr::scene::ISceneManager* mSceneManager;
    irr::scene::ICameraSceneNode* mIrrCamNode;	
    irr::scene::ISceneNode * mTargetNode;
    irr::core::vector3df mOldTargetPos;
    float mCurrentDistance;
    float mRotateVertical;
    float mRotateHorizontal;
    ThirdPersonCamSettings mSettings;
};

void UpdatePicking(scene::ICameraSceneNode* camera, scene::ISceneCollisionManager * collManager, gui::ICursorControl* cursor)
{
    static scene::ISceneNode* oldNode = 0;
    scene::ISceneNode* pickedNode = 0;
    core::position2d<s32> cursorpos  = cursor->getPosition();
	pickedNode = collManager->getSceneNodeFromScreenCoordinatesBB(cursorpos, 1);
	
	// draw debug boxes to show that node got picked
    if ( oldNode )
    {
        oldNode->setDebugDataVisible(scene::EDS_OFF);
    }
    if ( pickedNode )
    {
        pickedNode->setDebugDataVisible(scene::EDS_BBOX);
    }
    oldNode = pickedNode;	
}

int main()
{
	// let user select driver type

	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;

	// create device
	MyEventReceiver receiver;

	IrrlichtDevice* device = createDevice(driverType,
			core::dimension2d<u32>(640, 480), 16, false, false, false, &receiver);

	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	irr::scene::ISceneNode * node1 = smgr->addEmptySceneNode( NULL );
	node1->setID(1);
	irr::scene::ISceneNode * node2 = smgr->addEmptySceneNode( node1 );
	node2->setID(0);
	node2->setScale(core::vector3df(10.f, 10.f, 10.f));

	scene::IAnimatedMeshSceneNode* anms =
		smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/ninja.b3d"), node2, 1);

	if (anms)
	{
		anms->setMaterialFlag(video::EMF_LIGHTING, false);
		anms->setFrameLoop(0, 14);
		anms->setAnimationSpeed(15);

		anms->setScale(core::vector3df(2.f,2.f,2.f));
		anms->setRotation(core::vector3df(0,-90,0));
	}

	ThirdPersonCam myCam;
	myCam.Init(smgr);
	myCam.SetTargetNode(anms);


	while(device->run())
	{
		driver->beginScene(true, true, video::SColor(255,113,113,133));
		
		if ( receiver.IsKeyDown(KEY_LEFT) )
			myCam.RotateHorizontal(-.1);
		else if ( receiver.IsKeyDown(KEY_RIGHT) )			
			myCam.RotateHorizontal(.1);
		else 
			myCam.RotateHorizontal(.0);
		if ( receiver.IsKeyDown(KEY_UP) )
			myCam.RotateVertical(-.1);
		else if ( receiver.IsKeyDown(KEY_DOWN) )
			myCam.RotateVertical(.1);
		else 
			myCam.RotateVertical(.0);
		if ( receiver.IsKeyDown(KEY_ADD) )
			myCam.ZoomIn();
		else if ( receiver.IsKeyDown(KEY_SUBTRACT) )
			myCam.ZoomOut();
		myCam.Update();
		
		UpdatePicking(myCam.GetIrrCamNode(), smgr->getSceneCollisionManager(), device->getCursorControl());

		
		smgr->drawAll(); // draw the 3d scene
		device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)

		driver->endScene();
		
		device->sleep(10);
	}

	device->drop();
	
	return 0;
}
