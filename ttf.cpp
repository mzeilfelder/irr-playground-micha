// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Test for CGUITTFont.cpp code from John Norman (+ some changes from myself)
// To get it running run the Makefile with:
// make FREETYPE=1
// (will only work on a system with freetype installed)

#include <irrlicht.h>
#include "CGUITTFont.h"

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
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	
	irr::io::path fontName("my_media/fonts/cyrillic.ttf");
	unsigned int fontSize = 14;
	bool fontAntiAlias = true;
	bool fontTransparency = true;
	irr::gui::CGUITTFont * font = irr::gui::CGUITTFont::createTTFont(driver, device->getFileSystem(), fontName, fontSize, fontAntiAlias, fontTransparency);
	if ( font )
		font->setFontHinting(false, false);
	else
		return 1;

	// load same font twice (just testing if it causes any troubles)
	irr::gui::CGUITTFont * fontAgain = irr::gui::CGUITTFont::createTTFont(driver, device->getFileSystem(), fontName, fontSize, fontAntiAlias, fontTransparency);
	if ( fontAgain )
		fontAgain->drop();
	
	IGUISkin* skin = env->getSkin();
	if ( !skin )
		return 1;

	skin->setFont(font); 
	font->drop();
	
	env->addStaticText( L"The quick brown fox\n jumps over the lazy dog", rect<s32>(20, 20, 320, 60),true);
	IGUIEditBox * editBox = env->addEditBox(L"", rect<s32>(20, 70, 320, 110));

		// load same font with other size
	irr::gui::CGUITTFont * fontOtherSize = irr::gui::CGUITTFont::createTTFont(driver, device->getFileSystem(), fontName, fontSize-4, fontAntiAlias, fontTransparency);
	if ( fontOtherSize )
	{
		editBox->setOverrideFont( fontOtherSize );
		fontOtherSize->drop();
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

