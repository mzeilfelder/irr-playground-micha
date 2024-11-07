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

		// set texture, for textures you can use both an int and a float setPixelShaderConstant interfaces (You need it only for an OpenGL driver).
		s32 TextureLayerID = 0;
		services->setPixelShaderConstant(TextureID, &TextureLayerID, 1);
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

	const io::path mediaPath = getExampleMediaPath();
	const io::path shaderPath = "shader/";

	io::path vsFileName = shaderPath + "ogl_gs.vert";
	io::path psFileName = shaderPath + "ogl_gs.frag";

	// create materials
	video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	if ( !gpu )
		return 1; // can't do shaders without this


	u32 verticesOut = 0;	// sets it to the max, 0 is allow maximum. Can override in geometry shader with layout out parameter max_vertices 

	MyShaderCallBack* mc1 = new MyShaderCallBack();
	// can override in geometry shader with layout in/out
	scene::E_PRIMITIVE_TYPE inType1 = scene::EPT_TRIANGLES; // EPT_POINTS;
	scene::E_PRIMITIVE_TYPE outType1 = scene::EPT_TRIANGLE_STRIP;
	s32 newMaterialType1 = gpu->addHighLevelShaderMaterialFromFiles(
		vsFileName, "vertexMain", video::EVST_VS_1_1,
		psFileName, "pixelMain", video::EPST_PS_1_1,
		shaderPath + "ogl_spikes.geom", "main", video::EGST_GS_4_0, inType1, outType1, verticesOut,
		mc1, video::EMT_TRANSPARENT_ALPHA_CHANNEL, 0);
	mc1->drop();

	MyShaderCallBack* mc2 = new MyShaderCallBack();
	scene::E_PRIMITIVE_TYPE inType2 = scene::EPT_TRIANGLES;
	scene::E_PRIMITIVE_TYPE outType2 = scene::EPT_TRIANGLE_STRIP;
	s32 newMaterialType2 = gpu->addHighLevelShaderMaterialFromFiles(
		vsFileName, "vertexMain", video::EVST_VS_1_1,
		psFileName, "pixelMain", video::EPST_PS_1_1,
		shaderPath + "ogl_shells.geom", "main", video::EGST_GS_4_0, inType2, outType2, verticesOut,
		mc2, video::EMT_TRANSPARENT_ADD_COLOR, 0);	// Not using pixel alpha in shader with that one
	mc2->drop();

	// create test scene node 1, with the new created material type 1
#if 1 // sphere
	scene::IMeshSceneNode* node = smgr->addSphereSceneNode(25);
#else	// cube
	scene::IMeshSceneNode* node = smgr->addCubeSceneNode(50);
#endif
	node->setPosition(core::vector3df(0,0,-100));
	node->setMaterialTexture(0, driver->getTexture(mediaPath + "wall.bmp"));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType1);
	node->getMaterial(0).EmissiveColor = video::SColor(100,150,150,150);
	
	node->getMesh()->getMeshBuffer(0)->setPrimitiveType(inType1);

	smgr->addTextSceneNode(gui->getBuiltInFont(), L"1", video::SColor(255,200,155,155),	node);

	scene::ISceneNodeAnimator* anim = smgr->createRotationAnimator(
			core::vector3df(0,0.3f,0));
	node->addAnimator(anim);
	anim->drop();

#if 1 // create test node 2, with the new created material type 2
	node = smgr->addCubeSceneNode(50);
	node->setPosition(core::vector3df(0,-10,100)); 
	node->setMaterialTexture(0, driver->getTexture(mediaPath + "wall.bmp"));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialFlag(video::EMF_BLEND_OPERATION, true);
	node->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType2);
	node->getMaterial(0).EmissiveColor = irr::video::SColor(0,50,0,50);

	smgr->addTextSceneNode(gui->getBuiltInFont(), L"2",	video::SColor(255,200,155,155),	node);

	anim = smgr->createRotationAnimator(core::vector3df(0,0.3f,0));
	node->addAnimator(anim);
	anim->drop();
#endif

	// add a camera and disable the mouse cursor
	scene::ICameraSceneNode* cam = smgr->addCameraSceneNodeFPS();
	cam->setPosition(core::vector3df(-250,50,0));
	cam->setTarget(core::vector3df(0,0,0));
	device->getCursorControl()->setVisible(false);

	// With debug output enabled we want a bit longer normals
	smgr->getParameters()->setAttribute(scene::DEBUG_NORMAL_LENGTH, 10.f);

	//	draw everything
	int lastFPS = -1;

	while(device->run())
		if (device->isWindowActive())
	{
		driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,20,20,20));
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

	device->closeDevice();
	device->drop();

	return 0;
}
