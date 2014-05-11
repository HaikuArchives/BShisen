#include "PauseWindow.h"

#include <Message.h>
#include <View.h>
#include <Button.h>
#include <StringView.h>

PauseWindow::PauseWindow():
	BWindow(BRect(100, 100, 300, 175),
		"Paused Game",
		B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE)

{
BView * V;
BButton *B;
BStringView *S;

	V = new BView(Bounds(), NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	V->SetViewColor(216, 216, 216);
	AddChild(V);

	B = new BButton(BRect(10, 40, 90, 55), "Continue", 
		"Continue", new BMessage('cntu'));

	B->MakeDefault(true);

	V->AddChild(B);

	S = new BStringView(BRect(10, 10, 150, 25), "", "Game Is Paused.");
	V->AddChild(S);
}

PauseWindow::~PauseWindow()
{
}

void PauseWindow::MessageReceived(BMessage *msg)
{
	if (msg->what == 'cntu')
		write_port(Port, 65816, NULL, 0);
	else BWindow::MessageReceived(msg);
}

void PauseWindow::Go(void)
{
int32 foo;

	Port = create_port(1, "Pause Window Port");

	Show();
	read_port(Port, &foo, NULL, 0);
	Hide();
	Lock();
	Quit();
	//delete this;
}
	

