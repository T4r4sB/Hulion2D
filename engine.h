#pragma once
#include "itemclasses.h"
#include "menu.h"

const int DT=16;
const int AL=128/DT;
const Fixed G=390;
const Fixed MIN_V_DELTA=76; // вывелось экспериментально от G и DT

Level level;
Fixed viewX, viewY;


const int 
	MAX_UNITS   = SIZEX*SIZEY*3, 
	MAX_LOOTS   = SIZEX*SIZEY*1, 
	MAX_GIBS    = 256, 
	MAX_BULLETS = 256, 
	MAX_ITEMS   = MAX_UNITS+MAX_LOOTS+MAX_GIBS+MAX_BULLETS;

int cntUnits=MAX_UNITS, cntLoots=MAX_LOOTS, cntGibs=MAX_GIBS, cntBullets=MAX_BULLETS;

typedef tblib::list2d<MAX_ITEMS*4> IL;
tblib::carray<tblib::carray<IL::yclass, HMMX>, HMMY> itemCells;

IL itemList;
Item* player;
bool win=false;

#include "messages.h"

int XCenter (const tbal::Bitmap& b)
{
	return b.sizeX()/2;
}

int YCenter (const tbal::Bitmap& b)
{
	return b.sizeY()*2/3;
}

struct WeaponState
{
	ItemClass* weaponClass;
	bool has;
	int timeToShot;

	WeaponState () : weaponClass(NULL), has(false), timeToShot(0) {}
};

struct ArmorState
{
	int bulletCount;

	ArmorState () : bulletCount(0) {}
};

struct Controls
{
	bool up,down,left,right;
	bool shot;
	int wpnumber;

	Controls () : up(false), down(false), left(false), right(false), shot(false), wpnumber(-1) {}
};

const int DS_LEFT = -1;
const int DS_RIGHT = -2;
const int DS_KEY = 0;

struct Item : IL::xclass
{
	int index;
	bool active;
	bool animating;
	bool invalid;
	int inProcess;
	ItemClass* itemClass;  // !!!
	int wpnumber;
	tblib::carray<WeaponState, 8> weapons; // !!!
	tblib::carray<ArmorState, 4> armors;
	Controls controls;
	Point pp1, pp2, pp3, pos, delta;
	Fixed distancePassed;
	bool wantDown, idle;
	bool reflected;
	int health;
	tblib::carray<bool, MAX_DOORS> hasKey;
	int stepStage, wpStepStage; // стадии анимации
 	bool onGround, onStairs;
	int idleTime; // для деактивации
	int deathTime; // для смерти
	bool hostPlayer, hostMonster; // для пуль и оружия

	int doorStage; // для дверей

	Item* bornHost; // для рожалог // !!!
	int indexInBornHost;
	int bornStage;
	tblib::pool<Item*, 16> childs; // !!!

	uint8_t GetWall (int x, int y);

	bool OnGround ()
	{
		return onGround;
	}

	bool OnStairs ()
	{
		return onStairs;
	}

	void AI ()
	{
		if (itemClass->type == IT_UNIT)
			itemClass->unitAI(this);
	}

	void UseControls ()
	{
		if (deathTime==0)
		{
			if (controls.up)
			{
				if ((OnStairs() || itemClass->unitFly) && (itemClass->unitSpeed > fx0))	
					delta.y = std::max(MIN_V_DELTA, itemClass->unitSpeed) * Fixed(DT,1000);
				else if (OnGround())
					delta.y = itemClass->unitSpring * Fixed(DT,1000);
			}

			if (controls.right) 
			{
				delta.x += itemClass->unitSpeed * Fixed(DT,1000);
				reflected=false;
			}

			if (controls.left)  
			{
				delta.x -= itemClass->unitSpeed * Fixed(DT,1000);
				reflected=true;
			}

			if (controls.down)
			{
				wantDown = true;
				if (OnGround() || itemClass->unitFly)
					delta.y = -itemClass->unitSpeed * Fixed(DT,1000);
			} else
				wantDown = false;
			
			if (controls.wpnumber>=0 && controls.wpnumber<weapons.size() && controls.wpnumber != wpnumber)
			{
				WeaponState &ws = weapons[controls.wpnumber];
				if (ws.has 
					&& (ws.weaponClass->wpArmorIndex<0 
					|| armors[ws.weaponClass->wpArmorIndex].bulletCount>=ws.weaponClass->wpArmorsInShot))
				{
					wpnumber = controls.wpnumber;
					tblib::enlarge(ws.timeToShot, 500); // переключение типа вот столько занимает
				}
			}
		} else
		{
			controls.left = false;
			controls.right = false;
			controls.up = false;
			controls.down = false;
			controls.shot = false;
		}
	}

