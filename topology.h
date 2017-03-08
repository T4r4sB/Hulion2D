#pragma once
#include "tbarr.h"
#include "fxrand.h"
#include "ring.h"
#include "list2d.h"
#include "font.h"
#include "graph.h"

const int DIR_UP    = 2;
const int DIR_LEFT = 3;
const int DIR_DOWN  = 0;
const int DIR_RIGHT  = 1;

const int CP_CLOSED = 0;
const int CP_OPENED = 1;
const int CP_KEY = 2;

const int MAX_DOORS = 5;

const int SIZEX = 24, SIZEY = 6;
const int CELL_WIDTH = 200, CELL_HEIGHT = 480;

const int UNIT_HEIGHT = 32;
const int DOOR_HEIGHT = 50;
const int DOOR_WIDTH = 8;
const int ROOF_HEIGHT = CELL_HEIGHT/6;
const int MMX = SIZEX*CELL_WIDTH, MMY = SIZEY*CELL_HEIGHT + ROOF_HEIGHT;
const int HOLE_SHIFT = CELL_WIDTH/6;

const int HCSIZE=64;
const int HMMX = (MMX+HCSIZE-1)/HCSIZE, HMMY = (MMY+HCSIZE-1)/HCSIZE;

int __dx[4]={0,1,0,-1}, __dy[4]={1,0,-1,0};
const tblib::modarray<int,4> dx(__dx), dy(__dy);

enum Setting
{
	SG_FOREST,
	SG_HELL,
	SG_UNDERGROUND,
	SG_CAVES, 
	SG_BUNKER,
	SG_LAIR, 
	SG_COMPCENTER
};

bool IsSettingRegular(Setting s)
{
	return (s==SG_BUNKER || s==SG_UNDERGROUND || s==SG_COMPCENTER);
}

struct PassInfo
{
	int was;
	int key;
	int dist;
	int firstx, lastx;
	bool inOrder, inBestWay;
	Setting setting;
	tblib::modarray<int, 4> canPass;
};

const int MHDIST = 7;
typedef tblib::list2d<SIZEX*SIZEY*5*MHDIST*MHDIST> LL;
// SIZEX*SIZEY*5 - число источников
// MHDIST*MHDIST - число нод на источник

struct LightInfo : LL::xclass
{
	int x;
	int y;
	int r,g,b;
	int dst;
};

FxRandom rnd;

struct Pair
{
	int lo, hi;
};

tbal::Color __keyColors[MAX_DOORS+2] = {
	tbal::Color(0x00, 0x00, 0x00),
	tbal::Color(0xff, 0xff, 0x00),
	tbal::Color(0xff, 0x00, 0x00),
	tbal::Color(0x00, 0xff, 0x00),
	tbal::Color(0xff, 0xff, 0xff),
	tbal::Color(0x55, 0xbb, 0xff),
	tbal::Color(0xff, 0x00, 0xff)
};
tblib::carray<tbal::Color, MAX_DOORS+2> keyColors(__keyColors);

struct IPoint
{
	int x,y,dst;
	IPoint () {}
	IPoint (int x, int y, int dst) : x(x), y(y), dst(dst) {}
};

struct Topology
{
	Topology (const Topology&);
	Topology& operator = (const Topology&);
	Topology () {}

	tblib::carray<tblib::carray<Pair, MMX>, SIZEY> heights;
	tblib::carray<tblib::carray<Pair, MMY>, SIZEX> widths;
	tblib::carray<tblib::carray<PassInfo, SIZEX>, SIZEY> cells;

	int maxheight;

	LL lightslist;
	tblib::array<LightInfo, SIZEX*SIZEY*5> lights;
	tblib::carray<tblib::carray<LL::yclass, HMMX>, HMMY> lightcells;

	tblib::array<IPoint, SIZEX*SIZEY> orderedCells;

