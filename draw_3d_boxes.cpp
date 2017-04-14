// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Draw boxes

#include <irrlicht.h>

using namespace irr;
using namespace core;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

void setBoxBuffer(scene::SMeshBuffer * mb, const core::aabbox3df& box, video::SColor color)
{
	core::vector3df edges[8];
	box.getEdges(edges);

	for(u32 i = 0; i < 24; i++)
		mb->Vertices[i].Color = color;

	mb->Vertices[0].Pos = edges[5];
	mb->Vertices[1].Pos = edges[1];
	mb->Vertices[2].Pos = edges[1];
	mb->Vertices[3].Pos = edges[3];
	mb->Vertices[4].Pos = edges[3];
	mb->Vertices[5].Pos = edges[7];
	mb->Vertices[6].Pos = edges[7];
	mb->Vertices[7].Pos = edges[5];
	mb->Vertices[8].Pos = edges[0];
	mb->Vertices[9].Pos = edges[2];
	mb->Vertices[10].Pos = edges[2];
	mb->Vertices[11].Pos = edges[6];
	mb->Vertices[12].Pos = edges[6];
	mb->Vertices[13].Pos = edges[4];
	mb->Vertices[14].Pos = edges[4];
	mb->Vertices[15].Pos = edges[0];
	mb->Vertices[16].Pos = edges[1];
	mb->Vertices[17].Pos = edges[0];
	mb->Vertices[18].Pos = edges[3];
	mb->Vertices[19].Pos = edges[2];
	mb->Vertices[20].Pos = edges[7];
	mb->Vertices[21].Pos = edges[6];
	mb->Vertices[22].Pos = edges[5];
	mb->Vertices[23].Pos = edges[4];
	mb->setDirty();
}

int main()
{
	IrrlichtDevice *  device = createDevice(irr::video::EDT_DIRECT3D9, irr::core::dimension2d<irr::u32>(800,600));
	if (!device)
		return false;

	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver * videoDriver = device->getVideoDriver ();

	irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );
	camera->updateAbsolutePosition();
	camera->setTarget(core::vector3df(0,0,0));
	camera->updateAbsolutePosition();
	camera->setPosition(irr::core::vector3df(0, 50, -100));
	camera->updateAbsolutePosition();

	irr::video::SMaterial m;
	m.Lighting = false;
	m.Thickness = 5.f;

	videoDriver->setMinHardwareBufferVertexCount(0); // for testing hw buffers
	// create meshbuffer for the box
	scene::SMeshBuffer * boxMeshBuffer = new scene::SMeshBuffer();
	boxMeshBuffer->setHardwareMappingHint(scene::EHM_STREAM, scene::EBT_VERTEX);	// will be ignored by driver (except you hack 
	boxMeshBuffer->setHardwareMappingHint(scene::EHM_STATIC, scene::EBT_INDEX);
	boxMeshBuffer->setPrimitiveType(scene::EPT_LINES);
	boxMeshBuffer->Vertices.set_used(24);
	boxMeshBuffer->Indices.set_used(24);
	for ( int i=0; i<24; ++i )
		boxMeshBuffer->Indices[i] = i;

	s32 lastFPS = -1;
	while ( device->run() )
	{
		if ( device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			videoDriver->setMaterial(m);
			videoDriver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

			for ( float f=1.f; f<1000.f; f += 1.1f )
			{
				core::aabbox3df box(-f, -f, -f, f, f, f);
				video::SColor col(255, f, 255-f, 127+f);

				//videoDriver->draw3DBox( box, col);

				setBoxBuffer(boxMeshBuffer, box, col);
				videoDriver->drawMeshBuffer(boxMeshBuffer);
			}

			smgr->drawAll();

			videoDriver->endScene();

			s32 fps = videoDriver->getFPS();
			if (lastFPS != fps)
			{
				core::stringw str = L"FPS: ";
				str += fps;
				device->setWindowCaption(str.c_str());
				lastFPS = fps;
			}
		}

		device->sleep( 5 );
	}

	device->closeDevice();
	device->drop();
	device = NULL;

	return 0;
}
