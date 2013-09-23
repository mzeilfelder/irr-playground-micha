// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Experiments with text-breaking in Irrlicht gui elements.

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


void AddTestGuiElements(IGUIEnvironment* env, IGUIElement * parent)
{
	core::rect<s32> pos(20,40,20,20);
	core::dimension2du dim1 = env->getSkin()->getFont(EGDF_TOOLTIP)->getDimension(L"a b\n");
	dim1.Width += env->getSkin()->getSize(EGDS_TEXT_DISTANCE_X)*2;
	dim1.Height += env->getSkin()->getSize(EGDS_TEXT_DISTANCE_Y)*2;

	dim1.Width -= 1;	// this should lead to wrapping

	pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + dim1.Height-1;
	pos.LowerRightCorner.X = pos.UpperLeftCorner.X + dim1.Width;
	env->addStaticText(L"a b\n", pos, true, true, parent, -1, true);
	pos.UpperLeftCorner.X += 200;
	pos.LowerRightCorner.X += 200;
	env->addStaticText(L"a b", pos, true, true, parent, -1, true);
	pos.UpperLeftCorner.Y += 200;
	pos.LowerRightCorner.Y += 200;
	pos.LowerRightCorner.X -= env->getSkin()->getSize(EGDS_TEXT_DISTANCE_X)*2;
	env->addStaticText(L"a b", pos, false, true, parent, -1, true);
	pos.UpperLeftCorner.X -= 200;
	pos.LowerRightCorner.X -= 200;
	env->addStaticText(L"a b\n", pos, false, true, parent, -1, true);
	
	env->addComboBox(rect<s32>(20,320,120,340))->setToolTipText(L"In windowed mode, the resolution will not be\nlarger than the desktop resolution.");
}

int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	AddTestGuiElements(env, 0);

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