	void AddLight (int x, int y, int r, int g, int b, int dst)
	{
		lights.resize(lights.size()+1);
		LightInfo &l = lights.back();
		l.x = x;
		l.y = y;
		l.r = r;
		l.g = g;
		l.b = b;
		l.dst = dst;
		int x1 = std::max(0, (x-dst)/HCSIZE);
		int y1 = std::max(0, (y-dst)/HCSIZE);
		int x2 = std::min(HMMX-1, (x+dst)/HCSIZE);
		int y2 = std::min(HMMY-1, (y+dst)/HCSIZE);

		while (x2-x1+1>MHDIST) { if (abs(x-(x1*HCSIZE+HCSIZE/2))<abs(x-(x2*HCSIZE+HCSIZE/2))) --x2; else ++x1; }
		while (y2-y1+1>MHDIST) { if (abs(y-(y1*HCSIZE+HCSIZE/2))<abs(y-(y2*HCSIZE+HCSIZE/2))) --y2; else ++y1; }
		
		for (int y=y1; y<=y2; ++y)
			for (int x=x1; x<=x2; ++x)
				lightslist.put_x_to_y(&l, &lightcells[y][x]);				

	}

	int lastLabel;

	bool InField (int x, int y) const
	{
		return (y>=0 && y<cells.size() && x>=0 && x<cells[y].size());
	}
			
	void Draw(const tbal::Bitmap& b)
	{
		b.Fill(tbal::Fill(tbal::COLOR_BLACK));
		const int sz = std::min(b.sizeX()/(cells[0].size()+1), b.sizeY()/(cells.size()+1));
		
		for (int j=0; j<cells.size(); ++j)	for (int i=0; i<cells[j].size(); ++i) if (cells[j][i].was>0)
			{
				Point c(i+1,j+1);
				Point dl(1,0);
				tblib::carray<Point,4> pt;
				pt[0] = c*sz+dl*sz*2/5+dl.Rot90()*sz*2/5;
				pt[1] = c*sz+dl*sz*2/5-dl.Rot90()*sz*2/5;
				pt[2] = c*sz-dl*sz*2/5-dl.Rot90()*sz*2/5;
				pt[3] = c*sz-dl*sz*2/5+dl.Rot90()*sz*2/5;

				tbal::Color cl = keyColors[cells[j][i].was-1].Eigth();

				tbgraph::Poly(b, pt.size(), &pt[0], cl);

				//if (cells[j][i].inBestWay)
				//	tbgraph::Circle(b, Point(i+1,j+1)*sz, fx0, sz/4, tbal::COLOR_WHITE);
			}

		for (int j=0; j<cells.size(); ++j)	for (int i=0; i<cells[j].size(); ++i)
		{
			for (int d=0; d<4; ++d)
			{
				Point c(i+1,j+1);
				Point dl(dx[d],dy[d]);

				bool wall=false;
				if (!cells[j][i].canPass[d])
				{
					int ni=i+dx[d], nj=j+dy[d];
					if (InField(ni,nj))
						wall = !cells[nj][ni].canPass[d+2];
					else
						wall = true;

					if (!wall)
					{
						tblib::carray<Point,3> pt;
						pt[0] = c*sz+dl*sz*2/5;
						pt[1] = c*sz+dl*sz*2/3+dl.Rot90()*sz*2/15;
						pt[2] = c*sz+dl*sz*2/3-dl.Rot90()*sz*2/15;
						tbgraph::Poly(b, pt.size(), &pt[0], tbal::Color(0xff, 0xdd, 0xbb));
					}
				}
				if (wall)
				{
					tbgraph::Line(b, c*sz+dl*sz/2+dl.Rot90()*sz/2, c*sz+dl*sz/2-dl.Rot90()*sz/2, 1, 
						tbal::Color(0x44,0x44,0x44));
				} else if (cells[j][i].canPass[d]>=CP_KEY)
				{			
					tbgraph::Line(b, c*sz+dl*sz/2+dl.Rot90()*sz/3, c*sz+dl*sz/2-dl.Rot90()*sz/3, 1, 
						keyColors[cells[j][i].canPass[d]-CP_KEY]);
				}
			}
		}
			
		for (int j=0; j<cells.size(); ++j) for (int i=0; i<cells[j].size(); ++i)
		{
			static tbfont::Font f;
			f.Init(sz/3,1);			
			//f.OutText(buf, (i+1)*sz, (j+1)*sz, tblib::ToString(cells[j][i].dist), tbal::COLOR_WHITE, 1, 1); 

			if (cells[j][i].key>0)
			{
				f.OutText(b, (i+1)*sz, (j+1)*sz, "K", keyColors[cells[j][i].key], 1, 1); 
			}		
		}
	}	

