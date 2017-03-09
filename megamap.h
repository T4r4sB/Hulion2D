#pragma once
#include "art.h"

struct Level
{
	Level (const Level&);
	Level& operator = (const Level&);
	Level () {}

	uint32_t seed;

	Megamap map, bkForest, bkHell;

	// текстурки
	tblib::modarray<tblib::modarray<Fixed, 64>, 64> bunker1, bunker2;
	tblib::modarray<tblib::modarray<Fixed, 64>, 64> under1, under2, under3, under4;
	tblib::modarray<tblib::modarray<Fixed, 64>, 64> caves1, caves2, caves3;
	tblib::modarray<tblib::modarray<Fixed, 64>, 64> comp1;
	tblib::modarray<tblib::modarray<Fixed, 64>, 64> lair1, lair2;

	Topology t;

	int BigHoleBegin(int cx, int cy)
	{
		return CELL_WIDTH*cx + CELL_WIDTH*(7+cy%2*6)/24;
	}

	int BigHoleEnd(int cx, int cy)
	{
		return CELL_WIDTH*cx + CELL_WIDTH*(11+cy%2*6)/24;
	}

	int HoleBegin(int cx, int cy, int y, bool fakeSetting=false)
	{
		if (IsSettingRegular(t.cells[cy][cx].setting) ||
		t.cells[cy][cx].setting==SG_HELL && cy>0 && IsSettingRegular(t.cells[cy-1][cx].setting) ||
		fakeSetting)
			return BigHoleBegin(cx,cy)+CELL_WIDTH/48;
		else
			return BigHoleBegin(cx,cy)+std::max(0,CELL_WIDTH/24-t.widths[cx][y].lo);
		// 0 нужен для того, чтобы у дырки были ровные стены, чтобы гг не цеплялся ни за что
	}

	int HoleEnd(int cx, int cy, int y, bool fakeSetting=false)
	{
		if (IsSettingRegular(t.cells[cy][cx].setting) ||
		t.cells[cy][cx].setting==SG_HELL && cy>0 && IsSettingRegular(t.cells[cy-1][cx].setting) ||
		fakeSetting)
			return BigHoleEnd(cx,cy)-CELL_WIDTH/48;
		else
			return BigHoleEnd(cx,cy)-std::max(0,CELL_WIDTH/24-t.widths[cx][y].hi);
	}

	template <typename T>
	void FillCave (T t, Level& level, int x, int y)
	{
		int yhi = 0, ylo = map.sizeY()-1;
		for (int hi=x*CELL_WIDTH; hi<x*CELL_WIDTH+CELL_WIDTH; ++hi)
		{
			tblib::enlarge(yhi, level.t.heights[y][hi].hi);
			tblib::shrink (ylo, level.t.heights[y][hi].lo);
		}

		Megamap::line line = level.map[level.map.sizeY()-yhi];
		for (int l=yhi-1; l>=ylo; --l)
		{
			for (int hi=x*CELL_WIDTH; hi<x*CELL_WIDTH+CELL_WIDTH; ++hi)
				if (l>=level.t.heights[y][hi].lo && l<level.t.heights[y][hi].hi)
					t(hi,l,line[hi]);
			++line;
		}
	}

	template <typename T>
	class HasSetBounds
	{
		typedef char Yes;
		typedef Yes No[2];

		template <typename U, U> struct really_has;
		template <typename C> static Yes& Test(really_has <void (C::*)(int, int), &C::Bounds>*);
		template <typename> static No& Test(...);
	public:
		static bool const value = sizeof(Test<T>(0)) == sizeof(Yes);
	};

	template <typename T, bool B = HasSetBounds<T>::value>
	struct Check;

	template <typename T>
	struct Check <T, true>
	{
		static void SetBounds (T& t, int x1, int x2) {t.Bounds(x1,x2);}
	};

	template <typename T>
	struct Check <T, false>
	{
		static void SetBounds (T& t, int x1, int x2)
		{
			(void)t;
			(void)x1;
			(void)x2;
		}
	};

	template <typename T>
	void FillHole (T t, Level& level, int x, int y, bool bigHole=false)
	{
		int l1=MMY, l2=0;
		int
			hb=level.BigHoleBegin(x,y),
			he=level.BigHoleEnd  (x,y);

		int hi = hb;
		for (; hi<he; ++hi)
		{
			tblib::shrink  (l1,level.t.heights[y  ][hi].lo);
			tblib::enlarge (l2,level.t.heights[y-1][hi].lo);
		}

		Megamap::line ln = map[map.sizeY()-l2];
		for (int l=l2; l>l1; --l)
		{
			int bl = bigHole ? level.BigHoleBegin(x,y) : level.HoleBegin(x,y,map.sizeY()-1-l);
			int br = bigHole ? level.BigHoleEnd  (x,y) : level.HoleEnd  (x,y,map.sizeY()-1-l);

			Check<T>::SetBounds(t, bl, br);
			for (hi=bl; hi<br; ++hi)
				if (l<=level.t.heights[y-1][hi].lo && l>level.t.heights[y][hi].lo)
					t(hi, l, ln[hi]);
			++ln;
		}
	}

