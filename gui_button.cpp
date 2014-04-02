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

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

struct SAppContext
{
	IrrlichtDevice * Device;
	IGUICheckBox * CbScale;
	IGUICheckBox * CbAlpha;
	IGUICheckBox * CbBorder;
	IGUICheckBox * CbPushButton;
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
					if ( event.GUIEvent.Caller == Context.CbScale )
					{
						for ( u32 i=0; i< Context.Buttons.size(); ++i )
						{
							Context.Buttons[i]->setScaleImage( Context.CbScale->isChecked() );
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
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
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
	
	// dynamic options
	context.CbScale = env->addCheckBox (false, recti(10, 10, 100, 30), 0, -1, L"scale");
	context.CbAlpha = env->addCheckBox (false, recti(110, 10, 200, 30), 0, -1, L"alpha");
	context.CbBorder = env->addCheckBox (true, recti(210, 10, 300, 30), 0, -1, L"border");
	context.CbPushButton = env->addCheckBox (false, recti(310, 10, 400, 30), 0, -1, L"pushbutton");
	
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
	btn = env->addButton(recti(10, 50, 138, 82), 0, -1, L"image+pressed");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	btn->setPressedImage( driver->getTexture("my_media/btn_down.png") );
	
	btn = env->addButton(recti(140, 50, 268, 82), 0, -1, L"image");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	
	// create the buttons with images (old style) - larger than textures
	btn = env->addButton(recti(10, 100, 266, 164), 0, -1, L"image+pressed");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	btn->setPressedImage( driver->getTexture("my_media/btn_down.png") );
	
	btn = env->addButton(recti(300, 100, 556, 164), 0, -1, L"image");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	
	// create the buttons with images (old style) - smaller than textures
	btn = env->addButton(recti(10, 170, 70, 190), 0, -1, L"image+pressed");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );
	btn->setPressedImage( driver->getTexture("my_media/btn_down.png") );
	
	btn = env->addButton(recti(80, 170, 160, 190), 0, -1, L"image");
	btn->setImage( driver->getTexture("my_media/btn_up.png") );	

	// create the buttons with sprites
	btn = env->addButton(recti(10, 200, 138, 232), 0, -1, L"all states");
	btn->setSprite(EGBS_BUTTON_UP, 0);
	btn->setSprite(EGBS_BUTTON_DOWN, 1);
	btn->setSprite(EGBS_BUTTON_MOUSE_OVER, 2);
	btn->setSprite(EGBS_BUTTON_MOUSE_OFF, 3);
	btn->setSprite(EGBS_BUTTON_FOCUSED, 4);
	btn->setSprite(EGBS_BUTTON_NOT_FOCUSED, 5);
	btn->setSprite(EGBS_BUTTON_DISABLED, 6);
	
	btn = env->addButton(recti(140, 200, 268, 232), 0, -1, L"only button up");
	btn->setSprite(EGBS_BUTTON_UP, 0);

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
