// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Comparing matrix and quaternion rotations to have a test for the bug with right-hand rotations in quaternions:
// http://irrlicht.sourceforge.net/forum/viewtopic.php?f=7&t=50580

#include <irrlicht.h>
#include <cassert>

using namespace irr;
using namespace core;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


int main(int argc, char *argv[])
{
	vector3df p1(100,0,0);
	vector3df p2(300,300,300);
	vector3df angle(0,10,0);

	// matrix
	matrix4 m1, m2, m3;
	m1.buildRotateFromTo(p1, p2);
	vector3df angleM1 = m1.getRotationDegrees();
 	m2.setRotationDegrees(angle);
	m3 = m2;
	vector3df angleM2 = m2.getRotationDegrees();
	m2 = m1*m2;                      //Left handed multiplication
	vector3df angleMat = m2.getRotationDegrees();
	m3 *= m1;
	vector3df angleM3 = m3.getRotationDegrees();
 
	// quaternion
	quaternion q1, q2, q3;
    q1.rotationFromTo(p1, p2);
	vector3df angleQ1 = q1.getMatrix().getRotationDegrees();
    q2.set(angle*DEGTORAD);
	q3 = q2;
	vector3df angleQ2 = q2.getMatrix().getRotationDegrees();
	q2 = q1*q2;                          //Right handed multiplication
 	vector3df angleQuat = q2.getMatrix().getRotationDegrees();
	q3 *= q1;
	vector3df angleQ3 = q3.getMatrix().getRotationDegrees();

	vector3df diffAngle1 = angleM1 - angleQ1;
	vector3df diffAngle2 = angleM2 - angleQ2;
	vector3df diffAngle3 = angleM3 - angleQ3;
	vector3df diffAngleResult = angleMat - angleQuat;

	return 0;
}
