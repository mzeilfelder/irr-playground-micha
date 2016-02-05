// Code is under the zlib license (same as Irrlicht)
// Written by mybiandou based on Irrlicht example.
// Simplified by Michael Zeilfelder
// 
// Showing a bug in textscenenodes when used in splitscreen mode
// Also seems to have a memory leak with current svn trunk of Irrlicht

#include <irrlicht.h>
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
 
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
 
bool SplitScreen=true;
ICameraSceneNode *camera[2]={0,0};

class MyEventReceiver : public IEventReceiver
{
    public:
        virtual bool OnEvent(const SEvent& event)
        {
            //Key S enables/disables SplitScreen
            if (event.EventType == irr::EET_KEY_INPUT_EVENT &&
                event.KeyInput.Key == KEY_KEY_S && event.KeyInput.PressedDown)
            {
                SplitScreen = !SplitScreen;
                return true;
            }
            //Send all other events to last camera
            if (camera[1])
                return camera[1]->OnEvent(event);
            return false;
        }
};
 
int main()
{
    MyEventReceiver receiver;
	//video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;
	video::E_DRIVER_TYPE driverType=video::EDT_DIRECT3D9;
	const int ResX=800;
	const int ResY=600;
	IrrlichtDevice *device = createDevice(driverType,
            dimension2du(ResX,ResY), 32, false,
            false, false, &receiver);
    if (!device)
        return 1;
 
    ISceneManager *smgr = device->getSceneManager();
    IVideoDriver *driver = device->getVideoDriver();
 	irr::gui::IGUIEnvironment* gui = device->getGUIEnvironment();
 
    //add text node
    irr::gui::IGUIFont* font = gui->getBuiltInFont();
    ITextSceneNode *txt=smgr->addTextSceneNode(font,L"Irrlicht",irr::video::SColor(100,255,255,255),0,vector3df(0,0,0),-1);
 
	// create cams
    camera[0] = smgr->addCameraSceneNode(0, vector3df(300.f,0,0), vector3df(0,0,0));
    camera[1] = smgr->addCameraSceneNodeFPS();
	camera[1]->setPosition(core::vector3df(-50,0,-50));
   
    while(device->run())
    {
        //Set the viewpoint to the whole screen and begin scene
        driver->setViewPort(rect<s32>(0,0,ResX,ResY));
        driver->beginScene(true,true,SColor(255,100,100,100));

        if (SplitScreen)
        {
            //Set viewpoint to the first quarter (left top)
            smgr->setActiveCamera(camera[0]);
			driver->setViewPort(rect<s32>(0,0,ResX,ResY/2));
            smgr->drawAll();
 
            //Set viewport the last quarter (right bottom)
            driver->setViewPort(rect<s32>(0,ResY/2,ResX,ResY));
        }
        smgr->setActiveCamera(camera[1]);
        smgr->drawAll();
        driver->endScene();
    }
	device->closeDevice();
    device->drop();
    return 0;
}
