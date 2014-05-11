#ifndef __ID_WIN_H__
#define __ID_WIN_H__

#include <Window.h>
#include <Button.h>
#include <TextControl.h>
#include <OS.h>

enum { ACCEPT_BUTTON = 0x5000, CANCEL_BUTTON };

class ID_Win: public BWindow
{
public:
	ID_Win(BLooper *, unsigned int);
//unsigned int Go(unsigned int);
	~ID_Win();
virtual void MessageReceived(BMessage *);
virtual bool QuitRequested(void);

private:

BTextControl *Txt_Ctl;
int done;
unsigned int Game;
BLooper *loop;
//port_id Port;

};

#endif
