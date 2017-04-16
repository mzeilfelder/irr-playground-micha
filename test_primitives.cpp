// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Draw meshbuffers with different primitives

#include <irrlicht.h>

using namespace irr;
using namespace core;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

void initDefaults(irr::scene::SMeshBuffer * mb, irr::u32 size, irr::video::SColor color)
{
	mb->setHardwareMappingHint(scene::EHM_STATIC);
	mb->Vertices.set_used(size);
	mb->Indices.set_used(size);
	for ( u32 i=0; i<mb->Indices.size(); ++i )
		mb->Indices[i] = i;
	for ( u32 i=0; i<mb->Vertices.size(); ++i )
		mb->Vertices[i].Color = color;
}

int main()
{
	IrrlichtDevice *  device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800,600));
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
	m.Thickness = 4.f;

	videoDriver->setMinHardwareBufferVertexCount(0); // for testing hw buffers

	// point meshbuffer
	scene::SMeshBuffer * pointMeshBuffer = new scene::SMeshBuffer();
	pointMeshBuffer->setPrimitiveType(scene::EPT_POINTS);
	initDefaults(pointMeshBuffer, 4, video::SColor(255, 255, 0, 0));
	pointMeshBuffer->Vertices[0].Pos = core::vector3df(-20, -10, 0);
	pointMeshBuffer->Vertices[1].Pos = core::vector3df(20, -10, 0);
	pointMeshBuffer->Vertices[2].Pos = core::vector3df(20, 10, 0);
	pointMeshBuffer->Vertices[3].Pos = core::vector3df(-20, 10, 0);


	// line_strip meshbuffer
	scene::SMeshBuffer * lineStripMeshBuffer = new scene::SMeshBuffer();
	lineStripMeshBuffer ->setPrimitiveType(scene::EPT_LINE_STRIP);
	initDefaults(lineStripMeshBuffer, 5, video::SColor(255, 255, 127, 0));
	lineStripMeshBuffer->Vertices[0].Pos = core::vector3df(-7, -7, 0);
	lineStripMeshBuffer->Vertices[1].Pos = core::vector3df(-3, -3, 0);
	lineStripMeshBuffer->Vertices[2].Pos = core::vector3df(0, -5, 0);
	lineStripMeshBuffer->Vertices[3].Pos = core::vector3df(3, -3, 0);
	lineStripMeshBuffer->Vertices[4].Pos = core::vector3df(7, -7, 0);

	// line_loop meshbuffer
	scene::SMeshBuffer * lineLoopMeshBuffer = new scene::SMeshBuffer();
	lineLoopMeshBuffer ->setPrimitiveType(scene::EPT_LINE_LOOP);
	initDefaults(lineLoopMeshBuffer, 5, video::SColor(255, 255, 255, 0));
	lineLoopMeshBuffer->Vertices[0].Pos = core::vector3df(-7, 7, 0);
	lineLoopMeshBuffer->Vertices[1].Pos = core::vector3df(-3, 3, 0);
	lineLoopMeshBuffer->Vertices[2].Pos = core::vector3df(0, 5, 0);
	lineLoopMeshBuffer->Vertices[3].Pos = core::vector3df(3, 3, 0);
	lineLoopMeshBuffer->Vertices[4].Pos = core::vector3df(7, 7, 0);

	// line meshbuffer
	scene::SMeshBuffer * linesMeshBuffer = new scene::SMeshBuffer();
	linesMeshBuffer->setPrimitiveType(scene::EPT_LINES);
	initDefaults(linesMeshBuffer, 6, video::SColor(255, 255, 255, 127));
	linesMeshBuffer->Vertices[0].Pos = core::vector3df(-7, 17, 0);
	linesMeshBuffer->Vertices[1].Pos = core::vector3df(-7, 13, 0);
	linesMeshBuffer->Vertices[2].Pos = core::vector3df(-2, 17, 0);
	linesMeshBuffer->Vertices[3].Pos = core::vector3df(2, 11, 0);
	linesMeshBuffer->Vertices[4].Pos = core::vector3df(7, 15, 0);
	linesMeshBuffer->Vertices[5].Pos = core::vector3df(7, 11, 0);

	// triangle_strip meshbuffer
	scene::SMeshBuffer * triangleStripMeshBuffer = new scene::SMeshBuffer();
	triangleStripMeshBuffer->setPrimitiveType(scene::EPT_TRIANGLE_STRIP);
	initDefaults(triangleStripMeshBuffer, 6, video::SColor(255, 255, 255, 255));
	triangleStripMeshBuffer->Vertices[0].Pos = core::vector3df(-7, -18, 0);
	triangleStripMeshBuffer->Vertices[1].Pos = core::vector3df(-9, -13, 0);
	triangleStripMeshBuffer->Vertices[2].Pos = core::vector3df(0, -15, 0);
	triangleStripMeshBuffer->Vertices[3].Pos = core::vector3df(0, -11, 0);
	triangleStripMeshBuffer->Vertices[4].Pos = core::vector3df(7, -18, 0);
	triangleStripMeshBuffer->Vertices[5].Pos = core::vector3df(9, -13, 0);

	// triangle_fan meshbuffer
	scene::SMeshBuffer * triangleFanMeshBuffer = new scene::SMeshBuffer();
	triangleFanMeshBuffer->setPrimitiveType(scene::EPT_TRIANGLE_FAN);
	initDefaults(triangleFanMeshBuffer, 6, video::SColor(255, 127, 255, 255));
	triangleFanMeshBuffer->Vertices[0].Pos = core::vector3df(0, -25, 0);
	triangleFanMeshBuffer->Vertices[1].Pos = core::vector3df(-7, -28, 0);
	triangleFanMeshBuffer->Vertices[2].Pos = core::vector3df(-9, -23, 0);
	triangleFanMeshBuffer->Vertices[3].Pos = core::vector3df(0, -21, 0);
	triangleFanMeshBuffer->Vertices[4].Pos = core::vector3df(9, -23, 0);
	triangleFanMeshBuffer->Vertices[5].Pos = core::vector3df(7, -28, 0);

	// triangle meshbuffer
	scene::SMeshBuffer * triangleMeshBuffer = new scene::SMeshBuffer();
	triangleMeshBuffer->setPrimitiveType(scene::EPT_TRIANGLES);
	initDefaults(triangleMeshBuffer, 6, video::SColor(255, 0, 255, 255));
	triangleMeshBuffer->Vertices[0].Pos = core::vector3df(0, -35, 0);
	triangleMeshBuffer->Vertices[1].Pos = core::vector3df(-7, -38, 0);
	triangleMeshBuffer->Vertices[2].Pos = core::vector3df(-9, -33, 0);
	triangleMeshBuffer->Vertices[3].Pos = core::vector3df(0, -31, 0);
	triangleMeshBuffer->Vertices[4].Pos = core::vector3df(9, -33, 0);
	triangleMeshBuffer->Vertices[5].Pos = core::vector3df(7, -38, 0);

	// quad_strip meshbuffer
	scene::SMeshBuffer * quadStripMeshBuffer = new scene::SMeshBuffer();
	quadStripMeshBuffer->setPrimitiveType(scene::EPT_QUAD_STRIP);
	initDefaults(quadStripMeshBuffer, 6, video::SColor(255, 0, 127, 255));
	quadStripMeshBuffer->Vertices[0].Pos = core::vector3df(17, -18, 0);
	quadStripMeshBuffer->Vertices[1].Pos = core::vector3df(15, -13, 0);
	quadStripMeshBuffer->Vertices[2].Pos = core::vector3df(24, -15, 0);
	quadStripMeshBuffer->Vertices[3].Pos = core::vector3df(24, -11, 0);
	quadStripMeshBuffer->Vertices[4].Pos = core::vector3df(31, -18, 0);
	quadStripMeshBuffer->Vertices[5].Pos = core::vector3df(33, -13, 0);

	// quad meshbuffer
	scene::SMeshBuffer * quadMeshBuffer = new scene::SMeshBuffer();
	quadMeshBuffer->setPrimitiveType(scene::EPT_QUADS);
	initDefaults(quadMeshBuffer, 8, video::SColor(255, 0, 0, 255));
	quadMeshBuffer->Vertices[0].Pos = core::vector3df(17, -28, 0);
	quadMeshBuffer->Vertices[1].Pos = core::vector3df(15, -23, 0);
	quadMeshBuffer->Vertices[3].Pos = core::vector3df(24, -25, 0);
	quadMeshBuffer->Vertices[2].Pos = core::vector3df(24, -21, 0);
	quadMeshBuffer->Vertices[4].Pos = core::vector3df(24, -25, 0);
	quadMeshBuffer->Vertices[5].Pos = core::vector3df(24, -21, 0);
	quadMeshBuffer->Vertices[7].Pos = core::vector3df(31, -28, 0);
	quadMeshBuffer->Vertices[6].Pos = core::vector3df(33, -23, 0);

	// polygon meshbuffer (TODO: not working - likely because OpenGL has deprecated it)
	scene::SMeshBuffer * polygonMeshBuffer = new scene::SMeshBuffer();
	polygonMeshBuffer->setPrimitiveType(scene::EPT_POLYGON);
	initDefaults(polygonMeshBuffer, 6, video::SColor(255, 128, 0, 128));
	polygonMeshBuffer->Vertices[0].Pos = core::vector3df(-27, -18, 0);
	polygonMeshBuffer->Vertices[1].Pos = core::vector3df(-29, -13, 0);
	polygonMeshBuffer->Vertices[2].Pos = core::vector3df(-20, -20, 0);
	polygonMeshBuffer->Vertices[3].Pos = core::vector3df(-11, -13, 0);
	polygonMeshBuffer->Vertices[4].Pos = core::vector3df(-13, -18, 0);
	polygonMeshBuffer->Vertices[5].Pos = core::vector3df(-20, -10, 0);


	// point sprite meshbuffer
	scene::SMeshBuffer * pointSpriteMeshBuffer = new scene::SMeshBuffer();
	pointSpriteMeshBuffer->setPrimitiveType(scene::EPT_POINT_SPRITES);
	initDefaults(pointSpriteMeshBuffer, 4, video::SColor(255, 0, 128, 0));
	pointSpriteMeshBuffer->Vertices[0].Pos = core::vector3df(-20, 20, 0);
	pointSpriteMeshBuffer->Vertices[1].Pos = core::vector3df(20, 20, 0);
	pointSpriteMeshBuffer->Vertices[2].Pos = core::vector3df(20, 12, 0);
	pointSpriteMeshBuffer->Vertices[3].Pos = core::vector3df(-20, 12, 0);

	while ( device->run() )
	{
		if ( device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			videoDriver->setMaterial(m);
			videoDriver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

			videoDriver->drawMeshBuffer(pointMeshBuffer);
			videoDriver->drawMeshBuffer(lineStripMeshBuffer);
			videoDriver->drawMeshBuffer(lineLoopMeshBuffer);
			videoDriver->drawMeshBuffer(linesMeshBuffer);
			videoDriver->drawMeshBuffer(triangleStripMeshBuffer);
			videoDriver->drawMeshBuffer(triangleFanMeshBuffer);
			videoDriver->drawMeshBuffer(triangleMeshBuffer);
			videoDriver->drawMeshBuffer(quadStripMeshBuffer);
			videoDriver->drawMeshBuffer(quadMeshBuffer);
			videoDriver->drawMeshBuffer(polygonMeshBuffer);
			videoDriver->drawMeshBuffer(pointSpriteMeshBuffer);

			smgr->drawAll();

			videoDriver->endScene();
		}

		device->sleep( 5 );
	}

	device->closeDevice();
	device->drop();
	device = NULL;

	return 0;
}
