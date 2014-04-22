// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// profiler

#include <irrlicht.h>

#include "profiler.h"

namespace irr
{

	Profiler gPROFILER;

	IRRLICHT_API Profiler* IRRCALLCONV getProfiler()
	{
		return &gPROFILER;
	}
} // namespace irr


class ShowProfiler
{
public:
	ShowProfiler() : DisplayRect(20, 40, 600, 400), DisplayList(0), CurrentGroupIdx(0)
	{
	}

    // set the rectangle used for the display listbox
    void setDisplayRect(const irr::core::rect<irr::s32> &rect_)
	{
		DisplayRect = rect_;
	}

	// print current display group on screen
	void show(irr::gui::IGUIEnvironment* env_)
	{
		if ( !env_)
			return;

		hide(env_);

		// I had no table yet when programming this. Would probably be nicer.
		DisplayList = env_->addListBox(DisplayRect, 0, -1, true);

		irr::core::stringw wstrTitle(irr::ProfileData::makeTitleString());
		DisplayList->addItem(wstrTitle.c_str());

		irr::core::stringw wstrGroup(irr::getProfiler()->getGroupData(CurrentGroupIdx).getAsString());
		DisplayList->addItem(wstrGroup.c_str());

		// show overview over groups?
		if ( CurrentGroupIdx == 0 )
		{
			for ( irr::u32 i=1; i<irr::getProfiler()->getGroupCount(); ++i )
			{
				irr::core::stringw wstrData(irr::getProfiler()->getGroupData(i).getAsString());
				DisplayList->addItem(wstrData.c_str());
			}
		}
		// show data for current group
		else
		{
			const irr::ProfileData& data = irr::getProfiler()->getGroupData(CurrentGroupIdx);
			irr::core::stringw wstrData(data.getAsString());
			DisplayList->addItem(wstrData.c_str());
		}
	}

	void hide(irr::gui::IGUIEnvironment* env_)
	{
		if ( !env_)
			return;

		if ( DisplayList )
		{
			DisplayList->remove();
			DisplayList = 0;
		}
	}

	void nextDisplayGroup()
	{
		if ( ++CurrentGroupIdx >= irr::getProfiler()->getGroupCount() )
			CurrentGroupIdx = 0;
	}

	void previousDisplayGroup()
	{
		if ( CurrentGroupIdx > 0 )
			--CurrentGroupIdx;
		else
			CurrentGroupIdx = irr::getProfiler()->getGroupCount()-1;
	}

	void firstDisplayGroup()
	{
		CurrentGroupIdx = 0;
	}

protected:
    irr::core::rect<irr::s32> DisplayRect;
	irr::gui::IGUIListBox* DisplayList;
    irr::u32 CurrentGroupIdx;
};


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
		EP_Y
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

	ShowProfiler showProfiler;
	showProfiler.setDisplayRect(core::recti(20, 110, 600, 400));
//	showProfiler.nextDisplayGroup();

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			showProfiler.show(env);

			IRR_PROFILE(irr::getProfiler()->start(EP_Y));
			driver->beginScene(true, true, SColor(0,200,200,200));

			env->drawAll();

			driver->endScene();
			IRR_PROFILE(irr::getProfiler()->stop(EP_Y));
		}
	}

	device->drop();

	IRR_PROFILE(irr::getProfiler()->stop(EP_X));

	return 0;
}
