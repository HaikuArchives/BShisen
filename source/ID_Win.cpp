#include "ID_Win.h"
#include <stdio.h>
#include <string.h>
#include <Application.h>
#include <stdlib.h>


ID_Win::ID_Win(BLooper *l, unsigned int g): 
	Txt_Ctl(NULL), done(0), Game(0),
	BWindow(BRect(100, 100, 300, 180), "New Game By ID",
		B_TITLED_WINDOW,	
		B_NOT_RESIZABLE | /*B_NOT_CLOSABLE |*/ B_NOT_ZOOMABLE
		| B_NOT_MINIMIZABLE)
{
BView *back;
BButton * Cancel;
BButton * Accept;
BTextView *T;
register int i;

	loop = l;
	Game = g;
	back = new BView(Bounds(), NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	back->SetViewColor(216, 216, 216);
	AddChild(back);
	
	Accept = new BButton( BRect(10, 40, 90, 55), "Accept",
		"Accept", new BMessage(ACCEPT_BUTTON));
	Accept->MakeDefault(true);
	back->AddChild(Accept);

	Cancel = new BButton( BRect(120, 40, 190, 55), "Cancel",
		"Cancel", new BMessage(CANCEL_BUTTON));
	back->AddChild(Cancel);

	Txt_Ctl = new BTextControl(BRect(10, 10, 190, 25), "",
		"Game Number", "", NULL);
        T = Txt_Ctl->TextView();

	/*
	 * Only allow #s in there.
	 */
        for (i = 0; i < 256; i++)
                T->DisallowChar(i);

        for (i = '0'; i < '9' + 1; i++)
                T->AllowChar(i);   

	back->AddChild(Txt_Ctl);

	char buffer[64];
	if (!Game) Game = rand();
	sprintf(buffer, "%d", Game);
	Txt_Ctl->SetText(buffer);
	Txt_Ctl->MakeFocus(true);
	
	//Port = create_port(1, "ID Window Port");
	Show();
}



ID_Win::~ID_Win()
{
	//delete_port(Port);
}


void ID_Win::MessageReceived(BMessage *msg)
{
	if (msg->what == ACCEPT_BUTTON)
	{
		const char *cp;
		
		be_app->SetCursor(B_HAND_CURSOR);
		
		cp = Txt_Ctl->Text();
		if (cp && strlen(cp))
		{
			/*
			 * if  game == "0000000001", register
			 *
			 */
			if (strlen(cp) == 10)
			{
				if (cp[0] == '0' &&
					cp[1] == '0' &&
					cp[2] == '0' &&
					cp[3] == '0' &&
					cp[4] == '0' &&
					cp[5] == '0' &&
					cp[6] == '0' &&
					cp[7] == '0' &&
					cp[8] == '0' &&
					cp[9] == '1')
				{
					loop->PostMessage(new BMessage('_re_'));
				}
			}
			Game = 0;
			sscanf(cp, "%d", &Game);
			//if (!Game) Game = 0x65816;	// :-) 0 is now possible for a  game #
		}
		else // no # entered, just cancel
		{
			loop->PostMessage(new BMessage('nopa'));
			Quit();
		}
		
		
		BMessage *m = new BMessage('nopa');
		m->AddInt32("Game Number", Game);
		loop->PostMessage(m);
		Quit();
	}

	else if (msg->what == CANCEL_BUTTON)
	{
		be_app->SetCursor(B_HAND_CURSOR);
		Game = 0;
		be_app->SetCursor(B_HAND_CURSOR);
		loop->PostMessage(new BMessage('nopa'));
		Quit();
	}
}

bool ID_Win::QuitRequested(void)
{
	be_app->SetCursor(B_HAND_CURSOR);
	loop->PostMessage(new BMessage('nopa'));
	return true;
}
