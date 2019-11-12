// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Test filter-settings and mipmaps

#include <irrlicht.h>
#include <iostream>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class App : public IEventReceiver	// Lazily mixing application class and event-receiver
{
public:
	App(IrrlichtDevice * irrDevice) 
	{
		SceneManager = irrDevice->getSceneManager();
		GuiEnv = irrDevice->getGUIEnvironment();
		VideoDriver =  irrDevice->getVideoDriver();	
		CursorControl = irrDevice->getCursorControl();
		CursorControl->setVisible(false);

		FpsCam = SceneManager->addCameraSceneNodeFPS(0, 20.f, 0.1f, -1, 0, 0, true );
		FpsCam->setNearValue(10.f);
		FpsCam->setFarValue(5000.f);

		irrDevice->setEventReceiver(this);

		irr::gui::IGUIFont * font = GuiEnv->getFont("../../media/fonthaettenschweiler.bmp");
		if ( font )
			GuiEnv->getSkin()->setFont(font);
	}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			if ( event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
			{
				if ( event.GUIEvent.Caller == CbBilinear )
				{
					NodeMaterial.TextureLayer[0].BilinearFilter = CbBilinear->isChecked();
				}
				else if ( event.GUIEvent.Caller == CbTrilinear )
				{
					NodeMaterial.TextureLayer[0].TrilinearFilter = CbTrilinear->isChecked();
				}
				else if ( event.GUIEvent.Caller == CbAnisotropic )
				{
					NodeMaterial.TextureLayer[0].AnisotropicFilter = CbAnisotropic->isChecked() ? 16 : 0;
				}
				else if ( event.GUIEvent.Caller == CbMipmaps )
				{
					NodeMaterial.UseMipMaps = CbMipmaps->isChecked();
				}
				UpdateNodesMaterial();
			}
		}
		else if ( event.EventType == EET_MOUSE_INPUT_EVENT )
		{
			if ( event.MouseInput.isLeftPressed() && !FpsCam->isInputReceiverEnabled() && GuiEnv->getHovered() == GuiEnv->getRootGUIElement())
			{
				FpsCam->setInputReceiverEnabled(true);
				CursorControl->setPosition(0.5f, 0.5f);
				CursorControl->setVisible(false);
			}
			else if ( !event.MouseInput.isLeftPressed() && FpsCam->isInputReceiverEnabled() )
			{
				FpsCam->setInputReceiverEnabled(false);
				CursorControl->setVisible(true);
			}
		}

		return false;
	}

	void AddNodes()
	{
		const f32 near = FpsCam->getNearValue();
		const f32 far = FpsCam->getFarValue();
		const f32 depth = far-near;

		MeshNodes.push_back(SceneManager->addCubeSceneNode (50.f, 0, -1,core::vector3df(0, 0, 120)));
		MeshNodes.push_back(SceneManager->addCubeSceneNode (50.f, 0, -1, core::vector3df(150, -100, 250)));
		MeshNodes.push_back(SceneManager->addCubeSceneNode (500.f, 0, -1, core::vector3df(500, 0, 1000)));
		MeshNodes.push_back(SceneManager->addCubeSceneNode (depth, 0, -1, core::vector3df(0, -depth*0.7f, depth / 2.f)));

		for ( int i = 0; i < 4; ++i )
			MeshNodes.push_back(SceneManager->addSphereSceneNode(50.f + i * 10.f, 32, 0, -1, core::vector3df(300 - i * 150, 100, 500)));

		const f32 cubeSize = 100.f;
		for ( float d = near + cubeSize/2 + 1.f; d < far - (cubeSize/2 + 1.f); d+= (depth-cubeSize) / 12.f )
		{
			MeshNodes.push_back(SceneManager->addCubeSceneNode (cubeSize, 0, -1, core::vector3df(-1.5f*cubeSize + d, d/2.f, d)));
		}

		// default material settings
		NodeMaterial.Lighting = false;
		UpdateNodesMaterial();
	}

	void SetNodesTexture(const irr::io::path& textureName)
	{
		NodeMaterial.setTexture(0, VideoDriver->getTexture(textureName));
		UpdateNodesMaterial();
	}

	void AddGuiElements()
	{
		irr::core::recti r(10, 10, 100, 25);
		gui::IGUIElement* parent = 0;
		CbBilinear = GuiEnv->addCheckBox(NodeMaterial.TextureLayer[0].BilinearFilter, r, parent, -1, L"bilinear");
		CbBilinear->setDrawBackground(true);
		r += irr::core::position2di(0, 30);
		CbTrilinear = GuiEnv->addCheckBox(NodeMaterial.TextureLayer[0].TrilinearFilter, r, parent, -1, L"trilinear");
		CbTrilinear->setDrawBackground(true);
		r += irr::core::position2di(0, 30);
		CbAnisotropic = GuiEnv->addCheckBox(NodeMaterial.TextureLayer[0].AnisotropicFilter > 0, r, parent, -1, L"anisotropic");
		CbAnisotropic->setDrawBackground(true);
		r += irr::core::position2di(0, 30);
		CbMipmaps = GuiEnv->addCheckBox(NodeMaterial.UseMipMaps, r, parent, -1, L"mipmaps");
		CbMipmaps->setDrawBackground(true);
	}

	scene::ISceneManager* SceneManager;
	gui::IGUIEnvironment* GuiEnv;
    video::IVideoDriver* VideoDriver;
	gui::ICursorControl* CursorControl;
	core::array<scene::IMeshSceneNode *> MeshNodes;
	scene::ICameraSceneNode * FpsCam;
	video::SMaterial NodeMaterial;

	gui::IGUICheckBox * CbBilinear;
	gui::IGUICheckBox * CbTrilinear;
	gui::IGUICheckBox * CbAnisotropic;
	gui::IGUICheckBox * CbMipmaps;

protected:
	void UpdateNodesMaterial()
	{
		for ( u32 i=0; i < MeshNodes.size(); ++i )
			MeshNodes[i]->getMaterial(0) = NodeMaterial;
	}
};


int main(int argc, char *argv[])
{
	SIrrlichtCreationParameters params;
#if 1
	params.DriverType = video::EDT_OPENGL;
#else
	params.DriverType = video::EDT_DIRECT3D9;
#endif
	params.WindowSize = core::dimension2d<u32>(1024,768);
	IrrlichtDevice * device = createDeviceEx(params);
	if (!device)
		return false;

	device->setWindowCaption(L"Move cam with mouse-button pressed and cursor keys");

	App app(device);

	// Add some nodes to show a texture at different distances
	app.AddNodes();

	// Set a simple material with texture
	app.SetNodesTexture("my_media/patterns_270x270.png");

	app.AddGuiElements();

	while ( device->run() )
	{
		if ( device->isWindowActive() )
		{
			app.VideoDriver->beginScene(true, true, video::SColor(255,25,25,25));

			app.SceneManager->drawAll();
			app.GuiEnv->drawAll();

			app.VideoDriver->endScene();
		}
		device->sleep( 1 );
	}
	
	device->closeDevice();
	device->drop();
	
	return 0;
}
