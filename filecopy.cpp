// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// filecopy implemented with Irrlicht file functions.

#include <irrlicht.h>
#include <iostream>

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


int main(int argc, char **argv)
{
	using namespace irr;	
	
	if ( argc != 3 )
	{
		std::cout << "syntax: copy sourcefile targetfile" << std::endl;
		return 1;
	}
	
	video::E_DRIVER_TYPE driverType = video::EDT_NULL ;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	io::IFileSystem * fs =	device->getFileSystem ();
		
	
	io::IReadFile * file = fs->createAndOpenFile (argv[1]);
	if ( !file )
	{
		std::cout << "file not found" << std::endl;
		return 1;
	}
	char *buff = new char[file->getSize()];
	file->read(buff,file->getSize());
	io::IWriteFile *save = fs->createAndWriteFile(argv[2],false);
	save->write(buff,file->getSize()); 
	file->drop();
	save->drop();

	device->drop();

	return 0;
}

