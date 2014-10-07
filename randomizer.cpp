// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Test for a bug in the random generator reported by AndreeeCZ in #irrlicht IRC channel.

#include <irrlicht.h>
#include <iostream>

using namespace irr;

int main(int argc, char *argv[])
{
	IrrlichtDevice * device = createDevice(video::EDT_NULL);
	IRandomizer* randomizer = device->getRandomizer ();
	
	for ( int i=0; i<10; ++i)
		std::cout << randomizer->frand() << std::endl; // zeroes all the time
	
	// After resetting we only get 0's below - before it works.
	randomizer->reset((s32) device->getTimer()->getTime());
	
	for ( int i=0; i<10; ++i)
		std::cout << randomizer->frand() << std::endl; // zeroes all the time
	
	return 0;
}
