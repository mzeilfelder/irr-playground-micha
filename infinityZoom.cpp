// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Experimental infinity zoom (zooming into an array of images)
#include <irrlicht.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
 
using namespace irr;
using namespace core;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class CInfinityZoom
{
public:
	CInfinityZoom(irr::IrrlichtDevice * deviceIn) 
		: device(deviceIn), frontBB(0), backBB(0)
		, camera(0), oldCamera(0)
		, camNearHorizontal(0), camNearVertical(0)
		, camDepth(0), fillDir(FILL_VERTICAL)
		, activeIdx(0), earliestFlipDist(0), flipDist(0)
	{
	}

	~CInfinityZoom()
	{
	}

	// each picture-file must have 2x the resolution of the previous one, but same size
	void addFile(const irr::io::path& filename)
	{
		zoomFiles.push_back(filename);
	}

	// Cache all textures - otherwise they are loaded before display and removed again afterward 
	// You can only cache after you have added the files.
	// Usually you cache for realtime, while it's not necessary for some special-cases like frame-based video-recording.
	void cache()
	{
		irr::video::IVideoDriver * videoDriver = device->getVideoDriver ();
		textureCache.resize(zoomFiles.size());
		for ( size_t i=0; i<zoomFiles.size(); ++i )
		{
			textureCache[i] = videoDriver->getTexture( zoomFiles[i].c_str() );
		}
	}

	// Call once to initialize and start.
	// This will set a new camera.
	bool start()
	{
		if ( zoomFiles.size() < 2)
			return false;

		irr::video::IVideoDriver * videoDriver = device->getVideoDriver ();

		irr::video::ITexture * texFront = getTexture(0);
		irr::video::ITexture * texBack = getTexture(1);
		if ( !texFront || !texBack )
			return false;

		irr::scene::ISceneManager* smgr = device->getSceneManager();

		irr::core::dimension2d<irr::u32> dimInt = texFront->getSize();
		irr::core::dimension2df dim((irr::f32)dimInt.Width, (irr::f32)dimInt.Height);

		frontBB = smgr->addBillboardSceneNode(0, dim);
		backBB = smgr->addBillboardSceneNode(0, dim);

		irr::video::SMaterial material;
		material.Lighting = false;
		material.MaterialType = irr::video::EMT_TRANSPARENT_VERTEX_ALPHA;
		material.UseMipMaps = false;

		material.setTexture(0, texFront);
		frontBB->getMaterial(0) = material;
		material.setTexture(0, texBack);
		backBB->getMaterial(0) = material;

		oldCamera = smgr->getActiveCamera();

		camera = smgr->addCameraSceneNode();
		// camera = smgr->addCameraSceneNodeFPS(0, 0.f, 0.2f );			// TEST

		updateCameraFrustum();
		fullscreenBB = dim;
		if ( fullscreenBB.Width < camNearHorizontal && fullscreenBB.Height < camNearVertical )
		{
			fullscreenBB.Width = irr::core::min_(camNearHorizontal, camNearVertical);
			fullscreenBB.Height = fullscreenBB.Width;
		}
		assert( fullscreenBB.Width != 0 && fullscreenBB.Height != 0 );
		irr::f32 ratioBB = fullscreenBB.Width/fullscreenBB.Height;
		irr::f32 ratioCamera = camera->getAspectRatio();
		fillDir = (ratioCamera > ratioBB  ) ? FILL_HORIZONTAL : FILL_VERTICAL;

		irr::f32 startDistFront = getScreenFillDistance(fullscreenBB);
		frontBB->setPosition( irr::core::vector3df(0, 0, startDistFront) );
		backBB->setPosition( irr::core::vector3df(0, 0, getBackDistance(startDistFront)) );

		// flip when we are at half-size
		irr::f32 factor = getVisibleFactorAtDistance(startDistFront);
		earliestFlipDist = getScreenFillDistance(fullscreenBB*0.5f*factor);
		flipDist = earliestFlipDist - 100.f;

		return true;
	}

	// Move the billboards by stepSize closer to the camera 
	// (A negative stepSize will move it away, but won't flip - it's not really supported so far)
	// The billboards will automatically switch when necessary.
	bool step(irr::f32 stepSize=0.f)
	{
		irr::f32 frontDist = frontBB->getPosition().Z;

		// too close to camera - flip front and back billboard
		if ( flipDist > frontDist - stepSize )
		{
			irr::core::swap(frontBB, backBB);

			irr::video::IVideoDriver * videoDriver = device->getVideoDriver();

			if ( !frontBB->getMaterial(0).getTexture(0) )
				return false;	// done

			if ( textureCache.empty() )	// no cache, remove textures directly
				videoDriver->removeTexture(backBB->getMaterial(0).getTexture(0));

			++activeIdx;
			backBB->getMaterial(0).setTexture( 0, getTexture(activeIdx+1) );

			frontDist = frontBB->getPosition().Z;
		}
		else if ( earliestFlipDist > frontDist - stepSize && earliestFlipDist > flipDist )
		{
			//irr::f32 nextDist = frontDist - stepSize;
			//irr::f32 f = (earliestFlipDist - nextDist) / (earliestFlipDist - flipDist);
			//frontBB->setColor( irr::video::SColor(irr::u32(255.f - 255.f*f), 255, 255, 255) );
		}
		else
		{
			frontBB->setColor( irr::video::SColor(255, 255, 255, 255) );
			backBB->setColor( irr::video::SColor(255, 255, 255, 255) );
		}

		frontBB->setPosition( irr::core::vector3df(0, 0, frontDist-stepSize) );
		backBB->setPosition( irr::core::vector3df(0, 0, getBackDistance(frontDist-stepSize)) );

		// TEST: just check if distances are correct - flicker the nodes
		static bool front = true;
		if ( front )
		{
			frontBB->setVisible(true);
			backBB->setVisible(false);
		}
		else
		{
			frontBB->setVisible(false);
			backBB->setVisible(true);
		}
		front = !front;
		// TEST - END

		return true;
	}

	// this will restore the old camera and clear the cache
	void stop()
	{
		device->getSceneManager()->setActiveCamera(oldCamera);
		camera = NULL;

		irr::video::IVideoDriver * videoDriver = device->getVideoDriver();
		for ( size_t i=0; i<textureCache.size(); ++i )
		{
			videoDriver->removeTexture(textureCache[i]);
		}
		textureCache.clear();
	}

protected:
	irr::video::ITexture* getTexture(irr::u32 index)
	{
		if ( index >= zoomFiles.size() )
			return NULL;
		if ( !textureCache.empty() )
			return textureCache[index];
		return device->getVideoDriver()->getTexture( zoomFiles[index] );
	}

	void updateCameraFrustum()
	{
		const irr::scene::SViewFrustum * frustum = camera->getViewFrustum();
		camNearHorizontal = (frustum->getNearRightUp() - frustum->getNearLeftUp()).getLength();
		camNearVertical =(frustum->getNearRightUp() - frustum->getNearRightDown()).getLength();
	}

	// Get the distance a billboard with given dimensions would need be away from the camera
	// to fill the screen completely (if height/width of dim and screen are different this means part of the 
	// billboard in one direction can be outside the screen, while the other direction fits exactly)
	irr::f32 getScreenFillDistance(const irr::core::dimension2df& dim)
	{
		irr::f32 nearDist = camera->getNearValue();
		if ( fillDir == FILL_HORIZONTAL )
			return (nearDist*dim.Width)/camNearHorizontal;
		else
			return (nearDist*dim.Height)/camNearVertical;
	}

	// Get the visible dimension as a certain distance (a parallel viewplane to near and far-plane)
	irr::f32 getVisibleFactorAtDistance(irr::f32 dist)
	{
		irr::f32 nearDist = camera->getNearValue();
		if ( fillDir == FILL_HORIZONTAL )
			return (dist*(camNearHorizontal/nearDist))/fullscreenBB.Width;
		else
			return (dist*(camNearVertical/nearDist))/fullscreenBB.Height;
	}

	irr::f32 getBackDistance(irr::f32 frontDist)
	{
		irr::f32 factor = getVisibleFactorAtDistance(frontDist);
		irr::core::dimension2df dim2 = fullscreenBB*2.f*factor; // next image has twice the resolution
		return getScreenFillDistance(dim2);
	}


private:

	enum FILL_DIR
	{
		FILL_VERTICAL,
		FILL_HORIZONTAL
	};

	irr::IrrlichtDevice * device;
	irr::scene::IBillboardSceneNode* frontBB;
	irr::scene::IBillboardSceneNode* backBB;
	std::vector<irr::io::path> zoomFiles;	
	std::vector<irr::video::ITexture*> textureCache;
	irr::scene::ICameraSceneNode* camera;
	irr::scene::ICameraSceneNode* oldCamera;
	f32 camNearHorizontal;
	f32 camNearVertical;
	f32 camFarVertical;
	f32 camDepth;
	irr::core::dimension2df fullscreenBB;	// the dimension that counts as "fullscreen" for billboard
	FILL_DIR fillDir;
	size_t activeIdx;	// image used for frontBB
	irr::f32 earliestFlipDist;
	irr::f32 flipDist;
};

