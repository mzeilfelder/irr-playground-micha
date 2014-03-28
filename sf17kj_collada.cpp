// Testcase for loading materials with Collada which had been exported from Blender
// Written by  sf17k in the Irrlicht forum:
// http://irrlicht.sourceforge.net/forum/viewtopic.php?f=7&t=49694

#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
 
IrrlichtDevice *device;
IVideoDriver *driver;
ISceneManager *smgr;
 
void addMesh(const char *path, vector3df pos, u8 colorMaterial){
    ISceneNode *node = smgr->addMeshSceneNode(smgr->getMesh(path), 0, -1, pos);
    node->setMaterialFlag(EMF_LIGHTING, 1);
    node->getMaterial(0).ColorMaterial = colorMaterial;
}
 
void addVariations(const char *path, float yPos){
    addMesh(path, vector3df(-4.5, yPos, 5), ECM_NONE);
    addMesh(path, vector3df(-1.5, yPos, 5), ECM_DIFFUSE);
    addMesh(path, vector3df(+1.5, yPos, 5), ECM_AMBIENT);
    addMesh(path, vector3df(+4.5, yPos, 5), ECM_DIFFUSE_AND_AMBIENT);
}
 
int main(){
    device = createDevice(EDT_OPENGL, dimension2d<u32>(512, 256));
    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    smgr->addCameraSceneNode();
 
    // white light from above
    smgr->addLightSceneNode(0, vector3df(0,50,-10));
    // bluish ambient
    smgr->setAmbientLight(SColorf(0.1f,0.2f,0.3f));
 
    addVariations("media_sf17kj/no_mat.dae", +1.5f);
    addVariations("media_sf17kj/mat.dae", -1.5f);
 
    while (device->run())
    if (device->isWindowActive()) {
        driver->beginScene(true, true, 0);
        smgr->drawAll();
        driver->endScene();
    }
 
    device->drop();
    return 0;
}
