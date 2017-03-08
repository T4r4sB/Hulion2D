#pragma once
#include "textures.h"

typedef tblib::array2d_window<uint8_t> MegaWindow;
typedef tblib::array2d<uint8_t, MegaWindow> Megamap;

tblib::carray<tbal::Color, 256> colorTable, trueTable;

const uint8_t COLOR_WALL = 0;
const uint8_t COLOR_WALL_2 = 1;
const uint8_t COLOR_STAIRS_1 = 2;
const uint8_t COLOR_STAIRS = 3;
const uint8_t COLOR_EMPTY_1 = 4;
const uint8_t COLOR_BLOOD_1 = 5;
const uint8_t COLOR_BLOOD_2 = 6;
const uint8_t COLOR_EMPTY = 8;
const uint8_t COLOR_FOREST = 9;
const uint8_t COLOR_HELL = 10;
const uint8_t COLOR_FIRST = 11;

const tbal::Color halfmask (0xfe, 0xfe, 0xfe); // чтоб в цикле не считал

void InitColorTable ()
{
	for (int i=0; i<11; ++i)
	{
		colorTable[i] = tbal::COLOR_BLACK;
		trueTable[i] = tbal::COLOR_BLACK;
	}

	colorTable[COLOR_BLOOD_1] = tbal::Color(0x70, 0x00, 0x00);
	colorTable[COLOR_BLOOD_2] = tbal::Color(0xa0, 0x00, 0x00);
	
	colorTable[COLOR_FOREST ] = tbal::Color(0x00, 0x08, 0x10);
	colorTable[COLOR_HELL   ] = tbal::Color(0x40, 0x20, 0x00);
	
	for (int i=11; i<256; ++i)
	{
		const int r = (i-11)%7;
		const int g = (i-11)/7%5;
		const int b = (i-11)/35;
		colorTable[i] = tbal::Color (r*28+g*8+b*3+37, r*14+g*18+b*13+21, r*6+g*10+b*29+5); 
		trueTable[i] = tbal::Color(r,g,b);
		//colorTable[i] = tbal::Color(r*255/6, (g*255/4+r*255/6+b*255/6)/3, b*255/6);
	}
}

uint8_t ColorToByte (tbal::Color color)
{
	return uint8_t(11 + (color.R()/37) + 7*(color.G()/52) + 35*(color.B()/37));
}

uint8_t MulLight(uint8_t color, int r, int g, int b)
{
	if (color>=COLOR_FIRST)
	{		
		tbal::Color t = trueTable[color];
		int cr = t.R();
		int cg = t.G();
		int cb = t.B();

		assert(r<256);
		assert(g<256);
		assert(b<256);
		cr = (cr*r+128)/256;
		cg = (cg*g+128)/256;
		cb = (cb*b+128)/256;
		int result = 11 + cr + 7*cg + 35*cb;
		assert(result>=COLOR_FIRST && result<256);

		return uint8_t(result);
	} else
		return color;
}

struct TableCopy
{
	void operator () (tbal::Color& dst, uint8_t src)
	{
		dst = colorTable[src];
	}
};

struct ByteFill
{
	uint8_t c;
	ByteFill(uint8_t c) : c(c) {}
	void operator () (uint8_t& dst)
	{
		dst=c;
	}
};

struct HeightInfo
{
	std::vector <int> lo,hi;
};

void CreateCurvedFloor (std::vector<int>& v, int froma, int fromb)
{
	int hg = rnd.random(fromb-froma+1)+froma;
	int fp = rnd.random(7)-3;
	for (size_t i=0; i<v.size(); ++i)
	{
		tblib::inbound(hg, 0, fromb);
		v[i] = hg;
		int delta = rnd.random(5)-2+fp;
		tblib::inbound(delta, -1, 1);
		fp = fp + rnd.random(3)-1;
		tblib::inbound(fp, -3, 3);
		hg += delta;
	}
	
	hg = rnd.random(fromb-froma)+froma;
	fp = rnd.random(7)-3;
	for (size_t i=v.size()-1; i<v.size(); --i)
	{
		tblib::inbound(hg, 0, fromb);
		// ехал int() через int(), а size_t в векторе пиздец бесит БЕСИТ
		v[i] = (v[i]* int(v.size()-1-i) +hg* int(i) )/int(v.size()-1);
		//tblib::shrink(v[i], hg);
		int delta = rnd.random(5)-2+fp;
		tblib::inbound(delta, -1, 1);
		fp = fp + rnd.random(3)-1;
		tblib::inbound(fp, -3, 3);
		hg += delta;
	}
}

