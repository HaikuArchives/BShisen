//
// Last edited by Pistooli, 22/04/2014
//
#include "Shisen.h"
#include "GameWindow.h"
#include <View.h>
#include <StringView.h>
#include <TranslationUtils.h>
#include <TranslatorFormats.h> // Pistooli: inserted for TIFF formats
#include <OS.h>
#include <Entry.h>
#include "res.h"
#include "Prefs.h"
#include <FindDirectory.h>
#include <Path.h>
#include <Roster.h>
#include "CompileDefs.h"
#ifdef USE_YLANGUAGE
#include "YLanguageClass.h"
#endif
/*
 * $Id: Shisen.cpp,v 1.3 1999/07/09 05:21:55 baron Exp baron $
 */
 
#include <stdio.h>

void BuildLanguageList(void);

int REGISTERED;
int NAG;
uint64 KEY;

union split
{
	uint64 i64;
	uint32 i32[2];
	uint16 i16[4];
	uint8 i8[8];
};

Shisen::Shisen():
	BApplication("application/x-vnd.KWS-BShisen")
{
system_info info;
union split sp;
app_info app;

#ifdef USE_YLANGUAGE
	Language.SetName("English");
	if (Language.InitCheck() != B_OK)
		printf("error loading language file (English\n");
		
	BuildLanguageList();
#endif

	REGISTERED = false;
	NAG = false;
	
	get_system_info(&info);get_system_info(&info);
	
//	if (info.id[0] && info.id[1]) // P3 ?
//	{
//		sp.i32[0] = info.id[0];
//		sp.i32[1] = info.id[1];
		KEY = sp.i64;
//	}
//	else	// make a KEY from the CPU TYPE & the speed
//	{
//		KEY = info.cpu_clock_speed;
//		KEY |= info.cpu_revision;
//	}
	
	// sanity check
	if (!KEY) KEY = 0xbebebebe;
	
	//printf("key is %lx\n", KEY);
	
	
	/*
	 * 1) Check the creation time
	 * 2) Check the last accessed time
	 * 3) subtract
	 * if > 3 days, nag++
	 */
	// check ~/config/settings/BShisen_prefs

	BPath path;
	BEntry entry;
	
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK
		&& path.Append(PREFS_FILE_NAME, true) == B_OK
		&& entry.SetTo(path.Path(), true) == B_OK)
	{
		time_t t_t = 0;
		time_t c_t;
		
		if (entry.GetCreationTime(&c_t) == B_OK)
		{
			//ok, file exists.  Check the creation date & see if it was > 7 days ago
			t_t = (time_t)real_time_clock();
			
			if (t_t > c_t)
			{
				t_t -= c_t;
				
				// t_t == # of secs between when file was created & now
				
				if (t_t > 7 * 24 * 60 * 60) NAG = true;
			}
		}
	}	



#if 0	
	if (GetAppInfo(&app) == B_OK)
	{
		BEntry entry;
		struct stat st;
		time_t t_t;
		
		entry.SetTo(&app.ref, true);
		entry.GetStat(&st);
	
		t_t = st.st_atime - st.st_ctime;

		if (t_t > 5 * 24 * 60 * 60)
			NAG = true;
	}
#endif

	game = new GameWindow;
	game->Show();	
}

Shisen::~Shisen()
{
}


void Shisen::AboutRequested(void)
{
BWindow *about = NULL;
thread_id id;


	id = find_thread("w>About BShisen");
	if (id != B_NAME_NOT_FOUND)
	{
		about = (BWindow *)LooperForThread(id);
	}

	if (about)
	{
		about->SetWorkspaces(B_CURRENT_WORKSPACE);
		about->Activate();
	}
	else
	{
		BView *back;
		BStringView *str;
		BBitmap *bmap;
		
		about = new BWindow(BRect(50, 50, 360, 190), 
			_("ABOUT_BSHISEN","About BShisen"),
			B_TITLED_WINDOW, B_NOT_ZOOMABLE | B_NOT_RESIZABLE);
		
		back = new BView(about->Bounds(), NULL, B_FOLLOW_NONE, B_WILL_DRAW);
		back->SetViewColor(0, 0, 0);	//black background
		about->AddChild(back);
	
	
		str = new BStringView(BRect(100, 10, 310, 30), NULL, "BShisen 1.5.0-alpha");
		str->SetFontSize(14);
		str->SetHighColor(0,255,0);
		back->AddChild(str);
	
		str = new BStringView(BRect(100, 35, 310, 55), NULL,
			"(c) 1999-2014 Kelvin W Sherlock"
						);
		str->SetFontSize(12);
		str->SetHighColor(0,255,0);
		back->AddChild(str);
	
		str = new BStringView(BRect(100, 60, 310, 80), NULL,
			"Maintainer: pistooli-haiku@pistooli.com");
		str->SetFontSize(10);
		str->SetHighColor(0,255,0);
		back->AddChild(str);


#if BUILD == SHAREWARE	
		if (!REGISTERED) //70, 90
		{
			str = new BStringView(BRect(30, 85, 310, 105), NULL, "UNREGISTERED - PLEASE REGISTER");
			str->SetFontSize(12);
			str->SetHighColor(255,0,0);
			back->AddChild(str);
		}
#endif

#if BUILD == BEGROOVY
	{
			str = new BStringView(BRect(100, 95, 310, 110), NULL, 
				"Get your groove on - BeGroovy");
			str->SetFontSize(12);
			str->SetHighColor(255,0,0);
			back->AddChild(str);
			
			str = new BStringView(BRect(100, 110, 310, 130), NULL, 
				"www.BeGroovy.com");
			str->SetFontSize(14);
			str->SetHighColor(255,0,0);
			back->AddChild(str);
	
	}
#endif

#if BUILD == CUSTOM
	{
			str = new BStringView(BRect(100, 95, 310, 110), NULL, 
				B_UTF8_OPEN_QUOTE "It's the same train, but it's different."B_UTF8_CLOSE_QUOTE);
			str->SetFontSize(10);
			str->SetHighColor(255,0,0);
			back->AddChild(str);
			
			str = new BStringView(BRect(100, 110, 310, 130), NULL, 
				 B_UTF8_OPEN_QUOTE "Based on 1.2pre sources (Released 2014)" B_UTF8_CLOSE_QUOTE);
			str->SetFontSize(10);
			str->SetHighColor(255,0,0);
			back->AddChild(str);
	
	}
#endif

//		Pistooli: Again too lazy to search for ABOUT_PIC_ID, put TIFF into Resource File
		bmap = BTranslationUtils::GetBitmap(B_TIFF_FORMAT, 1);		
//		bmap = BTranslationUtils::GetBitmap(B_RAW_TYPE, ABOUT_PIC_ID);
		if (bmap)
		{
			back->SetViewBitmap(bmap, B_FOLLOW_ALL, 0);
			free(bmap);
		}
		about->Flush();
		about->Show();	
	}
	/* Add window controls */
}
