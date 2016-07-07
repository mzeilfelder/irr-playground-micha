// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Test for CGUITTFont.cpp code from John Norman (+ some changes from myself)
// To get it running run the Makefile with:
// make FREETYPE=1
// (will only work on a system with freetype installed)
//
// Open problem: 
// - When ETCF_ALLOW_MEMORY_COPY is not set in CGUITTFont.h then each character typed in the editbox which wasn't used before makes the characters less visible (maybe more bright)
// - setTransparency(false) makes no difference

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
//	video::E_DRIVER_TYPE driverType = video::EDT_SOFTWARE;	
//	video::E_DRIVER_TYPE driverType = video::EDT_BURNINGSVIDEO;
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	//irr::io::path fontName("my_media/fonts/cyrillic.ttf");
	irr::io::path fontName("my_media/fonts/andika/Andika-R.ttf");
	unsigned int fontSize = 14;
	bool fontAntiAlias = true;
	bool fontTransparency = true;
	irr::gui::CGUITTFont * font = irr::gui::CGUITTFont::createTTFont(driver, device->getFileSystem(), fontName, fontSize, fontAntiAlias, fontTransparency);
	if ( font )
	{
		font->setFontHinting(false, false);
		font->setTransparency(false);
	}
	else
		return 1;

	
#if 0	// load same font twice (just testing if it causes any troubles)
	irr::gui::CGUITTFont * fontAgain = irr::gui::CGUITTFont::createTTFont(driver, device->getFileSystem(), fontName, fontSize, fontAntiAlias, fontTransparency);
	if ( fontAgain )
		fontAgain->drop();
#endif
	
	IGUISkin* skin = env->getSkin();
	if ( !skin )
		return 1;

	skin->setFont(font); 
	font->drop();
	
	//skin->setFont(env->getFont("my_media/fonts/gentium12px.xml"));	// non-ttf font for comparison	
	core::stringw str(L"the quick brown fox\n jumps over äöü the lazy dog\nTHE LAZY DOG JUMPS OVER\nTHE QUICK BROWN FOX");
	env->addStaticText( str.c_str(), rect<s32>(20, 20, 320, 120),true);
	IGUIEditBox * editBox = env->addEditBox(L"", rect<s32>(20, 130, 320, 210));

#if 0	// load same font with other size
	irr::gui::CGUITTFont * fontOtherSize = irr::gui::CGUITTFont::createTTFont(driver, device->getFileSystem(), fontName, fontSize-4, fontAntiAlias, fontTransparency);
	if ( fontOtherSize )
	{
		editBox->setOverrideFont( fontOtherSize );
		fontOtherSize->drop();
	}
#endif

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, SColor(0,200,200,200));
	
			env->drawAll();
			
			const recti position(20, 220, 320, 320);
			font->draw(str, position, SColor(255,10,20,30));
			
			// TODO: drawing texture-pages not yet working - at least not with opengl. Maybe something about colors being messed up - not sure yet.
			core::position2di texPos(330, 10);
			u32 numPages = font->getLastGlyphPageIndex()+1;
			for ( u32 i=0; i < numPages; ++i )
			{
				ITexture* tex = font->getPageTextureByIndex(i);
				driver->draw2DImage(tex, texPos);
				texPos.Y += tex->getSize().Height + 10;
			}
		
			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
