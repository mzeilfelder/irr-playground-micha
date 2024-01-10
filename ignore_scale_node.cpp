// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Add scene-node in between which prevents that the parent-node's scaling is added to the child.
// NOTE: Since Irrlicht 1.9 ISceneNode has a flag to ignore parent transformations

#include <irrlicht.h>
 
using namespace irr;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

// Can be placed in betweeen parent-child to ignore the scale of the parent.
class IgnoreScaleSceneNode : public irr::scene::ISceneNode
{
public:

	//! Constructor
	IgnoreScaleSceneNode(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, s32 id=-1)
		: irr::scene::ISceneNode(parent, mgr, id) {}

	virtual void updateAbsolutePosition()
	{
		AbsoluteTransformation.makeIdentity();
		if (Parent)
		{
			AbsoluteTransformation.setRotationDegrees( Parent->getRotation() );
			AbsoluteTransformation.setTranslation( Parent->getPosition() );
			if ( Parent->getParent() )
			{
				AbsoluteTransformation = Parent->getParent()->getAbsoluteTransformation() * AbsoluteTransformation;
			}
		}
	}
	
	//! returns the axis aligned bounding box of this node
	virtual const core::aabbox3d<f32>& getBoundingBox() const
	{
		return Box;
	}


	//! Returns the relative transformation of the scene node.
	virtual core::matrix4 getRelativeTransformation() const
	{
		return RelativeTransformationMatrix;
	}

	//! does nothing.
	virtual void render() _IRR_OVERRIDE_ {}
		
private:
	irr::core::matrix4 RelativeTransformationMatrix;	// identity matrix
	irr::core::aabbox3d<f32> Box;
};



 
int main()
{
    IrrlichtDevice * Device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(1280,1024) );
    if (!Device)
        return false;
 
    scene::ISceneManager* smgr = Device->getSceneManager();
    gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();
    video::IVideoDriver* videoDriver =  Device->getVideoDriver();  
    smgr->setAmbientLight(video::SColorf(0.4f, 0.4f, 0.35f, 0.0f));
    smgr->addLightSceneNode(0, core::vector3df(0, 2000, -1000), video::SColorf(0.7f, 0.7f, 0.4f, 1.0f), 1000);
    smgr->addCameraSceneNodeFPS(0, 200.f, 0.1f );
 
    irr::core::vector3df scale(2.f, 1.f, 0.3f);
 
    irr::scene::ISceneNode * parent = smgr->addCubeSceneNode (10.0f, 0, -1,
        core::vector3df(0, 0, 100),
        core::vector3df(0, 0, 0),
        scale);
    parent->getMaterial(0).Lighting = true;
    parent->getMaterial(0).AmbientColor = video::SColor(0, 200, 0, 0);
 
    irr::scene::ISceneNode * child = smgr->addCubeSceneNode (30.0f, 0, -1,
        core::vector3df(0, 0, 50),
        core::vector3df(0, 10, 0),
        core::vector3df(1.f, 1.f, 1.f));
    child->getMaterial(0).Lighting = true;
    child->getMaterial(0).AmbientColor  = video::SColor(0, 0, 200, 0);
	
	IgnoreScaleSceneNode* ignoreNode = new IgnoreScaleSceneNode(parent, smgr);
    child->setParent(ignoreNode);
	ignoreNode->drop();
 
    while ( Device->run() )
    {
        if ( Device->isWindowActive() )
        {
            videoDriver->beginScene(true, true);
 
            smgr->drawAll();
            guiEnv->drawAll();
 
            videoDriver->endScene();
        }
        Device->sleep( 1 );
    }
 
    Device->closeDevice();
    Device->drop();
 
    return 0;
}
