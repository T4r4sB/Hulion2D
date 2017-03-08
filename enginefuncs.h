#pragma once
#include "engine.h"

bool PaintToBlood(Point p)
{		
	int x = p.x.Round();
	int y = MMY-p.y.Round()-1;
	uint8_t& px = level.map[y][x];
	if (px >= COLOR_FIRST)
		px = (rnd.random(2)==0 ? COLOR_BLOOD_1 : COLOR_BLOOD_2);
	return px>=COLOR_WALL_2;
}

void KillBullet (Item* item, Point d, Fixed dl)
// вспомогательный метод, который тащит говноконтекст
{
	if (item->itemClass->blWeaponClass)
	{
		// откатываем назад чтоб вылезла из того, в чём она застряла
		item->pos -= d;
		item->distancePassed -= dl;
		item->GenerateBullets(item->itemClass->blWeaponClass);
	}
	item->invalid = true;
}

struct ItemIsKillable 
{
	Item* item;

	ItemIsKillable (Item* item) : item(item) {}

	bool operator () (Item* it)
	{
		return 
			!it->invalid 
			&& it->itemClass->type==IT_UNIT  
			&& it->deathTime == 0 
			&& !(item->hostPlayer && it->itemClass->unitPlayer) 
			&& !(item->hostMonster && !it->itemClass->unitPlayer);
	}
};

void MoveBullet (Item* item)
{
	item->idleTime = 0;

	item->pp3 = item->pp2;
	item->pp2 = item->pp1;
	item->pp1 = item->pos;

	if (!item->itemClass->blFly)
		item->delta.y -= G*Fixed(DT,1000)*Fixed(DT,1000);

	Point d = item->delta;
	Point np = item->pos + d;
	int l = int(std::max(d.x.Abs(), d.y.Abs())) + 1;

	d /= l;
	Fixed dl = d.Length();

	ACheckPtr ci (ItemIsKillable(item),
		std::min(item->pos.x, np.x).Round(), std::min(item->pos.y, np.y).Round(),
		std::max(item->pos.x, np.x).Round(), std::max(item->pos.y, np.y).Round());
	
	ACheckPtr doors(ItemIsDoor(),
		std::min(item->pos.x, np.x).Round(), std::min(item->pos.y, np.y).Round(),
		std::max(item->pos.x, np.x).Round(), std::max(item->pos.y, np.y).Round());


	for (int i=0; i<l; ++i)
	{
		item->pos += d;
		item->distancePassed += dl;

		if (GetColor(item->pos.x.Round(), item->pos.y.Round(), doors) <= COLOR_WALL_2)
		{
			KillBullet(item, d, dl);
			return;			
		}
		if (item->distancePassed > Fixed(item->itemClass->blDistance))
		{
			KillBullet(item, d, dl);
			return;	
		}
		// проверка кококоллизий с монстрами
		for (int m=0; m<ci.size(); ++m)
		{
			if (!ci[m]->invalid
				&& item->pos.x >= ci[m]->pos.x - Fixed(ci[m]->itemClass->width,2)
				&& item->pos.x <= ci[m]->pos.x + Fixed(ci[m]->itemClass->width,2)
				&& item->pos.y >= ci[m]->pos.y
				&& item->pos.y <= ci[m]->pos.y + Fixed(ci[m]->itemClass->height))
			{
				DamageItem(ci[m], item->itemClass->blDamage);

				Point pp = item->pos;
				int c = std::min(100,l*item->itemClass->blDamage), c1 = rnd.random(c), c2 = c + rnd.random(c);

				pp += d*c1;
				for (int j=c1; j<c2; ++j)
				{
					if (!PaintToBlood(pp)) break;
					pp += d;
				}

				KillBullet(item, d, dl);
				return;
			}
		}
	}

	item->stepStage = (item->stepStage+1)%(AL*4);
}

void MoveItem (Item* item)
{
	item->onGround = item->GetWall(item->pos.x.Round(), item->pos.y.Round()-1) <= COLOR_STAIRS;
	item->onStairs = item->GetWall(item->pos.x.Round(), item->pos.y.Round()-1) == COLOR_STAIRS;

	if (!item->OnGround())
		item->pos.y -= fx1;
}

void MoveDoor (Item* item)
{
	if (item->stepStage>0)
	{
		item->pos.y -= fx1;
		++item->stepStage;
		if (item->stepStage>DOOR_HEIGHT)
			item->invalid = true;
	}
}

