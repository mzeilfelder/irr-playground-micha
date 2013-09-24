// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing collada export with custom export settings.

#include <irrlicht.h>
#include <cassert>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

const io::path COLLADA_FILENAME("irrExport.dae");

//! Callback interface for properties which can be used to influence collada writing
//! To see the default settings take a look at CColladaMeshWriterProperties in CColladaMeshWriter.cpp in Irrlicht sources.
class MyColladaMeshWriterProperties  : public irr::scene::IColladaMeshWriterProperties
{
public:

	// Different tools want their colladas with different flavors
	enum EExportTarget
	{
		MITSUBA,
		MAX3D,
	};

	MyColladaMeshWriterProperties::MyColladaMeshWriterProperties(IVideoDriver* driver, irr::scene::IColladaMeshWriterProperties * defaultProperties)
		: DefaultProperties(defaultProperties), ExportTarget(MITSUBA), Driver(driver)
	{
		assert(DefaultProperties);
	}

	//! target tool
	void setExportTarget(EExportTarget t)	
	{ 
		ExportTarget = t; 
	}

	//! Which lighting model should be used in the technique (FX) section when exporting effects (materials)
	virtual irr::scene::E_COLLADA_TECHNIQUE_FX getTechniqueFx(const irr::video::SMaterial& material) const
	{
		// Different tools support different types here
		if ( isTransparent(material) )
		{
			if ( ExportTarget == MAX3D )
				return ECTF_BLINN;
			return ECTF_CONSTANT;
		}
		return ECTF_PHONG;
	}

	//! Which texture index should be used when writing the texture of the given sampler color.
	virtual irr::s32 getTextureIdx(const irr::video::SMaterial & material, irr::scene::E_COLLADA_COLOR_SAMPLER cs) const
	{
		return 0;
//		return DefaultProperties->getTextureIdx(material, cs);
	}

	//! Return which color from Irrlicht should be used for the color requested by collada
	virtual irr::scene::E_COLLADA_IRR_COLOR getColorMapping(const irr::video::SMaterial & material, irr::scene::E_COLLADA_COLOR_SAMPLER cs) const
	{
		if ( ExportTarget == MAX3D && cs == ECCS_TRANSPARENT && isTransparent(material) )
			return ECIC_CUSTOM;

		if(cs == ECCS_EMISSIVE)
			return ECIC_NONE;
	
		return DefaultProperties->getColorMapping(material, cs);
	}

	//! Return custom colors for certain color types requested by collada. 
	virtual irr::video::SColor getCustomColor(const irr::video::SMaterial & material, irr::scene::E_COLLADA_COLOR_SAMPLER cs) const
	{
		if ( ExportTarget == MAX3D && cs == ECCS_TRANSPARENT && isTransparent(material) )
			return video::SColor(255,255,255,255);

		return DefaultProperties->getCustomColor(material, cs);
	}

	//! Return the settings for transparence
	virtual irr::scene::E_COLLADA_TRANSPARENT_FX getTransparentFx(const irr::video::SMaterial& material) const
	{
		if ( ExportTarget == MAX3D && isTransparent(material) )	
			return ECOF_A_ONE;	// no texture
		return ECOF_RGB_ZERO;	// this is what Max 3D wanted for textured alphas
	}

	//! Transparency value for that material. 
	virtual irr::f32 getTransparency(const irr::video::SMaterial& material) const
	{
		if ( isTransparent(material) )
		{
			if ( ExportTarget == MAX3D )
			{
				return 0.2f;
			}
			else
			{
				return 1.f;
			}
		}
		return -1.f;
	}

	//! Reflectivity value for that material
	virtual irr::f32 getReflectivity(const irr::video::SMaterial& material) const
	{
		return 0.5f;
	}

	//! Return index of refraction for that material
	virtual irr::f32 getIndexOfRefraction(const irr::video::SMaterial& material) const
	{
		return DefaultProperties->getIndexOfRefraction(material);
	}

