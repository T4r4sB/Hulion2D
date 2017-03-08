#include "tbal.h"
#include "fixed.h"
#include "math.h"
#include "point.h"
#include "graph.h"
#include "tbarr.h"

#include <sstream>

namespace tbgraph 
{
	void Poly (const tbal::Bitmap &b, size_t count, const Point pt[], tbal::Color color)
	{
		int hy1 = +1000;
		int hy2 = -1000;
		size_t M0 = count;
		for (size_t i=0; i<count; ++i)
		{
			if (i==0 || int(pt[i].y)<hy1)
			{
				hy1 = int(pt[i].y);
				M0 = i;
			} 
			if (i==0 || int(pt[i].y)>hy2)
			{
				hy2 = int(pt[i].y);
			}
		}
		size_t ip1, ip2, in1=M0, in2=M0;
		int y1 = std::max(0        , hy1);
		int y2 = std::min(b.sizeY(), hy2);
		
		Fixed x1, x2, dx1, dx2;

		if (y1>=y2) return;
		tbal::Bitmap::line line = b[y1];

		for (int y=y1; y<y2; ++y)
		{
			if (y>=int(pt[in1].y)) // переход к следующему отрезку
			{
				do
				{
					ip1 = in1;
					in1 = in1==0 ? count-1 : in1-1;
				} while (y>=int(pt[in1].y));

				if (y+1==int(pt[in1].y)) 
				{
					dx1 = pt[in1].x>pt[ip1].x ? Fixed(1000) : Fixed(-1000);
					x1=pt[ip1].x + (pt[in1].x-pt[ip1].x)
						*((Fixed(y+1) - pt[ip1].y)
						/( pt[in1].y  - pt[ip1].y));
				} else
				{
					dx1 = (pt[in1].x-pt[ip1].x)/(pt[in1].y-pt[ip1].y);
					x1=pt[ip1].x + (Fixed(y+1)-pt[ip1].y)*dx1;
				}
			}

			if (y>=int(pt[in2].y)) // переход к следующему отрезку
			{
				do // находим следующий нужный участок
				{  
					ip2 = in2;
					in2 = in2==count-1 ? 0 : in2+1;
				} while (y>=int(pt[in2].y));

				if (y+1==int(pt[in2].y)) // вырожденный случай
				{
					dx2 = pt[in2].x>pt[ip2].x ? Fixed(1000) : Fixed(-1000);
					x2=pt[ip2].x + (pt[in2].x-pt[ip2].x)
						*((Fixed(y+1) - pt[ip2].y)
						/( pt[in2].y  - pt[ip2].y));
				} else
				{
					dx2 = (pt[in2].x-pt[ip2].x)/(pt[in2].y-pt[ip2].y);
					x2=pt[ip2].x + (Fixed(y+1)-pt[ip2].y)*dx2;
				}
			}

			int ax1 = std::max(int(x1)+1, 0);
			int ax2 = std::min(int(x2)+1, b.sizeX());	
			for (int x=ax1; x<ax2; ++x)
				line[x] = color;

			x1   += dx1;
			x2   += dx2;
			++line;
		}
	}	

	const int tc=32;
	Point tt[tc+1];
	bool checkedtt = false;

	void CheckTable ()
	{
		if (!checkedtt)
		{	
			tt[0] = Point(fx1, fx0);
			for (int i=1; i<tc; ++i)
				tt[i] = SinCos(Fixed(i,tc)*fxPi);
			tt[tc] = Point(-fx1, fx0);
			checkedtt = true;
		}
	}
	
	bool CircleInBuffer (const tbal::Bitmap &b, const Point &center, Fixed r)
	{
		return (int(center.x+r)>=0 && int(center.y+r)>=0 && int(center.x-r)<b.sizeX() && int(center.y-r)<b.sizeY());
	}

