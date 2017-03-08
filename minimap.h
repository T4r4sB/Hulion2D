#pragma once

void ShowMinimap (const tbal::Bitmap& b)
{
	int my = level.t.maxheight;

	int scale = 20;
	int mmsy = my/scale, shy = MMY-1-(mmsy-1)*scale;

	int bx = MMX/scale+2, by = mmsy+2;

	tbal::Bitmap bw = b.Window(b.sizeX()/2 - bx/2, b.sizeY()/2 - by/2, bx, by);

	Bevel(bw, true);

	tbal::Bitmap::line l = bw[1];
	for (int j=0; j<mmsy; ++j)
	{
		for (int i=0; i<MMX/scale; ++i)
		{
			int mx = i*scale;
			int my = j*scale+shy, ty = MMY-1-my;

			bool ok = false;
			for (int mj=0; mj<level.t.cells.size(); ++mj)
			{
				if (level.t.cells[mj][mx/CELL_WIDTH].was && ty>=level.t.heights[mj][mx].lo)
				{
					ok=wasIn[mj][mx/CELL_WIDTH];
					break;
				}
			}

			if (ok)
				l[i+1] = colorTable [ level.map[my][mx] ];
		}

		++l;
	}

	Point ps = player->pos/scale;

	tbgraph::Circle(bw, Point(ps.x+fx1, Fixed(by-2)-ps.y), fx0, fx2, tbal::COLOR_WHITE); 

}