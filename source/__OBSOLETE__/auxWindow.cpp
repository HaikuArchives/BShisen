#include <Window.h>
#include <TextView.h>
#include <TextControl.h>
#include <stdio.h>
#include "auxWindow.h"

/*
 * Returns 0 if cancel was chose, otherwise
 * returns the game no
 *
 */
unsigned GameByID(int gameno /* default # */)
{
BTextView *T;
BTextControl *tC;
register int i;
char buffer[256];


	if (!gameno) gameno = 1;

	sprintf(buffer, "%d%c", gameno, 0);

	tC = new BTextControl( *(new BRect(10, 10, 20, 50)),
			"Game Number", buffer, NULL);

	T = tC->TextView();
	/* 
	 * Make sure only #s can be entered 
	 */
	for (i = 0; i < 256; i++)
		T->DisallowChar(i);

	for (i = '0'; i < '9' + 1; i++)
		T->AllowChar(i);

	AddChild(tC)

	Show();	// start running

}
