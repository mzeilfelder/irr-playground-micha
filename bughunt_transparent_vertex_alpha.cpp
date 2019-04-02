// About http://irrlicht.sourceforge.net/forum/viewtopic.php?f=4&t=52466
// solved now

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class ILineNode :  public scene::ISceneNode
{
  private:
    core::array<video::S3DVertex> lstVertices;

    core::aabbox3df Box;
    video::SMaterial Material;
    video::IVideoDriver* Driver;

    void createLine(core::vector3df center, f32 lheight, f32 lwidth)
	{
		core::vector3df normal(0,0,1);

		lstVertices.clear();

		Box.reset(center);

		// Set the color
		irr::video::SColor lColor(50, 0, 0xC0, 0);

		// Construct the vertices
		video::S3DVertex vertex;
		vertex = video::S3DVertex(center.X, center.Y, center.Z, normal.X, normal.Y, normal.Z, lColor, 0, 1);
		lstVertices.push_back(vertex);
		Box.addInternalPoint(vertex.Pos);

		vertex = video::S3DVertex(center.X, center.Y - lheight, center.Z, normal.X, normal.Y, normal.Z, lColor, 0, 1);
		lstVertices.push_back(vertex);
		Box.addInternalPoint(vertex.Pos);

		vertex = video::S3DVertex(center.X, center.Y - lheight, center.Z + lwidth, normal.X, normal.Y, normal.Z, lColor, 0, 1);
		lstVertices.push_back(vertex);
		Box.addInternalPoint(vertex.Pos);
	}

  public:
	ILineNode(core::vector3df center, f32 lheight, f32 lwidth, ISceneNode* parent, ISceneManager* smgr, s32 id=-1)
	:	ISceneNode(parent,smgr,id)
	{
		Driver = SceneManager->getVideoDriver();

		createLine(center, lheight, lwidth);

		AutomaticCullingState = irr::scene::EAC_OFF;

		Material.Lighting = false;
		Material.BackfaceCulling = false;
		Material.MaterialType = EMT_TRANSPARENT_VERTEX_ALPHA;
	}

    virtual void OnRegisterSceneNode()
	{
		if(IsVisible)
			SceneManager->registerNodeForRendering(this);
		ISceneNode::OnRegisterSceneNode();
	}

    virtual void render()
	{
		// Prep to render
		Driver->setMaterial(Material);
		Driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		u16 indices[] = { 0,1,2	};

		// render
		Driver->drawVertexPrimitiveList(lstVertices.pointer(), lstVertices.size(), indices, 1, irr::video::EVT_STANDARD, irr::scene::EPT_TRIANGLES, video::EIT_16BIT);
	}

    virtual const core::aabbox3d<f32>& getBoundingBox() const
	{
		return Box;
	}

    virtual u32 getMaterialCount() const
	{
		return 1;
	}

    virtual video::SMaterial& getMaterial(u32 i)
	{
		return Material;
	}

    virtual void setMaterial(video::SMaterial newMaterial)
	{
		Material = newMaterial;
	}
};

int main()
{
    video::E_DRIVER_TYPE driverType = EDT_OPENGL;
    IrrlichtDevice *device = createDevice(driverType,
            core::dimension2d<u32>(960, 540), 32, false);

    if (device == 0)
        return 1; // could not create selected driver.

    video::IVideoDriver* driver = device->getVideoDriver();
    scene::ISceneManager* smgr = device->getSceneManager();

    // add a first person shooter style user controlled camera
    irr::scene::ICameraSceneNode * cameraNode = smgr->addCameraSceneNodeFPS( NULL, 20.0f, 0.02f);
    cameraNode->setPosition(vector3df(0,-10, -30));

	// Some floor
    IAnimatedMesh* hillPlaneMesh = smgr->addHillPlaneMesh( "GROUND",
        core::dimension2d<f32>(10,10),
        core::dimension2d<u32>(200,200), 0, 0,
        core::dimension2d<f32>(0,0),
        core::dimension2d<f32>(10,10));

    ISceneNode* planeNode = smgr->addAnimatedMeshSceneNode(hillPlaneMesh);
    planeNode->setMaterialTexture(0, driver->getTexture("../../media/stones.jpg"));
    planeNode->setMaterialFlag(video::EMF_LIGHTING, false);
    planeNode->setPosition(core::vector3df(0, -50, 0));

	// add sydneys
    IAnimatedMesh* mesh = smgr->getMesh( "../../media/sydney.md2");
    IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );
    if (node)
    {
        node->setMaterialFlag(EMF_LIGHTING, false);
        node->setMD2Animation(scene::EMAT_STAND);
        node->setMaterialTexture( 0, driver->getTexture( "../../media/sydney.bmp") );
    }

	ILineNode* lineNode = new ILineNode(vector3df(20, 25, 0), 250, 150, smgr->getRootSceneNode(), smgr);

    while(device->run())
    {
        driver->beginScene( true, true, video::SColor(255,0xFF,0xFF,0xFF));

        smgr->drawAll();

        driver->endScene();
    }

	lineNode->drop();

    device->drop();

    return 0;
}
