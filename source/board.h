#ifndef __BOARD_H__
#define __BOARD_H__

/*
 * Copyright 1999 Kelin W Sherlock
 *
 * $Id: board.h,v 1.1 1999/03/06 19:58:35 baron Exp baron $
 */



#include "stack.cpp"
#include "rRect.h"

enum SHISEN_DIR { N, S, E, W };

typedef stack<SHISEN_DIR> DIR_stack;
typedef stack<rRect *>RECT_stack;

class board
{
public:

	board();
	board(int height, int width, int key);
virtual	~board();

virtual void	new_game(int height, int width, int key, int num_tiles = 36);
int	get(int h, int w) const;
int	set(int h, int w, int tile);
int	remaining(void) const;

/*
 * This function should be called when a tile is selected.
 *
 */
void	select(int width, int height);

bool	CanConnect(int x1,
				int y1,
				int x2,
				int y2,
				int sel_kind,
				bool draw = 0);


/*
 * searches for valid matches.
 *
 * returns FALSE on failure, TRUE if a move can be found
 *
 */
bool	SuggestMove(bool show_them);



/*
 * call this function to undo the last removed tile
 *
 */
void	undo(void);

/*
 * find matches to the selected tile
 *
 */
void	FindMatches(void);
/*
 * find matches to a specific tile
 *
 */
void	FindMatches(int height, int width);

/*
 * Function to hilite a tile
 *
 */
virtual void hilite(int height, int width) {};

/*
 * Function to unhilite a tile
 *
 */
virtual void unhilite(int height, int width) {};

/*
 * function to erase a tile
 *
 */
virtual void erase_tile(int height, int width) {};

/*
 * Function to draw a specific tile
 */
virtual void draw_tile(int height, int width, int type) {};

/*
 * Function to catch changes in undo-able status
 */
virtual void enable_undo(bool enable) {};

/*
 * Function to pause for a short amount of time
 */
virtual void delay(void) {};

virtual void DrawLine(rRect *r) {};
virtual void EraseLine(rRect *r) {};

/*
 * called when the # of tiles left changes
 *
 */
virtual void TileCount(unsigned int new_count) {};

/*
 * Called when the # of tiles left == 0
 * (ie the game is, well, done
 *
 *
 */
virtual void GameDone(void) {};

/*
 *
 * Called when a pair of tiles is removed, so you can make a noise,
 * or something.
 *
 */
virtual void PairRemoved(void) {};

/*
 * Called when a match fails (tiles don't match, too many lines)
 *
 */
virtual void match_failed(void) {};

private:

//stack<rRect *> pos_buffer;
stack<rRect *> undo_buffer;

//stack<enum DIR>	dir_stack;

int	k;	// game key
int	h;	//height of board
int	w;	// width of board
int**	b;

int count;

int _sel_h;
int _sel_w;
int _sel_kind;


void	init_board(int number_tiles);
void	sort_board(void);

//void	push_pos(enum DIR dir);
void	CalcPos(int x, int y, DIR_stack *, RECT_stack *);

int	connect(DIR_stack *,
			int orig_x,
			int orig_y,
			int x,
			int y,
			int dir,
			int dir_changes,
			int sel_kind);
};

#endif
