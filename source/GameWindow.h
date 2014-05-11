#ifndef __GAMEWINDOW_H__
#define __GAMEWINDOW_H__

#include <Window.h>
#include <StringView.h>
#include <OS.h>
#include <File.h>
#include <Resources.h>
#include <OS.h>
#include <Bitmap.h>
#include <String.h>
#include <SoundPlayer.h>
#include <Sound.h>

#include "board.h"

#include "Tiles.h"
#include "array2d.cpp"			//templated 2d array support
#include "Hall.h"
#include "Prefs.h"


class BMenuItem;
class BSimpleGameSound;
class BitmapControl;

const int SUGGEST_TIME = 30;		// suggest move adds 30 secs

enum
{ 	
	/* Menu items */
	NEW_MITEM	= '_new', 
	RESTART_MITEM	= 'rstr', 
	NEWBYID_MITEM	= 'nbid', 
	CLOSE_MITEM	= 'clse', 

	UNDO_MITEM	= 'undo',

	SUGGEST_MITEM	= 'sgst',

	SIZE_14x6_MITEM	= '14_6',
	SIZE_18x8_MITEM	= '18_8',
	SIZE_24x12_MITEM = '2412',
	SIZE_28x16_MITEM = '2816',
	SIZE_32x20_MITEM = '3220',

	HALL_MITEM	= 'hall',
	PAUSE_MITEM	= 'paus',

	PREFERENCES_MITEM	= 'pref',
	SOUND_MITEM			= 'snd_',
	
	STANDARD_MITEM = 'stdt',
	TILES_MITEM = 'elit',
	
	
	TILE_CTRL	= 'tile',


	//forced messages

	FORCE_RESTART	= 'frst',
	FORCE_NEW	= 'fnew',
	FORCE_QUIT	= 'fqui',
	
	
	/* functions posted so i (hopefully) wont' crash */
	HILITE_TILE		= 'htil',
	ERASE_TILE		= 'erti'

};

#define ID_BUTTON_CANCEL	1
#define ID_BUTTON_ACCEPT	2
#define ID_FIELD_TEXT		3


// size of tile data
const int TILE_LENGTH	= 12288;
const int TILE_HEIGHT	= 64;
const int TILE_WIDTH	= 48;
const int MENU_H	= 20;
const int CONTENT_H	= 20;





/*
 * Class for main playing window
 *
 *
 *
 *
 */

class GameWindow: public BWindow, public board
{
public:
	GameWindow();
	~GameWindow();
void	DoPause(void);
int	CloseGame(void);

void	ChangeGameSize(unsigned);
void	NewGame(int);
void	NewGameSize(int h, int w);

void	EnactPrefs(void);	//puts the preference into effect


virtual void Minimize(bool);
virtual void Hide(void);
virtual void Show(void);
virtual void WorkspaceActivated(int32, bool);
virtual void MessageReceived(BMessage *);
//virtual void RefsReceived(BMessage *message);
virtual bool QuitRequested(void);
virtual void WindowActivated(bool);
virtual void WorkspacesChanged(uint32, uint32);
virtual void FrameResized(float width, float height);




/*
 * overloaded board functions
 *
 */ 
virtual void new_game(int height, int width, int key, int num_tiles = 36);
virtual void enable_undo(bool);
virtual void delay(void);
virtual void draw_tile(int height, int width, int type); 
virtual void hilite(int height, int width);
virtual void unhilite(int height, int width); 
virtual void erase_tile(int height, int width);
virtual void TileCount(unsigned int new_count); 
virtual void DrawLine(rRect *r);
virtual void EraseLine(rRect *r);  
virtual void GameDone(void);
virtual void PairRemoved(void);
virtual void match_failed(void);

private:
BSoundPlayer *player;
BSound *sound_ok;
BSound *sound_bad;
int MakeNoise;

BSimpleGameSound *CustomConnectSound;
BSimpleGameSound *CustomNoConnectSound;
bool CustomConnect;
bool CustomNoConnect;


BMenuItem *SoundMItem;

int	Pause;
int	ActiveGame;

int	key;
int	h;
int	w;

long Time;

Hall *hall;
Prefs *prefs;

array2d<BitmapControl *> *controls;

BView *BackGround;
BView *BackGround2;
BBitmap *BackBitmap;
BString BackBitmapPath;

BMenu *FileM;
BMenu *EditM;
BMenu *GameM;
BMenu *SizeM;
BMenu *TileM;


Tiles tiles;		// class which provides the BBitmaps for the tiles



BStringView *Time_String;	// string that contains the time
BStringView *Number_String;	// string that contains the game #
BStringView *Remain_String;	// string containing # of tiles left

BView * Line1;
BView * Line2;
BView * Line3;


/*
 * Moves the above strings when the window changes size
 */
void MoveStrings(void);

void FixBitmap(void);		//fix/resize bitmap pic


int GameSize;

thread_id	connect_thread;
static int32	connect2(void *);
void		connect3(void);
void		QueueTiles(int x, int y);

/*
 * Glue code to run SuggestMove in it's own thread.
 *
 */
void		QueueSuggest(bool);
static int32	suggest2(void *);
void		suggest3(void);
void		suggest4(bool);




sem_id tile_sem;
sem_id timer_sem;

thread_id	timer_thread;
static int32	Thread_init(void *);
int32		Timer(void);

bool		__forceQuit;

void DrawTime(void);


int nTiles;	// # of kinds of tiles in current
		// board size
};

#endif
