#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "board.h"
#include "rRect.h"
#include <assert.h>

/*
 * $Id: board.cpp,v 1.2 1999/03/06 21:52:33 baron Exp $
 */
 
//const int NUMBER_TILES = 36;

board::board():
	h(0),
	w(0),
	b(NULL),
	count(0)
{
	enable_undo(false);
}


board::board(int height, int width, int key):
	k(key),
	h(height + 2), 
	w(width + 2), 
	b(NULL)
{
int i, j;

	count = height * width;		// number of tiles
	_sel_kind = 0;
	_sel_h = 0;
	_sel_w = 0;

	b = (int **) malloc(h * sizeof(int *));
	for (i=0; i < h; i++)
		b[i] = (int *)malloc(w * sizeof(int));

	srand(k);
	init_board(36);
	sort_board();

	enable_undo(false);

	// call overloaded function to draw all the tiles 
	for ( i = 1; i < h-1; i++)
		for (j = 1 ; j < width-1; j++)
			draw_tile (j, i, b[i][j]);
}


board::~board()
{
int i;

	if (b)
	{
		for (i=0; i < h; i++)
			free(b[i]);
		free(b);
	}
}

void board::new_game(int height, int width, int key, int num_tiles)
{
int i, j;

#if 0
// +++ safety check

	fprintf(stderr, "board::new_game: height: %d width %d : key %d num_tiles %d\n",
		height, width, key, num_tiles);
		
	if (height == 6)
		assert(num_tiles == 21);
	else if (height == 8)
		assert (num_tiles == 36);
	else if (height == 12)
		assert (num_tiles == 36);
	else if (height == 16)
		assert (num_tiles == 28);
	else if (height == 20)
		assert(num_tiles == 32);
#endif

	if (b)
	{
		for (i=0; i < h; i++)
			free(b[i]);
		free(b);
	}

	w = width + 2;
	h = height + 2;
	k = key;

	count = height * width;		// number of tiles

	TileCount(count);		// callback to set the # of tiles 
	_sel_kind = 0;
	_sel_h = 0;
	_sel_w = 0;

	b = (int **) malloc( h * sizeof(int *));
	for (i=0; i < h; i++)
		b[i] = (int *)malloc(w * sizeof(int));

	srand(k);
	init_board(num_tiles);
	sort_board();

	enable_undo(false);
	while (!undo_buffer.empty()) delete undo_buffer.pop();


	// call overloaded function to draw all the tiles 
	for ( i = 1; i < h-1; i++)
		for (j = 1 ; j < w-1; j++)
			draw_tile (i, j, b[i][j]);
}

void board::init_board(int number_tiles)
{
int i,j,k;

	// init everything to 0
	for(i = 0; i < h; i++)
		for(j = 0; j < w; j++)
			b[i][j] = 0;

	k = 1;
	for(i = 1; i < h-1; i++)
		for(j = 1; j < w-1; j++)
		{

			if (++k > number_tiles) k = 1;
			b[i][j] = k;
		}
}

static void swap(int& a, int & b)
{
int tmp;
	tmp = a;
	a = b;
	b = tmp;
}

void board::sort_board(void)
{
int i,j,k,j2,k2; 

	for (i = 0; i < 0x1000; i++)
	{
		j = (rand() % (h-2)) + 1;
		k = (rand() % (w-2)) + 1;
		j2 = (rand() % (h-2)) + 1;
		k2 = (rand() % (w-2)) + 1;
		swap(b[j][k], b[j2][k2]);
	}	
}


int board::get(int height, int width) const
{
	return b[height][width];
}
int board::set(int height, int width, int val)
{
	return b[height][width] = val;
}


int board::remaining(void) const
{
	return count;
}


/*
 *  if 0, 0, everything is deselected
 *
 *
 */