	void PutToCells ()
	{
		int x = pos.x.Round();
		int y = pos.y.Round();
		int w = itemClass->width;
		int h = itemClass->height;

		const int x1 = std::max(0, (x-w/2)/HCSIZE);
		const int y1 = std::max(0, (y+0)/HCSIZE);
		const int x2 = std::min(HMMX-1, (x+w/2)/HCSIZE);
		const int y2 = std::min(HMMY-1, (y+h)/HCSIZE);

		for (int j=y1; j<=y2; ++j) for (int i=x1; i<=x2; ++i)
			itemList.put_x_to_y (this, &itemCells[j][i]);
	}

	void DeleteFromCells ()
	{
		itemList.del_x_from_list (this);
	}

	void RegroupCells ()
	{
		DeleteFromCells ();
		PutToCells ();
	}

	void GenerateBullets (ItemClass* wpClass);
	void TryShot ();
	void CheckItems ();

	bool Valid ();

	void Move ()
	{		
		assert (Valid());

		if (animating)
			stepStage = (stepStage+1)%(4*AL);		
		else
		{
			AI();		
			UseControls();

			itemClass->move(this);

			if (itemClass->unitPlayer)
				CheckItems();		

			if (deathTime>0)
			{
				deathTime += DT;
				if (deathTime > 10000)
					invalid = true;
			}

			if (controls.shot)
				TryShot ();

			for (int i=0; i<weapons.size(); ++i)
			{
				weapons[i].timeToShot -= DT;
				tblib::enlarge(weapons[i].timeToShot, 0);
			}
			
			RegroupCells ();
		}
		idleTime += DT;
	}	

	bool GetWeapon (ItemClass* wpClass)
	{
		assert (wpClass);
		assert (wpClass->type == IT_WEAPON);
	
		WeaponState& ws = weapons[wpClass->wpIndex];
		bool nohas = !ws.has;
		if (nohas || ws.weaponClass == wpClass)
		{
			if (wpClass->wpArmorIndex>=0)
				armors[wpClass->wpArmorIndex].bulletCount += wpClass->wpInitialArmor;
			ws.has = true;
			ws.weaponClass = wpClass;
			if (nohas && wpnumber<wpClass->wpIndex)
				wpnumber = wpClass->wpIndex;
			return true;
		}	
		return false;
	}

	static int& TypeCount (ItemClass* itemClass)
	{
		switch (itemClass->type)
		{
		case IT_UNIT : case IT_DOOR : return cntUnits;
		case IT_KEY : case IT_WEAPON : case IT_ARMOR: case IT_HEALTH : case IT_EXIT : return cntLoots;
		case IT_BULLET : return cntBullets;
		case IT_GIBS : return cntGibs;
		}		
		assert(false);
		return *(int*)(NULL);
	}

	Item (ItemClass* itemClass, int x, int y)
	{
		this->itemClass = itemClass;

		--TypeCount(itemClass);
		
		invalid = false;
		animating = false;
		onGround = false;
		wantDown = false;
		idle = false;
		reflected = false;
		pos = Point (x,y);
		pp1 = pos;
		pp2 = pos;
		pp3 = pos;
		delta = Point(0,0);    
		active = false;
		idleTime = 0;
		deathTime = 0;
		inProcess = 0;
		stepStage = 0;
		wpStepStage = AL;
		distancePassed = fx0;
		health = itemClass->unitHealth;

		bornHost = NULL;
		bornStage = 0;
		wpnumber = -1;
	
		if (itemClass->needGround)
			while (!OnGround())
			  Move();
		RegroupCells();

		for (int i=0; i<MAX_DOORS; ++i)
			hasKey[i] = false;

		if (itemClass->unitDefaultweapon)
			GetWeapon(itemClass->unitDefaultweapon);
	}

