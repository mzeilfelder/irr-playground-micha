// Originally written by Sérgio Augusto Vianna
// Somewhat re-written by Michael Zeilfelder
// No license as it was just a bug-report.
// But you can use the workaround with the "Do What the Fuck You Want To Public License" (WTFPL)
// 
// Code shows a bug in Toolbar placement in Irrlicht 1.8
// Irrlicht does set it below the modal dialog pseudo element - so the toolbar is outside the window.

#include <irrlicht.h>

using namespace irr;
using namespace video;
using namespace core;
using namespace gui;

class LynxEventReceiver: public IEventReceiver
{
	public:
	bool OnEvent(const SEvent& event)
	{
		switch (event.EventType)
		{
			case EET_GUI_EVENT:
			{

				switch (event.GUIEvent.EventType)
				{
					case EGET_FILE_SELECTED:
					{
						IGUIToolBar * tb = gui->addToolBar();
						
						// Workaround needed up to Irrlicht 1.8 (will fix it for 1.9)
						core::rect<s32> rr =tb->getRelativePosition();
						rr.LowerRightCorner.Y -= rr.UpperLeftCorner.Y;
						rr.UpperLeftCorner.Y = 0;
						tb->setRelativePosition(rr);


						break;
					}

					default:
					{
					}
				}
				break;
			}
			default:
			{
			}
		}
		return false;
  }

	IGUIEnvironment* gui = 0;
};

int main() {

	LynxEventReceiver receiver;

	SIrrlichtCreationParameters params = SIrrlichtCreationParameters();
	params.Bits = 32;
	params.EventReceiver = &receiver;
	params.Fullscreen = false;
	params.Stencilbuffer = true;
	params.DriverType = EDT_OPENGL;
	params.WindowSize = dimension2du(1024, 768);
	IrrlichtDevice *device = createDeviceEx(params);
	if (!device)
		return 0;

	IGUIEnvironment* gui = device->getGUIEnvironment();
	receiver.gui = gui;

	//gui->addToolBar();	// this one shows up

	gui->addFileOpenDialog();

	stringw windowName(L"Toolbar bug test");
	device->setWindowCaption(windowName.c_str());

	IVideoDriver* driver = device->getVideoDriver();

	while (device->run())
	{
		driver->beginScene(true, true, SColor(255, 0, 0, 0));

		device->getGUIEnvironment()->drawAll();

		driver->endScene();
	}

	device->drop();

	return 0;
}
