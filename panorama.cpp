// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Experimenting with panorama pictures

#include <irrlicht.h>
 
using namespace irr;
using namespace core;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
 
class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver() : Zoom(0.f) {}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_MOUSE_INPUT_EVENT)
		{
			switch(event.MouseInput.Event)
			{
				case EMIE_MOUSE_WHEEL:
					if ( event.MouseInput.Wheel > 0 && Zoom < 100.f )
						Zoom += 1.f;
					else if ( event.MouseInput.Wheel < 0 && Zoom > 0.f )
						Zoom -= 1.f;
					break;
				default:
					break;
			}
		}

		return false;
	}

	float Zoom;
};



int main()
{
	IrrlichtDevice *  device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(1024,768));
	if (!device)
		return false;

	MyEventReceiver eventReceiver;
	device->setEventReceiver(&eventReceiver);
   
	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver * videoDriver = device->getVideoDriver ();
	const scene::IGeometryCreator* geomCreator = smgr->getGeometryCreator();
   
	irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );

	smgr->getParameters()->setAttribute(scene::DEBUG_NORMAL_LENGTH, 50.f );

	irr::video::ITexture * panoramaTex = videoDriver->getTexture("my_media/panorama.jpg");
	if (!panoramaTex)
		return 1;
	u32 horiRes=128;	// don't make larger or too many polygons for 16-bit mesh
	u32 vertRes=128;
	irr::f32 texturePercentage = 0.95f;
	irr::f32 spherePercentage = 2.f;
	irr::f32 radius = 150.f;	// seems to make no difference as long as it's inside near-far planes
	scene::ISceneNode* skyDomeNode = smgr->addSkyDomeSceneNode(panoramaTex, horiRes, vertRes, texturePercentage, spherePercentage, radius);
//	skyDomeNode->getMaterial(0).Wireframe = true;
	//skyDomeNode->setDebugDataVisible(scene::EDS_NORMALS|scene::EDS_MESH_WIRE_OVERLAY);	// note: can't see normals for skydome as they all go toward camera

	scene::IMesh* sphere = geomCreator->createSphereMesh(150.f, horiRes, vertRes);
	video::SMaterial& sphereMaterial = sphere->getMeshBuffer(0)->getMaterial();
//	sphereMaterial.Wireframe = true;
	sphereMaterial.setTexture(0, panoramaTex);
	sphereMaterial.Lighting = false;
	sphereMaterial.BackfaceCulling = false;
	scene::IMeshSceneNode* sphereNode = smgr->addMeshSceneNode(sphere, 0, -1, core::vector3df(-310.f, -200.f, -500.f));
	//sphereNode->setDebugDataVisible(scene::EDS_NORMALS|scene::EDS_MESH_WIRE_OVERLAY);
	sphere->drop();

	irr::f32 origFov = camera->getFOV();

	while ( device->run() )
	{
		if ( device->isWindowActive() )
		{
			camera->setFOV(origFov - eventReceiver.Zoom * 0.01f);
			
			videoDriver->beginScene(true, true);

			smgr->drawAll();

			videoDriver->endScene();
		}

		device->sleep( 5 );
	}

	device->closeDevice();
	device->drop();
	device = NULL;

	return 0;
}