	//! Should node be used in scene export? By default all visible nodes are exported.
	virtual bool isExportable(const irr::scene::ISceneNode * node) const
	{
		return DefaultProperties->isExportable(node);
	}

	//! Return the mesh for the given node. If it has no mesh or shouldn't export it's mesh return 0.
	virtual irr::scene::IMesh* getMesh(irr::scene::ISceneNode * node)
	{
		if ( !node )
			return 0;

		// If you have custom scenenodes you might have to check for them here and make sure 
		// they do return a mesh.
		//if ( node->getType() == my_special_type )
		//{
		//	return static_cast<MySpecialNode*>(node)->getMesh();
		//}
	
		return DefaultProperties->getMesh(node);
	}

	virtual bool useNodeMaterial(const scene::ISceneNode* node) const
	{
		return DefaultProperties->useNodeMaterial(node);
	}

protected:
	bool isTransparent(const irr::video::SMaterial& material) const
	{
		video::IMaterialRenderer* rnd =	Driver->getMaterialRenderer(material.MaterialType);
		return rnd && rnd->isTransparent();
	}

private:
	irr::scene::IColladaMeshWriterProperties * DefaultProperties;
	EExportTarget ExportTarget;
	IVideoDriver* Driver;
};

class MyColladaMeshWriterNames : public IColladaMeshWriterNames
{
public:
	MyColladaMeshWriterNames(IColladaMeshWriterNames* defaultNames)
		: DefaultNames(defaultNames)
	{
	}

	irr::core::stringw nameForMesh(const scene::IMesh* mesh, int instance)
	{
		return DefaultNames->nameForMesh(mesh, instance);
	}

	irr::core::stringw nameForNode(const scene::ISceneNode* node)
	{
		return DefaultNames->nameForNode(node);
	}

	irr::core::stringw nameForMaterial(const video::SMaterial & material, int materialId, const scene::IMesh* mesh, const scene::ISceneNode* node)
	{
//		return DefaultNames->nameForMaterial(material, materialId, mesh, node);

		// Some optimization not done by Irrlicht so far (slower export but less materials exported)
		core::stringw strMat = findExported(material);
		if ( ! strMat.empty() )
			return strMat;

		strMat = L"mat";

		if ( node && ( (node->getType() != ESNT_MESH )	||	
					   (node->getType() == ESNT_MESH && !static_cast<const IMeshSceneNode*>(node)->isReadOnlyMaterials())
					 ) 
		   )
		{
			strMat += nameForNode(node);
		}
		strMat += nameForMesh(mesh, 0);
		strMat += materialId;
		MaterialsExported.push_back(MaterialExported(material, strMat));
		return strMat;
	}

protected:
	irr::core::stringw findExported(const irr::video::SMaterial& material) const
	{
		for ( u32 i=0; i<MaterialsExported.size(); ++i )
		{
			if ( MaterialsExported[i].Material == material )
				return MaterialsExported[i].Name;
		}
		return irr::core::stringw();
	}

private:
	IColladaMeshWriterNames* DefaultNames;
	struct MaterialExported
	{
		MaterialExported(const video::SMaterial & material, const irr::core::stringw& name) 
			: Material(material), Name(name) 
		{}
		video::SMaterial Material;
		irr::core::stringw Name;
	};
	core::array< MaterialExported > MaterialsExported;
};

