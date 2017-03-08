#include "scale.h"
#include "tbal.h"
#include "graph.h"
#include "font.h"
#include "pasrand.h"
#include "saveload.h"

void HandleNumber (tbal::Action action, int n)
{	
	if (action==tbal::ACTION_DOWN)
	{
		if (menu.EnterSeed())
		{
			if (nextLevelSeed<10000000)
			{
				nextLevelSeed *= 10;
				nextLevelSeed += n;
				menu.SetState(MS_OPTIONS);
			}
		}
	}
}

void HandleVolume (tbal::Action action, int n)
{	
	if (action==tbal::ACTION_DOWN)
	{
		if (menu.EnterVolume())
		{
			volume += n*3;
			tblib::inbound(volume, 0, 127);
			menu.SetState(MS_OPTIONS);
		}
	}
}

bool TbalMain (tbal::Action action, tbal::Code code, int x, int y) 
{
  (void)y;
  switch (code) 
  {    
  case tbal::CODE_GETNAME : 
    tbal::SetProjectName("Lab2D"); 
    break;
	case tbal::CODE_TIMER :
		{
			if (menu.state==MS_NO)
			{
				// да, инквизитор, тут можно добавить условие на WASD
				player->controls.up    = tbal::Pressed(tbal::CODE_UP),
				player->controls.down  = tbal::Pressed(tbal::CODE_DOWN),
				player->controls.left  = tbal::Pressed(tbal::CODE_LEFT),
				player->controls.right = tbal::Pressed(tbal::CODE_RIGHT);
				player->controls.shot  = tbal::Pressed(tbal::CODE_CTRL);

				player->controls.wpnumber=-1;
				if (tbal::Pressed(tbal::CODE_1)) player->controls.wpnumber=0;
				if (tbal::Pressed(tbal::CODE_2)) player->controls.wpnumber=1;
				if (tbal::Pressed(tbal::CODE_3)) player->controls.wpnumber=2;
				if (tbal::Pressed(tbal::CODE_4)) player->controls.wpnumber=3;
				if (tbal::Pressed(tbal::CODE_5)) player->controls.wpnumber=4;
				if (tbal::Pressed(tbal::CODE_6)) player->controls.wpnumber=5;
				if (tbal::Pressed(tbal::CODE_7)) player->controls.wpnumber=6;
				if (tbal::Pressed(tbal::CODE_8)) player->controls.wpnumber=7;
				

				static int rem = 0;
				rem = std::min(rem+x, 100);

				while (rem>0)
				{
					Phys ();
					rem -= DT;
				}
			}
			DrawAll();
		}
		break;
	case tbal::CODE_START :
		{
			OpenMidi ();
			InitClasses ();

			if (!LoadLevel(slotAuto.c_str()))
				NewGame ();

			InitMenus(buf);
			tbal::EnableButtonDraw(false);
			tbal::EnableMouse(false);
			tbal::SetTimer(1);			
		}
		break;	
  case tbal::CODE_ESCAPE : 
		if (action==tbal::ACTION_DOWN)
		{
			menu.Escape ();
		}
		break;
	case tbal::CODE_UP : 
		if (menu.state != MS_NO && action==tbal::ACTION_DOWN)
			menu.UpChoise();
		break;
	case tbal::CODE_DOWN : 
		if (menu.state != MS_NO && action==tbal::ACTION_DOWN)
			menu.DownChoise();
		break;
	case tbal::CODE_ENTER :
		if (action==tbal::ACTION_DOWN)
		{
			menu.Select ();
			if (end)
				return false;
		}
		break;
	case tbal::CODE_F2 :
		if (action==tbal::ACTION_UP)
		{
			menu.SetState(MS_SAVE);
		}
		break;
	case tbal::CODE_F3 :
		if (action==tbal::ACTION_UP)
		{
			menu.SetState(MS_LOAD);
		}
		break;
	case tbal::CODE_F6 :
		if (action==tbal::ACTION_UP)
		{
			menu.HandleSave(slotFast);
		}
		break;
	case tbal::CODE_F9 :
		if (action==tbal::ACTION_UP)
		{
			menu.HandleLoad(slotFast);
		}
		break;
	case tbal::CODE_BACKSPACE : 
		if (action==tbal::ACTION_DOWN)
		{
			if (menu.EnterSeed())
			{
				nextLevelSeed /= 10;
				menu.SetState(MS_OPTIONS);
			}
		}
		break;
	case tbal::CODE_0 :	HandleNumber(action, 0);	break;
	case tbal::CODE_1 :	HandleNumber(action, 1);	break;
	case tbal::CODE_2 :	HandleNumber(action, 2);	break;
	case tbal::CODE_3 :	HandleNumber(action, 3);	break;
	case tbal::CODE_4 :	HandleNumber(action, 4);	break;
	case tbal::CODE_5 :	HandleNumber(action, 5);	break;
	case tbal::CODE_6 :	HandleNumber(action, 6);	break;
	case tbal::CODE_7 :	HandleNumber(action, 7);	break;
	case tbal::CODE_8 :	HandleNumber(action, 8);	break;
	case tbal::CODE_9 :	HandleNumber(action, 9);	break;
	case tbal::CODE_LEFT : HandleVolume(action, -1); break;
	case tbal::CODE_RIGHT : HandleVolume(action, 1); break;
	case tbal::CODE_EXIT :
		SaveLevel(slotAuto.c_str());
		CloseMidi();
		break;
  }
  return true;
}