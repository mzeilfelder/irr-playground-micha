// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder. 
// Based originally on EnvCubeMap example from irrSpintz

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#include <irrlicht.h>
#include "driverChoice.h"
#include "exampleHelper.h"

using namespace irr;

// Texture origins in OpenGL are usually left-bottom instead of the more common left-top.
// Irrlicht ignores that and uses textures with left-top origin and then flips all texture-matrices in the fixed-function pipeline.
// For shaders it's left to the users to handle those uv-flips for the texture-matrix.
// RTT's are rendered "correctly" with left-bottom origin. 
// In fixed function pipeline Irrlicht handles this by flipping the RTT's texture matrix once more (so all textures are upside down).
// (not sure why it doesn't use the original instead - haven't tried yet changing that)
// Cubemaps are once more different. Basically each side has left-top as origin. So not flipping textures for those would be fine.
// Except - OpenGL RTT's still seems to render left-bottom - even when the target is a cubemap RTT.
// I found no good way around this so far - it just seems messed up as we get a left-handed/right handed coordinate system change that way.
// So... following 2 defines are the 2 workarounds I found. Both are ugly, which one is better in reality depends probably on the scene.
// Only use one of those:
#define CUBEMAP_UPSIDE_DOWN_GL_PROJECTION
//#define CUBEMAP_USPIDE_DOWN_RTT

// Callback for cubemap shader
class CubeMapReflectionCallback : public video::IShaderConstantSetCallBack
{
public:
	CubeMapReflectionCallback(scene::ISceneManager* smgr, int styleUVW) 
		: SceneMgr(smgr) 
		, StyleUVW(styleUVW)
		, Roughness(0.f)
		, styleUvwID(-1)
		, worldViewProjID(-1)
		, worldID(-1)
		, cameraPosID(-1)
	{}

	void SetStyleUVW(int style)
	{
		StyleUVW = style;
	}

	int GetStyleUVW() const
	{
		return StyleUVW;
	}

	void SetRoughness(float roughness)
	{
		Roughness = roughness;
	}

	float getRoughness() const
	{
		return Roughness;
	}

	virtual void OnSetMaterial(const video::SMaterial& material)
	{}

	virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
	{
		video::IVideoDriver* driver = services->getVideoDriver();

		if ( worldViewProjID < 0 )	// first update
		{
			styleUvwID = services->getVertexShaderConstantID("StyleUVW");
			if( driver->getDriverType() == video::EDT_DIRECT3D9 )
			{
				worldViewProjID = services->getVertexShaderConstantID("WorldViewProj");
			}
			worldID = services->getVertexShaderConstantID("World");
			cameraPosID = services->getVertexShaderConstantID("CameraPos");
			roughnessID = services->getPixelShaderConstantID("Roughness");
		}

		services->setVertexShaderConstant(styleUvwID, &StyleUVW, 1 );

		irr::core::matrix4 world = driver->getTransform(irr::video::ETS_WORLD);
		services->setVertexShaderConstant(worldID, world.pointer(), 16);

		if( driver->getDriverType() == video::EDT_DIRECT3D9 )
		{
			irr::core::matrix4 worldViewProj;
			worldViewProj = driver->getTransform(irr::video::ETS_PROJECTION);
			worldViewProj *= driver->getTransform(irr::video::ETS_VIEW);
			worldViewProj *= world;
			services->setVertexShaderConstant(worldViewProjID, worldViewProj.pointer(), 16);
		}
		
		core::vector3df cameraPos = SceneMgr->getActiveCamera()->getAbsolutePosition();
		services->setVertexShaderConstant(cameraPosID, &cameraPos.X, 3 );
		services->setPixelShaderConstant(roughnessID, &Roughness, 1 );
	}

private:
	scene::ISceneManager* SceneMgr;

	int StyleUVW;		// 0 = specular, 1=diffuse, 2 = use model vertex coordinates for uvw.
	float Roughness;	// cubemap 0 = specular ... highest value depends on number of mipmaps in the texture

