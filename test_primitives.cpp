// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Draw meshbuffers with different primitives

#include <irrlicht.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace irr;
using namespace core;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

struct SContext
{
	bool setup()
	{
#ifdef __EMSCRIPTEN__
		device = createDevice(video::EDT_WEBGL1, irr::core::dimension2d<irr::u32>(800,600));
//		device = createDevice(video::EDT_OGLES2, irr::core::dimension2d<irr::u32>(800,600));
#else
		device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800,600));
#endif

		if (!device)
			return false;

		smgr = device->getSceneManager();
		videoDriver = device->getVideoDriver ();

#ifdef __EMSCRIPTEN__
		irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 0.f, 0.f );
#else
		irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );
#endif
		camera->updateAbsolutePosition();
		camera->setTarget(core::vector3df(0,0,0));
		camera->updateAbsolutePosition();
		camera->setPosition(irr::core::vector3df(0, 50, -100));
		camera->updateAbsolutePosition();

		videoDriver->setMinHardwareBufferVertexCount(0); // for testing hw buffers

		setupBuffers();

		return true;
	}

	bool update()
	{
		if(!device->run())
			return false;

#ifndef __EMSCRIPTEN__
		if ( device->isWindowActive() )
#endif
		{
			irr::video::SMaterial m;
			m.Lighting = false;
			m.Thickness = 4.f;
			videoDriver->setMaterial(m);

			videoDriver->beginScene(true, true);

			videoDriver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

			if ( pointMeshBuffer )
				videoDriver->drawMeshBuffer(pointMeshBuffer);
			if ( lineStripMeshBuffer )
				videoDriver->drawMeshBuffer(lineStripMeshBuffer);
			if ( lineLoopMeshBuffer )
				videoDriver->drawMeshBuffer(lineLoopMeshBuffer);
			if ( linesMeshBuffer )
				videoDriver->drawMeshBuffer(linesMeshBuffer);
			if ( triangleStripMeshBuffer )
				videoDriver->drawMeshBuffer(triangleStripMeshBuffer);
			if ( triangleFanMeshBuffer )
				videoDriver->drawMeshBuffer(triangleFanMeshBuffer);
			if ( triangleMeshBuffer )
				videoDriver->drawMeshBuffer(triangleMeshBuffer);
			if ( quadStripMeshBuffer )
				videoDriver->drawMeshBuffer(quadStripMeshBuffer);
			if ( quadMeshBuffer )
				videoDriver->drawMeshBuffer(quadMeshBuffer);
			if ( polygonMeshBuffer )
				videoDriver->drawMeshBuffer(polygonMeshBuffer);
			if ( pointSpriteMeshBuffer )
				videoDriver->drawMeshBuffer(pointSpriteMeshBuffer);

			smgr->drawAll();

			videoDriver->endScene();
		}
#ifndef __EMSCRIPTEN__		
		device->sleep( 5 );
#endif

		return true;
	}

	void close()
	{
		device->closeDevice();
		device->drop();
		device = NULL;
	}