void CreateLine (Setting s, int length, HeightInfo& h)
{
	assert(length>0);
	h.lo.resize(length);
	h.hi.resize(length);

	if (!IsSettingRegular(s))
	{
		CreateCurvedFloor(h.lo, CELL_HEIGHT/8, CELL_HEIGHT/4);
		
		int hg=0;
		int fp = rnd.random(7)-3;
		for (size_t i=0; i<h.hi.size(); ++i)
		{
			if (i<CELL_WIDTH/6) hg=0;
			else if (i<CELL_WIDTH/3 || s==SG_FOREST)
			{
				hg+=3;
				tblib::shrink(hg, CELL_HEIGHT*7/12);
			} else
			{				
				int delta = rnd.random(5)-2+fp;
				tblib::inbound(delta, -2, 2);
				fp = fp + rnd.random(3)-1;
				tblib::inbound(fp, -3, 3);
				hg += delta;
				tblib::inbound(hg, CELL_HEIGHT*2/12, CELL_HEIGHT*7/12);
			}
			h.hi[i] = h.lo[i]+hg;
		}		
		
		hg=0;
		fp = rnd.random(7)-3;
		for (size_t i=h.hi.size()-1; i<h.hi.size(); --i) // бля!
		{
			if (i>=h.hi.size()-CELL_WIDTH/6) hg=0;
			else if (i>=h.hi.size()-CELL_WIDTH/3 || s==SG_FOREST)
			{
				hg+=3;
				tblib::shrink(hg, CELL_HEIGHT*7/12);
			} else
			{
				int delta = rnd.random(5)-2+fp;
				tblib::inbound(delta, -2, 2);
				fp = fp + rnd.random(3)-1;
				tblib::inbound(fp, -3, 3);
				hg += delta;
				tblib::inbound(hg, CELL_HEIGHT*2/12, CELL_HEIGHT*7/12);
			}
			tblib::shrink(h.hi[i], h.lo[i]+hg);
		}
	} else
	{
		for (size_t i=0; i<h.lo.size(); ++i)
		{
			h.lo[i] = 0;
		}
		
		int bh = CELL_HEIGHT/7 + rnd.random(CELL_HEIGHT/8);

    int hg=0;
		for (size_t i=0; i<h.hi.size(); ++i)
		{
			if (i<CELL_WIDTH/6) hg=0;
			else hg=bh;
			tblib::shrink(hg, CELL_HEIGHT/2);
			h.hi[i] = h.lo[i]+hg;
		}		
    hg=0;
		for (size_t i=h.hi.size()-1; i<h.hi.size(); --i) // бля!
		{
			if (i>=h.hi.size()-CELL_WIDTH/6) hg=0;
			else hg=bh;
			tblib::shrink(hg, CELL_HEIGHT/2);
			tblib::shrink(h.hi[i], h.lo[i]+hg);
		}
	}
}

struct VertLine
{
	std::vector<int> tclo, tchi, tcmid;
};

void CreateVertLine (int length, int width, int dist, VertLine& l)
{
	l.tclo.resize(length);
	l.tchi.resize(length);
	l.tcmid.resize(length);
	CreateCurvedFloor(l.tclo , 0, width);
	CreateCurvedFloor(l.tchi , 0, width);
	CreateCurvedFloor(l.tcmid, 0, dist);
	for (size_t i=0; i<l.tcmid.size(); ++i)
	{
		l.tclo[i] -= l.tcmid[i]-dist/2;
		l.tchi[i] += l.tcmid[i]-dist/2;
	}
}


