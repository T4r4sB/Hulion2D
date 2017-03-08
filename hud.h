#pragma once
void ShowWeapons (const tbal::Bitmap &b)
{
	int w=0;
	for (int i=0; i<playerWp.size(); ++i)
	{
		w += playerWp[i]->frameWidth-8;
		if (i>0) w += 2;
	}

	int x = b.sizeX()/2 - w/2, y = b.sizeY() - 32;
	
	for (int i=0; i<playerWp.size(); ++i)
	{
		tbal::Bitmap bw = b.Window(x, y, playerWp[i]->frameWidth-8, 14);
		Bevel(bw, i==player->wpnumber);

		if (player->weapons[i].has)
		{
			assert (player->weapons[i].weaponClass == playerWp[i]);
			bw.Window(2,2,bw.sizeX()-4,bw.sizeY()-4).Copy(tbal::TransparentTest(tbal::COLOR_BLACK), 
				playerWp[i]->picturer
				.Window(5, playerWp[i]->frameHeight-10, playerWp[i]->frameWidth-10, 10),	0, 0);

			//bw.Window(2,2,bw.sizeX()-4,bw.sizeY()-4).Fill(HalfColor());
		
			if (player->weapons[i].weaponClass->wpArmorIndex >=0)
			{
				int cnt = player->armors[ player->weapons[i].weaponClass->wpArmorIndex ].bulletCount;
				f.OutText(bw, 2, 2, tblib::to_string(cnt), 
					i==player->wpnumber ? tbal::Color(0xff, 0x88, 0x00) : tbal::Color(0xdd, 0x22, 0x00));
			}
		}

		x += playerWp[i]->frameWidth-6;
	}
}

void ShowKeys (const tbal::Bitmap &b)
{
	int w=0;
	for (int i=0; i<icKeys.size(); ++i)
	{
		w += icKeys[i]->frameWidth+4;
		if (i>0) w += 2;
	}

	int x = b.sizeX()/2 - w/2, y = b.sizeY() - 16;
	
	for (int i=0; i<icKeys.size(); ++i)
	{
		tbal::Bitmap bw = b.Window(x, y, icKeys[i]->frameWidth+4, 14);
		Bevel(bw, false);
		x += icKeys[i]->frameWidth+6;
		if (player->hasKey[i])
			bw.Window(2,2,icKeys[i]->frameWidth,10).Copy(tbal::CopyPixel(), 
				icKeys[i]->picturel, 0, 0);
	}
}

void ShowHud (const tbal::Bitmap &b)
{
	ShowWeapons(b);
	ShowKeys(b);
}