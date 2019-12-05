// Written by Michael Zeilfelder
// Some test-cases from softbike
// See http://irrlicht.sourceforge.net/forum//viewtopic.php?t=43725

#include <irrlicht.h>
#include <iostream>

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

using namespace irr;
using namespace core;
using namespace gui;

const f32 eps = 0.1;

float toRange360(f32 x)
{
	if ( x < 0 )
		return x + 360.f;
	if ( x >= 360.f)
		return x - 360.f;
	return x;
}

vector3df toRange360(const vector3df& v)
{
	return vector3df(toRange360(v.X), toRange360(v.Y), toRange360(v.Z));
}

bool cmrRot(f32 a, f32 b)
{
	return core::equals( a, b, eps)
		||	core::equals( a+360.f, b, eps)
		||	core::equals( a, b+360.f, eps);
}

int main() 
{
	core::array<vector3df> orig;

#if 1
	orig.push_back(vector3df(0.00, -0.00, 0.00));
	orig.push_back(vector3df(90.00, -0.00, 0.00));
	orig.push_back(vector3df(179.30, 359.18, 0.58));
	orig.push_back(vector3df(355.82, 345.96, 273.26));
	orig.push_back(vector3df(4.85, 359.30, 94.33));
	orig.push_back(vector3df(0.00, 90.00, 270.00));
	orig.push_back(vector3df(3.23, 359.65, 10.21));
	orig.push_back(vector3df(3.23, 359.65, 10.17));
	orig.push_back(vector3df(0.00, 0.00, 24.04));
	orig.push_back(vector3df(0.58, 4.36, 334.36));
	orig.push_back(vector3df(0.00, 0.00, 103.99));
	orig.push_back(vector3df(0.00, 90.00, 159.51));
	orig.push_back(vector3df(0.00, 90.00, 249.48));
	orig.push_back(vector3df(0.00, 75.55, 155.89));
	orig.push_back(vector3df(11.64, 311.52, 345.35));
	orig.push_back(vector3df(0.00, 0.00, 276.00));
	orig.push_back(vector3df(90.95, 360.00, 360.00));
	orig.push_back(vector3df(76.68, 1.11, 18.65));
	orig.push_back(vector3df(360.00, 0.00, 295.00));
	orig.push_back(vector3df(360.00, 0.00, 0.00));
	orig.push_back(vector3df(351.10, 353.37, 9.27));
	orig.push_back(vector3df(360.00, 360.00, 80.00));
	orig.push_back(vector3df(360.00, 0.00, 80.00));
	orig.push_back(vector3df(0.04, 359.99, 9.50));
	orig.push_back(vector3df(360.00, 0.00, 80.00));
	orig.push_back(vector3df(360.00, 360.00, 80.00));
	orig.push_back(vector3df(12.10, 4.72, 11.24));
	orig.push_back(vector3df(360.00, 360.00, 80.00));
	orig.push_back(vector3df(0.00, 360.00, 80.00));
	orig.push_back(vector3df(0.34, 89.58, 360.00));
	orig.push_back(vector3df(0.00, 284.45, 155.89));
	orig.push_back(vector3df(14.63, 48.72, 31.79));
	orig.push_back(vector3df(0.00, 360.00, 286.29));
	orig.push_back(vector3df(269.05, 0.00, 0.00));
	orig.push_back(vector3df(283.32, 358.89, 18.65));
	orig.push_back(vector3df(360.00, 360.00, 295.00));
	orig.push_back(vector3df(0.00, 360.00, 0.00));
	orig.push_back(vector3df(8.90, 6.63, 9.27));
	orig.push_back(vector3df(360.00, 0.00, 80.00));
	orig.push_back(vector3df(360.00, 360.00, 80.00));
	orig.push_back(vector3df(359.96, 0.01, 9.50));
	orig.push_back(vector3df(360.00, 0.00, 80.00));
	orig.push_back(vector3df(360.00, 0.00, 80.00));
	orig.push_back(vector3df(347.90, 355.28, 11.24));
	orig.push_back(vector3df(0.00, 360.00, 80.00));
	orig.push_back(vector3df(360.00, 0.00, 80.00));
	orig.push_back(vector3df(0.01, 0.42, 90.38));
	orig.push_back(vector3df(0.01, 0.42, 90.38));
	orig.push_back(vector3df(115.52, 89.04, 205.51));
	orig.push_back(vector3df(180.41, 359.94, 179.69));
	orig.push_back(vector3df(0.00, 90.00, 269.91));
	orig.push_back(vector3df(0.00, 90.00, 269.91));
	orig.push_back(vector3df(180.92, 10.79, 144.53));
	orig.push_back(vector3df(0.00, 0.00, 261.73));
	orig.push_back(vector3df(358.24, 358.07, 342.82));
	orig.push_back(vector3df(0.00, 0.00, 71.19));
	orig.push_back(vector3df(90.01, 270.49, 360.00));
	orig.push_back(vector3df(180.09, 270.06, 0.00));	// different angles, but same rotation
	orig.push_back(vector3df(173.58, 348.13, 132.25));
	orig.push_back(vector3df(360.00, 0.00, 318.30));
	orig.push_back(vector3df(359.78, 357.69, 7.52));
	orig.push_back(vector3df(0.00, 360.00, 71.00));
	orig.push_back(vector3df(269.99, 270.49, 360.00));
	orig.push_back(vector3df(181.95, 270.03, 0.00));	// different angles, but same rotation
	orig.push_back(vector3df(300.00, 240.00, 0.00));	// different angles, but same rotation
#else
	orig.push_back(vector3df(180.09, 270.06, 0.00));
	orig.push_back(vector3df(0.00, 90.00, 270.00));
#endif

#if 1
	const f32 step = 15.f;
	for ( f32 x = -360.f; x <= 360.f; x += step )
		for ( f32 y = -360.f; y <= 360.f; y += step )
			for ( f32 z = -360.f; y <= 360.f; y += step )
			{
				orig.push_back(vector3df(x, y, z));
				orig.push_back(vector3df(x-0.05, y-0.05f, z-0.05));
				orig.push_back(vector3df(x+0.05, y+0.05f, z+0.05));
			}
#endif

#if 0	// test single candidates
	vector3df A(180.09, 270.06, 0.00);
	//vector3df A(0, 90, 270);
	{
#else
	for ( u32 i=0; i < orig.size(); ++i )
	{
		vector3df A(orig[i]);
#endif

		quaternion cQuat(A*core::DEGTORAD);
		core::vector3df B;
		cQuat.toEuler(B);
		B *= core::RADTODEG;

		B = toRange360(B);
		vector3df Ar = toRange360(A);

		// Test if we get same angles back
		const f32 eps = 0.01;
		if (	!cmrRot(Ar.X, B.X)
			||	!cmrRot(Ar.Y, B.Y)
			||	!cmrRot(Ar.Z, B.Z) )
		{
			// still a chance it's same rotation, but with different angles
			irr::core::vector3df v1(123, 345, 678);
			irr::core::vector3df v2(v1);
			irr::core::matrix4 m1, m2;
			m1.setRotationDegrees(A);
			m2.setRotationDegrees(B);
			m1.rotateVect(v1);
			m2.rotateVect(v2);

			if (  !v1.equals(v2, 1.f) )	// yeah, there is some difference around gimbal lock, but that's as good as it gets
			{
				std::cout << "A: " << A.X << " " << A.Y << " " << A.Z << "\n";
				std::cout << "B: " << B.X << " " << B.Y << " " << B.Z << "\n";

				//core::vector3df dbg;	// for easier debugging do calculation again
				//cQuat.toEuler(dbg);
			}
		}
	}

	std::cout << "done\n";
	int ch = std::cin.get();
}
