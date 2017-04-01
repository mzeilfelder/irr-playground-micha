// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Template for loading a model + a fps camera.

#include <irrlicht.h>
 
using namespace irr;
using namespace core;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
 
int main()
{
	IrrlichtDevice *  Device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800,600));
	if (!Device)
		return false;
   
	scene::ISceneManager* smgr = Device->getSceneManager();
	video::IVideoDriver * videoDriver = Device->getVideoDriver ();
   
	irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );
	camera->updateAbsolutePosition();
	camera->setTarget(core::vector3df(0,0,0));
	camera->updateAbsolutePosition();
	camera->setPosition(irr::core::vector3df(0, 50, -100));
	camera->updateAbsolutePosition();

	irr::io::path meshName("../../media/ninja.b3d");
	scene::IAnimatedMesh* m = Device->getSceneManager()->getMesh(meshName);
	if (!m)
		return 1;

	scene::IAnimatedMeshSceneNode* animModel = Device->getSceneManager()->addAnimatedMeshSceneNode(m);
	animModel->setMaterialFlag(video::EMF_LIGHTING, false);
	animModel->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, false);

	while ( Device->run() )
	{
		if ( Device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			smgr->drawAll();

			videoDriver->endScene();
		}

		Device->sleep( 5 );
	}

	Device->closeDevice();
	Device->drop();
	Device = NULL;

	return 0;
}
