#pragma once
#include "scale.h"
#include "funcptr.h"
#include "sounds.h"

enum ItemType
{
	IT_EXIT,
	IT_DOOR,
	IT_KEY,
	IT_WEAPON,
	IT_BULLET,
	IT_ARMOR,
	IT_HEALTH,
	IT_GIBS, 
	IT_UNIT
};

struct Item;

struct ItemClass
{
	// разновидность
	int index;
	ItemType type;	
	// то, что необходимо каждому предмету
	bool needGround;
	int width, height, frameWidth, frameHeight;
	tbal::Picture picturer, picturel;
	tblib::fptr<void(Item*, const tbal::Bitmap&, int, int)> draw;
	tblib::fptr<void(Item*)> move;
	// для юнитов
	Fixed unitSpeed;
	Fixed unitSpring;
	bool unitFly;
	bool unitPlayer;
	int unitHealth;
	ItemClass* unitChildClass;
	ItemClass* unitDefaultweapon;
	bool unitBorner;
	tblib::fptr<void(Item*)> unitAI;
	// для оружия
	Fixed wpBulletSpeed;
	int wpReloadTime;
	Fixed wpDispersion;
	int wpBulletsInShot;
	int wpArmorsInShot;
	int wpArmorIndex;
	int wpIndex;
	int wpInitialArmor;
	bool wpHasEmptyImage;
	MidiSound* wpSound;
	ItemClass* wpBulletClass;
	// для пуль
	int blDamage;
	int blDistance;
	bool blFly;
	tbal::Color blTraceColor;
	ItemClass* blWeaponClass;
	// для патронов
	int armIndex;
	int armCount;
	// для аптечек
	int hlCount;
	// для ключей
	int keyCode;
	tblib::stringref itemMessage;
	
	void LoadPictures (const char* filename, bool wpHasEmptyImage = false)
	{
		picturer.Load((std::string("sprites/")+filename).c_str());
		tblib::recreate(picturel, picturer.sizeX(), picturer.sizeY());
		ReflectBitmap(picturer, picturel);

		switch (type)
		{
		case IT_UNIT   : frameWidth = picturel.sizeX(); frameHeight = picturel.sizeY()/8;  break;
		case IT_DOOR :
		case IT_KEY :
		case IT_EXIT :
		case IT_HEALTH : 
		case IT_ARMOR  : frameWidth = picturel.sizeX(); frameHeight = picturel.sizeY();    break;
		case IT_WEAPON : 
			this->wpHasEmptyImage = wpHasEmptyImage;
			frameWidth = picturel.sizeX(); frameHeight = picturel.sizeY()/(wpHasEmptyImage ? 5 : 4);  break;
		default        : frameWidth = picturel.sizeX(); frameHeight = picturel.sizeY()/4;  break;
		}
	}

	ItemClass () 
	{
		frameHeight = 0;
		frameWidth = 0;
		width = 0;
		height = 0;
		unitChildClass = NULL;
		wpSound = 0;
	}

};


void AIPlayer (Item* item);
void AIStupidGroundUnit (Item* item); 
void AICleverGroundUnit (Item* item); 
void AIJumpingUnit (Item* item);
void AIFlyingUnit (Item* item);

void DrawItem (Item* item, const tbal::Bitmap& b, int viewX, int viewY);
void DrawUnit (Item* item, const tbal::Bitmap& b, int viewX, int viewY);
void DrawBullet (Item* item, const tbal::Bitmap& b, int viewX, int viewY);
void DrawDoor (Item* item, const tbal::Bitmap& b, int viewX, int viewY);
void DrawExit (Item* item, const tbal::Bitmap& b, int viewX, int viewY);

void MoveItem (Item* item);
void MoveUnit (Item* item);
void MoveWeapon (Item* item);
void MoveBullet (Item* item);
void MoveGibs (Item* item);
void MoveDoor (Item* item);
void MoveBorner (Item* item);

tblib::array<ItemClass, 128> itemClasses; // думаю, хватит 

ItemClass& nextRef ()
{
	itemClasses.emplace_back();
	itemClasses.back().index = itemClasses.size()-1;
	return itemClasses.back();
}

class ItemClassRef
{
	ItemClass& ref;
	ItemClassRef (const ItemClassRef&);
	ItemClassRef& operator= (const ItemClassRef&);
public :
	ItemClassRef () : ref (nextRef ()) {}
	operator ItemClass& () { return ref; }
	ItemClass* operator & () { return &ref; }
};


#include "icweapons.h"
#include "icmonsterweapons.h"
#include "icmonsters.h"
#include "icother.h"

void InitClasses ()
{
	InitWeapons ();
	InitMonsters ();
	InitMonsterWeapons ();
	InitOther ();
}