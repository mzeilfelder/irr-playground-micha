// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing button functions

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

struct SAppContext
{
	IrrlichtDevice * Device;
	IGUICheckBox * CbScaleImage;
	IGUICheckBox * CbAlpha;
	IGUICheckBox * CbBorder;
	IGUICheckBox * CbPushButton;
	IGUICheckBox * CbEnabled;
	IGUICheckBox * CbScaleSprite;
	array<IGUIButton*> Buttons;
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
				case EGET_CHECKBOX_CHANGED :
				{
					if ( event.GUIEvent.Caller == Context.CbScaleImage )
					{
						for ( u32 i=0; i< Context.Buttons.size(); ++i )
						{
							Context.Buttons[i]->setScaleImage( Context.CbScaleImage->isChecked() );
						}
					}
					else if ( event.GUIEvent.Caller == Context.CbAlpha )
					{
						for ( u32 i=0; i< Context.Buttons.size(); ++i )
						{
							Context.Buttons[i]->setUseAlphaChannel( Context.CbAlpha->isChecked() );
						}
					}
					else if ( event.GUIEvent.Caller == Context.CbBorder )
					{
						for ( u32 i=0; i< Context.Buttons.size(); ++i )
						{
							Context.Buttons[i]->setDrawBorder( Context.CbBorder->isChecked() );
						}
					}
					else if ( event.GUIEvent.Caller == Context.CbPushButton )
					{
						for ( u32 i=0; i< Context.Buttons.size(); ++i )
						{
							Context.Buttons[i]->setIsPushButton( Context.CbPushButton->isChecked() );
						}
					}
					else if ( event.GUIEvent.Caller == Context.CbEnabled )
					{
						for ( u32 i=0; i< Context.Buttons.size(); ++i )
						{
							Context.Buttons[i]->setEnabled( Context.CbEnabled->isChecked() );
						}
					}
					else if ( event.GUIEvent.Caller == Context.CbScaleSprite )
					{
						for ( u32 i=0; i< Context.Buttons.size(); ++i )
						{
							for ( u32 s=0; s < (u32)EGBS_COUNT; ++s )
							{
								Context.Buttons[i]->setSprite( (EGUI_BUTTON_STATE)s,
									Context.Buttons[i]->getSpriteIndex((EGUI_BUTTON_STATE)s),
									Context.Buttons[i]->getSpriteColor((EGUI_BUTTON_STATE)s),
									Context.Buttons[i]->getSpriteLoop((EGUI_BUTTON_STATE)s),
									Context.CbScaleSprite->isChecked() );
							}
						}
					}
				}
				break;
			default:
				break;
			}
		}

		return false;
	}
	
private:
	SAppContext & Context;	
};

