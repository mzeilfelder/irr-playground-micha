// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Some crazy test-case for triangle-line intersections. I think it's fixed now in Irrlicht and probably part of the official tests.

#include <irrlicht.h>
#include <iostream>
#include <cassert>
 
using namespace irr;
using namespace core;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
 
// helper function to draw a box around center
void dummyDrawBox(irr::core::vector3df center, irr::video::SColor col, irr::video::IVideoDriver * videoDriver )
{
	aabbox3df box(center);
	irr::f32 r = 0.5f;
	box.addInternalPoint( center + vector3df(r, 0, 0) );
	box.addInternalPoint( center + vector3df(0, r, 0) );
	box.addInternalPoint( center + vector3df(0, 0, r) );
	box.addInternalPoint( center + vector3df(-r, 0, 0) );
	box.addInternalPoint( center + vector3df(0, -r, 0) );
	box.addInternalPoint( center + vector3df(0, 0, -r) );
	videoDriver->draw3DBox( box, col);
}
 
// fix 1: work with doubles and catch res near 0 due to floating point troubles
template<class T>
static bool isOnSameSide(const vector3d<T>& p1, const vector3d<T>& p2,
        const vector3d<T>& a, const vector3d<T>& b)
{
	// Using f64 to avoid integer overflows when T=int (maybe also less floating point troubles).
	vector3d<f64> af64((f64)a.X, (f64)a.Y, (f64)a.Z);
	vector3d<f64> bf64((f64)b.X, (f64)b.Y, (f64)b.Z);
	vector3d<f64> p1f64((f64)p1.X, (f64)p1.Y, (f64)p1.Z);
	vector3d<f64> bminusa = bf64 - af64;
	vector3d<f64> cp1 = bminusa.crossProduct(p1f64 - af64);
	vector3d<f64> cp2 = bminusa.crossProduct(vector3d<f64>((f64)p2.X, (f64)p2.Y, (f64)p2.Z) - af64);
	f64 res = cp1.dotProduct(cp2);	
	if ( res < 0 )
	{
		// This catches some floating point troubles. Unfortunately slightly expensive and we don't really know the best epsilon for iszero.
		vector3d<f64> cp1 = bminusa.normalize().crossProduct((p1f64 - af64).normalize());
		if ( 	core::iszero(cp1.X, ROUNDING_ERROR_f32)
			&& 	core::iszero(cp1.Y, ROUNDING_ERROR_f32)
			&& 	core::iszero(cp1.Z, ROUNDING_ERROR_f32) )
		{
			res = 0.f;
		}
	}
	return (res >= 0.0f);
}
 
template<class T>
static bool isOnSameSideUsingPlanes(const vector3d<T>& p1, const vector3d<T>& p2, const vector3d<T>& a, const vector3d<T>& b)
{
	// Bad, bad idea - just here so the next person doesn't try it again
	irr::core::line3d<T> ab(a, b);
	vector3d<T> orig = ab.getClosestPoint(p2);
	plane3d<T> plane(orig, (p2-orig).normalize());
	EIntersectionRelation3D rel = plane.classifyPointRelation(p1);
	return rel == ISREL3D_FRONT || rel == ISREL3D_PLANAR;
}
 
// Fix 2 - work internally with doubles to get a more precise result
template<class T>
bool getIntersectionOfPlaneWithLinePrecise(const core::triangle3d<T>& triangleIn, const vector3d<T>& linePointIn,
        const vector3d<T>& lineVectIn, vector3d<T>& outIntersectionIn)
{
	const vector3d<f64> linePoint(linePointIn.X, linePointIn.Y, linePointIn.Z);
	const vector3d<f64> lineVect(lineVectIn.X, lineVectIn.Y, lineVectIn.Z);
	vector3d<f64> outIntersection;

	core::triangle3d<irr::f64> triangle(vector3d<f64>(triangleIn.pointA.X, triangleIn.pointA.Y, triangleIn.pointA.Z)
										,vector3d<f64>(triangleIn.pointB.X, triangleIn.pointB.Y, triangleIn.pointB.Z)
										, vector3d<f64>(triangleIn.pointC.X, triangleIn.pointC.Y, triangleIn.pointC.Z));
	const vector3d<irr::f64> normal = triangle.getNormal().normalize();
	irr::f64 t2;

	if ( core::iszero ( t2 = normal.dotProduct(lineVect) ) )
		return false;

	irr::f64 d = triangle.pointA.dotProduct(normal);
	irr::f64 t = -(normal.dotProduct(linePoint) - d) / t2;
	outIntersection = linePoint + (lineVect * t);
	outIntersectionIn.X = (f32)outIntersection.X;
	outIntersectionIn.Y = (f32)outIntersection.Y;
	outIntersectionIn.Z = (f32)outIntersection.Z;
	return true;
}