void board::select(int height, int width)
{

	/*
	 * if 0,0 deselect everything
	 */
	if (!height || !width)
	{
		if (_sel_kind)
			unhilite(_sel_h, _sel_w);

		_sel_kind = _sel_w = _sel_h = 0;
		return;
	}
	
	//printf("select with h = %d, w = %d\n", height, width);

	/*
	 * Check if a tile is already selected
	 *
	 */
	if (_sel_kind)
	{

		/*
		 * the same tile was pressed again 
		 *
		 * unhilite it & reset data
		 */
		if (height == _sel_h && width == _sel_w)
		{
			unhilite(height, width);
			_sel_h = _sel_w = _sel_kind = 0;
		}

		/*
		 * check if the tiles match
		 *
		 */
		else if (_sel_kind != get(height, width))
		{
			match_failed();
			unhilite(height, width);
			unhilite(_sel_h, _sel_w);
			_sel_h = _sel_w = _sel_kind = 0;
		}

		/*
		 * check if they are connectable
		 *
		 */	
		else if (CanConnect(_sel_w, _sel_h, width, height, _sel_kind, true))
		{
			//printf("connected!\n");
			//printf("_sel_w = %d\n", _sel_w);
			//printf("_sel_h = %d\n", _sel_h);
			//printf("width = %d\n", width);
			//printf("height = %d\n", height);
			
			unhilite(height, width);
			unhilite(_sel_h, _sel_w);
			erase_tile(height, width);
			erase_tile(_sel_h, _sel_w);
			set(height, width, 0);
			set(_sel_h, _sel_w, 0);
			PairRemoved();
			count -= 2;
			
			TileCount(count);
	
			undo_buffer.push(new rRect(_sel_h, _sel_w, height, width, 
				_sel_kind));

			enable_undo(true);

			_sel_h = _sel_w = _sel_kind = 0;

			/*
			 * If no more tiles, we are done
			 */
			if (count == 0)
				GameDone();
		}

		else
		{
			match_failed();
			unhilite(height, width);
			unhilite(_sel_h, _sel_w);
			_sel_h = _sel_w = _sel_kind = 0;

		}
	}

	/*
	 * nothing was previously selected, so cache the new data
	 *
	 */
	else
	{
		_sel_h = height;
		_sel_w = width;
		_sel_kind = get(height, width);
		
		hilite(height, width);
	}

}

/*
 * Undoes the last removal, if possible.
 *
 * returns TRUE if more moves can be undone
 */
void board::undo(void)
{
rRect *r;

	/*
	 * make sure there's something to undo
	 */
	if (!undo_buffer.empty())
	{

		/*
		 * unhilite anything that might be hilited
		 */
		#if 0
		if (_sel_kind)
		{
			unhilite(_sel_h, _sel_w);
			sel_h = sel_w = sel_kind = 0;
		}
		#endif
	
		r = undo_buffer.pop();

		set(r->x1(), r->y1(), r->type());
		set(r->x2(), r->y2(), r->type());
		draw_tile(r->x2(), r->y2(), r->type());
		draw_tile(r->x1(), r->y1(), r->type());
	
		count += 2;
		TileCount(count);
	
		delete r;
	}
	if (undo_buffer.empty())
		enable_undo(false);
}


void board::FindMatches(void)
{

struct foo {int h; int w;};

foo *found;
int i, j, num_found;

	found = NULL;
	num_found = 0;

	if (_sel_h  == 0 && _sel_w == 0) return;

	for (i = 1; i < h-1 ; i++)
		for (j = 1; j < w-1; j++)
			if (b[i][j] == _sel_kind)
			{
				if (num_found++)
					found = (foo *)realloc(found, 
						num_found *sizeof (foo));
				else found = (foo *)malloc(sizeof (foo));
				
				found[num_found-1].h = i;
				found[num_found-1].w = i;
			}


	if (num_found)
	{
		if (_sel_h && _sel_w) unhilite(_sel_h, _sel_w);
		for ( i = 0 ; i < 5; i++)
			for (j = 0; j < num_found; j++)
				hilite(found[j].h, found[j].w);
			
			delay();
			for (j = 0; j < num_found; j++)
				unhilite(found[j].h, found[j].w);
		if (_sel_h && _sel_w) hilite(_sel_h, _sel_w);
	}
}

/*
 * This is the heart of Shisen - a recursive function
 * to find matches.  The rest is just details... 
 *
 *
 */
int board::connect(DIR_stack *D,
		int orig_x,
		int orig_y,
		int start_x,
		int start_y,
		int dir,			// direction we're headed
		int dir_changes,
		int sel_kind)
{


#define MAX_CHANGES 3
#define X_MIN 0
#define Y_MIN 0
#define X_MAX (w - 1)
#define Y_MAX (h - 1)


int best_dir;
int least_moves;

	/*
	 * Check to see if too many directional 
	 * changes have occurred
	 */
	if (dir_changes > MAX_CHANGES)
		return 0;



	/* done yet? */
	if (start_x == orig_x && start_y == orig_y)
	{
		return 1;
	}

	
	/*
	 * Check if we're running off the board
	 *
	 */
	if (start_x < X_MIN) return 0;
	if (start_x > X_MAX) return 0;
	if (start_y < Y_MIN) return 0;
	if (start_y > Y_MAX) return 0;


	/* is the space clear? */
	if (b[start_y][start_x])	
		return 0;


	/* specially optimized cases */

	if (dir_changes == MAX_CHANGES && 
		(start_x == X_MIN 
		|| start_x == X_MAX
		|| start_y == Y_MIN
		|| start_y == Y_MAX))
	{
		return 0;
	}


#define PUSH(x) if (D) D->push(x)

	/*
	 * The deal with the initial dir != _ check
	 * is to prevent immediate backtracking
	 *
	 */

	if (start_x < X_MAX && dir != W &&
		connect (D,
			orig_x,
			orig_y,
			start_x + 1,
			start_y,
			E,
			dir == E ? dir_changes : dir_changes + 1,
			sel_kind))
		{
			PUSH(E);
			return 1;
		}

	if (start_x > X_MIN && dir != E &&
		connect (D,
			orig_x,
			orig_y,
			start_x - 1,
			start_y,
			W,
			dir == W ? dir_changes : dir_changes + 1,
			sel_kind))
		{
			PUSH(W);
			return 1;
		}
	if (start_y < Y_MAX && dir != N &&
		connect (D,
			orig_x,
			orig_y,
			start_x,
			start_y + 1,
			S,
			dir == S ? dir_changes : dir_changes + 1,
			sel_kind))
		{
			PUSH(S);
			return 1;
		}

	if (start_y > X_MIN && dir != S &&
		connect (D,
			orig_x,
			orig_y,
			start_x,
			start_y - 1,
			N,
			dir == N ? dir_changes : dir_changes + 1,
			sel_kind))
		{
			PUSH(N);
			return 1;
		}
		
	/* default - failure */

	return  0;
#undef PUSH
}



