// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 

#include <irrlicht.h>
#include <iostream>
#include <cassert>
 
using namespace irr;
using namespace core;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
 
void drawFrustum(irr::video::IVideoDriver * videoDriver, const irr::scene::SViewFrustum& frustum,  irr::video::SColor col)
{
	videoDriver->draw3DLine(frustum.getNearLeftUp(), frustum.getNearRightUp(), col);
	videoDriver->draw3DLine(frustum.getNearRightUp(), frustum.getNearRightDown(), col);
	videoDriver->draw3DLine(frustum.getNearRightDown(), frustum.getNearLeftDown(), col);
	videoDriver->draw3DLine(frustum.getNearLeftDown(), frustum.getNearLeftUp(), col);

	videoDriver->draw3DLine(frustum.getNearLeftUp(), frustum.getFarLeftUp(), col);
	videoDriver->draw3DLine(frustum.getNearRightUp(), frustum.getFarRightUp(), col);
	videoDriver->draw3DLine(frustum.getNearRightDown(), frustum.getFarRightDown(), col);
	videoDriver->draw3DLine(frustum.getNearLeftDown(), frustum.getFarLeftDown(), col);

	videoDriver->draw3DLine(frustum.getFarLeftUp(), frustum.getFarRightUp(), col);
	videoDriver->draw3DLine(frustum.getFarRightUp(), frustum.getFarRightDown(), col);
	videoDriver->draw3DLine(frustum.getFarRightDown(), frustum.getFarLeftDown(), col);
	videoDriver->draw3DLine(frustum.getFarLeftDown(), frustum.getFarLeftUp(), col);
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

	scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );
	camera->setTarget(irr::core::vector3df(0,0,0));
	camera->updateAbsolutePosition();
	camera->setPosition(irr::core::vector3df(0, 50, -100));
	camera->updateAbsolutePosition();

	camera->setNearValue(100.f);	// get a smaller frustum
	camera->setFarValue(500.f);
	camera->updateMatrices();
	scene::SViewFrustum origFrustum = *(camera->getViewFrustum());

	camera->setNearValue(1.f);
	camera->setFarValue(3000.f);

	array<line3df> lines, linesClipped;
	lines.push_back( line3df(-500.f,-500.f,-500.f, 500.f, 500.f, 500.f) );
	lines.push_back( line3df(500.f,-500.f,-500.f, -500.f, 500.f, 500.f) );
	lines.push_back( line3df(-500.f,500.f,-500.f, 500.f, -500.f, 500.f) );
	lines.push_back( line3df(-500.f,-500.f,500.f, 500.f, 500.f, -500.f) );

	for ( u32 i=0; i < lines.size(); ++i )
	{
		line3df clipped(lines[i]);
		origFrustum.clipLine(clipped);
		linesClipped.push_back(clipped);
	}

	while ( Device->run() )
	{
		if ( Device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			smgr->drawAll();

			videoDriver->setMaterial(material);            
			videoDriver->setTransform(video::ETS_WORLD, core::matrix4());

			videoDriver->draw3DBox( aabbox3df(-5.f, -5.f, -5.f, 5.f, 5.f, 5.f), video::SColor(255, 127, 127, 127));

			drawFrustum(videoDriver, origFrustum, video::SColor(255, 20, 255, 20) );

			for ( u32 i=0; i < lines.size(); ++i )
			{
				videoDriver->draw3DLine( lines[i].start, lines[i].end, video::SColor(255, 20, 20, 255) );
			}

			for ( u32 i=0; i < linesClipped.size(); ++i )
			{
				videoDriver->draw3DLine( linesClipped[i].start, linesClipped[i].end, video::SColor(255, 255, 20, 20) );
			}

			videoDriver->endScene();
		}

		Device->sleep( 5 );
	}

	Device->closeDevice();
	Device->drop();
	Device = NULL;

	return 0;
}
