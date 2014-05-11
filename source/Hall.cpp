#include "Hall.h"

#include <time.h>
#include <File.h>
#include <Button.h>
#include <Window.h>
#include <List.h>
#include <ListView.h>
#include <ListItem.h>
#include <ScrollView.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Application.h>
#include <Path.h>
#include <FindDirectory.h>
#include <StringView.h>
#include <Font.h>
#include <Entry.h>


extern uint32 REGISTERED;

// constructor
Hall::Hall(void):
	List(NULL),
	List1(NULL),
	List2(NULL),
	List3(NULL),
	List4(NULL),
	List5(NULL),
	path(NULL),
	_changed(false),
	_count(0)
{
BFile F;
off_t size;
struct score s;
int i;

	List = new BList(1);
	List1 = new BList(6);	// for 14x6
	List2 = new BList(6);	// for 18x8
	List3 = new BList(6);	// for 24 x 12
	List4 = new BList(6);	// for 28 x 16
	List5 = new BList(6);	// for 32 x 20

	path = new BPath();
	
	/*
	 * Find the scores file, if it exists
	 *
	 */
	/*B_COMMON_SETTINGS_DIRECTORY*/
	if (
		find_directory(B_USER_SETTINGS_DIRECTORY, path) == B_NO_ERROR
		&& path->Append(FILE_NAME, true) == B_NO_ERROR
		&& F.SetTo(path->Path(), B_READ_ONLY) == B_NO_ERROR)
	{
		if ((F.GetSize(&size) == B_NO_ERROR) 
			&& ((size % sizeof (struct score))==0))
		{
			_count = size / sizeof (struct score);
			if (_count && _count <= NUMBER)
				for ( i = 0; i < _count; i++)
				{
					BList *L;
					F.Read(&s, sizeof(struct score));
					HSList *h = new HSList();

					h->SetName((char *)s.Name);
					h->SetGameID(s.gameID);
					h->SetGameTime(s.gameTime);
					h->SetNumberTiles(s.gameTiles);
					h->SetTimeOfGame(s.absTime);

					//List->AddItem(h);
					switch(s.gameTiles)
					{
					case 14 * 6: L = List1; break;
					case 18 * 8: L = List2; break;
					case 24 * 12: L = List3; break;
					case 28 * 16: L = List4; break;
					case 32 * 20: L = List5; break;
					default: L = NULL;
					}
					if (L) L->AddItem(h);
				}
		}
	}
	//else fprintf(stderr, "Couldn't open high scores file\n");

}

//destructor

Hall::~Hall()
{
#define DELETE(arg) if (arg) delete arg

	DELETE(path);
	
	if (_changed)
		WriteScores();
	DELETE(List);
	DELETE(List1);
	DELETE(List2);
	DELETE(List3);
	DELETE(List4);
	DELETE(List5);

#undef DELETE	
}


void Hall::WriteScores(void)
{
BFile F;
int i;
struct score s;
entry_ref er;

BList *L;

	L = new BList(NUMBER);

	L->AddList(List1);
	L->AddList(List2);
	L->AddList(List3);
	L->AddList(List4);
	L->AddList(List5);	

	//fprintf(stderr, "WriteScores called\n");
	if (_changed && REGISTERED)
	{
		//fprintf(stderr, "Setting F\n");
		//get_ref_for_path("/boot/home/config/settings/BShisen.Scores", &er);
		if (F.SetTo(path->Path(),//&er, 
			B_WRITE_ONLY | B_ERASE_FILE | B_CREATE_FILE) == B_NO_ERROR)
		{
			for ( i = 0; i < L->CountItems(); i++)
			{
				HSList *h;

				memset(&s, 0, sizeof(struct score));

				h = (HSList *)L->ItemAt(i);
				s.gameID = h->GetGameID();
				s.gameTime =  h->GetGameTime();
				s.gameTiles = h->GetNumberTiles();
				s.absTime = h->GetTimeOfGame();
				memcpy(s.Name, h->GetName(), 
					strlen(h->GetName()) + 1);
				F.Write(&s, sizeof (struct score));
			}
			_changed = false;
		}
		//else fprintf(stderr, "Couldn't write to high scores file\n");
	}
	//fprintf(stderr, "WriteScores exiting\n");

	delete L;
}


/*
 * This function is called by BList::Sort() for sorting
 * scores.
 *
 * Caveats : the "void *"s are actualy "void **"s
 */
static int cmpFunc(const void *L1, const void *L2)
{
HSList *List1 = (HSList *)*(void **)L1;
HSList *List2 = (HSList *)*(void **)L2;


	if (List1->GetGameTime() < List2->GetGameTime()) return -1;
	if (List1->GetGameTime() > List2->GetGameTime()) return 1;

	return 0;
}

