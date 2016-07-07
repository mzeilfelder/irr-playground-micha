// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// 2 bugs: 
// 1. When setting no material (aka solid) then OpenGL still uses a second texture.
// 2. Coordinates of second textures set with EMT_TRANSPARENT_REFLECTION_2_LAYER are really strange. Flipped v coordinate and too big on opengl and too small on D3D. 
//    Also u coordinate is changing on resizing

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
	IrrlichtDevice *  device = createDevice(irr::video::EDT_DIRECT3D9, irr::core::dimension2d<irr::u32>(800,600));
	//IrrlichtDevice *  device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800,600));
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
	backCube->setMaterialType(video::EMT_TRANSPARENT_REFLECTION_2_LAYER);
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

			driver->beginScene(true, true);

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