void createScene(IVideoDriver* driver, ISceneManager* smgr)
{
	// some light
	smgr->setAmbientLight( irr::video::SColorf( 1.f, 1.f, 1.f, 1.f)  );

	// simple meshes 
	IMesh * cubeMesh = smgr->getGeometryCreator()->createCubeMesh();
	cubeMesh->getMeshBuffer(0)->getMaterial().setTexture(0, driver->getTexture("blue.jpg"));
	IMesh * cubeMesh2 = smgr->getGeometryCreator()->createCubeMesh();
	cubeMesh2->getMeshBuffer(0)->getMaterial().setTexture(0, driver->getTexture("blue.jpg"));

	// three nodes using the same mesh but overriding it with 2 different materials
	//IMeshSceneNode * cube1 = smgr->addMeshSceneNode( cubeMesh, 0, -1, vector3df(-20, 0, 0), vector3df(45.f, 0.f, 0.f)  );
	//cube1->getMaterial(0).setTexture(0, driver->getTexture("red.jpg"));

	//IMeshSceneNode * cube2 = smgr->addMeshSceneNode( cubeMesh, 0, -1, vector3df(0, 0, 0) );
	//cube2->getMaterial(0).setTexture(0, driver->getTexture("green.jpg"));

	//IMeshSceneNode * cube3 = smgr->addMeshSceneNode( cubeMesh, 0, -1, vector3df(20, 0, 0) );
	//cube3->getMaterial(0).setTexture(0, driver->getTexture("red.jpg"));

	// two nodes sharing the same mesh and material (deliberately unsymetric position to find placement bugs)
	IMeshSceneNode * cube4 = smgr->addMeshSceneNode( cubeMesh2, 0, -1, vector3df(-25, -20, 0), vector3df(0.f, 0.f, 0.f)  );
//	cube4->setReadOnlyMaterials(true);
	cube4->getMaterial(0).setTexture(0, driver->getTexture("green.jpg"));

	IMeshSceneNode * cube5 = smgr->addMeshSceneNode( cubeMesh2, 0, -1, vector3df(20, -20, 0), vector3df(60.f, 0.f, 0.f)  );
//	cube5->setReadOnlyMaterials(true);
	cube5->getMaterial(0).setTexture(0, driver->getTexture("red.jpg"));

	cubeMesh->drop();
	cubeMesh2->drop();
}

void exportScene(IrrlichtDevice *device, IVideoDriver* driver, ISceneManager* smgr)
{
	scene::IMeshWriter* writer = 0;
	writer = smgr->createMeshWriter(scene::EMWT_COLLADA);
	irr::scene::IColladaMeshWriter * colladaWriter = static_cast<irr::scene::IColladaMeshWriter *>(writer);
	colladaWriter->setWriteDefaultScene(true);
	colladaWriter->setGeometryWriting(ECGI_PER_MESH_AND_MATERIAL);

	MyColladaMeshWriterProperties* myProperties = new MyColladaMeshWriterProperties(driver, colladaWriter->getDefaultProperties());
	colladaWriter->setProperties( myProperties );
	myProperties->drop();

	MyColladaMeshWriterNames * myNameGenerator = new MyColladaMeshWriterNames(colladaWriter->getDefaultNameGenerator());
	colladaWriter->setNameGenerator(myNameGenerator);
	myNameGenerator->drop();

	io::IWriteFile * meshFile = device->getFileSystem()->createAndWriteFile(COLLADA_FILENAME);
	if ( meshFile )
	{
		colladaWriter->writeScene(meshFile, smgr->getRootSceneNode() );
		meshFile->drop();
	}
	writer->drop();
}

void importScene(IrrlichtDevice *device, ISceneManager* smgr)
{
	// make sure we get a whole collada scene and not just the first mesh
	smgr->getParameters()->setAttribute(scene::COLLADA_CREATE_SCENE_INSTANCES, true);

	// we want to import each time instead of getting the mesh from cache.
	IMeshCache * meshCache = device->getSceneManager()->getMeshCache();
	IAnimatedMesh * oldMesh = meshCache->getMeshByName(COLLADA_FILENAME);
	if ( oldMesh )
		meshCache->removeMesh(oldMesh);

	// import
	device->getSceneManager()->getMesh(COLLADA_FILENAME);
}

void addCamera(ISceneManager* smgr)
{
	irr::scene::ICameraSceneNode * cam = smgr->addCameraSceneNode();
//	cam->bindTargetAndRotation(true);
	cam->updateAbsolutePosition();
	cam->setPosition( irr::core::vector3df( 0, 0, 100) );
	cam->updateAbsolutePosition();
	cam->setTarget( irr::core::vector3df( 0, 0, 0) );
	cam->updateAbsolutePosition();

	const core::matrix4& mat = cam->getProjectionMatrix();
	irr::core::vector3df p = mat.getTranslation();
	irr::core::vector3df r = mat.getRotationDegrees();
	int dbg = 1;
}

