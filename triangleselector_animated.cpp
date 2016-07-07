// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Test for triangle selectors with animated meshes

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#include <irrlicht.h>

using namespace irr;

const irr::video::SColor NO_HIT_COL = irr::video::SColor(255, 100, 255, 100);
const irr::video::SColor HIT_COL = irr::video::SColor(255, 255, 100, 100);

class MyEventReceiver : public IEventReceiver
{
protected:
	irr::scene::ISceneCollisionManager* collisionManager;

public:
	irr::core::line3d<f32> ray;
	irr::video::SColor rayColor;
	scene::IAnimatedMeshSceneNode* unitNode;

	void Initialize(IrrlichtDevice* const deviceIn, core::vector2d<f32> initPos)
	{
		this->collisionManager = deviceIn->getSceneManager()->getSceneCollisionManager();

		scene::ISceneManager* smgr = deviceIn->getSceneManager();

//		scene::IAnimatedMesh* mesh1 = smgr->getMesh("../../media/ninja.b3d");
//		scene::IAnimatedMesh* mesh1 = smgr->getMesh("../../media/dwarf.x");
		scene::IAnimatedMesh* mesh1 = smgr->getMesh("media_inceptum/Cannonram.x");

		unitNode = smgr->addAnimatedMeshSceneNode(mesh1);
		unitNode->setAnimationSpeed(5.f);
		unitNode->setPosition(core::vector3df(initPos.X, 0.f, initPos.Y));
		unitNode->setScale(core::vector3df(3.0f, 3.0f, 3.0f));
		unitNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
		unitNode->setMaterialFlag(video::EMF_WIREFRAME, true);

		scene::ITriangleSelector* selector = smgr->createTriangleSelector(unitNode);
		unitNode->setTriangleSelector(selector);
		selector->drop();
	}

	bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_MOUSE_INPUT_EVENT)
		{
			if ( event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN )
			{
				ray = collisionManager->getRayFromScreenCoordinates( core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y) );
				core::vector3df point;
				core::triangle3df triangle;
				scene::ISceneNode* selectedSceneNode = collisionManager->getSceneNodeAndCollisionPointFromRay(ray,point, triangle, 0);
				if (selectedSceneNode)
				{
					rayColor = HIT_COL;
				}
				else
				{
					rayColor = NO_HIT_COL;
				}
			}
		}

		return false;
	}
};

int main()
{
	MyEventReceiver receiver;
	IrrlichtDevice *device = createDevice(video::EDT_DIRECT3D9, core::dimension2d<u32>(800, 600), 32, false, false, true, &receiver);

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	smgr->addCameraSceneNodeFPS();
	scene::ILightSceneNode* lightSun1 = smgr->addLightSceneNode(0,  core::vector3df(812, 812, 812),	video::SColorf(1, 1, 0.94f, 1), 5000.0f);

	receiver.Initialize(device, core::vector2d<f32>(312, 412));
	scene::ITriangleSelector* selector = receiver.unitNode->getTriangleSelector();
	s32 numTriangles = selector->getTriangleCount();
	core::triangle3df* triangles = new core::triangle3df[numTriangles];
	
	while (device->run())
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, video::SColor(255, 100, 101, 140));
			smgr->drawAll();

			irr::video::SMaterial lineMat;
			lineMat.Lighting = false;
			driver->setMaterial(lineMat);
			driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

			driver->draw3DBox(receiver.unitNode->getTransformedBoundingBox(), receiver.rayColor);
			driver->draw3DLine(receiver.ray.start, receiver.ray.end, receiver.rayColor);

			s32 outTriangleCount;
			selector->getTriangles(triangles, numTriangles, outTriangleCount);
			for ( s32 i=0; i < outTriangleCount; ++i )
			{
				driver->draw3DTriangle( triangles[i], irr::video::SColor(255, 0, 0, 127));
			}

			driver->endScene();
		}
		else
			device->yield();
	}

	delete[] triangles;

	device->drop();
	return 0;
}
