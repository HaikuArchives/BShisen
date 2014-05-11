#include "Prefs.h"

#include <Path.h>
#include <FindDirectory.h>
#include <Node.h>
#include <File.h>
#include <fs_attr.h>
#include <View.h>
#include <Button.h>
#include <StringView.h>
#include <Application.h>
#include <Box.h>
#include <RadioButton.h>
#include <TabView.h>
#include <TextControl.h>
#include <ColorControl.h>
#include <CheckBox.h>
#include <stdio.h>
#include <Entry.h>
#include <Path.h>
#include <MediaFile.h>

#include <FileGameSound.h>
#include <OS.h>

char *GetFileType(const entry_ref& e)
{
BNode n(&e);
char *cp = NULL;
struct attr_info Info;

	if (n.InitCheck() != B_NO_ERROR)
	{
		printf("failed InitCheck!\n");
		return NULL;
	}
	
	//if (n.Lock())
	//{
		if (n.GetAttrInfo("BEOS:TYPE", &Info) == B_NO_ERROR)
			//&& Info.type == B_MIME_TYPE) ?? why??
		{
			cp = new char[Info.size+1];
			n.ReadAttr("BEOS:TYPE", B_MIME_TYPE, 0, cp, Info.size);
			cp[Info.size] = 0;
		}
		//else
		//{
		//	printf("failed GetAttrInfo\n");
		//}
		//n.Unlock();
	//}
	//else
	//{
	//	printf("couldn't lock\n");
	//}
	return cp;
}


static int32 TrySound2(void *data)
{
BFileGameSound *f = (BFileGameSound *)data;

	while (f->IsPlaying())
		snooze(1000000);	//1 sec
	delete f;
	exit_thread(0);
	return 1;
}


// returns false if the sound isn't a sound
bool TrySound(const entry_ref *e)
{
BFileGameSound *FGS;
BMediaFile *mfile;

	//
	// check if a codec exists to decode the sucker 
	//
	mfile = new BMediaFile(e);
	if (mfile->InitCheck() != B_OK)
	{
		delete mfile;
		return false;
	}
	delete mfile;
	
	FGS = new BFileGameSound(e, false);

	if (!FGS) return false;
	if (FGS->InitCheck() != B_NO_ERROR)
		return false;

	if (FGS->StartPlaying() == B_NO_ERROR)
	{
		thread_id id;
		//spawn a thread to wait for the sound to stop & cleanup
		id = spawn_thread(TrySound2, "Wait for sound", B_NORMAL_PRIORITY, FGS);
		resume_thread(id);
		return true;
	}

	else
	{
		FGS->StopPlaying();
		delete FGS;
		return false;
	}

} 


Prefs::Prefs():
	Changed(false)
{
BNode N;
struct attr_info Info;


	tile = true;//false;
	CustomConnect = false;
	CustomNoConnect = false;
	back_color.red = 216;
	back_color.green = 216;
	back_color.blue = 216;
	back_color.alpha = 0;
	key = 0;
	fresh = false;

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_NO_ERROR
		&& path.Append(PREFS_FILE_NAME, true) == B_NO_ERROR
		&& N.SetTo(path.Path()) == B_NO_ERROR)
	{
		N.Lock();
	
		if (N.GetAttrInfo(PIC_PATH, &Info) == B_NO_ERROR
			&& Info.type == B_STRING_TYPE)
		{
			char *buffer = new char[Info.size];
			N.ReadAttr(PIC_PATH, B_STRING_TYPE, 0, buffer, Info.size);
			
			pic_path.SetTo(buffer, Info.size);
			delete []buffer;

		}
		
		if (N.GetAttrInfo(TILE_PIC, &Info) == B_NO_ERROR
			&& Info.type == B_INT32_TYPE)
		{
			N.ReadAttr(TILE_PIC, B_INT32_TYPE, 0, &tile, 4);
		}
	
		if (N.GetAttrInfo(BACK_COLOR, &Info) == B_NO_ERROR
			&& Info.type == B_RGB_COLOR_TYPE)
		{
			N.ReadAttr(BACK_COLOR, B_RGB_COLOR_TYPE, 0, &back_color,
				sizeof(struct rgb_color));
		}
	
		if (N.GetAttrInfo(KEY_PHRASE, &Info) == B_NO_ERROR
			&& Info.type == B_UINT64_TYPE)
		{
			N.ReadAttr(KEY_PHRASE, B_UINT64_TYPE, 0, &key,
				sizeof(uint64));
		}
		key=1; // hack
	
		// custom sound code - kws 8/14/99
		if (N.GetAttrInfo(CONNECT_SOUND, &Info) == B_NO_ERROR
			&& Info.type == B_STRING_TYPE)
		{
			char *buffer = new char[Info.size];
			N.ReadAttr(CONNECT_SOUND, B_STRING_TYPE, 0, buffer, Info.size);
			
			ConnectSound.SetTo(buffer, Info.size);
			delete []buffer;	
		}
		
		if (N.GetAttrInfo(NOCONNECT_SOUND, &Info) == B_NO_ERROR
			&& Info.type == B_STRING_TYPE)
		{
			char *buffer = new char[Info.size];
			N.ReadAttr(NOCONNECT_SOUND, B_STRING_TYPE, 0, buffer, Info.size);
			
			NoConnectSound.SetTo(buffer, Info.size);
			delete []buffer;	
		}
		if (N.GetAttrInfo(CUSTOM_CONNECT, &Info) == B_NO_ERROR
			&& Info.type == B_UINT32_TYPE)
		{
			N.ReadAttr(CUSTOM_CONNECT, B_INT32_TYPE, 0, &CustomConnect, 4);
		}
		if (N.GetAttrInfo(CUSTOM_NOCONNECT, &Info) == B_NO_ERROR
			&& Info.type == B_UINT32_TYPE)
		{
			N.ReadAttr(CUSTOM_NOCONNECT, B_INT32_TYPE, 0, &CustomNoConnect, 4);
		}
	
		N.Unlock();
	}
	else fresh = true;
}