	void InitHoleInfo ()
	{
		std::vector<int> maxh;
		maxh.resize(SIZEX*CELL_WIDTH);
		for (size_t i=0; i<maxh.size(); ++i)
			maxh[i] = ROOF_HEIGHT;

		t.maxheight = 0;
		for (int j=t.cells.size()-1; j>=0; --j)
		{
			for (int i=0;;)
			{
				while (i<t.cells[j].size() && t.cells[j][i].was==0)
					++i;
				if (i>=t.cells[j].size())
					break;

				int ni=i;
				while (t.cells[j][ni].canPass[DIR_RIGHT] || (ni+1<t.cells[j].size() && t.cells[j][ni+1].canPass[DIR_LEFT]))
					++ni;
				HeightInfo h;
				CreateLine(t.cells[j][i].setting, (ni-i+1)*CELL_WIDTH, h);

				int add = -CELL_HEIGHT;
				for (size_t hi=0; hi<h.lo.size(); ++hi)
					tblib::enlarge(add, maxh[size_t(i)*CELL_WIDTH+hi]-h.lo[hi]);

				for (size_t hi=0; hi<h.lo.size(); ++hi)
				{
					t.heights[j][i*CELL_WIDTH+int(hi)].lo = h.lo[hi]+add;
					t.heights[j][i*CELL_WIDTH+int(hi)].hi = h.hi[hi]+add;
					tblib::enlarge(t.maxheight, h.hi[hi]+add);
				}

				for (size_t hi=0; hi<h.lo.size(); ++hi)
					tblib::enlarge(maxh[size_t(i)*CELL_WIDTH+hi], add+h.hi[hi]+ROOF_HEIGHT);

				i=ni+1;
			}
		}

		std::vector<VertLine> vl;
		vl.resize(SIZEX);
		for (size_t i=0; i<vl.size(); ++i)
		{
			CreateVertLine(MMY, CELL_WIDTH/48, CELL_WIDTH/24, vl[i]);
			for (int j=0; j<MMY; ++j)
			{
				t.widths[int(i)][j].lo = vl[i].tclo[j];
				t.widths[int(i)][j].hi = vl[i].tchi[j];
			}
		}
	}

	void CreateDoorAt(int x, int y)
	{
		int sx = x*CELL_WIDTH;
		int y1 = 0, y1lo=MMY;
		for (int hi = sx-DOOR_WIDTH/2; hi<sx+DOOR_WIDTH/2; ++hi)
		{
			tblib::enlarge(y1, t.heights[y][hi].lo);
			tblib::shrink(y1lo, t.heights[y][hi].lo);
		}
		int y2 = y1+DOOR_HEIGHT + (t.cells[y][x].setting==SG_FOREST ? UNIT_HEIGHT : CELL_HEIGHT/2);

		Megamap::line line = map[map.sizeY()-y2];
		for (int l=y2-1; l>=y1+DOOR_HEIGHT; --l)
		{
			int dl = 0;
			for (int hi=sx-DOOR_WIDTH/2-dl; hi<sx+DOOR_WIDTH/2+dl; ++hi)
				if (l<t.heights[y][hi].hi)
					line[hi] = 0;
			++line;
		}

		line += DOOR_HEIGHT;
		for (int l=y1; l>=y1lo; --l)
		{
			for (int hi=sx-DOOR_WIDTH/2; hi<sx+DOOR_WIDTH/2; ++hi)
				if (l>t.heights[y][hi].lo)
					line[hi] = 0;
			++line;
		}
	}

	void CreateDoors ()
	{
		for (int j=0; j<t.cells.size(); ++j) for (int i=0; i<t.cells[j].size(); ++i)
		{
			for (int d=0; d<4; ++d)
			{
				if ((d==DIR_LEFT || d==DIR_RIGHT) && t.cells[j][i].canPass[d] && !t.cells[j+dy[d]][i+dx[d]].canPass[d+2])
					CreateDoorAt(i+int(d==DIR_RIGHT), j);
			}
		}

		for (int j=0; j<t.cells.size(); ++j) for (int i=0; i<t.cells[j].size(); ++i)
		{
			const int d = DIR_RIGHT;
			if (t.cells[j][i].canPass[d]>CP_OPENED && t.cells[j+dy[d]][i+dx[d]].canPass[d+2]>CP_OPENED)
				CreateDoorAt(i+1, j);
		}
	}

	static bool EmptyColor (uint8_t c)
	{
		return c == COLOR_EMPTY || c == COLOR_STAIRS_1 || c == COLOR_EMPTY_1;
	}

	struct HoleByteFill
	{
		uint8_t c;
		HoleByteFill(uint8_t c) : c(c) {}
		void operator () (int x, int y, uint8_t& dst)
		{
			(void)x;
			(void)y;
			dst=c;
		}
	};

