// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing results for IFileSystem::getRelativeFilename

#include <irrlicht.h>
#pragma comment (lib, "../../lib/Win32-visualstudio/Irrlicht.lib")

using namespace irr;
using namespace std;

void main(int argc, char** argv)
{
	IrrlichtDevice* device = createDevice(video::EDT_OPENGL, core::dimension2du(720, 480));

	irr::io::path dir = device->getFileSystem()->getFileDir(device->getFileSystem()->getAbsolutePath( io::path("main.cpp") ));
	irr::io::path rel = device->getFileSystem()->getRelativeFilename( io::path("D:/Dokumente und Einstellungen/Micha/Lokale Einstellungen/Anwendungsdaten/i3/3.0/textures/099_Wit_White.tga"), dir);
	irr::io::path rel2 = device->getFileSystem()->getRelativeFilename( io::path("E:/blabla.txt"), dir);
	irr::io::path rel3 = device->getFileSystem()->getRelativeFilename( io::path("e:/blabla.txt"), dir);
	irr::io::path rel4 = device->getFileSystem()->getRelativeFilename( io::path("E:\\official_irrlicht_windows\\irrlicht\\blabla.txt"), dir);
	irr::io::path rel5 = device->getFileSystem()->getRelativeFilename( io::path("e:\\official_IRRlicht_windows\\irrlicht\\blabla.txt"), dir);
	irr::io::path rel6 = device->getFileSystem()->getRelativeFilename( io::path("e:/official_irrlicht_windows/irrlicht/blabla.txt"), dir);

	device->drop();
}