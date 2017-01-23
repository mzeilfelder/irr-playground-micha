// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Loading tar.gz archives

#include <irrlicht.h>
#include <iostream>

using namespace irr;

int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.


	// get all files in some folder
    io::IFileSystem* fs = device->getFileSystem();
    fs->changeWorkingDirectoryTo("my_media");
    io::IFileList *fileList = fs->createFileList();
    fs->setFileListSystem(io::FILESYSTEM_NATIVE);	// not necessary here as it's default, but never hurts to ensure we got correct fs

    // open all .tar.gz archives in that folder
    for (u32 i = 0; i < fileList->getFileCount(); ++i)
    {
        io::path s = fileList->getFullFileName(i);
        std::cout << s.c_str() << "\n";;
        if (s.find(".tgz") >= 0 || s.find(".tar.gz") >= 0)
        {
        	std::cout << "loading file: " << s.c_str() << "\n";
			if ( fs->addFileArchive(s.c_str(), false, false, io::EFAT_GZIP) )
			{
				io::IFileArchive* tarArchive = fs->getFileArchive(fs->getFileArchiveCount()-1);
				const io::IFileList* flTar = tarArchive->getFileList();
				if ( flTar->getFileCount() > 0 && flTar->getFullFileName(0).find(".tar") ) // should have exactly one tar file
				{
					if ( fs->addFileArchive(flTar->getFullFileName(0).c_str(), false, false, io::EFAT_TAR) )
					{
						io::IFileArchive* gzArchive = fs->getFileArchive(fs->getFileArchiveCount()-1);
						const io::IFileList* flGz = gzArchive->getFileList();
						for ( u32 j=0; j<flGz->getFileCount(); ++j )
						{
							io::path s2 = flGz->getFullFileName(j);
							std::cout << s2.c_str() << "\n";;
						}
					}
				}
			}
        }
    }

    // Showing files in all archives
    fs->setFileListSystem(io::FILESYSTEM_VIRTUAL);
    fileList = fs->createFileList();
    for (u32 j = 0; j < fileList->getFileCount(); j++)
    {
        io::path s = fileList->getFullFileName(j);
        std::cout << s.c_str() << "\n";;
    }

    return 0;
}
