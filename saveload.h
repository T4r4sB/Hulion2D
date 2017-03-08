#pragma once
#include <fstream>
#include "enginefuncs.h"
#include "inititems.h"

template <typename T> 
void Write (std::ofstream &f, const T& t)
{
	f.write(reinterpret_cast<const char*>(&t), sizeof(t));
}

template <typename T> 
void Read (std::ifstream &f, T& t)
{
	f.read(reinterpret_cast<char*>(&t), sizeof(t));
}

const int cur_ver = 0;

void WriteBloodInfo (std::ofstream& f)
{
	for (int j=0; j<MMY; ++j)
	{
		Write(f, j);
		bool prevb=false;
		for (int i=0; i<=MMX; ++i)
		{
			bool curb = i<MMX && (level.map[j][i]==COLOR_BLOOD_1 || level.map[j][i]==COLOR_BLOOD_2);

			if (curb^prevb)
				Write(f, i);
			prevb=curb;
		}
		Write(f, -1);
	}
}

void ReadBloodInfo (std::ifstream& f)
{
	FxRandom localrnd (tbal::MSec());

	for (int j=0; j<MMY; ++j)
	{
		int y;
		Read(f, y);
		assert(y==j);

		int x1=-1, x2=-1;
		for (;;)
		{
			Read(f, x1);
			if (x1<=x2 || x1>=MMX) break;
			Read(f, x2);
			if (x2>MMX) break;

			for (int i=x1; i<x2; ++i)
				level.map[j][i] = localrnd.random(2)==0 ? COLOR_BLOOD_1 : COLOR_BLOOD_2;
		}
	}
}

void NewGame ()
{
	tblib::recreate(activeItems);
	tblib::recreate(level);
	tblib::recreate(items);
	tblib::recreate(itemCells);		

	level.Init(nextLevelSeed);

	/*
	tbal::Picture top(1024,600);
	level.t.Draw(top);
	top.Save("topology.bmp");

	tbal::Picture megamap(MMX, MMY);
	megamap.Copy(TableCopy(), level.map, 0, 0);
	megamap.Save("megamap.bmp");/**/

	InitItems ();
	Phys ();
}

bool SaveLevel (const char* filename)
{
	std::ofstream f( (std::string(tbal::GetExternalFilesDir())+filename).c_str(), std::ios::binary);
	if (f.is_open())
	{
		// мне похуй, что на 64 битах этот код будет вести себ€ иначе
		// или что сохранЄнки дебага и релиза несовместимы
		// не буду € за три дн€ ради трэш-проекта пилить нормальную сериализацию
		// перечисл€ть каждый компонент, ага, лол
		Write(f, cur_ver);
		Write(f, level.seed);
		Write(f, rnd.randseed);
		Write(f, isSounds);
		Write(f, volume);
		//
		Write(f, viewX);
		Write(f, viewY);

		Write(f, sizeof(Item));
		Write(f, &itemClasses);
		Write(f, &items);

		Write(f, items); // тупо, но без полноценных операторов сериализации только так
		Write(f, wasIn);
		WriteBloodInfo (f);

		f.flush();
		return ((f.rdstate() & std::ios::badbit) ==0);
	}
	return false;
}

class AdjustItem
{
	ItemClass* rdFirstClass;
	Item* rdFirstItem;

public :
	AdjustItem(ItemClass* rdFirstClass, Item* rdFirstItem)
		: rdFirstClass(rdFirstClass), rdFirstItem(rdFirstItem) {}

	void AdjustPtr (ItemClass* &ptr)
	{
		if (ptr)
			ptr = &itemClasses [int(ptr-rdFirstClass)];
	}

	void AdjustPtr (Item* &ptr)
	{
		if (ptr)
			ptr = &items [int(ptr-rdFirstItem)];
	}

	void operator () (Item& item)
	{
		new (&item) IL::xclass(); // тупо игнорируем мусорные данные про хэш после чтени€ из файла

		AdjustPtr (item.itemClass);

		int& tc = Item::TypeCount(item.itemClass);
		--tc;
		assert(tc>=0);

		item.PutToCells();

		for (int i=0; i<item.weapons.size(); ++i)
			AdjustPtr (item.weapons[i].weaponClass);

		AdjustPtr (item.bornHost);
		for (int i=0; i<item.childs.size(); ++i) if (item.childs.valid(i))
			AdjustPtr(item.childs[i]);

		if (item.active)
		{
			item.active = false;
			activeItems.emplace_back(&item);
			// тупо втыкаем в массив, не дела€ лишних движений
		}
	}
};

bool LoadLevel (const char* filename)
{
	std::ifstream f((std::string(tbal::GetExternalFilesDir())+filename).c_str(), std::ios::binary);
	if (f.is_open())
	{
		// чистка пулов
		tblib::recreate(level);
		tblib::recreate(items);
		tblib::recreate(itemCells);		
		cntUnits   = MAX_UNITS;
		cntLoots   = MAX_LOOTS;
		cntGibs    = MAX_GIBS;
		cntBullets = MAX_BULLETS;

		uint32_t ver;
		Read(f, ver);

		Read(f, level.seed);
		nextLevelSeed = level.seed;
		level.Init(nextLevelSeed);
		Read(f, rnd.randseed);

		Read(f, isSounds);
		Read(f, volume);
		Read(f, viewX);
		Read(f, viewY);

		int itemSize;
		Item* rdFirstItem;
		ItemClass* rdFirstClass;

		Read(f, itemSize);
		Read(f, rdFirstClass);
		Read(f, rdFirstItem);
		Read(f, items); // конструкторы не выполн€ютс€
		Read(f, wasIn);
		ReadBloodInfo(f);

		AdjustItem adjustItem(rdFirstClass, rdFirstItem);

		activeItems.shrink(0);

		for (int i=0; i<items.size(); ++i) if (items.valid(i))
			adjustItem(items[i]);
		player = &items[0];

		return true;
	}
	return false;
}
