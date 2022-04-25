// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Template for minimal Irrlicht gui-code.

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

#if IRRLICHT_VERSION_MAJOR > 1 || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR > 8 )
void print(EMESH_BUFFER_TYPE mbType)
{
	switch ( mbType )
	{
		case EMBT_STANDARD:	std::cout << "SMeshBuffer"; break;
		case EMBT_LIGHTMAP:	std::cout << "SMeshBufferLightMap"; break;
		case EMBT_TANGENTS:	std::cout << "SMeshBufferTangents"; break;
		case EMBT_DYNAMIC:	std::cout << "CDynamicMeshBuffer"; break;
		case EMBT_SHARED:	std::cout << "SSharedMeshBuffer"; break;
		case EMBT_SKIN:	std::cout << "SSkinMeshBuffer"; break;
		case EMBT_UNKNOWN:	std::cout << "Unknown"; break;
	}
	std::cout << "\n";
}
#endif

void dyn_meshbufferstests()
{
	video::S3DVertex vnorm(1.f, 2.f, 3.f, 0.f, 1.f, 0.f, video::SColor(0xffaaaaaa), 0.2f, 0.8f);
	video::S3DVertex2TCoords v2t(1.f, 2.f, 3.f, 0.f, 1.f, 0.f, video::SColor(0xffaaaaaa), 0.2f, 0.8f, 0.1f, 0.9f);
	video::S3DVertexTangents vtang(1.f, 2.f, 3.f, 0.f, 1.f, 0.f, video::SColor(0xffaaaaaa), 0.2f, 0.8f, 0.01f, 0.02f, 0.03f,0.04f, 0.05f, 0.06f);

	CDynamicMeshBuffer * mb = new CDynamicMeshBuffer(video::EVT_STANDARD, video::EIT_16BIT);
	scene::IVertexBuffer& vb = mb->getVertexBuffer();
	vb.push_back(vnorm);
	vb.push_back(v2t);
	vb.push_back(vtang);
	for ( u32 i=0; i < vb.size(); ++i )
	{
		video::S3DVertex& v = vb[i];
		std::cout << "X" << v.Pos.X << "Y" << v.Pos.Y << "Z" << v.Pos.Z << " nX" << v.Normal.X << "nY" << v.Normal.Y << "nZ" << v.Normal.Z << " tX" << v.TCoords.X<< "tY" << v.TCoords.Y << " c" << v.Color.color;
		std::cout << "\n";
	}
	std::cout << "\n";

	vb.setType(video::EVT_2TCOORDS);
	vb.push_back(vnorm);
	vb.push_back(v2t);
	vb.push_back(vtang);
	for ( u32 i=0; i < vb.size(); ++i )
	{
		video::S3DVertex2TCoords& v = (video::S3DVertex2TCoords&)vb[i];
		std::cout << "X" << v.Pos.X << "Y" << v.Pos.Y << "Z" << v.Pos.Z << " nX" << v.Normal.X << "nY" << v.Normal.Y << "nZ" << v.Normal.Z << " tX" << v.TCoords.X<< "tY" << v.TCoords.Y << " c" << v.Color.color;
		std::cout << " t2X" << v.TCoords2.X<< "t2Y" << v.TCoords2.Y;
		std::cout << "\n";
	}
	std::cout << "\n";

	vb.setType(video::EVT_TANGENTS);
	vb.push_back(vnorm);
	vb.push_back(v2t);
	vb.push_back(vtang);
	for ( u32 i=0; i < vb.size(); ++i )
	{
		video::S3DVertexTangents& v = (video::S3DVertexTangents&)vb[i];
		std::cout << "X" << v.Pos.X << "Y" << v.Pos.Y << "Z" << v.Pos.Z << " nX" << v.Normal.X << "nY" << v.Normal.Y << "nZ" << v.Normal.Z << " tX" << v.TCoords.X<< "tY" << v.TCoords.Y << " c" << v.Color.color;
		std::cout << " bX" << v.Binormal.X << "bY" << v.Binormal.Y << "bZ" << v.Binormal.Z << " tX" << v.Tangent.X << "tY" << v.Tangent.Y << "tZ" << v.Tangent.Z;
		std::cout << "\n";
	}
	std::cout << "\n";

	vb.setType(video::EVT_2TCOORDS);
	vb.setType(video::EVT_STANDARD);
	for ( u32 i=0; i < vb.size(); ++i )
	{
		video::S3DVertex& v = vb[i];
		std::cout << "X" << v.Pos.X << "Y" << v.Pos.Y << "Z" << v.Pos.Z << " nX" << v.Normal.X << "nY" << v.Normal.Y << "nZ" << v.Normal.Z << " tX" << v.TCoords.X<< "tY" << v.TCoords.Y << " c" << v.Color.color;
		std::cout << "\n";
	}
	std::cout << "\n";
}


int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	dyn_meshbufferstests();

	video::IVideoDriver* driver = device->getVideoDriver();

	while(device->run() && driver)
	{}

	device->drop();

	return 0;
}