Prefs::~Prefs()
{
}

void Prefs::WritePrefs(void)
{

	//if (!Changed) return;

	BNode N(path.Path());
	BFile F(path.Path(), B_WRITE_ONLY | B_CREATE_FILE);
	
	if (F.InitCheck() != B_OK)	//guarantee that the file exists
		return;
	
	F.Unset();
	
	N.Lock();
	//N.RemoveAttr(PIC_PATH);
	//N.RemoveAttr(TILE_PIC);
	//N.RemoveAttr(BACK_COLOR);
	
	N.WriteAttr(PIC_PATH, B_STRING_TYPE, 0, pic_path.String(),
		pic_path.Length());	// + 1 makes it include NULL 
	
	N.WriteAttr(TILE_PIC, B_INT32_TYPE, 0, &tile, 4);
	
	N.WriteAttr(BACK_COLOR, B_RGB_COLOR_TYPE, 0, &back_color,
		sizeof(struct rgb_color));
	// don't write if not registered 
	if (key)
	{
		N.WriteAttr(KEY_PHRASE, B_UINT64_TYPE, 0, &key, sizeof(uint64));
	}
	
	N.WriteAttr(CUSTOM_CONNECT, B_UINT32_TYPE, 0, &CustomConnect, sizeof(uint32));
	N.WriteAttr(CUSTOM_NOCONNECT, B_UINT32_TYPE, 0, &CustomNoConnect, sizeof(uint32));
	N.WriteAttr(CONNECT_SOUND, B_STRING_TYPE, 0, ConnectSound.String(),
		ConnectSound.Length());
	N.WriteAttr(NOCONNECT_SOUND, B_STRING_TYPE, 0, NoConnectSound.String(),
		NoConnectSound.Length());
	
	
	N.Unlock();
}

bool Prefs::GetCustomConnect(void) const
{
	return CustomConnect;
}
bool Prefs::GetCustomNoConnect(void) const
{
	return CustomNoConnect;
}
const char *Prefs::GetConnectSound(void) const
{
	return ConnectSound.String();
}
const char *Prefs::GetNoConnectSound(void) const
{
	return NoConnectSound.String();
}

void Prefs::SetCustomConnect(bool b)
{
	if (CustomConnect != b)
	{
		CustomConnect = b;
		Changed = true;
	}
}

void Prefs::SetCustomNoConnect(bool b)
{
	if (CustomNoConnect != b)
	{
		CustomNoConnect = b;
		Changed = true;
	}
}
void Prefs::SetConnectSound(const char *p)
{
	ConnectSound.SetTo(p);
	Changed = true;
}
void Prefs::SetNoConnectSound(const char *p)
{
	NoConnectSound.SetTo(p);
	Changed = true;
}




bool Prefs::GetTilePic() const
{
	return tile;
}
const char *Prefs::GetPicPath(void) const
{
	return pic_path.String();
}
const rgb_color& Prefs::GetBackColor(void) const
{
	return back_color;
}
uint64 Prefs::GetKey(void) const
{
	return key;
}
void Prefs::SetKey(uint64 k)
{
	key = k;
}
void Prefs::SetTilePic(bool b)
{
	tile = b;
	Changed = true;
}
void Prefs::SetPicPath(const char *p)
{
	pic_path.SetTo(p);
	Changed = true;
}
void Prefs::SetBackColor(const rgb_color& c)
{
	back_color = c;
	Changed = true;
}

