#pragma once
#include "time.h"

struct Random {
	unsigned int randseed;

	void NextSeed () 
	{
		randseed = randseed * 0x08088405 + 1;
	}

	Random () : randseed ((unsigned int)(time(NULL))) {}

	Random (int seed) : randseed(seed) {}

	inline int random (int range)
	{
		NextSeed ();
		return int(((long long)(randseed)*(long long)(range)) >> 32);
	}

	inline double frandom ()
	{
		const double rev_2e32 = 1.0/(65536.0*65536.0);
		NextSeed ();
		return randseed * rev_2e32;
	}
};
