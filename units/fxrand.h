#pragma once
#include "pasrand.h"
#include "fixed.h"

struct FxRandom : public Random
{
	FxRandom(int seed): Random(seed) {}
	FxRandom() : Random() {}

	Fixed frandom() 
	{
		return Fixed(Fixed::RAW, random(65536));
	}
};