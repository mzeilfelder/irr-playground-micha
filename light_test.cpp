// Experimenting with light strange behavior 

#include <irrlicht.h>
 
using namespace irr;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class MyEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_KEY_INPUT_EVENT && node)
		{
			if ( event.KeyInput.PressedDown )
			{
				switch ( event.KeyInput.Key )
				{
					case KEY_UP: 	node->setPosition(node->getPosition() + core::vector3df(0, 0, 10)); break;
					case KEY_DOWN:	node->setPosition(node->getPosition() + core::vector3df(0, 0, -10)); break;
					case KEY_LEFT:	node->setPosition(node->getPosition() + core::vector3df(-10, 0, 0)); break;
					case KEY_RIGHT:	node->setPosition(node->getPosition() + core::vector3df(10, 0, 0)); break;
					default:
						break;
				}
			}
		}

		return false;
	}
	irr::scene::ISceneNode * node = nullptr;
};
 
int main()
{
    auto driverType = video::E_DRIVER_TYPE::EDT_OPENGL;
    // create device
 
    auto device = createDevice(driverType,
            core::dimension2d<u32>(640, 480), 16, false);
       
    if (device == nullptr)
        return 1; // could not create driver.
	
	MyEventReceiver receiver;
	device->setEventReceiver(&receiver);
 
    auto driver = device->getVideoDriver();
    auto smgr = device->getSceneManager();

	core::vector3df camPos(60,200,-30);
	core::vector3df camTarget(60,0,-30);

	irr::scene::ICameraSceneNode * cam;
//	irr::scene::ICameraSceneNode * cam = smgr->addCameraSceneNodeFPS();
//	cam->setPosition(camPos);
//	cam->setTarget(camTarget);
    cam = smgr->addCameraSceneNode(nullptr, camPos, camTarget);
	cam->setUpVector(core::vector3df(0, 0, 1));
 
	auto topLight = smgr->addLightSceneNode(nullptr,
                          core::vector3df(100, 150, -30),  
                          video::SColorf(0.2f, 0.2f, 0.2f),
                          1500);
	receiver.node = topLight;	
  
	irr::scene::ISceneNode * bb = smgr->addBillboardSceneNode(topLight, core::dimension2d<f32>(5, 5));
	bb->setMaterialFlag(video::EMF_LIGHTING, false);
  
 
	smgr->setAmbientLight(video::SColorf(0.f, 0.0f, 0.0f));

	auto normal = irr::core::vector3df(0.0, 1.0, 0.0);
	auto color = irr::video::SColor(255, 255, 127, 128);

	// Create a mesh.
	auto mesh = new irr::scene::SMesh();
	auto buffer = new irr::scene::SMeshBuffer();
	mesh->addMeshBuffer(buffer);
	buffer->drop();

	// Add vertices
	buffer->Vertices.push_back(irr::video::S3DVertex(irr::core::vector3df(121.919998f, 0.0f, 0.0f),
												   normal, color,
												   irr::core::vector2df(6.09600019f, 0.0f)));

	buffer->Vertices.push_back(irr::video::S3DVertex(irr::core::vector3df(121.919998f, 0.0f, -60.9599991f),
												   normal, color,
												   irr::core::vector2df(6.09600019f, 3.04800010f)));

	buffer->Vertices.push_back(irr::video::S3DVertex(irr::core::vector3df(0.0f, 0.0f, -60.9599991f),
												   normal, color,
												   irr::core::vector2df(0.0f, 3.04800010f)));

	buffer->Vertices.push_back(irr::video::S3DVertex(irr::core::vector3df(0.0f, 0.0f, 0.0f),
												   normal, color,
												   irr::core::vector2df(0.0f, 0.0f)));

	buffer->Vertices.push_back(irr::video::S3DVertex(irr::core::vector3df(34.3258286f, 0.0f, -26.0343609f),
												   normal, color,
												   irr::core::vector2df(1.71629155f, 1.30171800f)));

	buffer->Vertices.push_back(irr::video::S3DVertex(irr::core::vector3df(57.3258286f, 0.0f, -26.0343609f),
												   normal, color,
												   irr::core::vector2df(2.86629152f, 1.30171800f)));

	buffer->Vertices.push_back(irr::video::S3DVertex(irr::core::vector3df(34.3258286f, 0.0f, -30.0573101f),
												   normal, color,
												   irr::core::vector2df(1.71629155f, 1.50286555f)));

	buffer->Vertices.push_back(irr::video::S3DVertex(irr::core::vector3df(57.3258286f, 0.0f, -30.0573101f),
												   normal, color,
												   irr::core::vector2df(2.86629152f, 1.50286555f)));

	// Add triangles

	buffer->Indices.push_back(0);
	buffer->Indices.push_back(1);
	buffer->Indices.push_back(2);

	buffer->Indices.push_back(2);
	buffer->Indices.push_back(3);
	buffer->Indices.push_back(4);

	buffer->Indices.push_back(5);
	buffer->Indices.push_back(4);
	buffer->Indices.push_back(3);

	buffer->Indices.push_back(2);
	buffer->Indices.push_back(4);
	buffer->Indices.push_back(6);

	buffer->Indices.push_back(5);
	buffer->Indices.push_back(3);
	buffer->Indices.push_back(0);

	buffer->Indices.push_back(2);
	buffer->Indices.push_back(6);
	buffer->Indices.push_back(7);

	buffer->Indices.push_back(7);
	buffer->Indices.push_back(5);
	buffer->Indices.push_back(0);

	buffer->Indices.push_back(0);
	buffer->Indices.push_back(2);
	buffer->Indices.push_back(7);

	buffer->Indices.push_back(4);
	buffer->Indices.push_back(5);
	buffer->Indices.push_back(7);

	buffer->Indices.push_back(7);
	buffer->Indices.push_back(6);
	buffer->Indices.push_back(4); 

	buffer->recalculateBoundingBox();
	mesh->recalculateBoundingBox();

	irr::scene::IMeshSceneNode* node = smgr->addMeshSceneNode(mesh);
	node->setMaterialFlag(irr::video::EMF_GOURAUD_SHADING, false);
	node->setDebugDataVisible(irr::scene::EDS_MESH_WIRE_OVERLAY|irr::scene::EDS_NORMALS );
    /*
    Now draw everything and finish.
    */
    while(device->run())
    {
		driver->beginScene(true, true, video::SColor(0,100,100,100));

		smgr->drawAll();

		driver->endScene();
    }
 
    device->drop();
   
    return 0;
}
