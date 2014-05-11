#ifndef __LINECONTROL_H__
#define __LINECONTROL_H__

#include <Control.h>


class LineControl: public BControl
{
public:
	LineControl(BRect rect);
	~LineControl();

virtual void    Draw(BRect updateRect);
virtual void	Hide(void);

private:
};
#endif