void ByteCircle(const Megamap& b, int x, int y, int r, uint8_t color)
{
	static tblib::carray<tblib::array<int, 16>, 16> dl;
	if (dl[0].size()==0)
	{
		for (int i=0; i<dl.size(); ++i)
		{
			int k=i;
			for (int j=0; j<=i; ++j)
			{
				while (k*k+j*j>i*i+i)
					--k;
				dl[i].push_back(k);
			}
		}
	}

	int y1 = std::max(0, y-r);
	int y2 = std::min(b.sizeY()-1, y+r);

	Megamap::line line = b[y1];
	for (int j=y1; j<=y2; ++j)
	{
		int xr = dl[r][abs(j-y)];
		int x1 = std::max(0, x-xr);
		int x2 = std::min(b.sizeX()-1, x+xr);
		for (int i=x1; i<=x2; ++i)
		{
			if (line[i]>=COLOR_EMPTY) line[i] = color;
		}
		++line;
	}
}

int BranchSize (int cur_size)
{
	return cur_size;
}

void DrawTreeRec(const Megamap& b, Point c, Point dc, int init_size, int iteration, int cur_size, int tonextb, uint8_t color1, uint8_t color2)
{
	if (cur_size<=0 || iteration>init_size)
	{
		int r = (rnd.random(5)/4*4+1)*rnd.random(init_size)/50;
		tblib::inbound(r, 1, 15);

		ByteCircle(b, int(c.x), int(c.y), r, color2);
	} else
	{		
		ByteCircle(b, int(c.x), int(c.y), cur_size/40+1, color1);
		if (tonextb==0)
		{
			for (int i=0; i<2; ++i)
			{			
				Fixed ds = (rnd.frandom()-fxHalf)*fxd(4)+(Fixed(i)-fxHalf)*fxd(4);
				Point a(fx1-ds*ds*fxHalf, ds);
				DrawTreeRec(b, c+dc*(rnd.random(2)*2+1), dc&a, init_size, iteration+1, BranchSize(cur_size)-1, rnd.random(cur_size/3)+2, color1, color2);	
			}
		} else
		{
			Fixed ds = (rnd.frandom()-fxHalf)*fxd(4);
			Point a(fx1-ds*ds*fxHalf, ds);
			DrawTreeRec(b, c+dc*(rnd.random(2)*2+1), dc&a, init_size, iteration+1, cur_size-1, tonextb-1, color1, color2);
		}
	}
}

void DrawTree(const Megamap& b, int x, int y, int size, uint8_t color1, uint8_t color2)
{
	DrawTreeRec(b, Point(x,y), Point(0, -1), size, 0, size, rnd.random(size/3)+2, color1, color2);
}

void DrawMechRec(const Megamap& b, Point c, Point dc, int init_size, int cur_size, int tonextb, uint8_t color1, uint8_t color2)
{
	if (cur_size==0)
	{
		ByteCircle(b, int(c.x), int(c.y), 1, color2);
	} else
	{		
		if (tonextb==0)
		{
			for (int i=0; i<2; ++i)
			{			
				Fixed ds = (rnd.frandom()-fxHalf)*fxd(8)+(Fixed(i)-fxHalf)*fxd(4);
				Point a(fx1-ds*ds*fxHalf, ds);

				Point ndc=dc*(rnd.random(2)*4+1);
				if (ndc.x>=ndc.y.Abs() || ndc.x<=-ndc.y.Abs())
					ndc.y=fx0;
				else 
					ndc.x=fx0;
				
				int l = 1+std::max(int(ndc.x.Abs()), int(ndc.y.Abs()));
				Point ddc = ndc/l;
				Point cc = c;
				for (int i=0; i<l; ++i)
				{
					ByteCircle(b, int(cc.x), int(cc.y), 0, color1);
					cc += ddc;
				}

				DrawMechRec(b, c+ndc, dc&a, init_size, cur_size-1, rnd.random(cur_size)+2, color1, color2);	
			}
		} else
		{
			Fixed ds = (rnd.frandom()-fxHalf)*fxd(8);
			Point a(fx1-ds*ds*fxHalf, ds);

			Point ndc=dc*(rnd.random(2)*4+1);
			if (ndc.x>=ndc.y.Abs() || ndc.x<=-ndc.y.Abs())
				ndc.y=fx0;
			else 
				ndc.x=fx0;
			
			int l = 1+std::max(int(ndc.x.Abs()), int(ndc.y.Abs()));
			Point ddc = ndc/l;
			Point cc = c;
			for (int i=0; i<l; ++i)
			{
				ByteCircle(b, int(cc.x), int(cc.y), 0, color1);
				cc += ddc;
			}

			DrawMechRec(b, c+ndc, dc&a, init_size, cur_size-1, tonextb-1, color1, color2);
		}
	}
}

