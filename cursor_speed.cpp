// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing a workaround for slow X11 mouse queries: http://irrlicht.sourceforge.net/forum/viewtopic.php?f=7&t=45525&start=15

#include <irrlicht.h>
#include <cstdio>
#include <sys/time.h>

using namespace irr;

int main()
{
	IrrlichtDevice * device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(640, 480));
	if ( !device )
		return 0;
	gui::ICursorControl * cursor = device->getCursorControl ();
	cursor->setPlatformBehavior(gui::ECPB_X11_CACHE_UPDATES);

	unsigned int utmp, min = 100000, max = 0, avg = 0;
	struct timeval one, two;

	int count = 10000;
	for (int i = 0; i < count; i++)
	{
		gettimeofday(&one, NULL);
		cursor->getPosition();
		gettimeofday(&two, NULL);

		utmp = (two.tv_sec - one.tv_sec) * 1000000;
		utmp += (two.tv_usec - one.tv_usec);

		avg += utmp;
		if (min > utmp) min = utmp;
		if (max < utmp) max = utmp;

		printf("%u  ", utmp);
		device->run();
	}

	avg /= count;

	printf("\nThe call took (%u, %u, %u) (min, avg, max) usec\n",
			min, avg, max);

	return 0;
}
