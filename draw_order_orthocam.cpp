// Written by MartinVee to hunt some troubles with order of drawing in d3d
// See http://irrlicht.sourceforge.net/forum/viewtopic.php?f=1&t=51598

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
 
#ifdef _IRR_WINDOWS_
  #pragma comment(lib, "Irrlicht.lib")
  #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

#define NB_STRESS_TEST (10000)
 
#define SCREEN_WIDTH (1024)
#define SCREEN_HEIGHT (768)
 
 
int main(int argc, const char* argv[])
{
	video::E_DRIVER_TYPE deviceType = video::EDT_DIRECT3D9;
	//video::E_DRIVER_TYPE deviceType = video::EDT_OPENGL;
	//video::E_DRIVER_TYPE deviceType = video::EDT_BURNINGSVIDEO;
	IrrlichtDevice *device = createDevice(deviceType, dimension2d<u32>(SCREEN_WIDTH, SCREEN_HEIGHT), 32, false, false, false, NULL);
 
	if(device)
	{
		IVideoDriver* driver = device->getVideoDriver();
		ISceneManager* smgr = device->getSceneManager();
 
		// Declare a screensize object using the values from the driver
		const core::dimension2d<u32> screenSize = driver->getScreenSize();
 
		// Add a camera node
		irr::core::vector3df camPos((f32)screenSize.Width / 2, -((f32)(screenSize.Height / 2)), -1);
		irr::core::vector3df camTarget((f32)screenSize.Width / 2, -((f32)(screenSize.Height / 2)), 100);
		ICameraSceneNode *cam = device->getSceneManager()->addCameraSceneNode(0, camPos, camTarget);
 
		// Create a projection matrix to translate a 3D view to the exact screen size, and an orthographic field of view of 1000 units.
		matrix4 projectionMatrix;
		projectionMatrix.buildProjectionMatrixOrthoLH((f32)screenSize.Width, (f32)screenSize.Height, 0.0f, 1000.0f);
 
		// Apply the projection matrix to the camera.
		cam->setProjectionMatrix(projectionMatrix, true);
 
		// The world and the view needs to be reset to the identity matrix, in order to remove any matrix transformation for field of view.
		driver->setTransform(video::ETS_WORLD, core::matrix4());
		driver->setTransform(video::ETS_VIEW, core::matrix4());
 
		// Note that performance is not the issue here.
		ISceneNode* testNodes[NB_STRESS_TEST];
		for(int iNode = 0; iNode < NB_STRESS_TEST; iNode++)
		{
			testNodes[iNode] = smgr->addBillboardSceneNode(NULL, core::dimension2df((f32)8, (f32)8));
			testNodes[iNode]->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
			testNodes[iNode]->setMaterialFlag(video::EMF_LIGHTING, false);
		}
 
		// Gap of 25 pixels on the sides as a validation that the billboards are really white.
		dimension2du gap = dimension2du(25, 25);
		// Size of a colored billboard
		dimension2du cbSize = dimension2du(screenSize.Width/2 - gap.Width, screenSize.Height/2 - gap.Height);
 
		// The four semi-transparent billboards.
		ISceneNode* transparentBillboards[4];
		// Colors of the billboards
		SColor tbColors[4] =
		{
			SColor(150, 255, 0, 0),
			SColor(150, 255, 255, 0),
			SColor(150, 0, 255, 255),
			SColor(150, 0, 0, 255),
		};
 
		transparentBillboards[0] = smgr->addBillboardSceneNode(NULL, core::dimension2df((f32)cbSize.Width, (f32)cbSize.Height), core::vector3df(gap.Width + (cbSize.Width / 2), -(f32)gap.Height - (cbSize.Height / 2), 1.0f), -1, tbColors[0], tbColors[0]);
		transparentBillboards[1] = smgr->addBillboardSceneNode(NULL, core::dimension2df((f32)cbSize.Width, (f32)cbSize.Height), core::vector3df(screenSize.Width - gap.Width - (cbSize.Width / 2), -(f32)gap.Height - (cbSize.Height / 2), 1.0f), -1, tbColors[1], tbColors[1]);
		transparentBillboards[2] = smgr->addBillboardSceneNode(NULL, core::dimension2df((f32)cbSize.Width, (f32)cbSize.Height), core::vector3df(gap.Width + (cbSize.Width / 2), -(f32)screenSize.Height + gap.Height + (cbSize.Height / 2), 1.0f), -1, tbColors[2], tbColors[2]);
		transparentBillboards[3] = smgr->addBillboardSceneNode(NULL, core::dimension2df((f32)cbSize.Width, (f32)cbSize.Height), core::vector3df(screenSize.Width - gap.Width - (cbSize.Width / 2), -(f32)screenSize.Height + gap.Height + (cbSize.Height / 2), 1.0f), -1, tbColors[3], tbColors[3]);
 
		for(int iNode = 0; iNode < 4; iNode++)
		{
			transparentBillboards[iNode]->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
			transparentBillboards [iNode]->setMaterialFlag(video::EMF_LIGHTING, false);
		}
 
		u32 currentTime = device->getTimer()->getTime();
		while(device->run())
		{
			// Poor man's way of randomizing the white billboards position at each 200 ms
			if(device->getTimer()->getTime() > currentTime + 200)
			{
				for(int i = 0; i < NB_STRESS_TEST; i++)
				{
					f32 X = (rand() % (screenSize.Width - 4));
					f32 Y = 0.0f - (rand() % (screenSize.Height - 4));
					f32 Z = 50.0f;
					testNodes[i]->setPosition(core::vector3df(X, Y, Z));
				}
				currentTime = device->getTimer()->getTime();
			}
 
			// The rendering
			driver->beginScene(true, true, SColor(255, 0, 0, 0));
			smgr->drawAll();
			driver->endScene();
		}
	
		device->drop();	
	}

	return 0;
}
