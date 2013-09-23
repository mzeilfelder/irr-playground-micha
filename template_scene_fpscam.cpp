// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Template for a minimal scene + a fps camera.
// Also contains some function to draw a helper-box at some point.

#include <irrlicht.h>
#include <iostream>
#include <cassert>
 
using namespace irr;
using namespace core;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
 
// helper function to draw a box around center
void dummyDrawBox(irr::core::vector3df center, irr::video::SColor col, irr::video::IVideoDriver * videoDriver )
{
	aabbox3df box(center);
	irr::f32 r = 0.5f;
	box.addInternalPoint( center + vector3df(r, 0, 0) );
	box.addInternalPoint( center + vector3df(0, r, 0) );
	box.addInternalPoint( center + vector3df(0, 0, r) );
	box.addInternalPoint( center + vector3df(-r, 0, 0) );
	box.addInternalPoint( center + vector3df(0, -r, 0) );
	box.addInternalPoint( center + vector3df(0, 0, -r) );
	videoDriver->draw3DBox( box, col);
}

int main()
{
	IrrlichtDevice *  Device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800,600));
	if (!Device)
		return false;
   
	scene::ISceneManager* smgr = Device->getSceneManager();
	video::IVideoDriver * videoDriver = Device->getVideoDriver ();
   
	video::SMaterial material;
	material.NormalizeNormals = true;
	material.setTexture(0, 0);
	material.Lighting = false;
	material.BackfaceCulling = false;

	triangle3df triangle(vector3df(11300.000000f, 129.411758f, 200.000000f),
						vector3df(11200.000000f, 94.117645f, 300.000000f),
						vector3df(11300.000000f, 129.411758f, 300.000000f));
	irr::core::vector3df center( (triangle.pointA+triangle.pointB+triangle.pointC)*(1.f/3.f) );

	irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );
	camera->updateAbsolutePosition();
	camera->setTarget(center);
	camera->updateAbsolutePosition();
	camera->setPosition(center+irr::core::vector3df(0, 50, -100));
	camera->updateAbsolutePosition();

	while ( Device->run() )
	{
		if ( Device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			smgr->drawAll();

			videoDriver->setMaterial(material);            
			videoDriver->setTransform(video::ETS_WORLD, core::matrix4());

			videoDriver->draw3DTriangle(triangle, video::SColor(255,0,0,255));

			videoDriver->endScene();
		}

		Device->sleep( 5 );
	}

	Device->closeDevice();
	Device->drop();
	Device = NULL;

	return 0;
}
