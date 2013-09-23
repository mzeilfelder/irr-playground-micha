// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// A simpler test-case for debugging our shadow volume troubles.

#include <irrlicht.h>
#include <iostream>
#include "driverChoice.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

int main()
{
	// ask user for driver
//	video::E_DRIVER_TYPE driverType= video::EDT_OPENGL;
	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;

	IrrlichtDevice *device =
		createDevice(driverType, core::dimension2d<u32>(640, 480),
		16, false, true);

	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	// room scene
	scene::IAnimatedMesh* mesh = smgr->getMesh("../../media/room.3ds");
	scene::ISceneNode* node = 0;
	node = smgr->addAnimatedMeshSceneNode(mesh);

	// create light
	node = smgr->addLightSceneNode(0, core::vector3df(0,0,0),
		video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 800.0f);
	node->setPosition( core::vector3df(-150, 150, 0) );
	scene::ISceneNodeAnimator* anim = 0;
	//anim = smgr->createFlyCircleAnimator (core::vector3df(0,150,0),250.0f);
	//node->addAnimator(anim);
	//anim->drop();

	// attach billboard to light
	node = smgr->addBillboardSceneNode(node, core::dimension2d<f32>(50, 50));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	node->setMaterialTexture(0, driver->getTexture("../../media/particlewhite.bmp"));

/*	// volumetric light
	scene::IVolumeLightSceneNode * n = smgr->addVolumeLightSceneNode(0, -1,
				32,                              // Subdivisions on U axis
				32,                              // Subdivisions on V axis
				video::SColor(0, 255, 255, 255), // foot color
				video::SColor(0, 0, 0, 0));      // tail color

	if (n)
	{
		n->setScale(core::vector3df(56.0f, 56.0f, 56.0f));
		n->setPosition(core::vector3df(-120,50,40));

		// load textures for animation
		core::array<video::ITexture*> textures;
		for (s32 g=7; g > 0; --g)
		{
			core::stringc tmp;
			tmp = "../../media/portal";
			tmp += g;
			tmp += ".bmp";
			video::ITexture* t = driver->getTexture( tmp.c_str() );
			textures.push_back(t);
		}

		// create texture animator
		scene::ISceneNodeAnimator* glow = smgr->createTextureAnimator(textures, 150);
		n->addAnimator(glow);
		glow->drop();
	}
	*/

	const bool zfail = true;
	const f32 infinity = 10000.f;
	smgr->setShadowColor(video::SColor(150,0,255,0));

	// add animated character
//	mesh = smgr->getMesh("../../media/sydney.md2");
//	scene::IAnimatedMeshSceneNode*	anode = smgr->addAnimatedMeshSceneNode(mesh);
//	anode->setPosition(core::vector3df(-50,70,-60));
//	anode->setAnimationSpeed(0);
//	anode->setCurrentFrame(280);
//
//	// add shadow
//	scene::IShadowVolumeSceneNode* shadow = anode->addShadowVolumeSceneNode(0, -1, zfail, infinity);
////	shadow->setDebugDataVisible(scene::EDS_MESH_WIRE_OVERLAY);
//
//	// make the model a little bit bigger and normalize its normals
//	// because of the scaling, for correct lighting
//	anode->setScale(core::vector3df(3,3,3));
//	anode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);

	// second character
//	scene::IMesh * cube = smgr->getGeometryCreator()->createCubeMesh( core::vector3df(50.f, 50.f, 50.f) );
//	scene::IMesh * cube = smgr->getGeometryCreator()->createSphereMesh( 50.f );
	scene::IMesh * cube = smgr->getGeometryCreator()->createArrowMesh( 2, 2, 50.f, 20, 15, 20);
	cube->getMeshBuffer(0)->getMaterial().BackfaceCulling = false;
	cube->getMeshBuffer(0)->getMaterial().Wireframe = true;
	scene::IAnimatedMesh * anicube = smgr->getMeshManipulator()->createAnimatedMesh(cube);
	cube->drop();
	scene::IAnimatedMeshSceneNode*	anode2 = smgr->addAnimatedMeshSceneNode(anicube);
	anicube->drop();
	anode2->setPosition(core::vector3df(50,70,60));
	scene::IShadowVolumeSceneNode* shadow2 = anode2->addShadowVolumeSceneNode(0, -1, zfail, infinity);
//	shadow2->setDebugDataVisible(scene::EDS_MESH_WIRE_OVERLAY);


	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS();
//	scene::ICameraSceneNode* camera = smgr->addCameraSceneNode();
	camera->setPosition(core::vector3df(-200,200,-100));
	camera->updateAbsolutePosition();
//	camera->setTarget(anode->getPosition());

	while(device->run())
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, 0);

			smgr->drawAll();

			driver->endScene();

			//core::stringw str( anode->getFrameNr() );
			//device->setWindowCaption(str.c_str());
		}
	}

	device->drop();

	return 0;
}