void MoveGibs (Item* item)
{			
	item->idleTime = 0;
	item->delta.y -= G*Fixed(DT,1000)*Fixed(DT,1000);

	Point d = item->delta;
	Point np = item->pos + d;
	int l = int(std::max(d.x.Abs(), d.y.Abs())) + 1;

	d /= l;
	Fixed dl = d.Length();	

	ACheckPtr ci(ItemIsDoor(),
		std::min(item->pos.x, np.x).Round(), std::min(item->pos.y, np.y).Round(),
		std::max(item->pos.x, np.x).Round(), std::max(item->pos.y, np.y).Round());
	
	for (int i=0; i<l; ++i)
	{
		if (GetColor(item->pos.x.Round(), item->pos.y.Round(), ci) <= COLOR_WALL_2)
		{
			item->invalid = true;
			return;			
		}
		item->pos += d;
		PaintToBlood(item->pos);
	}
}

void MoveBorner (Item* item)
{
	MoveItem (item);	
	if (item->deathTime == 0)
	{
		item->stepStage = (item->stepStage+1)%(AL*4);
		
		item->bornStage += DT;
		if (item->bornStage >= 900)
		{
			item->bornStage = 0;
			if (!item->childs.full())
				BornUnit(item->itemClass->unitChildClass, item);
		}
	} else
	{
		++item->stepStage;
		tblib::shrink(item->stepStage, AL*8-1);
	}
}

void MoveUnit (Item* item)
{		
	if (!item->itemClass->unitFly || item->deathTime > 0)
		item->delta.y -= G*Fixed(DT,1000)*Fixed(DT,1000);

	Point np = item->pos+item->delta;

	int x = item->pos.x.Round(), sx=x;
	int y = item->pos.y.Round(), sy=y;

	int nx = np.x.Round();
	int ny = np.y.Round();
	
	bool stopped;

	// сначала трассируемся по иксу
	stopped = false;
	int dx = nx>x ? 1 : -1;
	for (; x!=nx; x+=dx) 
	{
		assert(item->GetWall(x,y));
		bool can=false;
		int trc = (item->onGround || item->itemClass->unitFly) ? 6:1;
		for (int tr=0; tr<trc; ++tr) 
		{	
			for (int k=item->itemClass->unitFly ? 0 : 1; k<2; ++k)
			{
				int t = k==0 ? -tr : tr;

				if (item->GetWall(x+dx,y+t) > COLOR_STAIRS_1)
				{
					// сдвиг реальности О_О
					can=true;
					if (tr>0)
					{
						y += t;
						ny += t;
						sy += t;
						np.y += t;
					}
					goto TR_LOOP_BREAK;
				}
			}
		}
TR_LOOP_BREAK :
		if (!can)
		{
			stopped = true;
			break;
		}
	}
	assert(item->GetWall(x,y));

	if (stopped)
	{
		item->delta.x = x-sx;
		item->pos.x = x;
	} else
	{
		item->pos.x = np.x;
	}

	item->idle = true;
	if ((item->controls.left || item->controls.right) && !stopped)
		item->idle = false;		
	// теперь трассируемся по игреку
	stopped = false;
	int dy = ny>y ? 1 : -1;
	for (; y!= ny; y+=dy)
	{		
		assert(item->GetWall(x,y));
		if (item->GetWall(x,y+dy) <= COLOR_WALL_2
			|| !item->wantDown && dy<0 && item->GetWall(x,y+dy) <= COLOR_STAIRS)
		{
			stopped = true;
			break;
		}
	}		
	assert(item->GetWall(x,y));

	if (stopped)
	{
		item->delta.y = y-sy;
		item->pos.y = y;
	} else
	{
		item->pos.y = np.y;
	}
	//
	if ((item->controls.up || item->controls.down) && !stopped || item->itemClass->unitFly)
		item->idle = false;
	// 
	item->onGround = item->GetWall(item->pos.x.Round(), item->pos.y.Round()-1) <= COLOR_STAIRS;
	item->onStairs = item->GetWall(item->pos.x.Round(), item->pos.y.Round()-1) == COLOR_STAIRS;
	
	
	item->delta.x = fx0;
	if (item->OnGround() || (item->itemClass->unitFly && item->deathTime==0))
		item->delta.y = fx0;

	if (item->deathTime == 0)
	{
		if (item->idle)
			item->stepStage = 0;
		else
			item->stepStage = (item->stepStage+1)%(AL*4);	
	} else 
	{
		++item->stepStage;
		tblib::shrink(item->stepStage, AL*8-1);
	}

	if (item->wpStepStage>AL)
		item->wpStepStage = (item->wpStepStage-AL+1)%(AL*3)+AL;	

	assert(item->GetWall(item->pos.x.Round(), item->pos.y.Round()));
}