	struct UnregularHoleFill
	{
		Level& level;
		int cx, cy;
		bool stairs;
		UnregularHoleFill (Level& level, int cx, int cy, bool stairs) : level(level), cx(cx), cy(cy), stairs(stairs) {}
		UnregularHoleFill& operator=(const UnregularHoleFill&);

		void operator () (int x, int y, uint8_t& dst)
		{
			if (y>=level.t.heights[cy][x].lo-7)
				dst = stairs ? COLOR_STAIRS_1 : COLOR_WALL_2;
			else
				dst = COLOR_EMPTY_1;
		}
	};

	struct EmptyToColor
	{
		uint8_t color;
		EmptyToColor (uint8_t color) : color(color) {}

		void operator () (int sx, int sy, uint8_t& pixel) const
		{
			(void) sx;
			(void) sy;
			if (EmptyColor(pixel))
				pixel = color;
		}
	};

	struct EmptyToBunker
	{
		Level& level;
		EmptyToBunker(Level& level): level(level) {}
		EmptyToBunker& operator =(const EmptyToBunker&);

		void operator () (int sx, int sy, uint8_t& pixel) const
		{
			if (EmptyColor(pixel))
			{
				int k = int(level.bunker1[sy*17/16][sx*17/16]<<7)+int(level.bunker2[sy*7/16][sx*7/16]<<7);
				pixel = ColorToByte(tbal::Color(k,k,k));
			}
		}
	};

	struct EmptyToCompcenter
	{
		Level& level;
		EmptyToCompcenter(Level& level): level(level) {}
		EmptyToCompcenter& operator =(const EmptyToCompcenter&);

		void operator () (int sx, int sy, uint8_t& pixel) const
		{
			if (EmptyColor(pixel))
			{
				if ((sx&15)==0 || (sy&15)==0)
					pixel = ColorToByte(tbal::Color(0x66, 0x66, 0x66));
				else
				{
					int t = (level.comp1 [sy>>4] [sx>>4] << 15);

					pixel = ColorToByte(
						tbal::Color(0xa0 + (t&0x3f), 0xa0 + ((t>>3)&0x3f), 0xa0 + ((t>>6)&0x3f)));
				}
			}
		}
	};

	struct EmptyToUnderground
	{
		Level& level;
		EmptyToUnderground(Level& level): level(level) {}
		EmptyToUnderground& operator =(const EmptyToUnderground&);

		void operator () (int sx, int sy, uint8_t& pixel) const
		{
			if (EmptyColor(pixel))
			{
				int dx = int(level.under3[sy][sx]<<3);
				int dy = int(level.under2[sy*5/4][sx*5/4]<<3);
				if (level.under3[sy+dy][sx+dx] > level.under4[sy*17/16][sx*17/16]*fxd(2))
				{
					int r = int(level.under1[sy][sx]<<5) + int(level.under2[sy*7/16][sx*7/16]<<5) + 128;
					int g = int(level.under2[sy][sx]<<6) + int(level.under3[sy*7/16][sx*7/16]<<6);
					pixel = ColorToByte(tbal::Color(r,g,0));
				} else
				{
					int k = int(level.under1[sy*7/16][sx*7/16]<<5) + int(level.under4[sy*17/16][sx*17/16]<<5)+32;
					pixel = ColorToByte(tbal::Color(k,k,k));
				}
			}
		}
	};


	struct EmptyToCaves
	{
		Level& level;
		EmptyToCaves(Level& level): level(level) {}
		EmptyToCaves& operator =(const EmptyToCaves&);

		void operator () (int sx, int sy, uint8_t& pixel) const
		{
			if (EmptyColor(pixel))
			{
				int dx = int(level.caves2[sy*17/16][sx*17/16]<<5);
				int dy = int(level.caves2[sy*5/4][sx*5/4]<<5);
				int r = int(level.caves1[sy+dy][sx+dx]<<7)
					+int(level.caves2[sy*15/16][sx*15/16]<<6)
					+int(level.caves3[sy*9/8][sx*9/8]<<6);
				int g = int(level.caves3[sy*7/8+dx][sx*7/8+dy]<<7);

				pixel = ColorToByte(tbal::Color(r,g, std::min(r,g)));
			}
		}
	};

	struct EmptyToLair
	{
		Level& level;
		EmptyToLair(Level& level): level(level) {}
		EmptyToLair& operator =(const EmptyToLair&);

		void operator () (int sx, int sy, uint8_t& pixel) const
		{
			if (EmptyColor(pixel))
			{
				int dx = int(level.lair2[sy*17/16][sx*17/16]<<5);
				int dy = int(level.lair2[sy*5/4][sx*5/4]<<5);
				int c = int(level.lair1[sy+dy][sx+dx]<<7);
				pixel = ColorToByte(tbal::Color(c,c,c));
			}
		}
	};

	struct Stairs1
	{
		int l, r;

		void Bounds (int l, int r)
		{
			this->l = l;
			this->r = r;
		}

