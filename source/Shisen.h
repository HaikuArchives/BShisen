#ifndef __SHISEN_H__
#define __SHISEN_H__

/*
 * $Id: Shisen.h,v 1.2 1999/07/09 05:22:03 baron Exp baron $
 */

#include <Application.h>
#include "YLanguageClass.h"

class AboutWindow;
class GameWindow;

extern int GLOBAL_QUIT_FLAG;

class Shisen: public BApplication
{
public:
	Shisen();
	~Shisen();

virtual void AboutRequested(void);

private:
	GameWindow *game;
};

#endif