	~Item ()
	{		
		assert (!active);
		++TypeCount(itemClass);
		for (int i=0; i<childs.size(); ++i) if (childs.valid(i))
			childs[i]->bornHost = NULL;
	}
};

tblib::pool<Item, MAX_ITEMS> items;

bool Item::Valid ()
{
	return items.valid(index) 
		&& (bornHost==NULL || bornHost->childs[indexInBornHost]==this);
}

bool ItemInActiveArray (Item* it);

class ActiveKeeper
{
	Item* item;
	ActiveKeeper (const ActiveKeeper&);
	ActiveKeeper& operator = (const ActiveKeeper&);	
public :
	Item& operator * () { return *item; }
	Item* operator -> () { return item; }
	operator Item* () { return item; }
	
	const Item& operator * () const { return *item; }
	const Item* operator -> () const { return item; }
	operator const Item* () const { return item; }

	void Clear () { item=NULL; }

	ActiveKeeper (Item* item) : item(item) 
	{ 
		assert (!item->active); 
		assert (!ItemInActiveArray(item)); 
		assert (item->Valid()); 
		item->active = true; 
	}

	~ActiveKeeper ()		
	{ 
		if (item) 
		{ 
			assert (item->active); 
			item->active = false; 
			item->wpStepStage=AL; 
		} 
	}

	void Swap (ActiveKeeper &other) { std::swap(item, other.item); }
};

namespace std { void swap (ActiveKeeper& l, ActiveKeeper& r) { l.Swap(r); }};


tblib::array<ActiveKeeper, MAX_ITEMS> activeItems;


bool ItemInActiveArray (Item* it)
{
	for (int i=0; i<activeItems.size(); ++i)
		if (activeItems[i]==it)
			return true;
	return false;
}

class CheckPtr
{
	Item* item;
	CheckPtr (const CheckPtr&);
	CheckPtr& operator = (const CheckPtr&);	
public :
	Item& operator * () { return *item; }
	Item* operator -> () { return item; }
	operator Item* () { return item; }
	
	const Item& operator * () const { return *item; }
	const Item* operator -> () const { return item; }
	operator const Item* () const { return item; }

	CheckPtr (Item* item) : item(item) { ++item->inProcess; }
	~CheckPtr () { --item->inProcess; }
};

struct ItemTrue {	
	bool operator () (Item* it) {		(void) it;	return true;	} 
};

struct ItemIsDoor {	
	bool operator () (Item* it) {	return it->itemClass->type == IT_DOOR;	} 
};

struct ACheckPtr : tblib::array<Item*, MAX_ITEMS> 
{
	template <typename T>
	ACheckPtr (T f, int x1, int y1, int x2, int y2)
	{
		const int 
			hx1 = std::max(x1/HCSIZE, 0),
			hy1 = std::max(y1/HCSIZE, 0),
			hx2 = std::min(x2/HCSIZE, HMMX-1),
			hy2 = std::min(y2/HCSIZE, HMMY-1);

		for (int j=hy1; j<=hy2; ++j) for (int i=hx1; i<=hx2; ++i)
		{
			for (IL::xynode *node = itemCells[j][i].firstx; node; node=node->nextx)
			{
				Item* it = static_cast <Item*> (node->x);
				assert (it->Valid());
				if (!it->inProcess && f(it))
				{
					it->inProcess = true;
					emplace_back(it);
				}
			}
		}

		// я опять ниибу как предусмотреть исключение
		for (int i=0; i<size(); ++i)
			(*this)[i]->inProcess = false;
		// в этой редакции процесс делается только при конструировании массива
	}
};

bool TestDoor(int x, int y, Item* it)
{
	return (it->itemClass->type == IT_DOOR 
		&& abs(it->pos.x.Round()-x)<3
		&& (y>=it->pos.y.Round())
		&& (y<=it->pos.y.Round()+DOOR_HEIGHT));
}

