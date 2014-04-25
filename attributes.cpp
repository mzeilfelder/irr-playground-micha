// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Playing around with attributes

#include "irrlicht.h"

using namespace irr;
using namespace core;
using namespace io;

int main()
{
	IrrlichtDevice * device = irr::createDevice(video::EDT_NULL, dimension2d<u32>(1, 1));
	if ( !device )
		return 1;
	io::IFileSystem * fs = device->getFileSystem ();
	if ( !fs )
		return 1;

	void* p = (void*)0xabcdabcd;

	io::IAttributes* attr = fs->createEmptyAttributes();
	attr->addUserPointer("ValPointer", p);

	attr->setAttribute("ValPointer", "0xabbacadabba");

	void* p2 = attr->getAttributeAsUserPointer("ValPointer");

	attr->drop();
	device->closeDevice();
	device->drop();

	return 0;
}
