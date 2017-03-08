#pragma once
#include "megamap.h"

void ScaleBitmap (const tbal::Bitmap& src, const tbal::Bitmap& dst)
{
	if (src.sizeX() == dst.sizeX() && src.sizeY() == dst.sizeY())
	{
		dst.Copy(tbal::CopyPixel(), src, 0, 0);
	} else
	{
		tbal::Bitmap::line s=src[0], d=dst[0];
		const Fixed 
			dfx(src.sizeX(), dst.sizeX()), 
			dfy(src.sizeY(), dst.sizeY());
		Fixed fj=fx0;
		for (int j=0; j<dst.sizeY(); ++j)
		{
			Fixed fi=fx0;
			for (int i=0; i<dst.sizeX(); ++i)
			{
				d[i] = s[int(fi)];
				fi+=dfx;
			}
			++d;
			Fixed ofj=fj;
			fj+=dfy;
			s += (int(fj) - int(ofj));
		}
	}
}

void ReflectBitmap (const tbal::Bitmap& src, const tbal::Bitmap& dst)
{
	assert(src.sizeX()==dst.sizeX());
	assert(src.sizeY()==dst.sizeY());
	tbal::Bitmap::line s = src[0], d=dst[0];
	for (int j=0; j<dst.sizeY(); ++j)
	{
		for (int i=0; i<dst.sizeX(); ++i)
			d[i] = s[src.sizeX()-1-i];
		++s;
		++d;
	}
}