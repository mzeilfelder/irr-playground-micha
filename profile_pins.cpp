// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Similar to profile_meshscenenode.cpp for now, but want to try some other experiments with this one.
// You can pass 3 parameters for the number of nodes created in x, y and z direction
// So have x*y*z nodes in the end

#include <irrlicht.h>
#include <iostream>
#include <cassert>
 
using namespace irr;
using namespace core;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

// TODO: try with sorting materials

// Show a simple mesh several times.
// Bassic idea is to test how much faster it can get when knowing exacly what is needed (and also testing which flexibility costs most)
// - single material per mesh instance
// - not animated
// - no transpareny
// - parent transformations ignored
class SimpleMeshArray : public scene::ISceneNode
{
public:

	struct Instance
	{
		irr::video::SMaterial Material;
		irr::core::matrix4 Transform;
		
		bool operator < (const Instance& other) const
		{
			return (Material.EmissiveColor < other.Material.EmissiveColor);
		}
	
	};

	SimpleMeshArray(scene::IMesh * mesh, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id=-1)
		: scene::ISceneNode(parent, mgr, id)
		, Mesh(0)
	{
		setMesh(mesh);
	}

	virtual void OnRegisterSceneNode()
	{
		if (IsVisible && Mesh)
		{
			SceneManager->registerNodeForRendering(this, scene::ESNRP_SOLID);
		}

		ISceneNode::OnRegisterSceneNode();
	}

	virtual void render()
	{
		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		//driver->setMaterial(Instances[0].Material);	// test with setting just one material for all
		// scene::IMeshBuffer* mb = Mesh->getMeshBuffer(0);
		
		for ( u32 i=0; i < Instances.size(); ++i )
		{
			driver->setTransform(video::ETS_WORLD, Instances[i].Transform);
			driver->setMaterial(Instances[i].Material);
			
			//driver->drawMeshBuffer(mb);
			for (u32 m=0; m<Mesh->getMeshBufferCount(); ++m)
			{
				scene::IMeshBuffer* mb = Mesh->getMeshBuffer(m);
				driver->drawMeshBuffer(mb);
			}
		}
	}

	virtual const core::aabbox3d<f32>& getBoundingBox() const
	{
		return Box;
	}

	virtual u32 getMaterialCount() const
	{
		return Instances.size();
	}

	virtual video::SMaterial& getMaterial(u32 i)
	{
		return Instances[i].Material;
	}	
	
	//! Sets a new mesh to display
	void setMesh(irr::scene::IMesh* mesh)
	{
		if ( mesh != Mesh )
		{
			if ( mesh )
				mesh->grab();
			if ( Mesh )
			{
				Mesh->drop();
			}
			Mesh = mesh;
			if ( Mesh )
			{
				Box = Mesh->getBoundingBox(); // not correct yet - have to modify in addInstance				
				//std::cout << "meshbuffers: " << Mesh->getMeshBufferCount() << "\n";
			}
		}
	}

	//! Get the currently defined mesh for display.
	irr::scene::IMesh* getMesh(void)
	{
		return Mesh;
	}
	
	irr::u32 addInstance(const Instance& instance)
	{
		Instances.push_back(instance);
		return Instances.size()-1;
	}
	