class KeyEventReceiver : public IEventReceiver
{
public:
	KeyEventReceiver()
	{
		memset(KeysPressed, 0, sizeof KeysPressed);
	}

	bool OnEvent( const SEvent &event )
	{
		if (event.EventType == EET_KEY_INPUT_EVENT) 
		{
			KeysPressed[event.KeyInput.Key] = event.KeyInput.PressedDown;
		}
	
		return false;
	}
	bool KeysPressed[irr::KEY_KEY_CODES_COUNT];
};
 

int main()
{
	KeyEventReceiver keyEventReceiver;
	IrrlichtDevice *  Device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(400,300));
	if (!Device)
		return false;
	Device->setEventReceiver(&keyEventReceiver);

	scene::ISceneManager* smgr = Device->getSceneManager();
	video::IVideoDriver * videoDriver = Device->getVideoDriver ();

	CInfinityZoom infinityZoom(Device);
	for ( size_t i=2; i<=20; ++i )
	{
		irr::io::path filename("media_infinityzoom/");
		filename += irr::io::path(i);
		filename += ".png";
		infinityZoom.addFile(filename);
	}
	infinityZoom.cache();
	infinityZoom.start();

	while ( Device->run() )
	{
		if ( Device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			if ( keyEventReceiver.KeysPressed[KEY_PLUS] 
				|| keyEventReceiver.KeysPressed[KEY_ADD]
				|| keyEventReceiver.KeysPressed[KEY_UP]
				)
				infinityZoom.step(2.f);	// yeah, not time-independent
			else if ( keyEventReceiver.KeysPressed[KEY_MINUS] 
				|| keyEventReceiver.KeysPressed[KEY_SUBTRACT]
				|| keyEventReceiver.KeysPressed[KEY_DOWN]
				)
				infinityZoom.step(-5.f);

			smgr->drawAll();

			videoDriver->endScene();
		}

		Device->sleep( 20 );
	}
	infinityZoom.stop();

	Device->closeDevice();
	Device->drop();
	Device = NULL;

	return 0;
}