// returns true if score is destined for greatness
bool Hall::JudgeScore(unsigned long gametime, unsigned long game, int tiles)
{
bool Yes = false;

BList *L;
	
	switch(tiles)
	{
	case 14 * 6: L = List1; break;
	case 18 * 8: L = List2; break;
	case 24 * 12: L = List3; break;
	case 28 * 16: L = List4; break;
	case 32 * 20: L = List5; break;
	default: L = NULL;
	}

	if (!L)
	{
		//printf("No game size!\n");
		return false;
	}

	HSList *h = new HSList();
		

	h->SetGameID(game);
	h->SetGameTime(gametime);
	h->SetNumberTiles(tiles);
	h->SetTimeOfGame(time(NULL));
			

	h->SetName(NULL);

	/* Add the new item
	 */
	L->AddItem(h); 

	L->SortItems(cmpFunc);


	/*
	 * Limit entries to 5
	 */
	if (L->CountItems() > 5)
	{
		HSList *hs;
		hs = (HSList *)L->RemoveItem(5);
		delete hs;
	}

	if (L->HasItem(h))
	{
	AskName *Ask = new AskName();
	char *name;

		name = Ask->Go();
		Ask->Lock();
		Ask->Quit();
		be_app->SetCursor(B_HAND_CURSOR);
		h->SetName(name);
		free(name);
		_changed = true;
		Yes = true;
	}

	return Yes;
}

void Hall::ShowScores(BMessage *msg, BLooper *target)
{
	HighScores *s;

	s = new HighScores(*this, msg, target);

	//s->Go();
	s->Show();
}






HighScores::HighScores(Hall& H, BMessage *m, BLooper *l):
        BWindow(BRect(100, 100, 820, 500), "BShisen High Scores",
                B_TITLED_WINDOW,
                B_NOT_RESIZABLE | /*B_NOT_CLOSABLE |*/ B_NOT_ZOOMABLE)
{
BView *back;
BListView *LV;
BScrollView *SV;
BRect r;
BStringView *str;


	loop = l;
	msg = m;
	
	back = new BView(Bounds(), "BackGround", B_FOLLOW_ALL, B_WILL_DRAW); 

        back->SetViewColor(216, 216, 216);  

	AddChild(back);

	str = new BStringView(BRect(10, 10, 190, 25), "", "Name");
	str->SetHighColor(0, 0, 0);
	str->SetFontSize(14);
	str->SetFont(be_bold_font);
	back->AddChild(str);


	str = new BStringView(BRect(200, 10, 290, 25), "", "Time");
	str->SetHighColor(0, 0, 0);
	str->SetFontSize(14);
	str->SetFont(be_bold_font);
	back->AddChild(str);

	str = new BStringView(BRect(300, 10, 390, 25), "", "Game Size");
	str->SetHighColor(0, 0, 0);
	str->SetFontSize(14);
	str->SetFont(be_bold_font);
	back->AddChild(str);

	str = new BStringView(BRect(400, 10, 490, 25), "", "Game Number");
	str->SetHighColor(0, 0, 0);
	str->SetFontSize(14);
	str->SetFont(be_bold_font);
	back->AddChild(str);

	str = new BStringView(BRect(500, 10, 590, 25), "", "Date");
	str->SetHighColor(0, 0, 0);
	str->SetFontSize(14);
	str->SetFont(be_bold_font);
	back->AddChild(str);

	str = new BStringView(BRect(600, 10, 720, 25), "", "Tiles Per Minute");
	str->SetHighColor(0, 0, 0);
	str->SetFontSize(14);
	str->SetFont(be_bold_font);
	back->AddChild(str);


	r = Bounds();
	r.right -= 14;
	r.top += 30;

	LV = new BListView(r, "");
	SV = new BScrollView("", LV, B_FOLLOW_ALL, 0, false, true);

	LV->AddList(H.List);
	LV->AddList(H.List1);
	LV->AddList(H.List2);
	LV->AddList(H.List3);
	LV->AddList(H.List4);
	LV->AddList(H.List5);

	back->AddChild(SV);

}

HighScores::~HighScores()
{
}


void HighScores::Quit(void)
{
	//write_port(Port, 1, NULL, 0);
	loop->PostMessage(msg);
	Lock();
	BWindow::Quit();
}

#if 0
void HighScores::Go(void)
{
long foo;

	Port = create_port(1, "BShisen Port #2");
	Show();

	// block until the some action occurs
	read_port(Port, &foo, NULL, 0);

}
#endif
void HighScores::MessageReceived(BMessage *msg)
{

	switch (msg->what)
	{
	case 'cntu':
	//case CLOSE_WINDOW:

		//write_port(Port, 1, NULL, 0);
		loop->PostMessage(msg);
		Quit();		
		break;

	default: BWindow::MessageReceived(msg);
	}
}


#define ACCEPT_BUTTON 'acce'