		void operator () (int sx, int sy, uint8_t& pixel)
		{
			bool ok=false;
			switch (sy%8)
			{
			case 0 : ok = (sx>=l+2 && sx<l+4 || sx>=r-4 &&  sx<r-2); break;
			case 1 : ok = (sx>=l+2 && sx<r-2); break;
			case 2 : ok = (sx>=l+4 && sx<r-4); break;
			}
			if (ok && pixel == COLOR_EMPTY)
				pixel = COLOR_STAIRS;
		}
	};

	struct Stairs2
	{
		int l, r;

		void Bounds (int l, int r)
		{
			this->l = l;
			this->r = r;
		}

		void operator () (int sx, int sy, uint8_t& pixel)
		{
			bool ok=false;
			switch (sy%8)
			{
			case 0 : ok = (sx>=l+2 && sx<r-2); break;
			case 1 : ok = (sx>=l+2 && sx<r-2); break;
			default : ok = (sx>=l+4 && sx<l+6 || sx>=r-6 && sx<r-4); break;
			}
			if (ok && pixel == COLOR_EMPTY)
				pixel = COLOR_STAIRS;
		}
	};

	void InitTextures ()
	{
		FillPerlin(bunker1);
		FillPerlin(bunker2);
		FillPerlin(under1);
		FillPerlin(under2);
		FillPerlin(under3);

		for (int j=0; j<under3.size(); ++j)	for (int i=0; i<under3[j].size(); ++i)
		{
			int dj = j%8;
			int di = (i+(j/8%2*16))%32;

			dj = std::min(dj, 8-dj);
			di = std::min(di, 32-di);


			Fixed dk = std::min(fx1, std::min(Fixed(dj,5), Fixed(di,5)));
			under3[j][i]*=dk;
		}

		FillPerlin(under4);
		FillCell<16>(caves1);
		FillPerlin(caves2);
		FillPerlin(caves3);

		FillRandom(comp1);

		FillWaves<3>(lair1);
		FillPerlin(lair2);
	}

	int RandomNoStairX (int x, int y, bool afrUp, bool afrFromUp, bool afrDown)
	{
		tblib::array<int, 4> ranges;
		if (afrUp && t.cells[y][x].canPass[DIR_UP]
		|| afrFromUp && y>0 && t.cells[y-1][x].canPass[DIR_DOWN])
		{
			ranges.push_back(BigHoleBegin(x,y));
			ranges.push_back(BigHoleEnd(x,y));
		}

		if (afrDown && t.cells[y][x].canPass[DIR_DOWN])
		{
			ranges.push_back(BigHoleBegin(x,y+1));
			ranges.push_back(BigHoleEnd(x,y+1));
		}

		if (ranges.size()>2)
		{
			if (ranges[2]<ranges[0])
			{
				std::swap(ranges[0], ranges[2]);
				std::swap(ranges[1], ranges[3]);
			}
		}

		int range=CELL_WIDTH*2/4;
		for (int i=0; i<ranges.size(); i+=2)
			range-= (ranges[i+1]-ranges[i]);

		int result = CELL_WIDTH*x + CELL_WIDTH/4 + rnd.random(range);

		for (int i=0; i<ranges.size(); i+=2)
		{
			if (result>=ranges[i])
				result += (ranges[i+1]-ranges[i]);
		}

		return result;
	}

	void MakeForest (int x, int y)
	{
		for (int tr=0; tr<30; ++tr)
		{
			int tx = x*CELL_WIDTH + rnd.random(CELL_WIDTH);
			DrawTree(map, tx, map.sizeY()-1-t.heights[y][tx].lo, tr/29*60+4+rnd.random(7)*rnd.random(7),
				ColorToByte(tbal::Color(192, rnd.random(64), 0)),
				ColorToByte(tbal::Color(rnd.random(128), 192+rnd.random(64), 0)));
		}

		FillCave(EmptyToColor(COLOR_FOREST), *this, x, y);
	}

	void MakeForest2 (int x, int y)
	{
		for (int tr=0; tr<2; ++tr)
		{
			int tx = RandomNoStairX(x,y, true, true, true);
			int r = (rnd.random(5)+5)*(rnd.random(2)*2-1);
			int h = rnd.random(30)+30;
			int ty = map.sizeY()-1-t.heights[y][tx].lo-h;
			DrawStreetLight(map, tx, ty, r, h);
			t.AddLight(tx-2*r, ty, 255, 255, 128, 100+rnd.random(100));
		}

		FillCave(EmptyToColor(COLOR_FOREST), *this, x, y);
	}

	void MakeHell (int x, int y)
	{
		for (int tr=0; tr<5; ++tr)
		{
			int tx = x*CELL_WIDTH + rnd.random(CELL_WIDTH);
			DrawTree(map, tx, map.sizeY()-1-t.heights[y][tx].lo, 6+rnd.random(6)*rnd.random(6),
				ColorToByte(tbal::Color(255, 255, 255)),
				ColorToByte(tbal::Color(255, 255, 255)));
		}
		FillCave(EmptyToColor(COLOR_HELL), *this, x, y);
	}

