#ifndef __CWINDOW_H__
#define __CWINDOW_H__

#include <Window.h>
#include <OS.h>
#include "Dictionary.h"

class CWindow: public BWindow
{
public:
	CWindow();
	CWindow();

void	Go(void);

virtual void MessageReceived(BMessage *);


private:

Dictionary<uint32, uint32> dict;
port_id Port;

};

#endif