AskName::AskName():
	BWindow(BRect(100, 100, 500, 200), "Enter your name",
		B_TITLED_WINDOW,	
		B_NOT_RESIZABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE
		| B_NOT_MINIMIZABLE),
	Text(NULL),
	text(NULL)
{
BView *back;
BButton * Accept;
BTextView *TV;
BStringView *str;


	const char *names[] =
	{
		"BeOS forever!",
		"BShisen Rules!",
		"Thanks Sheppy",
		"Say NO to drugs!",
		"Say NO to piracy!",
		"Say YES to BeOS!",
		"Just say \"BShisen\"",
		"Say YES to BShisen!",
	};

	back = new BView(Bounds(), NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	back->SetViewColor(216, 216, 216);
	AddChild(back);
	
	str = new BStringView(BRect(10, 10, 390, 25), "",
		"Congratulations!  Enter your name for posterity.");
	
	str->SetFont(be_bold_font);
	back->AddChild(str);
	
	Accept = new BButton( BRect(10, 60+5, 90, 75+5), "Accept",
		"Accept", new BMessage(ACCEPT_BUTTON));
	Accept->MakeDefault(true);
	back->AddChild(Accept);

	Text = new BTextControl(BRect(10, 35, 390, 50), "",
		"Your Name", "", NULL);
	
	Text->SetDivider(Text->StringWidth("Your Name "));


	TV = Text->TextView();
	TV->SetMaxBytes(31);	

	Text->SetText(REGISTERED ? names[rand() % 8] : "I Want To Register!");

	back->AddChild(Text);
	Text->MakeFocus(true);
}



/*
 * Destructor - does nothing
 */
AskName::~AskName()
{
}

char *AskName::Go(void)
{
int32 foo = 0;

	Port = create_port(1, "BShisen Port #3");
	Show();

	// block until a button is pressed.

	read_port(Port, &foo, NULL, 0);


	Hide();
	return text;
}

void AskName::MessageReceived(BMessage *msg)
{

	if (msg->what == ACCEPT_BUTTON)
	{
		text = strdup(Text->Text());
		write_port(Port, 1, NULL, 0);
	}
	else BWindow::MessageReceived(msg);
}


/*
 * HSList
 * 
 *
 *
 */

HSList::HSList():
	BListItem(),
	tps(0),
	name(NULL),
	gameID(0),
	gameTime(0),
	numTiles(0),
	timeOfGame(0)
	
	
{
}
HSList::~HSList()
{
	if (name) free(name);
}

void HSList::DrawItem(BView *owner, BRect rect, bool everything)
{
char *buffer;

		buffer = new char[2048];
		owner->SetHighColor(0, 0, 0);
		owner->MovePenTo(rect.left+10, rect.bottom-2);
		if (name) owner->DrawString(name);
		owner->MovePenTo(rect.left+200, rect.bottom-2);
		if (gameTime)
		{
			sprintf(buffer, "%.2d:%.2d:%.2d%c",
				(int)(gameTime /3600),
				(int)(gameTime / 60) %60,
				(int)gameTime % 60,
				(int)0);
			owner->DrawString(buffer);
		}

		owner->MovePenTo(rect.left+300, rect.bottom-2);
		if (numTiles)
		{
			char *cp = "";
			switch(numTiles)
			{
			case 14 * 6: cp = "14 x 6"; break;
			case 18 * 8: cp = "18 x 8"; break;
			case 24 * 12: cp = "24 x 12"; break;
			case 28 * 16: cp = "28 x 16"; break;
			case 32 * 20: cp = "32 x 20"; break;
			}	
			owner->DrawString(cp);
		}
		owner->MovePenTo(rect.left+400, rect.bottom-2);
		if (gameID)
		{
			sprintf(buffer, "%d%c", (int)gameID, (int)0);
			owner->DrawString(buffer);
		}

		owner->MovePenTo(rect.left+500, rect.bottom-2);
		if (timeOfGame)
		{
			struct tm *t;
			time_t angie = (time_t)timeOfGame;

			t = localtime(&angie);

			sprintf(buffer, "%d/%d/%d%c",
				t->tm_mon + 1,
				t->tm_mday,
				t->tm_year + 1900,
				0);
			owner->DrawString(buffer);

		}
		owner->MovePenTo(rect.left+600, rect.bottom-2);
		if (tps)
		{
			sprintf(buffer, "%f%c", tps * 60, 0);
			owner->DrawString(buffer);
		}

		delete []buffer;
}

void HSList::SetName(const char *n)
{
	if (name) free(name);
	name = strdup(n);
}
void HSList::SetGameID(unsigned long id)
{
	gameID = id;
}
void HSList::SetGameTime(unsigned long t)
{
	gameTime = t;
	if (gameTime) tps = (numTiles * 1.0) / gameTime;
}
void HSList::SetNumberTiles(int cnt)
{
	numTiles = cnt;
	if (gameTime) tps = (numTiles * 1.0) / gameTime;
}
void HSList::SetTimeOfGame(unsigned long time)
{
	timeOfGame = time;
}
