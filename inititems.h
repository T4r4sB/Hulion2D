#pragma once

void AddItemToCell (int cx, int cy, ItemClass *itemClass)
{
	int x = level.RandomNoStairX(cx, cy, false, false, true);

	int x1 = x-itemClass->width/2, x2 = x+itemClass->width/2;

	int y = 0;
	for (int i=x1; i<=x2; ++i)
		tblib::enlarge(y, level.t.heights[cy][i].lo+1);

	NewItem(itemClass, x, y);
}

int wasWeapon;

void InitCellItems (int cx, int cy, int progress)
{	
	bool hl=false, wp=false, arm=false, m=false;

	
	if (wasWeapon==0 && progress>2)
		wp=true;
	else
	{
		// чуров, хуле
		bool good = level.t.cells[cy][cx].inBestWay 
			? (rnd.random(100) < 80) 
			: true;
		bool bad  = level.t.cells[cy][cx].inBestWay 
			? (rnd.random(100) < 40+progress/8) 
			: (rnd.random(100) < 20+progress/8);

		if (good)
		{
			int r = rnd.random(100);

			if (r<30)
				hl=true;
			else if (r<50)
				wp=true;	
			else if (r<90)
				arm=true;
		} 

		if (bad)
		{
			m=true;
		}
	}


	if (hl)
	{		
		// аптечки
		AddItemToCell(cx, cy, &icHealth25);
		if (rnd.random(5)==0)
			AddItemToCell(cx, cy, &icHealth25);
	} else if (wp)
	{
		// оружие
		int wpi = std::min(playerWp.size()-1,  1+progress*(playerWp.size()-1)/50);
		if (wpi>wasWeapon && wasWeapon<playerWp.size()-1)
			wpi = ++wasWeapon;
		else
			wpi = rnd.random(wpi)+1;

		//wpi=5;

		AddItemToCell(cx, cy, playerWp[wpi]);
	} else if (arm)
	{	
		ItemClass *wpClass;
		if (wasWeapon>=5 && rnd.random(4)==0)
		{
			wpClass = &icGrenade;
		} else
		{
			int k = std::min(3, progress*3/15);
			switch (rnd.random(k))
			{
			case 0 : wpClass = &icArmGun; break;
			case 1 : wpClass = &icArmPist; break;
			case 2 : wpClass = &icArmBlast; break;
			default : wpClass = NULL;
			}
		}
		AddItemToCell(cx, cy, wpClass);
		if (rnd.random(5)==0)
			AddItemToCell(cx, cy, wpClass);
	} 
	// без елса
	if (m)
	{
		const int mind=2;
		int dngr = progress*icMonsters.size()/50+mind;

		while (dngr>=mind)
		{
			int cd = std::min(icMonsters.size()-1, rnd.random(dngr-(mind-1)));
			//cd=8;
			do
			{
				dngr -= (cd+mind);
				AddItemToCell(cx, cy, icMonsters[cd]);
			} while (dngr>=cd+mind);
		}
	}

	if (!hl && !wp && !arm && !m)
		AddItemToCell(cx, cy, icMonsters[0]);
}

int NewDoor (int cx, int cy)
{
  int x = cx*CELL_WIDTH + CELL_WIDTH;
	int y = 0;
	for (int i=x-DOOR_WIDTH/2; i<x+DOOR_WIDTH/2; ++i)
		tblib::enlarge(y, level.t.heights[cy][i].lo+1);	
	return NewItem(&icDoor, x, y);
}

void InitItems ()
{
	AddItemToCell(0,0,&icPlayer);
	player = &items[0];

	for (int j=0; j<level.t.cells.size(); ++j) for (int i=0; i<level.t.cells[j].size(); ++i)
	{
		if (level.t.cells[j][i].was)
		{
			if (level.t.cells[j][i].canPass[DIR_RIGHT]>CP_OPENED)
			{
				int index = NewDoor(i,j);
				if (index>=0)
					items[index].doorStage = level.t.cells[j][i].canPass[DIR_RIGHT]-CP_KEY-1;
			}

			if (level.t.cells[j][i].canPass[DIR_RIGHT] && !level.t.cells[j][i+1].canPass[DIR_LEFT])
			{
				int index = NewDoor(i,j);
				if (index>=0)
					items[index].doorStage = DS_LEFT;
			}
			
			if (level.t.cells[j][i].canPass[DIR_LEFT] && !level.t.cells[j][i-1].canPass[DIR_RIGHT])
			{
				int index = NewDoor(i-1,j);
				if (index>=0)
					items[index].doorStage = DS_RIGHT;
			}


			int k = level.t.cells[j][i].key;

			if (k>0 && k<=MAX_DOORS)
			{				
				int x = i*CELL_WIDTH + CELL_WIDTH/2;
				NewItem(icKeys[k-1], x, level.t.heights[j][x].lo+20);
			} else if (k>MAX_DOORS)
			{
				int x = i*CELL_WIDTH + CELL_WIDTH/2;
				NewItem(&icExit, x, level.t.heights[j][x].lo+20);
			}
		}
	}

	wasWeapon=0;

	
	for (int i=3; i<level.t.orderedCells.size(); ++i)
	{
		int cx = level.t.orderedCells[i].x;
		int cy = level.t.orderedCells[i].y;
		
		int k = level.t.cells[cy][cx].key;
		if (k>0)
			; // тут ключ должен быть
		else
		{
			int progress = i*100/(level.t.orderedCells.size()-1);
			InitCellItems(cx, cy, progress);
		}
	}

	for (int j=0; j<SIZEY; ++j)
		for (int i=0; i<SIZEX; ++i)
			wasIn[j][i]=false;

	assert (player == &items[0]);

	AddMessage ("Управление стрелочками, стрельба - CTRL");
	AddMessage ("Оружие выбирается цифрами");
	AddMessage ("Миникарта - TAB, меню - ESC, сохр F6, загр F9");
	AddMessage ("Мочи монстров, рви их на куски!!!");
}