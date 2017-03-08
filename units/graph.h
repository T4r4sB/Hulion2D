#pragma once 
#include "tbal.h"
#include "fixed.h"
#include "point.h"

namespace tbgraph 
{
	void Poly   (const tbal::Bitmap &b, size_t count, const Point pt[], tbal::Color color);
	void Pie    (const tbal::Bitmap &b, const Point &center, Fixed r, const Point &a1, const Point &a2, tbal::Color color);
	void Line   (const tbal::Bitmap &b, const Point &p1, const Point &p2, Fixed width, tbal::Color color, bool sh1=true, bool sh2=true);
	void Arc    (const tbal::Bitmap &b, const Point &center, Fixed r, Fixed width, const Point &a1, const Point &a2, tbal::Color color, bool sh1=true, bool sh2=true);
	void Circle (const tbal::Bitmap &b, const Point &center, Fixed ri, Fixed ro, tbal::Color color);
	bool CircleInBuffer (const tbal::Bitmap &b, const Point &center, Fixed r);
};