	bool FindStupidWay (int x1, int y1, int x2, int y2, int cw)
	{
		bool result = false;
		tblib::ring<IPoint, SIZEX*SIZEY+1> r;

		for (int j=0; j<SIZEY; ++j) for (int i=0; i<SIZEX; ++i)
			cells[j][i].dist=-2;

		r.push(IPoint(x1, y1, 0));
		cells[y1][x1].dist = -1;
		while (!r.empty())
		{
			IPoint p = r.pop();

			if (cells[p.y][p.x].was!=0 && cells[p.y][p.x].was<cw)
			{
				x2=p.x; 
				y2=p.y;
				// обход с конца!!!
				while (y2!=y1 || x2!=x1)
				{
					if (cells[y2][x2].was==0)
					{
						cells[y2][x2].was = cw;
						result = true;
					}
					int d = cells[y2][x2].dist;
					assert (d>=0);
					d %= 4;

					int nx=x2+dx[d], ny=y2+dy[d];
					cells[ny][nx].canPass[d+2] = CP_OPENED;

					if (d==DIR_DOWN || (d==DIR_LEFT || d==DIR_RIGHT) && rnd.random(2)==0)
						cells[y2][x2].canPass[d] = CP_OPENED;

					x2=nx;
					y2=ny;
				}
				return result;
			}

			for (int d=0;d<4;++d)
			{
				bool ok = false;
					
				int nx=p.x+dx[d], ny=p.y+dy[d];

				if (InField(nx,ny) && cells[ny][nx].dist==-2)
				{
					if (cells[p.y][p.x].canPass[d] || cells[ny][nx].canPass[d+2])
						ok=true;
					else if (cells[ny][nx].was == 0)
						ok=true;
					else if (cells[ny][nx].was/2 == cells[p.y][p.x].was/2)
						ok=true;
				}

				if (ok)
				{
					r.push(IPoint(nx, ny, 0));
					cells[ny][nx].dist = d+2;
				}
			}
		}
		return false;
	}

	bool DoLoop(int x, int y, int curWas, int retC)
	{
		bool result=false;
		int cw=curWas, cx=x, cy=y;
		for (int i=0;; ++i)
		{
			if (cells[cy][cx].was==0)
			{
				cells[cy][cx].was = cw;
				result=true;
			}

			int d=-1;

			if (i>retC)
				return FindStupidWay (cx, cy, x, y, cw) || result;
			if (d>=0) goto KNOW_D;

			int c=0;
			// поиск1: в сторону пустой €чейки
			for (int ad=0; ad<4; ++ad)
			{
				int anx=cx+dx[ad], any=cy+dy[ad];
				if (InField(anx,any) && cells[any][anx].was==0)
				{
					if (cells[cy][cx].was/2<curWas/2 && dx[ad]==0)
						;  // вертикальные туннели с ключами? нафиг!
					else if (rnd.random(++c)==0)
						d=ad;
				}
			}
			if (d>=0) goto KNOW_D;
			//if (rnd.random(3)==0)
			{
				// поиск2: вылезти в место, где были ранее
				// злоупотребл€ть не стоит, а то одни двери везде будут, лол
				c=0;
				for (int ad=0; ad<4; ++ad)
				{
					int anx=cx+dx[ad], any=cy+dy[ad];
					if (ad!=DIR_UP && InField(anx,any) 
					&& cells[any][anx].was>0 && cells[any][anx].was/2 <= cells[cy][cx].was/2 && rnd.random(++c)==0)
						d=ad+4;
				}
				if (d>=0) goto KNOW_D;	
			}
			// поиск3: поход против шерсти
			c=0;
			for (int ad=0; ad<4; ++ad)
			{
				int anx=cx+dx[ad], any=cy+dy[ad];
				if (InField(anx,any) &&  cells[cy][cx].canPass[ad] && rnd.random(++c)==0)
					d=ad;
			}
			if (d>=0) goto KNOW_D;					
			
			// поиск4: куда попало
			c=0;
			for (int ad=0; ad<4; ++ad)
			{
				int anx=cx+dx[ad], any=cy+dy[ad];
				if (InField(anx,any)) 
				{
					if (ad==DIR_UP && curWas/2 > cells[any][anx].was/2)
						; // прорыв вверх запрещЄн
					else if (rnd.random(++c)==0)
						d=ad;
				}
			}		
KNOW_D:
			assert(d>=0);

			cells[cy][cx].canPass[d] = CP_OPENED;
			int nx=cx+dx[d], ny=cy+dy[d];

			if ((d<4) && (d==DIR_UP || (d==DIR_LEFT || d==DIR_RIGHT) && rnd.random(1)==0))
				cells[ny][nx].canPass[d+2] = CP_OPENED;
			else 
				cw=curWas+1;

			cx=nx;
			cy=ny;

      if (cells[cy][cx].was > 0 && cells[cy][cx].was<=curWas)
				break;
		}
		return result;
	}