void DrawMech(const Megamap& b, int x, int y, int size, uint8_t color1, uint8_t color2, bool up)
{	
	DrawMechRec(b, Point(x,y), Point(0, up?-1:1), size, size, rnd.random(size)+2, color1, color2);
}

void DrawBox(const Megamap& b, int x, int y, int size)
{
	int y1 = std::max(0, y-size+1);
	int y2 = std::min(b.sizeY(), y+1);
	int x1 = std::max(0, x-size/2);
	int x2 = std::min(b.sizeX(), x-size/2+size);

	uint8_t cl1 = ColorToByte(tbal::Color(64,64,0));
	uint8_t cl2 = ColorToByte(tbal::Color(128,128,0));
	uint8_t cl3 = ColorToByte(tbal::Color(255,255,0));

	for (int j=y1; j<y2; ++j)	for (int i=x1; i<x2; ++i)
	{
		if (b[j][i]>=COLOR_EMPTY)
		{
			int dx = i-(x-size/2);
			int dy = j-(y-size+1);
      if (dx==0 || dy==0 || dx==size-1 || dy==size-1)
				b[j][i] = cl1;
			else if (dx%4==0)
				b[j][i] = cl2;
			else
				b[j][i] = cl3;
		}
	}
}

void DrawScreen(const Megamap& b, int x, int y, int size)
{
	int y1 = std::max(0, y-size/2);
	int y2 = std::min(b.sizeY(), y+size/2);
	int x1 = std::max(0, x-size/2);
	int x2 = std::min(b.sizeX(), x+size/2);

	uint8_t cl1 = ColorToByte(tbal::Color(255,255,255));
	uint8_t cl2 = ColorToByte(tbal::Color(128, 128, 128));
	uint8_t cl3 = ColorToByte(tbal::Color(0,0,0));

	for (int j=y1; j<y2; ++j)	for (int i=x1; i<x2; ++i)
	{
		if (b[j][i]>=COLOR_EMPTY)
		{
			int dx = i-(x-size/2);
			int dy = j-(y-size/2);
      if (dx==0 || dy==0 || dx==size-1 || dy==size-1)
				b[j][i] = cl1;
			else if (dy%4==0)
				b[j][i] = cl2;
			else
				b[j][i] = cl3;
		}
	}
}

void DrawStreetLight(const Megamap& b, int x, int y, int r, int h)
{
	uint8_t cl1 = ColorToByte(tbal::Color(255,192,128));
	uint8_t cl2 = ColorToByte(tbal::Color(0,0,0));

	int rc = r*3/2;
	for (int i=0; i<abs(rc)+1; ++i)
	{
		Point a = SinCos(fxPi*Fixed(i,rc))*r;
		ByteCircle(b, x-r+int(a.x), y-int(a.y), 1, cl2);
	}
	for (int i=0; i<h/2+1; ++i)
	{
		ByteCircle(b, x, y+i*2, 1, cl2);
	}
	ByteCircle(b, x-r*2, y, 3, cl1);
}

void DrawGlowLamp(const Megamap& b, int x, int y, int h)
{
	uint8_t cl1 = ColorToByte(tbal::Color(255,255,0));
	uint8_t cl2 = ColorToByte(tbal::Color(0,0,0));

	for (int i=0; i<h+1; ++i)
		ByteCircle(b, x, y-i, 0, cl2);
	ByteCircle(b, x, y, 3, cl1);
}