uint8_t GetColor(int x, int y, ACheckPtr& ptr)
{		
	for (int i=0; i<ptr.size(); ++i)
		if (TestDoor(x, y, ptr[i]))
			return COLOR_WALL;
	return level.map[MMY-y][x];
}

uint8_t Item::GetWall (int x, int y)
{
	int w2 = itemClass->width/2;

	int x1=x-w2, x2=x+w2, y1=y, y2=y+itemClass->height;

	ACheckPtr doors(ItemIsDoor(), x1, y1, x2, y2);

	uint8_t res = 255;

	for (int i=x1; i<=x2; ++i)
	{
		tblib::shrink(res, GetColor(i, y2, doors));
	}

	for (int j=y2-1; j>=y1+1; --j)
	{
		tblib::shrink(res, GetColor(x1, j, doors));
		tblib::shrink(res, GetColor(x2, j, doors));
	}

	if (res > COLOR_WALL)
		res = COLOR_EMPTY; // башкой за лестницы цепляться не умеем :/
	
	for (int i=x1; i<=x2; ++i)
	{
		tblib::shrink(res, GetColor(i, y1, doors));
	}

	return res;
}


bool ItemVisible (Item* it1, Item* it2);

void ActivateItem (Item* item, bool animating=false)
{
	if (!item->active)
		activeItems.emplace_back(item);
	item->animating = animating;
	item->idleTime = 0;
}

int NewItem (ItemClass* itemClass, int x, int y)
{
	if (Item::TypeCount(itemClass)>0)
	{
		int result = items.emplace(itemClass, x, y);
		items[result].index = result;
		return result;
	} else
	{
		return -1;
	}
}

void Item::GenerateBullets (ItemClass* wpClass)
{		
	if (wpClass->wpSound)
		wpClass->wpSound->Play();

	for (int i=0; i<wpClass->wpBulletsInShot; ++i)
	{
		int index = NewItem(wpClass->wpBulletClass, 
			pos.x.Round()+itemClass->width/2*(reflected?1:-1), pos.y.Round()+itemClass->height/2);

		if (index>=0)
		{
			Item& it = items[index];
			it.hostPlayer = itemClass->type==IT_UNIT && itemClass->unitPlayer;
			it.hostMonster = itemClass->type==IT_UNIT && !itemClass->unitPlayer;

			it.delta = SinCos(rnd.frandom()*wpClass->wpDispersion*fxHalf);
			if (rnd.random(2)==0)
				it.delta.y = -it.delta.y;
			if (reflected)
				it.delta.x = -it.delta.x;

			//if (controls.down)
			//	it.delta = it.delta & Point(1,reflected?1:-1);

			if (!wpClass->wpBulletClass->blFly)
			{
				Fixed sp = Fixed(200) / wpClass->wpBulletSpeed;
				it.delta = it.delta & Point(1,reflected?-sp:sp);
			}

			it.delta = it.delta.Normalize()*wpClass->wpBulletSpeed*Fixed(DT,1000);
			ActivateItem(&it);
		}    
	}
}

void Item::TryShot ()
{
	bool ok;
	if (wpnumber>=0)
	{
		WeaponState& ws = weapons[wpnumber];
		ItemClass* wpClass = ws.weaponClass;
		if (ws.has && wpClass)
		{				
			if (ws.timeToShot<=0)
			{
				if (wpClass->wpArmorIndex<0 || armors[wpClass->wpArmorIndex].bulletCount>=wpClass->wpArmorsInShot)
				{
					if (wpClass->wpArmorIndex>=0)
						armors[wpClass->wpArmorIndex].bulletCount -= wpClass->wpArmorsInShot;
					wpStepStage = AL*2;
					ws.timeToShot = wpClass->wpReloadTime;
					GenerateBullets(wpClass);
					ok=true; // успешный выстрел
				} else ok=false;
			} else ok=true; // оружие есть, но не время стрелять
		} else ok=false;
	} else ok=false;

	if (ok)	return;

	for (int i=weapons.size()-1; i>=0; --i)
	{			
		WeaponState& ws = weapons[i];
		ItemClass* wpClass = ws.weaponClass;
		if (ws.has && wpClass
			&& (wpClass->wpArmorIndex<0 || armors[wpClass->wpArmorIndex].bulletCount>=wpClass->wpArmorsInShot))
		{
			controls.wpnumber = i;
			break;
		}
	}
}

