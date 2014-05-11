#ifndef __TILES_H__
#define __TILES_H__

#include <Bitmap.h>
class BMenu;

class Tiles
{
public:
	Tiles(void);
	~Tiles(void);

BBitmap *GetTile(int index) 
{
	if (use_custom) return tiles_custom[index];
	else return tiles_std[index]; 
};

int MakeTileMenu(BMenu *);

private:

BBitmap **tiles_std;
BBitmap **tiles_custom;

bool use_custom;


};

#endif
