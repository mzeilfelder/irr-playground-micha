#include <irrlicht.h>
#include "driverChoice.h"
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
 
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
 
bool SplitScreen=true;
 
//cameras
ICameraSceneNode *camera[4]={0,0,0,0};

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
            //Send all other events to camera4
            if (camera[3])
                return camera[3]->OnEvent(event);
            return false;
        }
};
 
int main()
{
    MyEventReceiver receiver;
	video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;
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
 
    // Create 3 fixed and one user-controlled cameras
    irr::f32 zFar = 300.f;
    camera[0] = smgr->addCameraSceneNode(0, vector3df(zFar,0,0), vector3df(0,0,0));
    camera[1] = smgr->addCameraSceneNode(0, vector3df(0,zFar,0), vector3df(0,0,0));
    camera[2] = smgr->addCameraSceneNode(0, vector3df(0,0,zFar), vector3df(0,0,0));
    camera[3] = smgr->addCameraSceneNodeFPS();
    if (camera[3])
        camera[3]->setPosition(core::vector3df(-50,0,-50));
   
    device->getCursorControl()->setVisible(false);	    //Hide mouse

    while(device->run())
    {
        //Set the viewpoint to the whole screen and begin scene
        driver->setViewPort(rect<s32>(0,0,ResX,ResY));
        driver->beginScene(true,true,SColor(255,100,100,100));
        //If SplitScreen is used
        if (SplitScreen)
        {
            //Activate camera1
            smgr->setActiveCamera(camera[0]);
            //Set viewpoint to the first quarter (left top)
            driver->setViewPort(rect<s32>(0,0,ResX,ResY/2));
            smgr->drawAll();
 
            //Set viewport the last quarter (right bottom)
            driver->setViewPort(rect<s32>(0,ResY/2,ResX,ResY));
        }
        //Activate camera4
        smgr->setActiveCamera(camera[3]);
        //Draw scene
        smgr->drawAll();
        driver->endScene();
    }
    //Delete device
    device->drop();
    return 0;
}
