ItemClassRef 
	icPlayer, 
	icTrilobit, icTrilobit2, icSnowman, icBorner1, icBorner2, icBorner3, icZombie,	
	icButcher, icLizard, icBigSnowman, icButcherZombie,
	icCock, icBigCock, icFlyingCock,
	icFlyingDrone, icFlyingHead,
	icShroom, icBigShroom,
	icFlyingShroom;

ItemClass *__icMonsters[19] = 
{
	&icTrilobit, &icSnowman, &icTrilobit2, &icCock, &icShroom, &icFlyingCock, &icFlyingShroom, &icBigCock,
	&icBorner1, &icBigSnowman, &icBigShroom, &icBorner2, &icButcher, &icZombie,	&icBorner3, 
	&icFlyingHead, &icLizard, &icFlyingDrone, &icButcherZombie		
};

tblib::carray<ItemClass*, 19> icMonsters(__icMonsters);


void InitClassPlayer ()
{
	ItemClass& ic = icPlayer;

	ic.type = IT_UNIT;
	ic.width = 8;
	ic.height = 32;
	ic.LoadPictures("gg.bmp");
	ic.draw = DrawUnit;
	ic.move = MoveUnit;
	ic.needGround = true;

	ic.unitSpeed  = Fixed(95);
	ic.unitSpring = Fixed(160);
	ic.unitFly = false;
	ic.unitPlayer = true;
	ic.unitHealth = 100;
	ic.unitDefaultweapon = &icSabre;
	ic.unitAI = AIPlayer;
	ic.unitBorner = false;
}

void InitClassTrilobit ()
{	
	for (int i=0; i<2; ++i)
	{

		ItemClass& ic = i==0 ? icTrilobit : icTrilobit2;

		ic.type = IT_UNIT;
		ic.width = 16;
		ic.height = 8;
		ic.LoadPictures(i==0 ? "m1.bmp" : "m3.bmp");
		ic.draw = DrawUnit;
		ic.move = MoveUnit;
		ic.needGround = true;

		ic.unitSpeed  = i==0 ? Fixed(50) : Fixed(55);
		ic.unitSpring = i==0 ? Fixed(100) : Fixed(110);
		ic.unitFly = false;
		ic.unitPlayer = false;
		ic.unitHealth = i==0? 30 : 60;
		ic.unitDefaultweapon = &icTrilobitClaws;
		ic.unitAI = i==0 ? AIStupidGroundUnit : AICleverGroundUnit;
		ic.unitBorner = false;

	}
}

void InitClassSnowman ()
{	
	ItemClass& ic = icSnowman;

	ic.type = IT_UNIT;
	ic.width = 8;
	ic.height = 16;
	ic.LoadPictures("m2.bmp");
	ic.draw = DrawUnit;
	ic.move = MoveUnit;
	ic.needGround = true;

	ic.unitSpeed  = Fixed(75);
	ic.unitSpring = Fixed(120);
	ic.unitFly = false;
	ic.unitPlayer = false;
	ic.unitHealth = 40;
	ic.unitDefaultweapon = &icSnowmanBlaster;
	ic.unitAI = AICleverGroundUnit;
	ic.unitBorner = false;
}

void InitClassZombie ()
{	
	ItemClass& ic = icZombie;

	ic.type = IT_UNIT;
	ic.width = 8;
	ic.height = 32;
	ic.LoadPictures("m4.bmp");
	ic.draw = DrawUnit;
	ic.move = MoveUnit;
	ic.needGround = true;

	ic.unitSpeed  = Fixed(90);
	ic.unitSpring = Fixed(150);
	ic.unitFly = false;
	ic.unitPlayer = false;
	ic.unitHealth = 200;
	ic.unitDefaultweapon = &icZombieClaws;
	ic.unitAI = AICleverGroundUnit;
	ic.unitBorner = false;
}

