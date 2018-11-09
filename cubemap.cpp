// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder based on irrSpintz example EnvCubeMap

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


#include "irrlicht.h"

using namespace irr;

class CubeMapReflectionCallback : public video::IShaderConstantSetCallBack
{
public:
	CubeMapReflectionCallback(scene::ISceneManager* smgr) 
		: SceneMgr(smgr) 
		, StyleUVW(2)
		, styleUvwID(-1)
		, worldViewProjID(-1)
		, worldID(-1)
		, cameraPosID(-1)
	{
	}

	virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
	{
		video::IVideoDriver* driver = services->getVideoDriver();

		if ( worldViewProjID < 0 )	// first update
		{
			styleUvwID = services->getVertexShaderConstantID("StyleUVW");
			worldViewProjID = services->getVertexShaderConstantID("WorldViewProj");
			worldID = services->getVertexShaderConstantID("World");
			cameraPosID = services->getVertexShaderConstantID("CameraPos");
		}

		services->setVertexShaderConstant(styleUvwID, &StyleUVW, 1 );

		if( driver->getDriverType() == video::EDT_DIRECT3D9 )
		{
			irr::core::matrix4 world = driver->getTransform(irr::video::ETS_WORLD);

			irr::core::matrix4 worldViewProj;
			worldViewProj = driver->getTransform(irr::video::ETS_PROJECTION);
			worldViewProj *= driver->getTransform(irr::video::ETS_VIEW);
			worldViewProj *= world;
			services->setVertexShaderConstant(worldViewProjID, worldViewProj.pointer(), 16);

			services->setVertexShaderConstant(worldID, world.pointer(), 16);
		}
		
		core::vector3df cameraPos = SceneMgr->getActiveCamera()->getAbsolutePosition();
		services->setVertexShaderConstant(cameraPosID, &cameraPos.X, 3 );
	}

private:
	scene::ISceneManager* SceneMgr;

	int StyleUVW;

	irr::s32 styleUvwID;
	irr::s32 worldViewProjID;
	irr::s32 worldID;
	irr::s32 cameraPosID;
};

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver() : Step(0) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown == false)
		{
			switch(event.KeyInput.Key )
			{
			case KEY_SPACE:
				Step = (Step+1)%4;
				break;
			default:
				break;
			}
		}

		return false;
	}

	int Step;
};

//! Copy texture to an image and write that to a file
void writeTextureToFile(irr::video::IVideoDriver* driver, irr::video::ITexture* texture, irr::u32 layer, const irr::io::path& name)
{
	if (!texture)
		return;

	void* data = texture->lock(irr::video::ETLM_READ_ONLY, layer);
	if (!data)
		return;

	irr::video::IImage* image = driver->createImageFromData(texture->getColorFormat(), texture->getSize(), data, false);
	texture->unlock();

	if (!image)
		return;

	const u32 quality = 100;
	driver->writeImageToFile(image, name, quality);

	image->drop();
}

void writeCubeTextureToFile(irr::video::IVideoDriver* driver, irr::video::ITexture* texture, const irr::io::path& name)
{
	io::path path, filename, extension;

	splitFilename(name, &path, &filename, &extension);

	for ( irr::u32 i=0; i<6; ++i )
	{
		io::path namet = mergeFilename(path, filename + io::path(i+1), extension);
		writeTextureToFile(driver, texture, i, namet);
	}
}


