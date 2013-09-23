// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Tests for integer triangles with large numbers (had failed in the past)
// Fixed and the tests are part of the official irrlicht tests by now.

#include "irrlicht.h"
#include <iostream>

using namespace irr;
using namespace core;

int main()
{
	if ( triangle3di(vector3di(25, 0, 0), vector3di(0, 0, 50), vector3di(50, 0, 50)).isPointInside(vector3di(30,0,30)) ) 
		std::cout << "true\n";
	else
		std::cout << "false\n"; 

	if ( triangle3di(vector3di(250, 0, 0), vector3di(0, 0, 500), vector3di(500, 0, 500)).isPointInside(vector3di(300,0,300)) )
		std::cout << "true\n";
	else
		std::cout << "false\n"; 

	if ( triangle3df(vector3df(25, 0, 0), vector3df(0, 0, 50), vector3df(50, 0, 50)).isPointInside(vector3df(30,0,30)) )
		std::cout << "true\n";
	else
		std::cout << "false\n"; 

	if ( triangle3df(vector3df(250, 0, 0), vector3df(0, 0, 500), vector3df(500, 0, 500)).isPointInside(vector3df(300,0,300)) )
		std::cout << "true\n";
	else
		std::cout << "false\n"; 

}