void Prefs::Go(BLooper *looper)
{
//bool ret_val;
PrefEdit *P;


	P = new PrefEdit(this, looper);
	P->Show();
	
	//ret_val = P->Go();
	//be_app->SetCursor(B_HAND_CURSOR);
	//P->Lock();
	//P->Quit();
	
	//return ret_val;
}


PrefEdit::PrefEdit(Prefs *P, BLooper *l):
	BWindow(BRect(100, 100, 470+20, 380+20), "BShisen Preferences",
		B_TITLED_WINDOW,	
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE),
		prefs(P)
{
BView *V;
BButton *B;
BBox *box;
BRect r;
BTab *tab;
const char *cp;

	loop = l;
	//RemoveShortcut('q', B_COMMAND_KEY);
	V = new BView(Bounds(), "back", B_FOLLOW_ALL, B_WILL_DRAW);
	V->SetViewColor(216, 216, 216);
	AddChild(V);

	B = new BButton(BRect(10, 160+50+50, 80, 190+50+50), "", "Accept", new BMessage('acce'));
	B->MakeDefault(true);
	V->AddChild(B);
	B = new BButton(BRect(90, 160+50+50, 160, 190+50+50), "", "Cancel", new BMessage('canc'));
	V->AddChild(B);


	r = Bounds();
	r.InsetBy(5, 5);
	
	r.bottom -= 50;
	
	TV = new BTabView(r, "");
	TV->SetViewColor(216, 216, 216);
	r.bottom -= TV->TabHeight(); 	
	V->AddChild(TV);

// general stuff
	V = new BView(r, "", B_FOLLOW_ALL, B_WILL_DRAW);
	V->SetViewColor(216, 216, 216);
	tab = new BTab(V);
	tab->SetLabel("Background");
	TV->AddTab(V, tab);
	
	btxt = new BTextControl(BRect(10, 10, 360, 25), "", "Background Picture",
		NULL, NULL);
	btxt->SetText(prefs->GetPicPath());
	btxt->SetDivider(btxt->StringWidth("Background Picture "));	
	
	
	
	V->AddChild(btxt);
	
	cbox = new BCheckBox(BRect(10, 35, 200, 50), "", "Tile Picture", NULL);
	
	cbox->SetValue(prefs->GetTilePic());	
	V->AddChild(cbox);
	
	box = new BBox(BRect(10, 60, 360, 200));
	box->SetLabel("Background Color");
	V->AddChild(box);
	
	color_sample = new BView(BRect(10, 15, 40, 45), "",	B_FOLLOW_ALL, B_WILL_DRAW);
	color_sample->SetViewColor(prefs->GetBackColor());
	box->AddChild(color_sample);
	//color_sample->Invalidate();
	//color_sample->Draw(BRect(10,15,40,45));
	
	cctl = new BColorControl(BPoint(10, 50/*10+10, 120*/), B_CELLS_32x8, 8, "",
			new BMessage('cccc'));
	cctl->SetValue(prefs->GetBackColor());
	box->AddChild(cctl);
	

//
// user select sounds
//
//
BRadioButton *rad;

	V = new BView(r, "", B_FOLLOW_ALL, B_WILL_DRAW);
	V->SetViewColor(216, 216, 216);
	tab = new BTab(V);
	tab->SetLabel("Sound");
	TV->AddTab(V, tab);

	box = new BBox(BRect(10, 10, 360, 100));
	box->SetLabel("Connect Sound");
	V->AddChild(box);
	
	rad = new BRadioButton(BRect(10, 20, 340, 35), "", "Default",
		new BMessage(CONNECT_DEFAULT));
	rad->SetValue(1);
	box->AddChild(rad);
	
	rConnectSound = new BRadioButton(BRect(10, 40, 340, 55), "", "Drag Sound Here",
		new BMessage(CONNECT_CUSTOM));

	cp = P->GetConnectSound();
	
	if (cp && *cp)
	{
		rConnectSound->SetLabel(cp);
	}
	else
	{
		rConnectSound->SetEnabled(false);
		P->SetCustomConnect(false);
	}
	box->AddChild(rConnectSound);

	if (P->GetCustomConnect())
	{
		rConnectSound->SetValue(true);
	}
	else
	{
		rad->SetValue(true);
	}





	//B = new BButton(BRect(10, 60, 100, 75), "", "Select Sound", 
	//	new BMessage(CONNECT_SELECT_SOUND));
	//box->AddChild(B);
	
	
	box = new BBox(BRect(10, 110, 360, 200));
	box->SetLabel("Unable To Connect Sound");
	V->AddChild(box);

	rad = new BRadioButton(BRect(10, 20, 340, 35), "", "Default",
		new BMessage(NOCONNECT_DEFAULT));
	box->AddChild(rad);
	
	rNoConnectSound = new BRadioButton(BRect(10, 40, 340, 55), "", "Drag Sound Here",
		new BMessage(NOCONNECT_CUSTOM));
	
	cp = P->GetNoConnectSound();
	
	if (cp && *cp)
	{
		rNoConnectSound->SetLabel(cp);
	}
	else
	{
		rNoConnectSound->SetEnabled(false);
		P->SetCustomNoConnect(false);
	}
	box->AddChild(rNoConnectSound);

	if (P->GetCustomNoConnect())
	{
		rNoConnectSound->SetValue(true);
	}
	else
	{
		rad->SetValue(true);
	}


	//B = new BButton(BRect(10, 60, 100, 75), "", "Select Sound", 
	//	new BMessage(NOCONNECT_SELECT_SOUND));
	//box->AddChild(B);


	// select the first tab & force a draw
	TV->Select(0);

}