void AIPlayer (Item* item) 
{
	(void) item;
}

void AIStupidGroundUnit (Item* item) 
{
	bool f = (item->pos.x - player->pos.x).Abs() > Fixed(12) 
		|| (item->pos.y - player->pos.y).Abs() > Fixed(32);
	bool cr = (item->pos.x < player->pos.x) && f;
	bool cl = (item->pos.x > player->pos.x) && f;
	item->controls.left = cl;
	item->controls.right = cr;

	bool needDown = (item->pos.y > player->pos.y && item->OnStairs ());

	item->controls.up = (!f || rnd.random(1000)<DT) && (!needDown);
	item->controls.down = needDown || (!item->controls.up && rnd.random(1000)<DT);
	item->controls.shot = true;
}

void AICleverGroundUnit (Item* item) 
{
	bool r = item->pos.x < player->pos.x;
	bool f = (item->pos.x - player->pos.x).Abs() > Fixed(12) 
		|| (item->pos.y - player->pos.y).Abs() > Fixed(32);
	bool cr = (item->pos.x < player->pos.x) && f;
	bool cl = (item->pos.x > player->pos.x) && f;
	item->controls.left = cl;
	item->controls.right = cr;

	bool needDown = 
		(item->pos.y - player->pos.y >  (item->pos.x - player->pos.x).Abs())
		|| (item->pos.y > player->pos.y && item->OnStairs());
	bool needUp   =
		(item->pos.y - player->pos.y < -(item->pos.x - player->pos.x).Abs())
		|| (item->pos.y < player->pos.y && item->OnStairs());

	item->controls.up = !f || needUp 
		|| !needDown && Visible(item->pos, item->pos + Point(r?1:-1, -1) * 5)
		|| !Visible(item->pos + Point(0,3), item->pos + Point((r?1:-1)*(item->itemClass->width+2), 3));

	item->controls.down = needDown;
	item->controls.shot = true;
}

void AIJumpingUnit (Item* item) 
{
	bool f = (item->pos.x - player->pos.x).Abs() > Fixed(12) 
		|| (item->pos.y - player->pos.y).Abs() > Fixed(64);
	bool cr = (item->pos.x < player->pos.x) && f;
	bool cl = (item->pos.x > player->pos.x) && f;
	item->controls.left = cl;
	item->controls.right = cr;

	bool needDown = item->pos.y - player->pos.y >  (item->pos.x - player->pos.x).Abs();

	item->controls.up = !needDown;
	item->controls.down = needDown;
	item->controls.shot = true;
}

void AIFlyingUnit (Item* item) 
{
	bool r = item->pos.x < player->pos.x;
	bool f = (item->pos.x - player->pos.x).Abs() > Fixed(12) 
		|| (item->pos.y - player->pos.y).Abs() > Fixed(12);
	bool cr = (item->pos.x < player->pos.x) && f;
	bool cl = (item->pos.x > player->pos.x) && f;
	item->controls.left = cl;
	item->controls.right = cr;

	Point pu = item->pos + Point(0, item->itemClass->height), pd = item->pos;

	bool vu = Visible (pu, pu + Point(r?1:-1, +1)*16);
	bool vd = Visible (pd, pd + Point(r?1:-1, -1)*16);
	bool ou = vu && !vd, od = vd && !vu;

	bool high = (item->pos.y - player->pos.y)>fx0; //*fx3 >  (item->pos.x - player->pos.x).Abs();

	bool needDown = ( high || od) && !ou;
	bool needUp   = (!high || ou) && !od;

	item->controls.up = needUp && !needDown;
	item->controls.down = !needUp && needDown;
	item->controls.shot = true;
}

void DrawItemStage (Item* item, const tbal::Bitmap& b, int viewX, int viewY, int stage)
{
	ItemClass *ic = item->itemClass;
	int x = item->pos.x.Round()-item->itemClass->frameWidth/2;
	int y = item->pos.y.Round()+item->itemClass->frameHeight;
	bool refl = item->reflected;

	b.Copy(tbal::TransparentTest(tbal::COLOR_BLACK), 
				(refl ? ic->picturel : ic->picturer)
				.Window(0, ic->frameHeight*stage, ic->frameWidth, ic->frameHeight), 
				XCenter(b) + (x - viewX), 
				YCenter(b) - (y - viewY - 1));
}

void DrawItem (Item* item, const tbal::Bitmap& b, int viewX, int viewY)
{
	DrawItemStage (item, b, viewX, viewY,	item->stepStage/AL);
}

