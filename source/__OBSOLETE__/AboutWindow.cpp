#include "AboutWindow.h"


AboutWindow::AboutWindow(BRect frame, const char *title,
window_type type, uint32 flags, uint32 workspaces):
BWindow(frame, title, type, flags, workspaces)
{


}

extern int GLOBAL_QUIT_FLAG;

// only hide it if it's visible
bool AboutWindow::QuitRequested(void)
{
	if (GLOBAL_QUIT_FLAG) return true;
	
	if (!IsHidden())
		Hide();
	return false;
}