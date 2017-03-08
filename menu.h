#pragma once

#include "tbal.h"
#include "font.h"
#include "bevel.h"
#include <sys/stat.h>

tbfont::Font f;

bool end=false;

uint32_t nextLevelSeed=15071987;

enum MenuState
{
	MS_NO, MS_WAIT, MS_MAIN, MS_NEW, MS_SAVE, MS_LOAD, MS_OPTIONS
};

void NewGame ();
bool SaveLevel (const char* filename);
bool LoadLevel (const char* filename);
void AddMessage (tblib::stringref text, bool canrepeat=false);

tbal::Picture buf(320, 200);

void Draw (const tbal::Bitmap& b);

void DrawAll ()
{	
	tbal::Buffer b;
	int sx = std::min(b.sizeX(), b.sizeY()*buf.sizeX()/buf.sizeY());
	int sy = std::min(b.sizeY(), b.sizeX()*buf.sizeY()/buf.sizeX());

	assert (sx==b.sizeX() || sy==b.sizeY());

	tbal::Bitmap bw = b.Window(b.sizeX()/2-sx/2, b.sizeY()/2-sy/2, sx, sy);
	if (sx==b.sizeX())
	{
		b.Window(0, 0, b.sizeX(), b.sizeY()/2 - sy/2);
		b.Window(0, b.sizeY()/2 - sy/2 + sy, b.sizeX(), b.sizeY() - (b.sizeY()/2 - sy/2 + sy));
	} else
	{
		b.Window(0, 0, b.sizeX()/2 - sx/2, b.sizeY());
		b.Window(b.sizeX()/2 - sx/2 + sx, 0, b.sizeX(), b.sizeX() - (b.sizeX()/2 - sx/2 + sx));
	}

	if (sx==buf.sizeX() && sy==buf.sizeY())
	{
		// спицально для силирона!!1
		Draw(bw);
	} else
	{
		Draw(buf);
		ScaleBitmap(buf, bw);
	}
}

const std::string slot1 = "slot1.sav";
const std::string slot2 = "slot2.sav";
const std::string slot3 = "slot3.sav";
const std::string slot4 = "slot4.sav";
const std::string slot5 = "slot5.sav";
const std::string slotFast = "fast.sav";
const std::string slotAuto = "auto.sav";

std::string IntToStr2 (int i)
{
	std::string result;
	result.resize(2);

	result[0] = char(int('0')+(i/10));
	result[1] = char(int('0')+(i%10));

	return result;
}

const char* __monNames [12] = {
	"ян",
	"фев",
	"мар",
	"апр",
	"май",
	"июн",
	"июл",
	"авг",
	"сен",
	"окт",
	"нов",
	"дек"
};

tblib::carray<const char*, 12> monNames(__monNames);

std::string FileInfo(std::string filename)
{
	struct _stat st;
	
	if (_stat((tbal::GetExternalFilesDir()+filename).c_str(), &st)==0)
	{
		struct tm *info = localtime(&st.st_mtime);
		return IntToStr2(info->tm_hour) + ":" + IntToStr2(info->tm_min) + ":" + IntToStr2(info->tm_sec)
			+ " " + tblib::to_string(info->tm_mday) + " " + monNames[info->tm_mon] + " " + tblib::to_string(info->tm_year-100);
	} else
	{
		return " -- пустой слот -- ";
	}
}

struct Menu
{
	MenuState state;
	tblib::array <std::string, 8> items;
	int choise;

	tblib::carray<int, 7> lastChoise;

	Menu () 
	{
		state = MS_NO;
		for (int i=0; i<lastChoise.size(); ++i)
			lastChoise[i]=2;
	}

	bool EnterSeed () 
	{
		return (state==MS_OPTIONS && choise==2);
	}
	
	bool EnterSeed (int n) 
	{
		return (state==MS_OPTIONS && n==2);
	}

	bool EnterVolume ()
	{
		return (state==MS_OPTIONS && choise==3);
	}
		
	void SetState (MenuState newState)
	{
		lastChoise[state]=choise;
		state = newState;	

		items.shrink(0);
		switch (state)
		{
		case MS_MAIN :
			items.emplace_back("Меню");
			items.emplace_back("");
			items.emplace_back("Заново");
			items.emplace_back("Продолжить");
			items.emplace_back("Сохранить");
			items.emplace_back("Загрузить");
			items.emplace_back("Настройки");
			items.emplace_back("Выйти");
			break;
		case MS_SAVE : 
			items.emplace_back("Сохранить");
			items.emplace_back("");
			items.emplace_back(FileInfo(slot1));
			items.emplace_back(FileInfo(slot2));
			items.emplace_back(FileInfo(slot3));
			items.emplace_back(FileInfo(slot4));
			items.emplace_back(FileInfo(slot5));
			items.emplace_back("Назад");
			break;
		case MS_LOAD : 
			items.emplace_back("Загрузить");
			items.emplace_back("");
			items.emplace_back(FileInfo(slot1));
			items.emplace_back(FileInfo(slot2));
			items.emplace_back(FileInfo(slot3));
			items.emplace_back(FileInfo(slot4));
			items.emplace_back(FileInfo(slot5));
			items.emplace_back("Назад");
			break;
		case MS_OPTIONS :
			items.emplace_back("Настройки");
			items.emplace_back("");
			items.emplace_back(std::string("Номер пещеры: ") + tblib::to_string(nextLevelSeed) + "|");
			items.emplace_back(std::string("Громкость: ") + VolumeToString());
			items.emplace_back("Назад");
			break;
		case MS_WAIT : 
			items.emplace_back("Подожди...");
			break;
		}

		if (items.size()>=3)
			choise=lastChoise[state];
	}

