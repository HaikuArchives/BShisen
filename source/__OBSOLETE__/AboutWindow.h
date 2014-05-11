#include <Window.h>
#ifndef __ABOUTWINDOW_H__
#define __ABOUTWINDOW_H__

class AboutWindow: public BWindow
{
public:
	AboutWindow(BRect, const char *, window_type, uint32, uint32 = B_CURRENT_WORKSPACE);
virtual bool QuitRequested(void);

private:
};

#endif