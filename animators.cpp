// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Experimenting with animators

#include <irrlicht.h>
#include <iostream>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class CApp : public IEventReceiver
{
    friend int main(int argc, char *argv[]);

public:
    CApp()
	: Device(0)
	, Camera(0)
	, MeshSceneNode(0)
	, CheckBoxUsePauses(0)
	, ButtonEnableCircle(0)
	, ButtonEnableStraight(0)
	, ButtonEnableSpline(0)
	, ButtonEnableRotate(0)
	, ButtonEnableTextures(0)
	, AnimatorCircle(0)
	, AnimatorStraight(0)
	, AnimatorSpline(0)
	, AnimatorRotate(0)
	, AnimatorTextures(0)
	{
	}

	virtual bool OnEvent(const SEvent &event)
	{
		if ( event.EventType == EET_GUI_EVENT )
		{
			if ( event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED  )
			{
				u32 currentTime = Device->getTimer()->getTime();

				if ( event.GUIEvent.Caller == ButtonEnableCircle )
				{
					AnimatorCircle->setEnabled( ButtonEnableCircle->isPressed(), currentTime );
				}
				else if ( event.GUIEvent.Caller == ButtonEnableStraight )
				{
					AnimatorStraight->setEnabled( ButtonEnableStraight->isPressed(), currentTime );
				}
				else if ( event.GUIEvent.Caller == ButtonEnableSpline )
				{
					AnimatorSpline->setEnabled( ButtonEnableSpline->isPressed(), currentTime );
				}
				else if ( event.GUIEvent.Caller == ButtonEnableRotate )
				{
					AnimatorRotate->setEnabled( ButtonEnableRotate->isPressed(), currentTime );
				}
				else if ( event.GUIEvent.Caller == ButtonEnableTextures )
				{
					AnimatorTextures->setEnabled( ButtonEnableTextures->isPressed(), currentTime );
				}
			}
		}
		return false;
	}

protected:

    bool init(int argc, char *argv[])
	{
		Device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(800,600));
		if (!Device)
			return false;

		Device->setEventReceiver(this);
		scene::ISceneManager* smgr = Device->getSceneManager();
		gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();

		ButtonEnableCircle = guiEnv->addButton(core::rect<s32>(10, 20, 100, 40), 0, -1, L"circle");
		ButtonEnableCircle->setIsPushButton(true);
		ButtonEnableStraight = guiEnv->addButton(core::rect<s32>(10, 50, 100, 70), 0, -1, L"straight");
		ButtonEnableStraight->setIsPushButton(true);
		ButtonEnableSpline = guiEnv->addButton(core::rect<s32>(10, 80, 100, 100), 0, -1, L"spline");
		ButtonEnableSpline->setIsPushButton(true);
		ButtonEnableRotate = guiEnv->addButton(core::rect<s32>(10, 110, 100, 130), 0, -1, L"rotate");
		ButtonEnableRotate->setIsPushButton(true);
		ButtonEnableTextures = guiEnv->addButton(core::rect<s32>(10, 140, 100, 160), 0, -1, L"textures");
		ButtonEnableTextures->setIsPushButton(true);

		core::vector3df camPos = core::vector3df(0, 150, -300);
		core::vector3df camLookAt = core::vector3df(0, 0, 0);
		Camera = smgr->addCameraSceneNode (0, camPos, camLookAt, -1);

		MeshSceneNode = smgr->addCubeSceneNode (10.0f, 0, -1,
		                                   core::vector3df(0, 0, 0),		// position
		                                   core::vector3df(0, 0, 0),			// rotation
		                                   core::vector3df(3.0f, 3.0f, 3.0f));	// scale
		addAnimators(MeshSceneNode);

		/*scene::ILightSceneNode* nodeLight = */smgr->addLightSceneNode(0, core::vector3df(0, 0, 0),
														video::SColorf(1.0f, 1.0f, 1.0f),
														300.0f);	// radius



		return true;
	}

    void run()
	{
		if ( !Device )
			return;

		// main application loop
		while(Device->run())
		{
			video::IVideoDriver* videoDriver =  Device->getVideoDriver();
			if ( Device->isWindowActive() )
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


			Device->sleep( 5 );
		}
	}

    void quit()
	{
		if ( AnimatorCircle )
		{
			AnimatorCircle->drop();
			AnimatorCircle = 0;
		}
		if ( AnimatorStraight )
		{
			AnimatorStraight->drop();
			AnimatorStraight = 0;
		}
		if ( AnimatorSpline )
		{
			AnimatorSpline->drop();
			AnimatorSpline = 0;
		}
		if ( AnimatorRotate )
		{
			AnimatorRotate->drop();
			AnimatorRotate = 0;
		}
		if ( AnimatorTextures )
		{
			AnimatorTextures->drop();
			AnimatorTextures = 0;
		}

		if ( Device )
		{
			Device->closeDevice();
			Device->drop();
			Device = NULL;
		}
	}

	void addAnimators(scene::IMeshSceneNode* node)
	{
		scene::ISceneManager * smgr = Device->getSceneManager();
		video::IVideoDriver* driver =  Device->getVideoDriver();

		core::vector3df circleCenter=core::vector3df(0.f, 0.f, 0.f);
		f32 circleRadius=100.f;
		f32 circleSpeed=0.001f;
		core::vector3df circleDirection=core::vector3df(0.f, 1.f, 0.f);
		f32 circleStartPosition=0.f;
		f32 circleRadiusEllipsoid=0.f;
		AnimatorCircle = smgr->createFlyCircleAnimator(circleCenter, circleRadius, circleSpeed, circleDirection, circleStartPosition, circleRadiusEllipsoid);
		AnimatorCircle->setEnabled(false);
		node->addAnimator(AnimatorCircle);

		core::vector3df straightStartPoint = core::vector3df(0.f, 0.f, 0.f);
		core::vector3df straightEndPoint  = core::vector3df(100.f, 100.f, 100.f);
		u32 straightTimeForWay = 3000;
		bool straightLoop=true;
		bool straightPingpong=true;
		AnimatorStraight = smgr->createFlyStraightAnimator(straightStartPoint, straightEndPoint, straightTimeForWay, straightLoop, straightPingpong);
		AnimatorStraight->setEnabled(false);
		node->addAnimator(AnimatorStraight);

		u32 currentTime = Device->getTimer ()->getTime();
		core::array< core::vector3df > points;
		points.push_back( MeshSceneNode->getPosition() );
		points.push_back( points[0] + core::vector3df(150, 0,0) );
		points.push_back( points[0] + core::vector3df(150, 150, 0) );
		points.push_back( points[0] + core::vector3df(0, 0, 150) );
		f32 speed = 1.0f;
		f32 tightness = 0.5f;
		bool loop=true;
		bool pingpong=true;
		AnimatorSpline = smgr->createFollowSplineAnimator(currentTime, points, speed, tightness, loop, pingpong);
		AnimatorSpline->setEnabled(false);
		node->addAnimator(AnimatorSpline);

		core::vector3df rotationSpeed = core::vector3df( 0.1f, 0.1f, 0.1f);
		AnimatorRotate = smgr->createRotationAnimator(rotationSpeed);
		AnimatorRotate->setEnabled(false);
		node->addAnimator(AnimatorRotate);

		core::array< video::ITexture * > textures;
		textures.push_back( driver->getTexture("my_media/red.jpg") );
		textures.push_back( driver->getTexture("my_media/green.jpg") );
		textures.push_back( driver->getTexture("my_media/blue.jpg") );
		s32 texTimePerFrame = 1000;
		bool texLoop=true;
		AnimatorTextures = smgr->createTextureAnimator (textures, texTimePerFrame, texLoop);
		AnimatorTextures->setEnabled(false);
		node->addAnimator(AnimatorTextures);
	}

private:
	IrrlichtDevice * 			Device;
	scene::ICameraSceneNode *	Camera;
	scene::IMeshSceneNode*	 	MeshSceneNode;
	gui::IGUICheckBox * CheckBoxUsePauses;
	gui::IGUIButton * ButtonEnableCircle;
	gui::IGUIButton * ButtonEnableStraight;
	gui::IGUIButton * ButtonEnableSpline;
	gui::IGUIButton * ButtonEnableRotate;
	gui::IGUIButton * ButtonEnableTextures;

	scene::ISceneNodeAnimator * AnimatorCircle;
	scene::ISceneNodeAnimator * AnimatorStraight;
	scene::ISceneNodeAnimator * AnimatorSpline;
	scene::ISceneNodeAnimator * AnimatorRotate;
	scene::ISceneNodeAnimator * AnimatorTextures;
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
