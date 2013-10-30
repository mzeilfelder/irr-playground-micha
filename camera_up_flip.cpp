// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Figure out why the camera flips here.
// Thread discussion here: http://irrlicht.sourceforge.net/forum/viewtopic.php?f=4&t=49219&p=283869#p283869

#include <irrlicht.h>

using namespace irr;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class OwnAnimatorRotation : public irr::scene::ISceneNodeAnimator
{
public:
	//! constructor
	OwnAnimatorRotation::OwnAnimatorRotation(const core::vector3df& rotation)
	: FirstRun(true), RotationPerSec(rotation)
	{
	}


	//! animates a scene node
	void animateNode(irr::scene::ISceneNode* node, u32 timeMs)
	{
		if (!node) 
			return;

		if( FirstRun )
		{
			FirstRun = false;
			StartTime = timeMs;
			StartRotation = node->getRotation();
		}

		const u32 diffTime = timeMs - StartTime;

		if (diffTime != 0)
		{
			core::vector3df rot = StartRotation +  RotationPerSec*(diffTime*0.1f);
			if (rot.X>360.f)
				rot.X=fmodf(rot.X, 360.f);
			if (rot.Y>360.f)
				rot.Y=fmodf(rot.Y, 360.f);
			if (rot.Z>360.f)
				rot.Z=fmodf(rot.Z, 360.f);
			node->setRotation(rot);
		}
	}


	virtual ISceneNodeAnimator* createClone(irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager=0) { return NULL; }

	private:
		bool FirstRun;
		core::vector3df StartRotation;
		core::vector3df RotationPerSec;
		u32 StartTime;
};
 
int main()
{
	IrrlichtDevice *device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(640, 480), 16, false);
	if (device == 0)
		return 1; // could not create selected driver.
 
	// create engine and camera
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	scene::ISceneNode* skybox=smgr->addSkyBoxSceneNode(
        driver->getTexture("../../media/irrlicht2_up.jpg"),
        driver->getTexture("../../media/irrlicht2_dn.jpg"),
        driver->getTexture("../../media/irrlicht2_lf.jpg"),
        driver->getTexture("../../media/irrlicht2_rt.jpg"),
        driver->getTexture("../../media/irrlicht2_ft.jpg"),
        driver->getTexture("../../media/irrlicht2_bk.jpg"));


	scene::IAnimatedMesh* aniMesh = smgr->getMesh( "my_media/asymetric_arc.obj" );
	if (!aniMesh)
		return 0;
	scene::IMesh * mesh = aniMesh->getMesh (0);
	if ( !mesh )
		return 0;
	mesh->setHardwareMappingHint(scene::EHM_STATIC);
	scene::IMeshSceneNode * myNode = smgr->addMeshSceneNode (mesh, NULL, -1, core::vector3df(0, 0, 0) );
	myNode->setMaterialFlag(video::EMF_LIGHTING, false);

//	irr::scene::ISceneNode *myNode = smgr->addCubeSceneNode( 10.f );


	scene::ISceneNodeAnimator* anim = new OwnAnimatorRotation(core::vector3df(0.f, 0, 0.8f));
    if(anim)
    {
        myNode->addAnimator(anim);
        anim->drop();
        anim = 0;
    }

	const irr::core::vector3df distance(0,140,0);
	smgr->addCameraSceneNode(NULL, -distance, core::vector3df(0,0,0));
 
    while(device->run())
    {
        driver->beginScene(true, true, video::SColor(0,100,100,100));  

		core::matrix4 targetMat = myNode->getAbsoluteTransformation();
		core::vector3df targetPos = targetMat.getTranslation();

        irr::core::vector3df up(0,1,0);
        targetMat.rotateVect(up);

		smgr->getActiveCamera()->setUpVector(up);
		smgr->getActiveCamera()->updateAbsolutePosition();

		//smgr->getActiveCamera()->setTarget( targetPos );	// stays the same anyway - always 0,0,0
		//smgr->getActiveCamera()->updateAbsolutePosition();

		irr::core::vector3df behind(distance);
		targetMat.rotateVect(behind);
		smgr->getActiveCamera()->setPosition( targetPos - behind );

        smgr->drawAll();
 
        driver->endScene();

		core::stringw str(L"UP: ");
		str += up.X;
		str += L"/";
		str += up.Y;
		str += L"/";
		str += up.Z;

		device->setWindowCaption(str.c_str());

		device->sleep(20, true);
		device->sleep(10, false);
    }

	device->closeDevice();
    device->drop();
   
    return 0;
}
