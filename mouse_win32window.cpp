// Slightly modified example 014 Win32 Window from Irrlicht examples.
// License is still zlib
//
// Added some output for position to figure out how coordinates are handled when Irrlicht uses
// other windows for rendering (should maybe clean this up a little and add similar information in the official example).
// Also added some button to see how/when the Irrlicht gui still works.

#include <irrlicht.h>
#ifndef _IRR_WINDOWS_
#error Windows only example
#else
#include <windows.h> // this example only runs with windows
#include <iostream>
#include "driverChoice.h"

using namespace irr;

#pragma comment(lib, "irrlicht.lib")

HWND hOKButton;
HWND hWnd;

static LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			HWND hwndCtl = (HWND)lParam;
			int code = HIWORD(wParam);

			if (hwndCtl == hOKButton)
			{
				DestroyWindow(hWnd);
				PostQuitMessage(0);
				return 0;
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

class MyEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_MOUSE_INPUT_EVENT)
		{
			mousePos = L"X: ";
			mousePos += irr::core::stringw(event.MouseInput.X);
			mousePos += L"Y: ";
			mousePos += irr::core::stringw(event.MouseInput.Y);
		}

		return false;
	}

	irr::core::stringw mousePos;
};



/*
   Now ask for the driver and create the Windows specific window.
*/
int main()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;

	printf("Select the render window (some dead window may exist too):\n"\
		" (a) Window with button (via CreationParam)\n"\
		" (b) Window with button (via beginScene)\n"\
		" (c) Own Irrlicht window (default behavior)\n"\
		" (otherKey) exit\n\n");

	char key;
	std::cin >> key;
	if (key != 'a' && key != 'b' && key != 'c')
		return 1;

	HINSTANCE hInstance = 0;
	// create dialog

	const char* Win32ClassName = "CIrrlichtWindowsTestDialog";

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)CustomWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= DLGWINDOWEXTRA;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= Win32ClassName;
	wcex.hIconSm		= 0;

	RegisterClassEx(&wcex);

	DWORD style = WS_SYSMENU | WS_BORDER | WS_CAPTION |
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX;

	int windowWidth = 440;
	int windowHeight = 380;

	hWnd = CreateWindow( Win32ClassName, "Irrlicht Win32 window example",
		style, 100, 100, windowWidth, windowHeight,
		NULL, NULL, hInstance, NULL);

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	windowWidth = clientRect.right;
	windowHeight = clientRect.bottom;

	// create ok button

	hOKButton = CreateWindow("BUTTON", "OK - Close", WS_CHILD | WS_VISIBLE | BS_TEXT,
		windowWidth - 160, windowHeight - 40, 150, 30, hWnd, NULL, hInstance, NULL);

	// create some text

	CreateWindow("STATIC", "This is Irrlicht running inside a standard Win32 window.\n"\
		"Also mixing with MFC and .NET Windows.Forms is possible.",
		WS_CHILD | WS_VISIBLE, 20, 20, 400, 40, hWnd, NULL, hInstance, NULL);

	// create window to put irrlicht in

	HWND hIrrlichtWindow = CreateWindow("BUTTON", "",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			50, 80, 320, 220, hWnd, NULL, hInstance, NULL);
	video::SExposedVideoData videodata((key=='b')?hIrrlichtWindow:0);

	/*
	So now that we have some window, we can create an Irrlicht device
	inside of it. We use Irrlicht createEx() function for this. We only
	need the handle (HWND) to that window, set it as windowsID parameter
	and start up the engine as usual. That's it.
	*/
	// create irrlicht device in the button window

	MyEventReceiver eventReceiver;
	irr::SIrrlichtCreationParameters param;
	param.EventReceiver = &eventReceiver;
	param.DriverType = driverType;
	if (key=='a')
		param.WindowId = reinterpret_cast<void*>(hIrrlichtWindow);

	irr::IrrlichtDevice* device = irr::createDeviceEx(param);

	// setup a simple 3d scene

	irr::scene::ISceneManager* smgr = device->getSceneManager();
	irr::gui::IGUIEnvironment* guiEnv = device->getGUIEnvironment();
	video::IVideoDriver* driver = device->getVideoDriver();
	irr::gui::ICursorControl* cursorControl = device->getCursorControl();
	guiEnv->getSkin()->setFont( guiEnv->getFont(irr::io::path(L"../../media/bigfont.png")) );

	guiEnv->addButton( irr::core::recti( 20, 20, 200, 100), 0, -1, L"click", L"do no thang");
	irr::gui::IGUIStaticText * cursorPos = guiEnv->addStaticText(L"", irr::core::recti( 1, 110, 800, 160), true );
	irr::gui::IGUIStaticText * mouseEventPos = guiEnv->addStaticText(L"", irr::core::recti( 1, 170, 800, 220), true );

	if (driverType==video::EDT_OPENGL)
	{
		HDC HDc=GetDC(hIrrlichtWindow);
		PIXELFORMATDESCRIPTOR pfd={0};
		pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
		int pf = GetPixelFormat(HDc);
		DescribePixelFormat(HDc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		pfd.dwFlags |= PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.cDepthBits=16;
		pf = ChoosePixelFormat(HDc, &pfd);
		SetPixelFormat(HDc, pf, &pfd);
		videodata.OpenGLWin32.HDc = HDc;
		videodata.OpenGLWin32.HRc=wglCreateContext(HDc);
		wglShareLists((HGLRC)driver->getExposedVideoData().OpenGLWin32.HRc, (HGLRC)videodata.OpenGLWin32.HRc);
	}
	scene::ICameraSceneNode* cam = smgr->addCameraSceneNode();
	cam->setTarget(core::vector3df(0,0,0));

	scene::ISceneNodeAnimator* anim =
		smgr->createFlyCircleAnimator(core::vector3df(0,15,0), 30.0f);
	cam->addAnimator(anim);
	anim->drop();

	scene::ISceneNode* cube = smgr->addCubeSceneNode(20);

	cube->setMaterialTexture(0, driver->getTexture("../../media/wall.bmp"));
	cube->setMaterialTexture(1, driver->getTexture("../../media/water.jpg"));
	cube->setMaterialFlag( video::EMF_LIGHTING, false );
	cube->setMaterialType( video::EMT_REFLECTION_2_LAYER );

	smgr->addSkyBoxSceneNode(
	driver->getTexture("../../media/irrlicht2_up.jpg"),
	driver->getTexture("../../media/irrlicht2_dn.jpg"),
	driver->getTexture("../../media/irrlicht2_lf.jpg"),
	driver->getTexture("../../media/irrlicht2_rt.jpg"),
	driver->getTexture("../../media/irrlicht2_ft.jpg"),
	driver->getTexture("../../media/irrlicht2_bk.jpg"));

	// show and execute dialog

	ShowWindow(hWnd , SW_SHOW);
	UpdateWindow(hWnd);

	while (device->run())
	{
		driver->beginScene(true, true, 0, videodata);
		irr::core::vector2di mousePos = cursorControl->getPosition();
		irr::core::stringw caption;
		caption += L"X: ";
		caption += irr::core::stringw(mousePos.X);
		caption += L"Y: ";
		caption += irr::core::stringw(mousePos.Y);
		device->setWindowCaption(caption.c_str());
		cursorPos->setText(caption.c_str());
		mouseEventPos->setText(eventReceiver.mousePos.c_str());
		smgr->drawAll();
		guiEnv->drawAll();
		driver->endScene();
	}

	device->closeDevice();
	device->drop();

	return 0;
}
#endif // if windows

/*
That's it, Irrlicht now runs in your own windows window.
**/
