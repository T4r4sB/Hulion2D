ItemClassRef 
	icTrilobitClaws, icBlTrilobitClaws,
	icSnowmanBlaster, icBlSnowmanBlaster,
	icZombieClaws, icBlZombieClaws,
	icButcherClaws, icBlButcherClaws,
	icBigSnowmanBlaster, icBlBigSnowmanBlaster,
	icLizardBlaster, icBlLizardBlaster,
	icCockClaws, icBlCockClaws,
	icDroneBlaster, icBlDroneBlaster,
	icHeadBlaster, icBlHeadBlaster,
	icShroomSpores1, icBlShroomSpores1,
	icShroomSpores2, icBlShroomSpores2;

void InitClassTrilobitClaws ()
{
	ItemClass& ic = icTrilobitClaws;

	ic.type = IT_WEAPON;
	ic.width = 32;
	ic.height = 8;
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;

	ic.wpBulletSpeed = Fixed(250);
	ic.wpArmorIndex = -1;
	ic.wpBulletsInShot = 8;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx1;
	ic.wpIndex = 0;
	ic.wpReloadTime = 900;
	ic.wpBulletClass = &icBlTrilobitClaws;
	
	ItemClass& icb = icBlTrilobitClaws;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;

	icb.blDamage = 1; // 1*8=16
 	icb.blDistance = 32;
	icb.blTraceColor = tbal::COLOR_IMPOSSIBLE;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassZombieClaws ()
{
	ItemClass& ic = icZombieClaws;

	ic.type = IT_WEAPON;
	ic.width = 32;
	ic.height = 8;
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;

	ic.wpBulletSpeed = Fixed(250);
	ic.wpArmorIndex = -1;
	ic.wpBulletsInShot = 8;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx1;
	ic.wpIndex = 0;
	ic.wpReloadTime = 900;
	ic.wpBulletClass = &icBlZombieClaws;
	
	ItemClass& icb = icBlZombieClaws;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;

	icb.blDamage = 4; // 2*8=16
 	icb.blDistance = 32;
	icb.blTraceColor = tbal::COLOR_IMPOSSIBLE;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassSnowmanBlaster ()
{
	ItemClass& ic = icSnowmanBlaster;

	ic.type = IT_WEAPON;
	ic.width = 32;
	ic.height = 8;
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;
	ic.LoadPictures("m2w.bmp");

	ic.wpBulletSpeed = Fixed(200);
	ic.wpArmorIndex = -1;
	ic.wpBulletsInShot = 1;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = Fixed(1,10);
	ic.wpIndex = 0;
	ic.wpReloadTime = 900;
	ic.wpBulletClass = &icBlSnowmanBlaster;
	
	ItemClass& icb = icBlSnowmanBlaster;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;
	icb.LoadPictures("m2b.bmp");

	icb.blDamage = 4;
 	icb.blDistance = 300;
	icb.blTraceColor = tbal::COLOR_IMPOSSIBLE;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassBigSnowmanBlaster ()
{
	ItemClass& ic = icBigSnowmanBlaster;

	ic.type = IT_WEAPON;
	ic.width = 32;
	ic.height = 8;
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;
	ic.LoadPictures("m6w.bmp");

	ic.wpBulletSpeed = Fixed(200);
	ic.wpArmorIndex = -1;
	ic.wpBulletsInShot = 1;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx2;
	ic.wpIndex = 0;
	ic.wpReloadTime = 200;
	ic.wpBulletClass = &icBlBigSnowmanBlaster;
	
	ItemClass& icb = icBlBigSnowmanBlaster;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;
	icb.LoadPictures("m2b.bmp");

	icb.blDamage = 6;
 	icb.blDistance = 300;
	icb.blTraceColor = tbal::COLOR_IMPOSSIBLE;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassLizardBlaster ()
{
	ItemClass& ic = icLizardBlaster;

	ic.type = IT_WEAPON;
	ic.width = 32;
	ic.height = 8;
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;
	ic.LoadPictures("m7w.bmp");

	ic.wpBulletSpeed = Fixed(1600);
	ic.wpArmorIndex = -1;
	ic.wpBulletsInShot = 1;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx0;
	ic.wpIndex = 0;
	ic.wpReloadTime = 1000;
	ic.wpBulletClass = &icBlLizardBlaster;
	
	ItemClass& icb = icBlLizardBlaster;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;

	icb.blDamage = 19;
 	icb.blDistance = 300;
	icb.blTraceColor = tbal::COLOR_GREEN;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassButcherClaws ()
{
	ItemClass& ic = icButcherClaws;

	ic.type = IT_WEAPON;
	ic.width = 32;
	ic.height = 8;
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;
	ic.LoadPictures("m5w.bmp");

	ic.wpBulletSpeed = Fixed(200);
	ic.wpArmorIndex = -1;
	ic.wpBulletsInShot = 1;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx0;
	ic.wpIndex = 0;
	ic.wpReloadTime = 1800;
	ic.wpBulletClass = &icBlButcherClaws;
	
	ItemClass& icb = icBlButcherClaws;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;
	icb.LoadPictures("gibs.bmp");

	icb.blDamage = 20;
 	icb.blDistance = 300;
	icb.blTraceColor = tbal::COLOR_IMPOSSIBLE;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassCockClaws ()
{
	ItemClass& ic = icCockClaws;

	ic.type = IT_WEAPON;
	ic.width = 32;
	ic.height = 8;
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;

	ic.wpBulletSpeed = Fixed(250);
	ic.wpArmorIndex = -1;
	ic.wpBulletsInShot = 4;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx1;
	ic.wpIndex = 0;
	ic.wpReloadTime = 800;
	ic.wpBulletClass = &icBlCockClaws;
	
	ItemClass& icb = icBlCockClaws;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;

	icb.blDamage = 2; // 2*6=12
 	icb.blDistance = 32;
	icb.blTraceColor = tbal::COLOR_IMPOSSIBLE;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassDroneBlaster ()
{
	ItemClass& ic = icDroneBlaster;

	ic.type = IT_WEAPON;
	ic.width = 32;
	ic.height = 8;
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;
	ic.LoadPictures("m11w.bmp");

	ic.wpBulletSpeed = Fixed(320);
	ic.wpArmorIndex = -1;
	ic.wpBulletsInShot = 1;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx0;
	ic.wpIndex = 0;
	ic.wpReloadTime = 1000;
	ic.wpBulletClass = &icBlDroneBlaster;
	
	ItemClass& icb = icBlDroneBlaster;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;

	icb.blDamage = 23;
 	icb.blDistance = 300;
	icb.blTraceColor = tbal::COLOR_YELLOW;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassHeadBlaster ()
{
	ItemClass& ic = icHeadBlaster;

	ic.type = IT_WEAPON;
	ic.width = 32;
	ic.height = 8;
	ic.draw = DrawItem;
	ic.move = MoveItem;
	ic.needGround = true;

	ic.wpBulletSpeed = Fixed(85);
	ic.wpArmorIndex = -1;
	ic.wpBulletsInShot = 1;
	ic.wpArmorsInShot = 1;
	ic.wpDispersion = fx0;
	ic.wpIndex = 0;
	ic.wpReloadTime = 600;
	ic.wpBulletClass = &icBlHeadBlaster;
	
	ItemClass& icb = icBlHeadBlaster;

	icb.type = IT_BULLET;
	icb.draw = DrawBullet;
	icb.move = MoveBullet;
	icb.needGround = false;

	icb.blDamage = 30;
 	icb.blDistance = 300;
	icb.blTraceColor = tbal::COLOR_RED;
	icb.blWeaponClass = NULL;
	icb.blFly = true;
}

void InitClassShroomSpores ()
{
	for (int t=0; t<2; ++t)
	{
		ItemClass& ic = t==0 ? icShroomSpores1 : icShroomSpores2;

		ic.type = IT_WEAPON;
		ic.width = 32;
		ic.height = 8;
		ic.draw = DrawItem;
		ic.move = MoveItem;
		ic.needGround = true;

		ic.wpBulletSpeed = Fixed(65);
		ic.wpArmorIndex = -1;
		ic.wpBulletsInShot = t==0 ? 1 : 7;
		ic.wpArmorsInShot = 1;
		ic.wpDispersion = fx2;
		ic.wpIndex = 0;
		ic.wpReloadTime = t==0 ? 200 : 1000;
		ic.wpBulletClass = t==0 ? &icBlShroomSpores1 : &icBlShroomSpores2;
		
		ItemClass& icb = t==0 ? icBlShroomSpores1 : icBlShroomSpores2;

		icb.type = IT_BULLET;
		icb.draw = DrawBullet;
		icb.move = MoveBullet;
		icb.needGround = false;
		icb.LoadPictures("m14b.bmp");

		icb.blDamage = 5;
 		icb.blDistance = 300;
		icb.blTraceColor = tbal::COLOR_IMPOSSIBLE;
		icb.blWeaponClass = NULL;
		icb.blFly = true;
	}
}

void InitMonsterWeapons ()
{
	InitClassTrilobitClaws ();
	InitClassSnowmanBlaster ();
	InitClassBigSnowmanBlaster ();
	InitClassButcherClaws ();
	InitClassZombieClaws ();
	InitClassLizardBlaster ();
	InitClassCockClaws ();
	InitClassDroneBlaster ();
	InitClassHeadBlaster ();
	InitClassShroomSpores ();
}