//
// Last edited on 22/04/2014 by Pistooli
//
#include "Tiles.h"
#include <TranslationUtils.h>
#include <TranslatorFormats.h> // Pistooli: inserted for PNG formats
#include <Alert.h>
#include <Application.h>


Tiles::Tiles(void):
	tiles_std(NULL),
	tiles_custom(NULL),
	use_custom(false)
	
{
register int i;

	tiles_std = new BBitmap *[36];
	tiles_custom = new BBitmap *[36];
	for (i = 0; i < 36; i++)
	{
		tiles_std[i] = NULL;
		tiles_custom[i] = NULL;
	}
#if 0
	tiles_std[0] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic1.bmp");
	tiles_std[1] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic2.bmp"); 
	tiles_std[2] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic3.bmp");
	tiles_std[3] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic4.bmp");
	tiles_std[4] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic5.bmp");
	tiles_std[5] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic6.bmp");
	tiles_std[6] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic7.bmp");
	tiles_std[7] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic8.bmp");
	tiles_std[8] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic9.bmp");
	tiles_std[9] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic10.bmp");
	tiles_std[10] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic11.bmp");
	tiles_std[11] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic12.bmp");
	tiles_std[12] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic13.bmp");
	tiles_std[13] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic14.bmp");
	tiles_std[14] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic15.bmp");
	tiles_std[15] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic16.bmp");
	tiles_std[16] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic17.bmp");
	tiles_std[17] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic18.bmp");
	tiles_std[18] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic19.bmp");
	tiles_std[19] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic20.bmp");
	tiles_std[20] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic21.bmp");
	tiles_std[21] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic22.bmp");
	tiles_std[22] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic23.bmp");
	tiles_std[23] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic24.bmp");
	tiles_std[24] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic25.bmp");
	tiles_std[25] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic26.bmp");
	tiles_std[26] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic27.bmp");
	tiles_std[27] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic28.bmp");
	tiles_std[28] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic29.bmp");
	tiles_std[29] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic30.bmp");
	tiles_std[30] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic31.bmp");
	tiles_std[31] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic32.bmp");
	tiles_std[32] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic33.bmp");
	tiles_std[33] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic34.bmp");
	tiles_std[34] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic35.bmp");
	tiles_std[35] = BTranslationUtils::GetBitmap("/boot/home/Desktop/BShisen-1.2/bmp/pic36.bmp");
	
#	endif
#if 0
	for (i = 5; i < 36; i++)
		tiles_std[i] = BTranslationUtils::GetBitmap("pic1.bmp"); 
#endif

	for (i = 0; i < 36; i++)
	{
//		Pistooli: old code deleted (resource file contained BMP - now PNG)
//		tiles_std[i] = BTranslationUtils::GetBitmap(B_RAW_TYPE, i+1);

//		Pistooli: Now code (it reads PNGs from the resource file)
		tiles_std[i] = BTranslationUtils::GetBitmap(B_PNG_FORMAT, i+1);

		
	}
		// if any are NULL, give alert & QUIT
	for (i = 0; i < 36; i++)
	{
		if (tiles_std[i] == NULL)
		{
			BAlert *b;
			b = new BAlert("", "Error: This game is corrupt. Please "
								"either reinstall or redownload.  "
								"Furthermore, stop sector editing me!",
								"Ok");
			b->Go();
			be_app->PostMessage(B_QUIT_REQUESTED);
		
		}
	
	}
	
}

Tiles::~Tiles(void)
{
register unsigned int i;

	for (i = 0; i < 36; i++)
	{
		if (tiles_std[i]) delete tiles_std[i];
		if (tiles_custom[i]) delete tiles_custom[i];
	}
	delete tiles_std;
	delete tiles_custom;
}
#include <Menu.h>
#include <MenuItem.h>
#include <Directory.h>
#include <Path.h>
#include <Application.h>
#include <Entry.h>
#include <Message.h>
#include <Roster.h>

#define TILES_MITEM 'elit'
int Tiles::MakeTileMenu(BMenu *menu)
{
//	menu->AddItem(new BMenuItem("test", NULL));

struct app_info info;
BEntry e;
BEntry e2;
BPath p;
BDirectory d;
//char name[B_FILE_NAME_LENGTH];
int cnt;
BMenuItem *m;
BMessage *msg;
	cnt = 0;
	//if (be_roster->GetActiveAppInfo(&info) != B_OK) return;
	if (be_app->GetAppInfo(&info) != B_OK) return 0;
	e.SetTo(&info.ref, true);
	e.GetParent(&e2);
	e2.GetPath(&p);	
	p.Append("Tiles");
	if (d.SetTo(p.Path()) != B_OK) return 0;
	//printf("%s\n",p.Path());


	while (d.GetNextEntry(&e) == B_OK)
	{
		cnt++;
		//e.GetName(name);
		e.GetPath(&p);
		msg = new BMessage(TILES_MITEM);
		msg->AddString("Pathname",p.Path());

		m = new BMenuItem(p.Leaf(), msg);
		menu->AddItem(m);
//		printf("%s\n", name);
	}
	return cnt;
}

#include <DataIO.h>
#include <File.h>
#include <Resources.h>