int main()
{
#if 0
	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;
#else
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
#endif
	IrrlichtDevice* device = createDevice( driverType, core::dimension2d<u32>( 1024, 768 ) );
	if (!device)
		return 1; 
	
	core::stringw cubeOutName( driverType == video::EDT_DIRECT3D9 ? "cubemap/cube_out_dx.jpg" : "cubemap/cube_out_gl.jpg");
	core::stringw cubeOutDynName( driverType == video::EDT_DIRECT3D9 ? "cubemap/cube_out_dyn_dx.jpg" : "cubemap/cube_out_dyn_gl.jpg");

	MyEventReceiver eventReceiver;
	device->setEventReceiver(&eventReceiver);

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	c8* vsFileName = 0;
	c8* psFileName = 0;

	switch( driverType )
	{
	case video::EDT_DIRECT3D9:
		vsFileName = "cubemap/IS_cubeMapReflectionVS.hlsl";
		psFileName = "cubemap/IS_cubeMapReflectionPS.hlsl";
		break;

	case video::EDT_OPENGL:
		vsFileName = "cubemap/IS_cubeMapReflection.vert";
		psFileName = "cubemap/IS_cubeMapReflection.frag";
		break;
	}	

	// create materials
	video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();

	s32 cubeMapReflectionMaterial = 0;
	if( gpu )
	{
		CubeMapReflectionCallback* cubeMapCB = new CubeMapReflectionCallback(smgr);
		cubeMapReflectionMaterial = gpu->addHighLevelShaderMaterialFromFiles(
			vsFileName, "VS", video::EVST_VS_1_1,
			psFileName, "PS", video::EPST_PS_2_0,
			cubeMapCB, video::EMT_SOLID );
		cubeMapCB->drop();
	}	

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	// add fps camera
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS(0, 100.f, 1.f);
	camera->setPosition( core::vector3df( 0,10,-200 ) );
	device->getCursorControl()->setVisible(false);


	// Create Cube Map Render Target and Camera for Render To Texture
	video::IRenderTarget* cubeMapRT = driver->addRenderTarget();

	core::array<video::IImage*> cubeMapImages;
	cubeMapImages.push_back(driver->createImageFromFile( "cubemap/cubemap_posx.jpg" ));
	cubeMapImages.push_back(driver->createImageFromFile( "cubemap/cubemap_negx.jpg" ));
	cubeMapImages.push_back(driver->createImageFromFile( "cubemap/cubemap_posy.jpg" ));
	cubeMapImages.push_back(driver->createImageFromFile( "cubemap/cubemap_negy.jpg" ));
	cubeMapImages.push_back(driver->createImageFromFile( "cubemap/cubemap_posz.jpg" ));
	cubeMapImages.push_back(driver->createImageFromFile( "cubemap/cubemap_negz.jpg" ));
	video::ITexture* cubeMap = 0;
	cubeMap = driver->addTextureCubemap("cm", cubeMapImages[0], cubeMapImages[1], cubeMapImages[2], cubeMapImages[3], cubeMapImages[4], cubeMapImages[5]);
	for ( u32 i=0; i<cubeMapImages.size(); ++i )
		cubeMapImages[i]->drop();
	cubeMapImages.clear();
//	writeCubeTextureToFile(driver, cubeMap, cubeOutName);

	video::ITexture* dynamicCubeMap = 0;
	video::ITexture* dynamicDebugRT = 0;
	scene::ICameraSceneNode* cubeMapCamera = 0;
	if( driver->queryFeature( video::EVDF_RENDER_TO_TARGET ) )
	{
		// Create cube map texture
		dynamicCubeMap = driver->addRenderTargetTextureCubemap(512, "cube_rt");
		dynamicDebugRT = driver->addRenderTargetTexture(core::dimension2du(512, 512), "cube_dbg_rt");

		cubeMapCamera = smgr->addCameraSceneNode();
		cubeMapCamera->setFOV(core::PI* 0.5f);	// 90°
		cubeMapCamera->setAspectRatio(1.f);
		smgr->setActiveCamera( camera );
	}

	// add sphere to reflect scene
	scene::ISceneNode* sphereNode = 0;
	scene::ISceneNode* sphereNode2 = 0;
	scene::IMesh* sphereMesh = smgr->getGeometryCreator()->createSphereMesh(100.f);
	if( sphereMesh )
	{
		sphereNode = smgr->addMeshSceneNode( sphereMesh );
		sphereNode->setPosition( core::vector3df(-250,0,0) );
		sphereNode->updateAbsolutePosition();
		sphereNode->setMaterialFlag( video::EMF_LIGHTING, false );
		sphereNode->setMaterialTexture( 0, dynamicCubeMap );
		sphereNode->setMaterialType( (video::E_MATERIAL_TYPE)cubeMapReflectionMaterial );
#if 1
		sphereNode2 = smgr->addMeshSceneNode( sphereMesh );
		sphereNode2->setPosition( core::vector3df(250,0,0) );
		sphereNode2->updateAbsolutePosition();
		sphereNode2->setMaterialFlag( video::EMF_LIGHTING, false );
		sphereNode2->setMaterialTexture( 0, cubeMap );
		sphereNode2->setMaterialType( (video::E_MATERIAL_TYPE)cubeMapReflectionMaterial );
#endif

		sphereMesh->drop();
	}

#if 1
	scene::ISceneNode* skybox = smgr->addSkyBoxSceneNode(
	driver->getTexture("cubemap/cubemap_posy.jpg"), // top
	driver->getTexture("cubemap/cubemap_negy.jpg"),	// bottom
	driver->getTexture("cubemap/cubemap_posz.jpg"),	// left
	driver->getTexture("cubemap/cubemap_negz.jpg"), // right
	driver->getTexture("cubemap/cubemap_posx.jpg"), // front
	driver->getTexture("cubemap/cubemap_negx.jpg")); // back
#else
	scene::IAnimatedMesh* cubeMesh = (scene::IAnimatedMesh*)smgr->getMesh( "cubemap/cubeBoxTest.ms3d" );
	if( cubeMesh )
	{
		smgr->getMeshManipulator()->setVertexColors( cubeMesh->getMeshBuffer(0), cubeMesh->getMeshBuffer(0)->getMaterial().DiffuseColor );
		smgr->getMeshManipulator()->setVertexColors( cubeMesh->getMeshBuffer(1), cubeMesh->getMeshBuffer(1)->getMaterial().DiffuseColor );
		smgr->getMeshManipulator()->setVertexColors( cubeMesh->getMeshBuffer(2), cubeMesh->getMeshBuffer(2)->getMaterial().DiffuseColor );
		smgr->getMeshManipulator()->setVertexColors( cubeMesh->getMeshBuffer(3), cubeMesh->getMeshBuffer(3)->getMaterial().DiffuseColor );
		smgr->getMeshManipulator()->setVertexColors( cubeMesh->getMeshBuffer(4), cubeMesh->getMeshBuffer(4)->getMaterial().DiffuseColor );
		smgr->getMeshManipulator()->setVertexColors( cubeMesh->getMeshBuffer(5), cubeMesh->getMeshBuffer(5)->getMaterial().DiffuseColor );

		scene::ISceneNode* coloredCubeNode = smgr->addMeshSceneNode( cubeMesh );
		coloredCubeNode->setScale( core::vector3df( 200, 200, 200 ) );
		coloredCubeNode->setMaterialFlag( video::EMF_LIGHTING, false );
	}
#endif

	// RTT's are rendered upside down in OpenGL.
	// Now comes the tricky part: In fixed function materials Irrlicht flips the texture-matrix for that.
	// So this hack I use here would be bad then. The nicer solution imho would be if Irrlicht would 
	// change the projection matrix if the target is a RTT on OpenGL. But maybe that has some problem I don't see.
	// The bad part now is - because I fix it here - correct rendered RTT's will no longer work with fixed-function materials in OpenGL *sigh*
	const float up = driverType == video::EDT_OPENGL ? 1.f : 1.f;

	while(device->run())
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true);

			static bool justOnce = true;	// if nothing changes that's enough, if things the sphere "sees" then we have to update constantly.

			if( dynamicCubeMap && sphereNode && justOnce )
			{
				justOnce = false;
				smgr->setActiveCamera( cubeMapCamera );

				sphereNode->setVisible( false );
				const core::vector3df center( sphereNode->getAbsolutePosition() );
				cubeMapCamera->setPosition( center );

				// Render all 6 directions
				cubeMapCamera->setUpVector( core::vector3df( 0,up,0 ) );
				cubeMapCamera->setTarget( center + core::vector3df(1.f, 0.f, 0.f) );
				cubeMapRT->setTexture(dynamicCubeMap, 0, video::ECS_POSX);
				driver->setRenderTargetEx(cubeMapRT, video::ECBF_ALL);
				smgr->drawAll();

				if ( dynamicDebugRT )
				{
					driver->setRenderTarget(dynamicDebugRT, true, true);
					smgr->drawAll();
					writeTextureToFile(driver, dynamicDebugRT, 0, io::path("cubemap/cube_out_dbg1.jpg"));
				}

				cubeMapCamera->setTarget( center + core::vector3df(-1.f, 0.f, 0.f) );
				cubeMapRT->setTexture(dynamicCubeMap, 0, video::ECS_NEGX);
				driver->setRenderTargetEx(cubeMapRT, video::ECBF_ALL);
				smgr->drawAll();

				if ( dynamicDebugRT )
				{
					driver->setRenderTarget(dynamicDebugRT, true, true);
					smgr->drawAll();
					writeTextureToFile(driver, dynamicDebugRT, 1, io::path("cubemap/cube_out_dbg2.jpg"));
				}

				cubeMapCamera->setUpVector( core::vector3df(0,0,-up) );
				cubeMapCamera->setTarget( center + core::vector3df(0.f, 1.f, 0.f) );
				cubeMapRT->setTexture(dynamicCubeMap, 0, video::ECS_POSY);
				driver->setRenderTargetEx(cubeMapRT, video::ECBF_ALL);
				smgr->drawAll();

				if ( dynamicDebugRT )
				{
					driver->setRenderTarget(dynamicDebugRT, true, true);
					smgr->drawAll();
					writeTextureToFile(driver, dynamicDebugRT, 2, io::path("cubemap/cube_out_dbg3.jpg"));
				}

				cubeMapCamera->setUpVector( core::vector3df(0,0,up) );
				cubeMapCamera->setTarget( center + core::vector3df(0.f, -1.f, 0.f) );
				cubeMapRT->setTexture(dynamicCubeMap, 0, video::ECS_NEGY);
				driver->setRenderTargetEx(cubeMapRT, video::ECBF_ALL);
				smgr->drawAll();

				if ( dynamicDebugRT )
				{
					driver->setRenderTarget(dynamicDebugRT, true, true);
					smgr->drawAll();
					writeTextureToFile(driver, dynamicDebugRT, 3, io::path("cubemap/cube_out_dbg4.jpg"));
				}

				cubeMapCamera->setUpVector( core::vector3df( 0,up,0 ) );
				cubeMapCamera->setTarget( center + core::vector3df(0.f, 0.f, 1.f) );
				cubeMapRT->setTexture(dynamicCubeMap, 0, video::ECS_POSZ);
				driver->setRenderTargetEx(cubeMapRT, video::ECBF_ALL);
				smgr->drawAll();

				if ( dynamicDebugRT )
				{
					driver->setRenderTarget(dynamicDebugRT, true, true);
					smgr->drawAll();
					writeTextureToFile(driver, dynamicDebugRT, 4, io::path("cubemap/cube_out_dbg5.jpg"));
				}

				cubeMapCamera->setTarget( center + core::vector3df(0.f, 0.f, -1.f) );
				cubeMapRT->setTexture(dynamicCubeMap, 0, video::ECS_NEGZ);
				driver->setRenderTargetEx(cubeMapRT, video::ECBF_ALL);
				smgr->drawAll();

				if ( dynamicDebugRT )
				{
					driver->setRenderTarget(dynamicDebugRT, true, true);
					smgr->drawAll();
					writeTextureToFile(driver, dynamicDebugRT, 5, io::path("cubemap/cube_out_dbg6.jpg"));
				}

				//dynamicCubeMap->regenerateMipMapLevels();

				driver->setRenderTarget(0);
				sphereNode->setVisible( true );
				smgr->setActiveCamera( camera );
			}
			smgr->drawAll();
			env->drawAll();

			driver->endScene();

			static bool first = true;
			if ( first )
			{
				first = false;
				dynamicDebugRT = 0;
				writeCubeTextureToFile(driver, dynamicCubeMap, cubeOutDynName);
			}
		}
	}

	device->drop();
	
	return 0;
}

