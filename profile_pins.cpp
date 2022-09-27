// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Similar to profile_meshscenenode.cpp for now, but want to try some other experiments with this one.
// You can pass 3 parameters for the number of nodes created in x, y and z direction
// So you have x*y*z nodes in the end

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
// Basic idea is to test how much faster it can get when knowing exactly what is needed (and also testing which flexibility costs most)
// - single material per mesh instance
// - not animated
// - no transparency
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

irr::video::ITexture* addColorTexture(video::IVideoDriver * videoDriver, irr::video::SColor c)
{
	irr::video::SColor bgra(c.getBlue(), c.getGreen(), c.getRed(), c.getAlpha());	// ECF_A8R8G8B8 is BGRA in memory
	irr::video::ITexture* result = 0;
	irr::core::dimension2du dim(16,16);
	size_t num = dim.Width*dim.Height;
	irr::video::SColor* data = new irr::video::SColor[num];
	for ( size_t i=0; i < num; ++i )
	{
		data[i] = bgra;
	}
	irr::video::IImage* img = videoDriver->createImageFromData(irr::video::ECF_A8R8G8B8, dim, (void*)data, true, false);
	result = videoDriver->addTexture(irr::io::path(c.color), img);
	delete img;
	delete[] data;
	return result;
}


int main(int argc, char* argv[])
{
	IrrlichtDevice *  device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800,600));
	if (!device)
		return 0;

	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver * videoDriver = device->getVideoDriver ();
	videoDriver->setMinHardwareBufferVertexCount(10);
	IRandomizer* randomizer = device->getRandomizer();

#if 1 // using some loaded mesh (a pin model)
	scene::IAnimatedMesh* aniMesh = smgr->getMesh( "my_media/pin.obj" );
	if (!aniMesh)
		return 0;
	scene::IMesh * mesh = aniMesh->getMesh (0);
#else	// using cubes
	scene::IMesh * mesh = smgr->getGeometryCreator()->createCubeMesh();
#endif
	if ( !mesh )
		return 0;
	mesh->setHardwareMappingHint(scene::EHM_STATIC);

	// Amount of 3d objects to create
	int nodesX = 100;
	int nodesY = 5;
	int nodesZ = 100;

	// can also pass them from command-line
	if ( argc > 1 )
		nodesX = atoi(argv[1]);
	if ( argc > 2 )
		nodesY = atoi(argv[2]);
	if ( argc > 3 )
		nodesZ = atoi(argv[3]);

	// at least 1
	if ( nodesX < 1 )
		nodesX = 1;
	if ( nodesY < 1 )
		nodesY = 1;
	if ( nodesZ < 1 )
		nodesZ = 1;

	// Make sure there's some gaps between each of them
	aabbox3df bbox = mesh->getBoundingBox();
	vector3df extent = bbox.getExtent();
	const f32 GAP = 10.f;
	f32 halfSizeX = 0.5f * (nodesX*extent.X + GAP*(nodesX-1));
	f32 halfSizeY = 0.5f * (nodesY*extent.Y + GAP*(nodesY-1));
	f32 halfSizeZ = 0.5f * (nodesZ*extent.Z + GAP*(nodesZ-1));

	// Some colors - we later use randomly one of them per mesh
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

//	videoDriver->setTextureCreationFlag(video::ETCF_SUPPORT_VERTEXT_TEXTURE, true);

	// Some textures - can be used randomly later
	irr::core::array<irr::video::ITexture*> Textures;
	for ( irr::u32 i=0;i<Colors.size();++i)
		Textures.push_back( addColorTexture(videoDriver, Colors[i]) );

#if 0	// using nodes
	SimpleMeshArray * arrayNode = NULL;
#else	// using a single array for all meshes
	SimpleMeshArray * arrayNode = new SimpleMeshArray(mesh, smgr->getRootSceneNode(), smgr);
	if ( arrayNode )
	{
		arrayNode->setAutomaticCulling(0);
	}
#endif

	// creating the 3d objects
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

					//for ( irr::u32 t = 0; t < irr::video::MATERIAL_MAX_TEXTURES_USED; ++t )
					//	instance.Material.setTexture(t, Textures[randomizer->rand() % Textures.size()]);

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

	// Test how much sorting by material improves things (less state-switches for material changes)
	if ( arrayNode )
	{
		arrayNode->sortInstancesByEmissiveColor();
	}

	// some light (for testing with/without lighting)
	/*scene::ILightSceneNode* nodeLight = */smgr->addLightSceneNode(0, core::vector3df(0, 300, 0),
														video::SColorf(1.0f, 1.0f, 1.0f),
														500.0f);

	// just a simple fps camera
	irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.5f );
	camera->updateAbsolutePosition();
	camera->setTarget( vector3df(0,0,0) );
	camera->updateAbsolutePosition();
	camera->setPosition(irr::core::vector3df(halfSizeX+extent.X, halfSizeY+extent.Y, halfSizeZ+extent.Z));
	camera->updateAbsolutePosition();

	s32 oldFPS = 0;
	while ( device->run() )
	{
		if ( device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			smgr->drawAll();

			videoDriver->endScene();

			// update information about current frame-rate
			s32 fps = videoDriver->getFPS();
			if ( fps != oldFPS )	// we only update when it changes as calling setWindowCaption can be really slow (too lazy to make a gui right now...)
			{
				oldFPS = fps;
				core::stringw str(L"FPS: ");
				str.append(core::stringw(fps));
				str += L" Tris: ";
				str.append(core::stringw(videoDriver->getPrimitiveCountDrawn()));
				device->setWindowCaption( str.c_str() );
			}
		}
		else
		{
			device->sleep( 5 );
		}
	}

	device->closeDevice();
	device->drop();
	device = NULL;

	// only useful when you profile Irrlicht internals
	core::stringw output;
	getProfiler().printAll(output);
	printf("%s", core::stringc(output).c_str());


	return 0;
}
