// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Template for a minimal Irrlicht scene - just a camera, light + node.

#include <irrlicht.h>
#include <iostream>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


int main(int argc, char *argv[])
{
	IrrlichtDevice * Device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(640,480) );
	if (!Device)
		return false;
	
	scene::ISceneManager* smgr = Device->getSceneManager();
	gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();
    video::IVideoDriver* videoDriver =  Device->getVideoDriver();	

	irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );

	irr::video::SMaterial materialCube;
	materialCube.Lighting = false;
	materialCube.setTexture(0, videoDriver->getTexture("../../media/irrlichtlogo3.png"));

	irr::scene::IMeshSceneNode * mesh1 = smgr->addCubeSceneNode (30.0f, 0, -1, 
	                        core::vector3df(30, 0, 100),
							core::vector3df(45, 45, 45),
							core::vector3df(1.0f, 1.0f, 1.0f));
	mesh1->getMaterial(0) = materialCube;

	irr::core::matrix4 textMat;
	textMat.setTextureScale(10.f, 10.f);
	mesh1->getMaterial(0).setTextureMatrix(0, textMat);

	irr::scene::IMeshSceneNode * mesh2 = smgr->addCubeSceneNode (30.0f, 0, -1, 
	                        core::vector3df(-30, 0, 100),
							core::vector3df(45, 45, 45),
							core::vector3df(1.0f, 1.0f, 1.0f));
	mesh2->getMaterial(0) = materialCube;

	while ( Device->run() )
	{
		if ( Device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			smgr->drawAll();
			guiEnv->drawAll();

			videoDriver->endScene();
		}
		Device->sleep( 1 );
	}
	
	Device->closeDevice();
	Device->drop();
	
	return 0;
}