void DrawFluor(const Megamap& b, int x, int y, int dx, int dy)
{
	uint8_t cl1 = ColorToByte(tbal::Color(255,255,255));
	uint8_t cl2 = ColorToByte(tbal::Color(0,0,0));

	ByteCircle(b, x-dx/2, y-dy/2, 1, cl2);
	ByteCircle(b, x+dx/2, y+dy/2, 1, cl2);
	for (int i=1; i<dx/2; ++i)
	{		
		ByteCircle(b, x-dx/2+i*2, y-dy/2+i*2*dy/dx, 1, cl1);
	}
}

void CreateForestBk (const Megamap& bkForest)
{
	bkForest.Fill(ByteFill(ColorToByte(tbal::Color(0,0,40))));
	for (int j=0; j<bkForest.sizeX()*bkForest.sizeY()/100; ++j)
	{
		bkForest[rnd.random(bkForest.sizeY())][rnd.random(bkForest.sizeX())] = ColorToByte(tbal::COLOR_WHITE);
	}

	for (int j=0; j<100; ++j)
	{
		int k = bkForest.sizeX()*2/3-j;
		int h = bkForest.sizeY()*3/4-j;
		std::vector<int> hl;
		hl.resize(k);
		CreateCurvedFloor(hl, h, h);

		uint8_t color = ColorToByte(tbal::Color(rnd.random(j), j, 0));
		int beg = rnd.random(bkForest.sizeX()-k+1+bkForest.sizeX()/3)-bkForest.sizeX()/6;
				
		if (j<5 || j%20==0)
		{
			int tb = rnd.random(k);
			DrawTree(bkForest, beg+tb, bkForest.sizeY()-1-(h-hl[tb]), 50+rnd.random(50), 
				ColorToByte(tbal::Color(j, rnd.random(j), 0)), ColorToByte(tbal::Color(rnd.random(j), j, 0)));
		}

		Megamap::line line = bkForest[bkForest.sizeY()-h];
		for (int l=h-1; l>=0; --l)
		{
			for (int i=0; i<k; ++i)
			{
				if (l < h-hl[i] && i+beg>=0 && i+beg<bkForest.sizeX())
					line[i+beg] = color;
			}
			++line;
		}
	}
}

tblib::modarray<tblib::modarray<Fixed, 512>, 512> lava; // иначе стек переполнитсо

void CreateHellBk (const Megamap& bkHell)
{	
	bkHell.Fill(ByteFill(ColorToByte(tbal::Color(255,0,0))));
	for (int j=0; j<bkHell.sizeX()*bkHell.sizeY()/200; ++j)
	{
		int x = rnd.random(bkHell.sizeX());
		int y = rnd.random(bkHell.sizeY()-4);
		int l = rnd.random(4)+2;
		for (int k=0; k<l; ++k)
		{
			bkHell[y+k][x]=ColorToByte(tbal::COLOR_YELLOW);
		}
	}

	FillPerlin(lava);

	int y1 = bkHell.sizeY()*2/3;
	Megamap::line line = bkHell[y1];

	for (int j=0; j<bkHell.sizeY()-y1; ++j)
	{
		for (int i=0; i<bkHell.sizeX(); ++i)
			line[i] = ColorToByte(tbal::Color(255, int(Fixed(255)*lava[j*4][i]), 0));
		++line;
	}

	for (int j=0; j<2; ++j)
	{
		int k = (bkHell.sizeX()-j)*4;
		k = (k+rnd.random(k))/2;
		int beg = -rnd.random(k/4);

		uint8_t color = ColorToByte(tbal::Color(40*(1-j), 0, 0));
		for (;;)
		{
			int h = bkHell.sizeY()/2;
			std::vector<int> hl;
			hl.resize(k);
			CreateCurvedFloor(hl, h, h);	

			Megamap::line line = bkHell[0];
			for (int l=h-1; l>=0; --l)
			{
				for (int i=0; i<k/4; ++i)
				{
					if (l>h-hl[i*4] && i+beg>=0 && i+beg<bkHell.sizeX())
						line[i+beg] = color;
				}
				++line;
			}

			beg += k/4;
			if (beg>=bkHell.sizeX())
				break;
			
			int k = (bkHell.sizeX()-j)*4;
			k = (k+rnd.random(k))/2;
		}
	}
}