void OpenDoor(Item* item)
{
	if (item->itemClass->type == IT_DOOR)
	{
		item->stepStage = 1;
		ActivateItem(item);
	}
}

tblib::stringref __doorMessages[MAX_DOORS] =
{
	"Тебе нужен ржавый ключ",
	"Тебе нужен медный ключ",
	"Тебе нужен серебрянный ключ",
	"Тебе нужен золотой ключ",
	"Тебе нужен трёхгранный ключ" 
};

tblib::carray<tblib::stringref, MAX_DOORS> doorMessages (__doorMessages);

void Item::CheckItems ()
{

	ACheckPtr ci(ItemTrue(),
		pos.x.Round() - itemClass->width/2, pos.y.Round(),
		pos.x.Round() + itemClass->width/2, pos.y.Round() + itemClass->height);

	for (int i=0; i<ci.size(); ++i)
	{
		Item* it = ci[i];

		if (it->pos.x - Fixed(it->itemClass->width,2) <= pos.x + Fixed(itemClass->width,2)
			&& it->pos.x + Fixed(it->itemClass->width,2) >= pos.x - Fixed(itemClass->width,2)
			&& it->pos.y <= pos.y + Fixed(itemClass->height)
			&& it->pos.y + Fixed(it->itemClass->height) >= pos.y)
		{
			switch (it->itemClass->type)
			{
			case IT_HEALTH : 
				if (health < itemClass->unitHealth)
				{
					soundHeal.Play();
					health = std::min(health+it->itemClass->hlCount, itemClass->unitHealth);
					AddMessage(it->itemClass->itemMessage, true);
					it->invalid = true;
				}
				break;
			case IT_WEAPON :
				if (GetWeapon (it->itemClass))
				{
					soundArm.Play();
					AddMessage(it->itemClass->itemMessage, true);
					it->invalid = true;
				}
				break;
			case IT_ARMOR :
				soundArm.Play();
				armors[it->itemClass->armIndex].bulletCount += it->itemClass->armCount;
				AddMessage(it->itemClass->itemMessage, true);						
				it->invalid = true;
				break;
			case IT_KEY :
				soundKey.Play();
				hasKey[it->itemClass->keyCode] = true;
				AddMessage(it->itemClass->itemMessage, true);
				it->invalid = true;
				break;
			case IT_EXIT :
				if (!win)
				{
					soundWin.Play();
					AddMessage("Ты нашёл выход!!! Ты победил.");
					AddMessage("Но можешь ещё побегать по лабиринту");
					win=true;
				}
				break;
			case IT_DOOR :
				switch (it->doorStage)
				{
				case DS_RIGHT : 
					if (pos.x > it->pos.x)	   
						OpenDoor(it); 
					else 
						AddMessage ("Эта дверь открывается с другой стороны");
					break;
				case DS_LEFT :  
					if (pos.x < it->pos.x)	   
						OpenDoor(it);	
					else 
						AddMessage ("Эта дверь открывается с другой стороны");
					break;
				default :   		
					if (hasKey[it->doorStage]) 
						OpenDoor(it);
					else 
						AddMessage (doorMessages[it->doorStage]); 
					break;
				}
				break;
			}
		}
	}	
}

bool Visible (Point pos1, Point pos2)
{
	Point d = pos2-pos1;
	int l = int(std::max(d.x.Abs(), d.y.Abs())) + 1;

	ACheckPtr doors (ItemIsDoor(),
		std::min(pos1.x, pos2.x).Round(), std::min(pos1.y, pos2.y).Round(),
		std::max(pos1.x, pos2.x).Round(), std::max(pos1.y, pos2.y).Round());

	d /= l;
	for (int i=0; i<l; ++i)
	{
		if (GetColor(pos1.x.Round(), pos1.y.Round(), doors) <= COLOR_WALL_2)
			return false;
		pos1 += d;
	}
	return true;
}

bool ItemVisible (Item* it1, Item* it2)
{
	return it1==it2 || Visible (
		it1->pos + Point(0, it1->itemClass->height/2), 
		it2->pos + Point(0, it2->itemClass->height/2));
}