	int GetKeyAv (int x, int y, int maxLoopId)
	{
		if (cells[y][x].was>0)
			return std::min(MAX_DOORS+1, (cells[y][x].was/2-1)*(MAX_DOORS+1)/(maxLoopId-1)+1);
		else
			return 0;
	}	

	void PutDists (int x, int y, int curKey)
	{		
		for (int j=0; j<cells.size(); ++j) for (int i=0; i<cells[j].size(); ++i)
			cells[j][i].dist=0;


		tblib::ring<IPoint, SIZEX*SIZEY+1> r;
		r.push(IPoint(x,y,4));
		cells[y][x].dist = 4;
		while (!r.empty())
		{
			IPoint p = r.pop();
			p.dst = (p.dst & (~3));
			if (!cells[p.y][p.x].inOrder)
			{
				cells[p.y][p.x].inOrder = true;
				orderedCells.emplace_back(p.x, p.y, 0);
			}

			for (int d=0; d<4; ++d) if (cells[p.y][p.x].canPass[d] && cells[p.y][p.x].canPass[d]<=curKey+2)
			{
				int nx=p.x+dx[d];
				int ny=p.y+dy[d];
				if (cells[ny][nx].dist==0)
				{
					cells[ny][nx].dist = p.dst+d+4;
					r.push(IPoint(nx,ny,p.dst+d+4));
				}
			}
		}
	}

	void PutKeys ()
	{
		int maxLoopId=2;		
		for (int j=0; j<cells.size(); ++j) for (int i=0; i<cells[j].size(); ++i) if (cells[j][i].was>0)
			tblib::enlarge(maxLoopId, cells[j][i].was/2);
		
		for (int j=0; j<cells.size(); ++j) for (int i=0; i<cells[j].size(); ++i) 
		{
			cells[j][i].was = GetKeyAv(i,j,maxLoopId);		
			cells[j][i].inBestWay = false;
		}
		
		for (int j=0; j<cells.size(); ++j) for (int i=0; i<cells[j].size(); ++i) if (cells[j][i].was>0)
		{
			for (int d=0; d<4; ++d) if (cells[j][i].canPass[d])
			{
				int nx=i+dx[d],ny=j+dy[d];
				int k1 = cells[j][i].was;
				int k2 = cells[ny][nx].was;

				if (k1<k2)
				{
					cells[j][i].canPass[d] = CP_KEY+k2-1;
					//if (cells[j+dy[d]][i+dx[d]].canPass[d+2])
						cells[j+dy[d]][i+dx[d]].canPass[d+2] = CP_KEY+k2-1;

						assert (dy[d]==0); // нет дверей с ключами вверх и вниз

				}
			}
		}

		tblib::carray<bool, MAX_DOORS+1> isArea;	
		for (int i=0; i<isArea.size(); ++i)
			isArea[i] = false;

		for (int j=0; j<cells.size(); ++j) for (int i=0; i<cells[j].size(); ++i) if (cells[j][i].was>0)
			isArea[cells[j][i].was-1] = true;
			
		int cx=0, cy=0;

		for (int k=0; k<=MAX_DOORS; ++k) if (k==MAX_DOORS || isArea[k+1])
		{
			PutDists(cx, cy, k);

			int kx=-1, ky=-1;

			for (int gw=k+1;gw>0;--gw)
			{
				int c=0, lk=0;
				for (int j=0; j<cells.size(); ++j) for (int i=0; i<cells[j].size(); ++i) if (cells[j][i].was>0 && cells[j][i].key==0)
				{
					//assert(cells[j][i].dist>0);
					if (cells[j][i].was==gw)
					{
						if (cells[j][i].dist>lk)
						{
							kx=i;
							ky=j;
							lk=cells[j][i].dist;
							c=0;
						} else if (cells[j][i].dist==lk && rnd.random(++c)==0)
						{
							kx=i;
							ky=j;
						}
					}
				}
				if (kx>=0)
				{
					cells[ky][kx].key = k+1;

					int ocx=cx, ocy=cy;

					cx=kx;
					cy=ky;

					while (kx!=ocx || ky!=ocy)
					{
						int d = cells[ky][kx].dist;
						kx-=dx[d];
						ky-=dy[d];
						cells[ky][kx].inBestWay=true;
					}

					break;
				}
			}
		}
	}