PrefEdit::~PrefEdit()
{
}
void PrefEdit::MessageReceived(BMessage *msg)
{
BMessage *bm;
const char *cp;

	switch(msg->what)
	{
	case 'cccc':
		color_sample->SetViewColor(cctl->ValueAsColor());
		color_sample->Invalidate();
		break;
//			uint32 new_val;
//			
//			if (msg->FindInt32("be:value", (int32 *)&new_val) == B_OK)
//			{
//				color_sample->SetHighColor(new_val);
//				color_sample->Invalidate();
//			}
//			//msg->PrintToStream();
//		}
//		break;
	case 'acce':
		prefs->SetTilePic(cbox->Value());
		prefs->SetPicPath(btxt->Text());
		prefs->SetBackColor(cctl->ValueAsColor());
		
		prefs->SetCustomConnect(rConnectSound->Value());
		prefs->SetCustomNoConnect(rNoConnectSound->Value());
		
		cp = rConnectSound->Label();
		if (cp && BString(cp) != "Drag Sound Here")
		{
			prefs->SetConnectSound(cp); 		
		}
		else prefs->SetConnectSound(NULL);
		
		cp = rNoConnectSound->Label();
		if (cp && BString(cp) != "Drag Sound Here")
		{
			prefs->SetNoConnectSound(cp); 		
		}
		else prefs->SetNoConnectSound(NULL);
		
		//write_port(Port, 1, NULL, 0);
		be_app->SetCursor(B_HAND_CURSOR);
		bm = new BMessage('nopa');
		bm->AddInt32("Prefs", true);
		loop->PostMessage(bm);
		Quit();
		break;
		
	case 'canc':
		//write_port(Port, 0, NULL, 0);
		be_app->SetCursor(B_HAND_CURSOR);
		loop->PostMessage(new BMessage('nopa'));
		Quit();
		break;
		
	
	//
	// user draged a file onto the window	
	case B_SIMPLE_DATA:
		if (TV->Selection() == 1)	// make sure is sound window
		{
			BPoint pnt;
			entry_ref ref;
			BPath *path;
			BEntry *entry;
			//char *type;
			
			if (msg->FindPoint("_drop_point_", &pnt) == B_NO_ERROR)
			{	// get real values for these!
			
				if (msg->FindRef("refs", 0, &ref) != B_NO_ERROR)
					break;
			
				// we check for codecs, so it isn't an issue :)
				// make sure it's in the audio/xxx family :)
				//type = GetFileType(ref);
				//if (type && (BString(type).Compare("audio", 5) ==  0))
				{
					//delete []type;
			
					BWindow::ConvertFromScreen(&pnt);
			
					// make sure it's in the x range
					if (pnt.x < 20 || pnt.x > 370) break;
				
					if (pnt.y > 50 && pnt.y < 130)
					{
						if (TrySound(&ref))
						{
							entry = new BEntry(&ref);
							path = new BPath(entry);
							rConnectSound->SetLabel(path->Path());
							rConnectSound->SetEnabled(true);
					
							delete path;
							delete entry;
						}
					}
				
					if (pnt.y > 150 && pnt.y < 230)
					{
						if (TrySound(&ref))
						{
							entry = new BEntry(&ref);
							path = new BPath(entry);
							rNoConnectSound->SetLabel(path->Path());
							rNoConnectSound->SetEnabled(true);
					
							delete path;
							delete entry;
						}
					}				
				}
			}
		}
		break;
	
	
	default: BWindow::MessageReceived(msg);
			//msg->PrintToStream();
	}

}

bool PrefEdit::QuitRequested(void)
{
	be_app->SetCursor(B_HAND_CURSOR);
	loop->PostMessage(new BMessage('nopa'));
	return true;
}

#if 0
bool PrefEdit::Go(void)
{
int32 retval;

	Port = create_port(1, "Preferences port");
	Show();
	read_port(Port, &retval, NULL, 0);
	Hide();
	delete_port(Port);
	return retval;
}
#endif