#ifndef __PAUSEWINDOW_H__
#define __PAUSEWINDOW_H__

#include <Window.h>
#include <OS.h>

class PauseWindow: public BWindow
{
public:
	PauseWindow();
	~PauseWindow();

void	Go(void);
virtual void MessageReceived(BMessage *);

private:
port_id Port;
};
#endif
