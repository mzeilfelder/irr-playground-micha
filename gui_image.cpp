// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Template for minimal Irrlicht gui-code.

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif


int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(800, 600));
	if (device == 0)
		return 1; // could not create selected driver.

	// irrlichtlogo2.png is 128x128
	
	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	
	IGUIImage * image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(0, 0));
	image->setScaleImage(true);
	
	IGUIImage * image2 = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(130, 0));	
	image2->setScaleImage(true);
	image2->setSourceRect(recti(10, 10, 60, 60));
	
	IGUIImage * image3 = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(260, 0));	
	image3->setScaleImage(true);
	image3->setSourceRect(recti(0, 0, 500, 500));
	
	IGUIImage * image4 = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(390, 0));	
	image4->setScaleImage(true);
	image4->setSourceRect(recti(-50, -50, 50, 50));
	
	IGUIImage * image5 = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(520, 0));	
	image5->setScaleImage(true);
	image5->setSourceRect(recti(50, 50, 0, 0));
	
	IGUIImage * image6 = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(650, 0));	
	image6->setScaleImage(true);
	image6->setSourceRect(recti(128, 0, 0, 128));	

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, SColor(0,200,200,200));
	
			env->drawAll();
		
			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
