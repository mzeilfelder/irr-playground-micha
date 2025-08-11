// Test using EJUOR_CONTROL to skin a mesh

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#include <irrlicht.h>
#include "driverChoice.h"
#include "exampleHelper.h"

using namespace irr;

irr::scene::ISkinnedMesh* createSkinnedMeshCopy(irr::scene::ISkinnedMesh* mesh, irr::scene::ISceneManager * sceneManager)
{
	using namespace irr;
	using namespace scene;

    if ( !mesh )
        return nullptr;

	ISkinnedMesh* skinnedMesh = sceneManager->createSkinnedMesh();

    for ( u32 i=0; i < mesh->getMeshBuffers().size(); ++i )
    {
        SSkinMeshBuffer * buffer = skinnedMesh->addMeshBuffer();
        *buffer = *(mesh->getMeshBuffers()[i]);
    }

    for ( u32 j=0; j < mesh->getAllJoints().size(); ++j )
    {
        ISkinnedMesh::SJoint *joint = skinnedMesh->addJoint();
        *joint = *(mesh->getAllJoints()[j]);
	}

    // fix children pointers (they still have old pointers)
    core::array<ISkinnedMesh::SJoint*> & newJoints = skinnedMesh->getAllJoints();
    core::array<ISkinnedMesh::SJoint*> & oldJoints = mesh->getAllJoints();
    for ( u32 i=0; i < newJoints.size(); ++i )
    {
        ISkinnedMesh::SJoint * joint = newJoints[i];
        for ( u32 c=0; c < joint->Children.size(); ++c )
        {
            // the child is one of the oldJoints and must be replaced by the newjoint on the same index
            for ( u32 k=0; k < oldJoints.size(); ++k )
            {
                if ( joint->Children[c] == oldJoints[k] )
                {
                    joint->Children[c] = newJoints[k];
                    break;
                }
            }
        }
    }

	skinnedMesh->finalize();

    return skinnedMesh;
}

int main()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;

	// create device
	IrrlichtDevice* device = createDevice(driverType,
			core::dimension2d<u32>(640, 480), 16, false, false, false);

	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	const io::path mediaPath = getExampleMediaPath();

	scene::ISkinnedMesh* ninjaMesh = static_cast<irr::scene::ISkinnedMesh*>(smgr->getMesh(mediaPath + "ninja.b3d"));
	scene::IAnimatedMeshSceneNode* ninjaNode = smgr->addAnimatedMeshSceneNode(ninjaMesh);
	if (ninjaNode)
	{
		ninjaNode->setJointMode(scene::EJUOR_READ);
		ninjaNode->setMaterialFlag(video::EMF_LIGHTING, false);	// have no light in example
		ninjaNode->setAnimationSpeed(15);
	}

	const int NUM_COPIES = 100;

	irr::core::array<scene::ISkinnedMesh*> ninjaMeshCopies;
	irr::core::array<scene::IAnimatedMeshSceneNode*> ninjaNodeCopies;
	for ( int i=0; i < NUM_COPIES; ++i )
	{
		ninjaMeshCopies.push_back( createSkinnedMeshCopy(ninjaMesh, smgr) );
		ninjaNodeCopies.push_back( smgr->addAnimatedMeshSceneNode(ninjaMeshCopies.getLast(), 0, -1, core::vector3df(6.f*i, 0, 0)) );
		ninjaNodeCopies.getLast()->setJointMode(scene::EJUOR_CONTROL);
		ninjaNodeCopies.getLast()->setMaterialFlag(video::EMF_LIGHTING, false);
		ninjaNodeCopies.getLast()->setAutomaticCulling(scene::EAC_OFF);
	}

	irr::core::vector3df meshExtent = ninjaMesh->getBoundingBox().getExtent();
	smgr->addCameraSceneNode(0, irr::core::vector3df(3.f*NUM_COPIES,meshExtent.Y,100.f), core::vector3df(3.f*NUM_COPIES,meshExtent.Y*0.5f,0));

	int lastFPS = -1;

	while(device->run())
	{
		driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,113,113,133));

		if (ninjaNode)
		{
			const u32 nrJoints = ninjaNode->getJointCount();

			for ( u32 j=0; j<nrJoints; ++j )
			{
				scene::IBoneSceneNode* bone = ninjaNode->getJointNode(j);

				for ( int i=0; i < NUM_COPIES; ++i )
				{
					scene::IBoneSceneNode* bone2 = ninjaNodeCopies[i]->getJointNode(j);
					bone2->setPosition( bone->getPosition() );
					bone2->setRotation( bone->getRotation() );
					bone2->setScale( bone->getScale() );
				}
			}
		}

		smgr->drawAll(); // draw the 3d scene
		device->getGUIEnvironment()->drawAll(); // draw the gui environment

		driver->endScene();

		int fps = driver->getFPS();
		if (lastFPS != fps)
		{
			core::stringw tmp(L"FPS: ");
			tmp += fps;
			device->setWindowCaption(tmp.c_str());
			lastFPS = fps;
		}
	}

	for ( int i=0; i < NUM_COPIES; ++i )
	{
		ninjaMeshCopies[i]->drop();
	}
	device->drop();

	return 0;
}
