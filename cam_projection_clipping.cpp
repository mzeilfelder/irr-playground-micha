// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Simple scene to test camera near/far planes

#include <irrlicht.h>
#include <cassert>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

void createScene(IVideoDriver* driver, ISceneManager* smgr)
{
	// some light
	smgr->setAmbientLight( irr::video::SColorf( 1.f, 1.f, 1.f, 1.f)  );

	scene::IAnimatedMesh* aniMeshArc = smgr->getMesh( "my_media/asymetric_arc.obj" );
	if (aniMeshArc)
	{
		scene::IMeshSceneNode * nodeArc = smgr->addMeshSceneNode (aniMeshArc, NULL, -1, vector3df(0.f, 0.f, 0.f), vector3df(45.f, 30.f, 60.f));
		nodeArc->setMaterialFlag(video::EMF_LIGHTING, false);
		nodeArc->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);

		scene::IDummyTransformationSceneNode* dummyTransformationNode = smgr->addDummyTransformationSceneNode();
		core::matrix4& dm = dummyTransformationNode->getRelativeTransformationMatrix();
		dm.setTranslation(vector3df(0.f, 0.f, 10.f));
		dm.setRotationDegrees(vector3df(45.f, 30.f, 60.f));
		core::matrix4 sm;
		sm.setScale(vector3df(2.f,3.f, 4.f));
		dm *= sm;
		scene::IMeshSceneNode * nodeArcD = smgr->addMeshSceneNode (aniMeshArc, dummyTransformationNode, -1);
		nodeArcD->setMaterialFlag(video::EMF_LIGHTING, false);

		scene::IMeshSceneNode * nodeArc2 = smgr->addMeshSceneNode (aniMeshArc, NULL, -1, vector3df(5.f, 0.f, -50.f), vector3df(45.f, 0.f, 0.f), vector3df(1.f, 1.f, 1.f) );
		nodeArc2->setMaterialFlag(video::EMF_LIGHTING, false);

		scene::IMeshSceneNode * nodeArc3 = smgr->addMeshSceneNode (aniMeshArc, NULL, -1, vector3df(-5.f, 10.f, 130.f), vector3df(0.f, 45.f, 0.f), vector3df(1.f, 1.f, 1.f) );
		nodeArc3->setMaterialFlag(video::EMF_LIGHTING, false);

		scene::IMeshSceneNode * nodeArc4 = smgr->addMeshSceneNode (aniMeshArc, NULL, -1, vector3df(0.f, -15.f, 0.f),vector3df(0.f, 0.f, 45.f), vector3df(1.f, 1.f, 1.f) );
		nodeArc4->setMaterialFlag(video::EMF_LIGHTING, false);
	}
}


irr::scene::ICameraSceneNode * addCamera(ISceneManager* smgr)
{
#if 0
	irr::scene::ICameraSceneNode * cam = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );
#else
	irr::scene::ICameraSceneNode * cam = smgr->addCameraSceneNode();
#endif
	cam->setNearValue(15.f);
	cam->setFarValue(75.f);
	cam->updateAbsolutePosition();
	cam->setPosition( irr::core::vector3df( 0, 0, 30) );
	cam->updateAbsolutePosition();
	cam->setTarget( irr::core::vector3df( 0, 0, 0) );
	cam->updateAbsolutePosition();

	return cam;
}

