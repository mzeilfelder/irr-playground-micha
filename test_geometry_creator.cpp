// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder

// Test geometry creator

#include <irrlicht.h>
 
using namespace irr;
using namespace core;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

namespace
{
	// some useful color constants
	const video::SColor SCOL_BLACK     = video::SColor(255, 0,   0,   0);
	const video::SColor SCOL_BLUE      = video::SColor(255, 0,   0,  255);
	const video::SColor SCOL_CYAN      = video::SColor(255, 0,  255, 255);
	const video::SColor SCOL_GRAY      = video::SColor(255, 128,128, 128);
	const video::SColor SCOL_GREEN     = video::SColor(255, 0,  255,  0);
	const video::SColor SCOL_MAGENTA   = video::SColor(255, 255, 0,  255);
	const video::SColor SCOL_RED       = video::SColor(255, 255, 0,   0);
	const video::SColor SCOL_YELLOW    = video::SColor(255, 255, 255, 0);
	const video::SColor SCOL_WHITE     = video::SColor(255, 255, 255, 255);
};

irr::video::ITexture* addCaroTexture(video::IVideoDriver * driver)
{
	const u32 width = 256;
	const u32 height = 256;
	video::IImage * imageA8R8G8B8 = driver->createImage (video::ECF_A8R8G8B8, core::dimension2d<u32>(width, height));
	if ( !imageA8R8G8B8 )
		return 0;
	const u32 pitch = imageA8R8G8B8->getPitch();

	// Some nice square-pattern with 9 typical colors
	// Note that the function put readability over speed, you shouldn't use setPixel at runtime but for initialization it's nice.
	for ( u32 y = 0; y < height; ++ y )
	{
		for ( u32 x = 0; x < pitch; ++x )
		{
			if ( y < height/3 )
			{
				if ( x < width/3 )
					imageA8R8G8B8->setPixel (x, y, SCOL_BLACK);
				else if ( x < 2*width/3 )
					imageA8R8G8B8->setPixel (x, y, SCOL_BLUE);
				else
					imageA8R8G8B8->setPixel (x, y, SCOL_CYAN);
			}
			else if ( y < 2*height/3 )
			{
				if ( x < width/3 )
					imageA8R8G8B8->setPixel (x, y, SCOL_GRAY);
				else if ( x < 2*width/3 )
					imageA8R8G8B8->setPixel (x, y, SCOL_GREEN);
				else
					imageA8R8G8B8->setPixel (x, y, SCOL_MAGENTA);
			}
			else
			{
				if ( x < width/3 )
					imageA8R8G8B8->setPixel (x, y, SCOL_RED);
				else if ( x < 2*width/3 )
					imageA8R8G8B8->setPixel (x, y, SCOL_YELLOW);
				else
					imageA8R8G8B8->setPixel (x, y, SCOL_WHITE);
			}
		}
	}

	video::ITexture* result = driver->addTexture (io::path("CARO_A8R8G8B8"), imageA8R8G8B8);

	imageA8R8G8B8->drop();

	return result;
}

irr::video::ITexture* addRadialTexture(irr::video::IVideoDriver* driver, const core::dimension2du& dim)
{
	irr::video::ITexture* result = 0;
	vector2df center(dim.Width*0.5f, dim.Height*0.5f);
	f32 radius = core::min_(center.X, center.Y);
	radius += 2;	// avoiding black gap at last line/row
	const u32 bytesPerPixel = 4;
	u8* data = new u8[dim.Width * dim.Height * bytesPerPixel];
	const irr::u32 pitch = dim.Width * bytesPerPixel;

	u32 stepsOutwards = 6;
	u32 stepsCircular = 12;

	for (u32 y = 0; y < dim.Height; ++y)
	{
		const u32 lineStart = y * pitch;
		for (u32 x = 0; x < dim.Width; ++x)
		{
			u32 idx = lineStart + x*bytesPerPixel;

			vector2df offset((f32)x-center.X, (f32)y-center.Y);
			f32 offsetLen = offset.getLength();
			if ( offsetLen > radius )
			{
				data[idx] = 0;
				data[idx+1] = 0;
				data[idx+2] = 0;
				data[idx+3] = 255;
			}
			else
			{
				int circleStep = (int)(offset.getAngle()*(float)stepsCircular/360.f);
				int outwardStep = (int)(offsetLen*(float)stepsOutwards/radius);
				data[idx] = 25+circleStep*230/stepsCircular;	// blue
				data[idx+1] = (data[idx]+127)%255;	// green
				data[idx+2] = 25+outwardStep*230/stepsOutwards;	// red
				data[idx+3] = 255;	// alpha
			}
		}
	}

	video::IImage* img = driver->createImageFromData(irr::video::ECF_A8R8G8B8, dim, data);
	if (img)
	{
		result = driver->addTexture("radialCheckerboard", img);
		img->drop();
	}
	delete[] data;

	return result;
}
 

