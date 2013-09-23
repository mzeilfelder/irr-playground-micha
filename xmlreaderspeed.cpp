// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Just reading a huge xml which has to be created by the xmlwriterspeed test first (and then running a profiler on this).

#include <irrlicht.h>
 
using namespace irr;
using namespace core;
using namespace scene;
using namespace io;

int main()
{
	IrrlichtDevice *device = createDevice(video::EDT_NULL, core::dimension2du(160,120), 32);
	if (!device)
		return false;

	irr::io::IFileSystem * fs = device->getFileSystem();
	io::IXMLReaderUTF8* reader1 = fs->createXMLReaderUTF8("xmlspeedtest.xml");
	if ( !reader1 )
	{
		return false;
	}
	
	while ( reader1->read() )
	{
		;
	}
			 
	// close 
	reader1->drop();
	
	return 0;
}