	void MakeBunker (int x, int y)
	{
		for (int tr=0; tr<10; ++tr)
		{
			int tx = x*CELL_WIDTH + rnd.random(CELL_WIDTH);

			int r = rnd.random(3);
			uint8_t endColor =
				r==0 ? ColorToByte(tbal::Color(255, 0, 0)) :
				r==1 ? ColorToByte(tbal::Color(0, 255, 0)) :
							 ColorToByte(tbal::Color(0, 0, 255));

			bool up = rnd.random(2)==0;

			DrawMech(map, tx, map.sizeY()-1-(up?t.heights[y][tx].lo:t.heights[y][tx].hi), 10+rnd.random(20)+rnd.random(5)/4*20,
				ColorToByte(tbal::Color(64, 64, 64)),
				endColor, up);
		}

		FillCave(EmptyToBunker(*this), *this, x, y);
		if (y>0 && t.cells[y-1][x].canPass[DIR_DOWN])
			FillHole(EmptyToBunker(*this), *this, x, y);
		if (t.cells[y][x].canPass[DIR_DOWN] && t.cells[y+1][x].setting==SG_HELL)
			FillHole(EmptyToBunker(*this), *this, x, y+1);
	}

	void MakeBunker2 (int x, int y)
	{
		for (int tr=0; tr<1; ++tr)
		{
			int tx = RandomNoStairX(x,y, true, true, false);
			int ty = map.sizeY()-1-t.heights[y][tx].hi+10;
			DrawFluor(map, tx, ty, 35+rnd.random(5), rnd.random(11)-5);
			t.AddLight(tx, ty, 128, 255, 255, 300+rnd.random(100));
		}
	}

	void MakeUnderground (int x, int y)
	{
		for (int tr=0; tr<3; ++tr)
		{
			int tx = RandomNoStairX(x,y, true, false, true);
			DrawBox(map, tx, map.sizeY()-1-t.heights[y][tx].lo, 8+rnd.random(16));
		}

		for (int tr=0; tr<3; ++tr)
		{
			int tx = RandomNoStairX(x,y, true, false, true);
			DrawTree(map, tx, map.sizeY()-1-t.heights[y][tx].lo, 7+rnd.random(20),
				ColorToByte(tbal::Color(255, 192+rnd.random(64), 64)),
				ColorToByte(tbal::Color(192+rnd.random(64), 192+rnd.random(64), 0)));
			DrawBox(map, tx, map.sizeY()-1-t.heights[y][tx].lo, 8+rnd.random(8));
		}

		FillCave(EmptyToUnderground(*this), *this, x, y);
		if (y>0 && t.cells[y-1][x].canPass[DIR_DOWN])
			FillHole(EmptyToUnderground(*this), *this, x, y);
		if (t.cells[y][x].canPass[DIR_DOWN] && t.cells[y+1][x].setting==SG_HELL)
			FillHole(EmptyToUnderground(*this), *this, x, y+1);
	}

	void MakeUnderground2 (int x, int y)
	{
		for (int tr=0; tr<4; ++tr)
		{
			int tx = RandomNoStairX(x,y, true, true, false);
			int h = 5+rnd.random(10);
			int ty = map.sizeY()-1-t.heights[y][tx].hi+h;
			DrawGlowLamp(map, tx, ty, h);
			t.AddLight(tx, ty, 255, 255, 128, 50+rnd.random(200));
		}
	}

	void MakeCaves (int x, int y)
	{
		bool blue = rnd.random(3)==0;
		for (int tr=0; tr<5; ++tr)
		{
			int tx = RandomNoStairX(x,y, true, false, true);
			uint8_t color = blue ?
				ColorToByte(tbal::Color(0, 255, 255)) :
				ColorToByte(tbal::Color(255, 255, 0));
			int y1 = map.sizeY()-1-t.heights[y][tx].lo;
			int diff =t.heights[y][tx].hi-t.heights[y][tx].lo;
			DrawTree(map, tx, y1, 3+rnd.random(4)*rnd.random(4)*(1+diff/90), color, color);
			t.AddLight(tx, y1, blue?0:255, blue?0:128, blue?255:0,	diff+rnd.random(100));
		}
		FillCave(EmptyToCaves(*this), *this, x, y);
		if (y>0 && t.cells[y-1][x].canPass[DIR_DOWN])
			FillHole(EmptyToCaves(*this), *this, x, y);
		if (t.cells[y][x].canPass[DIR_DOWN] && t.cells[y+1][x].setting==SG_HELL)
			FillHole(EmptyToCaves(*this), *this, x, y+1);
	}


