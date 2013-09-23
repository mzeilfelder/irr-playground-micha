// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Just writing out a huge xml (and then running a profiler on this).

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
	irr::io::IWriteFile* file = fs->createAndWriteFile("xmlspeedtest.xml");
	if ( !file )
		return false;
	
	irr::io::IXMLWriter* xmlWriter = fs->createXMLWriter(file);	
	if ( !xmlWriter )
	{
		file->drop();
		return false;
	}
			 
	xmlWriter->writeXMLHeader();
	xmlWriter->writeElement(L"root");

	const unsigned int MAX_ELEMENTS = 1000000;
	for (unsigned int i = 0; i<MAX_ELEMENTS; i++)
	{
		xmlWriter->writeElement(L"Element",true,
					L"Number",irr::core::stringw(i).c_str(),
					L"String",irr::core::stringw(L"just some text").c_str());
	}
		   
	xmlWriter->writeClosingTag(L"root");
	
	// close 
	xmlWriter->drop();
	file->drop();
	
	
	return 0;
}