void ActivateVisibleItems ()
{
	ACheckPtr pr (ItemTrue(),
		player->pos.x.Round()-200, player->pos.y.Round()-70,
		player->pos.x.Round()+200, player->pos.y.Round()+70);

	for (int i=0; i<pr.size(); ++i)
	{
		Item* it = pr[i];	
		if ((it->pos - player->pos).Length() < Fixed(200))
		{
			bool ok = false;
			// для неактивной проверяем честно, 
			// для активной только если она долго не проверялась
			if (!it->active || it->animating)
				ok = ItemVisible(it, player);
			else if (it->idleTime > 5000)
			{
        if (ItemVisible(it, player))
					ok = true;
				else 
					it->idleTime += 1000;
			}
			
			if (ok)
				ActivateItem(it);
		}
	}
}

int t=0;

void MoveActiveItems ()
{
	for (int i=0; i<activeItems.size(); ++i)
	{
		if (t==37 && i==22)
		{
			++t;
			--t;
		}
		activeItems[i]->Move();
	}
}

bool ActiveItemsValid ()
{
	for (int i=0; i<activeItems.size(); ++i)
	{
		if (!activeItems[i]->Valid())
			return false;
		if (!activeItems[i]->active)
			return false;
		for (int j=0; j<i; ++j)
			if (static_cast<Item*>(activeItems[i])==static_cast<Item*>(activeItems[j]))
				return false;
	}
	return true;
}

void CheckActiveItems ()
{
	int j=0;
	for (int i=0; i<activeItems.size(); ++i)
	{
		if (activeItems[i]->invalid
			&& (activeItems[i]->itemClass->type != IT_UNIT || !activeItems[i]->itemClass->unitPlayer))
		{
			activeItems[i]->active=false;
			items.free_index(activeItems[i]->index);
			activeItems[i].Clear();
		}	else if (activeItems[i]->idleTime > 6000 
			&& !(activeItems[i]->itemClass->needGround && !activeItems[i]->OnGround() ))
			; // она протухла, пропускаем её!
		else
			std::swap(activeItems[i], activeItems[j++]);
	}
	activeItems.shrink(j);
}

void BornUnit (ItemClass* bornClass, Item* host)
{
	int index = NewItem(bornClass, host->pos.x.Round(), host->pos.y.Round());
	if (index>=0)
	{
		Item& it = items[index];
		it.delta = Point(0,200) * Fixed(DT,1000);
		it.bornHost = host;
		it.indexInBornHost = host->childs.emplace(&it);
		ActivateItem(&it);
	}
}

void CheckDeadUnits ()
{	
	for (int i=0; i<activeItems.size(); ++i)
	{
		if (activeItems[i]->itemClass->type == IT_UNIT
			&& activeItems[i]->health <= 0
			&& activeItems[i]->deathTime == 0)
		{
			if (activeItems[i]->itemClass->unitPlayer)
				AddMessage("Тебя убили, начинай игру заново");

			if (activeItems[i]->bornHost)
			{
				activeItems[i]->bornHost->childs.free_index(activeItems[i]->indexInBornHost);
				activeItems[i]->bornHost = NULL;
			}

			if (!activeItems[i]->invalid)
			{
				soundHit.Play();
				activeItems[i]->deathTime = 1;
				activeItems[i]->stepStage = AL*4;
			}
		}
	}
}

void CreateGibs (Item* it)
{
	int cnt = it->itemClass->unitHealth/5;
	for (int i=0; i<cnt; ++i)
	{
		int index = NewItem(&icGibs, it->pos.x.Round(), it->pos.y.Round());
		if (index>=0)
		{
			items[index].delta = SinCos(rnd.frandom()*fxPi) * Fixed(100+rnd.random(200)) * Fixed(DT,1000);
			items[index].stepStage = rnd.random(4)*AL;
			ActivateItem(&items[index]);
		}
	}
}

