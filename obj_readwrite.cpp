// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Test for reading/writing a file in .obj format

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

	smgr->addCameraSceneNode (0, core::vector3df(0, 100, 200),
								core::vector3df(0, 0, 0),
								-1);

	scene::IAnimatedMesh* aniMeshOrig = smgr->getMesh( "my_media/asymetric_arc.obj" );
	scene::IMeshSceneNode * nodeOrig = smgr->addMeshSceneNode (aniMeshOrig, NULL, -1, core::vector3df(-100, 0, 0) );
	nodeOrig->setMaterialFlag(video::EMF_LIGHTING, false);

	aniMeshOrig->getMeshBuffer(0)->getMaterial().getTextureMatrix(0).setTextureScale(2.5f, 2.5f);

	scene::IMeshWriter * objMeshWriter = smgr->createMeshWriter (scene::EMWT_OBJ );
	io::IWriteFile * meshFile = device->getFileSystem()->createAndWriteFile("my_media/write_test.obj");
	objMeshWriter->writeMesh(meshFile, aniMeshOrig);
	meshFile->drop();
	objMeshWriter->drop();

	scene::IAnimatedMesh* aniMeshCopy = smgr->getMesh( "my_media/write_test.obj" );
	scene::IMeshSceneNode * nodeCopy = smgr->addMeshSceneNode (aniMeshCopy, NULL, -1, core::vector3df(100, 0, 0) );
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