int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(800, 600));
	if (device == 0)
		return 1; // could not create selected driver.
	
	SAppContext context;
	context.Device = device;
	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);	

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	IGUISkin * skin = env->getSkin();
	skin->setSize(EGDS_BUTTON_PRESSED_IMAGE_OFFSET_X, 5 );
	skin->setSize(EGDS_BUTTON_PRESSED_IMAGE_OFFSET_Y, 1 );
	skin->setSize(EGDS_BUTTON_PRESSED_TEXT_OFFSET_X, 5 );
	skin->setSize(EGDS_BUTTON_PRESSED_TEXT_OFFSET_Y, 10 );
	skin->setSize(EGDS_BUTTON_PRESSED_SPRITE_OFFSET_X, 1);
	skin->setSize(EGDS_BUTTON_PRESSED_SPRITE_OFFSET_Y, 1);
	
	// dynamic options
	context.CbScaleImage = env->addCheckBox (false, recti(10, 10, 100, 30), 0, -1, L"scale image");
	context.CbAlpha = env->addCheckBox (false, recti(110, 10, 200, 30), 0, -1, L"alpha");
	context.CbBorder = env->addCheckBox (true, recti(210, 10, 300, 30), 0, -1, L"border");
	context.CbPushButton = env->addCheckBox (false, recti(310, 10, 400, 30), 0, -1, L"pushbutton");
	context.CbEnabled = env->addCheckBox (true, recti(410, 10, 500, 30), 0, -1, L"enabled");
	context.CbScaleSprite = env->addCheckBox (false, recti(510, 10, 600, 30), 0, -1, L"scale sprite");
	
	// create a spritebank
	gui::IGUISpriteBank * buttonSpriteBank = env->addEmptySpriteBank(io::path("buttons"));
	buttonSpriteBank->addTexture( driver->getTexture("my_media/btn_sprites.png") );
	core::array< recti >& spritePositions = buttonSpriteBank->getPositions();
	spritePositions.push_back( recti(0,0, 128, 32) );
	spritePositions.push_back( recti(0,32, 128, 64) );
	spritePositions.push_back( recti(0,64, 128, 96) );
	spritePositions.push_back( recti(0,96, 128, 128) );
	spritePositions.push_back( recti(0,128, 128, 160) );
	spritePositions.push_back( recti(0,160, 128, 192) );
	spritePositions.push_back( recti(0,192, 128, 224) );
	core::array< SGUISprite >& sprites = buttonSpriteBank->getSprites();
	sprites.push_back( SGUISprite(SGUISpriteFrame(0, 0)) );
	sprites.push_back( SGUISprite(SGUISpriteFrame(0, 1)) );
	sprites.push_back( SGUISprite(SGUISpriteFrame(0, 2)) );
	sprites.push_back( SGUISprite(SGUISpriteFrame(0, 3)) );
	sprites.push_back( SGUISprite(SGUISpriteFrame(0, 4)) );
	sprites.push_back( SGUISprite(SGUISpriteFrame(0, 5)) );
	sprites.push_back( SGUISprite(SGUISpriteFrame(0, 6)) );

	// create the buttons with images (old style) - same size as textures
	IGUIButton * btn = 0;
	s32 top = 50;
	btn = env->addButton(recti(10, top, 138, top+32), 0, -1, L"image+pressed");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	btn->setPressedImage( driver->getTexture("my_media/btn_down.png") );
	
	btn = env->addButton(recti(140, top, 268, top+32), 0, -1, L"image");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	
	btn = env->addButton(recti(270, top, 398, top+32), 0, -1, L"rect");
	btn->setImage( driver->getTexture("my_media/btn_up.png"), recti(0,0,64,16) );
	btn->setPressedImage( driver->getTexture("my_media/btn_down.png"), recti(0,0,64,16) );
	
	// create the buttons with images (old style) - larger than textures
	top += 40;
	btn = env->addButton(recti(10, top, 266, top+64), 0, -1, L"image+pressed");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	btn->setPressedImage( driver->getTexture("my_media/btn_down.png") );
	
	btn = env->addButton(recti(270, top, 526, top+64), 0, -1, L"image");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	
	btn = env->addButton(recti(530, top, 786, top+64), 0, -1, L"rect");
	btn->setImage( driver->getTexture("my_media/btn_up.png"), recti(0,0,64,16) );
	btn->setPressedImage( driver->getTexture("my_media/btn_down.png"), recti(0,0,64,16) );
	
	// create the buttons with images (old style) - smaller than textures
	top += 70;
	btn = env->addButton(recti(10, top, 70, top+20), 0, -1, L"image+pressed");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	btn->setPressedImage( driver->getTexture("my_media/btn_down.png") );
	
	btn = env->addButton(recti(80, top, 160, top+20), 0, -1, L"image");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );	
	
	btn = env->addButton(recti(170, top, 230, top+20), 0, -1, L"rect");
	btn->setImage( driver->getTexture("my_media/btn_up.png"), recti(0,0,64,16) );
	btn->setPressedImage( driver->getTexture("my_media/btn_down.png"), recti(0,0,64,16) );

	// create the buttons with sprites
	top += 30;
	btn = env->addButton(recti(10, top, 138, top+32), 0, -1, L"all states");
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_DOWN, 1);
	btn->setSprite(EGBS_BUTTON_MOUSE_OVER, 2);
	btn->setSprite(EGBS_BUTTON_MOUSE_OFF, 3);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);
	btn->setSprite(EGBS_BUTTON_NOT_FOCUSED, 5);
	btn->setSprite(EGBS_BUTTON_DISABLED, 6);
	
	btn = env->addButton(recti(140, top, 268, top+32), 0, -1, L"button up and focused");
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);
	
	btn = env->addButton(recti(280, top, 408, top+32), 0, -1, L"only button up");
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);	
	
	// button with sprites  - larger than textures
	top += 40;
	btn = env->addButton(recti(10, top, 266, top+64), 0, -1, L"all states");
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_DOWN, 1);
	btn->setSprite(EGBS_BUTTON_MOUSE_OVER, 2);
	btn->setSprite(EGBS_BUTTON_MOUSE_OFF, 3);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);
	btn->setSprite(EGBS_BUTTON_NOT_FOCUSED, 5);
	btn->setSprite(EGBS_BUTTON_DISABLED, 6);
	
	btn = env->addButton(recti(270, top, 526, top+64), 0, -1, L"button up and focused");
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);
	
	btn = env->addButton(recti(530, top, 786, top+64), 0, -1, L"only button up");
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);	
	
	// button with sprites - smaller than textures
	top += 70;
	btn = env->addButton(recti(10, top, 70, top+20), 0, -1, L"all states");
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_DOWN, 1);
	btn->setSprite(EGBS_BUTTON_MOUSE_OVER, 2);
	btn->setSprite(EGBS_BUTTON_MOUSE_OFF, 3);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);
	btn->setSprite(EGBS_BUTTON_NOT_FOCUSED, 5);
	btn->setSprite(EGBS_BUTTON_DISABLED, 6);
	
	btn = env->addButton(recti(80, top, 140, top+20), 0, -1, L"button up and focused");
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);
	
	btn = env->addButton(recti(150, top, 210, top+20), 0, -1, L"only button up");
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);	
	
	// create the buttons with images (old style) and sprites - all kind of sizes
	top += 30;
	btn = env->addButton(recti(10, top, 138, top+32), 0, -1, L"all states");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	btn->setPressedImage( driver->getTexture("my_media/btn_down.png") );
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_DOWN, 1);
	btn->setSprite(EGBS_BUTTON_MOUSE_OVER, 2);
	btn->setSprite(EGBS_BUTTON_MOUSE_OFF, 3);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);
	btn->setSprite(EGBS_BUTTON_NOT_FOCUSED, 5);
	btn->setSprite(EGBS_BUTTON_DISABLED, 6);

	btn = env->addButton(recti(140, top, 396, top+64), 0, -1, L"all states");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	btn->setPressedImage( driver->getTexture("my_media/btn_down.png") );
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_DOWN, 1);
	btn->setSprite(EGBS_BUTTON_MOUSE_OVER, 2);
	btn->setSprite(EGBS_BUTTON_MOUSE_OFF, 3);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);
	btn->setSprite(EGBS_BUTTON_NOT_FOCUSED, 5);
	btn->setSprite(EGBS_BUTTON_DISABLED, 6);

	btn = env->addButton(recti(410, top, 470, top+20), 0, -1, L"all states");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	btn->setPressedImage( driver->getTexture("my_media/btn_down.png") );
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_DOWN, 1);
	btn->setSprite(EGBS_BUTTON_MOUSE_OVER, 2);
	btn->setSprite(EGBS_BUTTON_MOUSE_OFF, 3);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);
	btn->setSprite(EGBS_BUTTON_NOT_FOCUSED, 5);
	btn->setSprite(EGBS_BUTTON_DISABLED, 6);
	
	// create buttons with images (new style) - larger
	top += 70;
	btn = env->addButton(recti(10, top, 160, top+50), 0, -1, L"all images");
	btn->setImage(EGBIS_IMAGE_UP, driver->getTexture("my_media/btn_up.png") );
	btn->setImage(EGBIS_IMAGE_UP_MOUSEOVER, driver->getTexture("my_media/btn_up_over.png") );
	btn->setImage(EGBIS_IMAGE_UP_FOCUSED, driver->getTexture("my_media/btn_up_focused.png") );
	btn->setImage(EGBIS_IMAGE_UP_FOCUSED_MOUSEOVER, driver->getTexture("my_media/btn_up_focused_over.png") );
	btn->setImage(EGBIS_IMAGE_DOWN, driver->getTexture("my_media/btn_down.png") );
	btn->setImage(EGBIS_IMAGE_DOWN_MOUSEOVER, driver->getTexture("my_media/btn_down_over.png") );
	btn->setImage(EGBIS_IMAGE_DOWN_FOCUSED, driver->getTexture("my_media/btn_down_focused.png") );
	btn->setImage(EGBIS_IMAGE_DOWN_FOCUSED_MOUSEOVER, driver->getTexture("my_media/btn_down_focused_over.png") );
	btn->setImage(EGBIS_IMAGE_DISABLED, driver->getTexture("my_media/btn_disabled.png") );
	
	btn = env->addButton(recti(170, top, 320, top+50), 0, -1, L"img  up/down/disabled/down_focused_over");
	btn->setImage(EGBIS_IMAGE_UP, driver->getTexture("my_media/btn_up.png") );
	btn->setImage(EGBIS_IMAGE_DOWN, driver->getTexture("my_media/btn_down.png") );
	btn->setImage(EGBIS_IMAGE_DOWN_FOCUSED_MOUSEOVER, driver->getTexture("my_media/btn_down_focused_over.png") );
	btn->setImage(EGBIS_IMAGE_DISABLED, driver->getTexture("my_media/btn_disabled.png") );
	
	btn = env->addButton(recti(330, top, 480, top+50), 0, -1, L"img up + rect");
	btn->setImage(EGBIS_IMAGE_UP, driver->getTexture("my_media/btn_up.png"), recti(0,0,64,16) );


	

	// get all IGUIButtons and set the spritebank
	const auto& rootChildren = env->getRootGUIElement ()->getChildren();
	for ( auto it = rootChildren.begin(); it != rootChildren.end(); ++it )
	{
		if ( (*it)-> hasType(EGUIET_BUTTON) )
		{
			context.Buttons.push_back(static_cast<IGUIButton*>(*it));
			context.Buttons.getLast()->setSpriteBank(buttonSpriteBank);
		}
	}

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
