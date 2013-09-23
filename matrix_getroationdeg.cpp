// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Just one of those crazy floating-point troubles (probably only with certain floating point processor settings I guess, don't really remember).

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

int main()
{

	matrix4 mtrouble;
	mtrouble[0] = -1.1920929e-007;
	mtrouble[1]	= 1.1920929e-007;
	mtrouble[2]	= -1.0000001;
	mtrouble[3]	= 0.00000000;
	mtrouble[4]	= 1.0000001;
	mtrouble[5]	= -1.1920929e-007;
	mtrouble[6]	= 0.00000000;
	mtrouble[7]	= 0.00000000;
	mtrouble[8]	= 0.00000000;
	mtrouble[9]	= -1.0000001;
	mtrouble[10] = -2.3841858e-007;
	mtrouble[11] = 0.00000000;
	mtrouble[12] = 4.0880629e-008;
	mtrouble[13] = 19.588606;
	mtrouble[14] = 4.1348953;
	mtrouble[15] = 1.0000000;

	// will have wrong result in VS 
	vector3df rotDeg( mtrouble.getRotationDegrees() );
	
	std::cout <<  "X: " << rotDeg.X <<  " Y: " << rotDeg.Y <<  " Z: " << rotDeg.Z << std::endl;

	return 0;
}
