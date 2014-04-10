// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Experimenting with particles
// Based on my animator.cpp so I can test particles better when the node moves around.

#include <irrlicht.h>
#include <iostream>
#include <vector>
#include "irr_ptr.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

struct Emitter
{
	Emitter() {}
	Emitter(const core::stringw& name, IrrPtr<scene::IParticleEmitter> emitter)
	: Name(name), IrrEmitter(emitter)
	{
	}

	core::stringw Name;
	IrrPtr<scene::IParticleEmitter> IrrEmitter;
};

struct Affector
{
	Affector() {}
	Affector(const core::stringw& name, IrrPtr<scene::IParticleAffector> affector)
	: Name(name), IrrAffector(affector)
	{
	}

	core::stringw Name;
	IrrPtr<scene::IParticleAffector> IrrAffector;
};

struct GuiAttribute
{
	void remove()
	{
		if ( StaticText )
			StaticText->remove();
		if ( EditBox )
			EditBox->remove();
	}

	IrrPtr<gui::IGUIStaticText> StaticText;
	IrrPtr<gui::IGUIEditBox> EditBox;
};

class CApp : public IEventReceiver
{
    friend int main(int argc, char *argv[]);

public:
    CApp()
	: Device(0)
	, Camera(0)
	, MeshSceneNode(0)
	, ParticleSceneNode(0)
	, CheckBoxEnableCircle(0)
	, CheckBoxEnableStraight(0)
	, CheckBoxEnableSpline(0)
	, CheckBoxEnableRotate(0)
	, ListBoxEmitters(0)
	, CheckBoxParticlesVisibility(0)
	, ButtonClearParticles(0)
	, CheckBoxGlobalParticles(0)
	, SpinBoxSleep(0)
	, CheckBoxInvisibleEmitting(0)
	, CheckBoxInvisibleAffecting(0)
	, CheckBoxInvisibleAnimating(0)
	, CheckBoxClearOnInvisible(0)
	, CheckBoxEmitterIgnoreRotation(0)
	, CheckBoxEmitterFrameInterpolation(0)
	, ButtonUpdateAttributes(0)
	, ActiveAttributes(0)
	, ActiveAttributeObject(0)
	{
	}

