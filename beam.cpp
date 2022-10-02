// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 

#include <irrlicht.h>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class BeamNode : public irr::scene::ISceneNode
{
public:
	
	BeamNode(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* smgr, irr::s32 id)
		: irr::scene::ISceneNode(parent, smgr, id)
		, End(core::vector3df(500.f, 0.f, 0.f))
		, WidthStart(100.f)
		, WidthEnd(100.f)
	{
		MeshBuffer = new irr::scene::SMeshBuffer();
		MeshBuffer->Indices.set_used(6);
		MeshBuffer->Indices[0] = 0;
		MeshBuffer->Indices[1] = 1;
		MeshBuffer->Indices[2] = 2;
		MeshBuffer->Indices[3] = 3;
		MeshBuffer->Indices[4] = 0;
		MeshBuffer->Indices[5] = 2;
		MeshBuffer->Vertices.set_used(4);

		//Material.BackfaceCulling = false;
		Material.Lighting = false;
		Material.TextureLayer[0].AnisotropicFilter = true;
		Material.TextureLayer[0].TrilinearFilter = true;
	}

	virtual ~BeamNode()
	{
		MeshBuffer->drop();
	}

	//! Set the ruler end-point relative to it's start position
	void setRelativeEnd(const irr::core::vector3df& end)
	{
		End = end;
	}

	//! Width is radius right now...so, it's 2*width
	void setWidthStart(irr::f32 w)
	{
		WidthStart = w;
	}

	void setWidthEnd(irr::f32 w)
	{
		WidthEnd = w;
	}

	void OnRegisterSceneNode() override
	{
		if ( isVisible() )
		{
			SceneManager->isCulled(this);
			SceneManager->registerNodeForRendering(this);
		}
	}

	void render() override
	{
		scene::ICameraSceneNode* camera = SceneManager->getActiveCamera();
		irr::video::IVideoDriver * driver = SceneManager->getVideoDriver();

		refreshMesh(camera);

		driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
		driver->setMaterial(Material);
		driver->drawIndexedTriangleList((const irr::video::S3DVertex*)(MeshBuffer->getVertices()),
											MeshBuffer->getVertexCount(),
											MeshBuffer->getIndices(), MeshBuffer->getIndexCount()/3);

		irr::video::SMaterial m2 = Material;
		m2.Wireframe = true;
		m2.ZBuffer = video::ECFN_DISABLED;
		m2.setTexture(0,0);
		driver->setMaterial(m2);
		driver->drawIndexedTriangleList((const irr::video::S3DVertex*)(MeshBuffer->getVertices()),
											MeshBuffer->getVertexCount(),
											MeshBuffer->getIndices(), MeshBuffer->getIndexCount()/3);
	}

	const irr::core::aabbox3d<irr::f32>& getBoundingBox() const override
	{
		return MeshBuffer->getBoundingBox();
	}

	irr::u32 getMaterialCount() const override
	{
		return 1;
	}

	irr::video::SMaterial& getMaterial(irr::u32 i) override
	{
		return Material;
	}

protected:

	irr::core::vector3df getAbsoluteCenter() const { return getAbsolutePosition() + 0.5f*End; }

	void refreshMesh(const irr::scene::ICameraSceneNode* camera)
	{
		// Always aligns line to camera
		core::vector3df upVector = camera->isOrthogonal() ?  core::vector3df(0,1,0)
			: (camera->getAbsolutePosition() - getAbsoluteCenter() );

		core::vector3df upVectorN(upVector);
		upVectorN.normalize();
		core::vector3df dirN(End);
		dirN.normalize();
 		core::vector3df perp = -upVectorN.crossProduct(dirN);
		perp.normalize();

		const video::SColor LINE_COL(255,255,255,255);

		MeshBuffer->Vertices[0] = video::S3DVertex(perp*WidthStart, upVectorN, LINE_COL, core::vector2df(0,0));
		MeshBuffer->Vertices[1] = video::S3DVertex(End+perp*WidthEnd, upVectorN, LINE_COL, core::vector2df(1,0));
		MeshBuffer->Vertices[2] = video::S3DVertex(End-perp*WidthEnd, upVectorN, LINE_COL, core::vector2df(1,1));
		MeshBuffer->Vertices[3] = video::S3DVertex(-perp*WidthStart, upVectorN, LINE_COL, core::vector2df(0,1));
		MeshBuffer->recalculateBoundingBox();
	}


private:

	irr::core::vector3df End;
	irr::f32 WidthStart;
	irr::f32 WidthEnd;

	irr::scene::SMeshBuffer* MeshBuffer;
	irr::video::SMaterial	 Material;		
};

int main(int argc, char *argv[])
{
	IrrlichtDevice * Device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(1200,600) );
	if (!Device)
		return false;
	
	scene::ISceneManager* smgr = Device->getSceneManager();
	gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();
    video::IVideoDriver* videoDriver =  Device->getVideoDriver();	

	smgr->addCameraSceneNode (0, core::vector3df(0, 0, 0),
								core::vector3df(0, 0, 1),
								-1);

	smgr->addLightSceneNode(0, core::vector3df(0, 0, 0),
							video::SColorf(1.0f, 1.0f, 1.0f),
							100.0f);

	BeamNode * beam = new BeamNode(smgr->getRootSceneNode(), smgr, -1);
	beam->setPosition(core::vector3df(-500, 0, 500));
//	beam->getMaterial(0).setTexture(0, videoDriver->getTexture("../../media/irrlichtlogo.jpg"));
//	beam->getMaterial(0).setTexture(0, videoDriver->getTexture("my_media/rgbwb_cornerblocks_64x64.png"));
	beam->getMaterial(0).setTexture(0, videoDriver->getTexture("my_media/text_color_alpha_512x512.png"));

	while ( Device->run() )
	{
		if ( Device->isWindowActive() )
		{
			//irr::u32 time = Device->getTimer()->getRealTime();
			//beam->setWidthEnd(100.f + (time/50) % 150);
			beam->setWidthEnd(200.f);

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