	void Circle(const tbal::Bitmap &b, const Point &center, Fixed ri, Fixed ro, tbal::Color color)
	{
		if (!CircleInBuffer(b, center, ro))
			return;

		CheckTable();
		if (ri==fx0)
		{
			Point cur[tc*2];
			for (int i=0; i<tc; ++i)
			{
				cur[i   ]=center+tt[i]*ro;
				cur[i+tc]=center-tt[i]*ro;
			}
			Poly(b, tc*2, cur, color);
		} else {
			Point cur[4];
			for (int i=0; i<tc; ++i)
			{
				cur[0] = center+tt[i  ]*ri;
				cur[1] = center+tt[i  ]*ro;
				cur[2] = center+tt[i+1]*ro;
				cur[3] = center+tt[i+1]*ri;
				Poly(b, 4, cur, color);
				cur[0] = center+(center-cur[0]);
				cur[1] = center+(center-cur[1]);
				cur[2] = center+(center-cur[2]);
				cur[3] = center+(center-cur[3]);
				Poly(b, 4, cur, color);
			}
		}
	}

	void Pie (const tbal::Bitmap &b, const Point &center, Fixed r, const Point &a1, const Point &a2, tbal::Color color)
	{
		if (!CircleInBuffer(b, center, r))
			return;
		CheckTable();		
		const Point na1 = a1.Normalize();
		const Point na2 = a2.Normalize();
		Point cur[tc+2];
		const Point diff = na1&~na2;
		bool ok=true;
		int i;

		for (i=0; i<=tc; ++i)
		{
			Point da = na1&tt[i];
			if (tt[i].x<diff.x) {da=na2;ok=false;}
			cur[i]=center+da*r;
			if (!ok)break;
		}
		cur[i++]=center;
		Poly(b, i, cur, color);
	}

	void Line(const tbal::Bitmap &b, const Point &p1, const Point &p2, Fixed width, tbal::Color color, bool sh1, bool sh2)
	{
		width *= fxHalf;
		Point delta = p2-p1;
		if (delta==Point(fx0,fx0))
		{
			if (!sh1 || !sh2)
				Circle(b, p1, 0, width, color);
		} else 
		{
			delta = delta.Normalize().Rot90();
			const Point wdelta = delta*width;

			tblib::carray<Point, 4> pt;
			pt[0] = p1 - wdelta; 
			pt[1] = p1 + wdelta; 
			pt[2] = p2 + wdelta; 
			pt[3] = p2 - wdelta; 
			Poly(b, pt.size(), &pt[0], color);

			if (!sh1) Pie(b, p1, width, -delta,  delta, color);
			if (!sh2) Pie(b, p2, width,  delta, -delta, color);
		}
	}

	void Arc (const tbal::Bitmap &b, const Point &center, Fixed r, Fixed width, const Point &a1, const Point &a2, tbal::Color color, bool sh1, bool sh2)
	{
		if (!CircleInBuffer(b, center, r+width))
			return;

		CheckTable();
		if (r<Fixed(1,10)) return;
		width /= Fixed(2);
		const Fixed r1=r-width;
		const Fixed r2=r+width;

		//const Point na1 = a1*(fx1/a1.Length() + a1.Length()/(a1*a1))*fxHalf;
		//const Point na2 = a2*(fx1/a2.Length() + a2.Length()/(a2*a2))*fxHalf;
		
		const Point na1 = a1.Normalize();
		const Point na2 = a2.Normalize();
		
		tblib::carray<Point,4> cur;
		bool ok=true;
		int i;
		Point da(0,0), oda(0,0);
		for (i=0; i<=tc; ++i)
		{
			oda = da;
			da  = na1&tt[i];
			if ((da&~na2).y>fx0) {da=na2;ok=false;}

			if (i>0)
			{
				cur[0] = center+ da*r2;
				cur[1] = center+ da*r1;
				cur[2] = center+oda*r1;
				cur[3] = center+oda*r2;
				Poly(b, cur.size(), &cur[0], color);				
			}

			if (!ok)break;
		}
		
		if (!sh1) Pie(b, center+a1*r, width, -na1, na1, color);
		if (!sh2) Pie(b, center+a2*r, width, na2, -na2, color);
	}
};