	void sortInstancesByEmissiveColor()
	{
		Instances.sort();
	}
	
private:
	scene::IMesh * Mesh;
	core::aabbox3d<f32> Box;
	core::array<Instance> Instances;
};


 
int main(int argc, char* argv[])
{
	IrrlichtDevice *  device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800,600));
	if (!device)
		return 0;
   
	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver * videoDriver = device->getVideoDriver ();
	videoDriver->setMinHardwareBufferVertexCount(10);
	IRandomizer* randomizer = device->getRandomizer();

	scene::IAnimatedMesh* aniMesh = smgr->getMesh( "my_media/pin.obj" );
	if (!aniMesh)
		return 0;
	scene::IMesh * mesh = aniMesh->getMesh (0);
	if ( !mesh )
		return 0;
	mesh->setHardwareMappingHint(scene::EHM_STATIC);
	
	int nodesX = 100;
	int nodesY = 1;
	int nodesZ = 100;

	if ( argc > 1 )
		nodesX = atoi(argv[1]);
	if ( argc > 2 )
		nodesY = atoi(argv[2]);
	if ( argc > 3 )
		nodesZ = atoi(argv[3]);
	if ( nodesX < 1 )
		nodesX = 1;
	if ( nodesY < 1 )
		nodesY = 1;
	if ( nodesZ < 1 )
		nodesZ = 1;
	
	aabbox3df bbox = mesh->getBoundingBox();
	vector3df extent = bbox.getExtent();
	const f32 GAP = 10.f;
	f32 halfSizeX = 0.5f * (nodesX*extent.X + GAP*(nodesX-1));
	f32 halfSizeY = 0.5f * (nodesY*extent.Y + GAP*(nodesY-1));
	f32 halfSizeZ = 0.5f * (nodesZ*extent.Z + GAP*(nodesZ-1));

	irr::core::array<irr::video::SColor> Colors;
	Colors.push_back( irr::video::SColor(255, 0,   0,  139) );
	Colors.push_back( irr::video::SColor(255, 0,   0,  255) );
	Colors.push_back( irr::video::SColor(255, 0,  255, 255) );
	Colors.push_back( irr::video::SColor(255, 64,  64, 64) );
	Colors.push_back( irr::video::SColor(255, 128,128, 128) );
	Colors.push_back( irr::video::SColor(255, 0,  100,  0) );
	Colors.push_back( irr::video::SColor(255, 0,  255,  0) );
	Colors.push_back( irr::video::SColor(255, 192, 192,192) );
	Colors.push_back( irr::video::SColor(255, 255, 0,  255) );
	Colors.push_back( irr::video::SColor(255, 255, 192,128) );
	Colors.push_back( irr::video::SColor(255, 255, 176,176) );
	Colors.push_back( irr::video::SColor(255, 139, 0,   0) );
	Colors.push_back( irr::video::SColor(255, 255, 0,   0) );
	Colors.push_back( irr::video::SColor(255, 255, 255, 0) );
	Colors.push_back( irr::video::SColor(255, 255, 255, 255) );
	Colors.push_back( irr::video::SColor(255, 165, 42, 42) );
	Colors.push_back( irr::video::SColor(255, 255, 215, 0) );
	
	SimpleMeshArray * arrayNode = NULL;
	//SimpleMeshArray * arrayNode = new SimpleMeshArray(mesh, smgr->getRootSceneNode(), smgr);
	if ( arrayNode )
	{
		arrayNode->setAutomaticCulling(0);
	}
		
	for ( int x = 0; x < nodesX; ++x )
	{
		irr::f32 gapX = x > 0 ? (x-1)*GAP : 0.f;
		irr::f32 posX = -halfSizeX + x*extent.X + gapX;
		for ( int y = 0; y < nodesY; ++y )
		{
			irr::f32 gapY = y > 0 ? (y-1)*GAP : 0.f;
			irr::f32 posY = -halfSizeY + y*extent.Y + gapY;
			for ( int z=0; z < nodesZ; ++z )
			{
				irr::f32 gapZ = z > 0 ? (z-1)*GAP : 0.f;
				irr::f32 posZ = -halfSizeZ + z*extent.Z + gapZ;
				irr::video::SColor randCol = Colors[ randomizer->rand() % Colors.size() ];
				//irr::video::SColor randCol = video::SColor(randomizer->rand());
				
				if ( arrayNode )
				{
					SimpleMeshArray::Instance instance;
					instance.Material.EmissiveColor = randCol;
					instance.Transform.setTranslation( vector3df(posX, posY, posZ) );
					arrayNode->addInstance(instance);
				}
				else
				{
					scene::IMeshSceneNode * node = smgr->addMeshSceneNode (mesh, NULL, -1, vector3df(posX, posY, posZ) );
					//node->setMaterialFlag(video::EMF_LIGHTING, false);
					node->getMaterial(0).EmissiveColor = randCol;
				}
			}
		}
	}
	
	if ( arrayNode )
	{
		arrayNode->sortInstancesByEmissiveColor();
	}
	
	/*scene::ILightSceneNode* nodeLight = */smgr->addLightSceneNode(0, core::vector3df(0, 300, 0),
														video::SColorf(1.0f, 1.0f, 1.0f),
														500.0f); 
	
	irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.5f );
	camera->updateAbsolutePosition();
	camera->setTarget( vector3df(0,0,0) );
	camera->updateAbsolutePosition();
	camera->setPosition(irr::core::vector3df(halfSizeX+extent.X, halfSizeY+extent.Y, halfSizeZ+extent.Z));
	camera->updateAbsolutePosition();

	while ( device->run() )
	{
		if ( device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			smgr->drawAll();

			videoDriver->endScene();
			
			// update information about current frame-rate
			core::stringw str(L"FPS: ");
			str.append(core::stringw(videoDriver->getFPS()));
			str += L" Tris: ";
			str.append(core::stringw(videoDriver->getPrimitiveCountDrawn()));
			device->setWindowCaption( str.c_str() );
		}

		device->sleep( 5 );
	}

	device->closeDevice();
	device->drop();
	device = NULL;
	
	core::stringw output;
	getProfiler().printAll(output);
	printf("%s", core::stringc(output).c_str());
	

	return 0;
}