void InitClassButcher ()
{	
	ItemClass& ic = icButcher;

	ic.type = IT_UNIT;
	ic.width = 8;
	ic.height = 32;
	ic.LoadPictures("m5.bmp");
	ic.draw = DrawUnit;
	ic.move = MoveUnit;
	ic.needGround = true;

	ic.unitSpeed  = Fixed(30);
	ic.unitSpring = Fixed(30);
	ic.unitFly = false;
	ic.unitPlayer = false;
	ic.unitHealth = 400;
	ic.unitDefaultweapon = &icButcherClaws;
	ic.unitAI = AICleverGroundUnit;
	ic.unitBorner = false;
}

void InitClassButcherZombie ()
{	
	ItemClass& ic = icButcherZombie;

	ic.type = IT_UNIT;
	ic.width = 8;
	ic.height = 32;
	ic.LoadPictures("m4.bmp");
	ic.draw = DrawUnit;
	ic.move = MoveUnit;
	ic.needGround = true;

	ic.unitSpeed  = Fixed(40);
	ic.unitSpring = Fixed(30);
	ic.unitFly = false;
	ic.unitPlayer = false;
	ic.unitHealth = 200;
	ic.unitDefaultweapon = &icButcherClaws;
	ic.unitAI = AICleverGroundUnit;
	ic.unitBorner = false;
}

void InitClassLizard ()
{	
	ItemClass& ic = icLizard;

	ic.type = IT_UNIT;
	ic.width = 8;
	ic.height = 32;
	ic.LoadPictures("m6.bmp");
	ic.draw = DrawUnit;
	ic.move = MoveUnit;
	ic.needGround = true;

	ic.unitSpeed  = Fixed(50);
	ic.unitSpring = Fixed(50);
	ic.unitFly = false;
	ic.unitPlayer = false;
	ic.unitHealth = 200;
	ic.unitDefaultweapon = &icLizardBlaster;
	ic.unitAI = AICleverGroundUnit;
	ic.unitBorner = false;
}

void InitClassBigSnowman ()
{	
	ItemClass& ic = icBigSnowman;

	ic.type = IT_UNIT;
	ic.width = 8;
	ic.height = 32;
	ic.LoadPictures("m7.bmp");
	ic.draw = DrawUnit;
	ic.move = MoveUnit;
	ic.needGround = true;

	ic.unitSpeed  = Fixed(50);
	ic.unitSpring = Fixed(50);
	ic.unitFly = false;
	ic.unitPlayer = false;
	ic.unitHealth = 120;
	ic.unitDefaultweapon = &icBigSnowmanBlaster;
	ic.unitAI = AICleverGroundUnit;
	ic.unitBorner = false;
}

void InitClassBorner ()
{	
	for (int t=0; t<3; ++t)
	{
		ItemClass& ic = t==0 ? icBorner1 : t==1 ? icBorner2 : icBorner3;

		ic.type = IT_UNIT;
		ic.width = 16;
		ic.height = 32;
		ic.LoadPictures("borner.bmp");
		ic.draw = DrawUnit;
		ic.move = MoveBorner;
		ic.needGround = true;

		ic.unitSpeed  = fx0;
		ic.unitSpring = fx0;
		ic.unitFly = false;
		ic.unitPlayer = false;
		ic.unitHealth = 300;
		ic.unitDefaultweapon = NULL;
		ic.unitAI = AIStupidGroundUnit;
		ic.unitChildClass = t==0 ? &icTrilobit : t==1 ? &icSnowman : &icCock;
	}
}

void InitClassCock ()
{	
	for (int t=0; t<2; ++t)
	{
		ItemClass& ic = t==0 ? icCock : icBigCock;

		ic.type = IT_UNIT;
		ic.width = 8;
		ic.height = t==0 ? 16 : 32;
		ic.LoadPictures(t==0 ? "m8.bmp" : "m9.bmp");
		ic.draw = DrawUnit;
		ic.move = MoveUnit;
		ic.needGround = true;

		ic.unitSpeed  = t==0 ? Fixed(60) : Fixed(80);
		ic.unitSpring = t==0 ? Fixed(150) : Fixed(200);
		ic.unitFly = false;
		ic.unitPlayer = false;
		ic.unitHealth = t==0? 40 : 80;
		ic.unitDefaultweapon = &icCockClaws;
		ic.unitAI = AIJumpingUnit;
		ic.unitBorner = false;
	}
}

