// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// For those tired nights when you can no longer can remember the the value ranges.

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


int main()
{	
	vector2df a(0.f, 1.f);
	
	array<vector2df> t;
	t.push_back(vector2df(0.f, 1.f));
	t.push_back(vector2df(0.f, -1.f));
	t.push_back(vector2df(1.f, 0.f));
	t.push_back(vector2df(-1.f, 0.f));
	t.push_back(vector2df(0.707106781187f, 0.707106781187f));
	t.push_back(vector2df(0.707106781187f, -0.707106781187f));
	t.push_back(vector2df(-0.707106781187f, 0.707106781187f));
	t.push_back(vector2df(-0.707106781187f, -0.707106781187f));
	
	for (u32 i=0; i< t.size(); ++i )
	{
		float angle = atan2(a.Y, a.X) - atan2(t[i].Y, t[i].X);
		std::cout << "(" << t[i].X << "," << t[i].Y << "): " << a.dotProduct(t[i]) << " a: " << angle << "\n";
	}
	return 0;
}