	irr::s32 styleUvwID;
	irr::s32 worldViewProjID;
	irr::s32 worldID;
	irr::s32 cameraPosID;
	irr::s32 roughnessID;
};

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver() : Driver(0), Shader(0) 
		,BackgroundSkybox(0), BackgroundCube(0)
		, CubemapUpdates(0)
		, CurrentStyleUVW(0), CurrentRoughness(0)
		, NeedCubemapUpdate(true)
	{ 
		StyleNamesUVW.push_back( L"specular" );
		StyleNamesUVW.push_back( L"diffuse" );
		StyleNamesUVW.push_back( L"model coordinates" );
	}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown == false)
		{
			switch(event.KeyInput.Key )
			{
			case KEY_SPACE:
				if ( Shader )
				{
					Shader->SetStyleUVW((Shader->GetStyleUVW()+1)%StyleNamesUVW.size());
					updateStyleUVW();
				}
				break;
			case KEY_KEY_B:
				if ( BackgroundSkybox && BackgroundCube )
				{
					if ( BackgroundSkybox->isVisible() )
					{
						BackgroundSkybox->setVisible(false);
						BackgroundCube->setVisible(true);
					}
					else
					{
						BackgroundSkybox->setVisible(true);
						BackgroundCube->setVisible(false);
					}
					NeedCubemapUpdate = true;
				}
				break;
			case KEY_KEY_S:
				if ( Driver )
				{
					Driver->disableFeature(video::EVDF_TEXTURE_CUBEMAP_SEAMLESS, Driver->queryFeature(video::EVDF_TEXTURE_CUBEMAP_SEAMLESS) );
					updateSeamless();
				}
				break;
			case KEY_KEY_U:
				CubemapUpdates = (CubemapUpdates+1) % 2;
				updateCubemapUpdates();
				break;
			case KEY_PLUS:
			case KEY_ADD:
				if ( Shader )
				{
					Shader->SetRoughness( Shader->getRoughness() + 0.5f );
					updateRoughness();
				}
				break;
			case KEY_MINUS:
			case KEY_SUBTRACT:
			{
				if ( Shader )
				{
					float roughness = Shader->getRoughness() - 0.5f;
					if ( roughness >= 0.f )
					{
						Shader->SetRoughness(roughness);
						updateRoughness();
					}
				}
				break;
			}
			default:
				break;
			}
		}

		return false;
	}

	void updateStyleUVW()
	{
		if ( CurrentStyleUVW && Shader)
			CurrentStyleUVW->setText(StyleNamesUVW[Shader->GetStyleUVW()].c_str());
	}

	void updateRoughness()
	{
		if ( CurrentRoughness && Shader )
		{
			CurrentRoughness->setText( irr::core::stringw(Shader->getRoughness()).c_str() );
		}
	}

	void updateSeamless() 
	{
		if ( CurrentSeamlessCubemap && Driver )
		{
			CurrentSeamlessCubemap->setText( Driver->queryFeature(video::EVDF_TEXTURE_CUBEMAP_SEAMLESS) ? L"ON" : L"OFF" );
		}
	}

	void updateCubemapUpdates()
	{
		if ( CurrentCubemapUpdates )
		{
			CurrentCubemapUpdates->setText( CubemapUpdates == 0 ? L"static" : L"dynamic" );
		}
	}

	// Should cubemap textures be updated?
	bool CheckCubemapUpdate()
	{
		if ( NeedCubemapUpdate || CubemapUpdates == 1)
		{
			NeedCubemapUpdate = false;
			return true;
		}
		return false;
	}

	irr::video::IVideoDriver* Driver;
	CubeMapReflectionCallback* Shader;

	scene::ISceneNode* BackgroundSkybox;
	scene::ISceneNode* BackgroundCube;

	int CubemapUpdates;	// 0 = static, 1 = dynamic

	irr::core::array<irr::core::stringw> StyleNamesUVW;

	irr::gui::IGUIStaticText* CurrentStyleUVW;
	irr::gui::IGUIStaticText* CurrentRoughness;
	irr::gui::IGUIStaticText* CurrentSeamlessCubemap;
	irr::gui::IGUIStaticText* CurrentCubemapUpdates;

private:
	bool NeedCubemapUpdate;
};

