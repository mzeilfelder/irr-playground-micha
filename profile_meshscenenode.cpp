﻿// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Test for profiling rendering of simple static MeshSceneNodes
// You can pass 3 parameters for the number of nodes created in x, y and z direction
// So have x*y*z nodes in the end

#include <irrlicht.h>
#include <iostream>
#include <cassert>
 
using namespace irr;
using namespace core;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#define TARGET_APITRACE	// change tests somewhat to make them easier to check with apitrace tool (https://github.com/apitrace/apitrace)
 
int main(int argc, char* argv[])
{
	IrrlichtDevice *  device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800,600));
	//IrrlichtDevice * device = createDevice(irr::video::EDT_DIRECT3D9, irr::core::dimension2d<irr::u32>(800,600));
	//IrrlichtDevice * device = createDevice(irr::video::EDT_BURNINGSVIDEO, irr::core::dimension2d<irr::u32>(800,600));
	if (!device)
		return 0;
   
	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver * videoDriver = device->getVideoDriver ();
   
	scene::IAnimatedMesh* aniMesh = smgr->getMesh( "my_media/asymetric_arc.obj" );
	if (!aniMesh)
		return 0;
	scene::IMesh * mesh = aniMesh->getMesh (0);
	if ( !mesh )
		return 0;
	mesh->setHardwareMappingHint(scene::EHM_STATIC);
	
#ifdef TARGET_APITRACE
	int nodesX = 2;
	int nodesY = 1;
	int nodesZ = 2;
#else
	int nodesX = 30;
	int nodesY = 5;
	int nodesZ = 30;
#endif
	
	if ( argc > 1 )
		nodesX = atoi(argv[1]);
	if ( argc > 2 )
		nodesY = atoi(argv[2]);
	if ( argc > 3 )
		nodesZ = atoi(argv[3]);
	if ( nodesX < 1 )
		nodesX = 1;
	if ( nodesY < 1 )
		nodesY = 1;
	if ( nodesZ < 1 )
		nodesZ = 1;
	
	aabbox3df bbox = mesh->getBoundingBox();
	vector3df extent = bbox.getExtent();
	const f32 GAP = 10.f;
	f32 halfSizeX = 0.5f * (nodesX*extent.X + GAP*(nodesX-1));
	f32 halfSizeY = 0.5f * (nodesY*extent.Y + GAP*(nodesY-1));
	f32 halfSizeZ = 0.5f * (nodesZ*extent.Z + GAP*(nodesZ-1));
	
	irr::f32 changeMat = 0.f;
	for ( int x = 0; x < nodesX; ++x )
	{
		irr::f32 gapX = x > 0 ? (x-1)*GAP : 0.f;
		irr::f32 posX = -halfSizeX + x*extent.X + gapX;
		for ( int y = 0; y < nodesY; ++y )
		{
			irr::f32 gapY = y > 0 ? (y-1)*GAP : 0.f;
			irr::f32 posY = -halfSizeY + y*extent.Y + gapY;
			for ( int z=0; z < nodesZ; ++z )
			{
				irr::f32 gapZ = z > 0 ? (z-1)*GAP : 0.f;
				irr::f32 posZ = -halfSizeZ + z*extent.Z + gapZ;
				scene::IMeshSceneNode * node = smgr->addMeshSceneNode (mesh, NULL, -1, vector3df(posX, posY, posZ) );
				node->setMaterialFlag(video::EMF_LIGHTING, false);
#if 1
				node->getMaterial(0).MaterialTypeParam = changeMat;
				changeMat += 0.1f;	// enforce each material is different
#endif
			}
		}
	}
	
	irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );
	camera->updateAbsolutePosition();
	camera->setTarget( vector3df(0,0,0) );
	camera->updateAbsolutePosition();
	camera->setPosition(irr::core::vector3df(halfSizeX+extent.X, halfSizeY+extent.Y, halfSizeZ+extent.Z));
	camera->updateAbsolutePosition();

	s32 oldFPS = 0;

#ifdef TARGET_APITRACE
	videoDriver->beginScene(true, true);
	videoDriver->endScene();
#else	
	while ( device->run() )
#endif
	{
		if ( device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			smgr->drawAll();

			videoDriver->endScene();
			
			// update information about current frame-rate
			s32 fps = videoDriver->getFPS();
			if ( fps != oldFPS )
			{
				oldFPS = fps;
				core::stringw str(L"FPS: ");
				str.append(core::stringw(fps));
				str += L" Tris: ";
				str.append(core::stringw(videoDriver->getPrimitiveCountDrawn()));
				device->setWindowCaption( str.c_str() );
			}
		}
		else
		{
			device->sleep( 5 );
		}
	}

	device->closeDevice();
	device->drop();
	device = NULL;

	return 0;
}
