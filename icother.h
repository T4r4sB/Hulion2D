ItemClassRef 
	icHealth25,
	icGibs,
	icDoor, icExit, icKey1, icKey2, icKey3, icKey4, icKey5;

ItemClass *__icKeys[MAX_DOORS] = { &icKey1, &icKey2, &icKey3, &icKey4, &icKey5 };
tblib::carray <ItemClass*, MAX_DOORS> icKeys(__icKeys);

void InitClassDoor ()
{
	ItemClass& ic = icDoor;

	ic.type = IT_DOOR;
	ic.width = DOOR_WIDTH;
	ic.height = DOOR_HEIGHT;
	ic.draw = DrawDoor;
	ic.move = MoveDoor;
	ic.needGround = false;
	ic.LoadPictures("door.bmp");
}

void InitClassHealth25 ()
{
	ItemClass& ic = icHealth25;

	ic.type = IT_HEALTH;
	ic.width = 8;
	ic.height = 8;
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;
	ic.LoadPictures("h25.bmp");

	
	ic.itemMessage = "Ты подобрал аптечку +25";

	ic.hlCount = 25;
}

void InitClassGibs ()
{
	ItemClass& ic = icGibs;

	ic.type = IT_GIBS;
	ic.width = 6;
	ic.height = 6;
	ic.draw = DrawItem;
	ic.move = MoveGibs;
	ic.needGround = false;
	ic.LoadPictures("gibs.bmp");
}

void InitClassExit ()
{
	ItemClass& ic = icExit;

	ic.type = IT_EXIT;
	ic.width = 8;
	ic.height = 32;
	ic.draw = DrawExit;
	ic.move = MoveItem;
	ic.needGround = true;
	ic.LoadPictures("exit.bmp");
}

tblib::stringref __keyMessages[MAX_DOORS] =
{
	"Ты подобрал ржавый ключ",
	"Ты подобрал медный ключ",
	"Ты подобрал серебрянный ключ",
	"Ты подобрал золотой ключ",
	"Ты подобрал трёхгранный ключ" 
};

tblib::carray<tblib::stringref, MAX_DOORS> keyMessages (__keyMessages);

void InitClassKeys ()
{
	for (int i=0; i<MAX_DOORS; ++i)
	{
		ItemClass& ic = *icKeys[i];

		ic.type = IT_KEY;
		ic.width = 8;
		ic.height = 8;
		ic.draw = DrawItem;
		ic.move = MoveItem;
		ic.needGround = true;
		ic.LoadPictures((std::string("key") + tblib::to_string(i+1) + std::string(".bmp")).c_str());
		
		ic.itemMessage = keyMessages[i];
		ic.keyCode = i;
	}
}

void InitOther ()
{
	InitClassHealth25 ();
	InitClassGibs ();
	InitClassDoor ();
	InitClassKeys ();
	InitClassExit ();
}