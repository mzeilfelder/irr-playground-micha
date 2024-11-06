// Code is under the zlib license (same as Irrlicht)
// 
// Geometry shader example
// Based on Irrlicht example "10.Shaders"
// Modified by Michael Zeilfelder
//
// It requires files from the shader subfolder 
// as well as from the Irrlicht media folder

#include <irrlicht.h>
#include <iostream>
#include "exampleHelper.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

IrrlichtDevice* device = 0;

class MyShaderCallBack : public video::IShaderConstantSetCallBack
{
public:
	MyShaderCallBack() : WorldID(-1), ViewID(-1), ProjectionID(-1), LightPosID(-1),
						ColorID(-1), TextureID(-1), EmissiveID(-1)
	{
		for ( int i=0; i<4; ++i )
			Emissive[i] = 0.f;
	}

	virtual void OnCreate(video::IMaterialRendererServices* services, s32 userData)
	{
		// Get shader constants id.
		// Constants are "uniforms" in other shading languages.
		// And they are not constant at all but can be changed before every draw call
		// (the naming probably comes from Direct3D where they are called constants)
		WorldID = services->getVertexShaderConstantID("mWorld");
		ViewID = services->getVertexShaderConstantID("mView");
		ProjectionID = services->getVertexShaderConstantID("mProjection");
		LightPosID = services->getVertexShaderConstantID("mLightPos");
		ColorID = services->getVertexShaderConstantID("mLightColor");
		EmissiveID = services->getPixelShaderConstantID("mEmissive");

		// Textures ID are important only for OpenGL interface.
		video::IVideoDriver* driver = services->getVideoDriver();
		if(driver->getDriverType() == video::EDT_OPENGL)
			TextureID = services->getVertexShaderConstantID("myTexture");

		// Set light color 
		// That could be set as well in OnSetConstants, but there's some cost to setting shader constants
		// So when we have non-changing shader constants it's more performant to set them only once.
		video::SColorf col(0.0f,1.0f,1.0f,0.0f);
		services->setVertexShaderConstant(ColorID, reinterpret_cast<f32*>(&col), 4);
	}

	// Called when any SMaterial value changes
	virtual void OnSetMaterial(const irr::video::SMaterial& material)
	{
		// Remember material values to pass them on to shader in OnSetConstants
		Emissive[0] = material.EmissiveColor.getRed() / 255.0f;
		Emissive[1] = material.EmissiveColor.getGreen() / 255.0f;
		Emissive[2] = material.EmissiveColor.getBlue() / 255.0f;
		Emissive[3] = material.EmissiveColor.getAlpha() / 255.0f;
	}

	virtual void OnSetConstants(video::IMaterialRendererServices* services,
			s32 userData)
	{
		video::IVideoDriver* driver = services->getVideoDriver();

		// For now we do most matrix computations on CPU 
		// Could pre-calculate some matrix multiplications here instead of shader.
		irr::core::matrix4 world = driver->getTransform(video::ETS_WORLD);
		services->setVertexShaderConstant(WorldID, world.pointer(), 16);

		core::matrix4 view = driver->getTransform(video::ETS_VIEW);
		services->setVertexShaderConstant(ViewID, view.pointer(), 16);
	
		core::matrix4 projection = driver->getTransform(video::ETS_PROJECTION);
		services->setVertexShaderConstant(ProjectionID, projection.pointer(), 16);

		// set camera position
		core::vector3df pos = device->getSceneManager()->
			getActiveCamera()->getAbsolutePosition();
		services->setVertexShaderConstant(LightPosID, reinterpret_cast<f32*>(&pos), 3);

		// set texture, for textures you can use both an int and a float setPixelShaderConstant interfaces (You need it only for an OpenGL driver).
		s32 TextureLayerID = 0;
		services->setPixelShaderConstant(TextureID, &TextureLayerID, 1);

		// Set material values
		services->setPixelShaderConstant(EmissiveID, Emissive, 4);
	}

private:
	s32 WorldID;
	s32 ViewID;
	s32 ProjectionID;
	s32 LightPosID;
	s32 ColorID;
	s32 TextureID;

	s32 EmissiveID;
	irr::f32 Emissive[4];
};