/*
 * Convert the direction stack into 3 or less rRects
 *
 */
void board::CalcPos(int x, int y, DIR_stack *D, RECT_stack * R)
{
rRect *r = NULL;


enum SHISEN_DIR prev_dir = (enum SHISEN_DIR)-1;
enum SHISEN_DIR curr_dir;

	while (!D->empty())
	{
		curr_dir = D->pop();

		if (prev_dir != curr_dir)
		{
			if (r) R->push(r);	
			r = new rRect(x, y, x, y, curr_dir);
			prev_dir = curr_dir;
		}

		switch(curr_dir)
		{
		case N: r->set_y1(r->y1() - 1);	y--;	break;
		case S: r->set_y2(r->y2() + 1);	y++;	break;
		case E: r->set_x2(r->x2() + 1);	x++;	break;
		case W: r->set_x1(r->x1() - 1);	x--;	break;
		}
	}
	if (r) R->push(r);
}

/*
 *
 * Return true if the tiles can be connected.
 *
 */
bool board::CanConnect(int x_orig, int y_orig,
	int x, int y, int sel_kind, bool draw) 
{
int suck = 0;
DIR_stack * D = new DIR_stack; 


	if (connect(D, x_orig, y_orig, x, y-1, N, 1, sel_kind))
	{
		D->push(N);
		suck++; 
	}
	else if (connect(D, x_orig, y_orig, x+1, y, E, 1, sel_kind))
	{
		D->push(E);	
		suck++; 
	}
	else if (connect(D, x_orig, y_orig, x, y+1, S, 1, sel_kind))
	{
		D->push(S);	
		suck++; 
	}
	else if (connect(D, x_orig, y_orig, x-1, y, W, 1, sel_kind))
	{
		D->push(W);
		suck++; 
	}



	if (suck)
	{
	RECT_stack *R = new RECT_stack;
	RECT_stack *tmp = new RECT_stack;
	rRect * r;
	
		/*
		 * Calculate positions for drawing lines
		 */
		CalcPos(x, y, D, R);
		while (!R->empty())
		{
			r = R->pop();
			tmp->push(r);
			if (draw) DrawLine(r);
		}
		delay();
		while (!tmp->empty())
		{
			r = tmp->pop();
			if (draw) EraseLine(r);
			delete r;
		}
		delete R;
		delete tmp;
	}
	delete D;
	return suck;
}

/*
 * Returns TRUE if a move can be found.
 *
 *
 */
bool board::SuggestMove(bool show_them)
{
int x, y;

register int x2, y2;
int kind;

int matched = false;


//printf("SuggestMove entered...\n");

	if (_sel_kind) unhilite(_sel_h, _sel_w);

	for (y = 1; (y < h - 1) && !matched ; y++)
		for (x = 1; (x < w - 1) && !matched; x++)
		{
			kind = b[y][x];
			if (kind)
			{
				for (y2 = y; (y2 < h - 1) && !matched; y2++)
					for (x2 = 1; (x2 < w - 1) && !matched; x2++)
					{
						if (y2 == y && x2 == x)
							continue;

						if (b[y2][x2] != kind) 
							continue;

						if (connect(NULL, x, y, x2, y2-1, N, 1, kind)
						  || connect(NULL, x, y, x2+1, y2, E, 1, kind)
						  || connect(NULL, x, y, x2, y2+1, S, 1, kind)
						  || connect(NULL, x, y, x2-1, y2, W, 1, kind))
						{
							matched++;
							if (show_them)
							  {
								hilite(y2, x2);
								hilite(y, x);
								delay();
								delay();
								delay();
								delay();
								unhilite(y2, x2);
								unhilite(y, x);
							   }

						}
					}
			}
		}			


	if (_sel_kind) hilite(_sel_h, _sel_w);
	

//printf("SuggestMove exiting...\n");

	return matched;
}
