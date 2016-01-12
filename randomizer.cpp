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

	for ( int s = -3; s <= 3; ++ s )
	{
		randomizer->reset(s);
		std::cout << s << "." << std::endl;
		for ( int i=0; i<10; ++i)
			std::cout << randomizer->frand() << std::endl; // zeroes all the time

	}
	
	randomizer->reset(2147483647);
	std::cout << "with 2147483647" << std::endl;
	for ( int i=0; i<10; ++i)
		std::cout << randomizer->frand() << std::endl; // zeroes all the time
	
	randomizer->reset(2147483648);
	std::cout << "with 2147483648" << std::endl;
	for ( int i=0; i<10; ++i)
		std::cout << randomizer->frand() << std::endl; // zeroes all the time

	// correctness test:
	randomizer->reset(1);
	for ( int n=1; n<10000; ++n)
	{
		randomizer->rand();
	}
	std::cout << "should be: 1043618064  " << randomizer->rand() << std::endl; // zeroes all the time
	
	
	return 0;
}
