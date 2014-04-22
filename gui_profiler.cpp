// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// profiler

#include <irrlicht.h>

#include "profiler.h"
#include "IGUIProfile.h"

namespace irr
{
	CProfiler gPROFILER;

	IRRLICHT_API CProfiler* IRRCALLCONV getProfiler()
	{
		return &gPROFILER;
	}
} // namespace irr

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
	enum Profiles
	{
		EP_X,
		EP_Y,
		EP_SCOPE
	};

	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	irr::getProfiler()->init(device->getTimer());

	IRR_PROFILE(irr::getProfiler()->add(EP_X, L"A:X", L"group a"));
	IRR_PROFILE(irr::getProfiler()->add(EP_Y, L"B:Y", L"group b"));

	IRR_PROFILE(irr::getProfiler()->start(EP_X));


	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	// get a monospaced font
	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("../../media/fontcourier.bmp");
	if (font)
		skin->setFont(font);

	IGUIStaticText * staticText = env->addStaticText(L"The quick\n\n brown fox\n jumps over the lazy dog", recti(10,10, 400, 100), true, true);
	staticText->setWordWrap(false);

	IGUIProfile * guiProfile = new IGUIProfile(env, env->getRootGUIElement(), -1, core::recti(20, 110, 600, 400));

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			IRR_PROFILE(irr::CProfileScope p1(*irr::getProfiler(), EP_SCOPE, L"scope 1", L"group a"));
			IRR_PROFILE(irr::CProfileScope p2(*irr::getProfiler(), L"scope 2", L"group a"));

			IRR_PROFILE(irr::getProfiler()->start(EP_Y));
			driver->beginScene(true, true, SColor(0,200,200,200));

			env->drawAll();

			driver->endScene();
			IRR_PROFILE(irr::getProfiler()->stop(EP_Y));
		}
	}

	device->drop();

	IRR_PROFILE(irr::getProfiler()->stop(EP_X));

	core::stringw output;
	irr::getProfiler()->printAll(output);
	printf("%s", core::stringc(output).c_str() );

	return 0;
}