int main()
{
	IrrlichtDevice *  Device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(1024,768));
	//IrrlichtDevice *  Device = createDevice(irr::video::EDT_DIRECT3D9, irr::core::dimension2d<irr::u32>(1024,768));
	if (!Device)
		return false;
   
	scene::ISceneManager* smgr = Device->getSceneManager();
	video::IVideoDriver * videoDriver = Device->getVideoDriver ();
	const scene::IGeometryCreator* geom = smgr->getGeometryCreator();

	core::dimension2du dimRadial(1024, 1024);
	video::ITexture* texCaro = addCaroTexture(videoDriver);
	video::ITexture* texRadial = addRadialTexture(videoDriver, dimRadial);
	video::ITexture* texChecker = videoDriver->getTexture("my_media/checker_map_4k.png");
   
	u32 tessellation = 89;
	scene::IMesh* coneMesh = geom->createConeMesh(50.f, 200.f, tessellation, video::SColor(255, 255, 255, 255), video::SColor(255, 255, 255, 255), 0.f);
	coneMesh->getMeshBuffer(0)->getMaterial().Lighting = false;
	coneMesh->getMeshBuffer(0)->getMaterial().UseMipMaps = false;
	coneMesh->getMeshBuffer(0)->getMaterial().GouraudShading = false;
	coneMesh->getMeshBuffer(0)->getMaterial().setTexture(0, texRadial);
	//coneMesh->getMeshBuffer(0)->getMaterial().setTexture(0, texChecker);
	coneMesh->getMeshBuffer(0)->getMaterial().TextureLayer[0].AnisotropicFilter = 16;
	coneMesh->getMeshBuffer(0)->getMaterial().TextureLayer[0].TrilinearFilter = true;
	smgr->addMeshSceneNode(coneMesh);
	coneMesh->drop();

	scene::IMesh* cylinderMesh = geom->createCylinderMesh(50.f, 200.f, tessellation, video::SColor(0xffffffff), true, 0.f, 0);
	cylinderMesh->getMeshBuffer(0)->getMaterial().Lighting = false;
	cylinderMesh->getMeshBuffer(0)->getMaterial().setTexture(0, texChecker);
	smgr->addMeshSceneNode(cylinderMesh, 0, -1, core::vector3df(200, 0, 0));
	cylinderMesh->drop();

	scene::IMesh* arrowMesh = geom->createArrowMesh(tessellation, tessellation, 200.f,
			160.f, 30.f, 50.f, video::SColor(0xFFFFFFFF), video::SColor(0xFFFFFFFF));
	arrowMesh->getMeshBuffer(0)->getMaterial().Lighting = false;
	arrowMesh->getMeshBuffer(0)->getMaterial().setTexture(0, texChecker);
	arrowMesh->getMeshBuffer(1)->getMaterial().Lighting = false;
	arrowMesh->getMeshBuffer(1)->getMaterial().setTexture(0, texChecker);
	smgr->addMeshSceneNode(arrowMesh, 0, -1, core::vector3df(-200, 0, 0));
	arrowMesh->drop();

	scene::IMesh* torusMesh = smgr->getGeometryCreator()->createTorusMesh(30, 10, 32, 16, 135.f, 350.f, 3);
	scene::ISceneNode * torusNode = smgr->addMeshSceneNode(torusMesh, 0, -1, core::vector3df(400, 0, 0));
	torusMesh->drop();
	if (torusNode)
	{
		torusNode->getMaterial(0).setTexture(0, texCaro);
		torusNode->setMaterialFlag(video::EMF_LIGHTING, false);
		torusNode->setDebugDataVisible(scene::EDS_NORMALS);
	}

	irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );
	camera->updateAbsolutePosition();
	camera->setTarget(irr::core::vector3df(0,0,0));
	camera->updateAbsolutePosition();
	camera->setPosition(irr::core::vector3df(0, -150, -300));
	camera->updateAbsolutePosition();

	while ( Device->run() )
	{
		if ( Device->isWindowActive() )
		{
			videoDriver->beginScene(true, true, video::SColor(255, 200, 200, 200));

			smgr->drawAll();

			videoDriver->draw2DImage(texRadial, recti(0,0,256,256), recti(dimRadial), 0, 0, true);
			videoDriver->draw2DImage(texChecker, recti(768,0, 1024, 256), recti(texChecker->getOriginalSize()));

			videoDriver->endScene();
		}

		Device->sleep(0);
	}

	Device->closeDevice();
	Device->drop();
	Device = NULL;

	return 0;
}