int main()
{
	// No choice for driver - Irrlicht only has geometry shaders for OpenGL so far
	video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;

	device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (!device)
		return 1; // could not create selected driver.


	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* gui = device->getGUIEnvironment();

	smgr->getParameters()->setAttribute(scene::DEBUG_NORMAL_LENGTH, 10.f);


	const io::path shaderPath = "shader/";

	io::path vsFileName = shaderPath + "opengl.vert";
	io::path gsFileName = shaderPath + "opengl.geom";
	io::path psFileName = shaderPath + "opengl.frag";

	// create materials
	video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	s32 newMaterialType1 = 0;
	s32 newMaterialType2 = 0;
	if (gpu)
	{
		MyShaderCallBack* mcSolid = new MyShaderCallBack();
		MyShaderCallBack* mcTransparentAdd = new MyShaderCallBack();

		scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES;
		scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP;
		u32 verticesOut = 0;	// sets it to the max - we set real number in shader

		newMaterialType1 = gpu->addHighLevelShaderMaterialFromFiles(
			vsFileName, "vertexMain", video::EVST_VS_1_1,
			psFileName, "pixelMain", video::EPST_PS_1_1,
			gsFileName, "main", video::EGST_GS_4_0, inType, outType, verticesOut,
			mcSolid, video::EMT_TRANSPARENT_ALPHA_CHANNEL, 0);

		newMaterialType2 = gpu->addHighLevelShaderMaterialFromFiles(
			vsFileName, "vertexMain", video::EVST_VS_1_1,
			psFileName, "pixelMain", video::EPST_PS_1_1,
			gsFileName, "main", video::EGST_GS_4_0, inType, outType, verticesOut,
			mcTransparentAdd, video::EMT_TRANSPARENT_ADD_COLOR, 0);

		mcSolid->drop();
		mcTransparentAdd->drop();
	}

	const io::path mediaPath = getExampleMediaPath();

	// create test scene node 1, with the new created material type 1
	scene::ISceneNode* node = smgr->addCubeSceneNode(50);
	node->setPosition(core::vector3df(0,0,-100));
	node->setMaterialTexture(0, driver->getTexture(mediaPath + "wall.bmp"));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType1);

	smgr->addTextSceneNode(gui->getBuiltInFont(),
			L"Shader & EMT_TRANSPARENT_ALPHA_CHANNEL",
			video::SColor(255,255,255,255),	node);

	scene::ISceneNodeAnimator* anim = smgr->createRotationAnimator(
			core::vector3df(0,0.3f,0));
	node->addAnimator(anim);
	anim->drop();

	// create test scene node 2, with the new created material type 2
	node = smgr->addCubeSceneNode(50);
	node->setPosition(core::vector3df(0,-10,100)); 
	node->setMaterialTexture(0, driver->getTexture(mediaPath + "wall.bmp"));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialFlag(video::EMF_BLEND_OPERATION, true);
	node->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType2);
	node->getMaterial(0).EmissiveColor = irr::video::SColor(0,50,0,50);

	smgr->addTextSceneNode(gui->getBuiltInFont(),
			L"Shader & EMT_TRANSPARENT_ADD_COLOR",
			video::SColor(255,255,255,255),	node);

	anim = smgr->createRotationAnimator(core::vector3df(0,0.3f,0));
	node->addAnimator(anim);
	anim->drop();

	// add a scene node with no shader for comparison
	node = smgr->addCubeSceneNode(50);
	node->setPosition(core::vector3df(0,100,0));
	node->setMaterialTexture(0, driver->getTexture(mediaPath + "wall.bmp"));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setDebugDataVisible(irr::scene::EDS_NORMALS);
	smgr->addTextSceneNode(gui->getBuiltInFont(), L"NO SHADER",
		video::SColor(255,255,255,255), node);

	// add a nice skybox
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	smgr->addSkyBoxSceneNode(
		driver->getTexture(mediaPath + "irrlicht2_up.jpg"),
		driver->getTexture(mediaPath + "irrlicht2_dn.jpg"),
		driver->getTexture(mediaPath + "irrlicht2_lf.jpg"),
		driver->getTexture(mediaPath + "irrlicht2_rt.jpg"),
		driver->getTexture(mediaPath + "irrlicht2_ft.jpg"),
		driver->getTexture(mediaPath + "irrlicht2_bk.jpg"));
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	// add a camera and disable the mouse cursor
	scene::ICameraSceneNode* cam = smgr->addCameraSceneNodeFPS();
	cam->setPosition(core::vector3df(-250,50,0));
	cam->setTarget(core::vector3df(0,0,0));
	device->getCursorControl()->setVisible(false);

	//	draw everything
	int lastFPS = -1;

	while(device->run())
		if (device->isWindowActive())
	{
		driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,0,0,0));
		smgr->drawAll();
		driver->endScene();

		int fps = driver->getFPS();

		if (lastFPS != fps)
		{
			core::stringw str = driver->getName();
			str += L" FPS:";
			str += fps;

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
	}

	device->drop();

	return 0;
}
