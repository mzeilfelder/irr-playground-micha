// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Testing lineCount.patch.
// That should have been a quick-fix, but to my surprise stuff worked a little different inside Irrlicht
// that I had expected. So have to get back to this when I have more time and find the correct solution
// And more important - check if the current solution really always works correct in Irrlicht - it's 
// a little strange that statictext can be several lines without WordWrap but there are WordWrap checks
// in getTextHeight. This looks wrong on first view. My first guess is that IGUIFont::draw always breaks text.
// How to fix that without breaking interfaces... needs more time.

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _MSC_VER
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

	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("my_media/fonts/droid_sans12.xml");
	if (font)
		skin->setFont(font);

	IGUIStaticText * staticText = env->addStaticText(L"The quick\n\n brown fox\n jumps over the lazy dog", recti(10,10, 400, 100), true, true);
	staticText->setWordWrap(false);
	IGUIEditBox * editBox = env->addEditBox (L"", recti(10,110, 400, 150) );
	editBox->setWordWrap(false);
	editBox->setMultiLine(false);
	IGUIListBox * listBox = env->addListBox (recti(10,160, 400, 300), 0, -1, true);
	u32 lineCountStaticOld = 0;
	u32 lineCountEditOld = 0;

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			u32 lineCountStatic = staticText->getLineCount();
			u32 lineCountEdit = editBox->getLineCount();
			if ( lineCountStaticOld != lineCountStatic || lineCountEdit != lineCountEditOld )
			{
				lineCountStaticOld = lineCountStatic;
				lineCountEditOld = lineCountEdit;
				listBox->clear();

				core::stringw str(L"static: ");
				str += core::stringw(lineCountStatic);
				listBox->addItem(str.c_str());
				for ( u32 i=0; i < lineCountStatic; ++i )
					listBox->addItem(staticText->getLine(i));

				str = L"edit: ";
				str += core::stringw(lineCountEdit);
				listBox->addItem(str.c_str());
				for ( u32 i=0; i < lineCountEdit; ++i )
					listBox->addItem(editBox->getLine(i));
			}
			else
			{
				for ( u32 i=0; i < lineCountStatic; ++i )
					listBox->setItem(i+1, staticText->getLine(i), 0);
				for ( u32 i=0; i < lineCountEdit; ++i )
					listBox->setItem(i+2+lineCountStatic, editBox->getLine(i), 0);
			}

			driver->beginScene(true, true, SColor(0,200,200,200));

			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
