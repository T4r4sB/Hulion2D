#pragma once
#include "mmsystem.h"


bool isSounds = true;

// эту хуйню нахуй есличё
class Mp3Sound 
{
	static int index;
	bool opened;
	std::string filename;
	std::string alias;
	Mp3Sound (const Mp3Sound&);
	Mp3Sound& operator = (const Mp3Sound&);
public : 
	Mp3Sound (const char* filename) : opened(false), filename (filename)
	{
	}

	~Mp3Sound ()
	{
		if (opened)
		{
			char buffer[128]; int ret;
			sprintf(buffer, "CLOSE %s", alias.c_str());
			ret = mciSendString(buffer, NULL, 0, NULL);
			assert (ret==0);
			(void)ret;
		}
	}

	void Play ()
	{
		if (isSounds)
		{
			if (!opened) // ленивое говно
			{				
				alias = "sound" + tblib::to_string(index++);

				char buffer[128]; int ret;
				sprintf(buffer, "OPEN assets/sounds/%s.mp3 TYPE MpegVideo ALIAS %s", filename.c_str(), alias.c_str());
				ret = mciSendString(buffer, NULL, 0, NULL);   
				assert (ret==0);
				opened=true;
				(void)ret; 
			}

			{
				char buffer[128]; int ret;
				sprintf(buffer, "SEEK %s TO 0", alias.c_str());
				ret = mciSendString(buffer, NULL, 0, NULL);    
				assert (ret==0); 
				sprintf(buffer, "PLAY %s TO 100", alias.c_str());
				ret = mciSendString(buffer, NULL, 0, NULL);   
				assert (ret==0);
				(void)ret;
			}
		}
	}
};

int Mp3Sound::index = 0;
/*
Mp3Sound 
	soundWp1("wp1"), 
	soundWp2("wp2"), 
	soundWp3("wp3"), 
	soundWp4("wp4"), 
	soundWp6("wp6"), 
	soundWp7("wp7");*/

HMIDIOUT hMidi;
int volume=127;

std::string VolumeToString ()
{
	std::string result;
	result.resize(10);
	int pc = (volume+13)/14;

	for (int i=0; i<pc; ++i)
		result[i]='!';
	for (int i=pc; i<int(result.size()); ++i)
		result[i]='.';

	return result;
}

int __lastNota[4] = {-1,-1,-1,-1};
tblib::carray<int, 4> lastNota(__lastNota);

class MidiSound 
{
	int chanel;
	int instrument,nota;
	MidiSound (const MidiSound&);
	MidiSound& operator = (const MidiSound&);
public :
	MidiSound (int chanel, int instrument, int nota) 
		: chanel(chanel), instrument(instrument), nota(nota)
	{
		// никакой работы с устройством в конструкторе!!!
	}

  void Play ()
	{
		// установить струмент
		midiOutShortMsg(hMidi, 0xc0 | chanel | (instrument<<8));
		// заткнуться
		if (lastNota[chanel]>=0)
		 midiOutShortMsg(hMidi, 0x90 | chanel | (lastNota[chanel]<<8));
		// играть
		lastNota[chanel]=nota;
    midiOutShortMsg(hMidi, 0x90 | chanel | (nota<<8) | (volume<<16));
	}
};

MidiSound 
	soundWp1(0,127,50), 
	soundWp2(0,127,55), 
	soundWp3(0,127,45), 
	soundWp4(0,118,90), 
	soundWp6(0,127,60), 
	soundWp7(0,118,100),
	soundBlow(0,127,10),
	soundHit(2,117,90),

	soundHeal(1,120,60),
	soundArm(1,120,55),
	soundKey(1,120,45),
	soundWin(1,12,80),

	soundGibs(2,118,35);

void OpenMidi ()
{
	midiOutOpen(&hMidi, UINT(-1), 0, 0, 0);
}

void CloseMidi ()
{
	midiOutClose(hMidi);
}