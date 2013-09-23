// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing closing down and restarting a device (with a model in the original device)

#include <irrlicht.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


IrrlichtDevice *device;


class MyEventReceiver: public IEventReceiver {
public:

  MyEventReceiver() {
    switcher = false;
  }

  virtual bool OnEvent(const SEvent& event) {
    // React on GUI events:
    if (event.EventType == EET_GUI_EVENT) {

      switch (event.GUIEvent.EventType) {

      case EGET_BUTTON_CLICKED: {
        s32 buttonId = event.GUIEvent.Caller->getID();
        if (buttonId == 12345) {
          switchResolution();
          return true;
        }
        break;
      }

      default: {
        break;
      }

      }
    }

    return false;
  }

private:

  bool switcher;

  void switchResolution() {
    u32 resolutionX = 800;
    u32 resolutionY = 600;
    if (switcher) {
      resolutionX = 640;
      resolutionY = 480;
    }
    device->getLogger()->log("CLOSING DEVICE TO CHANGE RESOLUTION!");
    device->closeDevice();
    device->clearSystemMessages();
    device->run();
    device->drop();
    device = createDevice(video::EDT_OPENGL,
        dimension2du(resolutionX, resolutionY), 16, false, false, false, this);
    device->getLogger()->log("CREATED DEVICE WITH NEW RESOLUTION!");
    bool runReturn = device->run();
    if (runReturn) {
      device->getLogger()->log("Run returned 'true'.");
    } else {
      device->getLogger()->log("Run returned 'false'.");
    }
  }

};

int main() {

  // create device
  MyEventReceiver receiver;

  device = createDevice(video::EDT_OPENGL, dimension2d<u32>(640, 480), 16,
      false, false, false, &receiver);

  if (!device)
    return 1;

  device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");

  IVideoDriver* driver = device->getVideoDriver();
  ISceneManager* smgr = device->getSceneManager();

  IAnimatedMesh* mesh = smgr->getMesh("../../media/sydney.md2");
  if (!mesh) {
    device->drop();
    return 1;
  }
  IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);

  if (node) {
    node->setMaterialFlag(EMF_LIGHTING, false);
    node->setMD2Animation(scene::EMAT_STAND);
    node->setMaterialTexture(0, driver->getTexture("../../media/sydney.bmp"));
  }

  device->getLogger()->log("Adding camera...");
  smgr->addCameraSceneNode(0, vector3df(0, 30, -40), vector3df(0, 5, 0));

  device->getLogger()->log("Adding button...");
  device->getGUIEnvironment()->addButton(rect<s32>(10, 10, 150, 50), 0, 12345,
      L"Switch Resolution");

  while (device->run()) {
    device->getVideoDriver()->beginScene(true, true, SColor(255, 100, 101, 140));

    device->getSceneManager()->drawAll();
    device->getGUIEnvironment()->drawAll();

    device->getVideoDriver()->endScene();
  }

  device->drop();

  return 0;
}