	void MakeLair (int x, int y)
	{

		for (int tr=0; tr<5; ++tr)
		{
			int tx = RandomNoStairX(x,y, true, false, true);

			tbal::Color color;
			switch (rnd.random(6))
			{
			case 0 : color = tbal::COLOR_RED; break;
			case 1 : color = tbal::COLOR_GREEN; break;
			case 2 : color = tbal::COLOR_YELLOW; break;
			case 3 : color = tbal::COLOR_TEAL; break;
			case 4 : color = tbal::COLOR_BLUE; break;
			case 5 : color = tbal::COLOR_PURPLE; break;
			}

			int y1 = map.sizeY()-1-t.heights[y][tx].lo;
			int diff = t.heights[y][tx].hi-t.heights[y][tx].lo;

			int ty = y1 - rnd.random(diff);

			ByteCircle(map, tx, ty, 3, ColorToByte(color));
			t.AddLight(tx, ty, color.R(), color.G(), color.B(),	100+rnd.random(100));
		}
		FillCave(EmptyToLair(*this), *this, x, y);
		if (y>0 && t.cells[y-1][x].canPass[DIR_DOWN])
			FillHole(EmptyToLair(*this), *this, x, y);
		if (t.cells[y][x].canPass[DIR_DOWN] && t.cells[y+1][x].setting==SG_HELL)
			FillHole(EmptyToLair(*this), *this, x, y+1);
	}

	void MakeCompcenter (int x, int y)
	{
		for (int tr=0; tr<10; ++tr)
		{
			int tx = x*CELL_WIDTH + rnd.random(CELL_WIDTH);

			int r = rnd.random(3);
			uint8_t endColor =
				r==0 ? ColorToByte(tbal::Color(255, 0, 0)) :
				r==1 ? ColorToByte(tbal::Color(0, 255, 0)) :
							 ColorToByte(tbal::Color(0, 0, 255));

			bool up = false;

			DrawMech(map, tx, map.sizeY()-1-(up?t.heights[y][tx].lo:t.heights[y][tx].hi), 10+rnd.random(20)+rnd.random(5)/4*20,
				ColorToByte(tbal::Color(64, 64, 64)),
				endColor, up);
		}

		int cnt = 1;
		for (int tr=0; tr<cnt; ++tr)
		{
			int tx = RandomNoStairX(x,y, true, false, true);
			int ty = map.sizeY()-1-t.heights[y][tx].lo-24-rnd.random(16);
			DrawScreen(map, tx, ty, 24+rnd.random(16));
			//t.AddLight(tx, ty, 0, 255, 0, 100);
		}

		FillCave(EmptyToCompcenter(*this), *this, x, y);
		if (y>0 && t.cells[y-1][x].canPass[DIR_DOWN])
			FillHole(EmptyToCompcenter(*this), *this, x, y);
		if (t.cells[y][x].canPass[DIR_DOWN] && t.cells[y+1][x].setting==SG_HELL)
			FillHole(EmptyToCompcenter(*this), *this, x, y+1);
	}

	void MakeCompcenter2 (int x, int y)
	{

		for (int tr=0; tr<3; ++tr)
		{
			int tx = RandomNoStairX(x,y, true, false, true);
			DrawBox(map, tx, map.sizeY()-1-t.heights[y][tx].lo, 8+rnd.random(16));
		}

		for (int tr=0; tr<1; ++tr)
		{
			int tx = RandomNoStairX(x,y, true, true, false);
			int ty = map.sizeY()-1-t.heights[y][tx].hi+10;
			DrawFluor(map, tx, ty, 35+rnd.random(5), rnd.random(11)-5);
			t.AddLight(tx, ty, 128, 255, 255, 300+rnd.random(100));
		}
	}

