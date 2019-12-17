// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Several strange material behaviors:
// EMT_SPHERE_MAP is different on GL and D3D 
// On d3d it seems up-side-down on first view, but that might be correct. As both REFLECTION_2_LAYER materials also show up upside-down.

#include <irrlicht.h>
#include <iostream>
 
using namespace irr;
using namespace core;
using namespace scene;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
 
int main(int argc, char* argv[])
{
	//IrrlichtDevice *  device = createDevice(irr::video::EDT_DIRECT3D9, irr::core::dimension2d<irr::u32>(800,600));
	IrrlichtDevice *  device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800,600));
	//IrrlichtDevice *  device = createDevice(irr::video::EDT_BURNINGSVIDEO, irr::core::dimension2d<irr::u32>(800,600));
	if (!device)
		return 0;
   
	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver * driver = device->getVideoDriver ();
   
	video::SMaterial material;
	IMeshSceneNode * backCube = smgr->addCubeSceneNode();


	backCube->setPosition(vector3df(0, 0, 10));
	backCube->setMaterialTexture(0, driver->getTexture("../../media/wall.bmp"));
	//backCube->setMaterialTexture(0, driver->getTexture("../../media/irrlichtlogo3.png"));
	//backCube->setMaterialTexture(1, driver->getTexture("../../media/water.jpg"));
	backCube->setMaterialTexture(1, driver->getTexture("../../media/irrlichtlogo3.png"));
	// vertex color has alpha 255, hence solid
	backCube->setMaterialType(video::EMT_REFLECTION_2_LAYER);
	//backCube->setMaterialType(video::EMT_SPHERE_MAP);
	//backCube->setMaterialType(video::EMT_TRANSPARENT_REFLECTION_2_LAYER);
//	backCube->setMaterialType(video::EMT_SOLID_2_LAYER);
	backCube->setMaterialFlag(video::EMF_LIGHTING, false);
//	driver->getMeshManipulator()->setVertexColorAlpha(backCube->getMesh(), 45);

	smgr->addCameraSceneNode(0, vector3df(0, 0, -15));

	bool grow = true;
	irr::f32 s = 1.f;
	backCube->setScale(vector3df(5, 5, 1));
	while ( device->run() )
	{
		if ( device->isWindowActive() )
		{
			smgr->getMeshManipulator()->setVertexColorAlpha(backCube->getMesh()->getMeshBuffer(0), s * 50);

			backCube->setScale(vector3df(s, s, 1));

			if ( grow )
			{
				s += 0.01f;
				if ( s > 5.f )
					grow = false;
			}
			else
			{
				s -= 0.01f;
				if ( s < 1.f )
					grow = true;
			}

			driver->beginScene(true, true, video::SColor(255, 20, 30, 40));

			smgr->drawAll();

			driver->endScene();
		}

		device->sleep( 5 );
	}

	device->closeDevice();
	device->drop();
	device = NULL;

	return 0;
}