void DamageItem (Item* it, int damage)
{
	if (it->itemClass->unitPlayer)
		damage = (damage+4)/3;
	else
		damage = damage*3/2;

	it->health -= damage;

	if (it->health >= -it->itemClass->unitHealth*5/2)	
	{
		ActivateItem(it);
	} else 
	{
		if (!it->itemClass->unitPlayer)
			AddMessage("Ты порвал его на куски!!!");
		soundGibs.Play();
		CreateGibs(it);

		// а вот этого ифа не было в конкурсной версии, из-за этого был ебучий баг
		// потому что инвалидномуитему не запускается процесс смерти, 
		// он просто удаляется нафиг
		// но итем игрока удалять нельзя, так он и остаётся вечно живой
		if (!it->itemClass->unitPlayer)
		// в конкурсной версии была просто инвалидация, без ифа
			it->invalid = true;
	}
}

tblib::carray<tblib::carray<bool, SIZEX>, SIZEY> wasIn;

void MarkPlayerCell ()
{
	int px = player->pos.x.Round(), py = player->pos.y.Round();

	int cx = px/CELL_WIDTH;
	int cy = 0;
	for (int j=0; j<level.t.cells.size(); ++j)
	{
		if (level.t.cells[j][cx].was && py+UNIT_HEIGHT/2>=level.t.heights[j][px].lo)
		{
			cy = j;
			break;
		}
	}

	wasIn[cy][cx] = true;
}

void Phys ()
{
	++t;	

	ActivateVisibleItems ();	
	MoveActiveItems ();		
	CheckDeadUnits ();	
	CheckActiveItems ();
	ProcessMessages ();
	MarkPlayerCell ();

	tblib::inbound(viewX, player->pos.x-Fixed(50), player->pos.x+Fixed(50));
	tblib::inbound(viewY, player->pos.y-Fixed(30), player->pos.y+Fixed(30));
	if (viewX>player->pos.x) viewX-=Fixed(DT,16);
	if (viewX<player->pos.x) viewX+=Fixed(DT,16);
	if (viewY>player->pos.y) viewY-=Fixed(DT,16);
	if (viewY<player->pos.y) viewY+=Fixed(DT,16);

	viewX = int(viewX.Round());
	viewY = int(viewY.Round());
}

#include "hud.h"
#include "minimap.h"

class ItemPtr
{
	Item* item;
public :
	Item& operator * () { return *item; }
	Item* operator -> () { return item; }
	operator Item* () { return item; }
	
	const Item& operator * () const { return *item; }
	const Item* operator -> () const { return item; }
	operator const Item* () const { return item; }

	ItemPtr (Item* item) : item(item) {}
};

bool operator < (const ItemPtr &l, const ItemPtr &r)
{
	return l->itemClass->type < r->itemClass->type;
}

void Draw (const tbal::Bitmap& b)
{
	int vx = viewX.Round();
	int vy = viewY.Round();

	tblib::inbound(vx, XCenter(b), MMX-(b.sizeX()-XCenter(b)));
	tblib::inbound(vy, b.sizeY()-YCenter(b), MMY-YCenter(b));

	level.DrawToBuf(b, vx-XCenter(b), MMY-vy-YCenter(b));
		
	ACheckPtr drawed (ItemTrue(),
		vx-XCenter(b), vy+YCenter(b)-b.sizeY(),
		vx-XCenter(b)+b.sizeX(), vy+YCenter(b));

	for (int i=0; i<drawed.size(); ++i)
	{
		Item* item = drawed[i];
		// костыль для летунчиков
		if (item->itemClass->type == IT_UNIT 
			&& item->itemClass->unitFly
			&& !item->active
			&& item->deathTime==0)
			ActivateItem(item, true);
	}

	tblib::array<ItemPtr, MAX_ITEMS> sorted; // потому что ёбаный библиотечный свап требует копирование

	for (int i=0; i<drawed.size(); ++i)
		sorted.emplace_back((Item*)(drawed[i]));

	std::sort(sorted.begin(), sorted.end());
	for (int i=0; i<sorted.size(); ++i)
		sorted[i]->itemClass->draw(sorted[i], b, vx, vy);
	
	ShowHud (b);
	ShowMessages (b);

	if (tbal::Pressed(tbal::CODE_TAB))
		ShowMinimap (b);

	if (menu.state != MS_NO)
		DrawMenu (b);

	tbal::DrawAllButtons(b);

}