	void FillHoles ()
	{
		tblib::recreate(map, MMX, MMY);
		map.Fill(ByteFill(COLOR_WALL));

		// блядское хитрожопое заполнение дыр, чтобы гг не проваливался
		for (int j=t.cells.size()-1; j>=1; --j) for (int i=0; i<t.cells[j].size(); ++i) if (t.cells[j][i].was)
			if (t.cells[j-1][i].canPass[DIR_DOWN])
				FillHole(UnregularHoleFill(*this, i, j-1, t.cells[j][i].canPass[DIR_UP]>0), *this, i, j, true);

		for (int j=t.cells.size()-1; j>=1; --j) for (int i=0; i<t.cells[j].size(); ++i) if (t.cells[j][i].was)
			if (t.cells[j-1][i].canPass[DIR_DOWN])
				FillHole(HoleByteFill(COLOR_EMPTY), *this, i, j);

		for (int j=t.cells.size()-1; j>=0; --j) for (int i=0; i<t.cells[j].size(); ++i) if (t.cells[j][i].was)
			FillCave(HoleByteFill(COLOR_EMPTY), *this, i, j);

		for (int j=t.cells.size()-1; j>=0; --j) for (int i=0; i<t.cells[j].size(); ++i) if (t.cells[j][i].was)
			if (t.cells[j][i].canPass[DIR_UP])
			{
				bool su;
				switch (t.cells[j][i].setting)
				{
					case SG_UNDERGROUND : su=true; break;
					case SG_HELL : su=t.cells[j-1][i].setting==SG_UNDERGROUND; break;
					default : su=false;
				}

				if (su)
					FillHole(Stairs2(), *this, i, j);
				else
					FillHole(Stairs1(), *this, i, j);
			}



		CreateDoors ();

		InitTextures ();

		// два прохода
		for (int j=t.cells.size()-1; j>=0; --j) for (int i=0; i<t.cells[j].size(); ++i) if (t.cells[j][i].was)
		{
			switch (t.cells[j][i].setting)
			{
			case SG_FOREST : MakeForest(i,j); break;
			case SG_HELL : MakeHell(i,j); break;
			case SG_BUNKER : MakeBunker(i,j); break;
			case SG_UNDERGROUND : MakeUnderground(i,j); break;
			case SG_COMPCENTER : MakeCompcenter(i,j); break;
			case SG_CAVES : MakeCaves(i,j); break;
			case SG_LAIR : MakeLair(i,j); break;
			}
		}

		for (int j=t.cells.size()-1; j>=0; --j) for (int i=0; i<t.cells[j].size(); ++i) if (t.cells[j][i].was)
		{
			switch (t.cells[j][i].setting)
			{
			case SG_FOREST : MakeForest2(i,j); break;
			case SG_UNDERGROUND : MakeUnderground2(i,j); break;
			case SG_BUNKER : MakeBunker2(i,j); break;
			case SG_COMPCENTER : MakeCompcenter2(i,j); break;
			}
		}
	}

	typedef tbal::Color IntRGB;

	IntRGB GetLightInPoint(int x, int y)
	{
		int hx = x/HCSIZE;
		int hy = y/HCSIZE;

		int r,g,b;

		if (x<MMX && MMY-1-y>t.heights[0][x].lo && t.cells[0][x/CELL_WIDTH].was)
		{
			// лес
			r = 128;
			g = 128;
			b = 128;
		} else if (x<MMX && MMY-1-y<t.heights[SIZEY-1][x].hi)
		{
			// эт ад, там ярко, цвет портить не надо
			r=230;
			g=230;
			b=230;
			return tbal::Color(r,g,b);
		} else if (x<MMX && MMY-1-y<t.heights[SIZEY-1][x].hi+100 && t.cells[SIZEY-1][x/CELL_WIDTH].was)
		{
			int d = (100 - ((MMY-1-y) - (t.heights[SIZEY-1][x].hi)))*256/100;
			r = d;
			g = d;
			b = d;
		} else
		{
			r = 10;
			g = 10;
			b = 10;
		}


		if (hx<HMMX && hy<HMMY)
		{
			LL::yclass& cell = t.lightcells[hy][hx];

			for (LL::xynode *node = cell.firstx; node; node=node->nextx)
			{
				LightInfo& l = static_cast <LightInfo&> (*node->x);
				const int d = (l.x-x)*(l.x-x) + (l.y-y)*(l.y-y);
				const int md=l.dst*l.dst;
				if (d<md)
				{
					r += l.r*md/(d*64+md);
					g += l.g*md/(d*64+md);
					b += l.b*md/(d*64+md);
				}
			}
		}

		return tbal::Color (std::min(r,230), std::min(g,230), std::min(b,230));
	}

	static const int LG=8;
	typedef tblib::carray<bool, (MMX+LG)/LG> HRLine;
	tblib::carray<HRLine, (MMY+LG)/LG> hasRoom;

	//typedef tblib::carray<IntRGB, LG+1> KL;
	//typedef tblib::carray<KL, LG+1> KLCell;

	typedef tblib::carray<IntRGB, MMX+LG> KL;

	static void Mid (const IntRGB& l, const IntRGB& r, IntRGB& res)
	{
		res = (l&r) + (((l^r) & halfmask)>>1);
		assert(res.R()<=230 && res.G()<=230 && res.B()<=230);
	}

	struct MidColor
	{
		void operator () (const IntRGB& l, const IntRGB& r, IntRGB& res)
		{
			Mid (l,r,res);
		}
	};

	class MidLine
	{
		// понос чтоб компилятор мозг не ебал
		MidLine& operator = (const MidLine&);
	public :
		HRLine& line;
		MidLine(HRLine& line) : line(line) {}

		void operator () (const KL& l, const KL& r, KL& res)
		{
			for (int i=0; i<res.size()/LG; ++i)
				if (line[i])
					for (int j=i*LG; j<i*LG+LG; ++j)
						Mid(l[j], r[j], res[j]);
		}
	};

	template <typename T, typename Mid, int N>
	struct UselessWrapperFillMid
	{
		inline static void FillMid (T* t, Mid mid)
		{
			mid(t[0], t[N], t[N/2]);
			UselessWrapperFillMid<T,Mid,N/2>::FillMid (&t[0]  , mid);
			UselessWrapperFillMid<T,Mid,N/2>::FillMid (&t[N/2], mid);
		}
	};