//! Copy texture to an image and write that to a file
void writeTextureToFile(irr::video::IVideoDriver* driver, irr::video::ITexture* texture, irr::u32 layer, const irr::io::path& name)
{
	if (!texture)
		return;

	void* data = texture->lock(irr::video::ETLM_READ_ONLY, 0, layer);
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

// Flip frontface/backface culling for all nodes
void flipCullingFlags(const core::array<scene::ISceneNode*>& nodes)
{
	for ( irr::u32 n=0; n < nodes.size(); ++n )
	{
		scene::ISceneNode* node = nodes[n];
		const irr::u32 matCount = node->getMaterialCount();
		for ( irr::u32 m=0; m < matCount; ++m)
		{
			video::SMaterial& mat = node->getMaterial(m);
			mat.BackfaceCulling = !mat.BackfaceCulling;
			mat.FrontfaceCulling = !mat.FrontfaceCulling;
		}
	}
}

void renderCubeMap(irr::video::IVideoDriver* driver, irr::scene::ICameraSceneNode* cubeMapCamera, irr::scene::ISceneNode* cubeCenterNode, video::IRenderTarget* cubeMapRT, video::ITexture* dynamicCubeMapRTT, video::ITexture* depthStencilRTT)
{
	scene::ISceneManager* smgr = cubeMapCamera->getSceneManager();
	scene::ICameraSceneNode * oldCam = smgr->getActiveCamera();
	smgr->setActiveCamera( cubeMapCamera );

	cubeCenterNode->setVisible( false );
	const core::vector3df center( cubeCenterNode->getAbsolutePosition() );
	cubeMapCamera->setPosition( center );

	// Render all 6 directions
	const core::vector3df targetVecs[6] = {
		core::vector3df(1.f, 0.f, 0.f),
		core::vector3df(-1.f, 0.f, 0.f),
		core::vector3df(0.f, 1.f, 0.f),
		core::vector3df(0.f, -1.f, 0.f),
		core::vector3df(0.f, 0.f, 1.f),
		core::vector3df(0.f, 0.f, -1.f)
	};

	const core::vector3df upVecs[6] = {
		core::vector3df( 0,1,0 ),
		core::vector3df( 0,1,0 ),
		core::vector3df( 0,0,-1 ),
		core::vector3df( 0,0,1 ),
		core::vector3df( 0,1,0 ),
		core::vector3df( 0,1,0 )
	};
	for ( int s=0; s<6; ++s )
	{
		cubeMapCamera->setUpVector( upVecs[s] );
		cubeMapCamera->setTarget( center + targetVecs[s] );
		cubeMapRT->setTexture(dynamicCubeMapRTT, depthStencilRTT, (video::E_CUBE_SURFACE)(video::ECS_POSX + s));
		driver->setRenderTargetEx(cubeMapRT, video::ECBF_ALL);
		smgr->drawAll();

#ifdef CUBEMAP_USPIDE_DOWN_RTT
		// This works because the lock for rtt's always flips in Irrlicht. 
		// So in this case lock() unlock will result in a flipped texture
		driver->setRenderTarget(0);	// to avoid accessing active rt
		dynamicCubeMapRTT->lock(video::ETLM_READ_WRITE, 0, s, video::ETLF_FLIP_Y_UP_RTT);
		dynamicCubeMapRTT->unlock();
#endif
	}

	//dynamicCubeMapRTT->regenerateMipMapLevels();	// Unfortunately we can't have mipmaps for rtt's

	driver->setRenderTarget(0);
	cubeCenterNode->setVisible( true );
	smgr->setActiveCamera( oldCam );

#if 0	// write out dynamic cubemap texture to file once.
	static bool first = true;
	if ( first )
	{
		first = false;
		core::stringw cubeOutDynName( driver->getDriverType() == video::EDT_DIRECT3D9 ? "cubemap/cube_out_dyn_dx.jpg" : "cubemap/cube_out_dyn_gl.jpg");
		writeCubeTextureToFile(driver, dynamicCubeMapRTT, cubeOutDynName);
	}
#endif
}

int main()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType = driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;

	// Create device
	const core::dimension2d<u32> dimDevice(1024, 768);
	IrrlichtDevice* device = createDevice( driverType, dimDevice );
	if (!device)
		return 1; 

	MyEventReceiver eventReceiver;
	device->setEventReceiver(&eventReceiver);

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	eventReceiver.Driver = driver;

	// Set window title
	core::stringw strCaption(L"Cubemap example - Irrlicht Engine [");
	strCaption += driver->getName();
	strCaption += L"]";
	device->setWindowCaption(strCaption.c_str());

	// Decide on shader to use based on active driver 
	const c8* vsFileName = 0;
	const c8* psFileName = 0;
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

	// create shader material
	video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	s32 cubeMapReflectionMaterial = 0;
	if( gpu )
	{
		CubeMapReflectionCallback* cubeMapCB = new CubeMapReflectionCallback(smgr, 2);
		cubeMapReflectionMaterial = gpu->addHighLevelShaderMaterialFromFiles(
			vsFileName, "VS", video::EVST_VS_1_1,
			psFileName, "PS", video::EPST_PS_3_0,
			cubeMapCB, video::EMT_SOLID );
		eventReceiver.Shader = cubeMapCB;
		cubeMapCB->drop();
	}	

	
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

	video::ITexture* cubeMapStaticTex = 0;
	cubeMapStaticTex = driver->addTextureCubemap("cm", cubeMapImages[0], cubeMapImages[1], cubeMapImages[2], cubeMapImages[3], cubeMapImages[4], cubeMapImages[5]);
	for ( u32 i=0; i<cubeMapImages.size(); ++i )
		cubeMapImages[i]->drop();
	cubeMapImages.clear();
//	core::stringw cubeOutName( driverType == video::EDT_DIRECT3D9 ? "cubemap/cube_out_dx.jpg" : "cubemap/cube_out_gl.jpg");
//	writeCubeTextureToFile(driver, cubeMapStaticTex, cubeOutName);
	
	video::ITexture* dynamicCubeMapRTT = 0;
	video::ITexture* depthStencilRTT = 0;
	video::ITexture* dynamicCubeMapRTT_intermediate = 0;	// just for rendering, but not used in material
	video::ITexture* dynamicCubeMapTex = 0;					// dynamic and with mipmaps
	scene::ICameraSceneNode* cubeMapCamera = 0;
	if( driver->queryFeature( video::EVDF_RENDER_TO_TARGET ) )
	{
		// Create cube map textures
		const u32 dynamicCubeMapSize = 512;
		dynamicCubeMapRTT = driver->addRenderTargetTextureCubemap(dynamicCubeMapSize, "cube_rtr");
		depthStencilRTT = driver->addRenderTargetTexture(irr::core::dimension2du(dynamicCubeMapSize, dynamicCubeMapSize), "cubemap_ds", irr::video::ECF_D24S8);

		dynamicCubeMapRTT_intermediate = driver->addRenderTargetTextureCubemap(dynamicCubeMapSize, "cube_rtr");
		dynamicCubeMapTex = driver->addTextureCubemap(dynamicCubeMapSize, "cube_tex");

		cubeMapCamera = smgr->addCameraSceneNode();
		cubeMapCamera->setFOV(core::PI* 0.5f);	// 90° view angle
		cubeMapCamera->setAspectRatio(1.f);
		smgr->setActiveCamera( camera );
	}

	// add sphere to reflect scene
	scene::ISceneNode* sphereNode = 0;
	scene::ISceneNode* sphereNode2 = 0;
	scene::ISceneNode* sphereNode3 = 0;
	scene::IMesh* sphereMesh = smgr->getGeometryCreator()->createSphereMesh(100.f);
	if( sphereMesh )
	{
		sphereNode = smgr->addMeshSceneNode( sphereMesh );
		sphereNode->setPosition( core::vector3df(-250,0,0) );
		sphereNode->updateAbsolutePosition();
		sphereNode->setMaterialFlag( video::EMF_LIGHTING, false );
		sphereNode->setMaterialTexture( 0, dynamicCubeMapRTT );
		sphereNode->setMaterialType( (video::E_MATERIAL_TYPE)cubeMapReflectionMaterial );

		if ( dynamicCubeMapTex )
		{
			sphereNode3 = smgr->addMeshSceneNode( sphereMesh );
			sphereNode3->setPosition( core::vector3df(0,0,250) );
			sphereNode3->updateAbsolutePosition();
			sphereNode3->setMaterialFlag( video::EMF_LIGHTING, false );
			sphereNode3->setMaterialTexture( 0, dynamicCubeMapTex );
			sphereNode3->getMaterial(0).TextureLayer[0].TrilinearFilter = false; // this is default anyway. It would be faster - but you can only access integer mip-levels - no filtering between mip-levels.
			sphereNode3->setMaterialType( (video::E_MATERIAL_TYPE)cubeMapReflectionMaterial );
		}

		if ( cubeMapStaticTex )
		{
			sphereNode2 = smgr->addMeshSceneNode( sphereMesh );
			sphereNode2->setPosition( core::vector3df(250,0,0) );
			sphereNode2->updateAbsolutePosition();
			sphereNode2->setMaterialFlag( video::EMF_LIGHTING, false );
			sphereNode2->setMaterialTexture( 0, cubeMapStaticTex );
			sphereNode2->getMaterial(0).TextureLayer[0].TrilinearFilter = true;		// this way smoothing happens between different mip-levels.
			sphereNode2->setMaterialType( (video::E_MATERIAL_TYPE)cubeMapReflectionMaterial );
		}

		sphereMesh->drop();
	}

	eventReceiver.BackgroundSkybox = smgr->addSkyBoxSceneNode(
	driver->getTexture("cubemap/cubemap_posy.jpg"), // top
	driver->getTexture("cubemap/cubemap_negy.jpg"),	// bottom
	driver->getTexture("cubemap/cubemap_posz.jpg"),	// left
	driver->getTexture("cubemap/cubemap_negz.jpg"), // right
	driver->getTexture("cubemap/cubemap_posx.jpg"), // front
	driver->getTexture("cubemap/cubemap_negx.jpg")); // back

	scene::IAnimatedMesh* cubeMesh = smgr->getMesh( "cubemap/cubeBoxTest.ms3d" );
	if( cubeMesh )
	{
		smgr->getMeshManipulator()->setVertexColors( cubeMesh->getMeshBuffer(0), cubeMesh->getMeshBuffer(0)->getMaterial().DiffuseColor );
		smgr->getMeshManipulator()->setVertexColors( cubeMesh->getMeshBuffer(1), cubeMesh->getMeshBuffer(1)->getMaterial().DiffuseColor );
		smgr->getMeshManipulator()->setVertexColors( cubeMesh->getMeshBuffer(2), cubeMesh->getMeshBuffer(2)->getMaterial().DiffuseColor );
		smgr->getMeshManipulator()->setVertexColors( cubeMesh->getMeshBuffer(3), cubeMesh->getMeshBuffer(3)->getMaterial().DiffuseColor );
		smgr->getMeshManipulator()->setVertexColors( cubeMesh->getMeshBuffer(4), cubeMesh->getMeshBuffer(4)->getMaterial().DiffuseColor );
		smgr->getMeshManipulator()->setVertexColors( cubeMesh->getMeshBuffer(5), cubeMesh->getMeshBuffer(5)->getMaterial().DiffuseColor );

		eventReceiver.BackgroundCube = smgr->addMeshSceneNode( cubeMesh );
		eventReceiver.BackgroundCube->setScale( core::vector3df( 200, 200, 200 ) );
		eventReceiver.BackgroundCube->setMaterialFlag( video::EMF_LIGHTING, false );
		eventReceiver.BackgroundCube->setVisible(false);
	}

#ifdef CUBEMAP_UPSIDE_DOWN_GL_PROJECTION
	if ( driverType == video::EDT_OPENGL )
	{
		// Flip projection matrix (note this also flips front/backface culling)
		core::matrix4 matProj = cubeMapCamera->getProjectionMatrix();
		matProj[4] = -matProj[4];
		matProj[5] = -matProj[5];
		matProj[6] = -matProj[6];
		matProj[7] = -matProj[7];
		cubeMapCamera->setProjectionMatrix(matProj);
	}
#endif


	// set a nicer font
	gui::IGUISkin* skin = env->getSkin();
	gui::IGUIFont* font = env->getFont(getExampleMediaPath() + "fonthaettenschweiler.bmp");
	if (font)
		skin->setFont(font);
	
	// Add some UI
	if ( eventReceiver.Shader )
	{
		skin->setColor(gui::EGDC_3D_FACE, video::SColor(50, 160, 120, 120));

		u32 top = dimDevice.Height - 180;
		const u32 left = dimDevice.Width - 350;
		const u32 right = dimDevice.Width - 10;
		irr::gui::IGUIStaticText * stextUVW = env->addStaticText(L" Style of generating texture coordinates:\n Change with (space)", core::recti(left, top, right, top+35), false, true, 0, -1, true);
		top += 40;
		stextUVW->setTextAlignment(gui::EGUIA_UPPERLEFT, gui::EGUIA_UPPERLEFT);
		eventReceiver.CurrentStyleUVW = env->addStaticText(L"", core::recti(240,0, 400, 20), false, false, stextUVW);
		eventReceiver.updateStyleUVW();

		irr::gui::IGUIStaticText * stextRoughness = env->addStaticText(L" Roughness:\n Change with (+) and (-)", core::recti(left, top, right, top+35), false, true, 0, -1, true);
		top += 40;
		eventReceiver.CurrentRoughness = env->addStaticText( L"", core::recti(240,0, 400, 20), false, false, stextRoughness);
		eventReceiver.updateRoughness();

		irr::gui::IGUIStaticText * stextSeamlessCupemap = env->addStaticText(L" Seamless cubemap (with roughness):\n Change with (s)", core::recti(left, top, right, top+35), false, true, 0, -1, true);
		top += 40;
		eventReceiver.CurrentSeamlessCubemap = env->addStaticText( L"", core::recti(240,0, 400, 20), false, false, stextSeamlessCupemap);
		eventReceiver.updateSeamless();

		irr::gui::IGUIStaticText * stextUpdates = env->addStaticText(L" Cubemap updates:\nChange with (u)", core::recti(left, top, right, top+35), false, true, 0, -1, true);
		top += 40;
		eventReceiver.CurrentCubemapUpdates = env->addStaticText( L"", core::recti(240,0, 400, 20), false, false, stextUpdates);
		eventReceiver.updateCubemapUpdates();

		env->addStaticText(L" Change background with (b)", core::recti(left, top, right, top+15), false, true, 0, -1, true);
	}


	// Main loop
	while(device->run())
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true);

			if( dynamicCubeMapRTT && sphereNode && eventReceiver.CheckCubemapUpdate() )
			{
#ifdef CUBEMAP_UPSIDE_DOWN_GL_PROJECTION
				core::array<scene::ISceneNode*> allNodes;
				if ( driverType == video::EDT_OPENGL )
				{
					// Flipping projection matrix flips front/backface culling. 
					// We only have a skybox so in this case this still would be fast, with more objects it's getting more ugly.
					smgr->getSceneNodesFromType(scene::ESNT_ANY, allNodes);
					flipCullingFlags(allNodes);
				}
#endif
				// If rendered just once then this node has still a white (or even undefined) texture at this point
				// Just hiding it and render the background when rendering the cubemap for the other node is less noticable
				// than having a big white dot in the texture.
				if (sphereNode3)
					sphereNode3->setVisible(false);	// the renderCubeMap below will make it visible again
				
				renderCubeMap(driver, cubeMapCamera, sphereNode, cubeMapRT, dynamicCubeMapRTT, depthStencilRTT);

				// If we want mipmaps in the dynamic cubemap we have to copy it to a non-rtt texture.
				if ( sphereNode3 )
				{
					renderCubeMap(driver, cubeMapCamera, sphereNode3, cubeMapRT, dynamicCubeMapRTT_intermediate, depthStencilRTT);
					for ( int i=0; i<6; ++i)
					{
						void * rtData = dynamicCubeMapRTT_intermediate->lock(video::ETLM_READ_ONLY, 0, i, video::ETLF_NONE);
						void * tData = dynamicCubeMapTex->lock(video::ETLM_READ_WRITE, 0, i);
						memcpy(tData, rtData, dynamicCubeMapTex->getPitch()*dynamicCubeMapTex->getSize().Width);
						dynamicCubeMapRTT_intermediate->unlock();
						dynamicCubeMapTex->unlock();
						dynamicCubeMapTex->regenerateMipMapLevels();
					}
				}
				
#ifdef CUBEMAP_UPSIDE_DOWN_GL_PROJECTION
				if ( driverType == video::EDT_OPENGL )
				{
					flipCullingFlags(allNodes);
				}
#endif
			}
			
			smgr->drawAll();
			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();
	
	return 0;
}
