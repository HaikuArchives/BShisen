/*
 * Copyright 1998, 1999 Kelvin W Sherlock
 *
 */
// Last edited by Pistooli, 22/04/2014
#include <Menu.h>
#include <MenuItem.h>
#include <MenuBar.h>
#include <Application.h>
#include <Bitmap.h>
#include <Button.h>
#include <Alert.h>
#include <PictureButton.h>
#include <Picture.h>
#include <Box.h>
#include <View.h>
#include <Bitmap.h>
#include <TranslationUtils.h>
#include <Path.h>
#include <Screen.h>
#include <Sound.h>
#include <SoundPlayer.h>
#include <SimpleGameSound.h>
#include <Entry.h>
#include <MediaFile.h>
#include <OS.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>


#include "ID_Win.h"
#include "GameWindow.h"
#include "BitmapControl.h"
#include "pics.h"
#include "res.h"
#include "CompileDefs.h"


/*
 * $Id: GameWindow.cpp,v 1.11 1999/07/21 16:32:00 baron Exp baron $
 */

extern uint64 KEY;
extern uint32 REGISTERED;
extern uint32 NAG;


GameWindow::GameWindow(): 
	BWindow(BRect(10, 50, 100, 100), "BShisen", 
		B_TITLED_WINDOW, 
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_ASYNCHRONOUS_CONTROLS),
	board(),
	hall(NULL),
	prefs(NULL),
	controls(NULL),
	BackGround(NULL),
	BackGround2(NULL),
	BackBitmap(NULL),
	Time_String(NULL),
	Number_String(NULL),
	Remain_String(NULL),
	Line1(NULL),
	Line2(NULL),
	Line3(NULL),
	connect_thread(-1),
	__forceQuit(false)
{
BMenuBar *bm;
BMenuItem *NewMItem;


	BackGround = new BView(Bounds(), "", B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(BackGround);
	

/*
 * Create my semaphores for later use
 *
 */

	tile_sem = create_sem(1, "BShisen tile Semaphore");
	timer_sem = create_sem(1, "BShisen timer Semaphore");

/*
 * Initialize the hall of fame
 */ 
	hall = new Hall;
	prefs = new Prefs;
#if BUILD == SHAREWARE
	//if (prefs->GetKey() == KEY)
	if (prefs->GetKey() != 0)
	{
		REGISTERED = true;
	}
#else
	REGISTERED = true;
#endif

	//if (NAG && (!REGISTERED))
	//{
	//	BAlert *b = new BAlert( "", "You've now used BShisen for more than 7 days "
	//							"and have probably had quite a bit of fun.  Please "
	//							"support the BeOS and those that bring you high "
	//							"quality, fun software by registering.",
	//							"Ok");
	//	b->Go();
	//
	//}


	if (prefs->IsFresh())
	{
		BAlert *b = new BAlert("", "Welcome to BShisen!  I hope you have fun "
									"with this fun and addictive game. Please "
									"see the README for more details. "
									"Have fun!",
									"Ok");
									
		b->Go();
	
		prefs->WritePrefs();	//write the prefs to fake out being fresh
	}

	CustomConnectSound = NULL;
	CustomNoConnectSound = NULL;
	CustomConnect = false;
	CustomNoConnect	= false;


	/*
	 * Create the menu bar
	 */
	bm = new BMenuBar(BRect(0,0,20,20), "");

	/*
	 * Add the File Menu
	 */
	FileM = new BMenu(_("MENU_FILE","File"));

	NewMItem = new BMenuItem(_("MENU_ITEM_NEW_GAME","New Game"), 
		new BMessage(NEW_MITEM), 'N');
	FileM->AddItem(NewMItem);

	NewMItem = new BMenuItem(_("MENU_ITEM_RESTART_GAME","Restart Game"), 
		new BMessage(RESTART_MITEM), 'R');
	FileM->AddItem(NewMItem);

	NewMItem = new BMenuItem(_("MENU_ITEM_NEW_GAME_BY_ID","New Game By ID"), 
		new BMessage(NEWBYID_MITEM), 'I');
	FileM->AddItem(NewMItem);

	//FileM->AddSeparatorItem();

	//NewMItem = new BMenuItem("Close Game", 
	//	new BMessage(CLOSE_MITEM), 'W');
	//FileM->AddItem(NewMItem);

	FileM->AddSeparatorItem();

	NewMItem = new BMenuItem(_("MENU_ITEM_ABOUT_BSHISEN","About BShisen ..."), 
		new BMessage(B_ABOUT_REQUESTED));
	NewMItem->SetTarget(be_app);	//send it to the application, not the window
	FileM->AddItem(NewMItem);


	FileM->AddSeparatorItem();

	NewMItem = new BMenuItem(_("MENU_ITEM_QUIT","Quit"), 
		new BMessage(CLOSE_MITEM), 'Q');
	FileM->AddItem(NewMItem);


	bm->AddItem(FileM);


	/*
	 * Add the edit menu
	 */
	EditM = new BMenu(_("MENU_EDIT","Edit"));

	NewMItem = new BMenuItem(_("MENU_ITEM_UNDO","Undo"), 
		new BMessage(UNDO_MITEM), 'Z');

	EditM->AddItem(NewMItem);

	bm->AddItem(EditM);

	/*
	 * Add the Game menu
	 */
	GameM = new BMenu(_("MENU_GAME","Game"));

	NewMItem = new BMenuItem(_("MENU_ITEM_SUGGEST_MOVE","Suggest Move"), 
		new BMessage(SUGGEST_MITEM), 'G');

	GameM->AddItem(NewMItem);



	/*
	 * Create the size submenu
	 */

	SizeM = new BMenu(_("MENU_ITEM_SIZE","Size"));

	NewMItem = new BMenuItem("14 x 6", new BMessage(SIZE_14x6_MITEM));
	SizeM->AddItem(NewMItem);

	NewMItem = new BMenuItem("18 x 8", new BMessage(SIZE_18x8_MITEM));
	NewMItem->SetMarked(true);
	GameSize = SIZE_18x8_MITEM;	
	SizeM->AddItem(NewMItem);

	NewMItem = new BMenuItem("24 x 12", new BMessage(SIZE_24x12_MITEM));
	SizeM->AddItem(NewMItem);

	NewMItem = new BMenuItem("28 x 16", new BMessage(SIZE_28x16_MITEM));
	SizeM->AddItem(NewMItem);

	NewMItem = new BMenuItem("32 x 20", new BMessage(SIZE_32x20_MITEM));
	SizeM->AddItem(NewMItem);


	GameM->AddItem(SizeM);

	GameM->AddSeparatorItem();

	NewMItem = new BMenuItem(_("MENU_ITEM_HALL_OF_FAME","Hall of Fame"), 
		new BMessage(HALL_MITEM), 'H');
	GameM->AddItem(NewMItem);
	
	SoundMItem = new BMenuItem(_("MENU_ITEM_SOUND","Sound"), 
		new BMessage(SOUND_MITEM));
	GameM->AddItem(SoundMItem);
	
	NewMItem = new BMenuItem(_("MENU_ITEM_PREFERENCES","Preferences"), 
		new BMessage(PREFERENCES_MITEM));
	GameM->AddItem(NewMItem);
	
	NewMItem = new BMenuItem(_("MENU_ITEM_PAUSE","Pause"), 
		new BMessage(PAUSE_MITEM), 'A');
	GameM->AddItem(NewMItem);

	bm->AddItem(GameM);

#ifdef MULTI_TILE
	// tile menu
	TileM = new BMenu(_("MENU_TILES","Tiles"));
	TileM->SetRadioMode(true);
	NewMItem = new BMenuItem(_("MENU_ITEM_STANDARD","Standard"), 
		new BMessage(STANDARD_MITEM));
	NewMItem->SetMarked(true);
	TileM->AddItem(NewMItem);
	TileM->AddSeparatorItem();
	// call function to scan for tiles...
	tiles.MakeTileMenu(TileM);
	bm->AddItem(TileM);
#endif

	BackGround->AddChild(bm);

	srandom( (time(NULL) >> 4) & 0xffff );


	BRect r = BScreen(this).Frame();
		
	// for people w/ screen envy...
	if (r.Width() < 800)
	{
			h = 6;
			w = 14;
			nTiles = 21;	// +++ KWS why didn't I do it before? 
			NewGameSize(6, 14);
	}
	else
	{
		h = 8;		//default sizes
		w = 18;
		nTiles = 36; // +++ KWS
		NewGameSize(8, 18);
	}

	EnactPrefs();

	Pause = 0;
	ActiveGame = 1;
	Time = 0;

	NewGame(random());	

	enable_undo(false); // disable it

/*
 * load the sound from the resource
 *
 */
	const void *soundptr;
	size_t outs;
	// frame_rate, channel count, format, byte order, buffer size 
	media_raw_audio_format fmt = {44100/2, 1, 2, 2, 8192};
	bool SoundError = false;

//	Pistooli: I am too lazy now to look up SOUND_OK_ID (sound put into Resource File)	
//	soundptr = (be_app->AppResources())->LoadResource(B_RAW_TYPE, SOUND_OK_ID, &outs);
	soundptr = (be_app->AppResources())->LoadResource(B_RAW_TYPE, 1, &outs);
	
	if (soundptr)
	{
		sound_ok = new BSound((void *)soundptr, outs, fmt);

		if (sound_ok->InitCheck() != B_OK)
		{
			SoundError = true;
		}
	}

//	Pistooli: I am too lazy now to look up SOUND_BAD_ID (sound put into Resource File)	
//	soundptr = (be_app->AppResources())->LoadResource(B_RAW_TYPE, SOUND_BAD_ID, &outs);
	soundptr = (be_app->AppResources())->LoadResource(B_RAW_TYPE, 2, &outs);
	
	if (soundptr)
	{
		sound_bad = new BSound((void *)soundptr, outs, fmt);

		if (sound_bad->InitCheck() != B_OK)
		{
			SoundError = true;
		}
	}
	
	
	if (SoundError == false)
	{
		MakeNoise = true;
		SoundMItem->SetMarked(true);
		player = new BSoundPlayer(&fmt);
		//player = new BSoundPlayer();
		player->Start();
	}
	else
	{
		MakeNoise = false;
		player = NULL;
		SoundMItem->SetEnabled(false);
	}
	

// spawn the timer thread

	timer_thread = spawn_thread(Thread_init, "BShisen Timer", 
		B_LOW_PRIORITY, this);

	//start the timer thread
	resume_thread(timer_thread);
}

GameWindow::~GameWindow()
{
	delete_sem(tile_sem);
	delete_sem(timer_sem);
	
	if (BackBitmap) delete BackBitmap;
	
	if (player) delete player;
	
//	kill_thread(timer_thread);

	#if 0	// these are deleted automatically....
	if (controls) delete controls;
	if (hall) delete hall;
	if (prefs) delete prefs;
	#endif
}

void GameWindow::PairRemoved(void)
{
	if (!MakeNoise) return;
	
	if (CustomConnect && CustomConnectSound)
	{
		CustomConnectSound->StartPlaying();
	}
	else if (player)
	{
		player->StartPlaying(sound_ok);
	}
}

void GameWindow::match_failed(void)
{
	if (!MakeNoise) return;
	
	if (CustomNoConnect && CustomNoConnectSound)
	{
		CustomNoConnectSound->StartPlaying();
	}
	else if (player)
	{
		player->StartPlaying(sound_bad);
	}

}


void GameWindow::EnactPrefs(void)
{

BBitmap *pic;
char const *cp;
entry_ref ref;

	pic = NULL;
	cp = prefs->GetPicPath();
	
	if (BackBitmapPath != cp)
	{
		BackBitmapPath = cp;
		
		if (cp && *cp)
			pic = BTranslationUtils::GetBitmap(cp);

		if (pic)
		{
			delete BackBitmap;
			BackBitmap = pic;
		}
		else
		{
			delete BackBitmap;
			BackBitmap = NULL;
			prefs->SetPicPath("");
			prefs->WritePrefs();
		}
	}	
	BackGround->SetViewColor(prefs->GetBackColor());
	FixBitmap();
	BackGround->Invalidate();

	// custom sounds 
	cp = prefs->GetConnectSound();
	if (cp && *cp)
	{
		BMediaFile *f;
		BEntry entry(cp, true);	
	
		if (entry.InitCheck() == B_OK && entry.Exists() && entry.GetRef(&ref) == B_OK)
		{
			if (CustomConnectSound)
			{
				CustomConnectSound->StopPlaying();
				delete CustomConnectSound;
				CustomConnectSound = NULL;
			}
			f = new BMediaFile(&ref);
			if (f->InitCheck() == B_NO_ERROR)
			{
				delete f;
				CustomConnectSound = new BSimpleGameSound(&ref);
				if (CustomConnectSound->InitCheck() != B_OK)
				{
					prefs->SetCustomConnect(false);
					prefs->SetConnectSound(NULL);
					delete CustomConnectSound;
					CustomConnectSound = NULL;
				}
			}
			else
			{
				delete f;
				prefs->SetConnectSound(NULL);
				prefs->SetCustomConnect(false);
			}		
		}
		else
		{
			prefs->SetConnectSound(NULL);
			prefs->SetCustomConnect(false);
		}
	}
	else prefs->SetCustomConnect(false);
	
	cp = prefs->GetNoConnectSound();
	if (cp && *cp)
	{
		BEntry entry(cp, true);	
		BMediaFile *f;
		
		if (entry.InitCheck() == B_OK && entry.Exists() && entry.GetRef(&ref) == B_OK)
		{
			if (CustomNoConnectSound)
			{
				CustomNoConnectSound->StopPlaying();
				delete CustomNoConnectSound;
				CustomNoConnectSound = NULL;
			}
			
			f = new BMediaFile(&ref);
			
			if (f->InitCheck() == B_NO_ERROR)
			{
				delete f;
				CustomNoConnectSound = new BSimpleGameSound(&ref);
				if (CustomNoConnectSound->InitCheck() != B_OK)
				{
					prefs->SetCustomNoConnect(false);
					prefs->SetNoConnectSound(NULL);
					delete CustomNoConnectSound;
					CustomNoConnectSound = NULL;
				}
			}
			else
			{
				delete f;
				prefs->SetNoConnectSound(NULL);
				prefs->SetCustomNoConnect(false);			
			}
		}
		else
		{
			prefs->SetNoConnectSound(NULL);
			prefs->SetCustomNoConnect(false);
		}
	}
	else prefs->SetCustomNoConnect(false);
	
		
	CustomConnect = prefs->GetCustomConnect();
	CustomNoConnect	= prefs->GetCustomNoConnect();
}




void GameWindow::NewGameSize(int h, int w)
{
	//resize the game window 

	int width = (w + 2) * (TILE_WIDTH+2);
	int height = (h + 2) * (TILE_HEIGHT+2) + CONTENT_H;
	ResizeTo(width, height);

	MoveStrings();
}

void GameWindow::DoPause(void)
{
//PauseWindow *P;
BAlert *B;

	if (!ActiveGame) return;

	//+Pause++;
	//P = new PauseWindow();
	//P->Go();	//class, delete thyself

	B = new BAlert("", "Game is paused", "Continue");
	B->Go(new BInvoker(new BMessage('nopa'), this));
	//Pause--;


#if 0
BAlert *b;

	if (!ActiveGame) return;

	Pause++;
	b = new BAlert("", "Game is paused.", "Continue");
//	b->SetLook(B_TITLED_WINDOW_LOOK);
//	b->SetTitle("Paused");
	b->Go();
	Pause--;
#endif
}

int GameWindow::CloseGame(void)
{
BAlert *b;
int32 button;

	//+Pause++;

	b = new BAlert("", "Close current game?", "Yes", "No");
	b->SetShortcut(0, 'y');
	b->SetShortcut(1, 'n');
	button = b->Go();
	/*
	 * 0 = Yes
	 * 1 = No
	 *
	 */	

	//+Pause--;
	return (!button);
}

void GameWindow::ChangeGameSize(unsigned new_size)
{

	if (GameSize == new_size) return;

	Hide();

	//if (!REGISTERED)
	//{
	//	if (new_size == SIZE_14x6_MITEM || new_size == SIZE_18x8_MITEM)
	//	{
	//	}
	//	else
	//	{
	//		BAlert *b;
	//	
	//		b = new BAlert("", "This board size is not available to "
	//							"unregistered users.  If you enjoy "
	//							"BShisen, please consider registering.",
	//							"Ok");
	//		b->Go();
	//		Show();
	//		return;
	//	}
	//}	




	if (CloseGame())
	{
	
	
		(SizeM->FindItem(GameSize))->SetMarked(false);
		(SizeM->FindItem(new_size))->SetMarked(true);
		GameSize = new_size;

		switch (GameSize)
		{
		case SIZE_14x6_MITEM:
			w = 14;
			h = 6;
			nTiles = 21;
			break; 

		case SIZE_18x8_MITEM:
			w = 18;
			h = 8;
			nTiles = 36;
			break;

		case SIZE_24x12_MITEM:
			w = 24;
			h = 12;
			nTiles = 36;
			break;

		case SIZE_28x16_MITEM:
			w = 28;
			h = 16;
			nTiles = 28;
			break;

		case SIZE_32x20_MITEM:
			w = 32;
			h = 20;
			nTiles = 32;
			break;
		}

		NewGameSize(h, w);	// resize the window
		NewGame(key);		// start the new game

		if (BackBitmap) FixBitmap();	// move pic to center, if needed

	}
	while (IsHidden()) Show();


}

void GameWindow::Minimize(bool m)
{
	if (m) Pause++;
	else Pause = 0;
	//else if(Pause)
	//{
	//	Pause--;
	//}
	BWindow::Minimize(m);
}


void GameWindow::Hide(void)
{
	Pause++;
	BWindow::Hide();
}

void GameWindow::Show(void)
{

	if (Pause) Pause--;
	//Pause = 0;
	BWindow::Show();
}

void GameWindow::WorkspaceActivated(int32, bool active)
{

	if (active)
	{
		if (Pause) Pause--;
	}
	else Pause++;
}

void GameWindow::WindowActivated(bool active)
{
	if (active) Pause = 0;
}


void GameWindow::WorkspacesChanged(uint32 oldworkspace, uint32 newworkspace)
{
uint32 w;

	w = current_workspace();	// get current workspace
	if (w == 0) w = 1;
	else w = 2 << (w-1);


	if (newworkspace == w)
	{
		if (Pause) Pause--;
	}
	else if (oldworkspace == w)
		Pause++;

	BWindow::WorkspacesChanged(oldworkspace, newworkspace);
}



void GameWindow::FixBitmap(void)
{

	BackGround->ClearViewBitmap();
	
	if (!BackBitmap)
	{
		BackGround->Invalidate();	//force a redraw
		return;
	}

BRect backR = BackGround->Bounds();
BRect destR = BackBitmap->Bounds();		// don't do any weird scaling
			
			
			// why, the first time, does this not center ???
			if (!prefs->GetTilePic())
			{
				if (destR.Height() < backR.Height())
				{
					float tmp = backR.Height() - destR.Height();
					/*if (tmp > 38)*/ tmp /= 2;
					destR.OffsetBy(0, MAX(tmp, 19));
				}
				else destR.OffsetBy(0,19);	// account for menu bar
	
				if (destR.Width() < backR.Width())
				{
					float tmp = backR.Width() - destR.Width();
					tmp /= 2;
					destR.OffsetBy(tmp, 0);
				}				
				
				BackGround->SetViewBitmap(BackBitmap, 
					BackBitmap->Bounds(), destR, B_FOLLOW_NONE, 0);
		}
		else
		{
			destR.OffsetBy(0, 19);	//account for menu bar
			BackGround->SetViewBitmap(BackBitmap, BackBitmap->Bounds(), destR);	//tiled version
		}	
		BackGround->Invalidate();	//force a redraw
}







#include <stdio.h>

void GameWindow::MessageReceived(BMessage *Bm)
{

bool was_suggest = false;

	//acquire_sem(lock_sem);

	/*
	 * Make sure the timer thread isn't dead :)
	 *
	 */
	thread_info * ti = new thread_info;
	status_t status;
	status = get_thread_info(timer_thread ,ti);

	if (status != B_NO_ERROR)
	{
		(new BAlert("", "Please don't delete my threads.", 
			"Sorry", NULL, NULL, B_WIDTH_AS_USUAL,
			B_STOP_ALERT))->Go();

		Lock();
		__forceQuit = true;
		be_app->PostMessage(new BMessage(B_QUIT_REQUESTED));
		Unlock();
	}

	delete ti;




	rgb_color *color;
	ssize_t size;
	
	if ( Bm->FindData("RGBColor", B_RGB_COLOR_TYPE,
		(const void **)&color, &size) == B_OK)
	{
		prefs->SetBackColor(*color);
		EnactPrefs();
		//BackGround->SetViewColor(*color);
		//BackGround->Invalidate();
	
	}




	switch(Bm->what)
	{
	case '_re_':
		prefs->SetKey(KEY);
		prefs->WritePrefs();
		REGISTERED = true;
		break;
	case 'nopa':
		{
			int32 gno;
			
			if (Bm->FindInt32("Game Number", &gno) == B_OK)
			{
				//printf("foo\n");
				NewGame(gno);
				
			}
			else if (Bm->FindInt32("Prefs", &gno) == B_OK)
			{
				prefs->WritePrefs();
				EnactPrefs();
			}
		}
		//Pause = 0;
		//printf("Done pausing!\n");
		while (IsHidden()) Show();
		//Show();
		break;
	case B_KEY_DOWN:
	{
		/*
		 * if escape was pressed, deselect any tiles
		 *
		 */
		int32 tkey = 0;
		
		Bm->FindInt32("raw_char", &tkey);
		if (tkey == B_ESCAPE)
			select(0,0);
	}
	break;

	/*
	 * User has dropped file into our window,
	 * so changeth background to that pic
	 */	
	case B_SIMPLE_DATA:
	{
	entry_ref       ref;            // The entry_ref to open
	BEntry *entry;
	BPath *path;
	BBitmap *pic;


		if (Bm->FindRef("refs", 0, &ref)!= B_OK)
			break;
		// ref is the ref for the file.

		entry = new BEntry(&ref);
		path = new BPath(entry);
		pic = NULL;
		pic = BTranslationUtils::GetBitmap(path->Path());
		delete entry;
		
		if (pic)
		{
			BackBitmapPath = path->Path();
			prefs->SetPicPath(path->Path());
			prefs->WritePrefs();
			if (BackBitmap) delete BackBitmap;
			BackBitmap = pic;
			
			FixBitmap();			
		}
		delete path;
	}
	break;
		
	//case B_SET_PROPERTY:	// color dragged to my window:
	//case 'PSTE':			// why oh why???
		
	
#if 0
		static char buffer[5000];
		ssize_t spl = Bm->FlattenedSize();
		Bm->Flatten(&buffer[0], spl);
	
		write(1, buffer, spl);
#endif
	

		
	// do a new game w/o asking
	case FORCE_NEW:
		Hide();
		NewGame(0);
		while (IsHidden()) Show();
		//Pause = 0;
		break;

	case FORCE_QUIT:
		Lock();
		__forceQuit = true;
		be_app->PostMessage(new BMessage(B_QUIT_REQUESTED));
		Unlock();
		break;

	case FORCE_RESTART:
		Hide();
		new_game(h, w, key, nTiles);
		while (IsHidden()) Show();
		break;
		
	case SOUND_MITEM:
		MakeNoise = (!MakeNoise);
		SoundMItem->SetMarked(MakeNoise);
		break;

	case PREFERENCES_MITEM:
	/*
	 * Tell me why baby, why baby, why baby, why
	 * You make me cry baby, cry baby, cry baby, cry
	 *
	 * - George Jones
	 *
	 * For some idiotic reason, BWindows created in a different
	 * thread won't Show() from this thread (can't lock)
	 *
	 */
		Hide();
		prefs->Go(this);
		#if 0
		if (prefs->Go())
		{
			prefs->WritePrefs();
			EnactPrefs();
		}
		Show();
		#endif
		break;


	case NEW_MITEM:
		Hide();
		if (CloseGame()) NewGame(0);
		while (IsHidden()) Show();
		break;
	case RESTART_MITEM:
		Hide();
		if (CloseGame())
		{
			new_game(h, w, key, nTiles);
			Show();
			//Pause = 0;
		}
		while (IsHidden()) Show();
		break;

	case CLOSE_MITEM:

		be_app->PostMessage(new BMessage(B_QUIT_REQUESTED));
		break;

	case UNDO_MITEM:
		undo();
		break;


	case NEWBYID_MITEM:
		Hide();
		if (CloseGame())
		{
		ID_Win *Id;
		int g;

			//Pause++;
			Hide();	
			
	
			//	g = (time(NULL) ^ (rand()<<16)) ^ rand();
			srandom( (time(NULL) >> 4) & 0xffff );
			g = random();
			Id = new ID_Win(this, g);
			//g = Id->Go(g);
			//Id->Lock();
			//Id->Quit();

			//if (g) NewGame(g);

			//if (Pause) Pause--;
		}
		else Show();
		//while (IsHidden()) Show();
		
		break;

	case SUGGEST_MITEM:
		QueueSuggest(true);
		was_suggest = true;
		break;


	case SIZE_14x6_MITEM:
	case SIZE_18x8_MITEM:
	case SIZE_24x12_MITEM:
	case SIZE_28x16_MITEM:
	case SIZE_32x20_MITEM:
		ChangeGameSize(Bm->what);
		break;

	case HALL_MITEM:
		Hide();
		hall->ShowScores(new BMessage('nopa'), this);
		//Show();
		break;

	case PAUSE_MITEM:
		Hide();
		DoPause();
		//Show();
		break;	


	case TILE_CTRL:
	{

		uint32 _x = Bm->FindInt32("x_pos");
		uint32 _y = Bm->FindInt32("y_pos");
		//uint32 _t = Bm->FindInt32("tile");
		uint32 _b = Bm->FindInt32("mouse_buttons");

		if (_b == B_SECONDARY_MOUSE_BUTTON) ;
			//fprintf(stderr, "2nd mouse button pressed!\n");
		
		//
		//  spawn a thread to do the dirty work.
		QueueTiles(_x, _y);
	}
	/*
	 * Check if any moves are left
	 *
	 */
	//if (remaining() > 2) suggest4(false);//QueueSuggest(false);
	if (remaining() > 2) QueueSuggest(false);
	
	break;


	case HILITE_TILE:
		{
		BitmapControl *BC;
		int32 X, Y, mode;
				

			if (Bm->FindInt32("X", &X) == B_OK
				&& Bm->FindInt32("Y", &Y) == B_OK
				&& Bm->FindInt32("mode", &mode) == B_OK)
			{	

				BC = controls->GetData(X - 1, Y - 1);
				if (BC)
					BC->SetValue(mode);	
			}
		}
		break;
	
	case ERASE_TILE:
		{
		BitmapControl *BC;
		int32 X, Y;		
			
			if (Bm->FindInt32("X", &X) == B_OK
				&& Bm->FindInt32("Y", &Y) == B_OK)
			{
				BC = controls->GetData(X - 1, Y - 1);
				if (BC)
				{
					BC->Hide();
					BC->RemoveSelf();
					delete BC;
					controls->SetData(NULL, X - 1, Y - 1);
				} 	
			}
	
		}	
		break;




	default:
		#if 0
		{
			unsigned int code[2];
			code[0] = Bm->what;
			code[1] = 0;
			printf("code is %s\n", code);
		}
		#endif
		BWindow::MessageReceived(Bm);
	}


	//release_sem(lock_sem);
}

void GameWindow::NewGame(int game_no)
{
static char gstr[64];

	if (!game_no) game_no = random();

	key = game_no;


	sprintf(gstr, "Game Number: %d", key); 
	if (Number_String) Number_String->SetText(gstr);

	new_game(h, w, key, nTiles);
}

/*
 *  overloaded new_game to destroy any old tiles first
 *  reset clock, etc.
 *
 *
 */
void GameWindow::new_game(int h, int w, int key, int tiles)
{

	/* remove any past data  && remove any previous icons*/
        if (controls)
        {
            BitmapControl *BC;
            unsigned int x;
			unsigned int y;

				Lock();
                for (x = 0; x < controls->X(); x++)
                        for (y = 0; y < controls->Y(); y++)
                        {
                                BC = controls->GetData(x, y);
                                if (BC)
                                {
                                       
                                        BC->Hide();
                                        BC->RemoveSelf();
                               
                                        delete BC;
                                }
                        }
			Unlock();                      
			delete controls;
        }
        controls = new array2d<BitmapControl *>(w , h);

	board::new_game(h, w, key, tiles);

	ActiveGame = 1;
	Pause = 0;
	Time = 0;
	DrawTime();
}

bool GameWindow::QuitRequested(void)
{
	//printf("GW->QR\n");
	Hide();
	if (__forceQuit || CloseGame())
	{
		BAlert *b;
		// kill the timer thread
		kill_thread(timer_thread);	

		//Lock();
		if (!REGISTERED)
		{
			b = new BAlert("", "We hope you had fun playing BShisen.  "
							"If you did, please consider "
							"registering (It will get rid of these "
							"annoying messages!).",
							"Ok");
			b->Go();
		}
		be_app->PostMessage(new BMessage(B_QUIT_REQUESTED));
		return true;
	}
	//while (IsHidden()) Show();
	Show();
	return false;
}

void GameWindow::enable_undo(bool enable)
{
	(EditM->FindItem(UNDO_MITEM))->SetEnabled(enable);
}

void GameWindow::delay(void)
{
	Pause++;
	snooze(250000);	// wait 1/4 sec
	if (Pause) Pause--;
}

int32 GameWindow::Thread_init(void *data)
{
	return ((GameWindow *)data)->Timer();
}

int32 GameWindow::Timer(void)
{
	while (1)
	{
		snooze(1000000);	// 1 sec
		if (!Pause && ActiveGame)
		{
			acquire_sem(timer_sem);
			Time++;
			DrawTime();
			release_sem(timer_sem);
		}
	}
	return 0;
}
void GameWindow::DrawTime(void)
{
static char buffer[256];

unsigned int Temp;
int hours;
int mins;
int secs;

	Temp = Time;

	secs = Temp % 60;
	Temp /= 60;

	mins = Temp % 60;
	Temp /= 60;

	hours = Temp;

	sprintf(buffer, "Time: %.2d:%.2d:%.2d", hours, mins, secs);
	Lock();
	if (Time_String) Time_String->SetText(buffer);
	Unlock();
}

void GameWindow::MoveStrings(void)
{
BRect bounds = Bounds();
BRect rect;

static char gstr[64];

	Lock();

	if (Time_String)
	{
		Time_String->RemoveSelf();
		delete Time_String;
	}
	if (Number_String)
	{
		Number_String->RemoveSelf();
		delete Number_String;
	}
	if (Remain_String)
	{
		Remain_String->RemoveSelf();
		delete Remain_String;
	}
	if (BackGround2)
	{
		BackGround2->RemoveSelf();
		delete BackGround2;
	}

	rect = Bounds();
	
	rect.top = rect.bottom - 25;

	BackGround2 = new BView(rect, "", B_FOLLOW_ALL, B_WILL_DRAW);
	BackGround2->SetViewColor(216, 216, 216);//(prefs->GetBackColor());
	
	BackGround->AddChild(BackGround2);

	bounds = BackGround2->Bounds();

	 rect.left = bounds.left + 10;
	 rect.bottom = bounds.bottom - 5;
	 rect.top = rect.bottom - 20;
	 rect.right = rect.left + 150;
	
	Time_String = new BStringView(rect, "", "0"); 
	Time_String->SetFont(be_bold_font);
	Time_String->SetFontSize(12);

	BackGround2->AddChild(Time_String);

	DrawTime();




	float center;
	center =  bounds.right - bounds.left;
	center /= 2;
	
	rect.right =  center + 100;
	rect.left = center - 100;

	sprintf(gstr, "%d", key);

	Number_String = new BStringView(rect, "", gstr);
	Number_String->SetAlignment(B_ALIGN_CENTER);
	Number_String->SetFont(be_bold_font);
	Number_String->SetFontSize(12);
	BackGround2->AddChild(Number_String);



	// rect.bottom & top still ok
	rect.right = bounds.right - 10;
	rect.left = bounds.right - 150;

	Remain_String = new BStringView(rect, "", "");
	Remain_String->SetFont(be_bold_font);
	Remain_String->SetFontSize(12);
	Remain_String->SetAlignment(B_ALIGN_RIGHT);
	BackGround2->AddChild(Remain_String);

	Unlock();
}



/*
 * Overloaded board functions to do stuff
 *
 *
 */

void GameWindow::draw_tile(int height, int width, int type) 
{
BitmapControl *BC;
BBitmap *BM;
BRect rect;
BMessage * mesg;

	rect.left = width * TILE_WIDTH + width * 2;
	rect.top = height * TILE_HEIGHT + height *2;

	rect.right = rect.left + TILE_WIDTH -1;
	rect.bottom = rect.top + TILE_HEIGHT -1;

	BM = tiles.GetTile(type - 1);	

	mesg = new BMessage (TILE_CTRL);
	mesg->AddInt32("x_pos", width);
	mesg->AddInt32("y_pos", height);
	mesg->AddInt32("tile", type);
	mesg->AddInt32("mouse_buttons", 0);

	BC = new BitmapControl(rect, BM, NULL, NULL, mesg);
	BC->SetViewColor(100, 100, 100);
	BackGround->AddChild(BC);

	controls->SetData(BC, width-1, height-1);
}


void GameWindow::hilite(int height, int width)
{
BMessage *msg;

	msg = new BMessage(HILITE_TILE);
	
	msg->AddInt32("X", width);
	msg->AddInt32("Y", height);
	msg->AddInt32("mode", 1);
	PostMessage(msg);
	
	delete msg;

#if 0
BitmapControl *BC;

	acquire_sem(fuck_sem);
	BC = controls->GetData(width-1, height -1);
	if (BC)
	{
		Lock();
		BC->SetValue(1);
		Unlock();
	}
	release_sem(fuck_sem);
#endif
}

/*
 * Function to unhilite a tile
 *
 */
void GameWindow::unhilite(int height, int width)
{
BMessage *msg;

	msg = new BMessage(HILITE_TILE);
	msg->AddInt32("X", width);
	msg->AddInt32("Y", height);
	msg->AddInt32("mode", 0);

	PostMessage(msg);
	delete msg;

}

void GameWindow::erase_tile(int height, int width)
{
BMessage *msg;

	msg = new BMessage(ERASE_TILE);
	msg->AddInt32("X", width);
	msg->AddInt32("Y", height);
	
	PostMessage(msg);
	delete msg;
}


void GameWindow::TileCount(unsigned int new_count)
{
static	char buffer[64];

// Pistooli: Cosmetic change ('Tiles Remaining' changed to 'Tiles Left') for better visual
	sprintf(buffer, "Tiles Left: %d", new_count);

	Lock();
	if (Remain_String)
	{
		Remain_String->SetText(buffer);
		Remain_String->Invalidate();
	}
	Unlock();

	//if (!Remain_String) fprintf(stderr, "Remain_String missing!\n");
}


/*
 * Callback called after the frame size has changed.
 *
 */
void GameWindow::FrameResized(float width, float height)
{
}

void GameWindow::DrawLine(rRect *r)
{

BRect br;

BView *V;

	br.left = r->x1() * TILE_WIDTH + (r->x1() * 2) - 2 + TILE_WIDTH/2;
	br.right = r->x2() * TILE_WIDTH + (r->x2() * 2) + 2 + TILE_WIDTH/2;
	br.top = r->y1() * TILE_HEIGHT + (r->y1() * 2) - 2 + TILE_HEIGHT/2;
	br.bottom = r->y2() * TILE_HEIGHT + (r->y2() * 2) + 2 + TILE_HEIGHT/2;


	Lock();

	V = new BView(br, "", B_FOLLOW_NONE, B_WILL_DRAW); 
//	V = new BControl(br, "", "", NULL, B_FOLLOW_NONE, B_WILL_DRAW);
//	V = new BBox(br);
//	V = new LineControl(br);
	V->SetViewColor(255, 0, 0);
//	V->SetLowColor(255, 255, 255);


	if (!Line1)
	{
		Line1 = V;
//		Lock();
		BackGround->AddChild(V);
//		Unlock();
	}
	else if (!Line2)
	{
		Line2 = V;
//		Lock();
		BackGround->AddChild(V);
//		Unlock();
	}

	else if (!Line3)
	{
		Line3 = V;
		//Lock();
		BackGround->AddChild(V);
		//Unlock();
	}
	
	else
	{
		fprintf(stderr, "BShisen: Impossible condition: Too many Lines!!!!!!\n");
		exit(-1);
	}
	Unlock();

}


void GameWindow::EraseLine(rRect *r)  
{
	Lock();

	if (Line1)
	{

		Line1->Hide();
		Line1->RemoveSelf();

		delete Line1;
	}
	if (Line2)
	{
		Line2->Hide();
		Line2->RemoveSelf();
		delete Line2;
	}
	if (Line3)
	{
		Line3->Hide();
		Line3->RemoveSelf();
		delete Line3;
	}
	Line1 = Line2 = Line3 = NULL;
	Unlock();
}

int32 GameWindow::connect2(void * data)
{
	((GameWindow *)data)->connect3();
	return 1;
}

// glue code;
//
// this is run in it's own separate thread every time.
//
void GameWindow::connect3(void)
{
thread_id foo;

int X;
int Y;
int32 tmp;

	//get my x/y coords...
	tmp = receive_data(&foo, NULL, 0);

	Y = tmp & 0xffff;
	X = (tmp & 0xffff0000) >> 16;
	
	//fprintf(stderr, "x = %d, y = %d\n", X, Y);
	
	//acquire_sem(lock_sem);
	acquire_sem(tile_sem);
	select(Y, X);	
	release_sem(tile_sem);
	//release_sem(lock_sem);	

	exit_thread(0);
}

/*
 *
 *
 */
void GameWindow::QueueTiles(int x, int y)
{
thread_id ti = -1;

	ti = spawn_thread(connect2, "Hey there", B_NORMAL_PRIORITY, this);
	
	if (ti == -1)
	{
		fprintf(stderr, "BShisen: Error spawning thread.  You are in serious trouble\n"); 
		exit(1);
	}
	// start it up
	resume_thread(ti);
	// tell it where to look
	send_data ( ti, x << 16 | y, NULL, 0);

}

void GameWindow::QueueSuggest(bool draw)
{
thread_id ti = -1;

	ti = spawn_thread(suggest2, "Heather ain't here", 
		B_NORMAL_PRIORITY, this);
	
	if (ti == -1)
	{
		fprintf(stderr, "BShisen:  Error spawning thread.  You are in serious trouble\n");
		exit(1);
	}
	// start it up
	resume_thread(ti);
	// tell it where to look
	send_data ( ti, draw, NULL, 0);

}

int32 GameWindow::suggest2(void *data)
{
	((GameWindow *)data)->suggest3();
	return 1;

}

void GameWindow::suggest3(void)
{
thread_id foo;
int32 draw;

	draw = receive_data(&foo, NULL, 0);
	suggest4(draw);
	exit_thread(0);
}


void GameWindow::suggest4(bool draw)
{
bool val;

	//acquire_sem(lock_sem);
	acquire_sem(tile_sem);
	val = SuggestMove(draw);
	//release_sem(lock_sem);
	
	if (val && draw) //don't increment the time if it's not being drawn...
	{
		Pause++;	// be safe about it...
		acquire_sem(timer_sem);
		Time += SUGGEST_TIME;
		DrawTime();
		release_sem(timer_sem);
		if (Pause) Pause--;
	}
	/**
	 *
	 * No matches - warn user & offer to restart or start a new game
	 *
	 *
	 */
	if (!val)
	{
		Pause++;
		BAlert *al = new BAlert("", "No moves are left",
			"Okay", "Restart", "New Game", 
			B_WIDTH_AS_USUAL, B_STOP_ALERT);

		switch (al->Go())
		{
		case 0:	//okay - do nothing
			Pause--;
			break;
		case 1:	//restart
			this->PostMessage(FORCE_RESTART);
			break;
		case 2:	//new game
			this->PostMessage(FORCE_NEW);
			break;
		}
	}
	release_sem(tile_sem);		
}

void GameWindow::GameDone(void)
{
	Pause++;
	Hide();
	if (hall->JudgeScore(Time, key, h*w))
	{
		hall->ShowScores(new BMessage(FORCE_NEW), this);
		if (REGISTERED)
			hall->WriteScores();
	}
	//Show();
	else
	{
		BAlert *b;
		Show();
		b = new BAlert("", "Congratulations!  You cleared the board.",
						"Cool");
						
		b->Go();
		PostMessage(FORCE_NEW);
	}
	//this->PostMessage(FORCE_NEW);
}