	virtual bool OnEvent(const SEvent &event)
	{
		if ( event.EventType == EET_GUI_EVENT )
		{
			if ( event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED  )
			{
				if ( event.GUIEvent.Caller == ButtonClearParticles )
				{
					ParticleSceneNode->clearParticles();
				}
				else if ( event.GUIEvent.Caller == ButtonUpdateAttributes )
				{
					updateAttributeObject();
				}
			}
			else if ( event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
			{
				u32 currentTime = Device->getTimer()->getTime();
				if ( event.GUIEvent.Caller == CheckBoxEnableCircle )
				{
					AnimatorCircle->setEnabled( CheckBoxEnableCircle->isChecked(), currentTime );
					showAttributeValues(AnimatorCircle.get());
				}
				else if ( event.GUIEvent.Caller == CheckBoxEnableStraight )
				{
					AnimatorStraight->setEnabled( CheckBoxEnableStraight->isChecked(), currentTime );
					showAttributeValues(AnimatorStraight.get());
				}
				else if ( event.GUIEvent.Caller == CheckBoxEnableSpline )
				{
					AnimatorSpline->setEnabled( CheckBoxEnableSpline->isChecked(), currentTime );
					showAttributeValues(AnimatorSpline.get());
				}
				else if ( event.GUIEvent.Caller == CheckBoxEnableRotate )
				{
					AnimatorRotate->setEnabled( CheckBoxEnableRotate->isChecked(), currentTime );
					showAttributeValues(AnimatorRotate.get());
				}
				else if ( event.GUIEvent.Caller == CheckBoxParticlesVisibility )
				{
					ParticleSceneNode->setVisible( CheckBoxParticlesVisibility->isChecked() );
				}
				else if ( event.GUIEvent.Caller == CheckBoxGlobalParticles )
				{
					ParticleSceneNode->setParticlesAreGlobal( CheckBoxGlobalParticles->isChecked() );
				}
				else if ( event.GUIEvent.Caller == CheckBoxInvisibleEmitting
						|| event.GUIEvent.Caller == CheckBoxInvisibleAffecting
						|| event.GUIEvent.Caller == CheckBoxInvisibleAnimating
						|| event.GUIEvent.Caller == CheckBoxClearOnInvisible
						|| event.GUIEvent.Caller == CheckBoxEmitterIgnoreRotation
						|| event.GUIEvent.Caller == CheckBoxEmitterFrameInterpolation )
				{
					u32 behavior = 0;
					if ( CheckBoxInvisibleEmitting->isChecked()  )
						behavior |= scene::EPB_INVISIBLE_EMITTING;
					if ( CheckBoxInvisibleAffecting->isChecked()  )
						behavior |= scene::EPB_INVISIBLE_AFFECTING;
					if ( CheckBoxInvisibleAnimating->isChecked()  )
						behavior |= scene::EPB_INVISIBLE_ANIMATING;
					if ( CheckBoxClearOnInvisible->isChecked()  )
						behavior |= scene::EPB_CLEAR_ON_INVISIBLE;
					if ( CheckBoxEmitterIgnoreRotation->isChecked()  )
						behavior |= scene::EPB_EMITTER_VECTOR_IGNORE_ROTATION;
					if ( CheckBoxEmitterFrameInterpolation->isChecked()  )
						behavior |= scene::EPB_EMITTER_FRAME_INTERPOLATION;
					ParticleSceneNode->setParticleBehavior(behavior);
				}
				else
				{
					for ( size_t i=0; i < AffectorCheckBoxes.size(); ++i )
					{
						if ( event.GUIEvent.Caller == AffectorCheckBoxes[i] )
						{
							updateAffectors(ParticleSceneNode);
							showAttributeValues(Affectors[i].IrrAffector.get());
							break;
						}
					}
				}
			}
			else if ( event.GUIEvent.EventType == gui::EGET_LISTBOX_CHANGED ||
			          event.GUIEvent.EventType == gui::EGET_LISTBOX_SELECTED_AGAIN )
			{
				if ( event.GUIEvent.Caller == ListBoxEmitters )
				{
					s32 selected = ListBoxEmitters->getSelected();
					if ( selected >= 0 )
					{
						ParticleSceneNode->setEmitter( Emitters[selected].IrrEmitter.get() );
						showAttributeValues(Emitters[selected].IrrEmitter.get());
					}
					else
					{
						ParticleSceneNode->setEmitter(NULL);
					}
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

		video::IVideoDriver* driver =  Device->getVideoDriver();
		Device->setEventReceiver(this);
		scene::ISceneManager* smgr = Device->getSceneManager();
		io::IFileSystem * fs = Device->getFileSystem();

		ActiveAttributes = fs->createEmptyAttributes();

		core::vector3df camLookAt = core::vector3df(0, 0, 0);
		f32 camRotateSpeed = -1500.f;
		f32 camZoomSpeed = 200.f;
		f32 camTranslationSpeed = 1500.f;
		f32 camDistance = 300.f;
		Camera = smgr->addCameraSceneNodeMaya(0, camRotateSpeed, camZoomSpeed, camTranslationSpeed, -1, camDistance);
		Camera->updateAbsolutePosition();
		Camera->setTarget(camLookAt);

		// add a skybox to get a better sense of current camera rotation.
		smgr->addSkyBoxSceneNode(
				driver->getTexture("../../media/irrlicht2_up.jpg"),
				driver->getTexture("../../media/irrlicht2_dn.jpg"),
				driver->getTexture("../../media/irrlicht2_lf.jpg"),
				driver->getTexture("../../media/irrlicht2_rt.jpg"),
				driver->getTexture("../../media/irrlicht2_ft.jpg"),
				driver->getTexture("../../media/irrlicht2_bk.jpg"));


		MeshSceneNode = smgr->addCubeSceneNode (5.0f, 0, -1,
		                                   core::vector3df(0, 0, 0),		// position
		                                   core::vector3df(0, 0, 0),			// rotation
		                                   core::vector3df(0.33f, 2.f, 1.f));	// scale
		createAnimators(MeshSceneNode);

		bool withDefaultEmitter=false;
		core::vector3df particleNodePosition=core::vector3df(0, 10, 0);	// relative to cube
		core::vector3df particleNodeRotation=core::vector3df(0, 0, 0);
		core::vector3df particleNodeScale=core::vector3df(1.0f, 1.0f, 1.0f);
		ParticleSceneNode = smgr->addParticleSystemSceneNode(withDefaultEmitter, MeshSceneNode, -1, particleNodePosition, particleNodeRotation, particleNodeScale);
		createEmitters(ParticleSceneNode);
		createAffectors(ParticleSceneNode);
		// too lazy to create a gui for this - just change and re-compile for testing.
		ParticleSceneNode->setParticleBehavior(0);

		// light + billboard to show it's position
		scene::ISceneNode * lightNode = smgr->addLightSceneNode(0, core::vector3df(0, 0, 0),
		                                                         video::SColorf(1.0f, 1.0f, 1.0f),
															     300.0f); // radius
		scene::ISceneNode * lightBillboard = smgr->addBillboardSceneNode(lightNode, core::dimension2d<f32>(30, 30));
		lightBillboard->setMaterialFlag(video::EMF_LIGHTING, false);
		lightBillboard->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		lightBillboard->setMaterialTexture(0, driver->getTexture("../../media/particlewhite.bmp"));

		// GUI
		gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();
		guiEnv->getSkin()->setColor(gui::EGDC_3D_FACE, video::SColor(20,110,210,210));
		guiEnv->getSkin()->setColor(gui::EGDC_3D_DARK_SHADOW, video::SColor(20,0,50,50));

		CheckBoxEnableCircle = guiEnv->addCheckBox(false, core::rect<s32>(10, 20, 100, 40), 0, -1, L"circle");
		CheckBoxEnableStraight = guiEnv->addCheckBox(false, core::rect<s32>(10, 50, 100, 70), 0, -1, L"straight");
		CheckBoxEnableSpline = guiEnv->addCheckBox(false, core::rect<s32>(10, 80, 100, 100), 0, -1, L"spline");
		CheckBoxEnableRotate = guiEnv->addCheckBox(false, core::rect<s32>(10, 110, 100, 130), 0, -1, L"rotate");

		ListBoxEmitters = guiEnv->addListBox(core::rect<s32>(110, 20, 200, 130), 0, -1, true);
		for ( size_t i=0; i<Emitters.size(); ++i )
		{
			ListBoxEmitters->addItem(Emitters[i].Name.c_str());
		}

		s32 top = 20;
		for ( size_t i=0; i<Affectors.size(); ++i )
		{
			AffectorCheckBoxes.push_back( guiEnv->addCheckBox(false, core::rect<s32>(210, top, 300, top+20), 0, -1, Affectors[i].Name.c_str()) );
			top += 30;
		}
		ButtonUpdateAttributes = guiEnv->addButton(core::rect<s32>(60, 140, 120, 160), 0, -1, L"Set attributes");

		CheckBoxParticlesVisibility = guiEnv->addCheckBox(true, core::rect<s32>(310, 20, 400, 40), 0, -1, L"Visible");

		ButtonClearParticles = guiEnv->addButton(core::rect<s32>(310, 50, 400, 70), 0, -1, L"Clear particles");

		CheckBoxGlobalParticles = guiEnv->addCheckBox(true, core::rect<s32>(310, 80, 400, 100), 0, -1, L"Global particles");

		guiEnv->addStaticText(L"sleep (reduce FPS)", core::rect<s32>(310,110,400,130));
		SpinBoxSleep = guiEnv->addSpinBox(L"10", core::rect<s32>(310,130,400,150));
		SpinBoxSleep->setRange(0.f, 10000.f);
		SpinBoxSleep->setStepSize(10.f);
		SpinBoxSleep->setDecimalPlaces(0);

		CheckBoxInvisibleEmitting = guiEnv->addCheckBox(false, core::rect<s32>(410, 20, 500, 40), 0, -1, L"invisible emitting");
		CheckBoxInvisibleAffecting = guiEnv->addCheckBox(false, core::rect<s32>(410, 50, 500, 70), 0, -1, L"invisible affecting");
		CheckBoxInvisibleAnimating = guiEnv->addCheckBox(false, core::rect<s32>(410, 80, 500, 100), 0, -1, L"invisible animating");
		CheckBoxClearOnInvisible = guiEnv->addCheckBox(false, core::rect<s32>(410, 110, 500, 130), 0, -1, L"clear on invisible");
		CheckBoxEmitterIgnoreRotation = guiEnv->addCheckBox(false, core::rect<s32>(410, 140, 500, 160), 0, -1, L"emitter ignore rotation");
		CheckBoxEmitterFrameInterpolation = guiEnv->addCheckBox(false, core::rect<s32>(410, 170, 500, 190), 0, -1, L"frame interpolation");

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

			Device->sleep( (u32)SpinBoxSleep->getValue() );
		}
	}

    void quit()
	{
		if ( ActiveAttributes )
		{
			ActiveAttributes->drop();
			ActiveAttributes = NULL;
		}
		dropAnimators();
		dropAffectors();
		dropEmitters();

		if ( Device )
		{
			Device->closeDevice();
			Device->drop();
			Device = NULL;
		}
	}

	void createAnimators(scene::IMeshSceneNode* node)
	{
		scene::ISceneManager * smgr = Device->getSceneManager();
		video::IVideoDriver* driver =  Device->getVideoDriver();

		core::vector3df circleCenter=core::vector3df(0.f, 0.f, 0.f);
		f32 circleRadius=100.f;
		f32 circleSpeed=0.001f;
		core::vector3df circleDirection=core::vector3df(0.f, 1.f, 0.f);
		f32 circleStartPosition=0.f;
		f32 circleRadiusEllipsoid=0.f;
		AnimatorCircle.set( smgr->createFlyCircleAnimator(circleCenter, circleRadius, circleSpeed, circleDirection, circleStartPosition, circleRadiusEllipsoid), false);
		AnimatorCircle->setEnabled(false);
		node->addAnimator(AnimatorCircle.get());

		core::vector3df straightStartPoint = core::vector3df(0.f, 0.f, 0.f);
		core::vector3df straightEndPoint  = core::vector3df(100.f, 100.f, 100.f);
		u32 straightTimeForWay = 3000;
		bool straightLoop=true;
		bool straightPingpong=true;
		AnimatorStraight.set( smgr->createFlyStraightAnimator(straightStartPoint, straightEndPoint, straightTimeForWay, straightLoop, straightPingpong), false);
		AnimatorStraight->setEnabled(false);
		node->addAnimator(AnimatorStraight.get());

		u32 currentTime = Device->getTimer ()->getTime();
		core::array< core::vector3df > points;
		points.push_back( core::vector3df(0, 0, 0) );
		points.push_back( points[0] + core::vector3df(150, 0, 0) );
		points.push_back( points[0] + core::vector3df(150, 150, 0) );
		points.push_back( points[0] + core::vector3df(0, 0, 150) );
		f32 speed = 1.0f;
		f32 tightness = 0.5f;
		bool loop=true;
		bool pingpong=true;
		AnimatorSpline.set( smgr->createFollowSplineAnimator(currentTime, points, speed, tightness, loop, pingpong), false);
		AnimatorSpline->setEnabled(false);
		node->addAnimator(AnimatorSpline.get());

		core::vector3df rotationSpeed = core::vector3df( 0.1f, 0.1f, 0.1f);
		AnimatorRotate.set(smgr->createRotationAnimator(rotationSpeed), false);
		AnimatorRotate->setEnabled(false);
		node->addAnimator(AnimatorRotate.get());
	}

	void dropAnimators()
	{
		AnimatorCircle.reset();
		AnimatorStraight.reset();
		AnimatorSpline.reset();
		AnimatorRotate.reset();
	}

	void createEmitters(scene::IParticleSystemSceneNode * particleNode)
	{
		IrrPtr<scene::IParticleEmitter> emitter;

		bool useNormalDirection=true;	// using normals of the vertex it's emitted from in mesh-emitters
		core::vector3df direction(0.0f, 0.03f, 0.0f);
		f32 normalDirectionModifier=100.0f;	// when useNormalDirection the normals are divided by this factor
		bool everyMeshVertex=false; // when true it will emit min-max particles for every vertex in the mesh each second
		s32 meshBufferNumber=-1;	// -1 for all, otherwise use only a specific meshbuffer
		u32 minParticlesPerSecond=5;
		u32 maxParticlesPerSecond=10;
		video::SColor minStartColor(255, 0, 0, 0);
		video::SColor maxStartColor(255, 255, 255, 255);
		u32 lifeTimeMin=2000;
		u32 lifeTimeMax=4000;
		s32 maxAngleDegrees=0;	// max difference to original vector
		core::dimension2df minStartSize(5.0f, 5.0f);
		core::dimension2df maxStartSize(5.0f, 5.0f);
		core::vector3df center(0,0,0);
		f32 radius = 50.f;

//		scene::IAnimatedMeshSceneNode * aniNode = 0;
//		emitter.set( particleNode->createAnimatedMeshSceneNodeEmitter (aniNode, useNormalDirection, direction, normalDirectionModifier, meshBufferNumber, everyMeshVertex, minParticlesPerSecond, maxParticlesPerSecond, minStartColor, maxStartColor, lifeTimeMin, lifeTimeMax, maxAngleDegrees, minStartSize, maxStartSize), false);
//		Emitters.push_back(Emitter(L"animesh", emitter));

		core::aabbox3df box=core::aabbox3df(-10, 28,-10, 10, 30, 10);
		emitter.set( particleNode->createBoxEmitter (box, direction, minParticlesPerSecond, maxParticlesPerSecond, minStartColor, maxStartColor, lifeTimeMin, lifeTimeMax, maxAngleDegrees, minStartSize, maxStartSize), false);
		Emitters.push_back(Emitter(L"box", emitter));

		core::vector3df cylinderNormal(0.f, 1.f, 0.f);
		f32 cylinderLength = 10.f;
		bool cylinderOutlineOnly=false;
		emitter.set( particleNode->createCylinderEmitter (center, radius, cylinderNormal, cylinderLength, cylinderOutlineOnly, direction, minParticlesPerSecond, maxParticlesPerSecond, minStartColor, maxStartColor, lifeTimeMin, lifeTimeMax, maxAngleDegrees, minStartSize, maxStartSize), false);
		Emitters.push_back(Emitter(L"cylinder", emitter));

//		scene::IMesh * mesh = 0;
//		emitter.set( particleNode->createMeshEmitter (mesh, useNormalDirection, direction, normalDirectionModifier, meshBufferNumber, everyMeshVertex, minParticlesPerSecond, maxParticlesPerSecond, minStartColor, maxStartColor, lifeTimeMin, lifeTimeMax, maxAngleDegrees, minStartSize, maxStartSize), false);
//		Emitters.push_back(Emitter(L"mesh", emitter));

		emitter.set( particleNode->createPointEmitter (direction, minParticlesPerSecond, maxParticlesPerSecond, minStartColor, maxStartColor, lifeTimeMin, lifeTimeMax, maxAngleDegrees, minStartSize, maxStartSize), false);
		Emitters.push_back(Emitter(L"point", emitter));

		f32 ringThickness = 5.f;
		emitter.set( particleNode->createRingEmitter (center, radius, ringThickness, direction, minParticlesPerSecond, maxParticlesPerSecond, minStartColor, maxStartColor, lifeTimeMin, lifeTimeMax, maxAngleDegrees, minStartSize, maxStartSize), false);
		Emitters.push_back(Emitter(L"ring", emitter));

		emitter.set( particleNode->createSphereEmitter (center, radius, direction, minParticlesPerSecond, maxParticlesPerSecond, minStartColor, maxStartColor, lifeTimeMin, lifeTimeMax, maxAngleDegrees, minStartSize, maxStartSize), false);
		Emitters.push_back(Emitter(L"sphere", emitter));
	}

	void dropEmitters()
	{
		Emitters.clear();
	}

	void createAffectors(scene::IParticleSystemSceneNode * particleNode)
	{
		IrrPtr<scene::IParticleAffector> affector;

		core::vector3df attractPoint(0,50,0);
		f32 attractSpeed=1.0f;
		bool attract=true;
		bool affectX=true;
		bool affectY=true;
		bool affectZ=true;
		affector.set( particleNode->createAttractionAffector (attractPoint, attractSpeed, attract, affectX, affectY, affectZ), false);
		Affectors.push_back( Affector(L"attraction", affector) );

		video::SColor targetColor(0, 0, 0, 0);
		u32 timeNeededToFadeOut=1000;
		affector.set( particleNode->createFadeOutParticleAffector (targetColor, timeNeededToFadeOut), false);
		Affectors.push_back( Affector(L"fade out", affector) );

		core::vector3df gravity(0.0f,-0.03f, 0.0f);
		u32 timeForceLost=1000;
		affector.set( particleNode->createGravityAffector (gravity, timeForceLost), false);
		Affectors.push_back( Affector(L"gravity", affector) );

		core::vector3df rotationSpeed(5.0f, 5.0f, 5.0f);
		core::vector3df pivotPoint(0.0f, 0.0f, 0.0f);
		affector.set( particleNode->createRotationAffector (rotationSpeed, pivotPoint), false);
		Affectors.push_back( Affector(L"rotation", affector) );

		core::dimension2df scaleTo(3.0f, 3.0f);
		affector.set( particleNode->createScaleParticleAffector (scaleTo), false);
		Affectors.push_back( Affector(L"scale", affector) );
	}

	void updateAffectors(scene::IParticleSystemSceneNode * particleNode)
	{
		particleNode->removeAllAffectors();
		for ( size_t i=0; i < AffectorCheckBoxes.size(); ++i )
		{
			if ( AffectorCheckBoxes[i]->isChecked() )
				ParticleSceneNode->addAffector( Affectors[i].IrrAffector.get() );
		}
	}

	void dropAffectors()
	{
		Affectors.clear();
	}

	void showAttributeValues(io::IAttributeExchangingObject * attribObj)
	{
		ActiveAttributeObject = attribObj;
		if ( !ActiveAttributeObject || !ActiveAttributes )
			return;

		for ( size_t i=0; i < GuiAttributes.size(); ++i )
			GuiAttributes[i].remove();
		GuiAttributes.clear();
		ActiveAttributes->clear();

		ActiveAttributeObject->serializeAttributes(ActiveAttributes);

		gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();
		s32 left = 10;
		s32 top = 170;
		for ( s32 i=0; i< (s32)ActiveAttributes->getAttributeCount(); ++i )
		{
			GuiAttribute guiAttribute;
			core::stringw name(ActiveAttributes->getAttributeName(i));
			core::stringw value(ActiveAttributes->getAttributeAsString(i));

			guiAttribute.StaticText.set( guiEnv->addStaticText( name.c_str(), core::rect<s32> (left, top, left+60, top+20), true, false ), true);
			guiAttribute.EditBox.set( guiEnv->addEditBox( value.c_str(), core::rect<s32>(left+65, top, left+200, top+20) ), true);
			top += 25;

			GuiAttributes.push_back(guiAttribute);
		}
	}

	void updateAttributeObject()
	{
		if ( !ActiveAttributeObject || !ActiveAttributes )
			return;

		for ( s32 i=0; i< (s32)ActiveAttributes->getAttributeCount(); ++i )
		{
			core::stringc value( GuiAttributes[i].EditBox->getText() );
			ActiveAttributes->setAttribute(i, value.c_str() );
		}

		ActiveAttributeObject->deserializeAttributes(ActiveAttributes);
	}


private:
	IrrlichtDevice * Device;
	scene::ICameraSceneNode * Camera;
	scene::IMeshSceneNode* MeshSceneNode;
	scene::IParticleSystemSceneNode * ParticleSceneNode;

	gui::IGUICheckBox * CheckBoxEnableCircle;
	gui::IGUICheckBox * CheckBoxEnableStraight;
	gui::IGUICheckBox * CheckBoxEnableSpline;
	gui::IGUICheckBox * CheckBoxEnableRotate;
	gui::IGUIListBox * ListBoxEmitters;
	std::vector<gui::IGUICheckBox*> AffectorCheckBoxes;
	gui::IGUICheckBox * CheckBoxParticlesVisibility;
	gui::IGUIButton * ButtonClearParticles;
	gui::IGUICheckBox * CheckBoxGlobalParticles;
	gui::IGUIButton * ButtonUpdateAttributes;
	gui::IGUISpinBox * SpinBoxSleep;

	gui::IGUICheckBox * CheckBoxInvisibleEmitting;
	gui::IGUICheckBox * CheckBoxInvisibleAffecting;
	gui::IGUICheckBox * CheckBoxInvisibleAnimating;
	gui::IGUICheckBox * CheckBoxClearOnInvisible;
	gui::IGUICheckBox * CheckBoxEmitterIgnoreRotation;
	gui::IGUICheckBox * CheckBoxEmitterFrameInterpolation;

	IrrPtr<scene::ISceneNodeAnimator> AnimatorCircle;
	IrrPtr<scene::ISceneNodeAnimator> AnimatorStraight;
	IrrPtr<scene::ISceneNodeAnimator> AnimatorSpline;
	IrrPtr<scene::ISceneNodeAnimator> AnimatorRotate;

	std::vector<Emitter> Emitters;
	std::vector<Affector> Affectors;

	io::IAttributes* ActiveAttributes;
	io::IAttributeExchangingObject * ActiveAttributeObject;
	std::vector<GuiAttribute> GuiAttributes;
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