protected:

	void setupBuffers()
	{
		// point meshbuffer
		pointMeshBuffer = new scene::SMeshBuffer();
		pointMeshBuffer->setPrimitiveType(scene::EPT_POINTS);
		initBufferDefaults(pointMeshBuffer, 4, video::SColor(255, 255, 0, 0));
		pointMeshBuffer->Vertices[0].Pos = core::vector3df(-20, -10, 0);
		pointMeshBuffer->Vertices[1].Pos = core::vector3df(20, -10, 0);
		pointMeshBuffer->Vertices[2].Pos = core::vector3df(20, 10, 0);
		pointMeshBuffer->Vertices[3].Pos = core::vector3df(-20, 10, 0);


		// line_strip meshbuffer
		lineStripMeshBuffer = new scene::SMeshBuffer();
		lineStripMeshBuffer ->setPrimitiveType(scene::EPT_LINE_STRIP);
		initBufferDefaults(lineStripMeshBuffer, 5, video::SColor(255, 255, 127, 0));
		lineStripMeshBuffer->Vertices[0].Pos = core::vector3df(-7, -7, 0);
		lineStripMeshBuffer->Vertices[1].Pos = core::vector3df(-3, -3, 0);
		lineStripMeshBuffer->Vertices[2].Pos = core::vector3df(0, -5, 0);
		lineStripMeshBuffer->Vertices[3].Pos = core::vector3df(3, -3, 0);
		lineStripMeshBuffer->Vertices[4].Pos = core::vector3df(7, -7, 0);

		// line_loop meshbuffer
		lineLoopMeshBuffer = new scene::SMeshBuffer();
		lineLoopMeshBuffer ->setPrimitiveType(scene::EPT_LINE_LOOP);
		initBufferDefaults(lineLoopMeshBuffer, 5, video::SColor(255, 255, 255, 0));
		lineLoopMeshBuffer->Vertices[0].Pos = core::vector3df(-7, 7, 0);
		lineLoopMeshBuffer->Vertices[1].Pos = core::vector3df(-3, 3, 0);
		lineLoopMeshBuffer->Vertices[2].Pos = core::vector3df(0, 5, 0);
		lineLoopMeshBuffer->Vertices[3].Pos = core::vector3df(3, 3, 0);
		lineLoopMeshBuffer->Vertices[4].Pos = core::vector3df(7, 7, 0);

		// line meshbuffer
		linesMeshBuffer = new scene::SMeshBuffer();
		linesMeshBuffer->setPrimitiveType(scene::EPT_LINES);
		initBufferDefaults(linesMeshBuffer, 6, video::SColor(255, 255, 255, 127));
		linesMeshBuffer->Vertices[0].Pos = core::vector3df(-7, 17, 0);
		linesMeshBuffer->Vertices[1].Pos = core::vector3df(-7, 13, 0);
		linesMeshBuffer->Vertices[2].Pos = core::vector3df(-2, 17, 0);
		linesMeshBuffer->Vertices[3].Pos = core::vector3df(2, 11, 0);
		linesMeshBuffer->Vertices[4].Pos = core::vector3df(7, 15, 0);
		linesMeshBuffer->Vertices[5].Pos = core::vector3df(7, 11, 0);

		// triangle_strip meshbuffer
		triangleStripMeshBuffer = new scene::SMeshBuffer();
		triangleStripMeshBuffer->setPrimitiveType(scene::EPT_TRIANGLE_STRIP);
		initBufferDefaults(triangleStripMeshBuffer, 6, video::SColor(255, 255, 255, 255));
		triangleStripMeshBuffer->Vertices[0].Pos = core::vector3df(-7, -18, 0);
		triangleStripMeshBuffer->Vertices[1].Pos = core::vector3df(-9, -13, 0);
		triangleStripMeshBuffer->Vertices[2].Pos = core::vector3df(0, -15, 0);
		triangleStripMeshBuffer->Vertices[3].Pos = core::vector3df(0, -11, 0);
		triangleStripMeshBuffer->Vertices[4].Pos = core::vector3df(7, -18, 0);
		triangleStripMeshBuffer->Vertices[5].Pos = core::vector3df(9, -13, 0);

		// triangle_fan meshbuffer
		triangleFanMeshBuffer = new scene::SMeshBuffer();
		triangleFanMeshBuffer->setPrimitiveType(scene::EPT_TRIANGLE_FAN);
		initBufferDefaults(triangleFanMeshBuffer, 6, video::SColor(255, 127, 255, 255));
		triangleFanMeshBuffer->Vertices[0].Pos = core::vector3df(0, -25, 0);
		triangleFanMeshBuffer->Vertices[1].Pos = core::vector3df(-7, -28, 0);
		triangleFanMeshBuffer->Vertices[2].Pos = core::vector3df(-9, -23, 0);
		triangleFanMeshBuffer->Vertices[3].Pos = core::vector3df(0, -21, 0);
		triangleFanMeshBuffer->Vertices[4].Pos = core::vector3df(9, -23, 0);
		triangleFanMeshBuffer->Vertices[5].Pos = core::vector3df(7, -28, 0);

		// triangle meshbuffer
		triangleMeshBuffer = new scene::SMeshBuffer();
		triangleMeshBuffer->setPrimitiveType(scene::EPT_TRIANGLES);
		initBufferDefaults(triangleMeshBuffer, 6, video::SColor(255, 0, 255, 255));
		triangleMeshBuffer->Vertices[0].Pos = core::vector3df(0, -35, 0);
		triangleMeshBuffer->Vertices[1].Pos = core::vector3df(-7, -38, 0);
		triangleMeshBuffer->Vertices[2].Pos = core::vector3df(-9, -33, 0);
		triangleMeshBuffer->Vertices[3].Pos = core::vector3df(0, -31, 0);
		triangleMeshBuffer->Vertices[4].Pos = core::vector3df(9, -33, 0);
		triangleMeshBuffer->Vertices[5].Pos = core::vector3df(7, -38, 0);

//#if 0 // to see same results as in emscripten
#ifndef __EMSCRIPTEN__  // not supported in webgl, thought not sure about point-sprites
		// quad_strip meshbuffer
		quadStripMeshBuffer = new scene::SMeshBuffer();
		quadStripMeshBuffer->setPrimitiveType(scene::EPT_QUAD_STRIP);
		initBufferDefaults(quadStripMeshBuffer, 6, video::SColor(255, 0, 127, 255));
		quadStripMeshBuffer->Vertices[0].Pos = core::vector3df(17, -18, 0);
		quadStripMeshBuffer->Vertices[1].Pos = core::vector3df(15, -13, 0);
		quadStripMeshBuffer->Vertices[2].Pos = core::vector3df(24, -15, 0);
		quadStripMeshBuffer->Vertices[3].Pos = core::vector3df(24, -11, 0);
		quadStripMeshBuffer->Vertices[4].Pos = core::vector3df(31, -18, 0);
		quadStripMeshBuffer->Vertices[5].Pos = core::vector3df(33, -13, 0);

		// quad meshbuffer
		quadMeshBuffer = new scene::SMeshBuffer();
		quadMeshBuffer->setPrimitiveType(scene::EPT_QUADS);
		initBufferDefaults(quadMeshBuffer, 8, video::SColor(255, 0, 0, 255));
		quadMeshBuffer->Vertices[0].Pos = core::vector3df(17, -28, 0);
		quadMeshBuffer->Vertices[1].Pos = core::vector3df(15, -23, 0);
		quadMeshBuffer->Vertices[3].Pos = core::vector3df(24, -25, 0);
		quadMeshBuffer->Vertices[2].Pos = core::vector3df(24, -21, 0);
		quadMeshBuffer->Vertices[4].Pos = core::vector3df(24, -25, 0);
		quadMeshBuffer->Vertices[5].Pos = core::vector3df(24, -21, 0);
		quadMeshBuffer->Vertices[7].Pos = core::vector3df(31, -28, 0);
		quadMeshBuffer->Vertices[6].Pos = core::vector3df(33, -23, 0);

		// polygon meshbuffer (TODO: not working - likely because OpenGL has deprecated it)
		polygonMeshBuffer = new scene::SMeshBuffer();
		polygonMeshBuffer->setPrimitiveType(scene::EPT_POLYGON);
		initBufferDefaults(polygonMeshBuffer, 6, video::SColor(255, 128, 0, 128));
		polygonMeshBuffer->Vertices[0].Pos = core::vector3df(-27, -18, 0);
		polygonMeshBuffer->Vertices[1].Pos = core::vector3df(-29, -13, 0);
		polygonMeshBuffer->Vertices[2].Pos = core::vector3df(-20, -20, 0);
		polygonMeshBuffer->Vertices[3].Pos = core::vector3df(-11, -13, 0);
		polygonMeshBuffer->Vertices[4].Pos = core::vector3df(-13, -18, 0);
		polygonMeshBuffer->Vertices[5].Pos = core::vector3df(-20, -10, 0);


		// point sprite meshbuffer
		pointSpriteMeshBuffer = new scene::SMeshBuffer();
		pointSpriteMeshBuffer->setPrimitiveType(scene::EPT_POINT_SPRITES);
		initBufferDefaults(pointSpriteMeshBuffer, 4, video::SColor(255, 0, 128, 0));
		pointSpriteMeshBuffer->Vertices[0].Pos = core::vector3df(-20, 20, 0);
		pointSpriteMeshBuffer->Vertices[1].Pos = core::vector3df(20, 20, 0);
		pointSpriteMeshBuffer->Vertices[2].Pos = core::vector3df(20, 12, 0);
		pointSpriteMeshBuffer->Vertices[3].Pos = core::vector3df(-20, 12, 0);
#endif		
	}

	void initBufferDefaults(irr::scene::SMeshBuffer * mb, irr::u32 size, irr::video::SColor color)
	{
		mb->setHardwareMappingHint(scene::EHM_STATIC);
		mb->Vertices.set_used(size);
		mb->Indices.set_used(size);
		for ( u32 i=0; i<mb->Indices.size(); ++i )
			mb->Indices[i] = i;
		for ( u32 i=0; i<mb->Vertices.size(); ++i )
			mb->Vertices[i].Color = color;
	}

	private:
		IrrlichtDevice *device = 0;
		scene::ISceneManager* smgr = 0;
		video::IVideoDriver * videoDriver = 0;


		scene::SMeshBuffer * pointMeshBuffer = 0;
		scene::SMeshBuffer * lineStripMeshBuffer = 0;
		scene::SMeshBuffer * lineLoopMeshBuffer = 0;
		scene::SMeshBuffer * linesMeshBuffer = 0;
		scene::SMeshBuffer * triangleStripMeshBuffer = 0;
		scene::SMeshBuffer * triangleFanMeshBuffer = 0;
		scene::SMeshBuffer * triangleMeshBuffer = 0;
		scene::SMeshBuffer * quadStripMeshBuffer = 0;
		scene::SMeshBuffer * quadMeshBuffer = 0;
		scene::SMeshBuffer * polygonMeshBuffer = 0;
		scene::SMeshBuffer * pointSpriteMeshBuffer = 0;

} Context;

#ifdef __EMSCRIPTEN__
void one_iter()
{
	if ( !Context.update() )
	{
        emscripten_cancel_main_loop();
        return;
    }
}
#endif

int main()
{
	if ( !Context.setup() )
		return 1;

#ifdef __EMSCRIPTEN__
	int fps = 0;
	int simulate_infinite_loop = 1;
	emscripten_set_main_loop(one_iter, fps, simulate_infinite_loop);
#else

	while (Context.update())
	{
	}

	Context.close();
#endif

	return 0;
}