	void DownChoise ()
	{
		if (items.size()>=3)
		{
			++choise;
			if (choise>=items.size())
				choise=2;
		}
	}

	void UpChoise ()
	{
		if (items.size()>=3)
		{
			--choise;
			if (choise<2)
				choise=items.size()-1;
		}
	}
	
	void HandleSave (std::string filename)
	{
		MenuState cur=state;
		SetState(MS_WAIT); 
		DrawAll(); 
		if (SaveLevel(filename.c_str()))
		{
			SetState(MS_NO);
			AddMessage("Сохранено");
		}	else
		{
			SetState(cur);
			AddMessage("Проблема с сохранением!");
		}
	}
	
	void HandleLoad (std::string filename)
	{
		MenuState cur=state;
		SetState(MS_WAIT); 
		DrawAll(); 
		if (LoadLevel(filename.c_str()))
		{
			SetState(MS_NO);
			AddMessage("Загружено");
		}	else
		{
			SetState(cur);
			AddMessage("Проблема с загрузкой!");
		}
	}

	void Select ()
	{
		switch (state)
		{
		case MS_MAIN : 
			switch (choise)
			{
			case 2 : SetState(MS_WAIT); DrawAll(); NewGame(); SetState(MS_NO); break;
			case 3 : Escape(); break;
			case 4 : SetState(MS_SAVE); break;
			case 5 : SetState(MS_LOAD); break;
			case 6 : SetState(MS_OPTIONS); break;
			case 7 : SetState(MS_WAIT); DrawAll(); end=true; break;
			}
			break;
		case MS_SAVE : 
			switch (choise)
			{
			case 2 : HandleSave(slot1); break;
			case 3 : HandleSave(slot2); break;
			case 4 : HandleSave(slot3); break;
			case 5 : HandleSave(slot4); break;
			case 6 : HandleSave(slot5); break;
			case 7 : Escape (); break;
			}
			break;
		case MS_LOAD : 
			switch (choise)
			{
			case 2 : HandleLoad(slot1); break;
			case 3 : HandleLoad(slot2); break;
			case 4 : HandleLoad(slot3); break;
			case 5 : HandleLoad(slot4); break;
			case 6 : HandleLoad(slot5); break;
			case 7 : Escape (); break;
			}
			break;
		case MS_OPTIONS :
			switch (choise)
			{
			case 4 : Escape (); break;
			}
			break;
		}
	}

	void Escape ()
	{
		switch (state)
		{
		case MS_MAIN : case MS_WAIT :
			SetState(MS_NO);
			break;
		case MS_NO : case MS_SAVE : case MS_LOAD : case MS_OPTIONS :
			SetState(MS_MAIN);
			break;
		}
	}
};

Menu menu;

void DrawMenu (const tbal::Bitmap &b)
{
	if (menu.items.size()>0)
	{
		int w=20, h=10;
		const int mh = f.Height("88")*3/2;
		for (int i=0; i<menu.items.size(); ++i)
		{
			tblib::enlarge(w, f.Width(menu.items[i])+20);
			h += mh;
		}

		tbal::Bitmap bw = b.Window(b.sizeX()/2-w/2, b.sizeY()/2-h/2, w, h);
		Bevel(bw,true);
		bw = bw.Window(3, 3, bw.sizeX()-6, bw.sizeY()-6);
		Bevel(bw,true);
		for (int i=0; i<menu.items.size(); ++i)
		{
			int tw = f.Width(menu.items[i]);

			// какой костыль!
			tblib::stringref ref = menu.items[i];

			if (menu.EnterSeed(i) 
				&& (i!=menu.choise || (tbal::MSec()/500)%2==0))
				ref = ref.get_slice(ref.low(), ref.high()-1);

			f.OutText(bw, bw.sizeX()/2-tw/2, 2+i*mh, ref, i==menu.choise?textColor2:textColor1, 0, 0);
		}

	}
}

void InitMenus (const tbal::Bitmap &b)
{
	f.Init(Fixed(b.sizeY(),30), Fixed(b.sizeY(),160) );
}