void printScene(ISceneNode* node, int depth)
{
	for ( int i=0; i<depth; ++i )
	{
		printf("-");
	}
	printf("%s", node->getDebugName());
	irr::core::vector3df absPos = node->getAbsoluteTransformation().getTranslation();
	irr::core::vector3df relPos = node->getRelativeTransformation().getTranslation();
	printf(" abs: %.1f %.1f %.1f rel: %.1f %.1f %.1f\n", absPos.X, absPos.Y, absPos.Z, relPos.X, relPos.Y, relPos.Z );

	const core::list<ISceneNode*>& children = node->getChildren();
	for ( core::list<ISceneNode*>::ConstIterator it = children.begin(); it != children.end(); ++it )
	{
		printScene(*it, depth+1);
	}
}

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver() 
		: KeepRunning(true), Device(0), Camera(0), ButtonCreateScene(0), ButtonClearScene(0), ButtonResetMats(0), ButtonQuit(0)
	{ }

	void ResetMaterials()
	{
		ISceneManager * smgr = Device->getSceneManager();
		core::array<scene::ISceneNode*> nodes;
		smgr->getSceneNodesFromType(ESNT_ANY, nodes);
		for ( u32 i=0; i< nodes.size(); ++i)
		{
			ISceneNode* node = nodes[i];
			for ( u32 m=0; m < node->getMaterialCount(); ++m)
			{
				SMaterial& material = node->getMaterial(m);
				material = SMaterial();
				material.Lighting = false;
			}
		}
	}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT
			&& event.GUIEvent.EventType == EGET_BUTTON_CLICKED	)
		{
			if ( event.GUIEvent.Caller == ButtonClearScene )
			{
				Device->getSceneManager()->clear();
			}
			else if ( event.GUIEvent.Caller == ButtonCreateScene )
			{
				ISceneManager * smgr = Device->getSceneManager();
				smgr->clear();
				createScene(Device->getVideoDriver(), smgr);
				printScene(smgr->getRootSceneNode(), 0);
				Camera = addCamera(smgr);
			}
			else  if ( event.GUIEvent.Caller == ButtonResetMats )
			{
				ResetMaterials();
			}
			else if ( event.GUIEvent.Caller == ButtonQuit )
			{
				KeepRunning = false;
			}
		}
		else if ( event.EventType == EET_KEY_INPUT_EVENT
			&&	event.KeyInput.PressedDown == false )
		{
			switch ( event.KeyInput.Key )
			{
				case irr::KEY_KEY_M:
					ResetMaterials();
				break;
				case irr::KEY_SPACE:
					if ( Camera->isOrthogonal() )
						Camera->setNearValue( Camera->getNearValue() ); // enforces recalculation - which is always perspective matrix.
					else
					{
						video::E_DRIVER_TYPE driverType = Device->getVideoDriver()->getDriverType();
						irr::core::matrix4 m;
						m.buildProjectionMatrixOrthoLH( 50, 30, Camera->getNearValue(), Camera->getFarValue(), driverType == video::EDT_OPENGL ? false : true);
						Camera->setProjectionMatrix(m, true);
					}

					break;
				default:
				break;
			}
		}

		return false;
	}

	bool KeepRunning;
	IrrlichtDevice * Device;
	irr::scene::ICameraSceneNode * Camera;
	IGUIButton * ButtonCreateScene;
	IGUIButton * ButtonClearScene;
	IGUIButton * ButtonResetMats;
	IGUIButton * ButtonQuit;
};


int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	//video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;
	IrrlichtDevice *device = createDevice( driverType, dimension2d<u32>(1280, 752), 32, false, false, false, false);

	if (!device)
		return 1;

	device->getLogger()->setLogLevel(irr::ELL_DEBUG);	// useful for testing format import/exports
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	MyEventReceiver eventReceiver;
	eventReceiver.Device = device;
	eventReceiver.ButtonCreateScene = guienv->addButton(recti(20, 80, 100, 100), 0, -1, L"Create scene" );
	eventReceiver.ButtonClearScene = guienv->addButton(recti(20, 110, 100, 130), 0, -1, L"Clear scene" );
	eventReceiver.ButtonResetMats = guienv->addButton(recti(20, 140, 100, 160), 0, -1, L"Reset materials (m)" );
	eventReceiver.ButtonQuit = guienv->addButton(recti(20, 170, 100, 190), 0, -1, L"Quit" );

	device->setEventReceiver(&eventReceiver);

	createScene(driver, smgr);
	eventReceiver.Camera = addCamera(smgr);

	while ( device->run() && eventReceiver.KeepRunning )
	{
		if ( device->isWindowActive() )
		{
			driver->beginScene(true, true, SColor(255, 50, 50, 50));
			smgr->drawAll();
			guienv->drawAll();

			SMaterial matLines;
			matLines.Lighting = false;
			driver->setMaterial(matLines);
			driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
			driver->draw3DLine(vector3df(-10000,0,0), vector3df(10000,0,0), SColor(255, 255, 0, 0));
			driver->draw3DLine(vector3df(0,-10000,0), vector3df(0,10000,0), SColor(255, 0, 255, 0));
			driver->draw3DLine(vector3df(0,0,-10000), vector3df(0,0,10000), SColor(255, 0, 0, 255));

			driver->setTransform(video::ETS_WORLD, driver->getTransform(video::ETS_VIEW));
			for ( irr::f32 dist = 1.f; dist < 501.f;  dist += 10.f)
			{
				irr::f32 xPos = eventReceiver.Camera->isOrthogonal() ? -dist/10.f : -5.f;
				driver->draw3DLine(vector3df(xPos, 20, dist), vector3df(xPos, -20, dist), SColor(255, 200, 10, 100));
			}

			driver->endScene();
		}
		else
		{
			device->sleep(10);
		}
	}

	device->closeDevice();
	device->run();
	device->drop();

	return 0;
}
