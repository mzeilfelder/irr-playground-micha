// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Test for IGUIImages

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

enum EAutoStates
{
	AS_LEFT,
	AS_RIGHT,
	AS_TOP,
	AS_BOTTOM,
	AS_LEFT_RIGHT,
	AS_TOP_BOTTOM,
	AS_ALL_DIRS,
	AS_VERTICAL,
	AS_HORIZONTAL,
	AS_COUNT
};

struct SAppContext
{
	IrrlichtDevice * Device;
	IGUISpinBox * SpinLeft;
	IGUISpinBox * SpinTop;
	IGUISpinBox * SpinRight;
	IGUISpinBox * SpinBottom;
	IGUIButton * ButtonAutoBounds;
	array<IGUIImage*> Images;
	EAutoStates AutoState;
	u32 StateStartTime;
};


class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			switch(event.GUIEvent.EventType)
			{
				case EGET_SPINBOX_CHANGED:
				{
					for ( u32 i=0; i< Context.Images.size(); ++i )
					{
						rectf bounds(Context.SpinLeft->getValue(), Context.SpinTop->getValue()
									, Context.SpinRight->getValue(), Context.SpinBottom->getValue());
						Context.Images[i]->setDrawBounds(bounds);
					}
				}
			default:
				break;
			}
		}

		return false;
	}

private:
	SAppContext & Context;
};

void autoUpdate(SAppContext & Context)
{
	if ( !Context.ButtonAutoBounds->isPressed() )
		return;

	ITimer * timer = Context.Device->getTimer ();
	u32 timeNow = timer->getTime();
	if ( timeNow - Context.StateStartTime > 5000 )
	{
		Context.StateStartTime = timeNow;
		Context.AutoState = EAutoStates(((int)Context.AutoState+1)%(int)AS_COUNT);
	}

	u32 timeDiff = timeNow - Context.StateStartTime;
	irr::f32 f = (irr::f32)(timeDiff%1000)/1000.f;
	if ( (timeDiff/1000)%2 == 1 )
		f = 1.f - f;

	rectf bounds(0.f,0.f,1.f,1.f);
	switch ( Context.AutoState )
	{
		case AS_LEFT:
			bounds.UpperLeftCorner.X = f;
			break;
		case AS_RIGHT:
			bounds.LowerRightCorner.X = 1.f-f;
			break;
		case AS_TOP:
			bounds.UpperLeftCorner.Y = f;
			break;
		case AS_BOTTOM:
			bounds.LowerRightCorner.Y = 1.f-f;
			break;
		case AS_LEFT_RIGHT:
			bounds.UpperLeftCorner.X = f;
			bounds.LowerRightCorner.X = 1.f-f;
			break;
		case AS_TOP_BOTTOM:
			bounds.UpperLeftCorner.Y = f;
			bounds.LowerRightCorner.Y = 1.f-f;
			break;
		case AS_ALL_DIRS:
			bounds.UpperLeftCorner.X = f;
			bounds.LowerRightCorner.X = 1.f-f;
			bounds.UpperLeftCorner.Y = f;
			bounds.LowerRightCorner.Y = 1.f-f;
			break;
		case AS_VERTICAL:
			if ( f > 0.9f )
				f = 0.9f;
			bounds.UpperLeftCorner.X = f;
			bounds.LowerRightCorner.X = f+0.1f;
			break;
		case AS_HORIZONTAL:
			if ( f > 0.9f )
				f = 0.9f;
			bounds.UpperLeftCorner.Y = f;
			bounds.LowerRightCorner.Y = f+0.1f;
			break;
		case AS_COUNT:
			break;
	}

	Context.SpinLeft->setValue(bounds.UpperLeftCorner.X);
	Context.SpinTop->setValue(bounds.UpperLeftCorner.Y);
	Context.SpinRight->setValue(bounds.LowerRightCorner.X);
	Context.SpinBottom->setValue(bounds.LowerRightCorner.Y);

	for ( u32 i=0; i< Context.Images.size(); ++i )
	{
		Context.Images[i]->setDrawBounds(bounds);
	}
}


