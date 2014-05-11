#ifndef __BITMAPCONTROL_H__
#define __BITMAPCONTROL_H__

#include <Control.h>
#include <Bitmap.h>
#include <OS.h>

/*
 * Control class that draws a BBitmap.
 * 
 * The idea is something _faster_ than BPictureControl
 *
 */
class BitmapControl: public BControl
{
public:
		BitmapControl(BRect frame, BBitmap *On,
			const char *name, const char *label,
			BMessage *message, 
			uint32 resizingMode = B_FOLLOW_NONE,
			uint32 flags = B_WILL_DRAW);

		~BitmapControl();


virtual void 	SetValue(int32 value);
virtual void 	Draw(BRect updateRect);
virtual status_t SetMessage(BMessage *);
virtual void	MouseDown(BPoint point);
virtual void	MouseMoved(BPoint, uint32, const BMessage *);
virtual void	MouseUp(BPoint);


private:

int32		value;	//cached value;
BBitmap *	OnPic;
BRect 		rect;
BMessage *	mess;

};

#endif
