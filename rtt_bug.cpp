// TEST to debug a problem with rtt and d3d9

#include <irrlicht.h>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

// Create a rtt, draw something into it and copy resulting image to a newly created texture
irr::video::ITexture* drawIntoTexture(irr::video::IVideoDriver * driver)
{
	irr::core::dimension2d<irr::u32> texdim(64, 64);

	// set rt
	irr::video::ITexture * rtTex = driver->addRenderTargetTexture(texdim, "rtTxt", irr::video::ECF_A8R8G8B8);
	driver->setRenderTarget(rtTex, irr::video::ECBF_COLOR|irr::video::ECBF_DEPTH, irr::video::SColor(0,0,0,0));

	// draw some X into the texture
	driver->draw2DLine( irr::core::vector2di(0,0), irr::core::vector2di(texdim.Width,texdim.Height), irr::video::SColor(255,127,127,255));
	driver->draw2DLine( irr::core::vector2di(texdim.Width,0), irr::core::vector2di(0,texdim.Height), irr::video::SColor(255,127,127,255));

	// reset rt
	driver->setRenderTarget((irr::video::ITexture *)nullptr, irr::video::ECBF_COLOR|irr::video::ECBF_DEPTH);

	// Don't use rt directly as rtt's are reset on resize
	// But that part doesn't matter - the rtt is already getting a black texture (have debugged memory directly)
	irr::video::IImage * img = driver->createImage (rtTex, irr::core::position2di(0,0), rtTex->getSize());
	driver->removeTexture(rtTex);
	irr::video::ITexture*  result = driver->addTexture("texture", img);

	img->drop();

	return result;
}

int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;	// works with opengl
	IrrlichtDevice *device = createDevice(driverType, core::dimension2d<u32>(640, 480),	16, false, false);
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	// add camera
	scene::ICameraSceneNode* cam = smgr->addCameraSceneNode();
	cam->setPosition(core::vector3df(-50,50,-150));

	// create test cube
	scene::ISceneNode* test = smgr->addCubeSceneNode(60);
	test->setPosition(core::vector3df(-10,0,-10));
	test->setMaterialFlag(video::EMF_LIGHTING, false); // disable dynamic lighting

#if 1	// BUG: when set to 0 the rtt no longer works
	// We have to draw something once to get rtt working on D3D9 
	driver->beginScene(true, true, irr::video::SColor(0x00FFFFFF));
	driver->draw3DLine( core::vector3df(0,0,0), core::vector3df(1,1,1));	// 2d or 3d doesn't matter
	//driver->draw2DLine( irr::core::vector2di(0,0), irr::core::vector2di(10,10), irr::video::SColor(255,127,127,255));
	driver->endScene();
#endif

	// set cube texture to texture created from rtt
	test->setMaterialTexture(0, drawIntoTexture(driver));

	// just draw the cube
	while(device->run())
	{
		driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(0));
		smgr->drawAll();
		driver->endScene();
		device->sleep(10);
	}

	device->drop(); // drop device
	return 0;
}
