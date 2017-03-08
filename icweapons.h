ItemClassRef 
	icSabre, icBlSabre, icGun, icBlGun, icPistol, icBlPistol, icBlaster, icBlBlaster,
	icSupergun, icBlSupergun, icGrenade, icBlGrenade, icGrenBlow, icBlGrenBlow,
	icRifle, icBlRifle, icRailgun, icBlRailgun,

	icArmGun, icArmPist, icArmBlast;

ItemClass* __playerWp [8] = 
	{&icSabre, &icGun, &icPistol, &icSupergun, &icBlaster, &icGrenade, &icRifle, &icRailgun};

tblib::carray<ItemClass*, 8> playerWp (__playerWp);


void InitClassSabre ()
{
	ItemClass& ic = icSabre;

	ic.type = IT_WEAPON;
	ic.width = 32;
	ic.height = 8;
	ic.LoadPictures("sabre.bmp");
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;

	ic.wpBulletSpeed = Fixed(250);
	ic.wpArmorIndex = -1;
	ic.wpBulletsInShot = 16;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx2;
	ic.wpIndex = 0;
	ic.wpReloadTime = 600;
	ic.wpBulletClass = &icBlSabre;

	ic.itemMessage = "Ты подобрал саблю";
	
	// disp=2, dmgs=133
	
	ItemClass& icb = icBlSabre;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;

	icb.blDamage = 5; // 5*16=80
 	icb.blDistance = 32;
	icb.blTraceColor = tbal::COLOR_IMPOSSIBLE;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassGun ()
{
	ItemClass& ic = icGun;

	ic.type = IT_WEAPON;
	ic.width = 8;
	ic.height = 8;
	ic.LoadPictures("gun.bmp");
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;

	ic.wpBulletSpeed = Fixed(400);
	ic.wpArmorIndex = 0;
	ic.wpInitialArmor = 8;
	ic.wpBulletsInShot = 16;
	ic.wpArmorsInShot = 2;
	ic.wpDispersion = fx1;
	ic.wpIndex = 1;
	ic.wpReloadTime = 500;
	ic.wpBulletClass = &icBlGun;
	ic.wpSound = &soundWp1;
	
	ic.itemMessage = "Ты подобрал ружьё";

	// disp=1, dmgs=192
	
	ItemClass& icb = icBlGun;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;

	icb.blDamage = 6; 
 	icb.blDistance = 150;
	icb.blTraceColor = tbal::COLOR_WHITE;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassPistol ()
{
	ItemClass& ic = icPistol;

	ic.type = IT_WEAPON;
	ic.width = 8;
	ic.height = 8;
	ic.LoadPictures("pistol.bmp");
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;

	ic.wpBulletSpeed = Fixed(600);
	ic.wpArmorIndex = 1;
	ic.wpInitialArmor = 12;
	ic.wpBulletsInShot = 1;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx0;
	ic.wpIndex = 2;
	ic.wpReloadTime = 300;
	ic.wpBulletClass = &icBlPistol;
	ic.wpSound = &soundWp2;

	ic.itemMessage = "Ты подобрал пистолет";
	
	// disp=0, dmgs=267
	
	ItemClass& icb = icBlPistol;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;

	icb.blDamage = 80; 
 	icb.blDistance = 300;
	icb.blTraceColor = tbal::COLOR_WHITE;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassSuperGun ()
{
	ItemClass& ic = icSupergun;

	ic.type = IT_WEAPON;
	ic.width = 8;
	ic.height = 8;
	ic.LoadPictures("supergun.bmp");
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;

	ic.itemMessage = "Ты подобрал суперружьё";

	// disp=2, dmgs=267

	ic.wpBulletSpeed = Fixed(400);
	ic.wpArmorIndex = 0;
	ic.wpInitialArmor = 16;
	ic.wpBulletsInShot = 8;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx2;
	ic.wpIndex = 3;
	ic.wpReloadTime = 150;
	ic.wpBulletClass = &icBlSupergun;
	ic.wpSound = &soundWp3;
	
	ItemClass& icb = icBlSupergun;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;

	icb.blDamage = 5; 
 	icb.blDistance = 150;
	icb.blTraceColor = tbal::COLOR_WHITE;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassBlaster ()
{
	ItemClass& ic = icBlaster;

	ic.type = IT_WEAPON;
	ic.width = 8;
	ic.height = 8;
	ic.LoadPictures("blaster.bmp");
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;

	ic.wpBulletSpeed = Fixed(160);
	ic.wpArmorIndex = 2;
	ic.wpInitialArmor = 30;
	ic.wpBulletsInShot = 1;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx1;
	ic.wpIndex = 4;
	ic.wpReloadTime = 150;
	ic.wpBulletClass = &icBlBlaster;
	ic.wpSound = &soundWp4;

	ic.itemMessage = "Ты подобрал бластер";

	// disp=1, dmgs=300
	
	ItemClass& icb = icBlBlaster;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;
	icb.LoadPictures("m2b.bmp");

	icb.blDamage = 45; 
 	icb.blDistance = 200;
	icb.blTraceColor = tbal::COLOR_IMPOSSIBLE;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassGrenade ()
{
	ItemClass& ic = icGrenade;

	ic.type = IT_WEAPON;
	ic.width = 16;
	ic.height = 8;
	ic.LoadPictures("grenade.bmp", true);
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;

	ic.wpBulletSpeed = Fixed(200);
	ic.wpArmorIndex = 3;
	ic.wpInitialArmor = 1;
	ic.wpBulletsInShot = 1;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx0;
	ic.wpIndex = 5;
	ic.wpReloadTime = 1000;
	ic.wpBulletClass = &icBlGrenade;
	
	ic.itemMessage = "Ты подобрал гранату";
	
	ItemClass& icb = icBlGrenade;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;
	icb.LoadPictures("flyinggrenade.bmp");

	icb.blDamage = 0; 
 	icb.blDistance = 1000;
	icb.blTraceColor = tbal::COLOR_IMPOSSIBLE;
	icb.blWeaponClass = &icGrenBlow;
	icb.blFly = false;

	ItemClass &gc = icGrenBlow;
		
	gc.type = IT_WEAPON;
	gc.width = 8;
	gc.height = 8;
	gc.draw = DrawItem;
	gc.move = MoveItem;
	gc.needGround = true;

	gc.wpBulletSpeed = Fixed(400);
	gc.wpArmorIndex = -1;
	gc.wpInitialArmor = 1;
	gc.wpBulletsInShot = 64;
	ic.wpArmorsInShot = 1;
	gc.wpDispersion = fx2Pi;
	gc.wpIndex = 0;
	gc.wpReloadTime = 1;
	gc.wpBulletClass = &icBlGrenBlow;
	gc.wpSound = &soundBlow;
	
	ItemClass& gcb = icBlGrenBlow;

	gcb.type = IT_BULLET;
	gcb.draw = DrawBullet;
	gcb.move = MoveBullet;
	gcb.needGround = false;

	gcb.blDamage = 30; 
 	gcb.blDistance = 100;
	gcb.blTraceColor = tbal::COLOR_YELLOW;
	gcb.blWeaponClass = NULL;
	gcb.blFly = true;
}

void InitClassRifle ()
{
	ItemClass& ic = icRifle;

	ic.type = IT_WEAPON;
	ic.width = 8;
	ic.height = 8;
	ic.LoadPictures("rifle.bmp");
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;

	ic.wpBulletSpeed = Fixed(600);
	ic.wpArmorIndex = 1;
	ic.wpInitialArmor = 24;
	ic.wpBulletsInShot = 1;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx0;
	ic.wpIndex = 6;
	ic.wpReloadTime = 100;
	ic.wpBulletClass = &icBlRifle;
	ic.wpSound = &soundWp6;

	ic.itemMessage = "Ты подобрал автомат";
	
	// disp=0, dmgs=400
	
	ItemClass& icb = icBlRifle;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;

	icb.blDamage = 40; 
 	icb.blDistance = 200;
	icb.blTraceColor = tbal::COLOR_WHITE;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassRailgun ()
{
	ItemClass& ic = icRailgun;

	ic.type = IT_WEAPON;
	ic.width = 8;
	ic.height = 8;
	ic.LoadPictures("railgun.bmp");
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;

	ic.wpBulletSpeed = Fixed(1600);
	ic.wpArmorIndex = 2;
	ic.wpInitialArmor = 50;
	ic.wpBulletsInShot = 50;
	ic.wpArmorsInShot = 50;
	ic.wpDispersion = fx0;
	ic.wpIndex = 7;
	ic.wpReloadTime = 1000;
	ic.wpBulletClass = &icBlRailgun;
	ic.wpSound = &soundWp7;

	ic.itemMessage = "Ты подобрал рельсотрон";
	
	// disp=0, dmgs=1050
	
	ItemClass& icb = icBlRailgun;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;

	icb.blDamage = 25; 
 	icb.blDistance = 2000;
	icb.blTraceColor = tbal::COLOR_TEAL;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassArmGun ()
{
	ItemClass& ic = icArmGun;

	ic.type = IT_ARMOR;
	ic.width = 8;
	ic.height = 8;
	ic.LoadPictures("armgun.bmp", true);
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;
	
	ic.itemMessage = "Ты подобрал 12 патронов с дробью";

	ic.armIndex = 0;
	ic.armCount = 12;
}

void InitClassArmPist ()
{
	ItemClass& ic = icArmPist;

	ic.type = IT_ARMOR;
	ic.width = 8;
	ic.height = 8;
	ic.LoadPictures("armpist.bmp", true);
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;
	
	ic.itemMessage = "Ты подобрал 8 пуль";

	ic.armIndex = 1;
	ic.armCount = 8;
}

void InitClassArmBlast ()
{
	ItemClass& ic = icArmBlast;

	ic.type = IT_ARMOR;
	ic.width = 8;
	ic.height = 8;
	ic.LoadPictures("armblast.bmp", true);
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;
	
	ic.itemMessage = "Ты подобрал 30 энергоблоков";

	ic.armIndex = 2;
	ic.armCount = 30;
}


void InitWeapons ()
{	
	InitClassSabre ();
	InitClassGun ();
	InitClassPistol ();
	InitClassSuperGun ();
	InitClassBlaster ();
	InitClassGrenade ();
	InitClassRifle ();
	InitClassRailgun ();

	InitClassArmGun ();
	InitClassArmPist ();
	InitClassArmBlast ();
}