	void FillSegments ()
	{	
		for (int j=0; j<cells.size(); ++j) for (int i=0; i<cells[j].size(); ++i)
  	{
			if (cells[j][i].was)
			{
				int ni=i;

				while (ni<cells[j].size()-1 
					&& (cells[j][ni].canPass[DIR_RIGHT] || cells[j][ni+1].canPass[DIR_LEFT]))
					++ni;

				for (int k=i; k<=ni; ++k)
				{
					cells[j][k].firstx = i;
					cells[j][k].lastx = ni;
				}
          
				i = ni;
			}
		}
		
		for (int j=0; j<cells.size(); ++j) for (int i=0; i<cells[j].size(); ++i)
			cells[j][i].dist=0;

		tblib::array<IPoint, SIZEX*SIZEY> segments;

		for (int c=0; c<orderedCells.size(); ++c)
		{
			int cx = orderedCells[c].x;
			int cy = orderedCells[c].y;
			int segx = cells[cy][cx].firstx;
			if (cy>0 && cy<cells.size()-1 && cells[cy][segx].dist==0)
			{
				segments.emplace_back(segx, cy, 0);
				cells[cy][segx].dist=1;
			}
		}

		for (int s=0; s<segments.size(); ++s)
		{
			int pr = s*5/segments.size();
			int maxr, minr;
			switch (pr)
			{
				case 0  : minr=0; maxr=1; break;
				case 1  : minr=0; maxr=2; break;
				case 2  : minr=1; maxr=3; break;
				case 3  : minr=2; maxr=4; break;
				default : minr=3; maxr=4; break;
			}

			Setting sg;

			switch (minr + rnd.random(maxr-minr+1))
			{
				case 0  : sg = SG_UNDERGROUND;  break;
				case 1  : sg = SG_CAVES;        break;
				case 2  : sg = SG_BUNKER;       break;
				case 3  : sg = SG_LAIR;         break;
				default : sg = SG_COMPCENTER;   break;
			}
			
			int cx = segments[s].x;
			int cy = segments[s].y;
			
			for (int k=cells[cy][cx].firstx; k<=cells[cy][cx].lastx; ++k)
				cells[cy][k].setting = sg;			
		}
	}

	void MakePasses ()
	{				
		lastLabel=1;
		for (int j=0; j<cells.size(); ++j) for (int i=0; i<cells[j].size(); ++i)
		{
			PassInfo &pi = cells[j][i];
			for (int d=0; d<4; ++d)
				pi.canPass[d]=CP_CLOSED;
			pi.was = 0;
			pi.key = 0;
			pi.dist = 0;
			pi.setting = j==0 ? SG_FOREST : j==cells.size()-1 ? SG_HELL : SG_CAVES;
			pi.inOrder = false;
		}

		int ls = std::max(cells.size(),cells[0].size())/2;
		int cw=2;
		if (DoLoop(0, 0, cw, ls*2))
			cw+=2;
	
		for (int t=1; t<=cells.size()*cells[0].size()/2+4; ++t)
		{
			bool all=true;
			int x=-1,y=-1;
			int c=0;
			for (int j=0; j<cells.size(); ++j) for (int i=0; i<cells[j].size(); ++i)
			{
        if (cells[j][i].was==0)
				{
					all=false;
				} else if (rnd.random(++c)==0)
				{
					x=i; 
					y=j;
				}
			}
			if (all || !InField(x,y))
				break;
			if (DoLoop(x, y, cw, ls*2))
				cw+=2;
		}
	}

	void Generate ()
	{
		MakePasses ();
		PutKeys();
		FillSegments ();		
	}
};