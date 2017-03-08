#pragma once

tbal::Color textColor1 (0xaa, 0x66, 0x22), textColor2 (0xff, 0xbb, 0x77);

void Bevel (const tbal::Bitmap &b, bool hl)
{	
	tbal::Color cd = textColor1, cl = textColor2;

	if (!hl)
	{
		cd = cd.Half();
		cl = cl.Half();
	}

	tbal::Bitmap::line l = b[0];	

	for (int i=0; i<b.sizeX(); ++i) l[i] = cd;
	++l;

	for (int j=1; j<b.sizeY()-1; ++j)
	{
		l[0] = cd;
		for (int i=1; i<b.sizeX()-1; ++i)
			l[i] = tbal::COLOR_BLACK;
		l[b.sizeX()-1] = cl;
		++l;
	}
	
	for (int i=0; i<b.sizeX(); ++i) l[i] = cl;

}