// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Experimenting with skinned mesh loading.
// This is about thread http://irrlicht.sourceforge.net/forum/viewtopic.php?f=18&t=51158
// 3 problems:
// - calling finalize() once more creates memory leakes
// - calling finalize() 3 more times gives a crash
// - removing joints() does not work because CSkinnedMesh::finalize() does not clean up RootJoints (and also no other way to clean them)

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace scene;
using namespace core;

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

	smgr->addCameraSceneNode (0, core::vector3df(0, 0, 0),
								core::vector3df(30, 30, 0),
								-1);

IAnimatedMesh * dwarf_mesh = smgr->getMesh("../../media/dwarf.x");
dwarf_mesh = smgr->getMesh("../../media/dwarf.x");
ISkinnedMesh * skinned = (ISkinnedMesh *)dwarf_mesh;
 
//for (int i = 0; i < skinned->getJointCount(); i++)
//{
//      printf("Joint %i name: %s\n", i, skinned->getAllJoints()[i]->Name);
//}
// 
//skinned->getAllJoints().clear(); // remove all joints - its not important when i comment this line no changes for crashing
// 
//printf("\tbuffer count: %i", skinned->getMeshBufferCount());
//skinned->getMeshBuffers().erase(0);
//printf("\tbuffer count: %i", skinned->getMeshBufferCount());
 
skinned->finalize();
skinned->finalize();
skinned->finalize();
IMeshSceneNode * dwarf_node = smgr->addMeshSceneNode(dwarf_mesh, 0, 0, vector3df(30, 30, 0)); // works fine
 
IAnimatedMeshSceneNode * anim_node = smgr->addAnimatedMeshSceneNode(dwarf_mesh, 0, 0, vector3df(60, 30, 0)); // crashes

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
