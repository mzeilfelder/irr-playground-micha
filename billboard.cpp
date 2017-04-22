// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// Based on this test-cased from Seven: http://irrlicht.sourceforge.net/forum/viewtopic.php?f=4&t=49162&p=283384#p283384
//
// Tests for the billboards (boundingbox and scaling behaving strange)

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace core;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

void addBillboards(irr::core::array<IBillboardSceneNode*>& results, irr::scene::ISceneManager* smgr, video::IVideoDriver* driver, int numBillBoards = 4)
{
	core::dimension2d<f32> dim(30.f, 10.f);
    f32 x1 = -100.f*(numBillBoards/2.f);
    for (int x=0; x<numBillBoards; x++)
    {
		IBillboardSceneNode* billboard = smgr->addBillboardSceneNode(0, dim);
		results.push_back(billboard);
        billboard->setMaterialTexture(0, driver->getTexture("media/smoke.bmp"));
        billboard->setPosition(vector3df(x1,0.f,0.f));
        billboard->setMaterialFlag(video::EMF_LIGHTING, false);
        billboard->setDebugDataVisible(EDS_BBOX_ALL);
		billboard->setScale(vector3df(0.4f + 5.f*x, 1.f + 3.f*x, 1.f + 2.f*x));
        x1 += 100.f;
    }
}

int main()
{
	IrrlichtDevice* device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(640, 480),
        16, false, false);

    if (device == 0)
        return 1; // could not create selected driver.

    video::IVideoDriver* driver = device->getVideoDriver();
    scene::ISceneManager* smgr = device->getSceneManager();

    scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS();
    camera->setPosition(core::vector3df(-50,50,-150));
    device->getCursorControl()->setVisible(false);

    irr::core::array<IBillboardSceneNode*> billboards;
    addBillboards(billboards, smgr, driver);

    while(device->run())
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, irr::video::SColor(0));

			smgr->drawAll();

			for ( irr::u32 i=0; i<billboards.size(); ++i )
			{
				driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
				video::SMaterial m;
				m.Lighting = false;
				driver->setMaterial(m);
				driver->draw3DBox(billboards[i]->getTransformedBillboardBoundingBox(camera), video::SColor(0,255,100,100));
			}

			driver->endScene();
		}
	}

    device->drop();

    return 0;
}