int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(1000, 800));
	if (device == 0)
		return 1; // could not create selected driver.

	SAppContext context;
	context.Device = device;
	context.AutoState = AS_LEFT;
	context.StateStartTime = device->getTimer()->getTime();
	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);

	// irrlichtlogo2.png is 128x128

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	IGUIImage * image = NULL;

	// first row - scaled images with source-rect modified
	s32 top = 0;
	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(0, top));
	image->setScaleImage(true);

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(130, top));
	image->setScaleImage(true);
	image->setSourceRect(recti(10, 10, 60, 60));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(260, top));
	image->setScaleImage(true);
	image->setSourceRect(recti(0, 0, 500, 500));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(390, top));
	image->setScaleImage(true);
	image->setSourceRect(recti(-50, -50, 50, 50));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(520, top));
	image->setScaleImage(true);
	image->setSourceRect(recti(50, 50, 0, 0));	// illegal source rect (still works with scale it seems...)

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(650, top));
	image->setScaleImage(true);
	image->setSourceRect(recti(128, 0, 0, 128));	// illegal source rect	(still works with scale it seems...)

	// second row - unscaled images with source-rect modified - texture same size as image
	top += 130;
	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(0, top));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(130, top));
	image->setSourceRect(recti(10, 10, 60, 60));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(260, top));
	image->setSourceRect(recti(0, 0, 500, 500));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(390, top));
	image->setSourceRect(recti(-50, -50, 50, 50));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(520, top));
	image->setSourceRect(recti(50, 50, 0, 0));	// illegal source rect

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(650, top));
	image->setSourceRect(recti(128, 0, 0, 128));	// illegal source rect

	// third row - unscaled images with source-rect modified - texture larger than the image
	top += 130;
	irr::core::position2di size(80, 80);
	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(0, top));
	image->setRelativePosition( core::recti(image->getRelativePosition().UpperLeftCorner, image->getRelativePosition().UpperLeftCorner + size) );

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(130, top));
	image->setRelativePosition( core::recti(image->getRelativePosition().UpperLeftCorner, image->getRelativePosition().UpperLeftCorner + size) );
	image->setSourceRect(recti(10, 10, 60, 60));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(260, top));
	image->setRelativePosition( core::recti(image->getRelativePosition().UpperLeftCorner, image->getRelativePosition().UpperLeftCorner + size) );
	image->setSourceRect(recti(0, 0, 500, 500));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(390, top));
	image->setRelativePosition( core::recti(image->getRelativePosition().UpperLeftCorner, image->getRelativePosition().UpperLeftCorner + size) );
	image->setSourceRect(recti(-50, -50, 50, 50));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(520, top));
	image->setRelativePosition( core::recti(image->getRelativePosition().UpperLeftCorner, image->getRelativePosition().UpperLeftCorner + size) );
	image->setSourceRect(recti(50, 50, 0, 0));	// illegal source rect

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(650, top));
	image->setRelativePosition( core::recti(image->getRelativePosition().UpperLeftCorner, image->getRelativePosition().UpperLeftCorner + size) );
	image->setSourceRect(recti(128, 0, 0, 128));	// illegal source rect

	// fourth row - unscaled images with source-rect modified - image larger than the texture
	top += 90;
	size = irr::core::position2di (150, 160);

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(0, top));
	image->setRelativePosition( core::recti(image->getRelativePosition().UpperLeftCorner, image->getRelativePosition().UpperLeftCorner + size) );

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(160, top));
	image->setRelativePosition( core::recti(image->getRelativePosition().UpperLeftCorner, image->getRelativePosition().UpperLeftCorner + size) );
	image->setSourceRect(recti(10, 10, 60, 60));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(320, top));
	image->setRelativePosition( core::recti(image->getRelativePosition().UpperLeftCorner, image->getRelativePosition().UpperLeftCorner + size) );
	image->setSourceRect(recti(0, 0, 500, 500));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(480, top));
	image->setRelativePosition( core::recti(image->getRelativePosition().UpperLeftCorner, image->getRelativePosition().UpperLeftCorner + size) );
	image->setSourceRect(recti(-50, -50, 50, 50));

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(640, top));
	image->setRelativePosition( core::recti(image->getRelativePosition().UpperLeftCorner, image->getRelativePosition().UpperLeftCorner + size) );
	image->setSourceRect(recti(50, 50, 0, 0));	// illegal source rect

	image = env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),	core::position2d<s32>(800, top));
	image->setRelativePosition( core::recti(image->getRelativePosition().UpperLeftCorner, image->getRelativePosition().UpperLeftCorner + size) );
	image->setSourceRect(recti(128, 0, 0, 128));// illegal source rect

	// fifth row - test new draw background flag
	top += 170;
	image = env->addImage(core::recti(0, top, 64, top+64), 0, -1, L"text?", true);
	image->setDrawBackground(false);	// so will not show up as all

	image = env->addImage(core::recti(160, top, 224, top+64), 0, -1, L"text?", true);

	// get all IGUIImages
	const auto& rootChildren = env->getRootGUIElement ()->getChildren();
	for ( auto it = rootChildren.begin(); it != rootChildren.end(); ++it )
	{
		if ( (*it)-> hasType(EGUIET_IMAGE) )
			context.Images.push_back(static_cast<IGUIImage*>(*it));
	}


	top += 70;
	const f32 stepSize = 0.1f;
	const f32 rangeMin = -0.5f;
	const f32 rangeMax = 1.5f;
	context.SpinLeft = env->addSpinBox (L"0", core::recti(0,top+20, 50, top+40));
	context.SpinLeft->setStepSize(stepSize);
	context.SpinLeft->setRange (rangeMin, rangeMax);
	context.SpinTop = env->addSpinBox (L"0", core::recti(50,top, 100, top+20));
	context.SpinTop->setStepSize(stepSize);
	context.SpinTop->setRange (rangeMin, rangeMax);
	context.SpinRight = env->addSpinBox (L"1", core::recti(100,top+20, 150, top+40));
	context.SpinRight->setStepSize(stepSize);
	context.SpinRight->setRange (rangeMin, rangeMax);
	context.SpinBottom = env->addSpinBox (L"1", core::recti(50,top+40, 100, top+60));
	context.SpinBottom->setStepSize(stepSize);
	context.SpinBottom->setRange (rangeMin, rangeMax);

	context.ButtonAutoBounds = env->addButton(recti(200, top, 250, top+40), 0, -1, L"auto", L"enjoy the show");
	context.ButtonAutoBounds->setIsPushButton(true);

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			autoUpdate(context);

			driver->beginScene(true, true, SColor(0,200,200,200));

			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