template<class T>
static void testGetIntersectionWithLine(const core::triangle3d<T>& triangleIn, const core::line3d<T>& ray, irr::video::IVideoDriver * videoDriver)
{
	core::triangle3d<T> triangle(triangleIn);
	bool allExpected=true;
	const vector3d<T> linevect = ray.getVector().normalize();
	vector3d<T> intersection;
	for (u32 i=0; i<50; i+=1)
	{
		// fix 2: use a more precice intersection
		if (!getIntersectionOfPlaneWithLinePrecise(triangle, ray.start, linevect, intersection))
		//if (!triangle.getIntersectionOfPlaneWithLine(ray.start, linevect, intersection))
		{
			assert(!"should no happen");
		}
		bool inside = triangle.isPointInside(intersection);
		bool newInside = true;
		if (!isOnSameSide(intersection, triangle.pointA, triangle.pointB, triangle.pointC))
			newInside = false;
		if (!isOnSameSide(intersection, triangle.pointB, triangle.pointA, triangle.pointC))
			newInside = false;
		if (!isOnSameSide(intersection, triangle.pointC, triangle.pointA, triangle.pointB))
			newInside = false;

		if ( !newInside )       // fix 1: use another isOnSameSide function
//		if (!inside)
		{
			dummyDrawBox(intersection, irr::video::SColor(255, 255, 0, 127), videoDriver );
		}
		else
		{
			dummyDrawBox(intersection, irr::video::SColor(255, 0, 255, 127), videoDriver );
		}

		// Enable if you want to see all the triangles (doesn't help much for debuggin, but visualize it once maybe to get the problem)
		//if (!triangle.getIntersectionWithLine(ray.start, linevect, intersection))
		//{
		//	videoDriver->draw3DTriangle(triangle, irr::video::SColor(255, 255,0,0) );
		//}
		//else
		//{
		// 	videoDriver->draw3DTriangle(triangle, irr::video::SColor(255, 0,255,0) );
		//}

		triangle.pointB.Y += i;
	}
}
 
int main()
{
	IrrlichtDevice *  Device = createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800,600));
	if (!Device)
		return false;
   
	scene::ISceneManager* smgr = Device->getSceneManager();
	video::IVideoDriver * videoDriver = Device->getVideoDriver ();
   
	video::SMaterial material;
	material.NormalizeNormals = true;
	material.setTexture(0, 0);
	material.Lighting = false;
	material.BackfaceCulling = false;

	triangle3df triangle(vector3df(11300.000000f, 129.411758f, 200.000000f),
						vector3df(11200.000000f, 94.117645f, 300.000000f),
						vector3df(11300.000000f, 129.411758f, 300.000000f));
	core::line3df ray(vector3df(11250.000000f, 329.000000f, 250.000000f),
					 vector3df(11250.000000, -1000.000000, 250.000000));
	irr::core::vector3df center( (triangle.pointA+triangle.pointB+triangle.pointC)*(1.f/3.f) );

	irr::scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 20.f, 0.1f );
	camera->updateAbsolutePosition();
	camera->setTarget(center);
	camera->updateAbsolutePosition();
	camera->setPosition(center+irr::core::vector3df(0, 50, -100));
	camera->updateAbsolutePosition();

	while ( Device->run() )
	{
		if ( Device->isWindowActive() )
		{
			videoDriver->beginScene(true, true);

			smgr->drawAll();

			videoDriver->setMaterial(material);            
			videoDriver->setTransform(video::ETS_WORLD, core::matrix4());

			videoDriver->draw3DTriangle(triangle, video::SColor(255,0,0,255));
			videoDriver->draw3DLine(ray.start, ray.end, video::SColor(255, 0, 0, 255));

			testGetIntersectionWithLine(triangle, ray, videoDriver);

			videoDriver->endScene();
		}

		Device->sleep( 5 );
	}

	Device->closeDevice();
	Device->drop();
	Device = NULL;

	return 0;
}
