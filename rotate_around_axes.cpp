// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Example to rotate a node with pitch, yaw, roll (aka local axes)

#include <irrlicht.h>
#include <iostream>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

// rotation with quaternions
irr::core::vector3df applyRelativeRotation(const irr::core::vector3df& oldRotationEulerDeg, const irr::core::quaternion& relativeRotation)
{
	// Add relative rotation
	irr::core::quaternion qt(oldRotationEulerDeg*irr::core::DEGTORAD);
	irr::core::quaternion qt2(relativeRotation*qt);

	irr::core::vector3df rotateTarget;
	qt2.toEuler(rotateTarget);
	rotateTarget *= irr::core::RADTODEG;
	return rotateTarget;
}

// rotation with euler and matrices
irr::core::vector3df rotateAxesXYZToEuler(const irr::core::vector3df& oldRotation, const irr::core::vector3df& rotationAngles, bool useLocalAxes)
{
    irr::core::matrix4 transformation;
    transformation.setRotationDegrees(oldRotation);
    irr::core::vector3df axisX(1,0,0), axisY(0,1,0), axisZ(0,0,1);
    irr::core::matrix4 matRotX, matRotY, matRotZ;

    if ( useLocalAxes )
    {
        transformation.rotateVect(axisX);
        transformation.rotateVect(axisY);
        transformation.rotateVect(axisZ);
    }

    matRotX.setRotationAxisRadians(rotationAngles.X*irr::core::DEGTORAD, axisX);
    matRotY.setRotationAxisRadians(rotationAngles.Y*irr::core::DEGTORAD, axisY);
    matRotZ.setRotationAxisRadians(rotationAngles.Z*irr::core::DEGTORAD, axisZ);

    irr::core::matrix4 newTransform = matRotX * matRotY * matRotZ * transformation;
    return newTransform.getRotationDegrees();
}

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver() : Node(0) {}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_KEY_INPUT_EVENT && Node)
		{
			if ( event.KeyInput.PressedDown )
			{
				const f32 step_size = 5.f;
				core::vector3df oldRotation = Node->getRotation();
				irr::core::vector3df rotationAngles(0,0,0);
				switch ( event.KeyInput.Key )
				{
					case KEY_KEY_W: rotationAngles.X += step_size; break;
					case KEY_KEY_S:	rotationAngles.X -= step_size; break;
					case KEY_KEY_A:	rotationAngles.Y += step_size; break;
					case KEY_KEY_D:	rotationAngles.Y -= step_size; break;
					case KEY_KEY_Q:	rotationAngles.Z += step_size; break;
					case KEY_KEY_E:	rotationAngles.Z -= step_size; break;
					default:
						break;
				}
				if ( !rotationAngles.equals(irr::core::vector3df(0,0,0)) )
				{
#if 1	// without quaternions
					core::vector3df newRot = rotateAxesXYZToEuler(oldRotation, rotationAngles, true);
#else	// with quaternions
					irr::core::quaternion qt(rotationAngles*irr::core::DEGTORAD);
					core::vector3df newRot = applyRelativeRotation(oldRotation, qt);
#endif

					Node->setRotation(newRot);
				}
			}
		}
		return false;
	}


	irr::scene::ISceneNode* Node;
};


int main(int argc, char *argv[])
{
	IrrlichtDevice * device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(640,480) );
	if (!device)
		return false;

	MyEventReceiver receiver;
	device->setEventReceiver(&receiver);

	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guiEnv = device->getGUIEnvironment();
	video::IVideoDriver* videoDriver =  device->getVideoDriver();

	smgr->addCameraSceneNode (0, core::vector3df(0, 0, 0) );

	scene::IMesh * mesh = smgr->getGeometryCreator()->createCubeMesh(core::vector3df(5.f, 7.f, 10.f));
//	scene::IMesh * mesh = smgr->getGeometryCreator()->createArrowMesh( 4, 8, 10.f, 6.f, 1.f, 3.f, video::SColor(255, 255, 127, 127), video::SColor(255, 127, 255, 127));
	mesh->getMeshBuffer(0)->getMaterial().Lighting = false;
//	mesh->getMeshBuffer(1)->getMaterial().Lighting = false;
	receiver.Node = smgr->addMeshSceneNode( mesh, 0, -1, core::vector3df(0, 0, 20));
	mesh->drop();

	while ( device->run() )
	{
		if ( device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			smgr->drawAll();
			guiEnv->drawAll();

			videoDriver->endScene();
		}
		device->sleep( 1 );
	}

	device->closeDevice();
	device->drop();

	return 0;
}
