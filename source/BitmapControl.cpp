#include "BitmapControl.h"
#include <stdio.h>
#include <stdlib.h>


BitmapControl::BitmapControl(BRect frame, BBitmap *On,
		const char *name, const char *label,
		BMessage *message, uint32 resizingMode, uint32 flags):

	BControl(frame, name, label, message, resizingMode, flags),
	value(0),
	OnPic(On),
	rect(frame),
	mess(message)
{
}



BitmapControl::~BitmapControl()
{
}


void BitmapControl::SetValue(int32 newvalue)
{
	if ((bool)value != (bool)newvalue)
	{
		value = newvalue;

		SetDrawingMode(value ? B_OP_BLEND : B_OP_COPY);
		if (value) SetViewColor(0, 255, 0);
		else SetViewColor(0,0,0);
		Invalidate(); 	//force a redraw

		BControl::SetValue(newvalue);
	}
}

void BitmapControl::Draw(BRect updateRect)
{
	if (OnPic) DrawBitmap(OnPic);
#if 0
	printf("top = %f bottom = %f left = %f right = %f\n",
		updateRect.top,
		updateRect.bottom,
		updateRect.left,
		updateRect.right);
#endif

}

status_t BitmapControl::SetMessage(BMessage *message)
{
	mess = message;
	return BControl::SetMessage(message);
}

void BitmapControl::MouseDown(BPoint point)
{
	SetValue(!value);
	SetTracking(true);
	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
}

void BitmapControl::MouseMoved(BPoint point, uint32 transit, 
	const BMessage *msg)
{
	if (IsTracking() && 
		(transit == B_ENTERED_VIEW || transit == B_EXITED_VIEW))
	{
		SetValue(!value);
	}
}

void BitmapControl::MouseUp(BPoint point)
{
	if (Bounds().Contains(point)) Invoke();
	SetTracking(false);
}