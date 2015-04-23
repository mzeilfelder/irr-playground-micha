// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Test for reading/writing a file in .b3d format

#include <irrlicht.h>
#include <iostream>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


int main(int argc, char *argv[])
{
	IrrlichtDevice * device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(640,480) );
	if (!device)
		return false;

	scene::ISceneManager* smgr = device->getSceneManager();
    video::IVideoDriver* videoDriver = device->getVideoDriver();

	smgr->addCameraSceneNode (0, core::vector3df(0, 10, 20),
								core::vector3df(0, 0, 0),
								-1);

	scene::IAnimatedMesh* aniMeshOrig = smgr->getMesh( "../../media/ninja.b3d" );
	scene::IAnimatedMeshSceneNode  * nodeOrig = smgr->addAnimatedMeshSceneNode (aniMeshOrig, NULL, -1, core::vector3df(-10, 0, 0) );
	nodeOrig->setMaterialFlag(video::EMF_LIGHTING, false);

	scene::IMeshWriter * b3dMeshWriter = smgr->createMeshWriter (scene::EMWT_B3D );
	io::IWriteFile * meshFile = device->getFileSystem()->createAndWriteFile("my_media/write_ninja.b3d");
	b3dMeshWriter->writeMesh(meshFile, aniMeshOrig);
	meshFile->drop();
	b3dMeshWriter->drop();

	scene::IAnimatedMesh* aniMeshCopy = smgr->getMesh( "my_media/write_ninja.b3d" );
	scene::IAnimatedMeshSceneNode  * nodeCopy = smgr->addAnimatedMeshSceneNode (aniMeshCopy, NULL, -1, core::vector3df(10, 0, 0) );
	nodeCopy->setMaterialFlag(video::EMF_LIGHTING, false); 

	while ( device->run() )
	{
		if ( device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			smgr->drawAll();

			videoDriver->endScene();
		}
	}

	device->closeDevice();
	device->drop();

	return 0;
}
