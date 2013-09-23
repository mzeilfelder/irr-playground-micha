// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Recreating a device constantly. Leads for some reason to occasional kernel-crashes on my system (probably nvidia driver trouble).

#include <irrlicht.h>
#include <time.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace io;
using namespace gui;

IrrlichtDevice *device;
IVideoDriver* driver;
IGUIEnvironment* guienv;
ITimer* timer;

void wait()
{
	const u32 timeMs = 100;
	struct timespec ts;
	ts.tv_sec = (time_t) (timeMs / 1000);
	ts.tv_nsec = (long) (timeMs % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

void init()
{
	SIrrlichtCreationParameters params;
	
	params.AntiAlias = false;
	params.Bits = 16;
	params.DriverType = video::EDT_OPENGL;
	params.EventReceiver = 0;
	params.Fullscreen = false;
	params.HighPrecisionFPU = true;
	params.IgnoreInput = false;
	params.Stencilbuffer = false;
	params.Vsync = false;
	params.WindowId = 0;
	params.WindowSize = dimension2d<u32>(640, 480);
	params.WithAlphaChannel = false;
	params.Doublebuffer = true;
	device = createDeviceEx(params);
	
	
 	//	device = createDevice( video::EDT_SOFTWARE, dimension2d<u32>(640, 480), 16, false, false, false, 0);
	//device = createDevice( video::EDT_OPENGL, dimension2d<u32>(640, 480), 16, false, false, false, 0);
	device->getLogger()->setLogLevel(ELL_INFORMATION);
	driver = device->getVideoDriver();
	guienv = device->getGUIEnvironment();
	timer = device->getTimer();
}

int main()
{
	init();
	s32 startTime = timer->getRealTime();

	while(device->run())
	{
		s32 now = timer->getRealTime();
		if (now - startTime > 5000)
		{
			device->closeDevice();
			device->run();
			device->drop();
			init();
			startTime = now;
		}

		driver->beginScene(true, true, SColor(255,100,101,140));
		guienv->drawAll();
		driver->endScene();
	}
	device->drop();
	return 0;
} 