struct PurpleToBlack 
{
	void operator () (tbal::Color& dst, tbal::Color src) 
	{
		if (src == tbal::COLOR_PURPLE)
			dst = tbal::COLOR_BLACK;
		else if (src != tbal::COLOR_BLACK)
			dst = src;
	}
};

void DrawUnit (Item* item, const tbal::Bitmap& b, int viewX, int viewY)
{
	static tbal::Picture buf(128,128);
	ItemClass *ic = item->itemClass;

	int dx = 0;	
	if (item->deathTime==0 && item->wpnumber>=0)
	{
		ItemClass *wc = item->weapons[item->wpnumber].weaponClass;
		if (wc->frameWidth>0)
		{
			dx = wc->frameWidth - ic->frameWidth;
			assert(dx>=0);
		}
	}

	buf.Copy(tbal::CopyPixel(), 
		(item->reflected ? ic->picturel : ic->picturer)
		.Window(0, ic->frameHeight*(item->stepStage/AL), ic->frameWidth, ic->frameHeight), 
		item->reflected?dx:0, 0); 

	buf.Window(item->reflected ? 0 : ic->frameWidth, 0, dx, ic->frameHeight).Fill(tbal::Fill(tbal::COLOR_BLACK));

	if (item->deathTime==0 && item->wpnumber>=0)
	{		
		ItemClass *wc = item->weapons[item->wpnumber].weaponClass;

		int wss = item->wpStepStage/AL;

		if (wc->wpHasEmptyImage && wss==1 && wc->wpArmorIndex>=0 && item->armors[wc->wpArmorIndex].bulletCount==0)
			wss = 4;
		
		buf.Copy(PurpleToBlack(), 
			(item->reflected ? wc->picturel : wc->picturer)
			.Window(0, wc->frameHeight*wss, wc->frameWidth, wc->frameHeight), 
			item->reflected ? 0 : 0, 0);
	}


	int x = item->pos.x.Round()-item->itemClass->frameWidth/2;
	int y = item->pos.y.Round()+item->itemClass->frameHeight;
	b.Copy(tbal::TransparentTest(tbal::COLOR_BLACK), buf.Window(0, 0, ic->frameWidth+dx, ic->frameHeight),
		XCenter(b) + (x - viewX) - (item->reflected?dx:0), 
		YCenter(b) - (y - viewY - 1));
		
	if (item->health < ic->unitHealth && item->deathTime==0)
	{
		Point p1 (XCenter(b) + (x-viewX), YCenter(b) - (y-viewY-1) - 2);
		Point p2 = p1 + Point (ic->frameWidth, 0);
		Point p3 = p1 + Point (ic->frameWidth * item->health / ic->unitHealth, 0);

		tbgraph::Line(b, p1, p3, 1, tbal::COLOR_GREEN, true, true);
		tbgraph::Line(b, p3, p2, 1, tbal::COLOR_RED, true, true);
	}
}

void DrawBullet (Item* item, const tbal::Bitmap& b, int viewX, int viewY)
{
	if (item->itemClass->picturel.sizeX() > 0)
		DrawItemStage (item, b, viewX, viewY, item->stepStage/AL);

	if (item->itemClass->blTraceColor != tbal::COLOR_IMPOSSIBLE)
	{
		Point p1 = Point(XCenter(b), YCenter(b)) + ~(item->pp3 - Point(viewX,viewY));
		Point p2 = Point(XCenter(b), YCenter(b)) + ~(item->pos - Point(viewX,viewY));

		tbgraph::Line(b, p1, p2, 1, item->itemClass->blTraceColor);	
	}
}

struct CopyToNoblack 
{
	void operator () (tbal::Color &dst, tbal::Color src)
	{
		if (dst != tbal::COLOR_BLACK && src != tbal::COLOR_BLACK)
			dst=src;
	}
};

void DrawDoor (Item* item, const tbal::Bitmap& b, int viewX, int viewY)
{
	ItemClass *ic = item->itemClass;
	int x = item->pos.x.Round()-item->itemClass->frameWidth/2;
	int y = item->pos.y.Round()+item->itemClass->frameHeight;

	b.Copy(CopyToNoblack(), ic->picturel,
				XCenter(b) + (x - viewX), 
				YCenter(b) - (y - viewY - 1));
}

void DrawExit (Item* item, const tbal::Bitmap& b, int viewX, int viewY)
{
	ItemClass *ic = item->itemClass;
	int x = item->pos.x.Round()-item->itemClass->frameWidth/2;
	int y = item->pos.y.Round()+item->itemClass->frameHeight*2/3;

	b.Copy(CopyToNoblack(), ic->picturel,
				XCenter(b) + (x - viewX), 
				YCenter(b) - (y - viewY - 1));
}