#ifndef __HALL_H__
#define __HALL_H__

#include <Window.h>
#include <OS.h>
#include <TextControl.h>
#include <ListItem.h>

class BPath;

/*
 * Hall of fame front end class
 *
 *
 *
 */
class Hall
{
public:

	Hall();
	~Hall();
	
bool	JudgeScore(unsigned long time, unsigned long game, int tiles);	
void	ShowScores(BMessage *, BLooper *);
void	WriteScores(void);

friend class HighScores;

private:

BList *List;
BList *List1;
BList *List2;
BList *List3;
BList *List4;
BList *List5;

BWindow *Score_Window;
BPath *path;

#define FILE_NAME "BShisen.Scores"
#define NUMBER 25	// 5 for each size

struct score
{
	unsigned long gameID;
	unsigned long gameTime;
	unsigned long gameTiles;
	unsigned long absTime;	// = time(NULL)
	unsigned char Name[64];
};

bool _changed;
int _count;

};


class HighScores: public BWindow
{
public:
	HighScores( Hall&, BMessage *, BLooper *);
	~HighScores();
//void	Go(void);
virtual void MessageReceived(BMessage *);
virtual void Quit(void);
//virtual bool QuitRequested(void);

private:
//port_id Port;
BMessage *msg;
BLooper *loop;
};


class AskName: public BWindow
{
public:
	AskName();
	~AskName();
char *	Go(void);	//returns a char *, which must be freed

virtual void MessageReceived(BMessage *);

private:
BTextControl * Text;
port_id Port;
char *text;
};



/**
 * class HSList
 *
 * Purpose: overriden ListItem to show high score info.
 */
class HSList: public BListItem
{
public:
	HSList();
	~HSList();

void	SetName(const char *);
void	SetGameID(unsigned long);
void	SetGameTime(unsigned long);
void	SetNumberTiles(int);
void	SetTimeOfGame(unsigned long time);

#define RETURN(arg) { return arg;}

const char *	GetName(void) const RETURN(name);
unsigned long	GetGameID(void) const RETURN(gameID);
unsigned long	GetGameTime(void) const RETURN(gameTime);
int		GetNumberTiles(void) const RETURN(numTiles);
unsigned long	GetTimeOfGame(void) const RETURN(timeOfGame);
float		GetTilesPerSec(void) const RETURN(tps);
virtual void DrawItem(BView *owner, BRect itemRect, bool drawEverything = false);

private:
	float tps;
	char *name;
	unsigned long gameID;
	unsigned long gameTime;	//time of game, in seconds
	unsigned long numTiles;	// # of tiles
	unsigned long timeOfGame;

};





#endif
