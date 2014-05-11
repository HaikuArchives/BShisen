#include "LineControl.h"
#include <stdio.h>

LineControl::LineControl(BRect frame):
	BControl(frame, "", "", NULL, B_FOLLOW_ALL, B_WILL_DRAW)
{
}

LineControl::~LineControl()
{
	fprintf(stderr, "~LineControl called\n");
}


void LineControl::Draw(BRect)
{
	FillRect(Bounds());
}

void LineControl::Hide(void)
{
	FillRect(Bounds(), B_SOLID_LOW);
	BControl::Hide();
}