void InitClassFlyingCock ()
{	
	ItemClass& ic = icFlyingCock;

	ic.type = IT_UNIT;
	ic.width = 8;
	ic.height = 32;
	ic.LoadPictures("m10.bmp");
	ic.draw = DrawUnit;
	ic.move = MoveUnit;
	ic.needGround = false;

	ic.unitSpeed  = Fixed(50);
	ic.unitSpring = Fixed(50);
	ic.unitFly = true;
	ic.unitPlayer = false;
	ic.unitHealth = 100;
	ic.unitDefaultweapon = &icCockClaws;
	ic.unitAI = AIFlyingUnit;
	ic.unitBorner = false;
}

void InitClassFlyingDrone ()
{		
	for (int t=0; t<2; ++t)
	{
		ItemClass& ic = t==0 ? icFlyingDrone : icFlyingHead;

		ic.type = IT_UNIT;
		ic.width = 8;
		ic.height = 32;
		ic.LoadPictures(t==0? "m11.bmp" : "m12.bmp");
		ic.draw = DrawUnit;
		ic.move = MoveUnit;
		ic.needGround = false;

		ic.unitSpeed  = Fixed(70);
		ic.unitSpring = Fixed(70);
		ic.unitFly = true;
		ic.unitPlayer = false;
		ic.unitHealth = 180;
		ic.unitDefaultweapon = t==0 ? &icDroneBlaster : &icHeadBlaster;
		ic.unitAI = AIFlyingUnit;
		ic.unitBorner = false;
	}
}

void InitClassFlyingShroom ()
{	
	ItemClass& ic = icFlyingShroom;

	ic.type = IT_UNIT;
	ic.width = 8;
	ic.height = 32;
	ic.LoadPictures("m13.bmp");
	ic.draw = DrawUnit;
	ic.move = MoveUnit;
	ic.needGround = false;

	ic.unitSpeed  = Fixed(80);
	ic.unitSpring = Fixed(80);
	ic.unitFly = true;
	ic.unitPlayer = false;
	ic.unitHealth = 120;
	ic.unitDefaultweapon = &icTrilobitClaws;
	ic.unitAI = AIFlyingUnit;
	ic.unitBorner = false;
}

void InitClassShroom ()
{	
	for (int t=0; t<2; ++t)
	{
		ItemClass& ic = t==0 ? icShroom : icBigShroom;

		ic.type = IT_UNIT;
		ic.width = 8;
		ic.height = t==0 ? 16 : 32;
		ic.LoadPictures(t==0 ? "m14.bmp" : "m15.bmp");
		ic.draw = DrawUnit;
		ic.move = MoveUnit;
		ic.needGround = true;

		ic.unitSpeed  = fx0;
		ic.unitSpring = fx0;
		ic.unitFly = false;
		ic.unitPlayer = false;
		ic.unitHealth = t==0? 170 : 320;
		ic.unitDefaultweapon = t==0 ? &icShroomSpores1 : &icShroomSpores2;
		ic.unitAI = AIStupidGroundUnit;
		ic.unitBorner = false;
	}
}

void InitMonsters ()
{	
	InitClassPlayer ();
	InitClassTrilobit ();
	InitClassSnowman ();
	InitClassBorner ();
	InitClassZombie ();
	InitClassButcher ();
	InitClassLizard ();
	InitClassButcherZombie ();
	InitClassBigSnowman ();
	InitClassCock ();
	InitClassFlyingCock ();
	InitClassFlyingDrone ();
	InitClassShroom ();
	InitClassFlyingShroom ();

}