	template <typename T, typename Mid>
	struct UselessWrapperFillMid <T, Mid, 1>
	{
		inline static void FillMid (T* t, Mid mid)
		{
			(void)t;
			(void)mid;
		}
	};

	void FillLightLine (int j, KL& line)
	{
		for (int k=0; k<line.size()/LG; ++k)
		{
			if (hasRoom[j/LG][k] || (k>0 && hasRoom[j/LG][k-1])
				|| (j>=LG && (hasRoom[j/LG-1][k]) || (k>0 && hasRoom[j/LG-1][k-1])))
				line[k*LG] = GetLightInPoint(k*LG, j);
		}

		for (int k=0; k<line.size()/LG-1; ++k) if (hasRoom[j/LG][k] || (j>=LG && hasRoom[j/LG-1][k]))
			UselessWrapperFillMid<IntRGB, MidColor, LG>::FillMid(&line[k*LG], MidColor());
	}

	/*
	void FillLightLine (int j, tblib::carray<KLCell, MMX/LG+1> &cells)
	{
		for (int k=0; k<cells.size(); ++k)
		{
			cells[k][0] = cells[k][LG];
			GetLightInPoint(k*LG, j, cells[k][LG][0]);
		}

		for (int k=0; k<cells.size()-1; ++k)
		{
			cells[k][LG][LG] = cells[k+1][LG][0];
			MidStep<IntRGB, LG>::Fill (&cells[k][LG][0]);
		}
	}*/

	// два разных способа размещения ячеек в памяти
	tblib::carray<KL, LG+1> kl;
	//tblib::carray<KLCell, MMX/LG+1> cells;

	void MulLights ()
	{
		for (int j=0; j<hasRoom.size(); ++j) for (int i=0; i<hasRoom.size(); ++i)
			hasRoom[j][i]=false;
		for (int j=MMY-t.maxheight; j<MMY; ++j) for (int i=0; i<MMX; ++i)
			if (map[j][i]>=COLOR_FIRST)
				hasRoom[j/LG][i/LG]=true;

		FillLightLine ((MMY-t.maxheight)&(~LG), kl[LG]);

		//FillLightLine ((MMY-t.maxheight)&(~LG), cells);

		for (int j=(MMY-t.maxheight)&(~LG); j<MMY; ++j)
		{

			if (j%LG==0)
			{
				kl[0]=kl[LG];
				FillLightLine(j+LG, kl[LG]);
				UselessWrapperFillMid<KL, MidLine, LG>::FillMid(&kl[0], MidLine(hasRoom[j/LG]));
				//MidStep<KL, LG>::Fill(&kl[0]);
/*
				FillLightLine(j, cells);
				for (int k=0; k<cells.size(); ++k)
					MidStep<KL, LG>::Fill(&cells[k][0]);*/
			}

			for (int i=0; i<MMX; ++i) if (hasRoom[j/LG][i/LG]) // иначе все эти усреднения не пригодятся
			{

				if (map[j][i]>=COLOR_FIRST)
				{
					IntRGB& cur = kl[j%LG][i];

					//IntRGB& cur = cells[i/LG][j%LG][i%LG];
					int r = cur.R(), g=cur.G(), b=cur.B();

					r += rnd.random((r-20)/8);
					g += rnd.random((g-20)/8);
					b += rnd.random((b-20)/8);

					if (r>0 && g>0 && b>0)
						map[j][i] = MulLight(map[j][i], r,g,b);
					else
						map[j][i] = COLOR_FIRST;
				}
			}

		}
	}

	void Init(int seed)
	{
		this->seed = seed;
		rnd.randseed = seed;
		t.Generate();
		InitHoleInfo();

		InitColorTable ();
		tblib::recreate(bkForest,320,200);
		CreateForestBk (bkForest);
		tblib::recreate(bkHell,320,200);
		CreateHellBk (bkHell);

		FillHoles();
		MulLights();
	}

	void DrawToBuf (const tbal::Bitmap& buf, int x, int y)
	{
		assert(buf.sizeX()==bkForest.sizeX());
		assert(buf.sizeY()==bkForest.sizeY());

		const int y1 = std::max(-y, 0);
		const int y2 = std::min(map.sizeY()-y, buf.sizeY());
		const int x1 = std::max(-x, 0);
		const int x2 = std::min(map.sizeX()-x, buf.sizeX());

		tbal::Bitmap::line bufl = buf[y1];
		Megamap::line l = map[y1+y];
		Megamap::line f = bkForest[y1];
		Megamap::line h = bkHell[y1];

		for (int j=y1; j<y2; ++j)
		{
			for (int i=x1; i<x2; ++i)
			{
				if (l[i+x]==COLOR_FOREST)
					bufl[i]=colorTable[f[i]];
				else if (l[i+x]==COLOR_HELL)
					bufl[i]=colorTable[h[i]];
				else
					bufl[i] = colorTable[l[i+x]];
			}
			++bufl;
			++l;
			++f;
			++h;
		}
	}
};
