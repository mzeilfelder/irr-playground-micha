// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing messageboxes.

#include <irrlicht.h>
#include <iostream>

using namespace irr;

void AddMessageBoxes(video::IVideoDriver* driver, gui::IGUIEnvironment* env)
{
	video::ITexture * tex64 = driver->getTexture("my_media/64x64.jpg");
	env->addMessageBox (L"", L"", /*modal=*/false, /*s32 flags=*/gui::EMBF_OK, /*IGUIElement *parent=*/0, /*s32 id=*/-1, tex64);
	env->addMessageBox (L"Caption", L"text.", /*modal=*/false, /*s32 flags=*/gui::EMBF_OK, /*IGUIElement *parent=*/0, /*s32 id=*/-1, tex64);
	env->addMessageBox (L"Caption", L"text.\nLine two\n. Line looooooooooooooooooooooooooooooooooooooooooooong it still gooooooooes on and on and on and on!", /*modal=*/false, /*s32 flags=*/gui::EMBF_OK | gui::EMBF_YES | gui::EMBF_NO, /*IGUIElement *parent=*/0, /*s32 id=*/-1, tex64);
	env->addMessageBox (L"Caption", L"text many lines.\nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nlast line", /*modal=*/false, /*s32 flags=*/gui::EMBF_OK, /*IGUIElement *parent=*/0, /*s32 id=*/-1, tex64);	
	
	video::ITexture * tex420 = driver->getTexture("my_media/420x300.jpg");
	env->addMessageBox (L"Caption", L"text.\nLine two\n. Line three.", /*modal=*/false, /*s32 flags=*/gui::EMBF_OK, /*IGUIElement *parent=*/0, /*s32 id=*/-1, tex420);
	
	env->addMessageBox (L"Caption", L"text.", /*modal=*/false, /*s32 flags=*/gui::EMBF_OK, /*IGUIElement *parent=*/0, /*s32 id=*/-1, 0);	
	env->addMessageBox (L"Caption", L"text.\nLine two\n. Line three.", /*modal=*/false, /*s32 flags=*/gui::EMBF_OK, /*IGUIElement *parent=*/0, /*s32 id=*/-1, 0);	
	env->addMessageBox (L"Caption", L"text.\nLine two\n. Line looooooooooooooooooooooooooooooooooooooooooooong it still gooooooooes on and on and on and on!", /*modal=*/false, /*s32 flags=*/gui::EMBF_OK  | gui::EMBF_YES | gui::EMBF_NO, /*IGUIElement *parent=*/0, /*s32 id=*/-1, 0);
	env->addMessageBox (L"Caption", L"text many lines.\nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nLine \nlast line", /*modal=*/false, /*s32 flags=*/gui::EMBF_OK, /*IGUIElement *parent=*/0, /*s32 id=*/-1, 0);	
}

int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;

	IrrlichtDevice* device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	gui::IGUISkin * skin = env->getSkin();	
	
	skin->setSize(gui::EGDS_MESSAGE_BOX_GAP_SPACE, 5);
	
	skin->setSize(gui::EGDS_MESSAGE_BOX_MIN_TEXT_WIDTH, 150 );
	skin->setSize(gui::EGDS_MESSAGE_BOX_MAX_TEXT_WIDTH, 300);
	skin->setSize(gui::EGDS_MESSAGE_BOX_MIN_TEXT_HEIGHT, 100);
	skin->setSize(gui::EGDS_MESSAGE_BOX_MAX_TEXT_HEIGHT, 1200);
	
	AddMessageBoxes(driver, env);
	
	env->saveGUI("messageboxes.gui");
	env->clear();
	env->loadGUI("messageboxes.gui");

	while(device->run())
	{
		driver->beginScene(true, true, 0 );

		env->drawAll();

		driver->endScene();	   
	}

	device->drop();
	return 0;
}
