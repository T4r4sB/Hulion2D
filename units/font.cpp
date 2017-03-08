#include "graph.h"
#include "fontdata.h"	
#include "font.h"
#include "algorithm.h"
#include "tbstring.h"

namespace tbfont
{	
	struct FontPoint
	{
		unsigned char x:4; 
		unsigned char y:4;
	};

	struct SymbolHeader 
	{
		unsigned char width:4;
		unsigned char count:4;
	};

	struct Symbol
	{
		SymbolHeader header;
		FontPoint point[256];
	};

	const Symbol& GetData(char c)
	{
		return *reinterpret_cast<const Symbol*>(__data+__offsets[(unsigned char)c]);
	}

	Point xy2pt(int x, int y)
	{
		if (y>10) y-=16;
		return Point(x,10-y)/Fixed(10);
	}

	void OutSymbol (const tbal::Bitmap &b, char c, Point pos, Fixed scale, Fixed width, tbal::Color color)
	{
		const Symbol &s = GetData(c);
		for (int i=0; i<s.header.count; ++i)
		{
			Point p1 = xy2pt(s.point[i*3+0].x, s.point[i*3+0].y); 
			Point p2 = xy2pt(s.point[i*3+1].x, s.point[i*3+1].y);
			Point p3 = xy2pt(s.point[i*3+2].x, s.point[i*3+2].y);


			if (p1==p2)
			{
				if (p1==p3)
				{
					tbgraph::Line(b, pos + p1*scale, pos + p2*scale, width, color, false, false);
				}	else
				{
					const Point c = pos+(p1+p3)*scale*fxHalf;
					const Fixed r = (p3-p1).Length()*scale*fxHalf;
					tbgraph::Arc(b, c, r, width, Point( 1,0), Point(-1,0), color, true, true);
					tbgraph::Arc(b, c, r, width, Point(-1,0), Point( 1,0), color, true, true);
				}
			} else
			{
				Fixed s = (p2-p1)^(p3-p1);
				if (s == Fixed(0))
					tbgraph::Line(b, pos + p1*scale, pos + p2*scale, width, color, false, false);
				else 
				{				
					const Point sc = (((p1-p2)*(p3*p3)+(p2-p3)*(p1*p1)+(p3-p1)*(p2*p2))/(s*Fixed(2))).Rot90();		
					const Fixed sr = (sc-p3).Length();

					const Point a1 = (p1-sc)/sr;
					const Point a2 = (p2-sc)/sr;
					const Point a3 = (p3-sc)/sr;

					const Point scs = pos+sc*scale;
					const Fixed srs = sr*scale; 

					if ((a1^a3)>=fx0)
					{
						if ((a3^a2)>=fx0)
						{
							tbgraph::Arc(b, scs, srs, width, a1, a3, color, false, true);
							tbgraph::Arc(b, scs, srs, width, a3, a2, color, true,  false);
						} else if ((a1^a2)>=fx0)
						{
							tbgraph::Arc(b, scs, srs, width, a2, -a2, color, false, true);
							tbgraph::Arc(b, scs, srs, width, -a2, a1, color, true , false);
						} else
						{
							tbgraph::Arc(b, scs, srs, width, a1, -a1, color, false, true);
							tbgraph::Arc(b, scs, srs, width, -a1, a2, color, true , false);
						}
					} else 
					{
						if ((a3^a2)<=Fixed(0))
						{
							tbgraph::Arc(b, scs, srs, width, a2, a3, color, false, true);
							tbgraph::Arc(b, scs, srs, width, a3, a1, color, true , false);
						} else if ((a1^a2)<=Fixed(0))
						{
							tbgraph::Arc(b, scs, srs, width, a1, -a2, color, false, true);
							tbgraph::Arc(b, scs, srs, width, -a2, a2, color, true , false);
						} else
						{
							tbgraph::Arc(b, scs, srs, width, a2, -a1, color, false, true);
							tbgraph::Arc(b, scs, srs, width, -a1, a1, color, true , false);
						}
					} 					
				}
			}	
		}
	}

	const int fontaa = 4, fontaa2 = fontaa*fontaa;
	
	#ifdef TBAL_16_BITS
		class AMove
		{
			tbal::Color color;
		public:
				AMove (tbal::Color color) : color(color) {}
				void operator () (tbal::Color &dst, int src) const
				{
						dst = tbal::Color(
							dst.R() + (color.R()-dst.R())*src/fontaa2,
							dst.G() + (color.G()-dst.G())*src/fontaa2,
							dst.B() + (color.B()-dst.B())*src/fontaa2
						);
				}
		};
	#else			
		class AMove
		{
			tbal::Color color;
		public:
			AMove (tbal::Color color) : color(color & tbal::Color(0xf0, 0xf0, 0xf0)) {}
										
			void operator () (tbal::Color &dst, int src) const
			{
				if (src)
				{
					dst &= tbal::Color(0xf0, 0xf0, 0xf0);
					dst += ((color-dst)>>4)*src;
				}
			}
		};
	#endif

	struct SetNull { void operator () (int& i) const {i=0;}};
		
	void Font::Init (Fixed scale, Fixed width)
	{
		if (scale == this->scale || width == this->width) return;
		this->scale = scale;
		this->width = width;

		for (int i=0; i<256; ++i)
		{
			int sx = scale*Fixed(GetData(char(i)).header.width)/fx10+fxHalf;
			int sy = scale*Fixed(3,2)+width*fx3+fxHalf;     
			widths[i] = sx;			
			this->height = sy;
			if (sx*sy)
			{
				tbal::Picture p(sx*fontaa, sy*fontaa); 
				p.Fill(tbal::Fill(tbal::COLOR_BLACK));
				OutSymbol(p, (char)i, Point(width*Fixed(fontaa),width*Fixed(fontaa)), scale*Fixed(fontaa), width*Fixed(fontaa), tbal::COLOR_RED);
	        
				tblib::recreate(images[i],sx,sy);
				images[i].Fill(SetNull());
				for (int j=0; j<sy*fontaa; ++j)	for (int k=0; k<sx*fontaa; ++k)
				{
					if (p[j][k].R())
						++images[i][j/fontaa][k/fontaa];
				}
				
				for (int j=0; j<sy; ++j) for (int k=0; k<sx; ++k)
				{
					int &v = images[i][j][k];
					v = (v*3*fontaa2*fontaa2
					  + v*v*39*fontaa2 
					  - v*v*v*26)
					  /(16*fontaa2*fontaa2);
				}
			}				
		}
	}
		
	int Font::Width(tblib::stringref c) const
	{
		int result=0;
		for (int i=c.low(); i<c.high(); ++i)
			result+=widths[(unsigned char)(c[i])];
		return result;
	}

	int Font::Height(tblib::stringref c) const
	{
		(void)c;
		return height*2/3;
	}

	void Font::OutText(const tbal::Bitmap &b, int x, int y, tblib::stringref c, tbal::Color color, int alignX, int alignY) const
	{
		x -= Width  (c) * alignX / 2;
		y -= Height (c) * alignY / 2;

		for (int i=c.low(); i<c.high(); ++i)
		{
			int w = widths[(unsigned char)(c[i])];
			b.Copy(AMove(color), images[(unsigned char)(c[i])], x, y);
			x+=w;
		}
	}	
};
