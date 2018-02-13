// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Example for rotating an object toward a direction
// or for a simple 3d->2d mapping of vertices.

#include <irrlicht.h>
#include <iostream>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver() : node(0), buffer(0), activePolygon(0)
	{
	}

	virtual bool OnEvent(const SEvent& event)
	{
		if (buffer && event.EventType == EET_MOUSE_INPUT_EVENT)
		{
			switch ( event.MouseInput.Event )
			{
				case EMIE_LMOUSE_LEFT_UP:
					setPolygonColor(activePolygon, video::SColor(255,255,255,255));
					++activePolygon;
					if ( activePolygon >= buffer->getPrimitiveCount() )
						activePolygon = 0;
					setPolygonColor(activePolygon, video::SColor(255,255,0,0));
						
					break;
				case EMIE_RMOUSE_LEFT_UP:
					setPolygonColor(activePolygon, video::SColor(255,255,255,255));
					if ( activePolygon == 0 )
						activePolygon = buffer->getPrimitiveCount()-1;
					else
						--activePolygon;
					setPolygonColor(activePolygon, video::SColor(255,255,0,0));
					break;
				case EMIE_MMOUSE_LEFT_UP:
					rotateToPolygonUp(activePolygon);
					break;
			}
		}

		return false;
	}

	void setPolygonColor(irr::u32 idx, irr::video::SColor color)
	{
		irr::u16* indices = buffer->getIndices();
		video::S3DVertex* vertices = static_cast<video::S3DVertex*>(buffer->getVertices());
		
		vertices[ indices[idx*3] ].Color = color;
		vertices[ indices[idx*3+1] ].Color = color;
		vertices[ indices[idx*3+2] ].Color = color;
	}

	void rotateToPolygonUp(irr::u32 idx)
	{
		irr::u16* indices = buffer->getIndices();
		video::S3DVertex* vertices = static_cast<video::S3DVertex*>(buffer->getVertices());

		const irr::video::S3DVertex& v1 = vertices[ indices[idx*3] ];
		const irr::video::S3DVertex& v2 = vertices[ indices[idx*3+1] ];
		const irr::video::S3DVertex& v3 = vertices[ indices[idx*3+2] ];

		core::vector3df triangleNormal = (v2.Pos - v1.Pos).crossProduct(v3.Pos - v1.Pos);
		triangleNormal.normalize();

		core::matrix4 mat;
		core::vector3df up(0,0,1);
		irr::f32 dot = triangleNormal.dotProduct(up);
		if ( fabs(dot) > 0.99 )
			up = core::vector3df(0,1,0);
		mat.buildCameraLookAtMatrixLH(core::vector3df(0,0,0), triangleNormal, up);

		mat.transformVect(transformedTriangle.pointA, v1.Pos);
		mat.transformVect(transformedTriangle.pointB, v2.Pos);
		mat.transformVect(transformedTriangle.pointC, v3.Pos);

//		node->setRotation( mat.getRotationDegrees() );
	}

	scene::IMeshSceneNode * node;
	scene::IMeshBuffer* buffer;
	irr::u32 activePolygon;
	irr::core::triangle3df transformedTriangle;
};



int main(int argc, char *argv[])
{
	IrrlichtDevice * Device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(640,480) );
	if (!Device)
		return false;

	MyEventReceiver eventReceiver;
	Device->setEventReceiver(&eventReceiver);
	
	scene::ISceneManager* smgr = Device->getSceneManager();
	gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();
    video::IVideoDriver* videoDriver =  Device->getVideoDriver();	

	smgr->addCameraSceneNode (0, core::vector3df(0, 0, 0),
								core::vector3df(0, 0, 100),
								-1);

#if 0
	scene::IMeshSceneNode * node = smgr->addSphereSceneNode(25.0f, 4, 0, -1, core::vector3df(0, 0, 100));
#else
	scene::IMeshSceneNode * node = smgr->addCubeSceneNode (30.0f, 0, -1, 
	                        core::vector3df(0, 20, 100),
							core::vector3df(45, 45, 45),
							core::vector3df(1.0f, 1.0f, 1.0f));
#endif
	node->getMaterial(0).Lighting = false;
	node->getMaterial(0).Wireframe = true;
	node->getMaterial(0).BackfaceCulling = false;

	eventReceiver.node = node;
	eventReceiver.buffer = node->getMesh()->getMeshBuffer(0);
	eventReceiver.setPolygonColor(eventReceiver.activePolygon, irr::video::SColor(255,255,0,0));

	while ( Device->run() )
	{
		if ( Device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			smgr->drawAll();
			guiEnv->drawAll();

			core::matrix4 mp;
			mp.setTranslation( node->getPosition() );
			videoDriver->setTransform(video::ETS_WORLD, mp);
			videoDriver->draw3DTriangle( eventReceiver.transformedTriangle, video::SColor(255, 0, 0, 255));

			videoDriver->endScene();
		}
		Device->sleep( 1 );
	}
	
	Device->closeDevice();
	Device->drop();
	
	return 0;
}
