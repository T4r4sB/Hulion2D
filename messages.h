#pragma once
#include "font.h"
#include "tbal.h"
#include "bevel.h"

struct Message
{
	tblib::stringref text;
	int time;
	Message (tblib::stringref text, int time) : text(text), time(time) {}
};

tblib::array<Message, 4> messages;

void DeleteTopMessage ()
{
	for (int i=0; i<messages.size()-1; ++i)
		messages[i] = messages[i+1];
	messages.shrink(messages.size()-1);
}

bool operator == (tblib::stringref l, tblib::stringref r)
{
	if (l.high()-l.low() != r.high()-r.low())
		return false;

	for (int i=0; i<l.high()-l.low(); ++i)
		if (l[l.low()+i] != r[r.low()+i])
			return false;

	return true;
}

bool operator != (tblib::stringref l, tblib::stringref r)
{
	return ! (l==r);
}

void AddMessage (tblib::stringref text, bool canrepeat)
{
	if (canrepeat || messages.size()==0 || messages.back().text != text)
	{
		if (messages.size() == messages.capacity)
			DeleteTopMessage ();
		messages.emplace_back(text, 7000);
	}
}

void ProcessMessages ()
{
	for (int i=0; i<messages.size(); ++i)
		messages[i].time -= DT;

	if (messages.size()>0 && messages[0].time<0)
		DeleteTopMessage ();
}

struct HalfColor
{
	void operator () (tbal::Color &dst)
	{
		dst = dst.Half();
	}
};

void ShowMessages (const tbal::Bitmap& b)
{
	if (messages.size()>0)
	{
		tbal::Bitmap w = b.Window(b.sizeX()/40, 2, b.sizeX()*38/40, messages.size()*(b.sizeY()/20)+4);

		Bevel(w, false);

		for (int i=0; i<messages.size(); ++i)
			f.OutText(w, 2, i*(b.sizeY()/20)+2, messages[i].text, textColor1);
	}
}