void printScene(ISceneNode* node, int depth)
{
	for ( int i=0; i<depth; ++i )
	{
		printf("-");
	}
	printf("%s", node->getDebugName());
	irr::core::vector3df absPos = node->getAbsoluteTransformation().getTranslation();
	irr::core::vector3df relPos = node->getRelativeTransformation().getTranslation();
	printf(" abs: %.1f %.1f %.1f rel: %.1f %.1f %.1f\n", absPos.X, absPos.Y, absPos.Z, relPos.X, relPos.Y, relPos.Z );

	const core::list<ISceneNode*>& children = node->getChildren();
	for ( core::list<ISceneNode*>::ConstIterator it = children.begin(); it != children.end(); ++it )
	{
		printScene(*it, depth+1);
	}
}

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver() 
		: KeepRunning(true), Device(0), ButtonExport(0), ButtonImport(0), ButtonCreateScene(0), ButtonClearScene(0), ButtonQuit(0)
	{ }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT
			&& event.GUIEvent.EventType == EGET_BUTTON_CLICKED	)
		{
			if ( event.GUIEvent.Caller == ButtonExport )
			{
				exportScene(Device, Device->getVideoDriver(), Device->getSceneManager());
			}
			else if ( event.GUIEvent.Caller == ButtonImport )
			{
				ISceneManager * smgr = Device->getSceneManager();
				smgr->clear();
				importScene(Device, smgr);
				printScene(smgr->getRootSceneNode(), 0);
				addCamera(smgr);
			}
			else if ( event.GUIEvent.Caller == ButtonCreateScene )
			{
				ISceneManager * smgr = Device->getSceneManager();
				smgr->clear();
				createScene(Device->getVideoDriver(), smgr);
				printScene(smgr->getRootSceneNode(), 0);
				addCamera(smgr);
			}
			else if ( event.GUIEvent.Caller == ButtonClearScene )
			{
				Device->getSceneManager()->clear();
			}
			else if ( event.GUIEvent.Caller == ButtonQuit )
			{
				KeepRunning = false;
			}
		}

		return false;
	}

	bool KeepRunning;
	IrrlichtDevice * Device;
	IGUIButton * ButtonExport;
	IGUIButton * ButtonImport;
	IGUIButton * ButtonCreateScene;
	IGUIButton * ButtonClearScene;
	IGUIButton * ButtonQuit;
};


int main()
{
	IrrlichtDevice *device = createDevice( video::EDT_OPENGL, (const dimension2du&)dimension2d<u32>(1280, 752), 32, false, false, false, false);

	if (!device)
		return 1;

	device->getLogger()->setLogLevel(irr::ELL_DEBUG);	// useful for testing format import/exports
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	MyEventReceiver eventReceiver;
	eventReceiver.Device = device;
	eventReceiver.ButtonExport = guienv->addButton(recti(20, 20, 100, 40), 0, -1, L"Export" );
	eventReceiver.ButtonImport = guienv->addButton(recti(20, 50, 100, 70), 0, -1, L"Import" );
	eventReceiver.ButtonCreateScene = guienv->addButton(recti(20, 80, 100, 100), 0, -1, L"Create scene" );
	eventReceiver.ButtonClearScene = guienv->addButton(recti(20, 110, 100, 130), 0, -1, L"Clear scene" );
	eventReceiver.ButtonQuit = guienv->addButton(recti(20, 150, 100, 170), 0, -1, L"Quit" );

	device->setEventReceiver(&eventReceiver);

	createScene(driver, smgr);
	addCamera(smgr);

	while ( device->run() && eventReceiver.KeepRunning )
	{
		driver->beginScene(true, true);
		smgr->drawAll();
		guienv->drawAll();
		driver->endScene();
	}

	device->closeDevice();
	device->run();
	device->drop();

	return 0;
}
